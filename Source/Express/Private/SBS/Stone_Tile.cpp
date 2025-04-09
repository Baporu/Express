// Fill out your copyright notice in the Description page of Project Settings.


#include "SBS/Stone_Tile.h"

// Sets default values
AStone_Tile::AStone_Tile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AStone_Tile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStone_Tile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AStone_Tile::PressKey()
{

}

void AStone_Tile::DestroyTile()
{

}

