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
#include "SHS/TrainCargo.h"
#include "SHS/TrainCrafter.h"
#include "../Express.h"

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

    //����Ÿ�Ͽ� ���� ����
    GetCurrentTile();
	AItem* AxeItem;
    FVector SpawnLocation = CurrentTile->GetActorLocation();
    TArray<AItem*> TempItem;
    SpawnLocation.Z += 100;
    AxeItem = GetWorld()->SpawnActor<AItem>(AItem::StaticClass(), SpawnLocation, FRotator::ZeroRotator);
    AxeItem->CreateItem(EItemType::Axe);
    TempItem.Add(AxeItem);
    CurrentTile->SetContainedItem(TempItem);
    TempItem.Empty();

    //�� Ÿ�Ͽ� ��� ����
    GetFrontTile();
    AItem* PickaxeItem;
    SpawnLocation = FrontTile->GetActorLocation();
    SpawnLocation.Z += 100;
    PickaxeItem = GetWorld()->SpawnActor<AItem>(AItem::StaticClass(), SpawnLocation, FRotator::ZeroRotator);
    AxeItem->CreateItem(EItemType::Pickaxe);
    TempItem.Add(PickaxeItem);
    FrontTile->SetContainedItem(TempItem);
}

void ASBS_Player::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    //�տ� ���� �ȵ�������� �ϰ͵� ����
    if (HoldItems.IsEmpty()) return;
    
    if (!(HoldItems.IsEmpty()) && HoldItems[0]->IsTool) //TODO: ������ ��� ������ 0.5�ʸ��� ��Ȯ
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
    if(!(HoldItems[0]->IsTool)) //�����̸�
    {
        //�տ� ���� ������� �� �ٴ�Ÿ�� Ȯ��
        GetCurrentTile();
        TArray<AItem*> TargetItem;
        //helditme�� �ٴ� Ÿ�Ͽ� �ִ� �����۰� ������ attach�ϰ� helditem���ÿ� �߰�.
        if (CurrentTile)
        {
            if (!(CurrentTile->GetContainedItem().IsEmpty())) {
                TargetItem = CurrentTile->GetContainedItem();
                if (!TargetItem.IsEmpty() && HoldItems[0]->ItemType == TargetItem[0]->ItemType)
                {
                    //attach
                    TargetItem[0]->AttachToActor(HoldItems.Top(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(TEXT("ItemHead")));
                    //�迭�� �߰�
                    HoldItems.Append(TargetItem);
                    CurrentTile->RemoveContainedItem();
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
            //�ٴڿ� �ִ� �����۰� ��� �ִ� ������ Ÿ���� ���� ������. 
            //====��ü====
            if (HoldItems.Top()->ItemType != TargetItem[0]->ItemType)
            {
                ATile* PlaceTile = CurrentTile;
                TArray<AItem*> temp = TargetItem;
                if (PlaceTile && PlaceTile->TileType == ETileType::Ground)
                {
                    //�տ��� ����.
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
    //��������
    if (HoldItems.Num() == 0) //�ȵ�������� ����
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
                // TODO: ��üó�� �߰�
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
        UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, End, FLinearColor::Blue, 1, 15);

        // ���� �켱 Ž��
        if (ATrainCargo* cargo = Cast<ATrainCargo>(Hit.GetActor())) {
            // �տ� �� ��� ������ �ֱ�
            if (!HoldItems.IsEmpty()) {
                UE_LOG(LogTrain, Log, TEXT("Player Interaction: Cargo"));

                if (cargo->CheckAddResource()) {
                    for (int i = 0; i < HoldItems.Num(); i++) {
                        HoldItems[i]->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
                        cargo->AddResource(HoldItems.Top());
                        HoldItems.Pop();
                    }
                }
            }
            // �տ� ������ ������ �õ�
            else {
                UE_LOG(LogTrain, Log, TEXT("Hit Component: %s"), *Hit.GetComponent()->GetName());

                // ���� ��Ұ� ������ ����, ���̸� �� �������� (2���� Box Collider�� ���еǾ� ����)
                if (Hit.GetComponent()->GetName().Contains("Wood")) {
                    UE_LOG(LogTrain, Log, TEXT("Player Interaction: Cargo Wood"));

                    for (int i = 0; i < 4; i++) {
                        if (!cargo->CheckGetResource(EItemType::Wood)) return;

                        HoldItems.Add(cargo->GetResource(EItemType::Wood));
                        HoldItems[i]->AttachToComponent(TempHandMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
                    }
				}
                else {
                    UE_LOG(LogTrain, Log, TEXT("Player Interaction: Cargo Stone"));

                    for (int i = 0; i < 4; i++) {
                        if (!cargo->CheckGetResource(EItemType::Stone)) return;
     
                        HoldItems.Add(cargo->GetResource(EItemType::Stone));
                        HoldItems[i]->AttachToComponent(TempHandMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
                    }
                }
            }
        }

        else if (ATrainCrafter* crafter = Cast<ATrainCrafter>(Hit.GetActor())) {
            UE_LOG(LogTrain, Log, TEXT("Player Interaction: Crafter"));

            if (!HoldItems.IsEmpty()) return;

            if (!crafter->CheckRail()) return;

            UE_LOG(LogTrain, Log, TEXT("Player Interaction: Crafter Check Succeeded"));
            HoldItems.Add(crafter->GetRail());
            HoldItems[0]->AttachToComponent(TempHandMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
//             for (int i = 0; i < 4; i++)
//                 HoldItems[i] = crafter->GetRail();
        }
        
        else if (ATile* Tile = Cast<ATile>(Hit.GetActor()))
        {
            FrontTile = Tile;
        }
    }
    if (!FrontTile)
    {
        //UE_LOG(LogTemp, Warning, TEXT("No FrontTile"));
    }
}