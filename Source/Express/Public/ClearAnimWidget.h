// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ClearAnimWidget.generated.h"

/**
 * 
 */
UCLASS()
class EXPRESS_API UClearAnimWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, meta = (BindWidgetAnim), Transient, Category = "MySettings")
	class UWidgetAnimation* ClearAnim;

	// 게임 클리어 애니메이션 재생
	void PlayClearAnimation();
};
