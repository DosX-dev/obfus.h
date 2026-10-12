# Tests and local acceptance

## Local commands

Use Node.js 24 and run commands from this directory:

```text
node --test test/*.test.js
node cli.js test
node cli.js preview --toolchain PATH_TO_TCC_DIRECTORY
node cli.js acceptance --toolchain PATH_TO_TCC_DIRECTORY --count 10
node test/long-horizon.js 3653
```

`preview` builds locally without publishing. Private sources, targets, diagnostic logs and replay material are kept under ignored `out/`; never upload this working directory as an Actions artifact. A completed preview's `public/` directory contains only four player ZIPs. Its sibling `state/` directory contains authenticated encrypted answers and compressed history.

Tests check deterministic generation, domain separation, forward/inverse models, 1000 generated programs per level, meaningful structural fingerprints, answer authentication and publication recovery. Protected probes compare all intermediate states with the model and check correct/incorrect input, changed characters, transpositions and lengths. A passing model suite alone does not replace compiler, GUI, mutation or reverse-engineering acceptance.

Actions set `CRACKME_QUICK_TESTS=1`: the model sample is 100 programs per difficulty and the 36,525-day Easy simulation is skipped. Exhaustive primitive tests, family checks, 20-day four-level novelty admission, publication recovery and native/player verification remain enabled. Without that environment variable, `npm test` retains the full 1000-program samples and century simulation for deeper local validation.

Acceptance builds ten candidates per difficulty on both x86 and x64. It checks every possible single-byte replacement, including NUL and non-ASCII values; full-block boundary traces; removal of a real stage; a corrupted Hardcore VM operation; and an omitted final comparison coordinate. GUI checks cover repeated attempts, Enter, ANSI/Unicode clipboard paste, deliberately occupied clipboard recovery, and clean shutdown. Generated source and all test entry points remain private; final players are checked for plaintext passwords, test markers, symbols and intact logo resources. A verification failure cancels publication rather than silently reducing coverage.

The acceptance report also compares multisets of complete update-expression shapes, with constants and coordinate labels removed. This Jaccard distance makes it possible to detect variation that consists merely of renamed coordinates or rescheduled independent updates. It is a structural metric rather than a percentage of source lines or an estimate of solve time. Local decompiler reviews can guide the owner, but a guided entry-point analysis is not a blind difficulty assessment.

## Formatting

Use VS Code’s built-in JavaScript formatter with four-space indentation; do not introduce Prettier. C uses the repository .clang-format. PowerShell and embedded C# use four-space indentation and explicit statement/block layout. JSON uses four spaces; workflow YAML uses two. Keep generated private outputs out of formatting/search commands.

After refactoring, compare deterministic IRs, fingerprints and emitted C sources, run the Node tests, and complete a real local preview. If generated C changes, protected compiler/probe checks must pass again. Changes limited to publication should exercise request interruptions and replay.

Architecture tests check import boundaries; bundle tests check the public/private file boundary. Generator tests exercise independent inverse models and the century-scale variation run. Publication and transport tests inject failures without writing to GitHub. Retry tests check all three generation attempts and private diagnostics.

Algorithm tests independently check shift-XOR against BigInt arithmetic (including every byte), dependent update controls at numeric boundaries, both conditional paths, and unequal-partition Feistel routing/inversion. Negative admission tests reject insufficiently linked Medium checks and missing Hardcore rounds. Fingerprint tests distinguish conditional branch order and changed partition connections. Acceptance reports include each program's structural difficulty summary, separately from its native/VM test results.

Family tests exhaust all 65,536 byte pairs for each pair-lift form and all 65,536 words for a generated 16-bit substitution, compare independent forward formulas, and verify inverse uniqueness. They also check controller restoration across dependent swaps, every generated stage at word boundaries, table nonlinearity and coefficient limits. The optional long-horizon simulation uses a public fixture key and the real three-attempt novelty gate for every difficulty across the requested number of days. It reports recent-profile distances and family coverage without compiling or publishing. This is sample evidence, not a guarantee that independently generated future algorithms share no primitives.

### Generator v3 local evidence (2026-10-12)

