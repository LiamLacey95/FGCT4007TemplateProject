# DialogQuestSystem UI Customization

## Shipped UI Assets
The plugin ships default UI assets under plugin content:

- `/DialogQuestSystem/UI/WBP_DQSDialogue_Default`
- `/DialogQuestSystem/UI/WBP_DQSQuestTracker_Default`
- `/DialogQuestSystem/UI/WBP_DQSQuestLog_Default`
- `/DialogQuestSystem/UI/Themes/DA_DQS_DefaultUITheme`

These are the recommended starting point for real projects. They are Widget Blueprints, not just C++ classes, so you can duplicate them into project content and edit the widget tree directly in UMG.

## Recommended Workflow
1. Open plugin content in the Content Browser.
2. Duplicate the widget you want into your project, for example:
   - `Content/DQS/UI/WBP_MyDialogue`
   - `Content/DQS/UI/WBP_MyQuestTracker`
   - `Content/DQS/UI/WBP_MyQuestLog`
3. Duplicate the default theme asset if you want a project-specific theme.
4. Point your controller/game setup at your duplicated widget class instead of the plugin default.

## What You Can Edit In A Child Or Duplicate
Because the shipped defaults are Widget Blueprints with named elements, you can change:

- images and portrait frames
- text font, size, color, and wrapping
- panel width and height
- padding, spacing, margins, and alignment
- background/frame brushes
- added animations and transitions
- extra decorative elements and overlays

If you duplicate the widget Blueprint, you can freely reorganize the layout as long as you keep the expected named binding widgets that the C++ base class looks for.

## Named Dialogue Elements
The default dialogue widget expects these named widgets when present:

- `BackdropBorder`
- `DialogueWidthBox`
- `DialogueFrame`
- `DialogueSurface`
- `PortraitSizeBox`
- `PortraitFrame`
- `PortraitImage`
- `CaptionTextBlock`
- `SpeakerTextBlock`
- `BodyScrollBox`
- `BodyTextBlock`
- `ResponsesPanel`
- `ResponsesLabelText`
- `ResponsesScrollBox`
- `ResponsesList`
- `ContinueArea`
- `HintTextBlock`
- `ContinueButton`
- `ContinueButtonText`

## Named Quest Elements
Quest tracker:

- `TrackerPanel`
- `HeaderText`
- `LatestTitleText`
- `LatestMessageText`
- `TrackedLabelText`
- `ActiveQuestList`

Quest log:

- `QuestLogChrome`
- `HeaderText`
- `SubheaderText`
- `CloseButton`
- `QuestListBox`
- `SelectedQuestTitleText`
- `SelectedQuestStatusText`
- `SelectedQuestDescriptionText`
- `SelectedObjectivesBox`
- `EmptyStateText`

## Theme And Speaker Styling
The plugin supports:

- a global UI theme asset: `UDQSUIThemeAsset`
- per-speaker style assets: `UDQSSpeakerStyleAsset`
- per-node presentation overrides on dialogue nodes

Resolution order:

1. dialogue node presentation override
2. speaker style asset
3. global UI theme

Use speaker style assets for reusable portraits and speaker-specific accent styling. Use node presentation overrides when one specific line should swap portrait or hide the portrait.

## Quest Log Behavior
The quest log is intentionally hidden until toggled.

Expected setup:

1. Create the quest log widget.
2. Add it to the viewport.
3. Store the reference.
4. Call `Toggle Quest Log`, `Open Quest Log`, or `Close Quest Log` from input.

If it is added to the viewport and not visible yet, that is correct behavior until opened.
