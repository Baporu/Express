// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SHS/TrainModule.h"
#include "TrainCrafter.generated.h"

class ATrainCargo;

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
	TSubclassOf<AActor> BP_Rail;
	UPROPERTY(EditAnywhere, Category = "Craft")
	float MakeTime = 3.2f;
	UPROPERTY(EditAnywhere, Category = "Craft")
	int32 MaxStackSize = 3;

	void Init(ATrainEngine* EngineModule, float TrainSpeed, FVector Destination, ATrainCargo* CargoModule);

protected:

private:
	UPROPERTY(VisibleAnywhere, Category = "Craft")
	ATrainCargo* TrainCargo;
	UPROPERTY(VisibleAnywhere, Category = "Craft")
	bool bIsMaking = false;
	UPROPERTY(VisibleAnywhere, Category = "Craft")
	float MakeTimer = 0.0f;
	UPROPERTY(VisibleAnywhere, Category = "Craft")
	int32 StackSize = 0;

	void MakeRail();

};
