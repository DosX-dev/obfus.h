# Activation, state and recovery

Publication remains disabled until owner approval and hosted-runner verification. Preview and tests do not authorize release updates.

## Automatic latest compiler

Each preparation resolves the latest stable published release of `DosX-dev/TinyCC-builder` once. It selects exactly one ordinary `tinycc-windows-YYYY-MM-DD.zip`; UPX archives, drafts, prereleases and ambiguous/missing assets are rejected. The GitHub asset SHA-256 digest is mandatory. Downloads are bounded, checked for exact size and digest, and extracted anew from the verified archive. A cached ZIP can save a download but cannot select an older release or bypass verification.

No retained compiler release or `CRACKME_TOOLCHAIN_URL` variable is required. The actual release, asset identity, archive hash, download URL and both compiler version responses are recorded in the round manifest. Local `--toolchain` directories remain available for offline preview/acceptance, but are forbidden for daily publication. A new compiler is accepted only if the normal x86/x64 model/native checks, GUI checks and player audits pass for the entire round. Download, metadata, compiler or verification failures abort preparation before new publication; there is no fallback to an older compiler or weaker protection.

The rolling `daily-crackme` release is separate from compiler storage. Enabling publication requires both the workflow approval variable and the CLI publication switch; these barriers are deliberately still closed.

After the owner authorizes pushing the workflow, run its manual **preview** mode first. Preview has read-only repository permissions, uses an ephemeral test key, exercises real builds/GUI on the hosted Windows runner, and uploads no artifacts. The local GUI/compiler tests do not establish that a hosted runner has the same desktop environment. A green hosted preview is required before setting `CRACKME_PUBLICATION_APPROVED=v1`, adding the master secret and enabling production publishing. Manual runs default to preview; publishing requires an explicit mode selection.

JavaScript formatting follows VS Code's built-in `vscode.typescript-language-features` formatter. C formatting follows the repository's `.clang-format` configuration.

## Operational recovery

Transient GitHub reads are retried up to three times with bounded backoff. Rate-limit retry delays above one minute abort the run so a backup schedule can recover later. Mutations are not blindly retried: the publication journal and committed manifest resolve lost responses. Interrupted first-round uploads also recover without a committed manifest.

Saved state and journals are validated before release updates or deletion. Missing published releases, missing committed state and rollback journals fail closed instead of resetting numbering/history or overwriting previous challenges. External deletion of durable state requires owner recovery; it cannot safely be treated as a fresh installation. Child processes terminated without an exit code, exceeding time limits or output bounds fail verification.

Generation and verification failures retain private diagnostics and allow all three deterministic attempts. A failed local preview retains its replay key under its ignored output directory, allowing owner reproduction; daily mode never writes the master key to disk. JSON object-key ordering does not change the identity of a committed state.

One-time activation: allow the workflow to update repository contents/releases (including any applicable branch rules), configure and back up `CRACKME_MASTER_KEY`, run a green hosted preview, and then set `CRACKME_PUBLICATION_APPROVED=v1`. Keep the release mutable. After setup, ordinary daily operation and interruption recovery need no manual password, seed or release edits. Until owner approval, all of this remains local and publication stays disabled.
