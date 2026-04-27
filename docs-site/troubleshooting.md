# Troubleshooting

## Start Dialogue Does Nothing

Print the return value from `Start Dialogue`.

If it returns `false`, open Output Log and search for `StartDialogue failed`.

Common messages:

- `DialogueAsset is null`: the subsystem node was called without an asset.
- `Dialogue Asset is not set on the DQSDialogueParticipantComponent`: assign a Dialogue Asset on the NPC's DQS component.
- `missing Entry node/runtime entry id`: open the graph, confirm one `Entry`, connect it, validate, and save.
- `missing a required outbound connection`: connect the node's required output.

If it returns `true` but no UI appears, add `/DialogQuestSystem/UI/WBP_DQSDialogue_Default` to the viewport or use a custom widget inheriting from `DQSDialogueWidgetBase`.

## Dialogue Event Does Not Reach An Actor

- Confirm the dialogue uses `Set Condition -> Trigger Dialogue Event`.
- Confirm the target actor implements `DQS Dialogue Event Receiver Interface`.
- Confirm the target mode can find the actor.
- Add a temporary `Print String` inside `Receive DQS Dialogue Event`.

## The guide panel does not show

- Make sure `Guided Mode` is active if you want the default onboarding flow
- Use the toolbar `Guide Panel` button to reopen it
- If needed, reopen the asset editor so the dock layout refreshes

## The graph feels too limited

Switch to `Full Editor`. Guided Mode intentionally hides a small set of advanced nodes.

## Validation says a choice branch is unconnected

Every choice output needs a connected target. Add an `End` node if the branch should stop there.

## Quest objective will not advance

Check:

- the objective node has a valid `Objective Tag`
- gameplay is calling `AdvanceQuestObjective`
- the tag passed from gameplay matches the node tag exactly

## Default UI does not appear

Check:

- the widget is created on begin play
- `Add to Viewport` is being called
- dialogue is actually starting
- the player controller exists and owns the widget in the current map
