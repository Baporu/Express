// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SHS/TrainModule.h"
#include "TrainCargo.generated.h"

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
	UPROPERTY(VisibleAnywhere, Category = "Resource")
	TArray<class AActor*> Woods;
	// ö �����̳�
	UPROPERTY(VisibleAnywhere, Category = "Resource")
	TArray<class AActor*> Irons;

	void AddResource(class AActor* Resource);

protected:
	// ȭ������ ���� �� �ִ� �ִ� ����
	UPROPERTY(EditAnywhere, Category = "Resource")
	int32 MaxCount = 3;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* WoodComp;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* IronComp;

private:

};
