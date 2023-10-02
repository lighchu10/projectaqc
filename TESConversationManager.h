// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TES.h"
#include "Manager/TESManagerBase.h"
#include "TESConversationManager.generated.h"

/**
 *
 */
 UCLASS()
class TES_API UTESConversationManager : public UTESManagerBase
{
	GENERATED_BODY()

//. ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Override Functions ]
//. ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
public:
	virtual void Start() override;
	virtual void Reset() override;
	virtual void Shutdown() override;
	virtual void Tick(const float& deltaTime) override;

public:
	void Clear();

private:
	void ClearCharacterMap();

//. ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Tick Functions ]
//. ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	void TickCamera(const float& deltaSecond);
	void TickConversationCamera(const float& deltaSecond);
	void TickConversationCameraFov(const float& deltaSecond);
	void TickConversationCameraLocation(const float& deltaSecond);	

//. ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ General Functions ]
//. ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
public:
	//. 해당 대화 카테고리에 대한 정보들을 세팅합니다.
	void SetConversation(ETypeConversationType category);

	//. 해당 대화 카테고리에 대한 비동기로딩을 완료하고 난 이후의 행동을 세팅합니다.
	void SetConversationLoadingFinishCallback(FOnTESAction onFinishAction);

	//. 해당 대화 카테고리에 대한 대화를 시작합니다.
	//. ( 비동기 로드 -> 콜백호출 -> 대화 시작 )
	void StartConversation(FTESConversationInitData& initData);

	//. 대화를 시작할때 호출하는 함수입니다.
	void StartConversation(int32 conversationId, struct FTESConversationInitData conversationData);

	//. 대화를 종료할때 위젯에서 호출하는 함수
	void EndConversation();

	//. 로비에서 대화를 종료할때 호출하는 함수
	void LobbyEndConversation();

public:
	//. 회화 연출을 하고 있는 중인가? (스테이지 시간 계산에서 거르기 위한 용도)
	bool IsPerformConversation();
	void SetPerformConversation(bool isPerformConversation);

public:
	void UpdateConversation(int32 converationId);
	//void SetConversationCharacter();

private:
	void InitCharacter(int32 characterId, int32 slotIndex);
	void InitCamera();

private:
	void SetConversationCamera();

public:
	void SetConversationEnd();
	void SetCharacterToEnd();
	void SetToNextScene(ETESPhysicalGameSceneType physicalSceneType);

public:
	void SetCurrentConversationId(int32 conversationId);

private:
	void SetConversationCameraToEnd();

public:
	void SetConversationById(int32 conversationId);		//. 회화 테이블 아이디로 한번에 해당 회화 세팅

#pragma region ConversationData
public:
	//. 현재 대화 카테고리에 대한 데이터를 추가합니다.
    void AddConversationData(const FConversationRow* conversationTable);

	////. 현재 대화 카테고리에 대한 데이터를 제거합니다. (단일)
 //   void RemoveConversationData(const int32 removeIndex);

	//. 현재 대화 카테고리에 대한 데이터를 제거합니다. (전부)
	void RemoveConversationDataAll();

	//. 현재 대화 카테고리에 대한 데이터의 개수를 얻어옵니다.
	const int32 GetConversationDataCount();

	//. 회화에 필요한 데이터
	const FTESConversationData GetFirstConversationData();

	const FTESConversationData GetConversationDataById(int32 conversationId);

	//. 회화의 한 세트 전체 묶음(회화 아이디가 키값)
	const TMap<int32, FTESConversationData>& GetConversationDataMap() const;

	const FTESConversationInitData GetConversationInitData() const;
#pragma endregion ConversationData

public:
	void SetAttachToActor(const int32 slotIndex);
	void SetDetachToActor(const int32 slotIndex);

public:
	TArray_Conversation GetConversationTableRowDataById(const int32 conversationId);
	TArray_Conversation GetConversationTableRowDataByCategory(const ETypeConversationType category);

public:
	TMap<int32, class ATESCharacterConversation*> GetCurrentSpeakingCharacters();

private:
	ACameraActor* GetCameraActor(FString cameraActorName);
	
public:
	ACameraActor* GetCameraActor();

public:
	const bool CheckIfPossibleToPlay();

public:
	void SetIsEnterDevelopHelper(bool isOn);
	void EndDevelopHelper();

//. ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Member Variables ]
//. ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	UPROPERTY() bool _isPerformConversation = false;									//. 회화 연출을 하고 있는 중인가?

private:
	FOnTESAction _onLoadingFinishAction = {};

private:
	UPROPERTY() TMap<int32, class ATESCharacterConversation*> _conversationCharacterMap;

private:
	UPROPERTY() class ACameraActor* _conversationCamera = nullptr;
	UPROPERTY() FVector _originCameraVector = {};										//. 카메라 기준좌표
	UPROPERTY()	FVector _initConversationCameraLocation = FVector::ZeroVector;			//. 카메라 시작좌표
	UPROPERTY() FVector _destinationConversationCameraLocation = FVector::ZeroVector;	//. 카메라가 최종적으로 이동해야하는 좌표
	UPROPERTY() float _conversationCameraInitFov = 0.0f;
	UPROPERTY() float _conversationCameraDestinationFov = 0.0f;							//. 카메라 최종FoV
	UPROPERTY() float _conversationCameraFovTime = 0.0f;								//. 카메라 Fov가 바뀌는 시간
	UPROPERTY() float _conversationCameraMovingRealTimeForFoV = 0.0f;					//. 카메라가 현재 실시간으로 움직이는 타임(Fov)
	UPROPERTY() float _conversationCameraMovingRealTime = 0.0f;							//. 카메라가 현재 실시간으로 움직이는 타임
	UPROPERTY() float _conversationCameraInitTime = 0.0f;								//. 초기 위치로 이동하는 카메라가 움직여야하는 총 시간.
	UPROPERTY() float _conversationCameraDestinationTime = 0.0f;						//. 카메라가 움직여야하는 총 시간(3초에 걸쳐서 ~서부터 ~까지 이동. 3초)
	UPROPERTY() bool _initCameraMoving = false;											//. 대화 시작할때 카메라 이동
	UPROPERTY() bool _initCameraFovChanging = false;									//. 대화 시작할때 카메라 fov
	UPROPERTY() bool _movingLocationEnd = false;										//. 카메라 좌표 이동이 전부 끝났는지
	UPROPERTY() bool _movingFovEnd = false;												//. 카메라 fov 조절이 전부 끝났는지
	UPROPERTY() FVector _prevCameraLocation = FVector::ZeroVector;

private:
	UPROPERTY() int32 _currentConversationId = 0;										//. 현재 실행중인 회화 아이디.

private:
	//. 해당 회화에 들어가는 테이블데이터
	TArray<const FConversationRow*> _conversationTableRows = {};

	////. 실제 위젯에서 동작할 대화 데이터
	//UPROPERTY() TArray<FTESConversationData> _conversationDataArray = {};

	//. 아이디로 재생할 대화 데이터
	UPROPERTY() TMap<int32, FTESConversationData> _conversationDataMapById = {};		//. 기본 동작하는 방법과 달리 데이터를 지우지 않고 보관한다.

private:
	UPROPERTY() FTESConversationInitData _conversationInitData = {};

private:
	UPROPERTY() bool _isEnterDevelopHelper = false;
//. ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
};