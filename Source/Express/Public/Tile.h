// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Exp_GameMode.h"
#include "SBS/Item.h"
#include "Tile.generated.h"

UENUM(BlueprintType)
enum class ETileType : uint8
{
	Ground UMETA(DisplayName = "Ground"), // 바닥
	Wood UMETA(DisplayName = "Wood"), // 채취 가능 자원
	Stone UMETA(DisplayName = "Stone"), // 채취 가능 자원
	Rock UMETA(DisplayName = "Rock") // 채취 불가능
};
UCLASS()
class EXPRESS_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	class UStaticMeshComponent* TileMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Collision)
	class UBoxComponent* TileCollision;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	class UStaticMesh* GroundMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	class UStaticMesh* StoneMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	class UStaticMesh* WoodMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	class UStaticMesh* RockMesh;
	

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Material)
	//class UMaterial* WoodMat;
	//
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Material)
	//class UMaterial* StoneMat;
	//
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Material)
	//class UMaterial* GroundMat;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FSM)
	class UTile_FSM* TileFSM;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TArray<AItem*> ContainedItem; //타일 위 아이템

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
	ETileType TileType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTrigger = false;
	

	int MaxTileHP;
	int CurTileHP;


public:
	
	bool CanHarvest() const;
	void ReduceHP();
	
	void HarvestTile();

	void UpdateMeshMat();
	void CreateTile(ETileType Type);



	void SetContainedItem(TArray<AItem*> Item) {ContainedItem = Item;} //Set item
	TArray<AItem*> GetContainedItem() const{ return ContainedItem;} // Get Item
};
