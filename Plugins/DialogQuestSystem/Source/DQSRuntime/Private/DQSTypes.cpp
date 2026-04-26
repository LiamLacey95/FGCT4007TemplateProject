#include "DQSTypes.h"

FDQSVariableValue FDQSVariableValue::MakeBool(const bool bValue)
{
	FDQSVariableValue Value;
	Value.Type = EDQSValueType::Bool;
	Value.BoolValue = bValue;
	return Value;
}

FDQSVariableValue FDQSVariableValue::MakeInt(const int32 Value)
{
	FDQSVariableValue Result;
	Result.Type = EDQSValueType::Int;
	Result.IntValue = Value;
	return Result;
}

FDQSVariableValue FDQSVariableValue::MakeFloat(const float Value)
{
	FDQSVariableValue Result;
	Result.Type = EDQSValueType::Float;
	Result.FloatValue = Value;
	return Result;
}

FDQSVariableValue FDQSVariableValue::MakeName(const FName Value)
{
	FDQSVariableValue Result;
	Result.Type = EDQSValueType::Name;
	Result.NameValue = Value;
	return Result;
}

FDQSVariableValue FDQSVariableValue::MakeString(const FString& Value)
{
	FDQSVariableValue Result;
	Result.Type = EDQSValueType::String;
	Result.StringValue = Value;
	return Result;
}
