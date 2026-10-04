# Automated Windows TCC tests

```powershell
node tests/run.js
```

Requires Node.js 18+ and the Windows TCC distribution containing both `tcc.exe` and `i386-win32-tcc.exe`. The runner finds `Desktop/tcc/tcc` automatically, including a redirected OneDrive desktop. Override with `TCC_DIR` or `node tests/run.js --tcc-dir=C:\path\to\tcc`. No Python or npm packages are required.

One command builds and runs x86 and x64 binaries across six configurations: no obfuscation, default protection, VM without control flow, VM with default flow, advanced protection, and math virtualization. Compilation failures, missing results, crashes and timeouts fail the verdict. Exit code is 0 only when every required check passes. Artifacts, per-process output/timing logs and a machine-readable `results.json` remain in the printed temporary directory for inspection. Builds have a 15-second deadline, executions 30 seconds, and the suite 10 minutes. Timeout is a failure and terminates the entire child process tree; a deliberately hanging parent/child pair verifies this cleanup. Long-running tests print elapsed time and the latest reported phase. Override with `--build-timeout-ms=15000`, `--test-timeout-ms=30000` or `--suite-timeout-ms=600000`.

Coverage replaces the former manual examples:

| Former example | Automated replacement |
| --- | --- |
| `fib.c`, `sorter.c` | Recursion with known Fibonacci results; randomized insertion sort against `qsort`; nested control flow and side effects |
| `virtualmachine.c`, `virtualmachine_unit.c` | Every VM arithmetic/comparison/identity/branch macro, randomized reference results, boundary values and four concurrent workers |
| `dll.c`, `hello_dll.c` | Build a protected DLL, dynamic client and import-linked client; resolve/import exported code/data, mutate shared data and verify 2001 calls in each client |
| `hello_win.c` | Create a hidden window, exercise paint/create/destroy callbacks, geometry and positioning; close automatically |
| Old disassembly examples | Headless snake-style array movement; binary checks for hidden strings, protected sections, CPUID, RDTSCP, x64 SSE conversions and absence of selected direct imports |
| String/WinAPI regressions | Full-header file/memory/thread/event/window calls, long strings, guarded formatting and path boundaries |

The VM tests instrument actual calls to `Obfh_VirtualMachine`, so replacing protected arithmetic with normal C cannot pass just because results agree. The runner also builds a deliberately mutated header with `VM_ADD` bypassed and requires the test to fail. Branch tests additionally trace the dedicated VM command and all six microinstruction states, and reject ordinary-C replacements of each of VM_IF, VM_ELSE_IF and VM_ELSE. They cover side effects, fractional/pointer/NaN conditions, short circuiting, unbraced nesting, recursion and concurrent calls. These temporary mutants never modify the real header.

`integration.c` contains deliberate overflow, unbounded formatting and use-after-free modes. Each runs in a separate child process and must produce the expected access violation in both plain and protected builds. They are crash controls; invalid C is not made safe by obfuscation. Normal stress cases use valid inputs and guard bytes/pages.

`failures.c.in` mocks all thread-setup/wait/exit-code branches using the current header's setup block. `math.c` bridges `nan` and `remquo` to UCRT because old msvcrt does not export them; UCRT is required. Old decompiler snapshots are removed: structural binary checks do not claim immunity to every decompiler.

`protection.c` checks fractional/pointer conditions, loader calls and long console output through a private console buffer.

See [REVIEW.md](REVIEW.md) for the implementation changes and protection tradeoffs. Passing the suite establishes the tested contracts on these compiler builds and this Windows environment, not universal safety of arbitrary programs or third-party protectors.
