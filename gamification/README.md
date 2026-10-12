# Daily Crackmes

Four daily Windows reverse-engineering challenges protected with the repository's current `include/obfus.h`: Easy, Medium, Hardcore and Extreme. Node.js 24 generates reversible verification programs, builds them with the latest stable TinyCC-builder release, and checks them before preparing a release.

**Daily publication is active for all four levels.** Local previews never publish.

## Start locally

Run from this directory with Node.js 24:

```text
node cli.js test
node cli.js preview --toolchain PATH_TO_TCC_DIRECTORY
node cli.js acceptance --toolchain PATH_TO_TCC_DIRECTORY --count 10
```

`preview` prepares all four challenges without publishing. `test` runs model and operational tests. `acceptance` additionally verifies ten candidates per difficulty on both architectures. CLI command names and output layout remain stable; source modules are internal implementation details.

## Documentation

-   [Architecture and module contracts](docs/architecture.md): execution flow, dependencies, IR, release state and extension points.
-   [Generator and diversity](docs/generator.md): reversible operations, difficulty levels, numeric bounds and variation guarantees.
-   [Tests and formatting](docs/testing.md): model checks, protected builds, GUI checks and acceptance evidence.
-   [Activation and recovery](docs/operations.md): automatic compiler selection, secrets, workflow permissions and interruption handling.

## Files and publication

`src/` contains implementation modules; `test/` contains Node tests. Ignored `out/` contains private generated source, diagnostics, replay material and local previews. Ignored `toolchain/` contains the checksum-verified compiler cache. Neither ignored directory should become a public Actions artifact.

A successful preview's `public/` contains four player ZIPs, each containing one unchanged EXE. Private encrypted answers and fingerprint history are kept in the sibling `state/` directory. Only the latest round is retained in the rolling `daily-crackme` prerelease. The next successful publication discloses the actual saved answers from the previous round.

The workflow is in `../.github/workflows/daily-crackme.yml`. Its manual mode defaults to read-only preview. Production publishing additionally requires `--publish`, a backed-up master secret and the explicit approval variable. See the activation guide before changing these gates.
