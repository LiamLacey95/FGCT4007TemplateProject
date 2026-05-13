# Node Reference

## Dialogue Nodes

`Entry`: Required starting node. Connect it to the first dialogue node.

`Speech`: Presents speaker name, text, portrait/style overrides, presentation tags, and optional `VoiceSound`. If `VoiceSound` is set, the runtime plays it when the line is presented.

`Choice`: Presents player responses. Every choice output should connect to another node. Deprecated per-choice conditions/actions are preserved but ignored by runtime.

`Conditioned Branch`: Evaluates its `Conditions`. If all pass, execution follows the matched output; otherwise it follows the fallback output.

`Set Condition`: Executes its `Actions`, then continues to the next node. Use this for variables, tags, quest changes, and dialogue events.

`Quest Hook`: Starts, advances, completes, or fails a quest asset.

`Jump`: Jumps to its connected target.

`End`: Ends the active dialogue.

## Quest Nodes

`Start`: Required starting node.

`Objective`: Stops quest flow until `AdvanceQuestObjective` reaches `TargetCount` for `ObjectiveTag`.

`Condition` / `Branch`: Evaluates `Conditions` and routes to matched/fallback outputs.

`Reward`: Executes `Actions`, then continues.

`Subquest`: Starts another quest asset, then continues.

`Complete`: Marks the quest completed.

`Fail`: Marks the quest failed.
