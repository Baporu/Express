// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SHS/TrainModule.h"
#include "TrainCargo.generated.h"

class AItem;
enum class EItemType : uint8;

/**
 * 
 */
UCLASS()
class EXPRESS_API ATrainCargo : public ATrainModule
{
	GENERATED_BODY()
	
public:
	ATrainCargo();

public:
	// ���� �����̳�
	UPROPERTY(VisibleAnywhere, Category = "Item")
	TArray<AItem*> Woods;
	// ö �����̳�
	UPROPERTY(VisibleAnywhere, Category = "Item")
	TArray<AItem*> Stones;

	// ��ȣ�ۿ� ��������(�� �پ��ִ���) Ȯ���ϴ� �Լ�
	bool CheckInteraction();
	// ��ȿ�� ������ �������� Ȯ���ϴ� �Լ�
	bool CheckAddResource(EItemType ResourceType);
	// ������ ������ �� �ִ��� Ȯ���ϴ� �Լ�
	bool CheckGetResource(EItemType ResourceType);

	void AddResource(TArray<AItem*> Resources);
	TArray<AItem*> GetResource(EItemType ResourceType);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_AttachResource(AItem* Resource);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_AttachResourceToParent(AItem* Resource, AActor* ParentActor);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DetachResource(AItem* Resource);

protected:
	// ȭ������ ���� �� �ִ� �ִ� ����
	UPROPERTY(EditAnywhere, Category = "Item")
	int32 MaxCount = 3;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* WoodComp;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* StoneComp;

private:

};
