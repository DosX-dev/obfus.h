# Build-memory optimization — 2026-10-07

The optimization changes compile-time representation and parameter capture. It retains the 128 CFLOW templates, 128 stack-proxy templates, weighted selection, skipped payloads, local condition graphs, native inlining and existing public API. There is no shared runtime replacement for the insertions.

## Measurements

Windows TCC 0.9.27 x64; AdvCalc source body and resource object unchanged; `CFLOW_V2=1`, `VIRT=1`, default seed. Source copies differ only in the included header. Measurements are individual sequential runs, not timing medians. MiB means 1,048,576 bytes.

| Measurement | Original | Optimized |
| --- | ---: | ---: |
| Preprocessor peak committed memory | 1587.1 MiB | 212.2 MiB |
| Preprocessor peak working set | 810.2 MiB | 119.1 MiB |
| Direct compiler peak committed memory | 1618.6 MiB | 240.4 MiB |
| Direct compiler peak working set | 840.9 MiB | 148.1 MiB |
| Expanded C on disk | 485.0 MiB | 255.4 MiB |
| Preprocessing time | 14.32 s | 3.07 s |
| Direct compilation time | 13.54 s | 4.50 s |
| EXE size | 474,624 bytes | 473,600 bytes |

Both optimized runs succeeded with a Windows job limiting that compiler to 512 MiB of committed memory. The original direct build failed under the same limit with `memory full (malloc)`. The job did not modify system/pagefile settings. This verifies a compiler memory budget for this corpus; it is not a measurement of total RAM on an 8 GiB computer.

The lightweight editor fixture expands to 478,871 bytes; its TCC preprocessing peak is 8.1 MiB. This is a check of the editor representation, not a measurement of the actual cpptools process. Real TCC builds keep full protection even when `__INTELLISENSE__` is defined.

## Implementation and equivalence

- Capture ASM immediate parameters once in the local selection scope. Every parameter retains its previous range and purpose; CFLOW v2 captures separate parameters for its second insertion.
- Cache compact, equivalent ASM fragments above the selectors. Readable layout descriptions remain in the header; `node tests/compact_asm.js` regenerates a reference with both preprocessors and checks it. Application builds need no generator.
- Capture the second CFLOW index before feeding it to the 128-way selector, preventing repeated expansion of its mapping expression.
- Remove redundant parentheses from `RND`, keeping its exact arithmetic, counter, source-position and seed dependencies. `random_constants.c` checks it against the original formula.
- Provide a lightweight editor interface, including protected VM result types and standard-library aliases. The TCC implementation is isolated from this view.

An A/B build with fixed immediate parameters compared every forced CFLOW and stack-proxy insertion: 256 functions per architecture, 512 in total. Instruction bytes matched after normalizing the address of the same `_0` object in CFLOW forms 87 and 88. The readable/compact fragment check also passed after clang-format on a temporary header copy; the formatted header compiled and executed the interface fixture.

Random choices can differ from an older header because capture reduces consumed counter draws and source positions change. Same-source/same-header/same-seed reproducibility remains tested. The compact cache contains compiler ASM text only; it creates no runtime instruction table or centralized dispatcher.

## Snapshots and commands

Original header SHA-256: `41e0b9475ea33c9acfe3196921dbb422731a428b5c733edbe623e1b3820ff158`.

Optimized header SHA-256: `024d9cf4b84aba1f68e1b937f248ed2598b414addad4fe4e3ec6c4d44638d2e8`.

Raw snapshots, measurement JSON, capped failure and equivalence scripts are in the local `TEMPORARY/memory-opt` experiment directory. They are not required by users of the header.

```powershell
node tests/compact_asm.js
node tests/run.js --only-flow-tokens
node tests/run.js --only-stack-proxy
node tests/run.js --build-timeout-ms=120000
node tests/compiler_memory.js --stdout expanded.i --json memory.json --limit-mib 512 -- C:\path\to\tcc.exe -w -E project.c
```

Final verification is recorded below, including the remaining toolchain failure.

The optional profiler was subsequently ported to `compiler_memory.js`, with an embedded WinAPI monitor bootstrapped by TCC and no Python/npm dependency. Its source was reviewed without rerunning measurements. Historical measurements below were obtained with the original Python profiler; they are not runtime validation of the replacement.

## Follow-up experiments — no additional candidate adopted

The follow-up used the reorganized header (SHA-256 `7cd3e9afc71442e437622da86ba1db1c26066f7b0615dbc7a41aa3a32d067a07`) and the same AdvCalc body/resources. Builds ran sequentially under the same 512 MiB process commit cap. Direct-build values below are medians of three runs with alternating A/B order; preprocessing is one run per version.

| Header | Direct build peak commit | Direct build time | Preprocessor peak commit | Expanded C |
| --- | ---: | ---: | ---: | ---: |
| before | 240.06 MiB | 4.49 s | 212.41 MiB | 255.45 MiB |
| candidate | 239.96 MiB | 4.48 s | 212.22 MiB | 245.89 MiB |

The final experimental candidate declares only named ASM immediate operands actually referenced by each stack-proxy template. All 128 templates and all captured random parameters remain. Referenced operand sets were extracted from concatenated ASM literals for both architectures, with their union used per template. Average declarations dropped from 26 to 16.94. All 256 forced CFLOW/stack-proxy functions per architecture (512 total) matched baseline machine code with fixed parameters and normalization of the `_0` address.

Two earlier probes also failed to produce useful savings: capturing VM encoding/exit-selector constants yielded about 238.3 MiB in a single direct build; flattening CFLOW ASM literals yielded about 238.8 MiB while adding roughly 109 KB of generated header text. Neither was adopted. These probes are exploratory single measurements, not evidence of a reliable improvement.

