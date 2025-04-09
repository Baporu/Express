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

	// 열차의 모듈 배열을 가진 클래스
	UPROPERTY(EditAnywhere, Category = "Train")
	class ATrainEngine* TrainEngine;
	// 모듈 배열의 내 인덱스
	UPROPERTY(EditAnywhere, Category = "Train")
	int32 ModuleNumber = -1;

	// 화재 로직 관련
	UPROPERTY(EditAnywhere, Category = "Fire")
	float FireTime = 15.0f;
	float FireTimer = 0.0f;
	// 불 붙었으면 기능 정지
	UPROPERTY(EditAnywhere, Category = "Fire")
	bool bOnFire = false;
	// 화재 이펙트
	UPROPERTY(EditAnywhere, Category = "Fire")
	class UParticleSystem* FireEffect;
	// 화재 이펙트 인스턴스 담아둘 변수
	class UParticleSystemComponent* FireComp;

};
