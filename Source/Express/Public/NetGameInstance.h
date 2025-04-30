// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "NetGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FSessionInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString roomName;
	UPROPERTY(BlueprintReadOnly)
	FString hostName;
	UPROPERTY(BlueprintReadOnly)
	FString playerCount;
	UPROPERTY(BlueprintReadOnly)
	int32 pingSpeed;
	UPROPERTY(BlueprintReadOnly)
	int32 index;

	inline FString ToString()
	{
		return FString::Printf(TEXT("[%d] %s : %s - %s, %dms"), index, *roomName, *hostName, *playerCount, pingSpeed);
	}
};

// ���ǰ˻� ������ �� ȣ��� ��������Ʈ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSearchSignature, const FSessionInfo&, sessionInfo);

// ���ǰ˻� ���� ��������Ʈ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSearchStateSignature, bool, bIsSearching);

/**
 * 
 */
UCLASS()
class EXPRESS_API UNetGameInstance : public UGameInstance
{
	GENERATED_BODY()
	public:
	virtual void Init() override;
	
public:
	IOnlineSessionPtr sessionInterface;

	UFUNCTION(BlueprintCallable)
	void CreateMySession(FString roomName, int32 playerCount);

	// ����(ȣ��Ʈ) �̸�
	FString mySessionName = "SoBeomSoo";

	UFUNCTION()
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	// ��˻�
	TSharedPtr<FOnlineSessionSearch> sessionSearch;
	void FindOtherSession();

	void OnFindSessionsComplete(bool bWasSuccessful);

	// ��ã��Ϸ� �ݹ��� ����� ��������Ʈ
	FSearchSignature onSearchCompleted;

	// ��ã����� �ݹ� ��������Ʈ
	FSearchStateSignature onSearchState;


	// ����(��) ����
	void JoinSelectedSession(int32 index);

	// �������� �ݹ�
	void OnJoinSessionCompleted(FName sessionName, EOnJoinSessionCompleteResult::Type result);

	// ���� ����� �Լ���
	void RestartRoom();
	UFUNCTION(Server, Reliable)
	void ServerRPC_RestartRoom();

	// �� ���� �Լ���
	void ExitRoom();
	UFUNCTION(Server, Reliable)
	void ServerRPC_ExitRoom();
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_ExitRoom();
	void OnMyExitRoomCompleted(FName sessionName, bool bWasSuccessful);

	// �ٱ��� ���ڵ�
	FString StringBase64Encode(const FString& str);
	FString StringBase64Decode(const FString& str);


public:
	FName currentSessionName;

	int32 GetMaxPlayer();
};
