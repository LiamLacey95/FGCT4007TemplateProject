# DialogQuestSystem Architecture

## Purpose
DialogQuestSystem is structured as a precompiled C++ plugin that can be added to Blueprint-only UE 5.6 projects. The host project remains Blueprint-first; the plugin owns the engine/editor infrastructure.

## High-Level Model
- Dialogue and quests are separate assets.
- Both use graph-based authoring.
- Runtime state is shared across dialogue and quests.
- Branching logic is based on typed variables plus Gameplay Tags.
- UI listens to runtime events instead of polling game-specific logic.

## Modules
### DQSRuntime
- Defines the asset types: `UDialogueGraphAsset`, `UQuestGraphAsset`
- Defines node and state structs in `DQSTypes.h`
- Executes dialogue and quest graphs in `UDialogQuestSubsystem`
- Exposes Blueprint extension points via `UDQSCondition` and `UDQSAction`
- Persists runtime state through `UDQSSaveGame`

### DQSUI
- Provides `UDQSDialogueViewModel` and `UDQSQuestViewModel`
- Provides `UDQSDialogueWidgetBase` and `UDQSQuestTrackerWidgetBase`
- Keeps the runtime/UI boundary clean so projects can replace widget visuals without replacing quest/dialogue logic

### DQSEditor
- Registers custom asset actions for dialogue and quest assets
- Provides editor graph classes and node types
- Compiles graph nodes into runtime data arrays
- Runs lightweight validation and stores validation summaries on assets
- Opens a dedicated graph/details asset editor

## Runtime Flow
### Dialogue
1. An actor or Blueprint starts dialogue through `UDialogQuestSubsystem`.
2. The subsystem steps through non-presentational nodes immediately.
3. When it reaches a `Speech` or `Choice` node, it broadcasts an updated active line.
4. UI reads the active line and presents buttons or continue input.
5. Choice selection or continue advances the graph.

### Quests
1. A quest is started directly or through a dialogue quest hook.
2. The subsystem walks the quest graph until it reaches an unresolved objective, completion, or failure.
3. Objective progress is advanced externally through gameplay events or Blueprint calls.
4. The subsystem processes the next reachable nodes and emits quest update events.

## State Model
- `Global`: shared variables/tags used across the game
- `Quest`: per-quest runtime state and objective progress
- `Dialogue Session`: current active conversation and resumable node position

## Save/Load
- Save data stores global variables, global tags, active/completed quest runtime state, and active dialogue session state.
- Assets are referenced through soft references and stable IDs.
- Node positions are restored through GUIDs rather than editor order.

## Current Architectural Limits
- No multiplayer replication in v1
- No first-class cinematic graph layer in v1
- Validation currently focuses on graph structure and reachability, not deep semantic rule analysis
- The plugin ships base widget classes, not fully authored UMG content assets
