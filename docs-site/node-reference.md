# Node Reference

Dialogue nodes:

- `Entry`: start point.
- `Speech`: visible line, speaker, portrait/style, presentation tags, optional voice sound.
- `Choice`: player responses.
- `Conditioned Branch`: condition-based routing.
- `Set Condition`: action execution.
- `Quest Hook`: quest operation from dialogue.
- `Jump`: jump to another node.
- `End`: end dialogue.

Quest nodes:

- `Start`: start point.
- `Objective`: waits for objective progress.
- `Condition` / `Branch`: condition-based routing.
- `Reward`: action execution.
- `Subquest`: starts another quest.
- `Complete` / `Fail`: terminal quest states.

Deprecated choice-level conditions/actions are preserved but ignored by runtime.
