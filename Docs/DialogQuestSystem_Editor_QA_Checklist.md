# DialogQuestSystem Editor QA Checklist

Use this checklist when testing the custom graph editors in Unreal.

Important:

- Fully restart Unreal before running this checklist after plugin/editor code changes.
- Test on fresh assets first.
- Use one fresh `Dialogue Graph` and one fresh `Quest Graph`.
- If a check fails, write down:
  - graph type
  - node type
  - exact action performed
  - expected result
  - actual result

## Test Setup
1. Open [DialogQuestSystem.uproject](/F:/Ue5/Projects/DialogQuestSystem/DialogQuestSystem/DialogQuestSystem.uproject).
2. Create a fresh `Dialogue Graph` asset.
3. Create a fresh `Quest Graph` asset.
4. Open each asset in its custom graph editor, not the Blueprint class editor.

## Dialogue Graph Basics
- A new dialogue graph opens with exactly one default `Entry` node.
- After the default `Entry` exists, the graph creation menu does not offer another `Entry`.
- The graph does not show `EventGraph`, `Functions`, or `Macros`.
- Right-click empty graph space opens the dialogue node creation menu.
- The dialogue creation menu contains:
  - `Speech`
  - `Choice`
  - `Condition`
  - `Action`
  - `Quest Hook`
  - `Jump`
  - `End`

## Dialogue Node Behavior
### Entry
- `Entry` exists by default in a new graph.
- `Entry` has one input `In`.
- `Entry` has one output `Next`.
- `Entry` cannot be deleted.
- `Entry` cannot be duplicated.
- `Entry` can connect to the next node.

### Speech
- Add a `Speech` node from the context menu.
- `Speech` has one input `In`.
- `Speech` has one output `Next`.
- `Speech` can be deleted.
- `Speech` can be copied and pasted.
- `Speech` can be cut and pasted.
- `Speech` details allow editing:
  - `Title`
  - `Speaker Id`
  - `Text`
  - `Presentation Tags`
  - `Voice Sound`
  - `Conditions`
  - `Actions`

### Choice
- Add a fresh `Choice` node from the context menu.
- A fresh `Choice` node starts with two outputs.
- The number of outputs matches the number of `Choices` array entries.
- Changing the `Choices` array count updates the output count.
- Right-clicking the `Choice` node shows:
  - `Add Choice Output`
  - `Remove Last Choice Output`
- `Add Choice Output` adds one output pin.
- `Remove Last Choice Output` removes one output pin.
- Output pins stay wired to their intended branches after save and reopen.
- `Choice` can be deleted, copied, cut, and pasted.

### Condition
- Add a `Condition` node from the context menu.
- `Condition` has one input `In`.
- `Condition` has two outputs:
  - `Next Node`
  - `Alternative Node (Failed Condition)`
- Both outputs can be connected independently.
- `Condition` can be deleted, copied, cut, and pasted.

### Action
- Add an `Action` node from the context menu.
- `Action` has one input `In`.
- `Action` has one output `Next`.
- `Action` can be deleted, copied, cut, and pasted.

### QuestHook
- Add a `Quest Hook` node from the context menu.
- `QuestHook` has one input `In`.
- `QuestHook` has one output `Next`.
- `QuestHook` can be deleted, copied, cut, and pasted.

### Jump
- Add a `Jump` node from the context menu.
- `Jump` has one input `In`.
- `Jump` has one output branch used as the jump target connection.
- `Jump` can be deleted, copied, cut, and pasted.

### End
- Add an `End` node from the context menu.
- `End` has input only.
- `End` does not expose a normal outgoing `Next` pin.
- `End` can be deleted, copied, cut, and pasted.

## Quest Graph Basics
- A new quest graph opens with exactly one default `Start` node.
- After the default `Start` exists, the graph creation menu does not offer another `Start`.
- Right-click empty graph space opens the quest node creation menu.
- The quest creation menu contains:
  - `Objective`
  - `Condition`
  - `Branch`
  - `Reward`
  - `Fail`
  - `Complete`
  - `Subquest`

