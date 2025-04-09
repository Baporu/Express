// Fill out your copyright notice in the Description page of Project Settings.


#include "SBS/SBS_Player.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "Interface_Tile.h"
#include "SBS/Item.h"
#include "EngineUtils.h"

// Sets default values
ASBS_Player::ASBS_Player()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASBS_Player::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASBS_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASBS_Player::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC_Player, 0);
		}
	}
}

// Called to bind functionality to input
void ASBS_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ASBS_Player::Move);
		EIC->BindAction(IA_Interact, ETriggerEvent::Started, this, &ASBS_Player::Interact);
	}
}

void ASBS_Player::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		FVector DIr = (FVector::ForwardVector * MovementVector.Y) + (FVector::RightVector * MovementVector.X);
		// find out which way is forward
		if (!DIr.IsNearlyZero())
		{
			DIr.Normalize();
			FRotator TargetRotation = DIr.Rotation();
			TargetRotation.Pitch = 0;
			TargetRotation.Roll = 0;

			FRotator CurrentRotation = GetActorRotation();
			FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), RotationLerpRate); // 회전 속도 조절
			SetActorRotation(NewRotation);  // 이동 방향으로 캐릭터 회전
		}
		const FVector ForwardDirection = FVector::ForwardVector;
		const FVector RightDirection = FVector::RightVector;

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ASBS_Player::Interact(const FInputActionValue& Value)
{

	FVector CurLoc = GetActorLocation();
	FVector Forward = GetActorForwardVector();// 전방방향

	int CurrentTileX = FMath::FloorToInt(CurLoc.X / TileSize); // 정수로 내림(나누기)
	int CurrentTileY = FMath::FloorToInt(CurLoc.Y / TileSize);
	UE_LOG(LogTemp, Log, TEXT("CurrentTile: (%d, %d)"),CurrentTileX, CurrentTileY);

	AItem* TargetItem = nullptr;
	
	for (TActorIterator<AItem> Ite(GetWorld()); Ite; ++Ite)
	{
		AItem* Item = *Ite;
		if (Item)
		{
			FVector ItemLoc = Item->GetActorLocation();
			int ItemTileX = FMath::FloorToInt(ItemLoc.X / TileSize); // 정수로 내림(나누기)
			int ItemTileY = FMath::FloorToInt(ItemLoc.Y / TileSize); // 정수로 내림(나누기)
			if (ItemTileX == CurrentTileX && ItemTileY == CurrentTileY)
			{
				TargetItem = Item;
				break;
			}
		}
	}
	if (!TargetItem)
	{
		float ClosestDistance = InteractRadius * InteractRadius;
		for (TActorIterator<AItem> Ite(GetWorld()); Ite; ++Ite)
		{
			AItem* Item = *Ite;
			if (Item)
			{
				FVector ItemLoc = Item->GetActorLocation();
				float Distance = FVector::DistSquaredXY(CurLoc, ItemLoc);

				if (Distance <= ClosestDistance)
				{
					FVector DirectionToItem = (ItemLoc - CurLoc).GetSafeNormal();
					float DotProduct = FVector::DotProduct(Forward, DirectionToItem); //cos 값이기 때문에 0보다 크다 = +- 90도 

					if (DotProduct > 0.5) // -60~60
					{
						TargetItem = Item;
						ClosestDistance = Distance; // 더 가까운 아이템 우선
					}
				}
			}
		}
	}
	if (TargetItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("TargetItem: %s"), *TargetItem->GetName());

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Item Found"));
	}

}

