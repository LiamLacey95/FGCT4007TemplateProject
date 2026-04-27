# DialogQuestSystem Troubleshooting

## I opened a doc and it looks blank
- Confirm you opened the file under the project root `Docs` folder, not a stale tab or wrong relative path.
- Correct path example:
  - `F:\Ue5\Projects\DialogQuestSystem\DialogQuestSystem\Docs\DialogQuestSystem_FirstPlayableFlow.md`
- If your editor shows a blank preview, open the raw markdown text directly.

## The plugin is enabled but I do not see Dialogue Graph or Quest Graph in the editor
- Restart the Unreal Editor after the first successful plugin compile.
- Confirm the plugin is enabled in `Edit > Plugins`.
- Confirm the editor build succeeded and plugin binaries exist under:
  - `Plugins/DialogQuestSystem/Binaries/Win64`
- If needed, rebuild:
  - `Build.bat DialogQuestSystemEditor Win64 Development <uproject> -WaitMutex -NoHotReload`

## I created a QuestGraphAsset or DialogueGraphAsset Blueprint and got an EventGraph
- That is the wrong asset type for content authoring.
- You created a Blueprint class derived from the asset class instead of a custom `Quest Graph` or `Dialogue Graph` asset instance.
- Delete or ignore that Blueprint asset.
- In the Content Browser, create the asset by searching for `Quest Graph` or `Dialogue Graph`.
- The correct editor should not show `EventGraph`, `Functions`, or `Macros` on the left.

## Unreal says a C++ module is missing or out of date
- This plugin is code-based even though the host game is Blueprint-only.
- That means the project still needs the compiled plugin binaries that match your UE version.
- Rebuild the editor target after plugin code changes.

## I get warnings about `aqProf.dll`, `VtuneApi.dll`, `WinPixGpuCapturer.dll`, or missing engine icons
- Those are common environment/editor warnings.
- They are not DialogQuestSystem plugin failures by themselves.
- Ignore them unless they are paired with a real compile or module load failure.

## Dialogue or quest assets are not behaving as expected
- Save the asset to force compile from editor graph to runtime node data.
- Check that the graph has exactly one `Entry` or `Start` node.
- Check that each required output pin is connected.
- Check that the target quest asset is assigned on `QuestHook` nodes.
- Reopen the asset editor if the graph/details panel gets out of sync after heavy edits.

## Choice node output count does not match the number of choices
- Each `Choices` array element is supposed to create one output pin.
- Create `Choice` nodes directly from the graph context menu.
- Do not use the `Node Type` field as a conversion mechanism for existing graph nodes.
- You can use the node context menu entries `Add Choice Output` and `Remove Last Choice Output` to manage outputs explicitly.
- If a newly created `Choice` node still does not match the array count after a rebuild/reopen, report it as a bug with a screenshot and the exact sequence used to create the node.

## Dragging from a pin into empty space does not auto-connect the new node
- The intended workflow is drag from an existing pin, release in empty space, then select the node type from the popup menu.
- If the node appears but does not connect, rebuild/restart once to ensure the latest editor module is loaded.

## Delete or copy/paste shortcuts do not work in the graph editor
- The custom graph editor supports `Delete`, `Ctrl+C`, `Ctrl+V`, `Ctrl+X`, and duplication shortcuts after the updated editor module is loaded.
- If the shortcuts do nothing, rebuild the project plugin and restart the editor so the latest `DQSEditor` module is active.

## Node Id, Next Node Id, or Alternate Node Id are still visible in node details
- You do not need to recreate your graphs.
- A full editor restart is recommended for detail panel customization changes.
- `Ctrl+Alt+F11` Live Coding reload may compile code, but it does not always fully refresh custom Property Editor layouts.
- Close Unreal completely and reopen the project.
- These fields are internal graph/compiler data and are not meant for normal authoring.

## The dialogue UI does not update
- Confirm the widget derives from `DQSDialogueWidgetBase`.
- Confirm the widget is created and added to viewport.
- Confirm the subsystem is being reached from the current world/game instance.
- Confirm dialogue actually starts through the subsystem or participant component.

## I call Start Dialogue but nothing happens
- Print the return value from `Start Dialogue`.
- If it returns `false`, open Output Log and search for `StartDialogue failed`.
- Common messages:
  - `DialogueAsset is null`: the subsystem node was called without an asset.
  - `Dialogue Asset is not set on the DQSDialogueParticipantComponent`: select the NPC's `DQS Dialogue Participant Component` and assign the Dialogue Asset.
  - `missing Entry node/runtime entry id`: open the dialogue graph, confirm it has one `Entry`, connect `Entry -> first node`, validate, and save.
  - `missing a required outbound connection`: a flow node such as `Entry`, `Set Condition`, `Quest Hook`, or `Conditioned Branch` is not connected.
- If `Start Dialogue` returns `true` but the UI does not appear, the dialogue is running and the widget is the issue. Create/add `/DialogQuestSystem/UI/WBP_DQSDialogue_Default` to viewport on BeginPlay, or add your custom widget that inherits from `DQSDialogueWidgetBase`.

## My dialogue should change the second time I talk to an NPC
- Use a `Conditioned Branch` near the start of the dialogue.
- Check a tag or variable such as `Dialogue.NPC.Elder.SpokenOnce`.
- On the first-time path, add a `Set Condition` node that sets that tag/variable before the dialogue ends.
- On later starts, the `Conditioned Branch` will route to the alternate line.

## My dialogue event is not reaching an actor
- Confirm the dialogue uses a `Set Condition` node with a `Trigger Dialogue Event` action.
- Confirm the target actor implements `DQS Dialogue Event Receiver Interface`.
- Confirm the selected target mode can find the actor:
  - `Context Object` requires the context object to be the receiver.
  - `Actors With Tag` requires the Actor Tag to match.
  - `Actors Of Class` requires the class to match.
  - `All Receivers` only finds actors that implement the receiver interface.
- Add a temporary `Print String` inside `Receive DQS Dialogue Event` to confirm the actor receives the event.

## A quest does not complete
- Verify the quest `GraphId` and the objective tag being passed to `AdvanceQuestObjective`.
- Verify the active node is the expected `Objective` node.
- Verify the objective `TargetCount` is reached.

## I still need help with a specific error
Provide one of these:

- the full screenshot
- the exact error text
- the last 50 relevant lines from `Saved/Logs/DialogQuestSystem.log`

Without the actual message, the best I can do is narrow likely causes rather than identify the exact failure.
