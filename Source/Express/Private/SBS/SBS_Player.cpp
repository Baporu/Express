// Fill out your copyright notice in the Description page of Project Settings.


#include "SBS/SBS_Player.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Interface_Tile.h"
#include "SBS/Item.h"
#include "EngineUtils.h"
#include "Tile.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SHS/TrainCargo.h"
#include "SHS/TrainCrafter.h"
#include "Net/UnrealNetwork.h"
#include "Express/Express.h"


// Sets default values
ASBS_Player::ASBS_Player()
{
    PrimaryActorTick.bCanEverTick = true;
    TempHandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TempHandMesh"));
    TempHandMesh->SetupAttachment(GetMesh());

    //��Ʈ��ũ
    SetNetUpdateFrequency(100);

    SetReplicates(true);
    SetReplicateMovement(true);


}

void ASBS_Player::BeginPlay()
{
    Super::BeginPlay();

    //ȸ�� ���� �ʱ�ȭ
    Rep_Yaw = GetActorRotation().Yaw;
    if (HasAuthority())
    {
        if (AExp_GameMode* gamemode = GetWorld()->GetAuthGameMode<AExp_GameMode>())
        {
            if (!gamemode->bIsToolsSpawned)
            {
                SetToolsOnGround();
                gamemode->bIsToolsSpawned = true;
            }
        }
    }
}

void ASBS_Player::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    //�տ� ���� �ȵ�������� �ϰ͵� ����
    if (HoldItems.IsEmpty()) return;
    //������ ��� �ֵ��
	if (HoldItems[0] && HoldItems[0]->IsTool)//��� �ִ°� ����(�絿�� ��� ����)
	{
		HarvestTimer -= DeltaTime;
		//0.5�ʸ���
		if (HarvestTimer <= 0.f)
		{
			GetFrontTile();
			if (FrontTile)
			{
                //����ִ°� �絿�̰� Ÿ���� ���̸�
				if (HoldItems[0]->ItemType == EItemType::Bucket && FrontTile->TileType == ETileType::Water) //��� �ִ°� �絿�̸�
				{
					//TODO: �絿�̿� �����
					if (FrontTile && FrontTile->CanHarvest() && FrontTile->TileType == ETileType::Water)
					{
						FrontTile->ReduceHP();
						if (FrontTile->CurTileHP == 0)
						{
							HoldItems[0]->IsBucketEmpty = false;
							HoldItems[0]->UpdateMeshMat();
                            bHasWater = true;

                            FrontTile->CurTileHP = FrontTile->MaxTileHP;
						}
						HarvestTimer = 0.5f;

					}
					else
					{
						HarvestTimer = 0.5f;
					}

				}
                // ��� �ִ°� ������ Ÿ���� ������
				else if (HoldItems[0]->ItemType == EItemType::Axe && FrontTile->TileType == ETileType::Wood) 
				{
					if (FrontTile && FrontTile->CanHarvest() && FrontTile)
					{
						FrontTile->ReduceHP();
						HarvestTimer = 0.5f;
					}
					else
					{
						HarvestTimer = 0.5f;
					}
				}
                // ��� �ִ°� ��̰� Ÿ���� ���̸�
				else if (HoldItems[0]->ItemType == EItemType::Pickaxe && FrontTile->TileType == ETileType::Stone) 
				{
					if (FrontTile && FrontTile->CanHarvest() && FrontTile)
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
		}
	}
    //��� �ִ°� �ڿ��̸�
	else 
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

    //Ŭ��� ȸ�� ����
	if (!HasAuthority())
	{
		FRotator CurrentRotaion = GetActorRotation();
        FRotator TargetRotation(0,Rep_Yaw, 0);
		FRotator NewRotation = FMath::RInterpTo(CurrentRotaion, TargetRotation, DeltaTime, RotationLerpRate);
		SetActorRotation(NewRotation);
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
            //������
            if (HasAuthority())
            {
                //Ÿ�ٷ����̼��� �޾Ƽ� rep yaw�� �ְ�
                Rep_Yaw = TargetRotation.Yaw;
                //ȸ���Ѵ�.
                SetActorRotation(FRotator(0, Rep_Yaw, 0)); //
    
            }
            //Ŭ���
            else
            {
                //
                SetActorRotation(FRotator(0, TargetRotation.Yaw, 0));
                Server_UdateRotation(TargetRotation.Yaw);
            }
        }
        const FVector ForwardDirection = FVector::ForwardVector;
        const FVector RightDirection = FVector::RightVector;
        AddMovementInput(ForwardDirection, MovementVector.Y);
        AddMovementInput(RightDirection, MovementVector.X);
    }
}

