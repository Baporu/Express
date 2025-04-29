// Fill out your copyright notice in the Description page of Project Settings.


#include "SBS/SBS_Player.h"
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
#include "ClearAnimWidget.h"


// Sets default values
ASBS_Player::ASBS_Player()
{
    PrimaryActorTick.bCanEverTick = true;
    MyArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MyArrowMesh"));
    MyArrowMesh->SetupAttachment(RootComponent);

    MyArrowMesh->SetVisibility(false);
    MyArrowMesh->SetOnlyOwnerSee(false);
    MyArrowMesh->bHiddenInGame = true;
    MyArrowMesh->SetIsReplicated(false); //���� ����

    //��Ʈ��ũ
    SetNetUpdateFrequency(100);
    bReplicates = true;
	SetReplicateMovement(true);
}

void ASBS_Player::BeginPlay()
{
    Super::BeginPlay();

    //ȸ�� ���� �ʱ�ȭ
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
	auto pc = Cast<APlayerController>(GetController());
	if (pc && pc->IsLocalController())
	{
        MyArrowMesh->SetVisibility(true);
        MyArrowMesh->bHiddenInGame = false;
	}
}

void ASBS_Player::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // ���� ������ ��ȣ�ۿ� ����
    if (bIsEnded) return;

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
	else if(HasAuthority())
	{
		//�տ� ���� ������� �� �ٴ�Ÿ�� Ȯ��
		GetCurrentTile();
		TArray<AItem*> TargetItem;
		//helditme�� �ٴ� Ÿ�Ͽ� �ִ� �����۰� ������ attach�ϰ� helditem���ÿ� �߰�.
		if (CurrentTile)
		{
            TargetItem = CurrentTile->GetContainedItem();
			if(!TargetItem.IsEmpty() && HoldItems[0]->ItemType == TargetItem[0]->ItemType)
			{

				Server_AttachItems(TargetItem[0]);
				HoldItems.Append(TargetItem);
				CurrentTile->Server_SetContainedItem(TArray<AItem*>());
            
			}
			
		}
	}

    //Ŭ��� ȸ�� ����
	//if (!HasAuthority())
	//{
	//	FRotator CurrentRotaion = GetActorRotation();
	//	FRotator TargetRotation();
	//	FRotator NewRotation = FMath::RInterpTo(CurrentRotaion, TargetRotation, //DeltaTime, RotationLerpRate);
	//	SetActorRotation(NewRotation);
	//}


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
    // ���� ������ ��ȣ�ۿ� ����
    if (bIsEnded) { PRINTLOG(TEXT("Input Disabled, Can't Move")); return; }

    FVector2D MovementVector = Value.Get<FVector2D>();
    if (Controller != nullptr)
    {
        FVector DIr = (FVector::ForwardVector * MovementVector.Y) + (FVector::RightVector * MovementVector.X);
        FRotator TargetRotation = GetActorRotation();
        if (!DIr.IsNearlyZero())
        {
            DIr.Normalize();
            TargetRotation = DIr.Rotation();
            TargetRotation.Pitch = 0;
            TargetRotation.Roll = 0;
            SetActorRotation(TargetRotation); //
            //������
            if (HasAuthority())
            {
                //Ÿ�ٷ����̼��� �޾Ƽ� rep yaw�� �ְ�
                //Rep_Yaw = TargetRotation.Yaw;
                //ȸ���Ѵ�.

    
            }
            //Ŭ���
            else
            {
                //
                //SetActorRotation(FRotator(0, TargetRotation.Yaw, 0));
                Server_UpdateRotation(TargetRotation);
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
    // ���� ������ ��ȣ�ۿ� ����
    if (bIsEnded) { PRINTLOG(TEXT("Input Disabled, Can't Interact")); return; }

    // ������ ���� ��ȣ�ۿ��� �õ��ϰ�, ���������� �߰� ��ȣ�ۿ��� ���� ����
    if (!IsLocallyControlled()) return;

    Server_FindTrain(HoldItems);
    if (bHasFound) return;
      
    //if(!HasAuthority())
    Server_Interact();
  
}

void ASBS_Player::Release(const FInputActionValue& Value)
{
    // ���� ������ ��ȣ�ۿ� ����
    if (bIsEnded) { PRINTLOG(TEXT("Input Disabled, Can't Release")); return; }

    //��������
   Server_Release();
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
        CurrentTile->Server_SetContainedItem(TempItem);
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
        FrontTile->Server_SetContainedItem(TempItem);
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
        RightTile->Server_SetContainedItem(TempItem);
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
            TempItem.Add(Rail);
            if(i>0)
            Rail->Server_Attach(TempItem[i-1], FName(TEXT("ItemHead")));
        }
        LeftTile->Server_SetContainedItem(TempItem);
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
        //UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, End, FLinearColor::Red, 0.01, 15);
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
        //UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, End, FLinearColor::Blue, 1, 15);

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
        //UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, End, FLinearColor::Red, 0.01, 15);
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
        //UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, End, FLinearColor::Red, 0.01, 15);
        if (HitTile) //Ÿ���� ä�� �Ұ����϶�
        {
            LeftTile = HitTile;
            UE_LOG(LogTemp, Warning, TEXT("Current Tile HIt!!!"));

        }
    }
}

