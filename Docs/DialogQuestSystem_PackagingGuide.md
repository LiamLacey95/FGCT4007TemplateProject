# DialogQuestSystem Packaging Guide

## Goal

Package DialogQuestSystem as a precompiled plugin so Blueprint-only users can install it without compiling C++.

Unreal plugin binaries are engine-version and platform specific. A plugin packaged for UE 5.6 Win64 should be distributed to UE 5.6 Win64 users. Package again for every Unreal version and platform you want to support.

## Build A Distributable Plugin

Close Unreal Editor, then run:

```powershell
.\Scripts\Package-DialogQuestSystem.ps1 -EngineRoot "C:\Program Files\Epic Games\UE_5.6" -TargetPlatforms Win64 -Zip
```

By default the packaged folder is written to:

```text
Build/DialogQuestSystem
```

With `-Zip`, the script also creates a zip beside the packaged folder.

## What To Send Users

Send the packaged `DialogQuestSystem` folder from `Build/DialogQuestSystem` or the generated zip from `Build`, not the development copy from this project's `Plugins` folder.

The user's target project should end up with:

```text
TheirProject/
  Plugins/
    DialogQuestSystem/
      Binaries/
      Content/
      Resources/
      Source/
      DialogQuestSystem.uplugin
```

The package script marks the packaged `.uplugin` as `Installed: true`, which tells Unreal this is an installed/distributed plugin rather than a local development plugin.

## Why Another Computer Compiles From Source

That happens when the other computer receives the source plugin rather than a packaged binary plugin, or when the packaged binaries do not match that machine's Unreal version/platform.

Common causes:

- copying `Plugins/DialogQuestSystem` directly from this project
- missing `Binaries/Win64/*.dll`
- using a different Unreal version, for example packaging in UE 5.6 and opening in UE 5.5
- distributing only source files from GitHub
- deleting packaged binaries before zipping

## Before Packaging

- close Unreal Editor
- build `DialogQuestSystemEditor Win64 Development` successfully
- make sure sample/plugin content is saved
- keep editor-only code inside `DQSEditor`
- decide which UE version this release supports

## Smoke Test

Use a clean Blueprint-only UE project:

1. Create `Plugins` if it does not exist.
2. Copy the packaged `DialogQuestSystem` folder into `Plugins`.
3. Open the host project with the same Unreal version used to package.
4. Confirm Unreal does not ask to compile modules.
5. Enable the plugin if needed.
6. Create one dialogue graph and one quest graph.
7. Add the default UI widgets or project custom widgets.
8. Start a dialogue and accept a quest.

If Unreal asks to compile, treat the package as invalid for that target machine/version and rebuild with the matching engine.
