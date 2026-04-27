#include "DQSGraphEditorUtils.h"

#include "DQSEdGraphNodes.h"
#include "DQSDialogueGraphAsset.h"
#include "DQSGraphCompiler.h"
#include "DQSQuestGraphAsset.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraphNode_Comment.h"
#include "EdGraph/EdGraphPin.h"

namespace
{
	UEdGraphPin* GetInputPin(UEdGraphNode* Node)
	{
		if (!Node)
		{
			return nullptr;
		}

		for (UEdGraphPin* Pin : Node->Pins)
		{
			if (Pin && Pin->Direction == EGPD_Input)
			{
				return Pin;
			}
		}

		return nullptr;
	}

	UEdGraphPin* GetOutputPin(UEdGraphNode* Node, const int32 OutputPinIndex)
	{
		if (!Node)
		{
			return nullptr;
		}

		int32 CurrentOutputIndex = 0;
		for (UEdGraphPin* Pin : Node->Pins)
		{
			if (Pin && Pin->Direction == EGPD_Output)
			{
				if (CurrentOutputIndex == OutputPinIndex)
				{
					return Pin;
				}
				++CurrentOutputIndex;
			}
		}

		return nullptr;
	}

	void ResetSnippetChoiceIds(TArray<FDQSDialogueChoice>& Choices)
	{
		for (FDQSDialogueChoice& Choice : Choices)
		{
			if (!Choice.ChoiceId.IsValid())
			{
				Choice.ChoiceId = FGuid::NewGuid();
			}
		}
	}

	FText DialogueNodeTypeName(const EDQSDialogueNodeType NodeType)
	{
		return StaticEnum<EDQSDialogueNodeType>()->GetDisplayNameTextByValue(static_cast<int64>(NodeType));
	}

	FText QuestNodeTypeName(const EDQSQuestNodeType NodeType)
	{
		return StaticEnum<EDQSQuestNodeType>()->GetDisplayNameTextByValue(static_cast<int64>(NodeType));
	}

	void AddIssue(TArray<FDQSValidationIssue>& OutIssues, const EDQSValidationSeverity Severity, const FText& Title, const FText& Message, const FText& Guidance, const FGuid& NodeId = FGuid(), const bool bCanFix = false)
	{
		FDQSValidationIssue& Issue = OutIssues.AddDefaulted_GetRef();
		Issue.Severity = Severity;
		Issue.Title = Title;
		Issue.Message = Message;
		Issue.Guidance = Guidance;
		Issue.NodeId = NodeId;
		Issue.bCanFix = bCanFix;
	}

	FText GetQuestDisplayTitle(const UQuestGraphAsset* Asset)
	{
		return Asset && !Asset->Metadata.DisplayName.IsEmpty() ? Asset->Metadata.DisplayName : FText::FromString(TEXT("Quest"));
	}

	void ResetGraphNodes(UEdGraph* Graph)
	{
		if (!Graph)
		{
			return;
		}

		Graph->Modify();
		for (int32 Index = Graph->Nodes.Num() - 1; Index >= 0; --Index)
		{
			if (UEdGraphNode* Node = Graph->Nodes[Index])
			{
				Node->DestroyNode();
			}
		}
	}

	void ConfigureEditorDefaults(UDialogueGraphAsset* Asset)
	{
		if (!Asset)
		{
			return;
		}

#if WITH_EDITORONLY_DATA
		Asset->bUseSimpleEditorMode = true;
		Asset->bShowGuidedHelp = true;
#endif
	}

	void ConfigureEditorDefaults(UQuestGraphAsset* Asset)
	{
		if (!Asset)
		{
			return;
		}

#if WITH_EDITORONLY_DATA
		Asset->bUseSimpleEditorMode = true;
		Asset->bShowGuidedHelp = true;
#endif
	}

	void AddStarterComment(UEdGraph* Graph, const FVector2D& Location, const FVector2D& Size, const FString& Title, const FString& Body, const FLinearColor& Color)
	{
		FDQSGraphEditorUtils::CreateCommentNode(
			Graph,
			Location,
			Size,
			FText::FromString(Title),
			Color,
			FString::Printf(TEXT("%s\n\n%s"), *Title, *Body));
	}

}

bool FDQSGraphEditorUtils::IsSimpleMode(const UObject* Asset)
{
	if (const UDialogueGraphAsset* DialogueAsset = Cast<UDialogueGraphAsset>(Asset))
	{
#if WITH_EDITORONLY_DATA
		return DialogueAsset->bUseSimpleEditorMode;
#endif
	}

	if (const UQuestGraphAsset* QuestAsset = Cast<UQuestGraphAsset>(Asset))
	{
#if WITH_EDITORONLY_DATA
		return QuestAsset->bUseSimpleEditorMode;
#endif
	}

	return true;
}

void FDQSGraphEditorUtils::SetSimpleMode(UObject* Asset, const bool bUseSimpleMode)
{
	if (UDialogueGraphAsset* DialogueAsset = Cast<UDialogueGraphAsset>(Asset))
	{
#if WITH_EDITORONLY_DATA
		DialogueAsset->Modify();
		DialogueAsset->bUseSimpleEditorMode = bUseSimpleMode;
#endif
	}
	else if (UQuestGraphAsset* QuestAsset = Cast<UQuestGraphAsset>(Asset))
	{
#if WITH_EDITORONLY_DATA
		QuestAsset->Modify();
		QuestAsset->bUseSimpleEditorMode = bUseSimpleMode;
#endif
	}
}

bool FDQSGraphEditorUtils::IsGuidedHelpEnabled(const UObject* Asset)
{
	if (const UDialogueGraphAsset* DialogueAsset = Cast<UDialogueGraphAsset>(Asset))
	{
#if WITH_EDITORONLY_DATA
		return DialogueAsset->bShowGuidedHelp;
#endif
	}

	if (const UQuestGraphAsset* QuestAsset = Cast<UQuestGraphAsset>(Asset))
	{
#if WITH_EDITORONLY_DATA
		return QuestAsset->bShowGuidedHelp;
#endif
	}

	return true;
}

void FDQSGraphEditorUtils::SetGuidedHelpEnabled(UObject* Asset, const bool bEnabled)
{
	if (UDialogueGraphAsset* DialogueAsset = Cast<UDialogueGraphAsset>(Asset))
	{
#if WITH_EDITORONLY_DATA
		DialogueAsset->Modify();
		DialogueAsset->bShowGuidedHelp = bEnabled;
#endif
	}
	else if (UQuestGraphAsset* QuestAsset = Cast<UQuestGraphAsset>(Asset))
	{
#if WITH_EDITORONLY_DATA
		QuestAsset->Modify();
		QuestAsset->bShowGuidedHelp = bEnabled;
#endif
	}
}

UDQSDialogueEdGraphNode* FDQSGraphEditorUtils::CreateDialogueNode(UEdGraph* Graph, const FVector2D& Location, const EDQSDialogueNodeType NodeType, const FText& Title)
{
	if (!Graph)
	{
		return nullptr;
	}

	UDQSDialogueEdGraphNode* Node = NewObject<UDQSDialogueEdGraphNode>(Graph, UDQSDialogueEdGraphNode::StaticClass(), NAME_None, RF_Transactional);
	Node->CreateNewGuid();
	Node->NodePosX = Location.X;
	Node->NodePosY = Location.Y;
	Node->NodeData.NodeId = FGuid::NewGuid();
	Node->NodeData.NodeType = NodeType;
	Node->NodeData.Title = Title.IsEmpty() ? DialogueNodeTypeName(NodeType) : Title;
	if (NodeType == EDQSDialogueNodeType::Choice)
	{
		Node->NodeData.Choices.SetNum(2);
		ResetSnippetChoiceIds(Node->NodeData.Choices);
	}

	Graph->AddNode(Node, true, false);
	Node->AllocateDefaultPins();
	Node->AutowireNewNode(nullptr);
	Node->SnapToGrid(16);
	return Node;
}

