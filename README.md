# modorganizer-bsa_extractor

This change adds generic game-feature archive dispatch to `bsa_extractor`.

## Maintainer Notes

- New feature shape: `MOBase::GameArchiveHandler`
  - `supportedArchiveNameFilters()`
  - `supportsArchive(path)`
  - `extractArchive(path, outputDirectory, progress, errorMessage)`
  - optional `canCreateArchive(path)` / `createArchive(...)`
- Dispatch behavior:
  - ask the active game for `GameArchiveHandler`
  - delegate when the feature exists and claims the archive
  - otherwise use the existing bsatk `.bsa` / `.ba2` fallback unchanged
- Availability:
  - the plugin remains visible for now
  - unsupported games still behave gracefully because the plugin only acts when it
    finds candidate archives, and non-delegated `.bsa` / `.ba2` files still follow the
    existing bsatk path
- Temporary shim:
  - this repo carries only the new local header
    `src/uibase/game_features/gamearchivehandler.h`
  - the previous unsafe protected-member access hack was removed
  - while that shim exists, extractor-side feature lookup is intentionally disabled
    rather than using unsafe access to `IGameFeatures` internals
  - upstream `uibase` support is still required before delegated archive handling is
    fully active in this repo

Companion game-plugin work is still required for any non-bsatk format: the game plugin
must register `GameArchiveHandler` through MO2 game features.

This extractor branch should land with, or after, the matching `uibase` PR that adds
the canonical `MOBase::GameArchiveHandler` declaration and feature lookup support.