The final repeated experiment saved about 0.1 MiB of direct-build peak commit despite shrinking expanded C by 9.56 MiB. The extra operand-set maintenance is not justified by that reduction. No follow-up runtime/header changes were transferred to `include/obfus.h`; baseline source remained byte-identical throughout verification. Further large savings are not established by these experiments. Local snapshots/scripts/results live in `TEMPORARY/memory-opt/round2`.

## Verification and remaining bottlenecks

Verified header SHA-256: `7cd3e9afc71442e437622da86ba1db1c26066f7b0615dbc7a41aa3a32d067a07`. The full sequential suite completed **386/387** checks on an immutable snapshot (`obfh-js-suite-jp2FSS`). The only failure was `x64/default/numeric`, an ordered quiet-NaN comparison. All seeds, VM instruction/mutation checks, CFLOW transport/mutation checks, all 128 CFLOW and stack-proxy templates, native decoy bodies, seed distribution/reproducibility, EXE/DLL, multiple translation units, CRT/GUI/kernel integrations and debugger tests otherwise passed. The header remained unchanged throughout the completed run.

The runner now freezes its ordinary test C inputs and header, records the header hash and retains separate instrumented/mutant headers. A stale anti-debug fallback mutation used an exact one-line string; it now matches the same statement across formatter whitespace. Runtime anti-debug code was unchanged.

AdvCalc with the verified header passed 21 expressions, radians, 120 history shifts and recall against the previous release. A clang-format copy preserved significant tokens/ASM literals and passed editor/real-compiler interface execution on x86 and x64 (four checks). The API-name-key experiment computed each constant byte key once instead of expanding its formula for every character. Its single capped A/B build used 240.08 versus 239.84 MiB; that saving does not justify the extra definitions, so it was not adopted.

### Quiet-NaN toolchain issue: not hidden as a pass

The original pre-optimization header passed the default numeric test at seed 0 but failed at seeds 1, 2, 0xDEADBEEF and 0xFFFFFFFF. The optimized header failed the same ordered-comparison contract under all five tested seeds. This is a seed-sensitive pre-existing correctness issue; ordinary integer/GUI tests do not establish correctness of these floating-point cases.

`tests/tcc_nan.c` reproduces incorrect x64 TCC 0.9.27 results **without including obfus.h**: `!(NaN < 1)` produces an incorrect value in certain ternary/comma contexts. The tested x86 build returns the correct values. CFLOW's randomized polarity can expose that compiler behavior. Native value/branch comparisons without these contexts passed, so the issue must not be generalized to all TCC comparisons.

A trial materializing truth before encoding passed numeric tests for both architectures and all five seeds, but a stronger side-effect/comma regression exposed remaining compiler errors. Three alternative capture forms also failed that stronger case. The trial was rejected, the second full run stopped after the new early regressions failed, and the main header restored byte-for-byte to the successfully completed 386/387 snapshot. No partial runtime workaround or reduced protection was shipped. Local diagnostic outputs and experiments are in `TEMPORARY/memory-opt/verify`.

The remaining large expansion comes from compile-time alternatives and locally inlined kernels. Eliminating variants, sharing runtime bodies or coupling independent random parameters would alter protection and was not attempted. No further substantial memory reduction is established by these experiments.

Final source/editor/RND/immutability verification passed 15/15 checks on both architectures (obfh-js-suite-S3tUbI).

### CFLOW fix for compiler-sensitive negation (2026-10-07)

The final header replaces the randomized `!(condition)` arm in `OBFH_FLOW_CONDITION` with randomized tag permutation followed by XOR restoration. The user expression occurs once and no extra logical negation is introduced. The encoded state, graph families, stage transforms, address selectors and junk pools remain in place; this is a correctness change, not a reduction of the protection machinery.

Header SHA-256: `ca810e16c961a1aeb3d303a2a3d1ace1b7b97c3cc192a1679d6c50b046c43121`. The complete sequential suite passed **387/387**, with no failures and an unchanged header (`obfh-js-suite-0iSYb8`). The formerly failing x64/default/numeric check passes. Focused numeric builds also passed all five seeds on both architectures. CFLOW trace, stage/bypass mutants, both modes and all five seeds passed on both architectures.

The subsequently formatted header has SHA-256 `66818f5684f0928854ecea6126f7c61b6e7cfb66b4d582dd80f0211d2a221d5f`. Its only differences from the tested header are whitespace alignment on the six new CFLOW initialization lines; tokens, line numbers and random-expression expansion are unchanged.

The old header was retested against the unchanged numeric assertions: seed 0 fails `!VM_GTR_DBL(x, y)` at line 62 and seed 1 fails `!VM_GEQ(x, y)` at line 64. Additional `cflow.c` coverage checks eight ordered NaN comparisons in both operand orders with comma/side effects and a finite-value control. Those positive-comparison probes also pass the old header and are additional semantic coverage, rather than the discriminator for this particular bug; the numeric assertions supply that regression control.

AdvCalc comparison used the same source/resources and the immediate pre-fix header, with a 512 MiB process memory cap. One build each measured peak commit 238.64 MiB before and 239.50 MiB after, and EXE size 474,624 versus 475,648 bytes (+1,024). These single measurements are a smoke check, not a speed benchmark. Both GUI builds passed 21 expressions, radians, 120 history shifts and recall. Formatting a copy preserved all 78,177 significant tokens and ASM strings.

The separate TCC 0.9.27 x64 defect remains reproducible without protection in `tests/tcc_nan.c`: some user-authored negated ordered NaN comparisons in ternary/comma contexts are miscompiled. Removing header-injected negation does not patch that compiler. The installed toolchain was not replaced. Artifacts are in `TEMPORARY/tcc-nan-fix` (`full.log`, `numeric.json`, `final.json`, `finish.log`).