UDQSQuestEdGraphNode* FDQSGraphEditorUtils::CreateQuestNode(UEdGraph* Graph, const FVector2D& Location, const EDQSQuestNodeType NodeType, const FText& Title)
{
	if (!Graph)
	{
		return nullptr;
	}

	UDQSQuestEdGraphNode* Node = NewObject<UDQSQuestEdGraphNode>(Graph, UDQSQuestEdGraphNode::StaticClass(), NAME_None, RF_Transactional);
	Node->CreateNewGuid();
	Node->NodePosX = Location.X;
	Node->NodePosY = Location.Y;
	Node->NodeData.NodeId = FGuid::NewGuid();
	Node->NodeData.NodeType = NodeType;
	Node->NodeData.Title = Title.IsEmpty() ? QuestNodeTypeName(NodeType) : Title;

	Graph->AddNode(Node, true, false);
	Node->AllocateDefaultPins();
	Node->AutowireNewNode(nullptr);
	Node->SnapToGrid(16);
	return Node;
}

UEdGraphNode_Comment* FDQSGraphEditorUtils::CreateCommentNode(UEdGraph* Graph, const FVector2D& Location, const FVector2D& Size, const FText& Title, const FLinearColor& Color, const FString& CommentText)
{
	if (!Graph)
	{
		return nullptr;
	}

	UEdGraphNode_Comment* CommentNode = NewObject<UEdGraphNode_Comment>(Graph, UEdGraphNode_Comment::StaticClass(), NAME_None, RF_Transactional);
	CommentNode->CreateNewGuid();
	CommentNode->NodePosX = Location.X;
	CommentNode->NodePosY = Location.Y;
	CommentNode->NodeWidth = Size.X;
	CommentNode->NodeHeight = Size.Y;
	CommentNode->CommentColor = Color;
	CommentNode->NodeComment = CommentText;
	CommentNode->OnRenameNode(Title.ToString());
	Graph->AddNode(CommentNode, true, false);
	CommentNode->SnapToGrid(16);
	return CommentNode;
}

void FDQSGraphEditorUtils::ConnectNodes(UEdGraphNode* FromNode, const int32 OutputPinIndex, UEdGraphNode* ToNode)
{
	UEdGraphPin* OutputPin = GetOutputPin(FromNode, OutputPinIndex);
	UEdGraphPin* InputPin = GetInputPin(ToNode);
	if (OutputPin && InputPin)
	{
		OutputPin->MakeLinkTo(InputPin);
	}
}

