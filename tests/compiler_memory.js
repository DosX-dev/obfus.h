'use strict';
// Profile Windows TCC without Python or npm packages. The measured compiler
// also builds a small WinAPI monitor; that build is outside the measurement.
// node tests/compiler_memory.js --stdout expanded.i --limit-mib 512 -- tcc.exe -w -E project.c
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const { spawn } = require('node:child_process');

// Node has no native Job Object / process-memory API. Keep the native bridge
// private here, compiled into an owned temporary directory on demand.
const monitorSource = String.raw`
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

__declspec(dllimport) LPWSTR *WINAPI CommandLineToArgvW(LPCWSTR, int *);
typedef struct {
    DWORD cb, PageFaultCount;
    SIZE_T PeakWorkingSetSize, WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage, QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage, QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage, PeakPagefileUsage, PrivateUsage;
} MemoryCounters;
__declspec(dllimport) BOOL WINAPI GetProcessMemoryInfo(HANDLE, void *, DWORD);

int main(void) {
    int argc = 0, result = 1, done = 0;
    LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    HANDLE job = NULL, output = INVALID_HANDLE_VALUE;
    HANDLE errors = INVALID_HANDLE_VALUE, input = INVALID_HANDLE_VALUE;
    PROCESS_INFORMATION process = {0};
    STARTUPINFOW startup = {0};
    SECURITY_ATTRIBUTES security = {sizeof(security), NULL, TRUE};
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits = {0};
    LARGE_INTEGER frequency, started, ended;
    SIZE_T peak_commit = 0, peak_working = 0;
    unsigned long long limit = 0;
    DWORD exit_code = 0, wait_result;
    if (!argv) goto failure;
    if (argc != 5) { SetLastError(ERROR_INVALID_PARAMETER); goto failure; }
    for (LPCWSTR at = argv[4]; *at; ++at) {
        if (*at < L'0' || *at > L'9' || limit > (0xffffffffffffffffull - (*at - L'0')) / 10) {
            SetLastError(ERROR_INVALID_PARAMETER);
            goto failure;
        }
        limit = limit * 10 + (*at - L'0');
    }
    if (limit > (SIZE_T)-1) { SetLastError(ERROR_ARITHMETIC_OVERFLOW); goto failure; }
    job = CreateJobObjectW(NULL, NULL);
    if (!job) goto failure;
    limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    if (limit) {
        limits.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_PROCESS_MEMORY;
        limits.ProcessMemoryLimit = (SIZE_T)limit;
    }
    if (!SetInformationJobObject(job, JobObjectExtendedLimitInformation, &limits, sizeof(limits))) goto failure;
    output = CreateFileW(argv[2], GENERIC_WRITE, FILE_SHARE_READ, &security, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    errors = CreateFileW(argv[3], GENERIC_WRITE, FILE_SHARE_READ, &security, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    {
        HANDLE inherited = GetStdHandle(STD_INPUT_HANDLE);
        if (inherited && inherited != INVALID_HANDLE_VALUE) {
            if (!DuplicateHandle(GetCurrentProcess(), inherited, GetCurrentProcess(), &input, 0, TRUE, DUPLICATE_SAME_ACCESS))
                goto failure;
        } else {
            input = CreateFileW(L"NUL", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, &security, OPEN_EXISTING, 0, NULL);
        }
    }
    if (output == INVALID_HANDLE_VALUE || errors == INVALID_HANDLE_VALUE || input == INVALID_HANDLE_VALUE) goto failure;
    startup.cb = sizeof(startup);
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = input;
    startup.hStdOutput = output;
    startup.hStdError = errors;
    // Suspend before assigning the job so even short compiles obey the limit.
    if (!CreateProcessW(NULL, argv[1], NULL, NULL, TRUE, CREATE_NO_WINDOW | CREATE_SUSPENDED,
                        NULL, NULL, &startup, &process)) goto failure;
    if (!AssignProcessToJobObject(job, process.hProcess)) goto failure;
    if (!QueryPerformanceFrequency(&frequency) || !QueryPerformanceCounter(&started)) goto failure;
    if (ResumeThread(process.hThread) == (DWORD)-1) goto failure;
    for (;;) {
        MemoryCounters memory = {0};
        memory.cb = sizeof(memory);
        if (GetProcessMemoryInfo(process.hProcess, &memory, sizeof(memory))) {
            if (memory.PeakPagefileUsage > peak_commit) peak_commit = memory.PeakPagefileUsage;
            if (memory.PrivateUsage > peak_commit) peak_commit = memory.PrivateUsage;
            if (memory.PeakWorkingSetSize > peak_working) peak_working = memory.PeakWorkingSetSize;
        }
        wait_result = WaitForSingleObject(process.hProcess, 10);
        if (wait_result == WAIT_OBJECT_0) break;
        if (wait_result != WAIT_TIMEOUT) goto failure;
    }
    done = 1;
    if (!QueryPerformanceCounter(&ended) || !GetExitCodeProcess(process.hProcess, &exit_code)) goto failure;
    // The retained handle can expose a final peak even for a very short run.
    {
        MemoryCounters memory = {0};
        memory.cb = sizeof(memory);
        if (GetProcessMemoryInfo(process.hProcess, &memory, sizeof(memory))) {
            if (memory.PeakPagefileUsage > peak_commit) peak_commit = memory.PeakPagefileUsage;
            if (memory.PeakWorkingSetSize > peak_working) peak_working = memory.PeakWorkingSetSize;
        }
    }
    if (!QueryInformationJobObject(job, JobObjectExtendedLimitInformation, &limits, sizeof(limits), NULL)) goto failure;
    if (limits.PeakProcessMemoryUsed > peak_commit) peak_commit = limits.PeakProcessMemoryUsed;
    printf("{\"exit_code\":%lu,\"seconds\":%.9f,\"peak_commit_bytes\":%llu,\"peak_working_set_bytes\":%llu}\n",
           (unsigned long)exit_code, (double)(ended.QuadPart - started.QuadPart) / frequency.QuadPart,
           (unsigned long long)peak_commit, (unsigned long long)peak_working);
    result = 0;
    goto cleanup;
failure:
    fprintf(stderr, "WinAPI monitor failed (Windows error %lu)\n", (unsigned long)GetLastError());
cleanup:
    if (process.hProcess && !done) {
        TerminateProcess(process.hProcess, 1);
        WaitForSingleObject(process.hProcess, INFINITE);
    }
    if (process.hThread) CloseHandle(process.hThread);
    if (process.hProcess) CloseHandle(process.hProcess);
    if (job) CloseHandle(job);
    if (input != INVALID_HANDLE_VALUE) CloseHandle(input);
    if (output != INVALID_HANDLE_VALUE) CloseHandle(output);
    if (errors != INVALID_HANDLE_VALUE) CloseHandle(errors);
    if (argv) LocalFree(argv);
    return result;
}
`;

