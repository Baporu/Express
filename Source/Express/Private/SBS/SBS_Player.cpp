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
            if (FrontTile && FrontTile->CanHarvest())
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
    if (HoldItems.IsEmpty()) return;
    else
    {
        //손에 물건 들고있을 때 바닥타일 확인
        GetCurrentTile();
        TArray<AItem*> TargetItem;
        //helditme이 바닥 타일에 있는 아이템과 같으면 attach하고 helditem스택에 추가.
        if (CurrentTile)
        {
            if (!(CurrentTile->GetContainedItem().IsEmpty())) {
                TargetItem = CurrentTile->GetContainedItem();
                if (!TargetItem.IsEmpty() && HoldItems[0]->ItemType == TargetItem[0]->ItemType)
                {
                    //attach
                    TargetItem[0]->AttachToActor(HoldItems.Top(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(TEXT("ItemHead")));
                    //배열에 추가
                    HoldItems.Append(TargetItem);
                }
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

    TArray<AItem*> TargetItem;
    TargetItem.Empty();
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
    if (CurrentTile && TargetItem.IsEmpty())
    {
        GetFrontTile();
        if (FrontTile && FrontTile->TileType == ETileType::Ground)
        {
            TargetItem = FrontTile->GetContainedItem();
            UE_LOG(LogTemp, Log, TEXT("Checking FrontTile for item"));
        }
    }

    // 아이템 처리
    if (!(TargetItem.IsEmpty()))
    {
        if (!HoldItems.IsEmpty()) //물건 들고있으면
        {
            //바닥에 있는 아이템과 들고 있는 아이템 타입이 같지 않으면. 
            //====교체====
            if (HoldItems.Top()->ItemType != TargetItem[0]->ItemType)
            {
                ATile* PlaceTile = CurrentTile;
                TArray<AItem*> temp = TargetItem;
                if (PlaceTile && PlaceTile->TileType == ETileType::Ground)
                {
                    //손에서 땐다.
                    HoldItems[0]->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
                  
                    FVector TargetPos = PlaceTile->GetActorLocation();
                    TargetPos.Z += 100;
                    HoldItems[0]->SetActorLocation(TargetPos);
                    //temp[0]->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
                    temp[0]->AttachToComponent(TempHandMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

                    PlaceTile->SetContainedItem(HoldItems);
                    HoldItems.Empty();
                    HoldItems.Append(temp);
                    
                    //HoldItems = TargetItem;
                    //UE_LOG(LogTemp, Warning, TEXT("Swapped Item: %s"), *TargetItem->ItemType);
                }
            }
        }
        else //물건 안들고 있으면 =====줍기=====
        {
            //attach 하고 스택에 추가
            TargetItem[0]->AttachToComponent(TempHandMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
            HoldItems.Append(TargetItem);

            //타일에서 아이템 들었으니까 타일의 아이템 제거
            if (CurrentTile)
            {
                CurrentTile->SetContainedItem(TArray<AItem*>());
            }
            else if (FrontTile)
            {
                FrontTile->SetContainedItem(TArray<AItem*>());
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
    //내려놓기
    if (HoldItems.Num() == 0) //안들고있으면 안함
    {
        UE_LOG(LogTemp, Warning, TEXT("have no Item"));
        return;
    }
    else
    {
        if (!CurrentTile)
        {
            UE_LOG(LogTemp, Warning, TEXT("No CurrentTile"));
            return;
        }
        else
        {
            if (!(CurrentTile->GetContainedItem().IsEmpty()))
            {
                UE_LOG(LogTemp, Warning, TEXT("CurrentTile already has item"));
                // TODO: 교체처리 추가
                return;
            }
            GetCurrentTile();
            HoldItems[0]->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
            HoldItems[0]->SetActorRotation(FRotator(0,0,0));
            FVector TargetPos = CurrentTile->GetActorLocation();
            TargetPos.Z += 100;
            HoldItems[0]->SetActorLocation(TargetPos);
            CurrentTile->SetContainedItem(HoldItems);
            HoldItems.Empty();
            bIsholdingitem = false;
        }
    }
}

void ASBS_Player::GetCurrentTile()
{
    CurrentTile = nullptr;
    FVector CurLoc = GetActorLocation();

    FVector Start = FVector(CurLoc.X, CurLoc.Y, CurLoc.Z + 200.f);
    FVector End = FVector(CurLoc.X, CurLoc.Y, CurLoc.Z - 200.f);
    FHitResult Hit;
    FCollisionQueryParams params;
    params.AddIgnoredActor(this);
    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, params))
    {
        UE_LOG(LogTemp, Warning, TEXT("Hit Actor Name: %s"), *Hit.GetActor()->GetActorNameOrLabel());
        ATile* HitTile = Cast<ATile>(Hit.GetActor());
        UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, End, FLinearColor::Red, 0.01, 15);
        if (HitTile) //타일이 채취 불가능일때
        {
            CurrentTile = HitTile;
            UE_LOG(LogTemp, Warning, TEXT("Current Tile HIt!!!"));

        }
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
    FCollisionQueryParams params;
    params.AddIgnoredActor(this);
    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, params))
    {
        UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, End, FLinearColor::Blue, 1, 15);
        ATile* Tile = Cast<ATile>(Hit.GetActor());
        if (Tile)
        {
            FrontTile = Tile;
        }
    }
    if (!FrontTile)
    {
        //UE_LOG(LogTemp, Warning, TEXT("No FrontTile"));
    }
}