void FDQSGraphEditorUtils::ScaffoldDialogueAsset(UDialogueGraphAsset* Asset, const FDQSDialogueCreationOptions& Options)
{
	if (!Asset || !Asset->EditorGraph)
	{
		return;
	}

	Asset->Modify();
	Asset->Metadata.DisplayName = Options.DisplayName.IsEmpty() ? Asset->Metadata.DisplayName : Options.DisplayName;
	Asset->Metadata.Description = FText::FromString(TEXT("Starter dialogue scaffold generated by DialogQuestSystem."));
	Asset->Metadata.AuthorNotes = FText::FromString(TEXT("Guided Mode starts with editable comments in the graph. Replace the starter text before shipping."));
	Asset->Metadata.GraphId = Asset->Metadata.GraphId.IsNone() ? FName(*FString::Printf(TEXT("Dialogue.%s"), *Asset->GetName())) : Asset->Metadata.GraphId;
	Asset->Speakers.Reset();
	ConfigureEditorDefaults(Asset);

	FDQSSpeakerDefinition& Speaker = Asset->Speakers.AddDefaulted_GetRef();
	Speaker.SpeakerId = Options.SpeakerId.IsNone() ? FName(TEXT("NPC")) : Options.SpeakerId;
	Speaker.DisplayName = Options.SpeakerDisplayName.IsEmpty() ? FText::FromName(Speaker.SpeakerId) : Options.SpeakerDisplayName;

	ResetGraphNodes(Asset->EditorGraph);

	AddStarterComment(
		Asset->EditorGraph,
		FVector2D(-240.0f, -200.0f),
		FVector2D(1520.0f, 180.0f),
		TEXT("How To Use This Starter"),
		TEXT("Edit the text inside the named nodes, then expand the graph from the existing pins. Guided Mode keeps the palette small on purpose. Switch to Full Editor if you need conditioned branches, set-condition nodes, or jumps."),
		FLinearColor(0.16f, 0.22f, 0.32f, 1.0f));

	UDQSDialogueEdGraphNode* Entry = CreateDialogueNode(Asset->EditorGraph, FVector2D(0.0f, 56.0f), EDQSDialogueNodeType::Entry, FText::FromString(TEXT("Entry")));
	UDQSDialogueEdGraphNode* Speech = CreateDialogueNode(Asset->EditorGraph, FVector2D(280.0f, 56.0f), EDQSDialogueNodeType::Speech, FText::FromString(TEXT("Greeting")));
	Speech->NodeData.SpeakerId = Speaker.SpeakerId;
	Speech->NodeData.Text = FText::FromString(TEXT("Welcome. Edit this first line so it matches your NPC."));

	AddStarterComment(
		Asset->EditorGraph,
		FVector2D(180.0f, -24.0f),
		FVector2D(360.0f, 180.0f),
		TEXT("Step 1"),
		TEXT("Start here. Select the Greeting node and replace the sample text. If this character should use a different speaker entry, change Speaker Id in Details."),
		FLinearColor(0.39f, 0.25f, 0.12f, 1.0f));

	ConnectNodes(Entry, 0, Speech);

	if (Options.Pattern == EDQSDialogueStarterPattern::LinearConversation)
	{
		UDQSDialogueEdGraphNode* FollowUp = CreateDialogueNode(Asset->EditorGraph, FVector2D(560.0f, 56.0f), EDQSDialogueNodeType::Speech, FText::FromString(TEXT("Follow Up")));
		FollowUp->NodeData.SpeakerId = Speaker.SpeakerId;
		FollowUp->NodeData.Text = FText::FromString(TEXT("Add a second line here, or replace this node with something else."));

		UDQSDialogueEdGraphNode* End = CreateDialogueNode(Asset->EditorGraph, FVector2D(860.0f, 56.0f), EDQSDialogueNodeType::End, FText::FromString(TEXT("End")));
		ConnectNodes(Speech, 0, FollowUp);
		ConnectNodes(FollowUp, 0, End);

		AddStarterComment(
			Asset->EditorGraph,
			FVector2D(500.0f, -24.0f),
			FVector2D(480.0f, 180.0f),
			TEXT("Step 2"),
			TEXT("This starter is intentionally simple: one more line, then an End node. Keep it linear while you're learning, or branch it later with a Choice node."),
			FLinearColor(0.18f, 0.23f, 0.18f, 1.0f));
	}
	else
	{
		UDQSDialogueEdGraphNode* Choice = CreateDialogueNode(Asset->EditorGraph, FVector2D(600.0f, 56.0f), EDQSDialogueNodeType::Choice, FText::FromString(TEXT("Offer Choices")));
		Choice->NodeData.Text = FText::FromString(TEXT("Edit the player-facing response text below."));
		Choice->NodeData.Choices[0].Text = FText::FromString(TEXT("Yes, I can help."));
		Choice->NodeData.Choices[1].Text = FText::FromString(TEXT("Not right now."));
		Choice->ReconstructNode();

		AddStarterComment(
			Asset->EditorGraph,
			FVector2D(520.0f, -56.0f),
			FVector2D(420.0f, 220.0f),
			TEXT("Step 2"),
			TEXT("Choice outputs map directly to the rows in the Choices array. Edit each choice row and keep every output connected so the branch is playable."),
			FLinearColor(0.14f, 0.20f, 0.30f, 1.0f));

		ConnectNodes(Speech, 0, Choice);

		if (Options.Pattern == EDQSDialogueStarterPattern::AcceptDecline)
		{
			UDQSDialogueEdGraphNode* QuestHook = CreateDialogueNode(Asset->EditorGraph, FVector2D(980.0f, -64.0f), EDQSDialogueNodeType::QuestHook, FText::FromString(TEXT("Accept Quest")));
			QuestHook->NodeData.QuestHook.Operation = EDQSQuestOperation::StartQuest;
			UDQSDialogueEdGraphNode* AcceptedSpeech = CreateDialogueNode(Asset->EditorGraph, FVector2D(1240.0f, -64.0f), EDQSDialogueNodeType::Speech, FText::FromString(TEXT("Quest Accepted")));
			AcceptedSpeech->NodeData.SpeakerId = Speaker.SpeakerId;
			AcceptedSpeech->NodeData.Text = FText::FromString(TEXT("Great. I'll mark that quest as started here."));

			UDQSDialogueEdGraphNode* DeclinedSpeech = CreateDialogueNode(Asset->EditorGraph, FVector2D(980.0f, 192.0f), EDQSDialogueNodeType::Speech, FText::FromString(TEXT("Decline Path")));
			DeclinedSpeech->NodeData.SpeakerId = Speaker.SpeakerId;
			DeclinedSpeech->NodeData.Text = FText::FromString(TEXT("No problem. Come back if you change your mind."));

			UDQSDialogueEdGraphNode* EndAccepted = CreateDialogueNode(Asset->EditorGraph, FVector2D(1520.0f, -64.0f), EDQSDialogueNodeType::End, FText::FromString(TEXT("End")));
			UDQSDialogueEdGraphNode* EndDeclined = CreateDialogueNode(Asset->EditorGraph, FVector2D(1280.0f, 192.0f), EDQSDialogueNodeType::End, FText::FromString(TEXT("End")));

			ConnectNodes(Choice, 0, QuestHook);
			ConnectNodes(QuestHook, 0, AcceptedSpeech);
			ConnectNodes(AcceptedSpeech, 0, EndAccepted);
			ConnectNodes(Choice, 1, DeclinedSpeech);
			ConnectNodes(DeclinedSpeech, 0, EndDeclined);

			AddStarterComment(
				Asset->EditorGraph,
				FVector2D(920.0f, -152.0f),
				FVector2D(700.0f, 240.0f),
				TEXT("Step 3"),
				TEXT("The upper branch is the quest handoff. Pick the quest asset on the Quest Hook node and keep the follow-up speech so the player gets confirmation."),
				FLinearColor(0.15f, 0.27f, 0.18f, 1.0f));

			AddStarterComment(
				Asset->EditorGraph,
				FVector2D(920.0f, 120.0f),
				FVector2D(460.0f, 180.0f),
				TEXT("Optional"),
				TEXT("The decline path can be short. You can end it here, loop back later with another branch, or remove it in Full Editor if your design changes."),
				FLinearColor(0.27f, 0.19f, 0.12f, 1.0f));
		}
		else
		{
			UDQSDialogueEdGraphNode* AcceptSpeech = CreateDialogueNode(Asset->EditorGraph, FVector2D(980.0f, -80.0f), EDQSDialogueNodeType::Speech, FText::FromString(TEXT("Accept Path")));
			AcceptSpeech->NodeData.SpeakerId = Speaker.SpeakerId;
			AcceptSpeech->NodeData.Text = FText::FromString(TEXT("This is the positive branch. Replace it with your follow-up line."));

			UDQSDialogueEdGraphNode* DeclineSpeech = CreateDialogueNode(Asset->EditorGraph, FVector2D(980.0f, 192.0f), EDQSDialogueNodeType::Speech, FText::FromString(TEXT("Alternate Path")));
			DeclineSpeech->NodeData.SpeakerId = Speaker.SpeakerId;
			DeclineSpeech->NodeData.Text = FText::FromString(TEXT("This is the alternate branch. Replace it with the other outcome."));

			UDQSDialogueEdGraphNode* EndAccepted = CreateDialogueNode(Asset->EditorGraph, FVector2D(1320.0f, -80.0f), EDQSDialogueNodeType::End, FText::FromString(TEXT("End")));
			UDQSDialogueEdGraphNode* EndDeclined = CreateDialogueNode(Asset->EditorGraph, FVector2D(1320.0f, 192.0f), EDQSDialogueNodeType::End, FText::FromString(TEXT("End")));

			ConnectNodes(Choice, 0, AcceptSpeech);
			ConnectNodes(Choice, 1, DeclineSpeech);
			ConnectNodes(AcceptSpeech, 0, EndAccepted);
			ConnectNodes(DeclineSpeech, 0, EndDeclined);

			AddStarterComment(
				Asset->EditorGraph,
				FVector2D(920.0f, -152.0f),
				FVector2D(520.0f, 420.0f),
				TEXT("Step 3"),
				TEXT("Each output is a real conversation branch. Replace both follow-up speeches so the player sees a clear consequence for each choice."),
				FLinearColor(0.16f, 0.24f, 0.20f, 1.0f));
		}
	}

	Asset->EditorGraph->NotifyGraphChanged();
	FDQSGraphCompiler::CompileDialogueAsset(Asset);
}

