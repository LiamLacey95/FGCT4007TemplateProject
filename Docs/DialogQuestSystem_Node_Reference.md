# DialogQuestSystem Node Reference

This page documents the current authoring model for the custom `Dialogue Graph` and `Quest Graph` editors.

Important:

- Internal GUID fields and target ID fields are runtime/compiler data. They are not normal authoring fields.
- Create nodes from the graph context menu. Do not use `Node Type` as a normal conversion workflow.
- For new dialogue content, use `Conditioned Branch` for checks and `Set Condition` for state changes/actions.
- Legacy per-choice conditions/actions can still exist on older assets, but they are not the recommended workflow.

## Dialogue Graph Nodes

### Entry

Purpose:

- Starting point for a dialogue graph.
- `StartDialogue` enters the dialogue through this node.

Pins:

- Input: `In`
- Output: `Next`

Behavior:

- Immediately forwards execution to the connected `Next` node.
- A dialogue graph should have exactly one `Entry` node.

Common mistake:

- Leaving `Entry` unconnected. If this happens, `StartDialogue` can fail or immediately end.

### Speech

Purpose:

- Shows one line of dialogue to the player.

Pins:

- Input: `In`
- Output: `Next`

Behavior:

- Presents the line and waits for the player to continue.
- After continue, execution follows `Next`.

Settings:

- `Title`: editor label for the node.
- `Speaker Id`: matches a speaker configured on the dialogue asset.
- `Text`: player-facing line text.
- `Presentation Tags`: optional tags for UI or presentation logic.
- `Voice Sound`: optional voice/audio asset.
- `Presentation`: optional portrait/style override. Node overrides win over speaker style, which wins over global UI theme.

### Choice

Purpose:

- Presents player response options.

Pins:

- Input: `In`
- Output: one output per choice entry.

Behavior:

- The runtime displays all connected choices.
- Selecting a choice moves to that choice's connected output target.
- If a response should set state, connect that response to a `Set Condition` node.

Settings:

- `Title`: editor label for the node.
- `Speaker Id`: optional speaker context for the prompt.
- `Text`: optional prompt text for the choice point.
- `Choices`: player-facing response entries. Use the `Add Choice Output` / `Remove Last` buttons in the Choice node Details panel to change branch count; those buttons keep the array entries and graph output pins in sync.

Choice entry settings:

- `Text`: the response text shown to the player.
- `Failure Reason`: legacy/advanced field. In normal authoring, use `Conditioned Branch` before a choice when you need alternate/locked paths.

Node context actions:

- `Add Choice Output`: adds another response entry and output pin. Prefer this over pressing the raw array `+` button.
- `Remove Last Choice Output`: removes the last response entry and output pin.

### Conditioned Branch

Purpose:

- Routes dialogue based on conditions.
- Use this for "second time speaking", "already spoke to another NPC", "quest is active", "has item", or any state-based dialogue variation.

Pins:

- Input: `In`
- Output: `Next Node`
- Output: `Alternative Node (Failed Condition)`

Behavior:

- If all conditions pass, execution follows `Next Node`.
- If any condition fails, execution follows `Alternative Node (Failed Condition)`.

Settings:

- `Title`: editor label for the node.
- `Conditions`: condition objects to evaluate.

Examples:

- Check tag `Dialogue.NPC.Elder.SpokenOnce`.
- Check tag `Dialogue.NPC.Blacksmith.Spoken`.
- Check whether a quest is `Active`.
- Check whether a variable has a required value.

### Set Condition

Purpose:

- Runs one or more actions, then continues.
- Use this to set variables/tags or trigger gameplay events from dialogue.

Pins:

- Input: `In`
- Output: `Next`

Behavior:

- Executes configured actions and follows `Next`.

Settings:

- `Title`: editor label for the node.
- `Actions`: side effects triggered when the node runs.

Examples:

- Add tag `Dialogue.NPC.Elder.SpokenOnce`.
- Set bool variable `AcceptedFirstFavor = true`.
- Trigger event `Dialogue.Event.OpenVillageGate`.

### Quest Hook

Purpose:

- Changes quest state from dialogue.

Pins:

- Input: `In`
- Output: `Next`

Behavior:

- Performs a quest operation, then continues.

Settings:

- `Quest Hook > Operation`: `None`, `StartQuest`, `AdvanceObjective`, `CompleteQuest`, or `FailQuest`.
- `Quest Hook > Quest Asset`: target quest asset.
- `Quest Hook > Objective Tag`: objective tag used when advancing an objective.

