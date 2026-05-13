# Installation

## Blueprint-Only Projects

Use a packaged release of the plugin. Do not copy the source-development plugin from this repository into a Blueprint-only project unless you expect Unreal to compile C++.

Expected layout:

```text
YourProject/
  Plugins/
    DialogQuestSystem/
      Binaries/
      Content/
      Resources/
      Source/
      DialogQuestSystem.uplugin
```

The packaged plugin is engine-version and platform specific. A UE 5.6 Win64 package should be used with UE 5.6 on Win64.

## Source Development

Clone this repository only when editing or rebuilding the plugin. You need a C++ toolchain compatible with your Unreal version.

After changing code, rebuild/package with:

```powershell
.\Scripts\Package-DialogQuestSystem.ps1 -EngineRoot "C:\Program Files\Epic Games\UE_5.6" -TargetPlatforms Win64 -Zip
```

## First Project Setup

1. Enable the plugin.
2. Enable `Show Plugin Content` in the Content Browser.
3. Add the default dialogue widget `/DialogQuestSystem/UI/WBP_DQSDialogue_Default` to the viewport, or use your own widget derived from `DQSDialogueWidgetBase`.
4. Add `DQS Dialogue Participant Component` to an NPC and assign a dialogue asset.
5. Start dialogue through the component or `Get Dialog Quest Subsystem`.
