# Automated Windows TCC tests

```powershell
node tests/run.js
```

Requires Node.js 18+ and the Windows TCC distribution containing both `tcc.exe` and `i386-win32-tcc.exe`. The runner finds `Desktop/tcc/tcc` automatically, including a redirected OneDrive desktop. Override with `TCC_DIR` or `node tests/run.js --tcc-dir=C:\path\to\tcc`. No Python or npm packages are required.

`antidebug.c` checks statement syntax, normal execution, PEB fallback and remote nonreturning responses in both architectures. Response tests require a timeout after the entry marker and terminate the process tree; returning or crashing fails. Injected positive evidence must reach that response through `ANTI_DEBUG`. `antidebug_host.c` also launches the protected child under a real Windows debugger and requires detection followed by a live response without invalid-instruction exceptions. `failures.c.in` exercises the production probes with failed WinAPI calls, late worker completion, active/reserved debug-register bits and missing module/exports. Focused run: `node tests/run.js --only-antidebug`.

One command builds and runs x86 and x64 binaries across six configurations: no obfuscation, default protection, VM without control flow, VM with default flow, advanced protection, and math virtualization. Compilation failures, missing results, crashes and timeouts fail the verdict. Exit code is 0 only when every required check passes. Artifacts, per-process output/timing logs and a machine-readable `results.json` remain in the printed temporary directory for inspection. Builds have a 15-second deadline, executions 30 seconds, and the suite 10 minutes. Timeout is a failure and terminates the entire child process tree; a deliberately hanging parent/child pair verifies this cleanup. Long-running tests print elapsed time and the latest reported phase. Override with `--build-timeout-ms=15000`, `--test-timeout-ms=30000` or `--suite-timeout-ms=600000`.

Coverage replaces the former manual examples:

| Former example                              | Automated replacement                                                                                                                                                |
| ------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `fib.c`, `sorter.c`                         | Recursion with known Fibonacci results; randomized insertion sort against `qsort`; nested control flow and side effects                                              |
| `virtualmachine.c`, `virtualmachine_unit.c` | Every VM arithmetic/comparison/identity/branch macro, randomized reference results, boundary values and four concurrent workers                                      |
| `dll.c`, `hello_dll.c`                      | Build a protected DLL, dynamic client and import-linked client; resolve/import exported code/data, mutate shared data and verify 2001 calls in each client           |
| `hello_win.c`                               | Create a hidden window, exercise paint/create/destroy callbacks, geometry and positioning; close automatically                                                       |
| Old disassembly examples                    | Headless snake-style array movement; binary checks for hidden strings, protected sections, CPUID, RDTSCP, x64 SSE conversions and absence of selected direct imports |
| String/WinAPI regressions                   | Full-header file/memory/thread/event/window calls, long strings, guarded formatting and path boundaries                                                              |

The VM tests instrument actual calls to `Obfh_VirtualMachine`, so replacing protected arithmetic with normal C cannot pass just because results agree. The runner also builds a deliberately mutated header with `VM_ADD` bypassed and requires the test to fail. Branch tests additionally trace the dedicated VM command and all six microinstruction states, and reject ordinary-C replacements of each of VM_IF, VM_ELSE_IF and VM_ELSE. They cover side effects, fractional/pointer/NaN conditions, short circuiting, unbraced nesting, recursion and concurrent calls. These temporary mutants never modify the real header.

`integration.c` contains deliberate overflow, unbounded formatting and use-after-free modes. Each runs in a separate child process and must produce the expected access violation in both plain and protected builds. They are crash controls; invalid C is not made safe by obfuscation. Normal stress cases use valid inputs and guard bytes/pages.

`failures.c.in` mocks all thread-setup/wait/exit-code branches using the current header's setup block. `math.c` bridges `nan` and `remquo` to UCRT because old msvcrt does not export them; UCRT is required. Old decompiler snapshots are removed: structural binary checks do not claim immunity to every decompiler.

`protection.c` checks fractional/pointer conditions, loader calls, long console output and embedded NUL characters through a private console buffer.

