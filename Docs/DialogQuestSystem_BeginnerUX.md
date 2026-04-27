# DialogQuestSystem Beginner UX

This document describes the beginner-first editor flow that now ships inside the graph editors.

## New Asset Flow
- Creating a `Dialogue Graph` opens a starter wizard.
- Creating a `Quest Graph` opens a starter wizard.
- The wizard asks for only the minimum information needed to scaffold a useful first graph.
- New assets open with a starter pattern already connected instead of a blank graph.

## Guided Mode and Full Editor
- New assets default to `Guided Mode`.
- `Guided Mode` hides lower-value authoring fields in the Details panel and keeps the guide workflow front and center.
- `Full Editor` shows the full authoring surface.
- Use the toolbar to switch between modes.

## Guided Help
- The graph tab now shows a guidance banner with the next recommended step.
- The `Guide` tab shows:
  - a short welcome
  - the current checklist
  - contextual node help for the selected node
- Use `Toggle Help` in the toolbar if you want the graph editor to be quieter.

## Validation Assistant
- The `Validation` tab shows categorized issues:
  - blocking
  - warning
  - suggestion
- Each issue explains:
  - what is wrong
  - why it matters
  - how to fix it
- Use `Jump To Related Node` to move directly to the relevant graph node.
- Use `Fix First Issue` in the toolbar for common beginner fixes.

## Snippets
- Use `Insert Template` from the toolbar to drop a guided starter pattern with helper comments into the graph.
- Current snippets include:
  - dialogue acceptance pattern
  - quest objective pattern

## Simulation
- The `Simulation` tab provides a lightweight graph preview.
- Use:
  - `Start`
  - `Primary`
  - `Alternate`
  - `Reset`
- This is intended as a simple path preview, not a full runtime replacement.

## Intended Beginner Loop
1. Create a graph with the wizard.
2. Follow the banner and checklist.
3. Fill out the most visible missing fields first.
4. Use validation to resolve the first playable path.
5. Use snippets to extend the graph once the happy path works.
