# Blueprint Integration

## Starting Dialogue

Recommended NPC setup:

1. Add `DQS Dialogue Participant Component` to the NPC Blueprint.
2. Assign a Dialogue Graph asset.
3. On interaction, call `StartDialogue` on the component.
4. Pass `Self` as context when the NPC or interacting actor should receive context-sensitive actions.

You can also call `StartDialogue` directly on `Get Dialog Quest Subsystem`.

## Receiving Dialogue Events

For actor-facing events:

1. Add a `Set Condition` node.
2. Add `Trigger Dialogue Event` to the node's `Actions`.
3. Set `Event Tag` or `Event Name`.
4. Set `Target Mode`.
5. Implement `DQS Dialogue Event Receiver Interface` on the receiving actor or one of its actor components.
6. Implement `Receive DQS Dialogue Event` and branch on the tag/name.

Target modes:

- `Subsystem Only`: broadcasts `OnDialogueEventTriggered` on the subsystem.
- `Context Object`: calls the current context object if it implements the receiver interface.
- `All Actors With DQS Event Receiver`: calls every actor in the world that implements the interface, plus receiver components on those actors.
- `Actors With Actor Tag`: calls actors with the configured Actor Tag, plus receiver components on those actors.
- `Actors Of Class`: calls actors of the selected class, plus receiver components on those actors.

Quest reward actions run without dialogue context, so avoid `Context Object` for quest rewards unless you supply your own action class with context.

## Quest Progress

Call `StartQuest` with a Quest Graph asset. Advance objectives with:

```text
AdvanceQuestObjective(QuestId, ObjectiveTag, Delta)
```

The active quest objective must match `ObjectiveTag`.
