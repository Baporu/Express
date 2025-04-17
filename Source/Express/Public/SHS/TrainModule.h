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

	// ������ ��� �迭�� ���� Ŭ����
	UPROPERTY(VisibleAnywhere, Category = "Module")
	ATrainEngine* TrainEngine;
	// ��� �迭�� �� �ε���
	UPROPERTY(EditAnywhere, Category = "Module")
	int32 ModuleNumber = -1;

	// ���� �̵� ��ǥ
	UPROPERTY(EditAnywhere, Category = "Move")
	FVector NextPos = FVector::ZeroVector;
	TQueue<FVector> TileQueue;
	TQueue<double> RotatorQueue;
	double NextRot = 90.0;
	// ��� �̵� �ӵ�, �������� �ʱ�ȭ
	UPROPERTY(VisibleAnywhere, Category = "Move")
	float ModuleSpeed = 25.0f;
	UPROPERTY(EditAnywhere, Category = "Tile")
	class AGridManager* GridManager;
	class ATile* CurrentTile;
	// ���� ��ǥ Ÿ���� Row �ε���
	int32 RowIndex = -1;
	// ���� ��ǥ Ÿ���� Column �ε���
	int32 ColIndex = -1;

	// ȭ�� ���� ����
	UPROPERTY(EditAnywhere, Category = "Fire")
	float FireTime = 15.0f;
	// �� �پ����� ��� ����
	UPROPERTY(EditAnywhere, Category = "Fire")
	bool bOnFire = false;
	// ȭ�� ����Ʈ
	UPROPERTY(EditAnywhere, Category = "Fire")
	class UParticleSystem* FireEffect;
	// ȭ�� ����Ʈ �ν��Ͻ� ��Ƶ� ����
	class UParticleSystemComponent* FireComp;


	void CheckNextTile();

	virtual void MoveTrain(float DeltaTime);
	void RotateTrain(float DeltaTime);

	virtual void OnFire(float DeltaTime);

	UFUNCTION()
	virtual void OnWaterBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
