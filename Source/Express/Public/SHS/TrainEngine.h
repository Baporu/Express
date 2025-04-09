// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SHS/TrainModule.h"
#include "TrainEngine.generated.h"

class ATrainModule;

UCLASS()
class EXPRESS_API ATrainEngine : public ATrainModule
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrainEngine();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere)
	TArray<ATrainModule*> TrainModules;

	bool CheckModule(int32 ModuleIndex);
	ATrainModule* GetFrontModule(int32 ModuleIndex);
	ATrainModule* GetBackModule(int32 ModuleIndex);

	void AddFireTime(float WaterTankTime);

protected:
	UPROPERTY(EditAnywhere, Category = "Train")
	float TrainSpeed = 25.0f;
	UPROPERTY(EditAnywhere, Category = "Train")
	FVector NextPos = FVector::ZeroVector;

	
	virtual void OnFire(float DeltaTime) override;

private:
	// 임시 함수들
	void GetTileLocation();
	void RotateTrain(float DeltaTime);

	double NextRot;
	double rotA;


};