`cflow.c` checks automatic if/while interception in both modes, all 65,535 call-site parameters and four conversion routes, scalar truth, side effects, recursion and dangling else around for/switch. Handler tracing detects deliberately bypassed if and while macros. Run the focused checks with `node tests/run.js --only-cflow`.

`numeric.c` compares math and VM results with native operations, including signed zero, subnormals, tiny fractions, extreme finite values, infinities, quiet NaNs and 1,500 random bit patterns. It checks exact identity transport and single evaluation. Focused run: `node tests/run.js --only-numeric`.

`charset.c` exercises ANSI/Unicode calls and Unicode filenames. The multi-file tests build EXEs and DLLs across all configurations, call their exports and require a single cow export with no exported internal helpers. Focused run: `node tests/run.js --only-platform`.

`cache.c` exercises cold concurrent CRT resolution with eight workers, warm reuse, copied cache names, missing exports and capacity fallback. A cache-bypass mutant must fail the reuse check. The cache retains one CRT module reference per translation unit to keep cached addresses valid. Focused run: `node tests/run.js --only-cache`.

Passing the suite establishes the tested contracts on these compiler builds and this Windows environment, not universal safety of arbitrary programs or third-party protectors.

`junk.c` executes all 13 BREAK_STACK variants with live integer, pointer, floating-point and memory values in five stress runs, including four concurrent workers. Five build seeds exercise VM dispatch in both control-flow modes. Ordinary templates also contain four separately mixed byte values, a 32-bit junk immediate and a variable-length fill, all inside skipped regions. The binary check verifies the original jump lands at CPUID and that the bytes do not share one fixed affine pattern across the tested seeds. PE export boundaries isolate each site's machine code: all sites must change with a different seed and repeat with the same seed. Existing CPUID variants retain that instruction; the four lightweight variants omit it. Focused run: `node tests/run.js --only-junk`.

BREAK_STACK_CFLOW is the single public macro, used without arguments at every library insertion and inside automatic if interception. Its shared pool contains 128 templates: the previous 86 CFLOW combinations, all 13 former numbered variants and 29 paired predicates with additional branch layouts. Each expansion captures a fresh counter and mixes it with OBFH_BUILD_SEED before compile-time selection. Seven CPUID variants occupy 7 of 256 selection slots; the other 121 share the remaining slots. CFLOW_V2 emits a different second template from the lightweight pool. No runtime dispatcher is added. Selection, immediates and skipped byte payloads vary; repetitions remain possible. The flow tests execute all 128 selections and reject a bypass of the complete if macro. cflow_junk.c checks every template with live values and concurrent workers on five seeds. break_predicates.c replaces the stack-derived input with arbitrary values and checks the actual ASM flags for all 29 new identities on random inputs, byte boundaries and powers of two, on five seeds and both architectures. break_random.js generates 512 identical public-macro call sites and compares emitted selections with an independent uint32 reference on both architectures, measures distributions/repetitions, verifies seed changes and exact fixed-seed machine-code reproducibility. Sampling reports are saved as x64-break-random.json and x86-break-random.json in the artifact directory.

HIDE_STRING emits exactly one CFLOW template before its original expression, including with CFLOW_V2 or NO_CFLOW. Its compound literal remains in the caller scope. Wrapper checks retain several hidden-string pointers across other calls, exercise empty strings and verify independent writable buffers.

`choose_random.c` checks direct and captured `RND` expressions against the branches selected by `__builtin_choose_expr`, on both architectures and two seeds. It also checks that every advanced-mode secondary index differs from its primary and omits the CPUID variants.

`constant_data.c` checks the original volatile character/string/numeric types and values, including string terminators. Random retained data separates those objects in the binary. The data generator mixes the seed, source line and byte number without consuming `__COUNTER__`; the test guards that independence. Five seeds on x86/x64 must remove the old alphabet and numeric byte signatures; dumped padding must change by seed and reproduce exactly with a fixed seed. Ordinary probe binaries also reject those signatures without referencing the padding objects. Focused run: `node tests/run.js --only-data`.
