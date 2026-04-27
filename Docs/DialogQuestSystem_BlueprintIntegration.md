# DialogQuestSystem Blueprint Integration

## Core Runtime Entry Point
Use `Get Dialog Quest Subsystem` from `UDQSBlueprintLibrary` when you need to drive the system from Blueprint.

Main functions:
- `StartDialogue`
- `ContinueDialogue`
- `ChooseDialogueOption`
- `StartQuest`
- `AdvanceQuestObjective`
- `GetQuestState`
- `GetAllQuestStates`
- `SaveDialogQuestState`
- `LoadDialogQuestState`

## Starting Dialogue From an NPC
Recommended path:

1. Add `DQS Dialogue Participant Component` to an NPC Blueprint.
2. Assign a `Dialogue Asset` on the component.
3. Call the component's `StartDialogue` function from your interaction event.
4. Print the return value while testing. If it returns `false`, check Output Log for a `StartDialogue failed` message.

This is cleaner than resolving assets manually on every NPC.

Common setup:
- `World Context Object`: `Self`
- `Override Context`: `Self` or empty
- The component's `Dialogue Asset` must be set, otherwise nothing can start.

## Starting Dialogue From an Interface
If you want a provider pattern:

1. Implement `DQSDialogueProviderInterface` on your actor or object.
2. Implement `GetDialogueAsset`.
3. Call `StartDialogueFromProvider` from `UDQSBlueprintLibrary`.

## Driving UI
### Dialogue UI
- Fastest path: use the shipped widget Blueprint class `/DialogQuestSystem/UI/WBP_DQSDialogue_Default`.
- The plugin still ships `DQS Default Dialogue Widget` as the C++ base, but the recommended production path is to duplicate the widget Blueprint and edit that.
- `DQSDialogueWidgetBase` exposes helper functions for text, portrait, visibility, response state, and continue behavior.
- Standard UMG binding dropdowns do not expose functions that take parameters, so fixed helper functions are provided for common bindings.
- Dialogue start/end now automatically switch the first local player controller into and out of dialogue interaction mode:
  - show cursor on start
  - ignore move/look input during dialogue
  - restore game input on end
- The shipped default dialogue UI is horizontal and portrait-driven. If you create a child/duplicate of the widget Blueprint, you can directly change its named elements in UMG.

### Quest UI
- Fastest path: use the shipped widget Blueprints:
  - `/DialogQuestSystem/UI/WBP_DQSQuestTracker_Default`
  - `/DialogQuestSystem/UI/WBP_DQSQuestLog_Default`
- The C++ classes remain the runtime base classes, but the production-friendly editing path is to duplicate the widget Blueprints into project content.
- If you want a custom skin later, duplicate the shipped widget Blueprint or create a child widget Blueprint that preserves the named binding elements.
- Use `ViewModel -> LatestUpdate` for toast-style updates.
- Use `ViewModel -> QuestStates` for tracker lists.
- The quest log is hidden until toggled by design. Add it to the viewport, store a reference, then call `Toggle Quest Log`, `Open Quest Log`, or `Close Quest Log` from input.

## Triggering Quest Progress
Quest objectives are intended to be advanced from gameplay events.

Common examples:
- Pickup item: call `AdvanceQuestObjective`
- Kill enemy: call `AdvanceQuestObjective`
- Reach location: call `AdvanceQuestObjective`
- Speak to NPC: use a `QuestHook` node or explicit subsystem call

## Triggering Actor Events From Dialogue
Use dialogue events when a conversation should affect an actor without hard-referencing a placed actor in the dialogue asset.

Recommended graph setup:

1. Add a `Set Condition` node after the dialogue point that should trigger the event.
2. In the node's `Actions`, add `Trigger Dialogue Event`.
3. Set either `Event Tag` or `Event Name`.
4. Choose a target mode.
5. Connect the node to the next dialogue node.

Target modes:
- `Subsystem Only`: broadcasts `OnDialogueEventTriggered` on the DQS subsystem.
- `Context Object`: sends the event to the current dialogue context object if it implements the receiver interface.
- `All Receivers`: sends the event to every actor in the world that implements `DQS Dialogue Event Receiver Interface`.
- `Actors With Tag`: sends the event to actors whose Actor Tags contain the configured tag name.
- `Actors Of Class`: sends the event to actors of the selected class.

Receiver Blueprint setup:

1. Open the actor Blueprint that should react.
2. Add `DQS Dialogue Event Receiver Interface` in Class Settings.
3. Implement `Receive DQS Dialogue Event`.
4. Branch on `Event Tag` or `Event Name`.
5. Run your actor logic, such as opening a door, spawning an item, changing material, or starting animation.

Use Gameplay Tags when possible because they are easier to organize than raw names.

## Save/Load
- Call `SaveDialogQuestState` from pause menu, checkpoint, or manual save
- Call `LoadDialogQuestState` during load flow
- Keep slot naming consistent per profile

## Recommended Blueprint Base Classes
- `BP_DQSQuestGiverBase`
- `BP_DQSInteractableNPCBase`
- `BP_DQSObjectiveTriggerBase`
- `WBP_DQSDialogue`
- `WBP_DQSQuestTracker`
