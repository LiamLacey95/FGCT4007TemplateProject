# QA Checklist

Run this before distributing a plugin build.

## Graph Assets

- Create a new Dialogue Graph.
- Add `Speech`, `Choice`, `Conditioned Branch`, `Set Condition`, `Quest Hook`, `Jump`, and `End`.
- Validate and save.
- Reopen the asset and confirm connections and node details persist.
- Add a deprecated choice action in a test asset and confirm validation warns without deleting it.

## Runtime

- Start dialogue through `DQS Dialogue Participant Component`.
- Continue a speech line.
- Select a choice.
- Trigger a dialogue event to an actor receiver.
- Play a speech `VoiceSound`.
- Start a quest from dialogue.
- Advance an objective.
- Complete a quest.
- Save and load DQS state.

## Build Targets

- PIE.
- Standalone Game.
- Packaged Development.
- Packaged Shipping.

## Blueprint-Only Install

- Copy packaged plugin into a clean Blueprint-only project.
- Confirm Unreal does not ask to compile.
- Confirm default widgets load from plugin content.
