// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/PDA_MapList.h"
#include "Data/MapInfo.h"

bool UPDA_MapList::GetMapInfoByName(const FString& inMapName, FMapInfo& outMapinfo)
{
	for (const FMapInfo& info : availableMaps)
	{
		if (info.mapName.Equals(inMapName, ESearchCase::IgnoreCase))
		{
			outMapinfo = info;
			return true;
		}
	}
	return false;
}

bool UPDA_MapList::GetMapInfoByDisplayName(const FText& inDisplayMapName, FMapInfo& outMapinfo)
{
	for (const FMapInfo& info : availableMaps)
	{
		if (info.displayName.EqualToCaseIgnored(inDisplayMapName))
		{
			outMapinfo = info;
			return true;
		}
	}
	return false;
}

bool UPDA_MapList::GetMapObjectPathByName(const FString& inMapName, FString& outPath)
{
	outPath.Empty();
	for (const FMapInfo& info : availableMaps)
	{
		if (info.mapName.Equals(inMapName, ESearchCase::IgnoreCase))
		{
			if (info.mapAsset.IsNull())
			{
				return false;
			}
			outPath = info.mapAsset.ToSoftObjectPath().ToString();
			return true;
		}
	}
	return false;
}

bool UPDA_MapList::GetMapObjectPathByDisplayName(const FText& inDisplayMapName, FString& outPath)
{
	outPath.Empty();
	for (const FMapInfo& info : availableMaps)
	{
		if (info.displayName.EqualToCaseIgnored(inDisplayMapName))
		{
			if (info.mapAsset.IsNull())
			{
				return false;
			}
			outPath = info.mapAsset.ToSoftObjectPath().ToString();
			return true;
		}
	}
	return false;
}

FPrimaryAssetId UPDA_MapList::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("MapList", GetFName());
}
