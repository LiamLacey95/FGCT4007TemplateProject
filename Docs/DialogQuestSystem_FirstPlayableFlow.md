# DialogQuestSystem First Playable Flow

This guide gets the plugin from "compiled" to a working NPC conversation that starts a quest and updates a quest tracker in the current Third Person project.

This guide is written against the plugin as it exists in this project right now. It includes the current graph-editor workflow and the current caveats, so follow it literally rather than assuming stock Unreal graph behavior.

## 1. Restart the Editor
- Close the editor if it was open during plugin compilation.
- Reopen [DialogQuestSystem.uproject](/F:/Ue5/Projects/DialogQuestSystem/DialogQuestSystem/DialogQuestSystem.uproject).
- When the project opens, confirm the `DialogQuestSystem` plugin is enabled.

## 2. Create Base Content Folders
Create these folders in the Content Browser:

- `Content/DQS/Dialogue`
- `Content/DQS/Quests`
- `Content/DQS/UI`
- `Content/DQS/NPC`

## 3. Create the Quest Asset
Create a `Quest Graph` asset named `Quest_FirstFavor` in `Content/DQS/Quests`.

Important:

- Do not create a Blueprint class based on `QuestGraphAsset`.
- If you see `EventGraph`, `Functions`, `Macros`, or `Parent class: QuestGraphAsset`, you created the wrong thing.
- The correct asset is a custom content asset named `Quest Graph`.
- The correct editor opens a graph-focused asset window, not the standard Blueprint class editor.

How to add nodes in the quest graph editor:

1. Open the `Quest Graph` asset.
2. Right-click empty space in the graph canvas.
3. Search for the node type name.
4. Click the node type you want to add.

For this first quest, add exactly these nodes:

- `Start`
- `Objective`
- `Complete`

Important:

- A fresh quest graph may already contain a default `Start` node.
- If you already have a `Start` node, do not add a second one.
- The `Start` node is intended to stay in the graph and should be treated as the entry point.
- After the default `Start` exists, the node creation menu should no longer offer another `Start`.

Build this minimal quest:

1. Leave the default `Start` node in place.
2. Right-click and add one `Objective` node.
3. Right-click and add one `Complete` node.
4. Connect the `Next` output of `Start` to the `In` input of `Objective`.
5. Connect the `Next` output of `Objective` to the `In` input of `Complete`.
6. Click the `Objective` node and, in the Details panel, set:
   - Set `Title` to `Bring Back the Apple`
   - Set `Description` to `Find an apple and return to the villager.`
   - Set `Objective Tag` to `Quest.FirstFavor.ReturnApple`
   - Set `Target Count` to `1`
7. Click the asset background and, in the asset Details panel, set:
   - `Metadata > Display Name = First Favor`
8. Save the asset.

How this quest works at runtime:

- `Start` is the entry point used by `StartQuest`.
- The system immediately moves from `Start` to the next connected node.
- `Objective` pauses quest progression until its objective count reaches `Target Count`.
- `Complete` marks the quest completed and emits a quest update.

## 4. Create the Dialogue Asset
Create a `Dialogue Graph` asset named `Dialogue_VillagerIntro` in `Content/DQS/Dialogue`.

Important:

- Do not create a Blueprint class based on `DialogueGraphAsset`.
- If Unreal opens the standard Blueprint editor with `EventGraph`, delete that asset and create a `Dialogue Graph` asset instead.

How to add nodes in the dialogue graph editor:

1. Open the `Dialogue Graph` asset.
2. Right-click empty space in the graph canvas.
3. Search for the node type name.
4. Click the node type you want to add.

For this first conversation, you need:

- `Entry`
- `Speech`
- `Choice`
- `Quest Hook`
- `Speech`
- `End`

Important current workflow note for `Choice` nodes:

- Each item in the `Choices` array corresponds to one output pin.
- If `Choices` has 2 elements, the node should expose 2 outputs.
- `Node Type` is not part of the normal authoring workflow. Create the correct node type from the graph context menu instead.
- You can also right-click a `Choice` node and use `Add Choice Output` or `Remove Last Choice Output`.

Build this minimal conversation:

