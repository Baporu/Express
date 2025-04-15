// Fill out your copyright notice in the Description page of Project Settings.


#include "SBS/SBS_Player.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "Interface_Tile.h"
#include "SBS/Item.h"
#include "EngineUtils.h"
#include "Tile.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ASBS_Player::ASBS_Player()
{
    PrimaryActorTick.bCanEverTick = true;
    TempHandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TempHandMesh"));
    TempHandMesh->SetupAttachment(GetMesh());
}

void ASBS_Player::BeginPlay()
{
    Super::BeginPlay();
}

void ASBS_Player::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    //접촉시 0.5초마다 수확
    if (true) //TODO: 도구를 들고 있으면
    {
		HarvestTimer -= DeltaTime;
		if (HarvestTimer <= 0.f)
		{
			GetFrontTile();
			if (FrontTile && FrontTile->TileType != ETileType::Ground)
			{
				FrontTile->ReduceHP();
				HarvestTimer = 0.5f;
			}
			else
			{
				HarvestTimer = 0.5f;
			}
		}
    }
    else
    {

    }
    //손에 물건 안들고있으면 암것도 안함
    if (!HeldItem) return;
    //손에 물건 들고있을 때 바닥타일 확인
    GetCurrentTile();
    if (CurrentTile)
    {
        AItem* TargetItem = CurrentTile->GetContainedItem(); //손에 들고있는게 바닥 타일에 있는 아이템과 같으면
        if (TargetItem && HeldItem->ItemType == TargetItem->ItemType)
        {
            int TotalStack = HeldItem->ItemStack + TargetItem->ItemStack; // 스택 합친다.
            if (TotalStack <= 5)
            {
                HeldItem->SetStack(TotalStack);
                TargetItem->Destroy();
                CurrentTile->SetContainedItem(nullptr);
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
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(IMC_Player, 0);
        }
    }
}

void ASBS_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ASBS_Player::Move);
        EIC->BindAction(IA_Interact, ETriggerEvent::Completed, this, &ASBS_Player::Interact);
        EIC->BindAction(IA_Release, ETriggerEvent::Completed, this, &ASBS_Player::Release);
    }
}

void ASBS_Player::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();
    if (Controller != nullptr)
    {
        FVector DIr = (FVector::ForwardVector * MovementVector.Y) + (FVector::RightVector * MovementVector.X);
        if (!DIr.IsNearlyZero())
        {
            DIr.Normalize();
            FRotator TargetRotation = DIr.Rotation();
            TargetRotation.Pitch = 0;
            TargetRotation.Roll = 0;
            FRotator CurrentRotation = GetActorRotation();
            FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), RotationLerpRate);
            SetActorRotation(NewRotation);
        }
        const FVector ForwardDirection = FVector::ForwardVector;
        const FVector RightDirection = FVector::RightVector;
        AddMovementInput(ForwardDirection, MovementVector.Y);
        AddMovementInput(RightDirection, MovementVector.X);
    }
}

