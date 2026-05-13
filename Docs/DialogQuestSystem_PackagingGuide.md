# Packaging

Package the plugin when distributing it to Blueprint-only projects.

```powershell
.\Scripts\Package-DialogQuestSystem.ps1 -EngineRoot "C:\Program Files\Epic Games\UE_5.6" -TargetPlatforms Win64 -Zip
```

The packaged output should include `Binaries`, `Content`, `Source`, and `DialogQuestSystem.uplugin`.

Before packaging:

1. Close Unreal Editor.
2. Build or package with the target Unreal Engine version.
3. Open important dialogue/quest assets, validate them, and save them.
4. Smoke test in a clean Blueprint-only project.
5. Test PIE, Standalone Game, and packaged build for one dialogue event, one voice line, one quest start, and one objective advance.

Do not ship source-development folders as the no-compile student install.
