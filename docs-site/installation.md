# Installation

For Blueprint-only projects, install a packaged plugin build that matches the Unreal Engine version and platform.

```text
YourProject/
  Plugins/
    DialogQuestSystem/
```

Do not copy the source-development plugin into a Blueprint-only project unless you expect Unreal to compile C++.

After enabling the plugin, turn on `Show Plugin Content`, add the default dialogue widget to the viewport, and start dialogue through `DQS Dialogue Participant Component` or `Get Dialog Quest Subsystem`.
