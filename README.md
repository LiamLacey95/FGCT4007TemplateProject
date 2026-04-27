# FGCT4007TemplateProject

UE 5.6.1 third-person template project with the DialogQuestSystem plugin installed.

## Included

- UE third-person Blueprint template content
- DialogQuestSystem plugin in `Plugins/DialogQuestSystem`
- Default third-person map at `/Game/ThirdPerson/Lvl_ThirdPerson`

## Open

Open:

```text
FGCT4007TemplateProject.uproject
```

The project is intentionally minimal: no extra sample scripts, generated caches, or helper plugins are included.

## DialogQuestSystem Docs

Local plugin documentation is included in:

```text
Docs/
docs-site/
mkdocs.yml
```

Start with:

```text
Docs/DialogQuestSystem_FirstPlayableFlow.md
Docs/DialogQuestSystem_BlueprintIntegration.md
Docs/DialogQuestSystem_Node_Reference.md
Docs/DialogQuestSystem_Troubleshooting.md
```

The docs describe the current workflow: `Conditioned Branch` checks dialogue state, `Set Condition` sets tags/variables or triggers events, and `DQS Dialogue Participant Component` starts NPC dialogue from Blueprint.
