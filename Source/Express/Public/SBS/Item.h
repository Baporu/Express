// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Wood UMETA(DisplayName = "Wood"), //0
	Stone UMETA(DisplayName = "Stone"), //1
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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* MeshComp; //����ƽ�Ž�
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMesh* WoodMesh; //���� mesh

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMesh*  StoneMesh; //�� mesh

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	class UMaterialInterface* WoodMaterial; //���� material

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	class UMaterialInterface* StoneMaterial; //�� material

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemType ItemType; //������ Ÿ��

public:
	void UpdateMeshMat();
	void CreateItem(EItemType Type);

	void StackItem();
	
};
