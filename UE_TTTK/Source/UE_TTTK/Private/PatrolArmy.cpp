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
		
		idx = idx+1;
	}
	else
	{
		UE_LOG(LogTemp,Warning, TEXT("감소한다. %d"),idx);
		idx = idx-1;;
	}
	UE_LOG(LogTemp,Warning,TEXT("업데이트 "));
	
	
}

void APatrolArmy::SetIndex(int32 index)
{
	idx = index;
}

bool APatrolArmy::isFinalPatrolPoint()
{
	if (idx ==0 && !bIsForward) // 시작점이면서 역행 
	{
		UE_LOG(LogTemp,Warning,TEXT("역방향 도착"));
		return true;
	}
	else if (idx== armyPath->GetNumPatrolPoints()-1 && bIsForward)
	{
		UE_LOG(LogTemp,Warning,TEXT("도착했을까? 현재 인덱스 %d    도착하려면"),idx,armyPath->GetNumPatrolPoints()-1 );
		UE_LOG(LogTemp,Warning,TEXT("정방향 도착"));
		return true;
	}
	return false;
}

void APatrolArmy::PlayHaningMontage()
{
	bIsHangingFireAnimCompleted = false; // 플래그 리셋
	PlayAnimMontage(HangingFireMontage);
	bIsNight = true;
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