## Quest Node Behavior
### Start
- `Start` exists by default in a new graph.
- `Start` has one input `In`.
- `Start` has one output `Next`.
- `Start` cannot be deleted.
- `Start` cannot be duplicated.

### Objective
- Add an `Objective` node from the context menu.
- `Objective` has one input `In`.
- `Objective` has one output `Next`.
- `Objective` can be deleted, copied, cut, and pasted.
- `Objective` details allow editing:
  - `Title`
  - `Description`
  - `Objective Tag`
  - `Target Count`
  - `Conditions`
  - `Actions`

### Condition
- Add a `Condition` node from the context menu.
- `Condition` has one input `In`.
- `Condition` has two outputs:
  - `Next Node`
  - `Alternative Node`
- `Condition` can be deleted, copied, cut, and pasted.

### Branch
- Add a `Branch` node from the context menu.
- `Branch` has one input `In`.
- `Branch` has two outputs:
  - `Next Node`
  - `Alternative Node`
- `Branch` can be deleted, copied, cut, and pasted.

### Reward
- Add a `Reward` node from the context menu.
- `Reward` has one input `In`.
- `Reward` has one output `Next`.
- `Reward` can be deleted, copied, cut, and pasted.

### Fail
- Add a `Fail` node from the context menu.
- `Fail` has input only.
- `Fail` can be deleted, copied, cut, and pasted.

### Complete
- Add a `Complete` node from the context menu.
- `Complete` has input only.
- `Complete` can be deleted, copied, cut, and pasted.

### Subquest
- Add a `Subquest` node from the context menu.
- `Subquest` has one input `In`.
- `Subquest` has one output `Next`.
- `Subquest` can be deleted, copied, cut, and pasted.

## Connection Behavior
- Dragging from an output pin to an input pin creates a connection.
- Dragging from a pin into empty space and choosing a node auto-connects the new node.
- Every freshly created non-root node immediately shows its expected input/output pins without needing to reopen the asset.
- Invalid self-connections are rejected.
- Connecting two outputs together is rejected.
- Connecting two inputs together is rejected.
- Right-click a node and use `Break Node Links`; all links break correctly.

## Node Editing Shortcuts
- Select a node and press `Delete`; it is removed if it is not a protected entry node.
- Select a node and press `Ctrl+C`; it copies.
- Select a node and press `Ctrl+V`; it pastes.
- Select a node and press `Ctrl+X`; it cuts.
- Pasted nodes receive new internal IDs instead of reusing the original IDs.

## Details Panel Behavior
### Dialogue Nodes
- Normal node authoring details do not show internal fields:
  - `Node Id`
  - `Node Type`
  - `Next Node Id`
  - `Alternate Node Id`
  - `Jump Target Node Id`
- Dialogue choice entries do not show:
  - `Choice Id`
  - `Target Node Id`

### Quest Nodes
- Normal node authoring details do not show internal fields:
  - `Node Id`
  - `Node Type`
  - `Next Node Id`
  - `Alternate Node Id`

## Save / Reopen Behavior
- Saving the asset preserves layout and connections.
- Reopening the asset preserves layout and connections.
- Branch connections still compile correctly after reopen.
- Choice output count remains correct after reopen.
- Validation metadata remains accessible at the asset level.

## Asset-Level Checks
- Selecting graph background shows asset-level settings rather than node-level settings.
- Asset metadata can be edited:
  - `Graph Id`
  - `Display Name`
  - `Description`
  - `Tags`
  - `Author Notes`
- Dialogue asset background also exposes:
  - `Speakers`
  - `Entry Node Id`
  - `Allow Resume`
- Quest asset background also exposes:
  - `Entry Node Id`

## Regression Checks
- Older graph assets created before recent editor changes still open.
- Older graph assets can be edited and saved without crashing.
- Hidden internal fields remain hidden after a full editor restart.
- Choice nodes created after restart show the correct number of outputs.
- Drag-to-create auto-connect works in both graph types.

## Failure Notes Template
When something fails, record:

- Asset type:
- Node type:
- Steps to reproduce:
- Expected result:
- Actual result:
- Did it happen after a full editor restart:
- Screenshot attached:
