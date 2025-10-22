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
	
	idx++;
	if (armyPath->isFinalPoint(idx))
	{
		idx = armyPath->GetFinalPatrolPointIdx();
	}
	
}

 FPatrolPoint* APatrolArmy::GetcurrentPatrolPoint()
{
	return armyPath->GetPatrolPoint(idx);
}

 FPatrolPoint* APatrolArmy::GetNextPatrolPoint()
{
	return armyPath->FindNextPoint(idx);
}

struct FPatrolPoint* APatrolArmy::GetPreviousPatrolPoint()
{
	return armyPath->FindPervPoint(idx);
}
