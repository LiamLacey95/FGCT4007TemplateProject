# Packaging

Package with:

```powershell
.\Scripts\Package-DialogQuestSystem.ps1 -EngineRoot "C:\Program Files\Epic Games\UE_5.6" -TargetPlatforms Win64 -Zip
```

Before shipping, validate and save graph assets, then smoke test PIE, Standalone Game, and packaged builds.

Distribute the packaged plugin folder, not the source-development plugin.
