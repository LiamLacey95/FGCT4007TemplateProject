# DialogQuestSystem Authoring Guide

## Dialogue Authoring Rules
- Every dialogue asset should have exactly one `Entry` node.
- Use `Speech` nodes for presented NPC/player lines.
- Use `Choice` nodes only when the player must select between branches.
- Use `Condition` nodes for branch gating, not for presentation.
- Use `QuestHook` nodes when dialogue changes quest state.
- End every terminal path with an `End` node.

## Quest Authoring Rules
- Every quest asset should have exactly one `Start` node.
- Use `Objective` nodes for gameplay progress the player must satisfy.
- Use `Condition` or `Branch` nodes for alternate routes.
- Use `Reward` nodes for post-objective actions.
- Use `Complete` and `Fail` nodes for explicit end states.

## Naming Conventions
- Dialogue assets: `Dialogue_<Area>_<Purpose>`
- Quest assets: `Quest_<Area>_<Purpose>`
- NPC Blueprints: `BP_<Role>NPC`
- Widgets: `WBP_DQS<Feature>`
- Gameplay tags: `Quest.<QuestName>.<ObjectiveName>`

## Speaker Setup
- Add all speakers used by a dialogue asset to the `Speakers` array.
- Keep `Speaker Id` stable once referenced by nodes.
- Use readable display names; keep speaker IDs short and consistent.

## Choice Setup
- Every choice should have visible text.
- If a choice can be locked, set a clear `Failure Reason`.
- Put consequence logic in `Actions` or downstream nodes, not in widget code.

## Conditions and Actions
- Create Blueprint subclasses of `UDQSCondition` for reusable gating logic.
- Create Blueprint subclasses of `UDQSAction` for reusable side effects and rewards.
- Prefer reusable condition/action classes over writing one-off logic in every asset.

## Validation Habits
- Save assets after meaningful graph edits so the compiler updates runtime node data.
- Reopen and resave assets after node layout refactors that change connections heavily.
- Treat unreachable-node warnings as content bugs unless the node is intentionally unused.

## Graph Editor Rules
- New assets start with a creation wizard and a scaffolded starter pattern.
- `Guided Mode` is the default authoring mode for beginners.
- `Full Editor` exposes the full authoring surface when you need it.
- Add nodes by right-clicking in empty graph space.
- To auto-connect, drag from a node output pin into empty space and select the node type from the popup menu.
- Use the node created from the context menu directly; do not repurpose graph nodes by changing `Node Type` in details.
- For dialogue `Choice` nodes, each entry in the `Choices` array should create a corresponding output pin.
- You can right-click a `Choice` node to add or remove outputs directly from the node context menu.
- `Entry` and `Start` nodes are protected entry points and should remain in the graph.
- Use `Delete` to remove selected non-entry nodes.
- Use `Ctrl+C`, `Ctrl+V`, `Ctrl+X`, and duplicate shortcuts for fast graph editing.
- Ignore internal GUID and graph-link target fields unless you are debugging plugin internals.
- Use the `Guide` tab, `Validation` tab, and guidance banner as the primary in-editor help surface.
- Use `Fix First Issue` when the validation assistant offers a common beginner fix.
- Use `Insert Template` to add guided starter patterns with helper comments without starting from scratch.

## Beginner Workflow
1. Build the happy path first.
2. Add one refusal branch.
3. Add one conditional branch.
4. Add one quest hook.
5. Only after that, add reusable conditions/actions and richer UI.