bool ASBS_Player::FindTrain()
{
    Server_FindTrain(HoldItems);

    return bHasFound;

    {/*
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

                HoldItems[0]->Server_Detach();
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
                    HoldItems[0]->Server_Attach(this, TEXT("TempHandMesh"));
                }

                else {
                    UE_LOG(LogTrain, Log, TEXT("Player Interaction: Cargo Stone"));

                    if (!cargo->CheckGetResource(EItemType::Stone)) return false;

                    HoldItems.Append(cargo->GetResource(EItemType::Stone));
                    HoldItems[0]->Server_Attach(this, TEXT("TempHandMesh"));
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
            HoldItems[0]->Server_Attach(this, TEXT("TempHandMesh"));

            return true;
        }

        // ȭ������ �������� �ƴϴϱ� return false
        return false;
    */}
}

void ASBS_Player::Server_FindTrain_Implementation(const TArray<class AItem*>& PlayerItems) {
    // ���� �ʱ�ȭ
    bHasFound = false;

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
        return;

    Multicast_DrawRaycast(GetWorld(), Start, End, FLinearColor::Green, 1, 15);

    // ȭ�������� Ž��
    if (ATrainCargo* cargo = Cast<ATrainCargo>(Hit.GetActor())) {
        // ��ȣ�ۿ��� �� �ִ��� Ȯ�� (���� �پ������� return)
        if (!cargo->CheckInteraction()) return;

        // �տ� �� ��� ������ �ֱ�
        if (!PlayerItems.IsEmpty()) {
            PRINTTRAIN(TEXT("Player Interaction: Cargo"));

            // ��ȿ�� �ڿ����� Ȯ��
            if (!cargo->CheckAddResource(PlayerItems[0]->ItemType)) return;

            Client_DetachHoldItem(PlayerItems[0]);
            cargo->AddResource(PlayerItems);
            Multicast_RemoveHoldItem();

            bHasFound = true;
            return;
        }

        // �տ� ������ ������ �õ�
        else {
            // ���� ��Ұ� ������ ����, ���̸� �� �������� (2���� Box Collider�� ���еǾ� ����)
            if (Hit.GetComponent()->GetName().Contains("Wood")) {
                UE_LOG(LogTrain, Log, TEXT("Player Interaction: Cargo Wood"));

                if (!cargo->CheckGetResource(EItemType::Wood)) return;

                HoldItems.Append(cargo->GetResource(EItemType::Wood));
                Client_AttachHoldItem(HoldItems[0]);
            }

            else {
                UE_LOG(LogTrain, Log, TEXT("Player Interaction: Cargo Stone"));

                if (!cargo->CheckGetResource(EItemType::Stone)) return;

                HoldItems.Append(cargo->GetResource(EItemType::Stone));
                Client_AttachHoldItem(HoldItems[0]);
            }

            bHasFound = true;
            return;
        }
    }

    // ������ Ž��
    else if (ATrainCrafter* crafter = Cast<ATrainCrafter>(Hit.GetActor())) {
        UE_LOG(LogTrain, Log, TEXT("Player Interaction: Crafter"));

        // �տ� ������ ������ ��� ��ȣ�ۿ��� �ʿ� ����
        if (!HoldItems.IsEmpty()) return;

        // ������� ���� ������ ��� ��ȣ�ۿ��� �ʿ� ����
        if (!crafter->CheckRail()) return;

        UE_LOG(LogTrain, Log, TEXT("Player Interaction: Crafter Has Rails"));

        HoldItems.Append(crafter->GetRail());
        Client_AttachHoldItem(HoldItems[0]);

        bHasFound = true;
        return;
    }

    // ȭ������ �������� �ƴϴϱ� return false
    return;
}

void ASBS_Player::Client_AttachHoldItem_Implementation(class AItem* PlayerItem) {
    PlayerItem->AttachToComponent(this->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("HandSocket"));
}

