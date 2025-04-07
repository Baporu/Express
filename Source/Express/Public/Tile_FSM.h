// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Tile_FSM.generated.h"

UENUM()
enum class ETileState: uint8
{
	Empty UMETA(Displayname = "Empty"),
	Unmined UMETA(DisplayName = "Unmined"),
	//HasTrack UMETA(DisplayName = "HasTrack"),
	HasItem UMETA(DisplayName = "HasItem")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EXPRESS_API UTile_FSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTile_FSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UPROPERTY(VisibleAnywhere)
	ETileState CurrentState = ETileState::Empty;

	void EmptyState(); // 비어있을 때
	void UnminedState(); // 나무 , 돌 위에 있을 때
	void HasItemState(); // 아이탬이 있을때
	bool HasChildWithTag(FName Tag);
		
};
