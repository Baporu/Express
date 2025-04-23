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

	void Init(class AGridManager* Grid, class ATile* NextTile, int32 Row, int32 Column);

	bool CheckModule(int32 ModuleIndex);
	ATrainModule* GetFrontModule(int32 CurrentModuleIndex);
	ATrainModule* GetBackModule(int32 CurrentModuleIndex);
	void AttachModule(ATrainModule* TrainModule);
	void AttachModule(ATrainModule* TrainModule, int32 AttachIndex);

	void AddFireTime(float WaterTankTime);

	void CheckMakeRail();

	UPROPERTY(Replicated, VisibleAnywhere)
	TArray<ATrainModule*> TrainModules;

protected:
	UPROPERTY(EditAnywhere, Category = "Train")
	float TrainSpeed = 25.0f;

	UPROPERTY(Replicated, EditAnywhere, Category = "Tile")
	class AGridManager* GridManager;

	// 기차가 움직이기 전 대기 시간
	UPROPERTY(EditDefaultsOnly, Category = "Move")
	float InitTime = 10.0f;

	// 이동할 목표 타일
	UPROPERTY(VisibleAnywhere, Category = "Move")
	class ATile* CurrentTile;
	// 현재 목표 타일의 Row 인덱스
	UPROPERTY(VisibleAnywhere, Category = "Move")
	int32 RowIndex = -1;
	// 현재 목표 타일의 Column 인덱스
	UPROPERTY(VisibleAnywhere, Category = "Move")
	int32 ColIndex = -1;

	
	void CheckNextTile();

	virtual void MoveTrain(float DeltaTime) override;
//	virtual void RotateTrain(float DeltaTime) override;

	virtual void OnFire(float DeltaTime) override;

	// UFUNCTION은 상속할 때 매크로 빼야 함!
	virtual void OnWaterBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
									 int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Fire")
	class ATrainWaterTank* TrainWaterTank;

	UPROPERTY(VisibleAnywhere, Category = "Move")
	class ACameraActor* MainCam;

	// 임시 함수들
	void GetTileLocation();
	void SpawnDefaultModules();

	void GetMainCamera();
};