void ASBS_Player::Client_DetachHoldItem_Implementation(class AItem* PlayerItem) {
    PlayerItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void ASBS_Player::Multicast_RemoveHoldItem_Implementation() {
    HoldItems.Empty();
}

void ASBS_Player::Multicast_DrawRaycast_Implementation(const UObject* WorldContextObject, FVector const LineStart, FVector const LineEnd, FLinearColor Color, float LifeTime, float Thickness) {
    //UKismetSystemLibrary::DrawDebugLine(WorldContextObject, LineStart, LineEnd, Color, LifeTime, Thickness);
}

void ASBS_Player::Server_RequestEndFire_Implementation(class ATrainModule* TrainModule) {
    TrainModule->Server_EndFire(this);
}

void ASBS_Player::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ASBS_Player, HoldItems); //HoldItems 
    DOREPLIFETIME(ASBS_Player, bIsholdingitem); //bIsholdingitem

    DOREPLIFETIME(ASBS_Player, bHasWater);
    //DOREPLIFETIME(ASBS_Player, ReplicatedRotation);

}

void ASBS_Player::Server_UpdateRotation_Implementation(const FRotator& NewRotation)
{
    //ReplicatedRotation = NewRotation;
    SetActorRotation(NewRotation);
    //ForceNetUpdate();
}

void ASBS_Player::Server_Interact_Implementation()
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
               
                if (PlaceTile && PlaceTile->TileType == ETileType::Ground)
                {
                    //�տ��� ����.
                    HoldItems[0]->SetOwner(this);
                    HoldItems[0]->Server_Detach();
                    FVector TargetPos = PlaceTile->GetActorLocation();
                    TargetPos.Z += 100;
                    HoldItems[0]->SetActorLocation(TargetPos);
                    //temp[0]->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
                    //temp[0]->AttachToComponent(TempHandMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
                    TargetItem[0]->SetOwner(this);
                    TargetItem[0]->Server_Attach(this, FName(TEXT("HandSocket")));

                    //TArray<AItem*> temp = TargetItem;
                    PlaceTile->Server_SetContainedItem(HoldItems);

                    HoldItems.Empty();
                    HoldItems.Append(TargetItem);

                    //HoldItems = TargetItem;
                    //UE_LOG(LogTemp, Warning, TEXT("Swapped Item: %s"), *TargetItem->ItemType);
                }
            }
        }
        else //���� �ȵ�� ������ =====�ݱ�=====
        {
            //attach �ϰ� ���ÿ� �߰�
            TargetItem[0]->Server_Attach(this, FName(TEXT("HandSocket")));
            HoldItems.Append(TargetItem);

            //Ÿ�Ͽ��� ������ ������ϱ� Ÿ���� ������ ����
            if (CurrentTile)
            {
                CurrentTile->Server_SetContainedItem(TArray<AItem*>());
            }
            else if (FrontTile)
            {
                FrontTile->Server_SetContainedItem(TArray<AItem*>());
            }
            bIsholdingitem = true;
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No Item found"));
    }
}

void ASBS_Player::Server_Release_Implementation()
{
    GetCurrentTile();
    if (HoldItems.IsEmpty()) return;
    if (!CurrentTile) return;
    if (!CurrentTile->GetContainedItem().IsEmpty()) return;

    FVector TargetPos = CurrentTile->GetActorLocation();
    TargetPos.Z += 100;

    if (HoldItems.Top()->ItemType == EItemType::Rail)
    {
        if (ATile* PreviousTile = CurrentTile->CheckRail())
        {
			HoldItems.Top()->Server_Detach();
			HoldItems.Top()->SetActorRotation(FRotator(0, 0, 0));
			HoldItems.Top()->SetActorLocation(TargetPos);
			CurrentTile->Server_SetRail(PreviousTile);
			HoldItems.Pop();
			return;
        }
    }

    HoldItems[0]->Server_Detach();
    HoldItems[0]->SetActorRotation(FRotator(0, 0, 0));
    HoldItems[0]->SetActorLocation(TargetPos);
    CurrentTile->Server_SetContainedItem(HoldItems);

    HoldItems.Empty();
    bIsholdingitem = false;
}

void ASBS_Player::Server_AttachItems_Implementation(AItem* TargetItem)
{
    if (TargetItem && HoldItems.Num() > 0)
    {
       TargetItem->Server_Attach(HoldItems.Top(), FName(TEXT("ItemHead")));
       //ForceNetUpdate();
    }
}
