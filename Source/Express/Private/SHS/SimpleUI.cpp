// Fill out your copyright notice in the Description page of Project Settings.


#include "SHS/SimpleUI.h"
#include "Components/Button.h"
#include "NetGameInstance.h"
#include "Components/TextBlock.h"

void USimpleUI::NativeConstruct() {
	Super::NativeConstruct();

	btn_restart->OnClicked.AddDynamic(this, &USimpleUI::OnRestart);
	btn_exit->OnClicked.AddDynamic(this, &USimpleUI::OnExit);
}

void USimpleUI::OnRestart() {
	if (bHasAuthority) {
		auto gi = Cast<UNetGameInstance>(GetWorld()->GetGameInstance());

		if (!gi) return;

		gi->RestartRoom();
	}

	else {
		bIsWaiting = !bIsWaiting;

		btn_exit->SetIsEnabled(!bIsWaiting);
		txt_info->SetVisibility(ESlateVisibility::Hidden);
		txt_wait->SetVisibility(ESlateVisibility::Visible);

		GetWorld()->GetTimerManager().SetTimer(WaitHandle, this, &USimpleUI::OnWaitTimerExpired, 1.0f, true);
	}
}

void USimpleUI::OnExit() {
	auto gi = Cast<UNetGameInstance>(GetWorld()->GetGameInstance());

	if (!gi) return;

	gi->ExitRoom();
}

void USimpleUI::OnWaitTimerExpired() {
	FString str = FString(TEXT("Waiting for host"));

	for (int i = 0; i < WaitNum; ++i)
		str += FString(TEXT(". "));

	if (++WaitNum > 3) WaitNum = 1;

	txt_wait->SetText(FText::FromString(str));
}