void FDQSGraphEditorUtils::ScaffoldQuestAsset(UQuestGraphAsset* Asset, const FDQSQuestCreationOptions& Options)
{
	if (!Asset || !Asset->EditorGraph)
	{
		return;
	}

	Asset->Modify();
	Asset->Metadata.DisplayName = Options.DisplayName.IsEmpty() ? Asset->Metadata.DisplayName : Options.DisplayName;
	Asset->Metadata.Description = FText::FromString(TEXT("Starter quest scaffold generated by DialogQuestSystem."));
	Asset->Metadata.AuthorNotes = FText::FromString(TEXT("Objective nodes need clear text and a gameplay tag so Blueprint events can advance them."));
	Asset->Metadata.GraphId = Asset->Metadata.GraphId.IsNone() ? FName(*FString::Printf(TEXT("Quest.%s"), *Asset->GetName())) : Asset->Metadata.GraphId;
	ConfigureEditorDefaults(Asset);

	ResetGraphNodes(Asset->EditorGraph);

	AddStarterComment(
		Asset->EditorGraph,
		FVector2D(-220.0f, -200.0f),
		FVector2D(1400.0f, 180.0f),
		TEXT("How To Use This Starter"),
		TEXT("Keep Start connected, give the first Objective clear player text, and set an Objective Tag that your Blueprint events will advance. Guided Mode keeps the quest graph intentionally narrow until you need branching."),
		FLinearColor(0.14f, 0.23f, 0.23f, 1.0f));

	UDQSQuestEdGraphNode* Start = CreateQuestNode(Asset->EditorGraph, FVector2D(0.0f, 56.0f), EDQSQuestNodeType::Start, FText::FromString(TEXT("Start")));
	UDQSQuestEdGraphNode* Objective = CreateQuestNode(Asset->EditorGraph, FVector2D(280.0f, 56.0f), EDQSQuestNodeType::Objective, FText::FromString(TEXT("Objective")));
	Objective->NodeData.Description = Options.ObjectiveText;
	Objective->NodeData.ObjectiveTag = FGameplayTag::RequestGameplayTag(Options.ObjectiveTag, false);
	Objective->NodeData.TargetCount = 1;
	ConnectNodes(Start, 0, Objective);

	AddStarterComment(
		Asset->EditorGraph,
		FVector2D(180.0f, -32.0f),
		FVector2D(380.0f, 190.0f),
		TEXT("Step 1"),
		TEXT("Select the Objective node and replace the sample text. The Objective Tag is how gameplay tells this quest step that progress happened."),
		FLinearColor(0.22f, 0.28f, 0.16f, 1.0f));

	if (Options.Pattern == EDQSQuestStarterPattern::BranchingQuest)
	{
		UDQSQuestEdGraphNode* Branch = CreateQuestNode(Asset->EditorGraph, FVector2D(620.0f, 56.0f), EDQSQuestNodeType::Branch, FText::FromString(TEXT("Outcome Split")));
		UDQSQuestEdGraphNode* Reward = CreateQuestNode(Asset->EditorGraph, FVector2D(920.0f, -72.0f), EDQSQuestNodeType::Reward, FText::FromString(TEXT("Reward")));
		Reward->NodeData.Description = FText::FromString(TEXT("Optional reward or hand-in step."));
		UDQSQuestEdGraphNode* Complete = CreateQuestNode(Asset->EditorGraph, FVector2D(1220.0f, -72.0f), EDQSQuestNodeType::Complete, FText::FromString(TEXT("Complete")));
		UDQSQuestEdGraphNode* Fail = CreateQuestNode(Asset->EditorGraph, FVector2D(920.0f, 192.0f), EDQSQuestNodeType::Fail, FText::FromString(TEXT("Fail")));

		ConnectNodes(Objective, 0, Branch);
		ConnectNodes(Branch, 0, Reward);
		ConnectNodes(Reward, 0, Complete);
		ConnectNodes(Branch, 1, Fail);

		AddStarterComment(
			Asset->EditorGraph,
			FVector2D(560.0f, -136.0f),
			FVector2D(760.0f, 420.0f),
			TEXT("Step 2"),
			TEXT("Use Branch when the quest can end in two different ways. In Guided Mode the primary path is your main outcome and the alternate path is the backup or fail state."),
			FLinearColor(0.16f, 0.20f, 0.30f, 1.0f));
	}
	else if (Options.Pattern == EDQSQuestStarterPattern::AcceptAndCompleteLoop)
	{
		UDQSQuestEdGraphNode* Reward = CreateQuestNode(Asset->EditorGraph, FVector2D(620.0f, 56.0f), EDQSQuestNodeType::Reward, FText::FromString(TEXT("Hand-In Reward")));
		Reward->NodeData.Description = FText::FromString(TEXT("Use this node for the turn-in or reward moment."));
		UDQSQuestEdGraphNode* Complete = CreateQuestNode(Asset->EditorGraph, FVector2D(920.0f, 56.0f), EDQSQuestNodeType::Complete, FText::FromString(TEXT("Complete")));
		ConnectNodes(Objective, 0, Reward);
		ConnectNodes(Reward, 0, Complete);

		AddStarterComment(
			Asset->EditorGraph,
			FVector2D(560.0f, -32.0f),
			FVector2D(520.0f, 190.0f),
			TEXT("Step 2"),
			TEXT("This template works well for quests accepted in dialogue: objective first, then a reward or turn-in beat, then Complete."),
			FLinearColor(0.16f, 0.24f, 0.18f, 1.0f));
	}
	else
	{
		UDQSQuestEdGraphNode* Complete = CreateQuestNode(Asset->EditorGraph, FVector2D(620.0f, 56.0f), EDQSQuestNodeType::Complete, FText::FromString(TEXT("Complete")));
		ConnectNodes(Objective, 0, Complete);

		AddStarterComment(
			Asset->EditorGraph,
			FVector2D(560.0f, -32.0f),
			FVector2D(320.0f, 190.0f),
			TEXT("Step 2"),
			TEXT("A first quest can be this small. Keep it simple until the objective and quest log flow feel solid in game."),
			FLinearColor(0.18f, 0.24f, 0.16f, 1.0f));
	}

	Asset->EditorGraph->NotifyGraphChanged();
	FDQSGraphCompiler::CompileQuestAsset(Asset);
}

