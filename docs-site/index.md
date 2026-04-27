# DialogQuestSystem

DialogQuestSystem is a UE5 plugin for building branching dialogue and quest flows in Blueprint-only projects without forcing designers into raw data editing.

## What ships in the plugin

- Guided graph editors for `Dialogue Graph` and `Quest Graph`
- `Guided Mode` and `Full Editor`
- In-graph starter comments, checklists, validation, and simulation
- Runtime subsystem APIs for dialogue, quests, save/load, and input handoff
- Default dialogue UI, quest tracker, and quest log widgets
- Sample dialogue and quest assets under plugin content
- Dialogue event hooks for communicating with actors from conversations

## Best entry points

- Open the sample dialogue asset: `/DialogQuestSystem/Samples/Dialogue/DQS_Sample_QuestOffer`
- Open the sample quest asset: `/DialogQuestSystem/Samples/Quests/DQS_Sample_FirstFavor`
- Create a new `Dialogue Graph` or `Quest Graph` from the Content Browser and stay in `Guided Mode` for your first pass

## Documentation sections

- `Install`: project setup, plugin placement, and first load
- `Guided Editor`: how the graph UX works in Guided Mode vs Full Editor
- `Blueprint Integration`: NPC setup, UI setup, quest progress, and dialogue events
- `Node Reference`: what each dialogue and quest node does
- `Runtime UI`: shipped widgets and how to replace/theme them
- `Packaging`: packaging the plugin for reuse in other UE5 projects
- `Troubleshooting`: common editor and runtime issues

## Screenshot workflow

Store documentation screenshots under `docs-site/assets/screenshots/` and keep file names task-specific, for example:

- `guided-mode-dialogue-overview.png`
- `validation-panel-blocking-issue.png`
- `default-dialogue-widget-runtime.png`
