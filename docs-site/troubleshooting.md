# Troubleshooting

If behavior works in PIE but not Standalone or packaged, validate and save the graph asset. Packaged builds use compiled runtime graph data, and current plugin builds compile graph assets automatically before save/cook.

If `Set Condition` events do not fire, confirm the node is connected, the action is on the node, and the receiving actor or one of its actor components implements `DQS Dialogue Event Receiver Interface`.

If old choice-level conditions/actions are found, migrate them to `Conditioned Branch` and `Set Condition` nodes.

If voice does not play, set `VoiceSound` on a `Speech` node and save the graph.
