// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolArmy.h"

#include "PatrolPath.h"

class APatrolPath* APatrolArmy::GetPatrolPath()
{
	return armyPath;
}

void APatrolArmy::SetPatrolPath(class APatrolPath* path)
{
	armyPath = path;
}

void APatrolArmy::UpdateIndex()
{
	if (!armyPath)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] UpdateIndex: armyPath is null!"), *GetName());
		return;
	}

	int32 oldIdx = idx;
	bool oldDirection = bIsForward;

	if (bIsForward)
	{
		UE_LOG(LogTemp,Warning, TEXT("증가한다. %d"),idx);
		
		idx++;
	}
	else
	{
		UE_LOG(LogTemp,Warning, TEXT("감소한다. %d"),idx);
		idx--;
	}
	UE_LOG(LogTemp,Warning,TEXT("업데이트 "));
	if (armyPath->isFinalPoint(idx))
	{
		bIsForward = !bIsForward;
		UE_LOG(LogTemp, Warning, TEXT("[%s] 방향 전환! idx[%d->%d], 방향[%s->%s]"),
			*GetName(), oldIdx, idx,
			oldDirection ? TEXT("Forward") : TEXT("Backward"),
			bIsForward ? TEXT("Forward") : TEXT("Backward"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] UpdateIndex: idx[%d->%d], 방향[%s]"),
			*GetName(), oldIdx, idx, bIsForward ? TEXT("Forward") : TEXT("Backward"));
	}
}

void APatrolArmy::SetIndex(int32 index)
{
	idx = index;
}

FPatrolPoint* APatrolArmy::GetcurrentPatrolPoint()
{
	if (!armyPath)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] GetcurrentPatrolPoint: armyPath is null!"), *GetName());
		return nullptr;
	}
	UE_LOG(LogTemp, Warning, TEXT("[%s] GetcurrentPatrolPoint 호출 - idx: %d"), *GetName(), idx);
	return armyPath->GetPatrolPoint(idx);
}

 FPatrolPoint* APatrolArmy::GetNextPatrolPoint()
{
	if (!armyPath)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] GetNextPatrolPoint: armyPath is null!"), *GetName());
		return nullptr;
	}
	UE_LOG(LogTemp, Warning, TEXT("[%s] GetNextPatrolPoint 호출 - 현재 idx: %d"), *GetName(), idx);
	return armyPath->FindNextPoint(idx);
}

struct FPatrolPoint* APatrolArmy::GetPreviousPatrolPoint()
{
	if (!armyPath)
	{
		UE_LOG(LogTemp, Error, TEXT("GetPreviousPatrolPoint: armyPath is null!"));
		return nullptr;
	}
	return armyPath->FindPervPoint(idx);
}
