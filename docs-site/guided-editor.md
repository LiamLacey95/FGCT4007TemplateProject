# Guided Editor

## Guided Mode

Guided Mode is the default authoring experience for new assets.

It does four things on purpose:

- starts from a scaffolded template instead of a blank graph
- adds comment nodes that explain what to edit first
- keeps the palette focused on common node types
- keeps the guide and validation surfaces front and center

For dialogue graphs, Guided Mode emphasizes:

- `Speech`
- `Choice`
- `Quest Hook`
- `End`

For quest graphs, Guided Mode emphasizes:

- `Objective`
- `Branch`
- `Reward`
- `Complete`
- `Fail`

## Full Editor

Full Editor opens the full palette and advanced authoring surface.

Use it when you need:

- dialogue `Condition`, `Action`, or `Jump`
- quest `Condition` or `Subquest`
- lower-level graph shaping beyond the guided happy path

## Guide panel

The guide panel is a real docked tab. Toggling it from the toolbar opens or closes the tab instead of only hiding the text.

Use it for:

- checklist progress
- selected-node help
- next-step guidance

## Drag from pin

You can drag from a node's flow pin into empty canvas, type the node you want, and select it from the popup. The editor should create the new node and automatically connect it to the dragged pin.

If the node is created without a connection, restart the editor after rebuilding the plugin so the latest `DQSEditor` module is loaded.

## Validation panel

Validation is intended to be a working assistant, not just a compiler dump.

Each issue card shows:

- severity
- what is wrong
- why it matters
- how to fix it
- jump-to-node action
- `Fix It` when an automatic repair is supported

## Simulation panel

Simulation uses graph-native wording when possible:

- `Choice 1` / `Choice 2` on dialogue choice nodes
- `Next Node` / `Failed Condition` on dialogue condition nodes
- `Primary Branch` / `Alternative Branch` on quest branch nodes

## Guided starter comments

Starter templates use standard graph comment nodes so they are visible in the editor but excluded from runtime compilation.

The intent is simple:

- you should be able to open a fresh asset and understand what to edit first
- the graph should teach the workflow without requiring external docs
