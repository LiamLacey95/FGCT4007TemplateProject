#pragma once

#include "CoreMinimal.h"
#include "DQSTypes.h"

class UDialogueGraphAsset;
class UEdGraph;
class UEdGraphNode;
class UEdGraphNode_Comment;
class UObject;
class UQuestGraphAsset;
class UDQSDialogueEdGraphNode;
class UDQSQuestEdGraphNode;

enum class EDQSValidationSeverity : uint8
{
	Blocking,
	Warning,
	Suggestion
};

enum class EDQSDialogueStarterPattern : uint8
{
	LinearConversation,
	AcceptDecline,
	BranchingConversation
};

enum class EDQSQuestStarterPattern : uint8
{
	SimpleObjective,
	AcceptAndCompleteLoop,
	BranchingQuest
};

enum class EDQSDialogueSnippet : uint8
{
	SpeechChoiceEnd,
	QuestAcceptance
};

enum class EDQSQuestSnippet : uint8
{
	StartObjectiveComplete,
	BranchingObjective
};

struct FDQSValidationIssue
{
	EDQSValidationSeverity Severity = EDQSValidationSeverity::Suggestion;
	FText Title;
	FText Message;
	FText Guidance;
	FGuid NodeId;
	bool bCanFix = false;
};

struct FDQSChecklistItem
{
	FText Label;
	bool bCompleted = false;
	FText Guidance;
	FGuid NodeId;
};

struct FDQSNodeHelpEntry
{
	FText Title;
	FText Purpose;
	FText UseWhen;
	FText BeginnerMistake;
	FText Example;
};

struct FDQSDialogueCreationOptions
{
	FText DisplayName;
	EDQSDialogueStarterPattern Pattern = EDQSDialogueStarterPattern::AcceptDecline;
	FName SpeakerId = TEXT("NPC");
	FText SpeakerDisplayName = FText::FromString(TEXT("Guide"));
};

struct FDQSQuestCreationOptions
{
	FText DisplayName;
	EDQSQuestStarterPattern Pattern = EDQSQuestStarterPattern::SimpleObjective;
	FText ObjectiveText = FText::FromString(TEXT("Complete the first objective."));
	FName ObjectiveTag = TEXT("Quest.NewQuest.Objective");
};

class DQSEDITOR_API FDQSGraphEditorUtils
{
public:
	static bool IsSimpleMode(const UObject* Asset);
	static void SetSimpleMode(UObject* Asset, bool bUseSimpleMode);
	static bool IsGuidedHelpEnabled(const UObject* Asset);
	static void SetGuidedHelpEnabled(UObject* Asset, bool bEnabled);

	static UDQSDialogueEdGraphNode* CreateDialogueNode(UEdGraph* Graph, const FVector2D& Location, EDQSDialogueNodeType NodeType, const FText& Title = FText::GetEmpty());
	static UDQSQuestEdGraphNode* CreateQuestNode(UEdGraph* Graph, const FVector2D& Location, EDQSQuestNodeType NodeType, const FText& Title = FText::GetEmpty());
	static UEdGraphNode_Comment* CreateCommentNode(UEdGraph* Graph, const FVector2D& Location, const FVector2D& Size, const FText& Title, const FLinearColor& Color, const FString& CommentText);
	static void ConnectNodes(UEdGraphNode* FromNode, int32 OutputPinIndex, UEdGraphNode* ToNode);
	static void ScaffoldDialogueAsset(UDialogueGraphAsset* Asset, const FDQSDialogueCreationOptions& Options);
	static void ScaffoldQuestAsset(UQuestGraphAsset* Asset, const FDQSQuestCreationOptions& Options);
	static void InsertDialogueSnippet(UDialogueGraphAsset* Asset, EDQSDialogueSnippet Snippet);
	static void InsertQuestSnippet(UQuestGraphAsset* Asset, EDQSQuestSnippet Snippet);

	static void BuildDialogueValidationIssues(UDialogueGraphAsset* Asset, TArray<FDQSValidationIssue>& OutIssues);
	static void BuildQuestValidationIssues(UQuestGraphAsset* Asset, TArray<FDQSValidationIssue>& OutIssues);
	static void BuildDialogueChecklist(UDialogueGraphAsset* Asset, TArray<FDQSChecklistItem>& OutItems);
	static void BuildQuestChecklist(UQuestGraphAsset* Asset, TArray<FDQSChecklistItem>& OutItems);

	static FDQSNodeHelpEntry GetDialogueNodeHelp(EDQSDialogueNodeType NodeType);
	static FDQSNodeHelpEntry GetQuestNodeHelp(EDQSQuestNodeType NodeType);
};
