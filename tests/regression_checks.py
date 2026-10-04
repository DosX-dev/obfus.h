"""Isolate changed helpers/macros from unrelated obfuscation assembly.
Usage: python tests/regression_checks.py path/to/tcc.exe
"""
from pathlib import Path
import re
import subprocess
import sys
import tempfile

root = Path(__file__).resolve().parents[1]
source = (root / "include/obfus.h").read_text(encoding="utf-8")
def function(name):
    match = re.search(r"^(?:ULONG_PTR|char \*)\s*" + name + r"\([^\n]*\{", source, re.M)
    start = match.start()
    end = source.index("\n}", start) + 2
    return source[start:end]

preamble = """#define OBFH_SECTION_ATTRIBUTE
#define SALT_SHIFT 3
#define BREAK_STACK_1
#define BAD_JMP
#define _0 0
#define _1 1
#define _2 2
#define TRUE 1
#define FALSE 0
int obfh_condition_true(void) { return 1; }
int obfh_int_proxy(int value) { return value; }
"""
sections = preamble + function("obfh_uintptr_proxy") + "\n" + function("obfh_process_hidden_string")
sections += "\n#define OBFH_PTR(type, value) ((type)obfh_uintptr_proxy((ULONG_PTR)(value)))\n"
sections += source[source.index("#define GetParent(hWnd)"):source.index("#define abs(x)")]
with tempfile.TemporaryDirectory(prefix="obfh-regression-") as temp:
    directory = Path(temp)
    (directory / "regression_sections.h").write_text(sections, encoding="utf-8")
    exe = directory / "regression.exe"
    subprocess.run([sys.argv[1], "-w", "-I" + temp, str(root / "tests/regression_wrappers.c"), "-o", str(exe), "-luser32", "-lgdi32"], check=True)
    subprocess.run([str(exe)], check=True)

# Exercise every setup/wait/exit-code branch with deterministic WinAPI failures.
block = source[source.index("    // Registers validation"):source.index("    // Dynamic antidebugger")]
harness = r"""
#include <windows.h>
#include <stdio.h>
#define _0 0
#define FALSE 0
static int mode, duplicate_calls, create_calls, wait_calls, exit_calls, closed_worker, closed_main;
static BOOL fake_duplicate(HANDLE a, HANDLE b, HANDLE c, HANDLE *out, DWORD d, BOOL e, DWORD f) {
    duplicate_calls++; if (mode == 1) return FALSE; *out = (HANDLE)11; return TRUE;
}
static HANDLE fake_create(void *a, SIZE_T b, void *c, void *d, DWORD e, DWORD *f) {
    create_calls++; return mode == 2 ? NULL : (HANDLE)22;
}
static DWORD fake_wait(HANDLE h, DWORD timeout) { wait_calls++; return mode == 3 ? WAIT_FAILED : WAIT_OBJECT_0; }
static BOOL fake_exit(HANDLE h, DWORD *out) { exit_calls++; if (mode == 4) return FALSE; *out = mode == 5 ? 7 : 0; return TRUE; }
static BOOL fake_close(HANDLE h) { if (h == (HANDLE)11) closed_main++; else if (h == (HANDLE)22) closed_worker++; else return FALSE; return TRUE; }
#define DuplicateHandle fake_duplicate
#define CreateThread fake_create
#define WaitForSingleObject fake_wait
#define GetExitCodeThread fake_exit
#define CloseHandle fake_close
#define ThreadCompareDRs NULL
int check_setup(void) {
""" + block + r"""
    return 0;
}
int main(void) {
    for (mode = 0; mode <= 5; mode++) {
        duplicate_calls = create_calls = wait_calls = exit_calls = closed_worker = closed_main = 0;
        int result = check_setup();
        if (result != (mode == 5 ? 7 : 0) || duplicate_calls != 1 ||
            create_calls != (mode != 1) || wait_calls != (mode != 1 && mode != 2) ||
            exit_calls != (mode != 1 && mode != 2 && mode != 3) ||
            closed_main != (mode == 2) || closed_worker != (mode != 1 && mode != 2)) return 1;
    }
    puts("ANTIDEBUG_V2 failure branches passed"); return 0;
}
"""
with tempfile.TemporaryDirectory(prefix="obfh-failures-") as temp:
    directory = Path(temp)
    file = directory / "failures.c"
    file.write_text(harness, encoding="utf-8")
    exe = directory / "failures.exe"
    subprocess.run([sys.argv[1], "-w", str(file), "-o", str(exe)], check=True)
    subprocess.run([str(exe)], check=True)