void FDQSGraphEditorUtils::InsertDialogueSnippet(UDialogueGraphAsset* Asset, const EDQSDialogueSnippet Snippet)
{
	if (!Asset || !Asset->EditorGraph)
	{
		return;
	}

	const float BaseX = 240.0f * Asset->EditorGraph->Nodes.Num();
	if (Snippet == EDQSDialogueSnippet::QuestAcceptance)
	{
		AddStarterComment(
			Asset->EditorGraph,
			FVector2D(BaseX - 100.0f, 380.0f),
			FVector2D(900.0f, 220.0f),
			TEXT("Inserted Guided Template"),
			TEXT("This pattern handles a quest offer with accept/decline responses. Connect the first node from your current graph into the leftmost speech if needed."),
			FLinearColor(0.15f, 0.23f, 0.30f, 1.0f));

		UDQSDialogueEdGraphNode* Speech = CreateDialogueNode(Asset->EditorGraph, FVector2D(BaseX, 460.0f), EDQSDialogueNodeType::Speech, FText::FromString(TEXT("Quest Offer")));
		Speech->NodeData.Text = FText::FromString(TEXT("Would you like to help me?"));
		UDQSDialogueEdGraphNode* Choice = CreateDialogueNode(Asset->EditorGraph, FVector2D(BaseX + 280.0f, 460.0f), EDQSDialogueNodeType::Choice, FText::FromString(TEXT("Accept Or Decline")));
		Choice->NodeData.Choices[0].Text = FText::FromString(TEXT("Yes, tell me more."));
		Choice->NodeData.Choices[1].Text = FText::FromString(TEXT("No, not right now."));
		Choice->ReconstructNode();
		UDQSDialogueEdGraphNode* Hook = CreateDialogueNode(Asset->EditorGraph, FVector2D(BaseX + 620.0f, 380.0f), EDQSDialogueNodeType::QuestHook, FText::FromString(TEXT("Start Quest")));
		Hook->NodeData.QuestHook.Operation = EDQSQuestOperation::StartQuest;
		UDQSDialogueEdGraphNode* AcceptedSpeech = CreateDialogueNode(Asset->EditorGraph, FVector2D(BaseX + 920.0f, 380.0f), EDQSDialogueNodeType::Speech, FText::FromString(TEXT("Accepted")));
		AcceptedSpeech->NodeData.Text = FText::FromString(TEXT("Perfect. Your quest begins now."));
		UDQSDialogueEdGraphNode* DeclinedSpeech = CreateDialogueNode(Asset->EditorGraph, FVector2D(BaseX + 620.0f, 560.0f), EDQSDialogueNodeType::Speech, FText::FromString(TEXT("Declined")));
		DeclinedSpeech->NodeData.Text = FText::FromString(TEXT("Come back if you change your mind."));
		UDQSDialogueEdGraphNode* EndA = CreateDialogueNode(Asset->EditorGraph, FVector2D(BaseX + 1180.0f, 380.0f), EDQSDialogueNodeType::End, FText::FromString(TEXT("End")));
		UDQSDialogueEdGraphNode* EndB = CreateDialogueNode(Asset->EditorGraph, FVector2D(BaseX + 920.0f, 560.0f), EDQSDialogueNodeType::End, FText::FromString(TEXT("End")));
		ConnectNodes(Speech, 0, Choice);
		ConnectNodes(Choice, 0, Hook);
		ConnectNodes(Hook, 0, AcceptedSpeech);
		ConnectNodes(AcceptedSpeech, 0, EndA);
		ConnectNodes(Choice, 1, DeclinedSpeech);
		ConnectNodes(DeclinedSpeech, 0, EndB);
	}
	else
	{
		AddStarterComment(
			Asset->EditorGraph,
			FVector2D(BaseX - 100.0f, 380.0f),
			FVector2D(780.0f, 220.0f),
			TEXT("Inserted Guided Template"),
			TEXT("This pattern is a reusable two-choice conversation beat. Replace the speeches, then connect it into the rest of your graph."),
			FLinearColor(0.18f, 0.24f, 0.18f, 1.0f));

		UDQSDialogueEdGraphNode* Speech = CreateDialogueNode(Asset->EditorGraph, FVector2D(BaseX, 460.0f), EDQSDialogueNodeType::Speech, FText::FromString(TEXT("Starter Speech")));
		Speech->NodeData.Text = FText::FromString(TEXT("This is the new conversation beat."));
		UDQSDialogueEdGraphNode* Choice = CreateDialogueNode(Asset->EditorGraph, FVector2D(BaseX + 260.0f, 460.0f), EDQSDialogueNodeType::Choice, FText::FromString(TEXT("Player Choice")));
		Choice->NodeData.Choices[0].Text = FText::FromString(TEXT("Choice 1"));
		Choice->NodeData.Choices[1].Text = FText::FromString(TEXT("Choice 2"));
		Choice->ReconstructNode();
		UDQSDialogueEdGraphNode* BranchA = CreateDialogueNode(Asset->EditorGraph, FVector2D(BaseX + 560.0f, 380.0f), EDQSDialogueNodeType::Speech, FText::FromString(TEXT("Choice 1 Result")));
		UDQSDialogueEdGraphNode* BranchB = CreateDialogueNode(Asset->EditorGraph, FVector2D(BaseX + 560.0f, 560.0f), EDQSDialogueNodeType::Speech, FText::FromString(TEXT("Choice 2 Result")));
		UDQSDialogueEdGraphNode* EndA = CreateDialogueNode(Asset->EditorGraph, FVector2D(BaseX + 860.0f, 380.0f), EDQSDialogueNodeType::End, FText::FromString(TEXT("End")));
		UDQSDialogueEdGraphNode* EndB = CreateDialogueNode(Asset->EditorGraph, FVector2D(BaseX + 860.0f, 560.0f), EDQSDialogueNodeType::End, FText::FromString(TEXT("End")));
		ConnectNodes(Speech, 0, Choice);
		ConnectNodes(Choice, 0, BranchA);
		ConnectNodes(Choice, 1, BranchB);
		ConnectNodes(BranchA, 0, EndA);
		ConnectNodes(BranchB, 0, EndB);
	}

	Asset->EditorGraph->NotifyGraphChanged();
	FDQSGraphCompiler::CompileDialogueAsset(Asset);
}

void FDQSGraphEditorUtils::InsertQuestSnippet(UQuestGraphAsset* Asset, const EDQSQuestSnippet Snippet)
{
	if (!Asset || !Asset->EditorGraph)
	{
		return;
	}

	const float BaseX = 240.0f * Asset->EditorGraph->Nodes.Num();
	if (Snippet == EDQSQuestSnippet::BranchingObjective)
	{
		AddStarterComment(
			Asset->EditorGraph,
			FVector2D(BaseX - 100.0f, 380.0f),
			FVector2D(880.0f, 220.0f),
			TEXT("Inserted Guided Template"),
			TEXT("This pattern adds a branch after an objective so you can model success vs alternate outcome."),
			FLinearColor(0.15f, 0.22f, 0.29f, 1.0f));

		UDQSQuestEdGraphNode* Objective = CreateQuestNode(Asset->EditorGraph, FVector2D(BaseX, 460.0f), EDQSQuestNodeType::Objective, FText::FromString(TEXT("Objective")));
		Objective->NodeData.Description = FText::FromString(TEXT("Describe the new objective here."));
		UDQSQuestEdGraphNode* Branch = CreateQuestNode(Asset->EditorGraph, FVector2D(BaseX + 300.0f, 460.0f), EDQSQuestNodeType::Branch, FText::FromString(TEXT("Branch")));
		UDQSQuestEdGraphNode* Complete = CreateQuestNode(Asset->EditorGraph, FVector2D(BaseX + 620.0f, 380.0f), EDQSQuestNodeType::Complete, FText::FromString(TEXT("Complete")));
		UDQSQuestEdGraphNode* Fail = CreateQuestNode(Asset->EditorGraph, FVector2D(BaseX + 620.0f, 560.0f), EDQSQuestNodeType::Fail, FText::FromString(TEXT("Fail")));
		ConnectNodes(Objective, 0, Branch);
		ConnectNodes(Branch, 0, Complete);
		ConnectNodes(Branch, 1, Fail);
	}
	else
	{
		AddStarterComment(
			Asset->EditorGraph,
			FVector2D(BaseX - 100.0f, 380.0f),
			FVector2D(760.0f, 220.0f),
			TEXT("Inserted Guided Template"),
			TEXT("This pattern adds a clean objective-to-complete loop. It works well for a small add-on quest beat."),
			FLinearColor(0.17f, 0.24f, 0.17f, 1.0f));

		UDQSQuestEdGraphNode* Objective = CreateQuestNode(Asset->EditorGraph, FVector2D(BaseX, 460.0f), EDQSQuestNodeType::Objective, FText::FromString(TEXT("Objective")));
		Objective->NodeData.Description = FText::FromString(TEXT("Describe the objective here."));
		UDQSQuestEdGraphNode* Complete = CreateQuestNode(Asset->EditorGraph, FVector2D(BaseX + 320.0f, 460.0f), EDQSQuestNodeType::Complete, FText::FromString(TEXT("Complete")));
		ConnectNodes(Objective, 0, Complete);
	}

	Asset->EditorGraph->NotifyGraphChanged();
	FDQSGraphCompiler::CompileQuestAsset(Asset);
}

