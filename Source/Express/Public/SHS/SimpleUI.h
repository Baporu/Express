// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SimpleUI.generated.h"

/**
 * 
 */
UCLASS()
class EXPRESS_API USimpleUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 서버인지 아닌지 담아두는 변수
	bool bHasAuthority = false;

	UPROPERTY(meta = (BindWidget))
	class UButton* btn_restart;
	// Restart 대기 중인지 확인하는 변수
	bool bIsWaiting = false;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* txt_wait;
	FTimerHandle WaitHandle;
	int WaitNum = 1;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* btn_exit;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnRestart();
	UFUNCTION()
	void OnExit();

	void OnWaitTimerExpired();
};
