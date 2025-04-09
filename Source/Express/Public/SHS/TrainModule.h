// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrainModule.generated.h"

UCLASS()
class EXPRESS_API ATrainModule : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrainModule();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void AttachModule(ATrainModule* TrainModule);
	void StartFire();
	void OnFire(float DeltaTime);
	void EndFire();

protected:
	UPROPERTY(EditAnywhere)
	class USceneComponent* SceneComp;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* MeshComp;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* ModuleComp;

	// ������ ��� �迭�� ���� Ŭ����
	UPROPERTY(EditAnywhere, Category = "Train")
	class ATrainEngine* TrainEngine;
	// ��� �迭�� �� �ε���
	UPROPERTY(EditAnywhere, Category = "Train")
	int32 ModuleNumber = -1;

	// ȭ�� ���� ����
	UPROPERTY(EditAnywhere, Category = "Fire")
	float FireTime = 15.0f;
	float FireTimer = 0.0f;
	// �� �پ����� ��� ����
	UPROPERTY(EditAnywhere, Category = "Fire")
	bool bOnFire = false;
	// ȭ�� ����Ʈ
	UPROPERTY(EditAnywhere, Category = "Fire")
	class UParticleSystem* FireEffect;
	// ȭ�� ����Ʈ �ν��Ͻ� ��Ƶ� ����
	class UParticleSystemComponent* FireComp;

};
