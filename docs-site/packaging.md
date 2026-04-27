# Packaging

## Source plugin structure

The plugin is split into:

- `DQSRuntime`
- `DQSUI`
- `DQSEditor`

Runtime builds only need `DQSRuntime` and `DQSUI`. Editor-only features stay in `DQSEditor`.

## Package for reuse

Use the packaging script from the project root:

```powershell
.\Scripts\Package-DialogQuestSystem.ps1 -EngineRoot "C:\Program Files\Epic Games\UE_5.6"
```

This runs Unreal Automation Tool `BuildPlugin` and writes a packaged plugin build under `Build/DialogQuestSystem` by default.

## Distribution checklist

- build the plugin from a clean editor-closed state
- verify the packaged output loads in a separate UE5 project
- verify sample assets appear under plugin content
- verify editor module does not load in packaged runtime builds
- verify the docs site matches the shipped UX and screenshots

## Compatibility policy

The source layout is prepared for broad UE5 use, but every engine minor version should be smoke-tested before release.

Recommended minimum checks:

- sample dialogue opens and validates
- sample quest opens and validates
- default widgets render correctly
- plugin packages with `BuildPlugin`
