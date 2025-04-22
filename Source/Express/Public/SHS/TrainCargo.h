// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SHS/TrainModule.h"
#include "TrainCargo.generated.h"

class AItem;
enum class EItemType : uint8;

/**
 * 
 */
UCLASS()
class EXPRESS_API ATrainCargo : public ATrainModule
{
	GENERATED_BODY()
	
public:
	ATrainCargo();

public:
	// 나무 컨테이너
	UPROPERTY(VisibleAnywhere, Category = "Item")
	TArray<AItem*> Woods;
	// 철 컨테이너
	UPROPERTY(VisibleAnywhere, Category = "Item")
	TArray<AItem*> Stones;

	// 상호작용 가능한지(불 붙어있는지) 확인하는 함수
	bool CheckInteraction();
	// 유효한 아이템 종류인지 확인하는 함수
	bool CheckAddResource(EItemType ResourceType);
	// 아이템 가져올 수 있는지 확인하는 함수
	bool CheckGetResource(EItemType ResourceType);

	void AddResource(TArray<AItem*> Resources);
	TArray<AItem*> GetResource(EItemType ResourceType);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_AttachResource(AItem* Resource);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_AttachResourceToParent(AItem* Resource, AActor* ParentActor);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DetachResource(AItem* Resource);

protected:
	// 화물차가 담을 수 있는 최대 수량
	UPROPERTY(EditAnywhere, Category = "Item")
	int32 MaxCount = 3;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* WoodComp;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* StoneComp;

private:

};
