# Install

## Project layout

Place the plugin at:

`<YourProject>/Plugins/DialogQuestSystem`

The host project can remain Blueprint-only. The plugin itself contains the C++ runtime, UI, and editor code.

## First load

1. Open the project in Unreal Editor.
2. Let Unreal compile the plugin if binaries are missing.
3. Restart the editor if Unreal prompts for it.
4. Confirm the plugin is enabled in `Edit > Plugins`.

## Sample content

The plugin ships sample graph assets under plugin content:

- `/DialogQuestSystem/Samples/Dialogue/DQS_Sample_QuestOffer`
- `/DialogQuestSystem/Samples/Quests/DQS_Sample_FirstFavor`

These are safe starting references for documentation, onboarding, and QA.

## First project test

1. Add `/DialogQuestSystem/UI/WBP_DQSDialogue_Default` to the viewport on player controller begin play.
2. Add `/DialogQuestSystem/UI/WBP_DQSQuestTracker_Default` to the viewport on begin play.
3. Add `/DialogQuestSystem/UI/WBP_DQSQuestLog_Default`, store the reference, and bind a toggle key.
4. Put a `DQS Dialogue Participant Component` on an NPC and assign a dialogue asset.
5. Start dialogue from overlap or interact input.
