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
- Progress callback:
  - delegated extraction now follows the canonical `uibase` progress callback
    `std::function<void(qint64 current, qint64 total)>`
  - the delegated progress dialog therefore shows archive-level progress instead of
    per-file names; bsatk extraction keeps its existing filename detail

Companion game-plugin work is still required for any non-bsatk format: the game plugin
must register `GameArchiveHandler` through MO2 game features.

This branch now expects the canonical `uibase` `GameArchiveHandler` API to be available.
