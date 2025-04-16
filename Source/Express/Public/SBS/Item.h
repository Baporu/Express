// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Wood	UMETA(DisplayName = "Wood"),	// 0
	Stone	UMETA(DisplayName = "Stone"),	// 1
	Rail	UMETA(DisplayName = "Rail"),	// 2
	Axe		UMETA(DisplayName = "Axe"),
	Pickaxe UMETA(DisplayName = "Pickaxe")
};

UCLASS()
class EXPRESS_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* MeshComp; //½ºÅÂÆ½¸Å½¬
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMesh* WoodMesh; //³ª¹« mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMesh*  StoneMesh; //µ¹ mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMesh* RailMesh; //·¹ÀÏ mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMesh* AxeMesh; //µµ³¢ mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMesh* PickaxeMesh; //°î±ªÀÌ mesh

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	class UMaterialInterface* WoodMaterial; //³ª¹« material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	class UMaterialInterface* StoneMaterial; //µ¹ material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	class UMaterialInterface* RailMaterial; //·¹ÀÏ material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	class UMaterialInterface* AxeMaterial; //µµ³¢ material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	class UMaterialInterface* PickaxeMaterial; //°î±ªÀÌ material

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemType ItemType; //¾ÆÀÌÅÛ Å¸ÀÔ
	UPROPERTY(EditAnywhere)
	bool IsTool = false;
public:
	void UpdateMeshMat();
	void CreateItem(EItemType Type);

	void StackItem();
	
};