-   Model suite: 1000 reversible programs at each level and 36,525 daily Easy programs; no repeated structural, normalized-shape or behavioral fingerprints in the applicable checks.
-   Ten-year fixture simulation: 3653 days, 10,959 admitted challenges, no retries or repeated fingerprints. Minimum distances against each level's last seven challenges were Easy 0.6462, Medium 0.5844 and Hardcore 0.7827, above the 0.35 admission threshold. This does not compare every historical pair or measure human difficulty.
-   Native family matrix: ten generated family compositions per level on both x86/x64, 60 protected TCC probes. Accepted/rejected inputs and six complete boundary traces per probe agreed with the independent model. Hardcore executed real VM instructions.
-   Complete local round: all three levels passed both architecture probes, 50,932 input cases, intermediate states, final x64 GUI interactions and binary/password audits. Hardcore executed 108,910 VM steps on both architectures.
-   Negative controls on both architectures detected a removed new layer, corrupted VM arithmetic and an omitted comparison coordinate. Existing encrypted-answer and publication-recovery tests passed.

Reports and private generated fixtures remain under ignored `out/families-v3-*`. No release upload was performed and the full library test suite was not run for this generator-only change.

### Generator v4 focused smoke checks

The within-family changes passed focused family, algorithm, diversity, authenticated-history and stored-answer tests. A 100-day three-level simulation cleared novelty admission. Twelve protected probes (two family compositions per level on x86/x64) matched six complete boundary traces and accepted/rejected inputs. Separately, both architectures evaluated all 16 truth-table inputs of each of 64 generated Boolean substitution circuits through real Hardcore VM helpers and matched independent lookup evaluation. Fixtures remain under ignored `out/families-v4-*`. The v3 decade and century evidence above applies to v3; the v4 smoke run does not claim a new decade-long simulation or full GUI acceptance.

Novelty tests reject near-copies even with three fresh hashes, verify bounded recent-profile retention and authenticated reload, and reject malformed profile data. A 100-day simulation exercises all three levels through both the complete fingerprint history and recent similarity gate under the normal three-attempt budget.

Recovery regressions also cover loss of both the committed manifest and every release asset. Rejected novelty attempts retain private diagnostics without reaching the compiler. GUI acceptance checks About through both command activation and Enter, closes it with No, and verifies that this never invokes the password check or opens the browser.
Packaging tests check ZIP round-trip, corruption rejection and the four-link presentation. Storage tests check draft routing, visibility guards and lost creation responses. Publication tests interrupt initial, daily and extra-round publication before and after every request, using the current four-player schema.

## Fast daily publication checks

Actions set `CRACKME_FAST_VERIFICATION=1` for both preview and publish preparation. Each architecture runs 100 text inputs and 100 byte inputs per level: the correct password, every coordinate changed individually, invalid bytes across all positions, empty/short/long inputs, adjacent swaps and deterministic random incorrect strings. Six full-block boundary traces, the accepted-input trace, real VM execution, x64 GUI interactions, binary leak/resource audits, provenance and transactional publication checks remain mandatory. Failures still prevent publication. This is representative daily verification, not the exhaustive replacement matrix.

Local preparation defaults to full verification. `acceptance` also keeps the complete alphabet/byte replacement tests regardless of the fast preparation environment variable. The quick Node suite retains 100 independently generated models per level and exercises the seven-entry novelty window across 20 days; its full local mode retains 1000 models and the 100-day/century simulations. Workflow timeouts are 20 minutes as a failure ceiling, not a desired duration. Measure hosted elapsed time before claiming a fixed publication SLA.

## Local version-5 evidence (2026-10-12)

-   100 independently generated models per level and the 100-round novelty simulation passed before enabling the fast daily mode. Final fast suite: 73 tests, 72 passed, one intentional century-test skip, zero failures (about 54 seconds locally).
-   Full Extreme x86/x64 matrix: 4,581 text cases, 12,243 byte cases and six boundary traces per architecture. Both executed 501,203 VM trace steps on the accepted input. A second generated program passed both architectures, boundary traces and stage/VM/comparison negative controls. The batched full text verifier also passed.
-   Fresh four-level fast preparation passed native x86/x64, final GUI interactions and binary leak audits in 121.6 seconds locally. Each level/architecture processed 100 text and 100 byte cases. This is local timing, not a hosted Actions guarantee or a measured human-difficulty multiplier. No version-5 round was published by these checks.