void FDQSGraphEditorUtils::BuildDialogueValidationIssues(UDialogueGraphAsset* Asset, TArray<FDQSValidationIssue>& OutIssues)
{
	OutIssues.Reset();
	if (!Asset)
	{
		return;
	}

	FDQSGraphCompiler::CompileDialogueAsset(Asset);
	if (!Asset->EntryNodeId.IsValid())
	{
		AddIssue(OutIssues, EDQSValidationSeverity::Blocking, FText::FromString(TEXT("Missing Entry")), FText::FromString(TEXT("This dialogue has no Entry node.")), FText::FromString(TEXT("Keep exactly one Entry node so the conversation has a clear starting point.")));
	}

	bool bHasPlayerPath = false;
	for (const FDQSDialogueNode& Node : Asset->Nodes)
	{
		if (Node.NodeType == EDQSDialogueNodeType::Entry && !Node.NextNodeId.IsValid())
		{
			AddIssue(OutIssues, EDQSValidationSeverity::Blocking, FText::FromString(TEXT("Entry Not Connected")), FText::FromString(TEXT("The Entry node does not lead to anything.")), FText::FromString(TEXT("Connect Entry to the first speech, choice, or conditioned branch node.")), Node.NodeId);
		}

		if (Node.NodeType == EDQSDialogueNodeType::Speech)
		{
			if (Node.Text.IsEmpty())
			{
				AddIssue(OutIssues, EDQSValidationSeverity::Warning, FText::FromString(TEXT("Empty Speech Text")), FText::Format(FText::FromString(TEXT("Speech node '{0}' has no line text.")), Node.Title), FText::FromString(TEXT("Add the spoken line so beginners can preview the conversation in context.")), Node.NodeId);
			}
			if (!Node.SpeakerId.IsNone() && !Asset->Speakers.ContainsByPredicate([&Node](const FDQSSpeakerDefinition& Speaker) { return Speaker.SpeakerId == Node.SpeakerId; }))
			{
				AddIssue(OutIssues, EDQSValidationSeverity::Warning, FText::FromString(TEXT("Unknown Speaker")), FText::Format(FText::FromString(TEXT("Node '{0}' references speaker '{1}' that is not configured on the asset.")), Node.Title, FText::FromName(Node.SpeakerId)), FText::FromString(TEXT("Add that speaker in the asset details or change the speaker on the node.")), Node.NodeId, true);
			}
		}
		else if (Node.NodeType == EDQSDialogueNodeType::Choice)
		{
			bHasPlayerPath = true;
			if (Node.Choices.Num() < 2)
			{
				AddIssue(OutIssues, EDQSValidationSeverity::Blocking, FText::FromString(TEXT("Choice Needs More Branches")), FText::Format(FText::FromString(TEXT("Choice node '{0}' only has one response.")), Node.Title), FText::FromString(TEXT("Add at least a second player response so the choice feels meaningful.")), Node.NodeId, true);
			}
			for (const FDQSDialogueChoice& Choice : Node.Choices)
			{
				if (Choice.Text.IsEmpty())
				{
					AddIssue(OutIssues, EDQSValidationSeverity::Warning, FText::FromString(TEXT("Empty Choice Text")), FText::Format(FText::FromString(TEXT("Choice node '{0}' has a response with no visible text.")), Node.Title), FText::FromString(TEXT("Write response text so players understand what they are choosing.")), Node.NodeId);
				}
				if (!Choice.TargetNodeId.IsValid())
				{
					AddIssue(OutIssues, EDQSValidationSeverity::Blocking, FText::FromString(TEXT("Unconnected Choice Branch")), FText::Format(FText::FromString(TEXT("Choice node '{0}' has an option that does not lead anywhere.")), Node.Title), FText::FromString(TEXT("Connect every choice output to a follow-up node or an End node.")), Node.NodeId);
				}
			}
		}
		else if (Node.NodeType == EDQSDialogueNodeType::ConditionedStart)
		{
			if (Node.Conditions.IsEmpty())
			{
				AddIssue(OutIssues, EDQSValidationSeverity::Warning, FText::FromString(TEXT("Conditioned Branch Has No Conditions")), FText::Format(FText::FromString(TEXT("Conditioned branch node '{0}' has no conditions and will always take the matched branch.")), Node.Title), FText::FromString(TEXT("Add at least one condition, or replace this with a normal direct connection if branching is not needed.")), Node.NodeId);
			}
			if (!Node.NextNodeId.IsValid())
			{
				AddIssue(OutIssues, EDQSValidationSeverity::Blocking, FText::FromString(TEXT("Conditioned Branch Missing Matched Path")), FText::Format(FText::FromString(TEXT("Conditioned branch node '{0}' has no matched branch output connected.")), Node.Title), FText::FromString(TEXT("Connect the Matched Branch output to the dialogue path for players who meet the conditions.")), Node.NodeId);
			}
			if (!Node.AlternateNodeId.IsValid())
			{
				AddIssue(OutIssues, EDQSValidationSeverity::Warning, FText::FromString(TEXT("Conditioned Branch Missing Fallback Path")), FText::Format(FText::FromString(TEXT("Conditioned branch node '{0}' has no fallback branch output connected.")), Node.Title), FText::FromString(TEXT("Connect Fallback Branch so new or unmatched players still receive a valid path.")), Node.NodeId);
			}
		}
		else if (Node.NodeType == EDQSDialogueNodeType::QuestHook)
		{
			if (Node.QuestHook.Operation != EDQSQuestOperation::None && Node.QuestHook.QuestAsset.IsNull())
			{
				AddIssue(OutIssues, EDQSValidationSeverity::Warning, FText::FromString(TEXT("Quest Hook Missing Quest")), FText::Format(FText::FromString(TEXT("Quest hook node '{0}' has an operation but no quest asset assigned.")), Node.Title), FText::FromString(TEXT("Assign a quest asset so this dialogue action has a real target.")), Node.NodeId);
			}
		}
		else if (Node.NodeType != EDQSDialogueNodeType::End && Node.NodeType != EDQSDialogueNodeType::ConditionedStart && Node.NodeType != EDQSDialogueNodeType::Choice && Node.NodeType != EDQSDialogueNodeType::Jump && !Node.NextNodeId.IsValid())
		{
			AddIssue(OutIssues, EDQSValidationSeverity::Suggestion, FText::FromString(TEXT("Terminal Path Could End Explicitly")), FText::Format(FText::FromString(TEXT("Node '{0}' stops without an End node.")), Node.Title), FText::FromString(TEXT("Consider connecting the path to an End node to make exits clearer for beginners.")), Node.NodeId, true);
		}
	}

	if (!bHasPlayerPath)
	{
		AddIssue(OutIssues, EDQSValidationSeverity::Suggestion, FText::FromString(TEXT("No Player Choice Yet")), FText::FromString(TEXT("This dialogue has no choice nodes yet.")), FText::FromString(TEXT("That is fine for a linear conversation, but adding one simple choice is a good next step.")));
	}

	TArray<FText> CompilerMessages;
	FDQSGraphCompiler::ValidateDialogueAsset(Asset, CompilerMessages);
	for (const FText& Message : CompilerMessages)
	{
		AddIssue(OutIssues, EDQSValidationSeverity::Warning, FText::FromString(TEXT("Graph Validation")), Message, FText::FromString(TEXT("Reconnect or remove orphaned nodes so the flow is easy to follow.")));
	}
}