void ASBS_Player::Interact(const FInputActionValue& Value)
{

    AItem* TargetItem = nullptr;
    GetCurrentTile();
    // 1순위: CurrentTile
    if (CurrentTile)
    {
        TargetItem = CurrentTile->GetContainedItem(); //바닥타일에 있는 아이템이 타겟 아이템
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No CurrentTile"));
    }

    // 2순위: CurrentTile에 아이템 없으면 FrontTile 체크
    if (!TargetItem && CurrentTile)
    {
        GetFrontTile();
        if (FrontTile && FrontTile->TileType == ETileType::Ground)
        {
            TargetItem = FrontTile->GetContainedItem();
            UE_LOG(LogTemp, Log, TEXT("Checking FrontTile for item"));
        }
    }

    // 아이템 처리
    if (TargetItem)
    {
        if (HeldItem)
        {
            if (HeldItem->ItemType != TargetItem->ItemType)
            {
                HeldItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
                // 놓을 타일 선택: CurrentTile 우선, 없으면 FrontTile
                ATile* PlaceTile = CurrentTile ? CurrentTile : (FrontTile && FrontTile->TileType == ETileType::Ground ? FrontTile : nullptr);
                if (PlaceTile)
                {
                    HeldItem->SetActorLocation(PlaceTile->GetActorLocation());
                    PlaceTile->SetContainedItem(HeldItem);
                    TargetItem->AttachToComponent(TempHandMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
                    HeldItem = TargetItem;
                    UE_LOG(LogTemp, Warning, TEXT("Swapped Item: %s"), *HeldItem->GetName());
                }
            }
        }
        else
        {
            TargetItem->AttachToComponent(TempHandMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
            HeldItem = TargetItem;
            //타일에서 아이템 들었으니까 타일의 아이템 제거
            if (CurrentTile && CurrentTile->GetContainedItem() == TargetItem)
            {
                CurrentTile->SetContainedItem(nullptr);
            }
            else if (FrontTile && FrontTile->GetContainedItem() == TargetItem)
            {
                FrontTile->SetContainedItem(nullptr);
            }
            bIsholdingitem = true;
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No Item found"));
    }
}

void ASBS_Player::Release(const FInputActionValue& Value)
{
    //Q로 내려놓기
    if (!HeldItem) //안들고있으면 안함
    {
        return;
    }
    GetCurrentTile();
    if (!CurrentTile)
    {
        UE_LOG(LogTemp, Warning, TEXT("No CurrentTile")); 
        return;
    }
    if (CurrentTile->GetContainedItem())
    {
        UE_LOG(LogTemp, Warning, TEXT("CurrentTile already has item"));
        // TODO: 교체처리 추가
        return;
    }
    if (HeldItem)
    {
        HeldItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        HeldItem->SetActorTransform(CurrentTile->GetActorTransform());
        CurrentTile->SetContainedItem(HeldItem);
        HeldItem = nullptr;
        bIsholdingitem = false;
    }
}

void ASBS_Player::GetCurrentTile()
{
    CurrentTile = nullptr;
    FVector CurLoc = GetActorLocation();

    FVector Start = FVector(CurLoc.X, CurLoc.Y, CurLoc.Z + 200.f);
    FVector End = FVector(CurLoc.X, CurLoc.Y, CurLoc.Z - 100.f);
    FHitResult Hit;
    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility))
    {
        ATile* Tile = Cast<ATile>(Hit.GetActor());
        if (Tile && Tile->TileType == ETileType::Ground && Tile->TileType == ETileType::Stone) //타일이 채취 불가능일때
        {
            CurrentTile = Tile;
           //if (bDebugTiles)
           //{
           //
			//	UKismetSystemLibrary::DrawDebugBox(GetWorld(), Tile->GetActorLocation(), FVector(TileSize, TileSize, 10.f), FColor::Red, /FRotator::ZeroRotator, /1);
           //}
        }
		// else if (bDebugTiles) //타일이 채취 가능일때
		// {
		//     UKismetSystemLibrary::DrawDebugBox(GetWorld(), Tile->GetActorLocation(), FVector(TileSize, TileSize, 10.f), FColor::Green, /FRotator::ZeroRotator, /1);
		// }
    }
    else if (bDebugTiles)
    {
        //UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, End, FColor::Blue, 0.1f, 1.f); //아무것도 못치면
    }

    if (!CurrentTile)
    {
        UE_LOG(LogTemp, Warning, TEXT("No CurrentTile"));
    }
}

void ASBS_Player::GetFrontTile()
{
    FrontTile = nullptr;
    FVector CurLoc = GetActorLocation();
    FVector Forward = GetActorForwardVector();

    FVector ForwardLoc = CurLoc + Forward * TileSize;
    int ForwardTileX = FMath::RoundToInt(ForwardLoc.X / TileSize) * TileSize;
    int ForwardTileY = FMath::RoundToInt(ForwardLoc.Y / TileSize) * TileSize;
    FVector Start = FVector(ForwardTileX, ForwardTileY, CurLoc.Z + 200.f);
    FVector End = FVector(ForwardTileX, ForwardTileY, CurLoc.Z - 100.f);
    FHitResult Hit;
    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility))
    {
        ATile* Tile = Cast<ATile>(Hit.GetActor());
        if (Tile)
        {
            FrontTile = Tile;
        }
    }
    if (!FrontTile)
    {
        UE_LOG(LogTemp, Warning, TEXT("No FrontTile"));
    }
}