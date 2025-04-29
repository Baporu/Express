// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
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
	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	class UStaticMeshComponent* MyArrowMesh;
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
	UPROPERTY(EditAnywhere)
	bool bDebugTiles = true;

	float TileSize = 100;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	TArray<AItem*> HoldItems;

	ATile* CurrentTile;
	ATile* FrontTile;
	ATile* RightTile;
	ATile* LeftTile;

	UPROPERTY(Replicated, BlueprintReadOnly);
	bool bIsholdingitem = false;

	float HarvestTimer = 0;
	int HarvestCount = 0;
	ATile* LastHarvestTile = nullptr;
	float Rep_Yaw;

public:

	void Move(const FInputActionValue& Value);
	void Interact(const FInputActionValue& Value);
	void Release(const FInputActionValue& Value);

	void SetToolsOnGround();
	//void GetGroundTile_Location(ATile*& GroundTile) const;
	void GetCurrentTile();
	void GetFrontTile();
	void GetRightTile();
	void GetLeftTile();
	//void HarvestTile(UPrimitiveComponent*OverlappedComponent, AActor* OtherActor, UPrimitiveComponent*OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	// FindTrain()을 RPC로 바꾸면서 void로 바뀌어서 대신 넣은 변수
	bool bHasFound = false;

	// 게임 끝나는 거 탐지하는 변수
	UPROPERTY(VisibleAnywhere, Category = "State")
	bool bIsEnded = false;

	bool FindTrain();

 	UFUNCTION(Server, Reliable)
 	void Server_FindTrain(const TArray<class AItem*>& PlayerItems);
	UFUNCTION(NetMulticast, Unreliable)
	void Client_AttachHoldItem(class AItem* PlayerItem);
	UFUNCTION(Client, Unreliable)
	void Client_DetachHoldItem(class AItem* PlayerItem);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RemoveHoldItem();
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_DrawRaycast(const UObject* WorldContextObject, FVector const LineStart, FVector const LineEnd, FLinearColor Color, float LifeTime, float Thickness);

	UFUNCTION(Server, Reliable)
	void Server_RequestEndFire(class ATrainModule* TrainModule);


//네트워크
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	UFUNCTION(Server, Reliable)
	void Server_UpdateRotation(const FRotator& NewRotation);
	UFUNCTION(Server, Reliable)
	void Server_Interact();
	UFUNCTION(Server, Reliable)
	void Server_Release();
	UFUNCTION(Server, Reliable)
	void Server_AttachItems(AItem* TargetItem);

	UPROPERTY(Replicated, EditAnywhere, Category = Item)
	bool bHasWater = false;

	//UFUNCTION(Client,Reliable)
	//void ArrowVisible();




};
