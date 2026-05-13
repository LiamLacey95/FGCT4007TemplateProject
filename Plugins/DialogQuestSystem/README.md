# DialogQuestSystem

DialogQuestSystem is a UE 5.6 plugin for authoring and running branching dialogue and quest content in Blueprint-only host projects.

It provides:

- `Dialogue Graph` and `Quest Graph` asset types
- Runtime execution through `UDialogQuestSubsystem`
- Blueprint-extensible conditions and actions
- Save/load support for dialogue and quest state
- Default dialogue, quest tracker, and quest log widgets
- Blueprint-authored default widget assets and a reusable UI theme asset
- Guided graph authoring with starter comments, validation, and simulation
- Sample dialogue and quest assets under plugin content
- Custom editor tooling for graph authoring and validation

## Modules
- `DQSRuntime`: runtime data model, execution, save/load, Blueprint APIs
- `DQSUI`: widget/view-model layer for dialogue and quest presentation
- `DQSEditor`: graph asset factories, schemas, compiler, validation, and asset editors

## Documentation
- [Documentation Index](../../Docs/DialogQuestSystem_Docs_Index.md)
- [Installation](../../Docs/DialogQuestSystem_Installation.md)
- [Authoring Guide](../../Docs/DialogQuestSystem_AuthoringGuide.md)
- [Node Reference](../../Docs/DialogQuestSystem_Node_Reference.md)
- [Blueprint Integration](../../Docs/DialogQuestSystem_BlueprintIntegration.md)
- [Runtime UI](../../Docs/DialogQuestSystem_RuntimeUI.md)
- [Packaging Guide](../../Docs/DialogQuestSystem_PackagingGuide.md)
- [Troubleshooting](../../Docs/DialogQuestSystem_Troubleshooting.md)
- [QA Checklist](../../Docs/DialogQuestSystem_QA_Checklist.md)

## Current State
The plugin compiles for `DialogQuestSystemEditor Win64 Development` on UE 5.6 in this project.

Sample graph assets are shipped under plugin content:

- `/DialogQuestSystem/Samples/Dialogue/DQS_Sample_QuestOffer`
- `/DialogQuestSystem/Samples/Quests/DQS_Sample_FirstFavor`

Dialogue authoring uses `Conditioned Branch` for checks and `Set Condition` for state changes/actions. Deprecated choice-level conditions/actions are preserved for compatibility, but runtime does not execute them.
