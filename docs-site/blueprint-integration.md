# Blueprint Integration

## Starting Dialogue From An NPC

Recommended setup:

1. Add `DQS Dialogue Participant Component` to the NPC Blueprint.
2. Set the component's `Dialogue Asset`.
3. From your overlap/interact event, call `Start Dialogue` on that component.
4. Use `Self` for `World Context Object`.
5. Use `Self` or empty for `Override Context`.
6. Print the return value while testing.

If `Start Dialogue` returns `false`, open Output Log and search for `StartDialogue failed`.

## Runtime UI

Fastest setup:

1. On BeginPlay, create `/DialogQuestSystem/UI/WBP_DQSDialogue_Default` and add it to viewport.
2. Create `/DialogQuestSystem/UI/WBP_DQSQuestTracker_Default` and add it to viewport.
3. Create `/DialogQuestSystem/UI/WBP_DQSQuestLog_Default`, store the reference, and add it to viewport.
4. Bind a key to `Toggle Quest Log` on the stored quest log reference.

For custom UI, duplicate the Widget Blueprints into project content and edit them there. Keep the same parent base classes so helper functions and events stay available.

## Advancing Quest Objectives

Objectives are event-driven. From gameplay Blueprint, call `AdvanceQuestObjective` when something happens:

- Pickup item.
- Kill enemy.
- Reach location.
- Speak to NPC.
- Complete interaction.

The `Objective Tag` passed to `AdvanceQuestObjective` must match the active quest objective node.

## Triggering Actor Events From Dialogue

Use this when a dialogue should affect an actor, such as opening a door or changing an NPC state.

1. In the dialogue graph, add a `Set Condition` node.
2. Add a `Trigger Dialogue Event` action.
3. Set `Event Tag` or `Event Name`.
4. Choose a target mode.
5. On the receiving actor Blueprint, add `DQS Dialogue Event Receiver Interface`.
6. Implement `Receive DQS Dialogue Event`.

Prefer Gameplay Tags for event identity because they are easier to organize than raw names.

## Common Failure

If the log says:

`Dialogue Asset is not set on the DQSDialogueParticipantComponent`

Then select the NPC's `DQS Dialogue Participant Component` and assign the dialogue asset in Details.
