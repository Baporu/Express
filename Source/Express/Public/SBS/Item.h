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
	UStaticMeshComponent* MeshComp; //스태틱매쉬
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMesh* WoodMesh; //나무 mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMesh*  StoneMesh; //돌 mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMesh* RailMesh; //레일 mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMesh* AxeMesh; //도끼 mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMesh* PickaxeMesh; //곡괭이 mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMesh* BucketMesh; //양동이 mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMesh* BucketMesh_Empty; //빈 양동이 mesh


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	class UMaterialInterface* WoodMaterial; //나무 material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	class UMaterialInterface* StoneMaterial; //돌 material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	class UMaterialInterface* RailMaterial; //레일 material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	class UMaterialInterface* AxeMaterial; //도끼 material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	class UMaterialInterface* PickaxeMaterial; //곡괭이 material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	class UMaterialInterface* BucketMaterial; //양동이 material

public:
	void UpdateMeshMat();
	void CreateItem(EItemType Type);

//네트워크
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicatedusing=OnRep_ItemType, EditAnywhere, BlueprintReadWrite)
	EItemType ItemType; //아이템 타입
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
