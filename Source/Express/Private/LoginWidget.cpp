// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginWidget.h"
#include "NetGameInstance.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Blueprint/UserWidget.h"

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	gi = Cast<UNetGameInstance>(GetWorld()->GetGameInstance());
	gi->onSearchCompleted.AddDynamic(this, &ULoginWidget::AddSlotWidget);
	gi->onSearchState.AddDynamic(this, &ULoginWidget::OnChangeButtonEnable);
	
	btn_createRoom->OnClicked.AddDynamic(this, &ULoginWidget::CreateRoom);
	slider_playerCount->OnValueChanged.AddDynamic(this, &ULoginWidget::OnValueChanged);

	btn_createSession->OnClicked.AddDynamic(this, &ULoginWidget::SwitchCreatePanel);
	btn_findSession->OnClicked.AddDynamic(this, &ULoginWidget::SwitchFindPanel);

	btn_back->OnClicked.AddDynamic(this, &ULoginWidget::BackToMain);
	btn_back_1->OnClicked.AddDynamic(this, &ULoginWidget::BackToMain);

	btn_find->OnClicked.AddDynamic(this, &ULoginWidget::OnClickedFindSession);
}

void ULoginWidget::CreateRoom()
{
	if (gi && edit_roomName->GetText().IsEmpty() == false)
	{
		FString roomName = edit_roomName->GetText().ToString();
		int32 playerCount = slider_playerCount->GetValue();
		gi->CreateMySession(roomName, playerCount);
	}
}

void ULoginWidget::OnValueChanged(float Value)
{
	txt_playerCount->SetText(FText::AsNumber(Value));
}

void ULoginWidget::SwitchCreatePanel()
{
	WidgetSwitcher->SetActiveWidgetIndex(1);
}

void ULoginWidget::SwitchFindPanel()
{
	WidgetSwitcher->SetActiveWidgetIndex(2);
	OnClickedFindSession();
}

void ULoginWidget::BackToMain()
{
	WidgetSwitcher->SetActiveWidgetIndex(0);
}

void ULoginWidget::OnClickedFindSession()
{
	// 기존 슬롯이 있다면 모두 지운다
	scroll_roomList->ClearChildren();

	if (gi != nullptr)
	{
		gi->FindOtherSession();
	}
}

void ULoginWidget::OnChangeButtonEnable(bool bIsSearching)
{
	btn_find->SetIsEnabled(!bIsSearching);

	if (bIsSearching == true)
	{
		// 검색중 보이도록 처리
		txt_findingMsg->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		// 검색중 사라지도록 처리
		txt_findingMsg->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ULoginWidget::AddSlotWidget(const struct FSessionInfo& sessionInfo)
{
	auto slot = CreateWidget<USessionSlotWidget>(this, sessionInfoWidget);
	slot->Set(sessionInfo);

	scroll_roomList->AddChild(slot);
}
