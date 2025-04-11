// Fill out your copyright notice in the Description page of Project Settings.


#include "SBS/SBS_Player.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "Interface_Tile.h"
#include "SBS/Item.h"
#include "EngineUtils.h"
#include "Tile.h"

// Sets default values
ASBS_Player::ASBS_Player()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TempHandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TempHandMesh"));
	TempHandMesh->SetupAttachment(GetMesh());

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

	if (!HeldItem) return; // 안들고있을때

	ATile* GroundTile = nullptr; //발밑 타일 아직 할당 안됐으면
	GetGroundTile(GroundTile);

	if (GroundTile)
	{
		AItem* TargetItem = GroundTile->GetContainedItem(); // 바닥에 있는 아이템찾기
		if (TargetItem && HeldItem->ItemType == TargetItem->ItemType)
		{
			int TotalStack = HeldItem->ItemStack + TargetItem->ItemStack;
			if (TotalStack <= 5)
			{
				HeldItem->SetStack(TotalStack);
				TargetItem->Destroy();
				GroundTile->SetContainedItem(nullptr);
			}
			else
			{
				int PickStack = 5 - HeldItem->ItemStack;
				HeldItem->SetStack(5);
				TargetItem->SetStack(TargetItem->ItemStack - PickStack);
			}
		}
	}
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
		EIC->BindAction(IA_Release, ETriggerEvent::Started, this, &ASBS_Player::Release);
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

	ATile* GroundTile = nullptr; 
	GetGroundTile(GroundTile);
	
	//암것도 없으면 로그출력
	if (!GroundTile)
	{
		UE_LOG(LogTemp, Warning, TEXT("No ground No"));
		return;
	}

	AItem* TargetItem = GroundTile->GetContainedItem(); // 바닥에 있는 아이템찾기
	//아이템 들고있으면
	if (HeldItem)
	{
		// 바닥에 아이템이 있고, 바닥에 있는 아이템 타입과 다르면 "교체"
		if (TargetItem && HeldItem->ItemType != TargetItem->ItemType)
		{
			HeldItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			HeldItem->SetActorLocation(GroundTile->GetActorLocation());
			GroundTile->SetContainedItem(HeldItem); // 바닥에 들고있던 아이템 할당
			TargetItem->AttachToComponent(TempHandMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			HeldItem = TargetItem; 
			UE_LOG(LogTemp, Warning, TEXT("Swapped Item: %s"), *HeldItem->GetName());
		}
		////바닥에 아이템이 없으면
		//else if (!TargetItem)
		//{
		//	
		//}
	}
	//아이템 안들고있고, 바닥에 아이템 있으면 "들기"
	else if (TargetItem)
	{
		TargetItem->AttachToComponent(TempHandMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		HeldItem = TargetItem;
		GroundTile->SetContainedItem(nullptr);
		bIsholdingitem = true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Item"));
	}

}

void ASBS_Player::Release(const FInputActionValue& Value)
{
	if (!HeldItem)
	{
		return; // 안들고있으면 안함
	}

	ATile* GroundTile = nullptr;
	GetGroundTile(GroundTile);
	if (!GroundTile)
	{
		return;
	}
	if (GroundTile->GetContainedItem())
	{
		UE_LOG(LogTemp, Warning, TEXT("GroundTile already has item"));
		return; // 바닥에 이미 아이템이 있으면 안함
	}
	if (HeldItem)
	{
		HeldItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		HeldItem->SetActorLocation(GroundTile->GetActorLocation());
		HeldItem = nullptr;
		bIsholdingitem = false;
	}
}

void ASBS_Player::GetGroundTile(ATile*& GroundTile) const
{
	GroundTile = nullptr; // 초기화
	FVector CurLoc = GetActorLocation();
	FVector Forward = GetActorForwardVector();
	int CurrentTileX = FMath::FloorToInt(CurLoc.X / TileSize); // 내 X축
	int CurrentTileY = FMath::FloorToInt(CurLoc.Y / TileSize); // 내 Y축
	

	for (TActorIterator<ATile> Ite(GetWorld()); Ite; ++Ite)
	{
		ATile* Tile = *Ite;
		if (Tile && Tile->TileType == ETileType::Ground)
		{
			FVector TileLoc = Tile->GetActorLocation();
			int TileX = FMath::FloorToInt(TileLoc.X / TileSize); // 타일 X축
			int TileY = FMath::FloorToInt(TileLoc.Y / TileSize); // 타일 Y축
			if (TileX == CurrentTileX && TileY == CurrentTileY) // 내 위치와 타일 위치가 같으면
			{
				GroundTile = Tile; // 타일 할당
				UE_LOG(LogTemp, Log, TEXT("GroundTile: (%d, %d)"), TileX, TileY);
				break;
			}
		}
	}
	if (!GroundTile)
	{
		float ClosestDistance = InteractRadius * InteractRadius;
		for (TActorIterator<ATile> Ite(GetWorld()); Ite; ++Ite)
		{
			ATile* Tile = *Ite;
			if (Tile && Tile->TileType == ETileType::Ground)
			{
				FVector TileLoc = Tile->GetActorLocation();
				float Distance = FVector::DistSquared(CurLoc, TileLoc);
				if (Distance <= ClosestDistance)
				{
					FVector DirectionToTile = (TileLoc - CurLoc).GetSafeNormal();
					float DotProduct = FVector::DotProduct(Forward, DirectionToTile);
					if (DotProduct > 0.866f) // 60도
					{
						GroundTile = Tile;
						
						ClosestDistance = Distance;
					}
				}
			}
		}
	}

	
}

