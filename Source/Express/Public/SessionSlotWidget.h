// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class EXPRESS_API USessionSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, meta = ( BindWidget))
	class UTextBlock* txt_roomName;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* txt_hostName;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* txt_playerCount;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* txt_pingSpeed;
	
	int32 sessionNumber;
	
	void Set(const struct FSessionInfo& sessionInfo);
	
	// 세션조인
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_join;
	
	virtual void NativeConstruct() override;
	UFUNCTION()
	void JoinSession();
};
