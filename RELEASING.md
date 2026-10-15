# Releasing

Draft from git history. Confirm each step before relying on it.

## Before tagging

1. `mise run build` passes on Windows and Linux.
2. Play-test a release candidate with a real game, in both gamepad and keyboard/mouse modes.
3. Bump `APP_VERSION` in `CMakeLists.txt`.
4. Update the `VGP_Data_Exchange` submodule only if the protocol changed.
5. Regenerate docs if the API changed: `mise run docs` (CI also does this).

## Package

```
mise run install
```

Output goes to `./dist` (`dist/bin/` on Windows).

## Tag

`git tag -a vX.Y.Z -m "Release vX.Y.Z"` and `git push origin vX.Y.Z`.

## Publish binaries

TODO: the `VGP-builds` repo has a manual workflow ("VGamepad PC produce binaries") that builds from a branch, tag or commit. Document how its artifacts are published, and any Windows installer signing steps.
