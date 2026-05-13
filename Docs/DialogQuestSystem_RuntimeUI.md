# Runtime UI

The plugin ships default widgets:

- `/DialogQuestSystem/UI/WBP_DQSDialogue_Default`
- `/DialogQuestSystem/UI/WBP_DQSQuestTracker_Default`
- `/DialogQuestSystem/UI/WBP_DQSQuestLog_Default`
- `/DialogQuestSystem/UI/Themes/DA_DQS_DefaultUITheme`

The safest customization path is to duplicate the widget Blueprints into project content and edit the duplicates.

## Dialogue UI

Dialogue widgets should inherit from `DQSDialogueWidgetBase`. Useful helpers include:

- `ContinueDialogue`
- `ChooseDialogueOption`
- `GetSpeakerNameText`
- `GetDialogueBodyText`
- `GetChoiceCount`
- `GetChoiceText`
- `IsChoiceEnabled`
- `GetSpeakerPortrait`
- `GetSpeakerStyle`
- `GetVoiceSound`

`VoiceSound` is also played by the runtime when the line is presented.

## Quest UI

Quest tracker and log widgets use `DQSQuestViewModel`, which listens to subsystem quest updates. Add tracker/log widgets to the viewport from your player controller, HUD, or UI manager.
