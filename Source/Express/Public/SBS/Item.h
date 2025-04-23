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

public:
	void UpdateMeshMat();
	void CreateItem(EItemType Type);

//��Ʈ��ũ
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicatedusing=OnRep_ItemType, EditAnywhere, BlueprintReadWrite)
	EItemType ItemType; //������ Ÿ��
	UPROPERTY(Replicatedusing = OnRep_IsTool, EditAnywhere, BlueprintReadWrite)
	bool IsTool = false;
	UPROPERTY(Replicatedusing = OnRep_IsBucketEmpty, EditAnywhere, BlueprintReadWrite)
	bool IsBucketEmpty = true;
	UPROPERTY(Replicated, EditAnywhere, Category = "Item")
    bool bHasWater = false;

	UFUNCTION()
	void OnRep_ItemType();
	UFUNCTION()
	void OnRep_IsTool();
	UFUNCTION()
	void OnRep_IsBucketEmpty();

	UFUNCTION(Server, Reliable)
	void Server_Attach(AActor* TargetActor, FName SocketName);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Attach(AActor* TargetActor, FName SocketName);

	UFUNCTION(Server, Reliable)
	void Server_SetItemLocation(const FVector& Newlocation);
	UFUNCTION(Server, Reliable)
	void Server_Detach();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Detach();
	UFUNCTION(Server, Reliable)
	void Server_CreateItem(EItemType Type);
	//UFUNCTION(NetMulticast, Reliable)
	//void Multicast_UpdateMeshMat();
};