// Windows CRT quoting: double backslashes before a quote or the closing quote.
function quoteArgument(value) {
    return '"' + value.replace(/(\\*)"/g, '$1$1\\"').replace(/\\+$/g, '$&$&') + '"';
}

function run(executable, args, stdin = 'ignore') {
    return new Promise((resolve, reject) => {
        const child = spawn(executable, args, { windowsHide: true, stdio: [stdin, 'pipe', 'pipe'] });
        let stdout = '',
            stderr = '';
        child.stdout.setEncoding('utf8');
        child.stderr.setEncoding('utf8');
        child.stdout.on('data', (data) => {
            stdout += data;
        });
        child.stderr.on('data', (data) => {
            stderr += data;
        });
        child.once('error', reject);
        child.once('close', (code, signal) => {
            if (code !== 0) reject(new Error(`${path.basename(executable)} failed (${code ?? signal}): ${stderr}`));
            else resolve(stdout);
        });
    });
}

function readErrors(file) {
    const fd = fs.openSync(file, 'r');
    try {
        const buffer = Buffer.alloc(1024 * 1024);
        return buffer.subarray(0, fs.readSync(fd, buffer, 0, buffer.length, 0)).toString('utf8');
    } finally {
        fs.closeSync(fd);
    }
}

async function profile(command, stdoutPath = null, limitMib = null, { compiler = command?.[0] } = {}) {
    if (process.platform !== 'win32') throw new Error('Windows process counters are required.');
    if (
        !Array.isArray(command) ||
        command.length === 0 ||
        command.some((value) => typeof value !== 'string' || value.includes('\0'))
    )
        throw new Error('A nonempty compiler command of strings is required.');
    if (typeof compiler !== 'string' || !compiler || compiler.includes('\0'))
        throw new Error('A TCC compiler is required to build the native monitor.');
    if (
        limitMib !== null &&
        (!Number.isSafeInteger(limitMib) || limitMib <= 0 || !Number.isSafeInteger(limitMib * 1024 * 1024))
    )
        throw new Error('Memory limit must be a positive integer in MiB.');
    const tempRoot = path.resolve(os.tmpdir());
    const directory = fs.mkdtempSync(path.join(tempRoot, 'obfh-memory-'));
    try {
        const source = path.join(directory, 'monitor.c'),
            monitor = path.join(directory, 'monitor.exe');
        const errors = path.join(directory, 'stderr.log');
        const output = stdoutPath === null ? path.join(directory, 'stdout.log') : path.resolve(stdoutPath);
        fs.writeFileSync(source, monitorSource);
        await run(compiler, ['-w', source, '-o', monitor, '-lpsapi', '-lshell32']);
        const metrics = JSON.parse(
            await run(
                monitor,
                [
                    command.map(quoteArgument).join(' '),
                    output,
                    errors,
                    String(limitMib === null ? 0 : limitMib * 1024 * 1024)
                ],
                'inherit'
            )
        );
        return { command: [...command], ...metrics, limit_mib: limitMib, stderr: readErrors(errors) };
    } finally {
        // Delete only the exact directory owned by this invocation.
        const resolved = path.resolve(directory);
        if (path.dirname(resolved) !== tempRoot || !path.basename(resolved).startsWith('obfh-memory-'))
            throw new Error('Unexpected profiling temporary directory.');
        fs.rmSync(resolved, { recursive: true, force: true });
    }
}

async function main(args) {
    let stdoutPath = null,
        jsonPath = null,
        limitMib = null,
        index = 0;
    const usage = 'node tests/compiler_memory.js [--stdout file] [--json file] [--limit-mib N] -- tcc.exe [arguments]';
    while (index < args.length && args[index].startsWith('--')) {
        const flag = args[index++];
        if (flag === '--') break;
        if (flag === '--help') {
            console.log(usage);
            return;
        }
        if (!['--stdout', '--json', '--limit-mib'].includes(flag) || index === args.length) throw new Error(usage);
        const value = args[index++];
        if (flag === '--stdout') stdoutPath = value;
        if (flag === '--json') jsonPath = value;
        if (flag === '--limit-mib') limitMib = /^\d+$/.test(value) ? Number(value) : NaN;
    }
    const result = await profile(args.slice(index), stdoutPath, limitMib);
    const text = JSON.stringify(result, null, 2);
    if (jsonPath !== null) fs.writeFileSync(jsonPath, text, 'utf8');
    console.log(text);
    process.exitCode = result.exit_code | 0;
}

module.exports = { profile };
if (require.main === module)
    main(process.argv.slice(2)).catch((error) => {
        console.error(error.message);
        process.exitCode = 1;
    });
