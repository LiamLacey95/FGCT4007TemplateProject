#include "DQSEdGraphNodeDetails.h"

#include "DQSEdGraphNodes.h"
#include "DQSTypes.h"
#include "DetailLayoutBuilder.h"
#include "PropertyHandle.h"

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

	TArray<TWeakObjectPtr<UDQSDialogueEdGraphNode>> GetSelectedDialogueNodes(IDetailLayoutBuilder& DetailBuilder)
	{
		TArray<TWeakObjectPtr<UDQSDialogueEdGraphNode>> DialogueNodes;
		TArray<TWeakObjectPtr<UObject>> SelectedObjects;
		DetailBuilder.GetObjectsBeingCustomized(SelectedObjects);
		for (const TWeakObjectPtr<UObject>& SelectedObject : SelectedObjects)
		{
			UDQSDialogueEdGraphNode* DialogueNode = Cast<UDQSDialogueEdGraphNode>(SelectedObject.Get());
			if (DialogueNode)
			{
				DialogueNodes.Add(DialogueNode);
			}
		}
		return DialogueNodes;
	}

	void BindDialogueChoiceArrayRefresh(IDetailLayoutBuilder& DetailBuilder)
	{
		TSharedRef<IPropertyHandle> NodeDataHandle = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UDQSDialogueEdGraphNode, NodeData));
		if (TSharedPtr<IPropertyHandle> ChoicesHandle = NodeDataHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, Choices)))
		{
			const TArray<TWeakObjectPtr<UDQSDialogueEdGraphNode>> DialogueNodes = GetSelectedDialogueNodes(DetailBuilder);
			const FSimpleDelegate RefreshDelegate = FSimpleDelegate::CreateLambda([DialogueNodes]()
			{
				for (const TWeakObjectPtr<UDQSDialogueEdGraphNode>& WeakNode : DialogueNodes)
				{
					UDQSDialogueEdGraphNode* DialogueNode = WeakNode.Get();
					if (DialogueNode && DialogueNode->NodeData.NodeType == EDQSDialogueNodeType::Choice)
					{
						DialogueNode->RefreshPinsAfterDetailsChange();
					}
				}
			});
			if (TSharedPtr<IPropertyHandleArray> ChoicesArrayHandle = ChoicesHandle->AsArray())
			{
				ChoicesArrayHandle->SetOnNumElementsChanged(RefreshDelegate);
			}
			ChoicesHandle->SetOnPropertyValueChanged(RefreshDelegate);
			ChoicesHandle->SetOnChildPropertyValueChanged(RefreshDelegate);
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
	BindDialogueChoiceArrayRefresh(DetailBuilder);
}

TSharedRef<IDetailCustomization> FDQSQuestEdGraphNodeDetails::MakeInstance()
{
	return MakeShared<FDQSQuestEdGraphNodeDetails>();
}

void FDQSQuestEdGraphNodeDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	HideQuestInternalFields(DetailBuilder);
}
