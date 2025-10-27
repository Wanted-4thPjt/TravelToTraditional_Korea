#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "HeritageObjectData.generated.h"

// 유물 데이터 구조체 (데이터 테이블의 한 행)
USTRUCT(BlueprintType)
struct UE_TTTK_API FHeritageObjectData : public FTableRowBase
{
	GENERATED_BODY()

	// 유물 고유 ID (예: "heritage_jar_01")
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic Info")
	FString ObjectID;

	// 유물 이름 (예: "조선시대 항아리")
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic Info")
	FText ObjectName;

	// 상세 설명
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic Info")
	FText Description;

	// 발견 시 점수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	int32 DiscoveryScore = 10;

	// 기본 생성자
	FHeritageObjectData()
	{
		ObjectID = TEXT("");
		ObjectName = FText::FromString(TEXT("Unknown Heritage"));
		Description = FText::FromString(TEXT("No description available"));
		DiscoveryScore = 10;
	}
};
