#pragma once

UENUM(BlueprintType)
enum class EGameStatus : uint8
{
	None,
	Training,
	WaitingForPlayers,
	InProgress,
	Final,
	Max,
};
