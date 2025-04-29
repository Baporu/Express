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
    MyArrowMesh->SetIsReplicated(false); //복제 방지

    //네트워크
    SetNetUpdateFrequency(100);
    bReplicates = true;
	SetReplicateMovement(true);
}

void ASBS_Player::BeginPlay()
{
    Super::BeginPlay();

    //회전 변수 초기화
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

    // 게임 끝나면 상호작용 중지
    if (bIsEnded) return;

    //손에 물건 안들고있으면 암것도 안함
    if (HoldItems.IsEmpty()) return;
    //물건을 들고 있드면
	if (HoldItems[0] && HoldItems[0]->IsTool)//들고 있는게 도구(양동이 곡괭이 도끼)
	{
		HarvestTimer -= DeltaTime;
		//0.5초마다
		if (HarvestTimer <= 0.f)
		{
			GetFrontTile();
			if (FrontTile)
			{
                //들고있는게 양동이고 타일이 물이면
				if (HoldItems[0]->ItemType == EItemType::Bucket && FrontTile->TileType == ETileType::Water) //들고 있는게 양동이면
				{
					//TODO: 양동이에 물담기
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
                // 들고 있는게 도끼고 타일이 나무면
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
                // 들고 있는게 곡괭이고 타일이 돌이면
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
    //들고 있는게 자원이면
	else if(HasAuthority())
	{
		//손에 물건 들고있을 때 바닥타일 확인
		GetCurrentTile();
		TArray<AItem*> TargetItem;
		//helditme이 바닥 타일에 있는 아이템과 같으면 attach하고 helditem스택에 추가.
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

    //클라면 회전 보간
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
    // 게임 끝나면 상호작용 중지
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
            //서버는
            if (HasAuthority())
            {
                //타겟로케이션을 받아서 rep yaw에 넣고
                //Rep_Yaw = TargetRotation.Yaw;
                //회전한다.

    
            }
            //클라는
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
    // 게임 끝나면 상호작용 중지
    if (bIsEnded) { PRINTLOG(TEXT("Input Disabled, Can't Interact")); return; }

    // 기차와 먼저 상호작용을 시도하고, 성공했으면 추가 상호작용을 하지 않음
    if (!IsLocallyControlled()) return;

    Server_FindTrain(HoldItems);
    if (bHasFound) return;
      
    //if(!HasAuthority())
    Server_Interact();
  
}

void ASBS_Player::Release(const FInputActionValue& Value)
{
    // 게임 끝나면 상호작용 중지
    if (bIsEnded) { PRINTLOG(TEXT("Input Disabled, Can't Release")); return; }

    //내려놓기
   Server_Release();
}

void ASBS_Player::SetToolsOnGround()
{
    if(!HasAuthority()) return;

    //현재타일에 도끼 놓기
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
    //앞 타일에 곡괭이 놓기
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
    //오른쪽 타일에 양동이 놓기
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
    //왼쪽 타일에 레일 놓기
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
        if (HitTile) //타일이 채취 불가능일때
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
        if (HitTile) //타일이 채취 불가능일때
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
        // 현재 위치와 타일 크기로 전방의 타일 위치를 계산
        FVector CurLoc = GetActorLocation();
        FVector ForwardLoc = CurLoc + GetActorForwardVector() * TileSize;

        // 계산된 타일의 위치를 타일 크기에 맞게 보정
        int ForwardTileX = FMath::RoundToInt(ForwardLoc.X / TileSize) * TileSize;
        int ForwardTileY = FMath::RoundToInt(ForwardLoc.Y / TileSize) * TileSize;

        FVector Start = FVector(ForwardTileX, ForwardTileY, CurLoc.Z + 200.f);
        FVector End = FVector(ForwardTileX, ForwardTileY, CurLoc.Z - 100.f);
        FHitResult Hit;
        FCollisionQueryParams params;
        params.AddIgnoredActor(this);

        // LineTrace가 실패한 경우 = 기차를 탐지하지 못 함
        if (!GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, params))
            return false;

        UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, End, FLinearColor::Green, 1, 15);

        // 화물차부터 탐색
        if (ATrainCargo* cargo = Cast<ATrainCargo>(Hit.GetActor())) {
            // 상호작용할 수 있는지 확인 (불이 붙어있으면 false를 return)
            if (!cargo->CheckInteraction()) return false;

            // 손에 뭐 들고 있으면 넣기
            if (!HoldItems.IsEmpty()) {
                UE_LOG(LogTrain, Log, TEXT("Player Interaction: Cargo"));

                // 유효한 자원인지 확인
                if (!cargo->CheckAddResource(HoldItems[0]->ItemType)) return false;

                HoldItems[0]->Server_Detach();
                cargo->AddResource(HoldItems);
                HoldItems.Empty();

                return true;
            }

            // 손에 없으면 빼오기 시도
            else {
                // 보관 장소가 나무면 나무, 돌이면 돌 가져오기 (2개의 Box Collider로 구분되어 있음)
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

        // 제작차 탐색
        else if (ATrainCrafter* crafter = Cast<ATrainCrafter>(Hit.GetActor())) {
            UE_LOG(LogTrain, Log, TEXT("Player Interaction: Crafter"));

            // 손에 아이템 있으면 얘랑 상호작용할 필요 없음
            if (!HoldItems.IsEmpty()) return false;

            // 만들어진 선로 없으면 얘랑 상호작용할 필요 없음
            if (!crafter->CheckRail()) return false;

            UE_LOG(LogTrain, Log, TEXT("Player Interaction: Crafter Has Rails"));

            HoldItems.Append(crafter->GetRail());
            HoldItems[0]->Server_Attach(this, TEXT("TempHandMesh"));

            return true;
        }

        // 화물차도 제작차도 아니니까 return false
        return false;
    */}
}

void ASBS_Player::Server_FindTrain_Implementation(const TArray<class AItem*>& PlayerItems) {
    // 먼저 초기화
    bHasFound = false;

    // 현재 위치와 타일 크기로 전방의 타일 위치를 계산
    FVector CurLoc = GetActorLocation();
    FVector ForwardLoc = CurLoc + GetActorForwardVector() * TileSize;

    // 계산된 타일의 위치를 타일 크기에 맞게 보정
    int ForwardTileX = FMath::RoundToInt(ForwardLoc.X / TileSize) * TileSize;
    int ForwardTileY = FMath::RoundToInt(ForwardLoc.Y / TileSize) * TileSize;

    FVector Start = FVector(ForwardTileX, ForwardTileY, CurLoc.Z + 200.f);
    FVector End = FVector(ForwardTileX, ForwardTileY, CurLoc.Z - 100.f);
    FHitResult Hit;
    FCollisionQueryParams params;
    params.AddIgnoredActor(this);

    // LineTrace가 실패한 경우 = 기차를 탐지하지 못 함
    if (!GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, params))
        return;

    Multicast_DrawRaycast(GetWorld(), Start, End, FLinearColor::Green, 1, 15);

    // 화물차부터 탐색
    if (ATrainCargo* cargo = Cast<ATrainCargo>(Hit.GetActor())) {
        // 상호작용할 수 있는지 확인 (불이 붙어있으면 return)
        if (!cargo->CheckInteraction()) return;

        // 손에 뭐 들고 있으면 넣기
        if (!PlayerItems.IsEmpty()) {
            PRINTTRAIN(TEXT("Player Interaction: Cargo"));

            // 유효한 자원인지 확인
            if (!cargo->CheckAddResource(PlayerItems[0]->ItemType)) return;

            Client_DetachHoldItem(PlayerItems[0]);
            cargo->AddResource(PlayerItems);
            Multicast_RemoveHoldItem();

            bHasFound = true;
            return;
        }

        // 손에 없으면 빼오기 시도
        else {
            // 보관 장소가 나무면 나무, 돌이면 돌 가져오기 (2개의 Box Collider로 구분되어 있음)
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

    // 제작차 탐색
    else if (ATrainCrafter* crafter = Cast<ATrainCrafter>(Hit.GetActor())) {
        UE_LOG(LogTrain, Log, TEXT("Player Interaction: Crafter"));

        // 손에 아이템 있으면 얘랑 상호작용할 필요 없음
        if (!HoldItems.IsEmpty()) return;

        // 만들어진 선로 없으면 얘랑 상호작용할 필요 없음
        if (!crafter->CheckRail()) return;

        UE_LOG(LogTrain, Log, TEXT("Player Interaction: Crafter Has Rails"));

        HoldItems.Append(crafter->GetRail());
        Client_AttachHoldItem(HoldItems[0]);

        bHasFound = true;
        return;
    }

    // 화물차도 제작차도 아니니까 return false
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
               
                if (PlaceTile && PlaceTile->TileType == ETileType::Ground)
                {
                    //손에서 땐다.
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
        else //물건 안들고 있으면 =====줍기=====
        {
            //attach 하고 스택에 추가
            TargetItem[0]->Server_Attach(this, FName(TEXT("HandSocket")));
            HoldItems.Append(TargetItem);

            //타일에서 아이템 들었으니까 타일의 아이템 제거
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
