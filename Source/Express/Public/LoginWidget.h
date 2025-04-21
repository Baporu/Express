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

	// ����ȭ�鵹�ư��� ��ư
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UButton* btn_back;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UButton* btn_back_1;

	UFUNCTION()
	void BackToMain();

	// ��˻� ��ư
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UButton* btn_find;

	// �˻��� �޽���
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UTextBlock*	txt_findingMsg;




	// ��ã�� ��ư Ŭ���� ȣ��� �ݹ�
	UFUNCTION()
	void OnClickedFindSession();

	// ��ã�� ���� �̺�Ʈ �ݹ�
	UFUNCTION()
	void OnChangeButtonEnable(bool bIsSearching);




	// ���ǽ���
	// Canvas_FindRoom �� ��ũ�ѹڽ�����
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UScrollBox* scroll_roomList;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class USessionSlotWidget> sessionInfoWidget;

	UFUNCTION()
	void AddSlotWidget(const struct FSessionInfo& sessionInfo);
	
};
