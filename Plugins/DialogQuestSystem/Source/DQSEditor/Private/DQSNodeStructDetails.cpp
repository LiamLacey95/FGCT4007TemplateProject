#include "DQSNodeStructDetails.h"

#include "DQSEdGraphNodes.h"
#include "DQSGraphEditorUtils.h"
#include "DQSTypes.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyAccess.h"
#include "PropertyHandle.h"

namespace
{
	void AddVisibleChildren(
		TSharedRef<IPropertyHandle> StructPropertyHandle,
		IDetailChildrenBuilder& StructBuilder,
		const TSet<FName>& HiddenChildren)
	{
		uint32 NumChildren = 0;
		StructPropertyHandle->GetNumChildren(NumChildren);

		for (uint32 Index = 0; Index < NumChildren; ++Index)
		{
			TSharedPtr<IPropertyHandle> ChildHandle = StructPropertyHandle->GetChildHandle(Index);
			if (!ChildHandle.IsValid())
			{
				continue;
			}

			if (HiddenChildren.Contains(ChildHandle->GetProperty()->GetFName()))
			{
				continue;
			}

			StructBuilder.AddProperty(ChildHandle.ToSharedRef());
		}
	}

	bool IsSimpleModeForProperty(const TSharedRef<IPropertyHandle>& StructPropertyHandle)
	{
		TArray<UObject*> OuterObjects;
		StructPropertyHandle->GetOuterObjects(OuterObjects);
		if (OuterObjects.IsEmpty())
		{
			return true;
		}

		for (UObject* OuterObject : OuterObjects)
		{
			if (FDQSGraphEditorUtils::IsSimpleMode(OuterObject))
			{
				return true;
			}

			if (const UEdGraphNode* Node = Cast<UEdGraphNode>(OuterObject))
			{
				if (FDQSGraphEditorUtils::IsSimpleMode(Node->GetGraph() ? Node->GetGraph()->GetOuter() : nullptr))
				{
					return true;
				}
			}
		}

		return false;
	}

	EDQSDialogueNodeType GetDialogueNodeTypeForProperty(const TSharedRef<IPropertyHandle>& StructPropertyHandle)
	{
		TSharedPtr<IPropertyHandle> NodeTypeHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, NodeType));
		uint8 NodeTypeValue = static_cast<uint8>(EDQSDialogueNodeType::Speech);
		if (NodeTypeHandle.IsValid() && NodeTypeHandle->GetValue(NodeTypeValue) == FPropertyAccess::Success)
		{
			return static_cast<EDQSDialogueNodeType>(NodeTypeValue);
		}

		return EDQSDialogueNodeType::Speech;
	}
}

TSharedRef<IPropertyTypeCustomization> FDQSDialogueNodeDetails::MakeInstance()
{
	return MakeShared<FDQSDialogueNodeDetails>();
}

void FDQSDialogueNodeDetails::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructPropertyHandle->CreateDefaultPropertyButtonWidgets();
	HeaderRow.NameContent()[StructPropertyHandle->CreatePropertyNameWidget()];
}

void FDQSDialogueNodeDetails::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	const EDQSDialogueNodeType NodeType = GetDialogueNodeTypeForProperty(StructPropertyHandle);

	TSet<FName> HiddenChildren =
	{
		GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, NodeId),
		GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, NodeType),
		GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, NextNodeId),
		GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, AlternateNodeId),
		GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, JumpTargetNodeId)
	};

	if (NodeType != EDQSDialogueNodeType::ConditionedStart)
	{
		HiddenChildren.Add(GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, Conditions));
	}

	if (NodeType != EDQSDialogueNodeType::Action)
	{
		HiddenChildren.Add(GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, Actions));
	}

	if (IsSimpleModeForProperty(StructPropertyHandle))
	{
		HiddenChildren.Append(
		{
			GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, PresentationTags),
			GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, VoiceSound)
		});
	}

	if (NodeType == EDQSDialogueNodeType::ConditionedStart || NodeType == EDQSDialogueNodeType::Condition)
	{
		HiddenChildren.Append(
		{
			GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, Title),
			GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, SpeakerId),
			GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, Text),
			GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, PresentationTags),
			GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, VoiceSound),
			GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, Presentation),
			GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, Choices),
			GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, Actions),
			GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, QuestHook)
		});
	}
	else if (NodeType == EDQSDialogueNodeType::Action)
	{
		HiddenChildren.Append(
		{
			GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, SpeakerId),
			GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, Text),
			GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, PresentationTags),
			GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, VoiceSound),
			GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, Presentation),
			GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, Choices),
			GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, Conditions),
			GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, QuestHook)
		});
	}

	AddVisibleChildren(StructPropertyHandle, StructBuilder, HiddenChildren);
}

TSharedRef<IPropertyTypeCustomization> FDQSQuestNodeDetails::MakeInstance()
{
	return MakeShared<FDQSQuestNodeDetails>();
}

void FDQSQuestNodeDetails::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructPropertyHandle->CreateDefaultPropertyButtonWidgets();
	HeaderRow.NameContent()[StructPropertyHandle->CreatePropertyNameWidget()];
}

void FDQSQuestNodeDetails::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TSet<FName> HiddenChildren =
	{
		GET_MEMBER_NAME_CHECKED(FDQSQuestNode, NodeId),
		GET_MEMBER_NAME_CHECKED(FDQSQuestNode, NodeType),
		GET_MEMBER_NAME_CHECKED(FDQSQuestNode, NextNodeId),
		GET_MEMBER_NAME_CHECKED(FDQSQuestNode, AlternateNodeId)
	};

	if (IsSimpleModeForProperty(StructPropertyHandle))
	{
		HiddenChildren.Append(
		{
			GET_MEMBER_NAME_CHECKED(FDQSQuestNode, Conditions),
			GET_MEMBER_NAME_CHECKED(FDQSQuestNode, Actions)
		});
	}

	AddVisibleChildren(StructPropertyHandle, StructBuilder, HiddenChildren);
}

TSharedRef<IPropertyTypeCustomization> FDQSDialogueChoiceDetails::MakeInstance()
{
	return MakeShared<FDQSDialogueChoiceDetails>();
}

void FDQSDialogueChoiceDetails::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructPropertyHandle->CreateDefaultPropertyButtonWidgets();
	HeaderRow.NameContent()[StructPropertyHandle->CreatePropertyNameWidget()];
}

void FDQSDialogueChoiceDetails::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TSet<FName> HiddenChildren =
	{
		GET_MEMBER_NAME_CHECKED(FDQSDialogueChoice, ChoiceId),
		GET_MEMBER_NAME_CHECKED(FDQSDialogueChoice, TargetNodeId),
		GET_MEMBER_NAME_CHECKED(FDQSDialogueChoice, Conditions),
		GET_MEMBER_NAME_CHECKED(FDQSDialogueChoice, Actions)
	};

	AddVisibleChildren(StructPropertyHandle, StructBuilder, HiddenChildren);
}
