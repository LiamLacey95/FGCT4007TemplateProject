# DialogQuestSystem Documentation

DialogQuestSystem is a UE 5.6 dialogue and quest plugin for Blueprint-first projects. The plugin has three modules:

- `DQSRuntime`: graph assets, runtime subsystem, conditions, actions, save/load.
- `DQSUI`: default widgets and view models.
- `DQSEditor`: custom graph editors, compilation, validation, and packaged editor tooling.

Use these docs in order:

1. [Installation](DialogQuestSystem_Installation.md)
2. [Authoring Guide](DialogQuestSystem_AuthoringGuide.md)
3. [Node Reference](DialogQuestSystem_Node_Reference.md)
4. [Blueprint Integration](DialogQuestSystem_BlueprintIntegration.md)
5. [Runtime UI](DialogQuestSystem_RuntimeUI.md)
6. [Packaging](DialogQuestSystem_PackagingGuide.md)
7. [Troubleshooting](DialogQuestSystem_Troubleshooting.md)
8. [QA Checklist](DialogQuestSystem_QA_Checklist.md)

Important compatibility note: deprecated choice-level conditions/actions are preserved when old assets are opened, but runtime does not execute them. Move that logic into `Conditioned Branch` and `Set Condition` nodes.
