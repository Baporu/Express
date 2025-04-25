// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NetPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class EXPRESS_API ANetPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY()
	class AExp_GameMode* gm;

	virtual void BeginPlay() override;



	UFUNCTION(BlueprintCallable)
    void SetMainCamera();

	UFUNCTION(BlueprintCallable, Server, Reliable)
    void Server_SetMainCamera();

	//UFUNCTION(Server, Reliable)
	//void ServerRPC_RespawnPlayer();

	//UPROPERTY(EditDefaultsOnly, Category = UI)
	//TSubclassOf<class MainUI> mainUIWidget;

	//UPROPERTY()
	//class UMainUI* mainUI;


public:
	UFUNCTION(Client, Reliable)
	void Client_DisableInput();
};
