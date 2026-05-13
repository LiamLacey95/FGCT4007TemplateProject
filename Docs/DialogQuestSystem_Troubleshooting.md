# Troubleshooting

## Dialogue Works In PIE But Not Standalone Or Packaged

Open the graph, click Validate, save the asset, and package again. Packaged builds use compiled runtime node arrays; editor graph data is stripped. Current plugin builds also compile Dialogue and Quest Graph assets automatically before save/cook.

## Set Condition Event Does Not Fire

Check that:

- The `Set Condition` node is connected in the dialogue flow.
- The action is on the `Set Condition` node, not deprecated choice-level action data.
- `Target Mode` matches the receiver setup.
- The receiving actor, or one of its actor components, implements `DQS Dialogue Event Receiver Interface`.
- The receiver exists in the runtime world when the event fires.

## Old Choice Actions Or Conditions Stopped Working

Old per-choice conditions/actions are preserved but not executed. Move conditions to a `Conditioned Branch` node and actions to a `Set Condition` node after the choice output.

## Voice Sound Does Not Play

Set `VoiceSound` on a `Speech` node. Validate and save the graph. The runtime plays it when that speech line is presented.

## Quest Does Not Advance

Confirm the active quest is on an `Objective` node and the `ObjectiveTag` passed to `AdvanceQuestObjective` exactly matches the node's tag.

## Unreal Asks To Compile In A Blueprint Project

The project probably received a source plugin or binaries for the wrong Unreal version/platform. Use a packaged plugin build matching the student's engine version.
