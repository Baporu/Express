// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile_FSM.h"
#include "Components/SceneComponent.h"

// Sets default values for this component's properties
UTile_FSM::UTile_FSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTile_FSM::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UTile_FSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	switch (CurrentState)
	{
	case ETileState::Empty: EmptyState();
		break;
	case ETileState::Unmined: UnminedState();
		break;
	case ETileState::HasItem: HasItemState();
		break;
	default:
		break;
	}
}

void UTile_FSM::EmptyState()
{
	//if()
}

void UTile_FSM::UnminedState()
{

}

void UTile_FSM::HasItemState()
{
	if (HasChildWithTag("Item"))
	{
		//Item가 자식이면
	}
	else
	{
		CurrentState = ETileState::Empty;
	}
}

bool UTile_FSM::HasChildWithTag(FName Tag)
{
	//for (USceneComponent* Child : GetAttachChildren())
	//{
	//	if (Child->ComponentHasTag(Tag))
	//	{
	//		return true;
	//	}
	//}
	//return false;
	return false;
}

