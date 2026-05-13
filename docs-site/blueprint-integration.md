# Blueprint Integration

Start dialogue from an NPC with `DQS Dialogue Participant Component`.

For dialogue events:

1. Add a `Set Condition` node.
2. Add `Trigger Dialogue Event`.
3. Set tag/name and target mode.
4. Implement `DQS Dialogue Event Receiver Interface` on an actor or one of its actor components.

`All Actors With DQS Event Receiver` calls actors and actor components that implement the interface. `Context Object` only works when the current dialogue context object implements the interface.

Advance quests with `AdvanceQuestObjective(QuestId, ObjectiveTag, Delta)`.
