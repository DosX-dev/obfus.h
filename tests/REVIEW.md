# Regression and protection review

The header preserves automatic operator and CRT interception; virtualization remains optional. Verification covers behavior and selected protection mechanisms on TCC x86 and x64.

## Preserved mechanisms

- Active VM macros dispatch encoded operations to the interpreter. Floating arithmetic operands retain their double conversions; the VM identity operation preserves long-double input precision. Per-call volatile scratch storage supports recursive and concurrent execution.
- VM branch macros evaluate scalar conditions once, then dispatch an encoded branch command. A runtime nonce and call-site value select three local decision programs; an encoded instruction pointer leads to a masked token that is verified through the VM. Nested and concurrent chains share no branch state.
- Conditions retain the float argument and conversion back to the proxy result. Truth normalization handles fractional values and pointers before entering that chain.
- Timestamp checks still invoke the integer proxy. The second timestamp instruction is actual RDTSCP, with a cached CPU capability check and RDTSC fallback.
- Loader names use volatile characters, arithmetic, generated masks and integer proxies. The false switch branch, junk calls and working-set option remain. The loader address cache is initialized atomically.
- CRT calls traverse the LoadLibrary proxy chain and custom PE export resolver. Loader references are balanced; CRT dispatch does not introduce standard-function address fallbacks.
- printf retains its opaque loop and junk calls, uses console proxies for ordinary console output and dynamic CRT dispatch for redirected output or count conversions. Formats containing a count conversion execute once, preserving their writes to user memory. The loop arithmetic uses sufficient unsigned width to avoid overflow changing its continuation condition.
- HIDE_STRING retains both timestamp branches. CRT module-name hiding is copied while its source buffer is still alive. Character masks use caller-owned bounded storage.
- Anti-debug arithmetic and its failure response remain. Worker setup retains failure cleanup and the correct Windows callback ABI.
- Custom abs retains its FALSE expression and evaluates the user argument once. Its comparison uses signed operands.

## Correctness fixes

Pointer-width proxies and typed address recovery avoid truncation and out-of-object pointer arithmetic. Assembly declares register and flag effects. Export parsing checks ranges and handles ordinals and forwarders. CRT prototypes and math output-pointer arguments match their APIs.

The joke export retains HIDE_STRING("Moo") as requested. Its returned buffer still has block lifetime; it is not a persistent-string API.

## Verification

The JavaScript runner builds and executes both architectures and all configurations, including multithreaded stress, DLLs, streams, console output and isolated invalid-memory cases. It checks VM dispatch at runtime with a bypass mutation, selected source contracts, hidden strings, timestamp instructions and selected absent native imports in generated binaries. These checks verify specified mechanisms; they do not compare every instruction with the original binary.