### Jump

Purpose:

- Redirects execution to another connected point in the dialogue graph.

Pins:

- Input: `In`
- Output: jump target branch.

Behavior:

- Immediately redirects execution to the connected target node.

### End

Purpose:

- Ends the active dialogue session.

Pins:

- Input: `In`

Behavior:

- Terminates the active dialogue when reached.

### Condition (Legacy)

Purpose:

- Old condition-check node kept so older assets can still open.

Use instead:

- Create a `Conditioned Branch` node for new content.
- Existing legacy condition nodes are treated as conditioned branches where possible.

## Triggering Actor Events From Dialogue

Use dialogue events when a conversation needs to communicate with world actors without hard-referencing placed actors in the dialogue asset.

Graph setup:

1. Add a `Set Condition` node at the point where the event should fire.
2. Add an action of type `Trigger Dialogue Event`.
3. Set `Event Tag` or `Event Name`.
4. Choose a target mode.
5. Connect the node to the next dialogue node.

Target modes:

- `Subsystem Only`: broadcasts only on the DQS subsystem.
- `Context Object`: sends to the dialogue context object if it implements the receiver interface.
- `All Receivers`: sends to all world actors that implement `DQS Dialogue Event Receiver Interface`.
- `Actors With Tag`: sends to actors with the configured Actor Tag.
- `Actors Of Class`: sends to actors of the selected class.

Receiver setup:

1. Open the actor Blueprint that should react.
2. Add `DQS Dialogue Event Receiver Interface` in Class Settings.
3. Implement `Receive DQS Dialogue Event`.
4. Branch on `Event Tag` or `Event Name`.
5. Run the actor behavior.

Example:

- Dialogue reaches `Set Condition: Open Gate`.
- `Trigger Dialogue Event` uses tag `Dialogue.Event.OpenVillageGate`.
- The gate actor implements the receiver interface and opens when it receives that tag.

## Quest Graph Nodes

### Start

Purpose:

- Starting point for a quest graph.
- `StartQuest` enters the quest through this node.

Pins:

- Input: `In`
- Output: `Next`

Behavior:

- Immediately forwards execution to the connected `Next` node.
- A quest graph should have exactly one `Start` node.

### Objective

Purpose:

- Represents an active quest objective.

Pins:

- Input: `In`
- Output: `Next`

Behavior:

- The quest pauses here until `AdvanceQuestObjective` reaches `Target Count`.
- When complete, execution follows `Next`.

Settings:

- `Title`: objective title.
- `Description`: text shown in tracker/log UI.
- `Objective Tag`: gameplay tag used by Blueprint calls to advance this objective.
- `Target Count`: required progress count.

### Branch

Purpose:

- Routes quest flow based on conditions.

Pins:

- Input: `In`
- Output: `Next Node`
- Output: `Alternative Node (Failed Condition)`

Behavior:

- If conditions pass, follows `Next Node`.
- If conditions fail, follows `Alternative Node (Failed Condition)`.

### Condition

Purpose:

- Quest condition check.

Use:

- Use when a quest needs to branch based on variables, tags, or other condition objects.

### Reward

Purpose:

- Runs reward/actions, then continues.

Pins:

- Input: `In`
- Output: `Next`

Settings:

- `Actions`: reward side effects.

### Subquest

Purpose:

- Starts another quest graph from this quest.

Pins:

- Input: `In`
- Output: `Next`

Settings:

- `Subquest Asset`: quest asset to start.

### Complete

Purpose:

- Marks the quest completed.

Pins:

- Input: `In`

### Fail

Purpose:

- Marks the quest failed.

Pins:

- Input: `In`

## First Playable Patterns

Simple dialogue:

1. `Entry -> Speech -> End`
2. Set `Speaker Id` and `Text` on `Speech`.
3. Add the dialogue asset to an NPC's `DQS Dialogue Participant Component`.

Dialogue with memory:

1. `Entry -> Conditioned Branch`
2. Success path: returning-player `Speech`
3. Failed path: first-time `Speech -> Set Condition -> End`
4. In `Set Condition`, add tag `Dialogue.NPC.Name.SpokenOnce`.

Quest acceptance:

1. `Entry -> Speech -> Choice`
2. Accept output: `Quest Hook(StartQuest) -> Speech -> End`
3. Decline output: `Speech -> End`

Simple quest:

1. `Start -> Objective -> Complete`
2. Set objective description, gameplay tag, and target count.
3. From gameplay, call `AdvanceQuestObjective` with the same quest id/tag.
