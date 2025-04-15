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

    //���˽� 0.5�ʸ��� ��Ȯ
    if (true) //TODO: ������ ��� ������
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
    //�տ� ���� �ȵ�������� �ϰ͵� ����
    if (HoldItems.IsEmpty()) return;
    else
    {
        //�տ� ���� ������� �� �ٴ�Ÿ�� Ȯ��
        GetCurrentTile();
        TArray<AItem*> TargetItem;
        //helditme�� �ٴ� Ÿ�Ͽ� �ִ� �����۰� ������ attach�ϰ� helditem���ÿ� �߰�.
        if (CurrentTile && HoldItems.Num()<=5)
        {
            if(!(CurrentTile->GetContainedItem().IsEmpty()))//�ٴڿ� �������� ������
            TargetItem = CurrentTile->GetContainedItem();
       
            if (HoldItems[0]->ItemType == TargetItem[0]->ItemType) //����ִ� �������� �ٴ� �����۰� ������
            {
               //attach
               TargetItem[0]->AttachToActor(HoldItems.Top(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(TEXT("ItemHead")));
               //�迭�� �߰�
               HoldItems.Append(TargetItem);
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
    // 1����: CurrentTile
    if (CurrentTile)
    {
        TargetItem = CurrentTile->GetContainedItem(); //�ٴ�Ÿ�Ͽ� �ִ� �������� Ÿ�� ������
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No CurrentTile"));
    }

    // 2����: CurrentTile�� ������ ������ FrontTile üũ
    if (CurrentTile && TargetItem.IsEmpty())
    {
        GetFrontTile();
        if (FrontTile && FrontTile->TileType == ETileType::Ground)
        {
            TargetItem = FrontTile->GetContainedItem();
            UE_LOG(LogTemp, Log, TEXT("Checking FrontTile for item"));
        }
    }

    // ������ ó��
    if (!(TargetItem.IsEmpty()))
    {
        if (!HoldItems.IsEmpty()) //���� ���������
        {
            //====��ü====
            //�ٴڿ� �ִ� �����۰� ��� �ִ� ������ Ÿ���� ���� ������. 
            if (HoldItems.Top()->ItemType != TargetItem[0]->ItemType)
            {
                HoldItems[0]->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
                // ���� Ÿ�� ����: CurrentTile �켱, ������ FrontTile
                ATile* PlaceTile = CurrentTile;
                if (PlaceTile && PlaceTile->TileType == ETileType::Ground)
                {   
                    //��ġ ��ü.
                    TargetItem[0]->AttachToComponent(TempHandMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
                    HoldItems[0]->SetActorLocation(PlaceTile->GetActorLocation());
                    //���� ��ü.
                    PlaceTile->SetContainedItem(HoldItems);
                    HoldItems=TargetItem;
                    //UE_LOG(LogTemp, Warning, TEXT("Swapped Item: %s"), *TargetItem->ItemType);
                }
            }
        }
        else //���� �ȵ�� ������ =====�ݱ�=====
        {
            //attach �ϰ� ���ÿ� �߰�
            TargetItem[0]->AttachToComponent(TempHandMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
            HoldItems.Append(TargetItem);

            //Ÿ�Ͽ��� ������ ������ϱ� Ÿ���� ������ ����
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
    //Q�� ��������
    if (HoldItems.Num()== 0) //�ȵ�������� ����
    {
        return;
    }
    GetCurrentTile();
    if (!CurrentTile)
    {
        UE_LOG(LogTemp, Warning, TEXT("No CurrentTile")); 
        return;
    }
    if ((CurrentTile->GetContainedItem().IsEmpty()))
    {
        UE_LOG(LogTemp, Warning, TEXT("CurrentTile already has item"));
        // TODO: ��üó�� �߰�
        return;
    }
    if (HoldItems.Num()>0) //����ִٸ�
    {
        HoldItems[0]->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        HoldItems[0]->SetActorTransform(CurrentTile->GetActorTransform());
        CurrentTile->SetContainedItem(HoldItems);
        HoldItems.Empty();
        bIsholdingitem = false;
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
    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility,params))
    {
        ATile* HitTile = Cast<ATile>(Hit.GetActor());
		UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, End, FLinearColor::Red, 5, 30);
        if (HitTile) //Ÿ���� ä�� �Ұ����϶�
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
        UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, End, FLinearColor::Blue, 5, 30);
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