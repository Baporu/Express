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
	Ground	UMETA(DisplayName = "Ground"),	// �ٴ�
	Wood	UMETA(DisplayName = "Wood"),	// ä�� ���� �ڿ�
	Stone	UMETA(DisplayName = "Stone"),	// ä�� ���� �ڿ�
	Rock	UMETA(DisplayName = "Rock"),	// ä�� �Ұ���
	Rail	UMETA(DisplayName = "Rail"),	// ����
	Water	UMETA(DisplayName = "Water"),	// ��
	Station_A UMETA(DisplayName = "Station_A"),	// ��� ��
	Station_Z UMETA(DisplayName = "Station_Z")	// ���� ��
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	class UStaticMesh* WaterMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	class UStaticMesh* StationMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FSM)
	class UTile_FSM* TileFSM;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TArray<AItem*> ContainedItem; //Ÿ�� �� ������

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


	bool CheckContainedItem() { if (ContainedItem.IsEmpty()) return false; else return true; };
	void SetContainedItem(TArray<AItem*> Item) {ContainedItem = Item;} //Set item
	TArray<AItem*> GetContainedItem() const{ return ContainedItem;} // Get Item
	void RemoveContainedItem() { ContainedItem.Empty(); };

	
public:
	UPROPERTY(EditAnywhere)
	bool bIsPassed = false;

	UPROPERTY(VisibleAnywhere, Category = "Tile")
	int32 gridRow = -1;
	UPROPERTY(VisibleAnywhere, Category = "Tile")
	int32 gridColumn = -1;

	UPROPERTY(VisibleAnywhere, Category = "Grid")
	class AGridManager* GridManager;

	bool CheckRail();
	void SetRail();
};
