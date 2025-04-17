// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrainModule.generated.h"

class ATrainEngine;
class UBoxComponent;

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
	float FireTimer = 0.0f;

	void Init(ATrainEngine* EngineModule, float TrainSpeed, FVector Destination);
	void SetModuleIndex(int32 ModuleIndex);
	UBoxComponent* GetModuleComp();

	void StartFire();
	virtual void EndFire();

	void SetModuleLocation(FVector CurrentLocation);
	void SetModuleRotation(double CurrentYaw);

protected:
	UPROPERTY(EditAnywhere)
	class USceneComponent* SceneComp;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* MeshComp;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* ChainComp;
	UPROPERTY(EditAnywhere)
	UBoxComponent* ModuleComp;
	UPROPERTY(EditAnywhere)
	UBoxComponent* WaterComp;

	// 열차의 모듈 배열을 가진 클래스
	UPROPERTY(VisibleAnywhere, Category = "Module")
	ATrainEngine* TrainEngine;
	// 모듈 배열의 내 인덱스
	UPROPERTY(EditAnywhere, Category = "Module")
	int32 ModuleNumber = -1;

	// 다음 이동 좌표
	UPROPERTY(EditAnywhere, Category = "Move")
	FVector NextPos = FVector::ZeroVector;
	TQueue<FVector> TileQueue;
	TQueue<double> RotatorQueue;
	double NextRot = 90.0;
	// 모듈 이동 속도, 엔진에서 초기화
	UPROPERTY(VisibleAnywhere, Category = "Move")
	float ModuleSpeed = 25.0f;
	UPROPERTY(EditAnywhere, Category = "Tile")
	class AGridManager* GridManager;
	class ATile* CurrentTile;
	// 현재 목표 타일의 Row 인덱스
	int32 RowIndex = -1;
	// 현재 목표 타일의 Column 인덱스
	int32 ColIndex = -1;

	// 화재 로직 관련
	UPROPERTY(EditAnywhere, Category = "Fire")
	float FireTime = 15.0f;
	// 불 붙었으면 기능 정지
	UPROPERTY(EditAnywhere, Category = "Fire")
	bool bOnFire = false;
	// 화재 이펙트
	UPROPERTY(EditAnywhere, Category = "Fire")
	class UParticleSystem* FireEffect;
	// 화재 이펙트 인스턴스 담아둘 변수
	class UParticleSystemComponent* FireComp;


	void CheckNextTile();

	virtual void MoveTrain(float DeltaTime);
	void RotateTrain(float DeltaTime);

	virtual void OnFire(float DeltaTime);

	UFUNCTION()
	virtual void OnWaterBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
