// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SHS/TrainModule.h"
#include "TrainCrafter.generated.h"

class ATrainCargo;
class AItem;

/**
 * 
 */
UCLASS()
class EXPRESS_API ATrainCrafter : public ATrainModule
{
	GENERATED_BODY()
	
public:
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditDefaultsOnly, Category = "Craft")
	TSubclassOf<AItem> BP_Rail;
	UPROPERTY(VisibleAnywhere, Category = "Craft")
	TArray<AItem*> Rails;
	UPROPERTY(EditAnywhere, Category = "Craft")
	float MakeTime = 3.2f;
	UPROPERTY(EditAnywhere, Category = "Craft")
	int32 MaxStackSize = 3;

	void Init(ATrainEngine* EngineModule, float TrainSpeed, FVector Destination, ATrainCargo* CargoModule);
	void CheckMakeRail();

	bool CheckRail();
	TArray<AItem*> GetRail();

private:
	UPROPERTY(VisibleAnywhere, Category = "Craft")
	ATrainCargo* TrainCargo;
	UPROPERTY(VisibleAnywhere, Category = "Craft")
	bool bIsMaking = false;
	UPROPERTY(VisibleAnywhere, Category = "Craft")
	float MakeTimer = 0.0f;

	void MakeRail();

	UFUNCTION(Server, Reliable)
	void Server_CheckMakeRail();
	UFUNCTION(Client, Reliable)
	void Client_AttachRail(AItem* Rail, AActor* ParentActor);
	UFUNCTION(Client, Reliable)
	void Client_DetachRail(AItem* Rail);
};