void ASBS_Player::Interact(const FInputActionValue& Value)
{
    // ������ ���� ��ȣ�ۿ��� �õ��ϰ�, ���������� �߰� ��ȣ�ۿ��� ���� ����
    if (FindTrain()) return;

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
            FVector TargetPos = CurrentTile->GetActorLocation();
            TargetPos.Z += 100;

            // ���� �������� ������ ���
            if (HoldItems.Top()->ItemType == EItemType::Rail) {
                // ���� ������ �������� Ȯ��
                if (ATile* PreviousTile = CurrentTile->CheckRail()) {
                    // ���� ������ �����ϸ� ���� �� ���θ� ������
                    HoldItems.Top()->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
                    HoldItems.Top()->SetActorRotation(FRotator(0, 0, 0));
                    HoldItems.Top()->SetActorLocation(TargetPos);

                    CurrentTile->SetRail(PreviousTile);
                    HoldItems.Pop();

                    return;
                }

                // ���� ������ �Ұ����ϸ� �׳� ���������� �ǹǷ� �߰� ���� ���� �ϴ� �ڵ� ����
            }

            HoldItems[0]->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
            HoldItems[0]->SetActorRotation(FRotator(0,0,0));
            HoldItems[0]->SetActorLocation(TargetPos);
            CurrentTile->SetContainedItem(HoldItems);
            HoldItems.Empty();
            bIsholdingitem = false;
        }
    }
}

void ASBS_Player::SetToolsOnGround()
{
    if(!HasAuthority()) return;



    //����Ÿ�Ͽ� ���� ����
    GetCurrentTile();
    if (CurrentTile)
    {
        AItem* AxeItem;
        FVector SpawnLocation = CurrentTile->GetActorLocation();
        TArray<AItem*> TempItem;
        SpawnLocation.Z += 100;
        AxeItem = GetWorld()->SpawnActor<AItem>(AItem::StaticClass(), SpawnLocation, FRotator::ZeroRotator);
        AxeItem->CreateItem(EItemType::Axe);
        TempItem.Add(AxeItem);
        CurrentTile->SetContainedItem(TempItem);
    }
    //�� Ÿ�Ͽ� ��� ����
    GetFrontTile();
    if (FrontTile)
    {

        AItem* PickaxeItem;
        FVector SpawnLocation = FrontTile->GetActorLocation();
        TArray<AItem*> TempItem;
        SpawnLocation.Z += 100;
        PickaxeItem = GetWorld()->SpawnActor<AItem>(AItem::StaticClass(), SpawnLocation, FRotator::ZeroRotator);
        PickaxeItem->CreateItem(EItemType::Pickaxe);
        TempItem.Add(PickaxeItem);
        FrontTile->SetContainedItem(TempItem);
    }
    //������ Ÿ�Ͽ� �絿�� ����
    GetRightTile();
    if (RightTile)
    {
        AItem* Bucket;
        FVector SpawnLocation = RightTile->GetActorLocation();
        TArray<AItem*> TempItem;
        SpawnLocation.Z += 100;
        Bucket = GetWorld()->SpawnActor<AItem>(AItem::StaticClass(), SpawnLocation, FRotator::ZeroRotator);
        Bucket->CreateItem(EItemType::Bucket);
        TempItem.Add(Bucket);
        RightTile->SetContainedItem(TempItem);
    }
    //���� Ÿ�Ͽ� ���� ����
    GetLeftTile();
    if (LeftTile)
    {
        AItem* Rail;
        FVector SpawnLocation = LeftTile->GetActorLocation();
        TArray<AItem*> TempItem;
        SpawnLocation.Z += 100;
        for (int i = 0; i < 3; i++)
        {
            Rail = GetWorld()->SpawnActor<AItem>(AItem::StaticClass(), SpawnLocation, FRotator::ZeroRotator);
            Rail->CreateItem(EItemType::Rail);
            TempItem.Push(Rail);
            if(i>0)
            Rail->AttachToActor(TempItem[i-1], FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(TEXT("ItemHead")));
        }
        LeftTile->SetContainedItem(TempItem);
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

        if (ATile* Tile = Cast<ATile>(Hit.GetActor()))
        {
            FrontTile = Tile;
        }
    }
    if (!FrontTile)
    {
        //UE_LOG(LogTemp, Warning, TEXT("No FrontTile"));
    }
}

void ASBS_Player::GetRightTile()
{
    RightTile = nullptr;
    FVector CurLoc = GetActorLocation();
    FVector Right = GetActorRightVector();

    FVector RightLoc = CurLoc + Right * TileSize;
    int RightTileX = FMath::RoundToInt(RightLoc.X / TileSize) * TileSize;
    int RightTileY = FMath::RoundToInt(RightLoc.Y / TileSize) * TileSize;
    FVector Start = FVector(RightTileX, RightTileY, CurLoc.Z + 200.f);
    FVector End = FVector(RightTileX, RightTileY, CurLoc.Z - 100.f);
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
            RightTile = HitTile;
            UE_LOG(LogTemp, Warning, TEXT("Current Tile HIt!!!"));

        }
    }
}

