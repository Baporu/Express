// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"	
#include "../Express.h"
#include "../../../../Plugins/Online/OnlineBase/Source/Public/Online/OnlineSessionNames.h"
#include "NetPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Exp_GameState.h"

void UNetGameInstance::Init()
{
	Super::Init();

	if (auto subsys = IOnlineSubsystem::Get())
	{
		// ����ý������κ��� �����������̽� ��������
		sessionInterface = subsys->GetSessionInterface();
		if (sessionInterface.IsValid())
		{
			sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNetGameInstance::OnCreateSessionComplete);
			sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UNetGameInstance::OnFindSessionsComplete);
			sessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UNetGameInstance::OnJoinSessionCompleted);
			sessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UNetGameInstance::OnMyExitRoomCompleted);
		}
	}
}

void UNetGameInstance::CreateMySession(FString roomName, int32 playerCount)
{
	// ���Ǽ�������
	FOnlineSessionSettings sessionSettings;

	// 1. Dedicated Server ���ӿ���
	sessionSettings.bIsDedicated = false;

	// 2. ����(����)��Ī�� ���� Steam ��Ī�� ���� ����
	FName subsysName = IOnlineSubsystem::Get()->GetSubsystemName();
	sessionSettings.bIsLANMatch = (subsysName == "NULL");

	// 3.��Ī�� �¶����� ���� ������� ����
	// false �̸� �ʴ븦 ���ؼ��� ������ ����
	// SendSessionInviteToFriend() �Լ��� ���� ģ���ʴ븦 �� �� �ִ�
	sessionSettings.bShouldAdvertise = true;

	// 4. �¶��� ����(Presence) ������ Ȱ������ ����
	sessionSettings.bUsesPresence = true;
	sessionSettings.bUseLobbiesIfAvailable = true; // <<=====

	// 5. ���������߿� ���� �㰡���� ����
	sessionSettings.bAllowJoinViaPresence = true;
	sessionSettings.bAllowJoinInProgress = true;

	// 6. ���ǿ� ������ �� �ִ� ����(public) ������ �ִ� ��� ��
	sessionSettings.NumPublicConnections = playerCount;

	// 7. Ŀ���� ����� ����
	sessionSettings.Set(FName("ROOM_NAME"), StringBase64Encode(roomName), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// 8. ȣ��Ʈ���� ����
	sessionSettings.Set(FName("HOST_NAME"), StringBase64Encode(mySessionName), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// netID
	FUniqueNetIdPtr netID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();

	PRINTLOG(TEXT("Create Session Start : %s"), *mySessionName);
	sessionInterface->CreateSession(*netID, FName(mySessionName), sessionSettings);

}

void UNetGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	PRINTLOG(TEXT("SessionName : %s, bWasSuccessful : %d"), *SessionName.ToString(), bWasSuccessful);

	if (bWasSuccessful == true)
	{
		//GetWorld()->ServerTravel(TEXT("/Game/SBS/SBS_Level?listen"));
		GetWorld()->ServerTravel(TEXT("/Game/Network/WaitingMap?listen"));

	}
}

void UNetGameInstance::FindOtherSession()
{
	onSearchState.Broadcast(true);

	sessionSearch = MakeShareable(new FOnlineSessionSearch());

	// 1. ���� �˻� ���� ����
	sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	// 2. Lan ����
	sessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == FName("NULL");

	// 3. �ִ� �˻� ���� ��
	sessionSearch->MaxSearchResults = 10;

	// 4. ���ǰ˻�
	sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());
}

void UNetGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (!sessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("OnFindSessionsComplete: sessionInterface is null"));
		onSearchState.Broadcast(false);
		return;
	}

	// sessionSearch ��ȿ�� �˻�
	if (!sessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("OnFindSessionsComplete: sessionSearch is null"));
		onSearchState.Broadcast(false);
		return;
	}
	// ã�� ���н�
	if (bWasSuccessful == false)
	{
		onSearchState.Broadcast(false);
		PRINTLOG(TEXT("Session search failed..."));
		return;
	}

	// ���ǰ˻���� �迭
	auto results = sessionSearch->SearchResults;
	//PRINTLOG(TEXT("Search Result Count : %d"), results.Num());

	for (int i = 0; i < results.Num(); i++)
	{
		auto sr = results[i];
		if (sr.IsValid() == false)
		{
			continue;
		}

		FString roomName;
		FString hostName;

		// �������� ����ü����
		FSessionInfo sessionInfo;
		sessionInfo.index = i;

		sr.Session.SessionSettings.Get(FName("ROOM_NAME"), roomName);
		sr.Session.SessionSettings.Get(FName("HOST_NAME"), hostName);
		sessionInfo.roomName = StringBase64Decode(roomName);
		sessionInfo.hostName = StringBase64Decode(hostName);

		//sr.Session.SessionSettings.Get(FName("ROOM_NAME"), sessionInfo.roomName);

		//sr.Session.SessionSettings.Get(FName("HOST_NAME"), sessionInfo.hostName);

		// ���尡���� �÷��̾��
		int32 maxPlayerCount = sr.Session.SessionSettings.NumPublicConnections;
		// ���� ������ �÷��̾� �� ( �ִ� - ���� ���� ������ �� )
		// NumOpenPublicConnections ���������� ���������� ���� ���´�
		int32 currentPlayerCount = maxPlayerCount - sr.Session.NumOpenPublicConnections;

		sessionInfo.playerCount = FString::Printf(TEXT("(%d/%d)"), currentPlayerCount, maxPlayerCount);

		// �� ����( ���������� 9999�� ���´� )
		sessionInfo.pingSpeed = sr.PingInMs;

		PRINTLOG(TEXT("%s"), *sessionInfo.ToString());

		// ��������Ʈ�� ������ �˷��ֱ�
		onSearchCompleted.Broadcast(sessionInfo);
	}

	onSearchState.Broadcast(false);
}

void UNetGameInstance::JoinSelectedSession(int32 index)
{
	auto sr = sessionSearch->SearchResults;

	// �̰� ���� �𸮾� ����
	sr[index].Session.SessionSettings.bUseLobbiesIfAvailable = true;
	sr[index].Session.SessionSettings.bUsesPresence = true;

	sessionInterface->JoinSession(0, FName(mySessionName), sr[index]);
}

void UNetGameInstance::OnJoinSessionCompleted(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	if (result == EOnJoinSessionCompleteResult::Success)
	{
		auto pc = GetWorld()->GetFirstPlayerController();

		FString url;
		sessionInterface->GetResolvedConnectString(sessionName, url);

		PRINTLOG(TEXT("Join URL : %s"), *url);

		if (url.IsEmpty() == false)
		{
			pc->ClientTravel(url, ETravelType::TRAVEL_Absolute);
		}
	}
	else
	{
		PRINTLOG(TEXT("Join Session failed : %d"), result);
	}
}

void UNetGameInstance::RestartRoom() {
	// GameState���� Input Mode �ʱ�ȭ ��û
	Cast<AExp_GameState>(GetWorld()->GetGameState())->ResetInputMode();

	// �������� ����� ��û
	ServerRPC_RestartRoom();
}

void UNetGameInstance::ServerRPC_RestartRoom_Implementation() {
	// ServerTravel()�� ���� ���� �Լ���, ���ӵ� ��� Ŭ���̾�Ʈ�� ���󰣴�.
	GetWorld()->ServerTravel(TEXT("/Game/Network/WaitingMap?listen"));
}

void UNetGameInstance::ExitRoom() {
	// �������� ���� ��û
	ServerRPC_ExitRoom();
}

void UNetGameInstance::ServerRPC_ExitRoom_Implementation() {
	// ������ ��� Ŭ���̾�Ʈ���׼� ������ ���־� �ϹǷ� Multicast
	MultiRPC_ExitRoom();
}

void UNetGameInstance::MultiRPC_ExitRoom_Implementation() {
	// �÷��̾� ���� ó��
	sessionInterface->DestroySession(FName(*mySessionName));
}

void UNetGameInstance::OnMyExitRoomCompleted(FName sessionName, bool bWasSuccessful) {
	auto pc = GetWorld()->GetFirstPlayerController();
	FString url = TEXT("/Game/Network/LobbyMap");
	pc->ClientTravel(url, TRAVEL_Absolute);
}

FString UNetGameInstance::StringBase64Encode(const FString& str)
{
	// Set �� �� : FString -> UTF8 (std::string) -> TArray<uint8> -> base64 �� Encode
	std::string utf8String = TCHAR_TO_UTF8(*str);
	TArray<uint8> arrayData = TArray<uint8>((uint8*)(utf8String.c_str()), utf8String.length());
	return FBase64::Encode(arrayData);
}

FString UNetGameInstance::StringBase64Decode(const FString& str)
{
	// Get �� �� : base64 �� Decode -> TArray<uint8> -> TCHAR
	TArray<uint8> arrayData;
	FBase64::Decode(str, arrayData);
	std::string utf8String((char*)(arrayData.GetData()), arrayData.Num());
	return UTF8_TO_TCHAR(utf8String.c_str());
}