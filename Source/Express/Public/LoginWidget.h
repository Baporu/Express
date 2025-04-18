// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoginWidget.generated.h"

/**
 * 
 */
UCLASS()
class EXPRESS_API ULoginWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget))
	class UButton* btn_createRoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget))
	class UEditableText*	edit_roomName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget))
	class USlider* slider_playerCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget))
	class UTextBlock* txt_playerCount;

	UPROPERTY()
	class UNetGameInstance* gi;

public:
	virtual void NativeConstruct()	override;


	UFUNCTION()
	void CreateRoom();

	// Slider callback
	UFUNCTION()
	void OnValueChanged(float Value);

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UWidgetSwitcher* WidgetSwitcher;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UButton* btn_createSession;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UButton* btn_findSession;


	UFUNCTION()
	void SwitchCreatePanel();
	UFUNCTION()
	void SwitchFindPanel();

	// 메인화면돌아가기 버튼
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UButton* btn_back;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UButton* btn_back_1;

	UFUNCTION()
	void BackToMain();

	// 방검색 버튼
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UButton* btn_find;

	// 검색중 메시지
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UTextBlock*	txt_findingMsg;




	// 방찾기 버튼 클릭시 호출될 콜백
	UFUNCTION()
	void OnClickedFindSession();

	// 방찾기 상태 이벤트 콜백
	UFUNCTION()
	void OnChangeButtonEnable(bool bIsSearching);




	// 세션슬롯
	// Canvas_FindRoom 의 스크롤박스위젯
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UScrollBox* scroll_roomList;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class USessionSlotWidget> sessionInfoWidget;

	UFUNCTION()
	void AddSlotWidget(const struct FSessionInfo& sessionInfo);
	
};