void FDQSGraphEditorUtils::BuildQuestValidationIssues(UQuestGraphAsset* Asset, TArray<FDQSValidationIssue>& OutIssues)
{
	OutIssues.Reset();
	if (!Asset)
	{
		return;
	}

	FDQSGraphCompiler::CompileQuestAsset(Asset);
	if (!Asset->EntryNodeId.IsValid())
	{
		AddIssue(OutIssues, EDQSValidationSeverity::Blocking, FText::FromString(TEXT("Missing Start")), FText::FromString(TEXT("This quest has no Start node.")), FText::FromString(TEXT("Keep exactly one Start node so the quest knows where to begin.")));
	}

	bool bHasCompletion = false;
	bool bHasFailure = false;
	for (const FDQSQuestNode& Node : Asset->Nodes)
	{
		if (Node.NodeType == EDQSQuestNodeType::Start && !Node.NextNodeId.IsValid())
		{
			AddIssue(OutIssues, EDQSValidationSeverity::Blocking, FText::FromString(TEXT("Start Not Connected")), FText::FromString(TEXT("The Start node does not lead anywhere.")), FText::FromString(TEXT("Connect Start to the first objective, branch, reward, or end-state node.")), Node.NodeId);
		}
		else if (Node.NodeType == EDQSQuestNodeType::Objective)
		{
			if (Node.Description.IsEmpty())
			{
				AddIssue(OutIssues, EDQSValidationSeverity::Warning, FText::FromString(TEXT("Objective Needs Description")), FText::Format(FText::FromString(TEXT("Objective node '{0}' has no player-facing description.")), Node.Title), FText::FromString(TEXT("Write objective text so the quest log can explain what the player should do.")), Node.NodeId);
			}
			if (!Node.ObjectiveTag.IsValid())
			{
				AddIssue(OutIssues, EDQSValidationSeverity::Blocking, FText::FromString(TEXT("Objective Missing Tag")), FText::Format(FText::FromString(TEXT("Objective node '{0}' has no objective tag.")), Node.Title), FText::FromString(TEXT("Set an objective tag so gameplay can advance this quest step.")), Node.NodeId, true);
			}
			if (Node.TargetCount <= 0)
			{
				AddIssue(OutIssues, EDQSValidationSeverity::Blocking, FText::FromString(TEXT("Invalid Target Count")), FText::Format(FText::FromString(TEXT("Objective node '{0}' needs a target count greater than zero.")), Node.Title), FText::FromString(TEXT("Most first objectives should use a target count of 1.")), Node.NodeId);
			}
		}
		else if (Node.NodeType == EDQSQuestNodeType::Complete)
		{
			bHasCompletion = true;
		}
		else if (Node.NodeType == EDQSQuestNodeType::Fail)
		{
			bHasFailure = true;
		}
	}

	if (!bHasCompletion)
	{
		AddIssue(OutIssues, EDQSValidationSeverity::Suggestion, FText::FromString(TEXT("No Complete Node")), FText::Format(FText::FromString(TEXT("{0} never reaches an explicit Complete node.")), GetQuestDisplayTitle(Asset)), FText::FromString(TEXT("Add a Complete node so the happy path has a visible ending.")), FGuid(), true);
	}
	if (!bHasFailure)
	{
		AddIssue(OutIssues, EDQSValidationSeverity::Suggestion, FText::FromString(TEXT("No Fail Node")), FText::Format(FText::FromString(TEXT("{0} has no fail path yet.")), GetQuestDisplayTitle(Asset)), FText::FromString(TEXT("That is okay for a first quest, but adding a fail route later makes branching clearer.")));
	}

	TArray<FText> CompilerMessages;
	FDQSGraphCompiler::ValidateQuestAsset(Asset, CompilerMessages);
	for (const FText& Message : CompilerMessages)
	{
		AddIssue(OutIssues, EDQSValidationSeverity::Warning, FText::FromString(TEXT("Graph Validation")), Message, FText::FromString(TEXT("Reconnect or remove orphaned nodes so the quest path stays readable.")));
	}
}

void FDQSGraphEditorUtils::BuildDialogueChecklist(UDialogueGraphAsset* Asset, TArray<FDQSChecklistItem>& OutItems)
{
	OutItems.Reset();
	if (!Asset)
	{
		return;
	}

	FDQSChecklistItem& RootItem = OutItems.AddDefaulted_GetRef();
	RootItem.Label = FText::FromString(TEXT("Connect Entry to the first playable node"));
	RootItem.bCompleted = Asset->Nodes.ContainsByPredicate([](const FDQSDialogueNode& Node) { return Node.NodeType == EDQSDialogueNodeType::Entry && Node.NextNodeId.IsValid(); });
	RootItem.Guidance = FText::FromString(TEXT("Drag from Entry.Next to a Speech, Choice, or Conditioned Branch node."));

	FDQSChecklistItem& SpeakerItem = OutItems.AddDefaulted_GetRef();
	SpeakerItem.Label = FText::FromString(TEXT("Add at least one speaker"));
	SpeakerItem.bCompleted = Asset->Speakers.Num() > 0;
	SpeakerItem.Guidance = FText::FromString(TEXT("Use the asset details panel to add a speaker ID and display name."));

	FDQSChecklistItem& LineItem = OutItems.AddDefaulted_GetRef();
	LineItem.Label = FText::FromString(TEXT("Write the first line of dialogue"));
	LineItem.bCompleted = Asset->Nodes.ContainsByPredicate([](const FDQSDialogueNode& Node) { return Node.NodeType == EDQSDialogueNodeType::Speech && !Node.Text.IsEmpty(); });
	LineItem.Guidance = FText::FromString(TEXT("Select a Speech node and write its Text."));

	FDQSChecklistItem& EndItem = OutItems.AddDefaulted_GetRef();
	EndItem.Label = FText::FromString(TEXT("Finish at least one path with End"));
	EndItem.bCompleted = Asset->Nodes.ContainsByPredicate([](const FDQSDialogueNode& Node) { return Node.NodeType == EDQSDialogueNodeType::End; });
	EndItem.Guidance = FText::FromString(TEXT("Add an End node to make the first playable path explicit."));
}

void FDQSGraphEditorUtils::BuildQuestChecklist(UQuestGraphAsset* Asset, TArray<FDQSChecklistItem>& OutItems)
{
	OutItems.Reset();
	if (!Asset)
	{
		return;
	}

	FDQSChecklistItem& RootItem = OutItems.AddDefaulted_GetRef();
	RootItem.Label = FText::FromString(TEXT("Connect Start to the first quest step"));
	RootItem.bCompleted = Asset->Nodes.ContainsByPredicate([](const FDQSQuestNode& Node) { return Node.NodeType == EDQSQuestNodeType::Start && Node.NextNodeId.IsValid(); });
	RootItem.Guidance = FText::FromString(TEXT("Drag from Start.Next to an Objective or Branch node."));

	FDQSChecklistItem& ObjectiveItem = OutItems.AddDefaulted_GetRef();
	ObjectiveItem.Label = FText::FromString(TEXT("Create an objective with text and a tag"));
	ObjectiveItem.bCompleted = Asset->Nodes.ContainsByPredicate([](const FDQSQuestNode& Node) { return Node.NodeType == EDQSQuestNodeType::Objective && !Node.Description.IsEmpty() && Node.ObjectiveTag.IsValid(); });
	ObjectiveItem.Guidance = FText::FromString(TEXT("Select an Objective node and set Description plus Objective Tag."));

	FDQSChecklistItem& CountItem = OutItems.AddDefaulted_GetRef();
	CountItem.Label = FText::FromString(TEXT("Set a valid target count"));
	CountItem.bCompleted = Asset->Nodes.ContainsByPredicate([](const FDQSQuestNode& Node) { return Node.NodeType == EDQSQuestNodeType::Objective && Node.TargetCount > 0; });
	CountItem.Guidance = FText::FromString(TEXT("Most first objectives should use a target count of 1."));

	FDQSChecklistItem& EndItem = OutItems.AddDefaulted_GetRef();
	EndItem.Label = FText::FromString(TEXT("Add a completion path"));
	EndItem.bCompleted = Asset->Nodes.ContainsByPredicate([](const FDQSQuestNode& Node) { return Node.NodeType == EDQSQuestNodeType::Complete; });
	EndItem.Guidance = FText::FromString(TEXT("Add a Complete node so the quest has a visible ending."));
}