void ASBS_Player::GetLeftTile()
{
    LeftTile = nullptr;
    FVector CurLoc = GetActorLocation();
    FVector Left = GetActorRightVector()*-1;

    FVector LeftLoc = CurLoc + Left * TileSize;
    int LeftTileX = FMath::RoundToInt(LeftLoc.X / TileSize) * TileSize;
    int LeftTileY = FMath::RoundToInt(LeftLoc.Y / TileSize) * TileSize;
    FVector Start = FVector(LeftTileX, LeftTileY, CurLoc.Z + 200.f);
    FVector End = FVector(LeftTileX, LeftTileY, CurLoc.Z - 100.f);
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
            LeftTile = HitTile;
            UE_LOG(LogTemp, Warning, TEXT("Current Tile HIt!!!"));

        }
    }
}

bool ASBS_Player::FindTrain()
{
    // ���� ��ġ�� Ÿ�� ũ��� ������ Ÿ�� ��ġ�� ���
    FVector CurLoc = GetActorLocation();
    FVector ForwardLoc = CurLoc + GetActorForwardVector() * TileSize;

    // ���� Ÿ���� ��ġ�� Ÿ�� ũ�⿡ �°� ����
    int ForwardTileX = FMath::RoundToInt(ForwardLoc.X / TileSize) * TileSize;
    int ForwardTileY = FMath::RoundToInt(ForwardLoc.Y / TileSize) * TileSize;

    FVector Start = FVector(ForwardTileX, ForwardTileY, CurLoc.Z + 200.f);
    FVector End = FVector(ForwardTileX, ForwardTileY, CurLoc.Z - 100.f);
    FHitResult Hit;
    FCollisionQueryParams params;
    params.AddIgnoredActor(this);

    // LineTrace�� ������ ��� = ������ Ž������ �� ��
    if (!GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, params))
        return false;

    UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, End, FLinearColor::Green, 1, 15);

    // ȭ�������� Ž��
    if (ATrainCargo* cargo = Cast<ATrainCargo>(Hit.GetActor())) {
        // ��ȣ�ۿ��� �� �ִ��� Ȯ�� (���� �پ������� false�� return)
        if (!cargo->CheckInteraction()) return false;

        // �տ� �� ��� ������ �ֱ�
        if (!HoldItems.IsEmpty()) {
            UE_LOG(LogTrain, Log, TEXT("Player Interaction: Cargo"));

            // ��ȿ�� �ڿ����� Ȯ��
            if (!cargo->CheckAddResource(HoldItems[0]->ItemType)) return false;

            HoldItems[0]->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
            cargo->AddResource(HoldItems);
            HoldItems.Empty();

            return true;
        }

        // �տ� ������ ������ �õ�
        else {
            // ���� ��Ұ� ������ ����, ���̸� �� �������� (2���� Box Collider�� ���еǾ� ����)
            if (Hit.GetComponent()->GetName().Contains("Wood")) {
                UE_LOG(LogTrain, Log, TEXT("Player Interaction: Cargo Wood"));

                if (!cargo->CheckGetResource(EItemType::Wood)) return false;

                HoldItems.Append(cargo->GetResource(EItemType::Wood));
                HoldItems[0]->AttachToComponent(TempHandMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
            }

            else {
                UE_LOG(LogTrain, Log, TEXT("Player Interaction: Cargo Stone"));

                if (!cargo->CheckGetResource(EItemType::Stone)) return false;

                HoldItems.Append(cargo->GetResource(EItemType::Stone));
                HoldItems[0]->AttachToComponent(TempHandMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
            }

            return true;
        }
    }

    // ������ Ž��
    else if (ATrainCrafter* crafter = Cast<ATrainCrafter>(Hit.GetActor())) {
        UE_LOG(LogTrain, Log, TEXT("Player Interaction: Crafter"));

        // �տ� ������ ������ ��� ��ȣ�ۿ��� �ʿ� ����
        if (!HoldItems.IsEmpty()) return false;

        // ������� ���� ������ ��� ��ȣ�ۿ��� �ʿ� ����
        if (!crafter->CheckRail()) return false;

        UE_LOG(LogTrain, Log, TEXT("Player Interaction: Crafter Has Rails"));

        HoldItems.Append(crafter->GetRail());
        HoldItems[0]->AttachToComponent(TempHandMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

        return true;
    }

    // ȭ������ �������� �ƴϴϱ� return false
    return false;
}

void ASBS_Player::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ASBS_Player, Rep_Rotation); // 
    DOREPLIFETIME(ASBS_Player, HoldItems); //HoldItems 
    DOREPLIFETIME(ASBS_Player, bIsholdingitem); //bIsholdingitem

}

void ASBS_Player::Server_UdateRotation_Implementation(float NewYaw)
{
    Rep_Yaw = NewYaw;
    SetActorRotation(FRotator(0, Rep_Yaw, 0));
}

