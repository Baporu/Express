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
	Pickaxe UMETA(DisplayName = "Pickaxe"),
	Bucket  UMETA(DisplayName = "Bucket")
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
	UStaticMeshComponent* MeshComp; //����ƽ�Ž�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMesh* WoodMesh; //���� mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMesh*  StoneMesh; //�� mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMesh* RailMesh; //���� mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMesh* AxeMesh; //���� mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMesh* PickaxeMesh; //��� mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMesh* BucketMesh; //�絿�� mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMesh* BucketMesh_Empty; //�� �絿�� mesh


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	class UMaterialInterface* WoodMaterial; //���� material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	class UMaterialInterface* StoneMaterial; //�� material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	class UMaterialInterface* RailMaterial; //���� material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	class UMaterialInterface* AxeMaterial; //���� material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	class UMaterialInterface* PickaxeMaterial; //��� material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	class UMaterialInterface* BucketMaterial; //�絿�� material

	UPROPERTY(EditAnywhere)
	bool IsTool = false;

	UPROPERTY(EditAnywhere)
	bool IsBucketEmpty = true;
public:
	void UpdateMeshMat();
	void CreateItem(EItemType Type);

//��Ʈ��ũ

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicatedusing=OnRep_ItemType, EditAnywhere, BlueprintReadWrite)
	EItemType ItemType; //������ Ÿ��
	UFUNCTION()
	void OnRep_ItemType();
	UFUNCTION(Server, Reliable)
	void Server_CreateItem(EItemType Type);
	UFUNCTION(Server, Reliable)
	void Server_UpdateMeshMat();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateMeshMat();
};
