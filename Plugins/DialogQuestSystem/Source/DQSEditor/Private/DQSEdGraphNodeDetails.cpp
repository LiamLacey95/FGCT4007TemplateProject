#include "DQSEdGraphNodeDetails.h"

#include "DQSEdGraphNodes.h"
#include "DQSTypes.h"
#include "PropertyHandle.h"
#include "DetailLayoutBuilder.h"

namespace
{
	void HideDialogueInternalFields(IDetailLayoutBuilder& DetailBuilder)
	{
		TSharedRef<IPropertyHandle> NodeDataHandle = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UDQSDialogueEdGraphNode, NodeData));

		const TArray<FName> HiddenFields =
		{
			GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, NodeType),
			GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, NodeId),
			GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, NextNodeId),
			GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, AlternateNodeId),
			GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, JumpTargetNodeId)
		};

		for (const FName FieldName : HiddenFields)
		{
			if (TSharedPtr<IPropertyHandle> ChildHandle = NodeDataHandle->GetChildHandle(FieldName))
			{
				DetailBuilder.HideProperty(ChildHandle);
			}
		}
	}

	void HideQuestInternalFields(IDetailLayoutBuilder& DetailBuilder)
	{
		TSharedRef<IPropertyHandle> NodeDataHandle = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UDQSQuestEdGraphNode, NodeData));

		const TArray<FName> HiddenFields =
		{
			GET_MEMBER_NAME_CHECKED(FDQSQuestNode, NodeType),
			GET_MEMBER_NAME_CHECKED(FDQSQuestNode, NodeId),
			GET_MEMBER_NAME_CHECKED(FDQSQuestNode, NextNodeId),
			GET_MEMBER_NAME_CHECKED(FDQSQuestNode, AlternateNodeId)
		};

		for (const FName FieldName : HiddenFields)
		{
			if (TSharedPtr<IPropertyHandle> ChildHandle = NodeDataHandle->GetChildHandle(FieldName))
			{
				DetailBuilder.HideProperty(ChildHandle);
			}
		}
	}
}

TSharedRef<IDetailCustomization> FDQSDialogueEdGraphNodeDetails::MakeInstance()
{
	return MakeShared<FDQSDialogueEdGraphNodeDetails>();
}

void FDQSDialogueEdGraphNodeDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	HideDialogueInternalFields(DetailBuilder);
}

TSharedRef<IDetailCustomization> FDQSQuestEdGraphNodeDetails::MakeInstance()
{
	return MakeShared<FDQSQuestEdGraphNodeDetails>();
}

void FDQSQuestEdGraphNodeDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	HideQuestInternalFields(DetailBuilder);
}
