// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SHS/TrainModule.h"
#include "TrainWaterTank.generated.h"

UCLASS()
class EXPRESS_API ATrainWaterTank : public ATrainModule
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	virtual void EndFire() override;

protected:
	UPROPERTY()
	class UMaterialInstanceDynamic* TankMaterial;
	FLinearColor MaterialColor;

private:
	void ChangeTankColor();
};
