// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SHS/TrainModule.h"
#include "TrainWaterTank.generated.h"

UCLASS()
class EXPRESS_API ATrainWaterTank : public ATrainModule
{
	GENERATED_BODY()
	
public:
	ATrainWaterTank();

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

	UPROPERTY(EditAnywhere, Category = "Interaction")
	class UBoxComponent* IAComp;


	UFUNCTION()
	void OnPlayerBeginOverlap (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	void ChangeTankColor();
};
