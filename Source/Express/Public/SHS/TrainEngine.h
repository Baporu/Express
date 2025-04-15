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
	UPROPERTY(EditDefaultsOnly, Category = "Train")
	TSubclassOf<class ATrainWaterTank> BP_WaterTank;
	UPROPERTY(EditDefaultsOnly, Category = "Train")
	TSubclassOf<class ATrainCargo> BP_Cargo;
	UPROPERTY(EditDefaultsOnly, Category = "Train")
	TSubclassOf<class ATrainCrafter> BP_Crafter;

	void Init();

	bool CheckModule(int32 ModuleIndex);
	ATrainModule* GetFrontModule(int32 CurrentModuleIndex);
	ATrainModule* GetBackModule(int32 CurrentModuleIndex);
	void AttachModule(ATrainModule* TrainModule);
	void AttachModule(ATrainModule* TrainModule, int32 AttachIndex);

	void AddFireTime(float WaterTankTime);

	void CheckMakeRail();

	UPROPERTY(VisibleAnywhere)
	TArray<ATrainModule*> TrainModules;

protected:
	UPROPERTY(EditAnywhere, Category = "Train")
	float TrainSpeed = 25.0f;

	
	virtual void MoveTrain(float DeltaTime) override;
//	virtual void RotateTrain(float DeltaTime) override;

	virtual void OnFire(float DeltaTime) override;

private:
	// 임시 함수들
	void GetTileLocation();
	void SpawnDefaultModules();

};
