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
		// 서브시스템으로부터 세션인터페이스 가져오기
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
	// 세션설정변수
	FOnlineSessionSettings sessionSettings;

	// 1. Dedicated Server 접속여부
	sessionSettings.bIsDedicated = false;

	// 2. 랜선(로컬)매칭을 할지 Steam 매칭을 할지 여부
	FName subsysName = IOnlineSubsystem::Get()->GetSubsystemName();
	sessionSettings.bIsLANMatch = (subsysName == "NULL");

	// 3.매칭이 온라인을 통해 노출될지 여부
	// false 이면 초대를 통해서만 입장이 가능
	// SendSessionInviteToFriend() 함수를 통해 친구초대를 할 수 있다
	sessionSettings.bShouldAdvertise = true;

	// 4. 온라인 상태(Presence) 정보를 활용할지 여부
	sessionSettings.bUsesPresence = true;
	sessionSettings.bUseLobbiesIfAvailable = true; // <<=====

	// 5. 게임진행중에 참여 허가할지 여부
	sessionSettings.bAllowJoinViaPresence = true;
	sessionSettings.bAllowJoinInProgress = true;

	// 6. 세션에 참여할 수 있는 공개(public) 연결의 최대 허용 수
	sessionSettings.NumPublicConnections = playerCount;

	// 7. 커스텀 룸네임 설정
	sessionSettings.Set(FName("ROOM_NAME"), StringBase64Encode(roomName), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// 8. 호스트네임 설정
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

	// 1. 세션 검색 조건 설정
	sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	// 2. Lan 여부
	sessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == FName("NULL");

	// 3. 최대 검색 세션 수
	sessionSearch->MaxSearchResults = 10;

	// 4. 세션검색
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

	// sessionSearch 유효성 검사
	if (!sessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("OnFindSessionsComplete: sessionSearch is null"));
		onSearchState.Broadcast(false);
		return;
	}
	// 찾기 실패시
	if (bWasSuccessful == false)
	{
		onSearchState.Broadcast(false);
		PRINTLOG(TEXT("Session search failed..."));
		return;
	}

	// 세션검색결과 배열
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

		// 세션정보 구조체선언
		FSessionInfo sessionInfo;
		sessionInfo.index = i;

		sr.Session.SessionSettings.Get(FName("ROOM_NAME"), roomName);
		sr.Session.SessionSettings.Get(FName("HOST_NAME"), hostName);
		sessionInfo.roomName = StringBase64Decode(roomName);
		sessionInfo.hostName = StringBase64Decode(hostName);

		//sr.Session.SessionSettings.Get(FName("ROOM_NAME"), sessionInfo.roomName);

		//sr.Session.SessionSettings.Get(FName("HOST_NAME"), sessionInfo.hostName);

		// 입장가능한 플레이어수
		int32 maxPlayerCount = sr.Session.SessionSettings.NumPublicConnections;
		// 현재 입장한 플레이어 수 ( 최대 - 현재 입장 가능한 수 )
		// NumOpenPublicConnections 스팀에서만 정상적으로 값이 들어온다
		int32 currentPlayerCount = maxPlayerCount - sr.Session.NumOpenPublicConnections;

		sessionInfo.playerCount = FString::Printf(TEXT("(%d/%d)"), currentPlayerCount, maxPlayerCount);

		// 핑 정보( 스팀에서는 9999로 나온다 )
		sessionInfo.pingSpeed = sr.PingInMs;

		PRINTLOG(TEXT("%s"), *sessionInfo.ToString());

		// 델리게이트로 위젯에 알려주기
		onSearchCompleted.Broadcast(sessionInfo);
	}

	onSearchState.Broadcast(false);
}

void UNetGameInstance::JoinSelectedSession(int32 index)
{
	auto sr = sessionSearch->SearchResults;

	// 이건 현재 언리얼 버그
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
	// GameState한테 Input Mode 초기화 요청
	Cast<AExp_GameState>(GetWorld()->GetGameState())->ResetInputMode();

	// 서버한테 재시작 요청
	ServerRPC_RestartRoom();
}

void UNetGameInstance::ServerRPC_RestartRoom_Implementation() {
	// ServerTravel()은 서버 전용 함수로, 접속된 모든 클라이언트도 따라간다.
	GetWorld()->ServerTravel(TEXT("/Game/Network/WaitingMap?listen"));
}

void UNetGameInstance::ExitRoom() {
	// 서버한테 퇴장 요청
	ServerRPC_ExitRoom();
}

void UNetGameInstance::ServerRPC_ExitRoom_Implementation() {
	// 서버와 모든 클라이언트한테서 정보를 없애야 하므로 Multicast
	MultiRPC_ExitRoom();
}

void UNetGameInstance::MultiRPC_ExitRoom_Implementation() {
	// 플레이어 퇴장 처리
	sessionInterface->DestroySession(FName(*mySessionName));
}

void UNetGameInstance::OnMyExitRoomCompleted(FName sessionName, bool bWasSuccessful) {
	auto pc = GetWorld()->GetFirstPlayerController();
	FString url = TEXT("/Game/Network/LobbyMap");
	pc->ClientTravel(url, TRAVEL_Absolute);
}

FString UNetGameInstance::StringBase64Encode(const FString& str)
{
	// Set 할 때 : FString -> UTF8 (std::string) -> TArray<uint8> -> base64 로 Encode
	std::string utf8String = TCHAR_TO_UTF8(*str);
	TArray<uint8> arrayData = TArray<uint8>((uint8*)(utf8String.c_str()), utf8String.length());
	return FBase64::Encode(arrayData);
}

FString UNetGameInstance::StringBase64Decode(const FString& str)
{
	// Get 할 때 : base64 로 Decode -> TArray<uint8> -> TCHAR
	TArray<uint8> arrayData;
	FBase64::Decode(str, arrayData);
	std::string utf8String((char*)(arrayData.GetData()), arrayData.Num());
	return UTF8_TO_TCHAR(utf8String.c_str());
}