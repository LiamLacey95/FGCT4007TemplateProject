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
- [Documentation Index](/F:/Ue5/Projects/DialogQuestSystem/DialogQuestSystem/Docs/DialogQuestSystem_Docs_Index.md)
- [Architecture](/F:/Ue5/Projects/DialogQuestSystem/DialogQuestSystem/Docs/DialogQuestSystem_Architecture.md)
- [Authoring Guide](/F:/Ue5/Projects/DialogQuestSystem/DialogQuestSystem/Docs/DialogQuestSystem_AuthoringGuide.md)
- [Beginner UX](/F:/Ue5/Projects/DialogQuestSystem/DialogQuestSystem/Docs/DialogQuestSystem_BeginnerUX.md)
- [Blueprint Integration](/F:/Ue5/Projects/DialogQuestSystem/DialogQuestSystem/Docs/DialogQuestSystem_BlueprintIntegration.md)
- [Node Reference](/F:/Ue5/Projects/DialogQuestSystem/DialogQuestSystem/Docs/DialogQuestSystem_Node_Reference.md)
- [Editor QA Checklist](/F:/Ue5/Projects/DialogQuestSystem/DialogQuestSystem/Docs/DialogQuestSystem_Editor_QA_Checklist.md)
- [First Playable Flow](/F:/Ue5/Projects/DialogQuestSystem/DialogQuestSystem/Docs/DialogQuestSystem_FirstPlayableFlow.md)
- [UI Customization](/F:/Ue5/Projects/DialogQuestSystem/DialogQuestSystem/Docs/DialogQuestSystem_UICustomization.md)
- [Packaging Guide](/F:/Ue5/Projects/DialogQuestSystem/DialogQuestSystem/Docs/DialogQuestSystem_PackagingGuide.md)
- [Release Checklist](/F:/Ue5/Projects/DialogQuestSystem/DialogQuestSystem/Docs/DialogQuestSystem_ReleaseChecklist.md)
- [Troubleshooting](/F:/Ue5/Projects/DialogQuestSystem/DialogQuestSystem/Docs/DialogQuestSystem_Troubleshooting.md)

## Current State
The plugin compiles for `DialogQuestSystemEditor Win64 Development` on UE 5.6 in this project.

Sample graph assets are shipped under plugin content:

- `/DialogQuestSystem/Samples/Dialogue/DQS_Sample_QuestOffer`
- `/DialogQuestSystem/Samples/Quests/DQS_Sample_FirstFavor`

Dialogue authoring now uses `Conditioned Branch` for checks and `Set Condition` for state changes/actions. Dialogue can also trigger actor-facing events through the `Trigger Dialogue Event` action and `DQS Dialogue Event Receiver Interface`.
