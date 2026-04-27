# Runtime UI

## Shipped widgets

The plugin ships with editable Widget Blueprints and C++ base classes.

Recommended editable defaults:

- `/DialogQuestSystem/UI/WBP_DQSDialogue_Default`
- `/DialogQuestSystem/UI/WBP_DQSQuestTracker_Default`
- `/DialogQuestSystem/UI/WBP_DQSQuestLog_Default`

C++ fallback/base classes:

- `DQS Default Dialogue Widget`
- `DQS Default Quest Tracker Widget`
- `DQS Default Quest Log Widget`

## Default dialogue widget

The default dialogue widget is designed to be immediately usable:

- full-screen backdrop
- centered bottom dialogue card
- scrolling dialogue body region
- scrolling response list
- built-in continue and choice handling

It is intended as a polished default, not a mandatory visual style. For custom looks, duplicate the Widget Blueprint into project content and edit the widget tree, fonts, images, padding, and animations there.

## Tracker and quest log

The tracker is compact and always-on. The quest log is larger and intended for a toggle key or menu action.

The tracker focuses on:

- latest quest update
- active quests list

The quest log focuses on:

- full quest list
- status badges
- current objective description/progress

## Recommended setup

1. Create and add the dialogue widget on player controller begin play.
2. Create and add the tracker widget on player controller begin play.
3. Create the quest log widget and store the reference.
4. Bind `J` or `L` to `Toggle Quest Log`.

## Replacing the visuals

The runtime logic is not tied to the shipped look. If you replace the widgets:

- keep the same subsystem/view-model flow
- preserve continue/choice button wiring
- keep long text and large choice counts scroll-safe
- inherit from the matching base class if you want helper functions such as `GetDialogueBodyText`, `GetChoiceCount`, `ChooseDialogueOption`, `Open Quest Log`, and `Toggle Quest Log`