1. Leave the default `Entry` node in place.
2. Right-click and add one `Speech` node. Rename its title to `Greeting`.
3. Right-click and add one fresh `Choice` node. Rename its title to `Offer Choices`.
4. Right-click and add one `Quest Hook` node. Rename its title to `Accept Quest`.
5. Right-click and add one `Speech` node. Rename its title to `Quest Accepted`.
6. Right-click and add one `End` node.
7. Connect:
   - `Entry.Next -> Greeting.In`
   - `Greeting.Next -> Offer Choices.In`
8. Click the graph background and, in the asset Details panel, add one speaker in `Speakers`:
    - `Speaker Id`: `Villager`
    - `Display Name`: `Village Elder`
9. Click the `Greeting` node and set:
    - `Speaker Id`: `Villager`
    - `Text`: `Traveler, could you help me with a small problem?`
10. Click the `Offer Choices` node and set:
    - `Text = Will you help the villager?`
    - In `Choices`, make sure there are exactly `2` array elements
    - On `Index [0]`, set `Text = Sure, what do you need?`
    - On `Index [1]`, set `Text = Not right now.`
    - Alternative workflow: right-click the `Offer Choices` node and use `Add Choice Output` until the node has the output count you want, then fill the matching `Choices` entries in the Details panel
11. The `Offer Choices` node should now show two output pins, one for each choice.
12. Connect:
    - first choice output -> `Accept Quest.In`
    - second choice output -> `End.In`
13. Click the `Accept Quest` node and set:
    - `Quest Hook > Operation`: `StartQuest`
    - `Quest Hook > Quest Asset`: `Quest_FirstFavor`
14. Connect `Accept Quest.Next -> Quest Accepted.In`
15. Click the `Quest Accepted` node and set:
    - `Speaker Id`: `Villager`
    - `Text`: `Please bring me an apple from the nearby fields.`
16. Connect `Quest Accepted.Next -> End.In`
17. Click the graph background and, in the asset Details panel, set:
    - `Metadata > Display Name = Villager Intro`
18. Save the asset.

How this dialogue works at runtime:

- `Entry` is the starting node used by `StartDialogue`.
- `Speech` presents a line and then waits for the player to continue.
- `Choice` presents the player response options and waits for a selection.
- `Quest Hook` changes quest state when reached. In this case it starts the quest.
- `End` ends the active conversation.

## 5. Create the NPC Blueprint
Create a new Actor Blueprint named `BP_VillagerNPC` in `Content/DQS/NPC`.

Set it up like this:

1. Add a visible mesh component for the NPC.
2. Add a `DQS Dialogue Participant Component`.
3. On the component, assign `Dialogue Asset = Dialogue_VillagerIntro`.
4. Add a `Box Collision` or `Sphere Collision` for interaction range.

In the Event Graph:

1. Add a custom event `StartConversation`.
2. From the `DQS Dialogue Participant Component`, call `Start Dialogue`.
3. Pass `Self` as the `World Context Object`.
4. Leave `Override Context` empty unless you want a different context object.

For a quick prototype, you can trigger `StartConversation` from overlap or a key press while the player is nearby.

## 6. Use the Shipped Default Dialogue UI
You no longer need to create a dialogue widget asset from scratch just to get a working UI.

The plugin now ships a generated widget Blueprint:

- `/DialogQuestSystem/UI/WBP_DQSDialogue_Default`

This widget is backed by the C++ base:

- [DQSDefaultDialogueWidget.h](/F:/Ue5/Projects/DialogQuestSystem/DialogQuestSystem/Plugins/DialogQuestSystem/Source/DQSUI/Public/DQSDefaultDialogueWidget.h)

What it includes:

- horizontal conversation strip
- portrait panel
- speaker label
- scrollable body text
- response list that collapses entirely when there are no choices
- continue area that appears only for continue-only dialogue
- built-in button wiring to `ContinueDialogue` and `ChooseDialogueOption`

Use this widget Blueprint directly, or duplicate it into project content before restyling.

## 7. Use the Shipped Default Quest Tracker UI
The plugin also ships a generated tracker widget Blueprint:

- `/DialogQuestSystem/UI/WBP_DQSQuestTracker_Default`

This widget is implemented in:

- [DQSDefaultQuestTrackerWidget.h](/F:/Ue5/Projects/DialogQuestSystem/DialogQuestSystem/Plugins/DialogQuestSystem/Source/DQSUI/Public/DQSDefaultQuestTrackerWidget.h)

What it includes:

- top-right quest tracker panel
- latest quest update title/message
- active quest list

## 8. Optional: Use the Shipped Quest Log UI
The plugin also ships a generated quest log widget Blueprint:

- `/DialogQuestSystem/UI/WBP_DQSQuestLog_Default`

This widget includes:

- large right-side quest journal panel
- full quest list
- status pills for active/completed/failed quests
- current objective text where available

Recommended use:

1. Create the widget on `BeginPlay`.
2. Add it to the viewport.
3. Keep it closed by default. This is expected behavior.
4. Bind an input such as `J` or `L` in your player controller and call `Toggle Quest Log` on the stored widget reference.

## 9. Add the Widgets to the Player
Open `BP_ThirdPersonPlayerController` and on `BeginPlay`:

1. Create Widget with class `/DialogQuestSystem/UI/WBP_DQSDialogue_Default`, store it, and add it to viewport.
2. Create Widget with class `/DialogQuestSystem/UI/WBP_DQSQuestTracker_Default`, store it, and add it to viewport.
3. Optionally create Widget with class `/DialogQuestSystem/UI/WBP_DQSQuestLog_Default`, store it, and add it to viewport.

## 10. Place the NPC in the Level
- Open `Lvl_ThirdPerson`.
- Place `BP_VillagerNPC` in front of the player start.
- Add a temporary interaction trigger:
  - easiest: overlap starts conversation
  - better: bind an input action and call `StartConversation` when the player is inside the trigger volume

## 11. Test the Happy Path
Expected result:

1. Player approaches NPC.
2. Dialogue opens.
3. Player picks `Sure, what do you need?`
4. `Quest_FirstFavor` starts.
5. Quest tracker shows the objective message.

To simulate completion in Blueprint:

1. Get the `Dialog Quest Subsystem`.
2. Call `Advance Quest Objective`.
3. `Quest Id`: the `Graph Id` from `Quest_FirstFavor`
4. `Objective Tag`: `Quest.FirstFavor.ReturnApple`
5. `Delta`: `1`

The quest should then advance to `Complete`.

How to find the `Graph Id`:

1. Open `Quest_FirstFavor`.
2. Click the graph background so the asset itself is selected.
3. In the Details panel, open `Metadata`.
4. Read the `Graph Id` field.

Current graph editor tips:

- Use right-click in empty canvas space to add nodes.
- Connect by dragging from an output pin to an input pin.
- You can also drag out from an existing pin, release in empty space, and pick a node type from the menu. The new node should auto-connect.
- Freshly created nodes should show their pins immediately. If a node appears as title-only with no input/output pins, that is editor-bug behavior rather than intended authoring behavior.
- Keep exactly one `Entry` node in dialogue graphs.
- Keep exactly one `Start` node in quest graphs.
- Do not create Blueprint subclasses of `DialogueGraphAsset` or `QuestGraphAsset` for content authoring.
- Use `Delete` to remove selected nodes.
- Use `Ctrl+C`, `Ctrl+V`, and `Ctrl+X` for node editing.
- `Node Type` is not the normal way to change a graph node. Create the correct node type directly from the context menu.
- Internal GUID and link-target fields are plugin-managed and should normally be ignored.

Dialogue input behavior:

- Starting dialogue now enables the mouse cursor automatically.
- Starting dialogue now sets the local player controller to dialogue-friendly input mode.
- Starting dialogue now stops player move/look input.
- Ending dialogue restores game input and cursor state.

## Recommended Next Blueprints
- `BP_DQSInteractionPrompt`: world-space prompt for `Press E`
- `BP_DQSQuestGiverBase`: NPC parent blueprint with the dialogue participant component already configured
- `BP_DQSApplePickup`: overlap actor that calls `Advance Quest Objective`
- optional later: duplicate the shipped widget Blueprints into project content if you want to reskin them instead of starting from a blank widget

## Notes
- The plugin ships default widget Blueprints and a default UI theme asset under plugin content.
- If the custom `Dialogue Graph` or `Quest Graph` asset types do not appear in the Add Asset menu immediately, restart the editor once after the first successful compile.
