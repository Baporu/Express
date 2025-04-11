// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Exp_GameMode.h"
#include "SBS/Item.h"
#include "Tile.generated.h"

enum class ETileType : uint8
{
	Ground UMETA(DisplayName = "Ground"), // �ٴ�
	Wood UMETA(DisplayName = "Wood"), // ä�� ���� �ڿ�
	Stone UMETA(DisplayName = "Stone") // ä�� ���� �ڿ�
};

USTRUCT() // ����ü ����
struct FTileHeightData : public FTableRowBase // ��� UDatatable�� �� ������ ������ ǥ��ȭ
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Height; // ���̰� ���ڿ�
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
	virtual void Tick(float DeltaTime) override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	class UStaticMeshComponent* TileMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FSM)
	class UTile_FSM* TileFSM;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
	ETileType TileType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	AItem* ContainedItem = nullptr; //Ÿ�� �� ������

public:
	
	bool CanHarvest() const;
	void HarvestTile();
	void InitializeTile(ETileType Type) {TileType = Type;}
	void SetContainedItem(AItem* Item) {ContainedItem = Item;} //Set item
	AItem* GetContainedItem() const{ return ContainedItem;} // Get Item
};
