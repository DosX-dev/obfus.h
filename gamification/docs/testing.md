# Tests and local acceptance

## Local commands

Use Node.js 24 and run commands from this directory:

```text
node --test test/*.test.js
node cli.js test
node cli.js preview --toolchain PATH_TO_TCC_DIRECTORY
node cli.js acceptance --toolchain PATH_TO_TCC_DIRECTORY --count 10
```

`preview` builds locally without publishing. Private sources, targets, diagnostic logs and replay material are kept under ignored `out/`; never upload this working directory as an Actions artifact. A completed preview's `public/` directory contains only the intended player files and authenticated encrypted answers.

Tests check deterministic generation, domain separation, forward/inverse models, 1000 generated programs per level, meaningful structural fingerprints, answer authentication and publication recovery. Protected probes compare all intermediate states with the model and check correct/incorrect input, changed characters, transpositions and lengths. A passing model suite alone does not replace compiler, GUI, mutation or reverse-engineering acceptance.

Acceptance builds ten candidates per difficulty on both x86 and x64. It checks every possible single-byte replacement, including NUL and non-ASCII values; full-block boundary traces; removal of a real stage; a corrupted Hard VM operation; and an omitted final comparison coordinate. GUI checks cover repeated attempts, Enter, ANSI/Unicode clipboard paste, deliberately occupied clipboard recovery, and clean shutdown. Generated source and all test entry points remain private; final players are checked for plaintext passwords, test markers, symbols and intact logo resources. A verification failure cancels publication rather than silently reducing coverage.

The acceptance report also compares multisets of complete update-expression shapes, with constants and coordinate labels removed. This Jaccard distance makes it possible to detect variation that consists merely of renamed coordinates or rescheduled independent updates. It is a structural metric rather than a percentage of source lines or an estimate of solve time. Local decompiler reviews can guide the owner, but a guided entry-point analysis is not a blind difficulty assessment.

## Formatting

Use VS Code’s built-in JavaScript formatter with four-space indentation; do not introduce Prettier. C uses the repository .clang-format. PowerShell and embedded C# use four-space indentation and explicit statement/block layout. JSON uses four spaces; workflow YAML uses two. Keep generated private outputs out of formatting/search commands.

After refactoring, compare deterministic IRs, fingerprints and emitted C sources, run the Node tests, and complete a real local preview. If generated C changes, protected compiler/probe checks must pass again. Changes limited to publication should exercise request interruptions and replay.

Architecture tests check import boundaries; bundle tests check the public/private file boundary. Generator tests exercise independent inverse models and the century-scale variation run. Publication and transport tests inject failures without writing to GitHub. Retry tests check all three generation attempts and private diagnostics.

Algorithm tests independently check shift-XOR against BigInt arithmetic (including every byte), dependent update controls at numeric boundaries, both conditional paths, and unequal-partition Feistel routing/inversion. Negative admission tests reject insufficiently linked Medium checks and missing Hard rounds. Fingerprint tests distinguish conditional branch order and changed partition connections. Acceptance reports include each program's structural difficulty summary, separately from its native/VM test results.

Novelty tests reject near-copies even with three fresh hashes, verify bounded recent-profile retention and authenticated reload, and reject malformed profile data. A 100-day simulation exercises all three levels through both the complete fingerprint history and recent similarity gate under the normal three-attempt budget.

Recovery regressions also cover loss of both the committed manifest and every release asset. Rejected novelty attempts retain private diagnostics without reaching the compiler. GUI acceptance checks About through both command activation and Enter, closes it with No, and verifies that this never invokes the password check or opens the browser.
