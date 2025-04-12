// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputActionValue.h"
#include "Item.h"
#include "Tile.h"
#include "SBS_Player.generated.h"

UCLASS()
class EXPRESS_API ASBS_Player : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASBS_Player();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	
	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	class UStaticMeshComponent* TempHandMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Collision")
	class UBoxComponent* FrontBoxcomp;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* IMC_Player;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Move;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Interact;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Release;

	UPROPERTY(EditAnywhere)
	float MoveSpeed = 600.0f;
	UPROPERTY(EditDefaultsOnly)
	float RotationLerpRate = 10;
	UPROPERTY(EditDefaultsOnly)
	float InteractRadius = 150;

	float TileSize = 100;

	AItem* HeldItem = nullptr;
	bool bIsholdingitem = false;


public:

	void Move(const FInputActionValue& Value);
	void Interact(const FInputActionValue& Value);
	void Release(const FInputActionValue& Value);
	void GetGroundTile(ATile*& GroundTile) const;
	void HarvestTile(UPrimitiveComponent*OverlappedComponent, AActor* OtherActor, UPrimitiveComponent*OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