FDQSNodeHelpEntry FDQSGraphEditorUtils::GetDialogueNodeHelp(const EDQSDialogueNodeType NodeType)
{
	FDQSNodeHelpEntry Entry;
	switch (NodeType)
	{
	case EDQSDialogueNodeType::Entry:
		Entry.Title = FText::FromString(TEXT("Entry"));
		Entry.Purpose = FText::FromString(TEXT("The fixed starting point for the dialogue."));
		Entry.UseWhen = FText::FromString(TEXT("Always keep exactly one Entry node."));
		Entry.BeginnerMistake = FText::FromString(TEXT("Leaving Entry unconnected, so the dialogue never starts."));
		Entry.Example = FText::FromString(TEXT("Entry -> Greeting Speech"));
		break;
	case EDQSDialogueNodeType::Speech:
		Entry.Title = FText::FromString(TEXT("Speech"));
		Entry.Purpose = FText::FromString(TEXT("Shows a line spoken by a character."));
		Entry.UseWhen = FText::FromString(TEXT("Use for NPC lines, narration, or short spoken beats."));
		Entry.BeginnerMistake = FText::FromString(TEXT("Creating a Speech node but forgetting to write the Text."));
		Entry.Example = FText::FromString(TEXT("Villager: \"Could you help me with a task?\""));
		break;
	case EDQSDialogueNodeType::Choice:
		Entry.Title = FText::FromString(TEXT("Choice"));
		Entry.Purpose = FText::FromString(TEXT("Lets the player pick between two or more responses."));
		Entry.UseWhen = FText::FromString(TEXT("Use when the player should steer the conversation."));
		Entry.BeginnerMistake = FText::FromString(TEXT("Only wiring one branch, which makes the choice feel fake."));
		Entry.Example = FText::FromString(TEXT("Accept quest / Decline quest"));
		break;
	case EDQSDialogueNodeType::ConditionedStart:
		Entry.Title = FText::FromString(TEXT("Conditioned Branch"));
		Entry.Purpose = FText::FromString(TEXT("Branches the opening of a dialogue based on conditions like first visit, prior NPC interactions, or quest progress."));
		Entry.UseWhen = FText::FromString(TEXT("Use near Entry when returning players should see different opening lines."));
		Entry.BeginnerMistake = FText::FromString(TEXT("Forgetting to set a fallback branch for new players."));
		Entry.Example = FText::FromString(TEXT("Entry -> Conditioned Branch -> Returning Greeting / First-Time Greeting"));
		break;
	case EDQSDialogueNodeType::Action:
		Entry.Title = FText::FromString(TEXT("Set Condition"));
		Entry.Purpose = FText::FromString(TEXT("Sets tags or variables that later Conditioned Branch nodes can check."));
		Entry.UseWhen = FText::FromString(TEXT("Use after a choice, quest hook, or speech when the conversation should remember player state."));
		Entry.BeginnerMistake = FText::FromString(TEXT("Forgetting to add any set operations, so the node does nothing."));
		Entry.Example = FText::FromString(TEXT("Set HasMet.Elder = true, then continue."));
		break;
	case EDQSDialogueNodeType::QuestHook:
		Entry.Title = FText::FromString(TEXT("Quest Hook"));
		Entry.Purpose = FText::FromString(TEXT("Starts, advances, completes, or fails a quest from dialogue."));
		Entry.UseWhen = FText::FromString(TEXT("Use when the conversation changes quest state."));
		Entry.BeginnerMistake = FText::FromString(TEXT("Forgetting to pick a quest asset or operation."));
		Entry.Example = FText::FromString(TEXT("Start Quest_FirstFavor after the player accepts"));
		break;
	default:
		Entry.Title = DialogueNodeTypeName(NodeType);
		Entry.Purpose = FText::FromString(TEXT("Special-purpose dialogue flow node."));
		Entry.UseWhen = FText::FromString(TEXT("Use when the graph needs explicit flow-control logic."));
		Entry.BeginnerMistake = FText::FromString(TEXT("Skipping an End node on terminal paths."));
		Entry.Example = FText::FromString(TEXT("Set Condition -> End"));
		break;
	}

	return Entry;
}

FDQSNodeHelpEntry FDQSGraphEditorUtils::GetQuestNodeHelp(const EDQSQuestNodeType NodeType)
{
	FDQSNodeHelpEntry Entry;
	switch (NodeType)
	{
	case EDQSQuestNodeType::Start:
		Entry.Title = FText::FromString(TEXT("Start"));
		Entry.Purpose = FText::FromString(TEXT("The fixed starting point for the quest."));
		Entry.UseWhen = FText::FromString(TEXT("Always keep exactly one Start node."));
		Entry.BeginnerMistake = FText::FromString(TEXT("Leaving Start unconnected."));
		Entry.Example = FText::FromString(TEXT("Start -> Objective"));
		break;
	case EDQSQuestNodeType::Objective:
		Entry.Title = FText::FromString(TEXT("Objective"));
		Entry.Purpose = FText::FromString(TEXT("Represents a quest step the player must satisfy."));
		Entry.UseWhen = FText::FromString(TEXT("Use for collect, talk-to, reach-location, or count-based steps."));
		Entry.BeginnerMistake = FText::FromString(TEXT("Forgetting the objective tag, so gameplay cannot advance it."));
		Entry.Example = FText::FromString(TEXT("Bring back the apple (1/1)"));
		break;
	case EDQSQuestNodeType::Branch:
		Entry.Title = FText::FromString(TEXT("Branch"));
		Entry.Purpose = FText::FromString(TEXT("Splits quest flow into primary and alternate outcomes."));
		Entry.UseWhen = FText::FromString(TEXT("Use when success and failure should go to different follow-up nodes."));
		Entry.BeginnerMistake = FText::FromString(TEXT("Forgetting to connect the alternate path."));
		Entry.Example = FText::FromString(TEXT("If player helped in time -> Reward, else -> Fail"));
		break;
	default:
		Entry.Title = QuestNodeTypeName(NodeType);
		Entry.Purpose = FText::FromString(TEXT("Quest flow node."));
		Entry.UseWhen = FText::FromString(TEXT("Use when the quest should continue, complete, fail, or reward the player."));
		Entry.BeginnerMistake = FText::FromString(TEXT("Leaving important paths without a clear end state."));
		Entry.Example = FText::FromString(TEXT("Reward -> Complete"));
		break;
	}

	return Entry;
}






