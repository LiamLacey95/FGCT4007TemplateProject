# Node Reference

The current beginner-facing dialogue model is:

- Use `Conditioned Branch` when dialogue needs to check state.
- Use `Set Condition` when dialogue needs to set state, add tags, set variables, or trigger events.
- Use `Quest Hook` when dialogue needs to start, advance, complete, or fail a quest.

## Dialogue Nodes

| Node | Use it for | Main outputs |
| --- | --- | --- |
| `Entry` | Dialogue start point | `Next` |
| `Speech` | A shown line of dialogue | `Next` |
| `Choice` | Player responses | One output per choice |
| `Conditioned Branch` | State-based routing | `Next Node`, `Alternative Node (Failed Condition)` |
| `Set Condition` | Tags, variables, and actions | `Next` |
| `Quest Hook` | Quest operations from dialogue | `Next` |
| `Jump` | Redirecting flow | Jump target |
| `End` | Ending dialogue | None |

Choice nodes create one output pin per `Choices` entry. If you add or remove entries in the Details panel, the graph node refreshes its visible pins so each response can connect to a separate target.

## Quest Nodes

| Node | Use it for |
| --- | --- |
| `Start` | Quest start point |
| `Objective` | Event-driven objective progress |
| `Branch` / `Condition` | Quest routing |
| `Reward` | Reward/actions |
| `Subquest` | Starting another quest |
| `Complete` | Completing the quest |
| `Fail` | Failing the quest |

## Dialogue Memory Pattern

To change what an NPC says after the first conversation:

1. Connect `Entry -> Conditioned Branch`.
2. Add a condition that checks a tag such as `Dialogue.NPC.Elder.SpokenOnce`.
3. Send the success path to the returning-player line.
4. Send the failed path to the first-time line.
5. Before the first-time path ends, add `Set Condition` and set the tag.

## Triggering Actor Events

To make dialogue communicate with actors:

1. Add a `Set Condition` node.
2. Add a `Trigger Dialogue Event` action.
3. Set `Event Tag` or `Event Name`.
4. Pick a target mode.
5. On the receiving actor, implement `DQS Dialogue Event Receiver Interface`.
6. Implement `Receive DQS Dialogue Event` and branch on the tag/name.

Target modes:

- `Subsystem Only`: broadcast through the subsystem.
- `Context Object`: send to the current dialogue context.
- `All Receivers`: send to all actors implementing the receiver interface.
- `Actors With Tag`: send to actors with a matching Actor Tag.
- `Actors Of Class`: send to actors of a selected class.

For the full local reference, see `Docs/DialogQuestSystem_Node_Reference.md` in the repository.
