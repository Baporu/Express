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
	// ���� �����̳�
	UPROPERTY(VisibleAnywhere, Category = "Item")
	TArray<AItem*> Woods;
	// ö �����̳�
	UPROPERTY(VisibleAnywhere, Category = "Item")
	TArray<AItem*> Stones;

	bool CheckAddResource();
	bool CheckGetResource(EItemType ResourceType);
	void AddResource(AItem* Resource);
	AItem* GetResource(EItemType ResourceType);

protected:
	// ȭ������ ���� �� �ִ� �ִ� ����
	UPROPERTY(EditAnywhere, Category = "Item")
	int32 MaxCount = 3;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* WoodComp;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* StoneComp;

private:

};
