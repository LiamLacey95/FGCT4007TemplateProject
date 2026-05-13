# Authoring Guide

## Dialogue Flow

Create a Dialogue Graph and connect an `Entry` node to the first runtime node. Dialogue execution runs through flow nodes until it reaches a visible `Speech`, `Choice`, or `End` node.

Recommended pattern:

```text
Entry -> Speech -> Choice -> Speech -> End
```

Use `Conditioned Branch` for checks and `Set Condition` for side effects.

## Conditions And Actions

Conditions are `UDQSCondition` objects. Actions are `UDQSAction` objects. Built-in actions include variable changes, tag changes, and `Trigger Dialogue Event`.

Use them like this:

```text
Choice output -> Set Condition -> next Speech or End
Choice output -> Conditioned Branch -> matched/fallback paths
```

Deprecated choice-level `Conditions` and `Actions` are preserved for compatibility, but runtime no longer executes them. The validator warns when it finds them.

## Quest Flow

Create a Quest Graph with a `Start` node. Runtime advances through flow nodes until it reaches an `Objective`, `Complete`, or `Fail` node.

Use `Objective` nodes for progress gates, `Branch` or `Condition` nodes for checks, `Reward` nodes for reward actions, and `Subquest` nodes to start another quest.

## Saving Work

After editing graph details or node connections, save the graph asset. The editor compiles graph nodes into runtime data before save so Standalone and packaged builds can run without editor-only graph data.
