// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TES.h"
#include "Structs/Manager/TESManagerBase.h"
#include "TESPartySettingManager.generated.h"

///**
// * 파티세팅으로 진입하는 루트에 대한 타입
// */
//UENUM()
//enum class ETESPartySettingEntryRoute
//{
//	None,
//	InLobbyField,				//. 로비 필드에서 진입했습니다.
//	InMainStory,				//. 메인 스토리에서 진입했습니다.
//	InUniqueAbyss,				//. 유니크 어비스에서 진입했습니다.
//	InCharacterInventoryBack,	//. 캐릭터 인벤토리에서 뒤로가기 진입을 했습니다.
//	Max
//};

UCLASS()
class TES_API UTESPartySettingManager : public UTESManagerBase
{
	GENERATED_BODY()

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Override Functions ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
public:
	virtual void Start() override;
	virtual void Reset() override;
	virtual void Shutdown() override;
	virtual void Tick(const float& deltaTime) override;

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Scene Functions ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
public:
	void InitScene(/*ETESPartySettingEntryRoute entryRoute,*/ TMap<FString, int32> sceneDatas = {});
	void EnterScene(int32 partyUnique, int32 stageId, int32 numofLockedSlot, int32 difficultyLevel);
	void ClearScene();

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Init Functions ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	void InitSceneCameraActor();
	void InitSceneCameraOffsetActor();
	//. void InitPartySettingByContents();

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Clear Functions ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	//void ClearCurrentData();
	void ClearMainWidget();
	void ClearTeamTagMainWidget();
	void ClearMirageGardenWidget();
	void ClearDetailWidget();
	void ClearCharSlotWidgets();
	void ClearSwapSlotWidgets();
	void ClearCharacterActors();

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Set Functions ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
public:
	void SetSceneCameraActor(class AActor* offsetActor);
	void SetPartyPresetCharacterModelingFromLobby();
	void SetPartyPresetCharacterModelingByPartyUnique(uint16 partyUnique);
	void SetPartyCharacterModelingBySallyForced(uint16 partyUnique);					//. 강제 출격으로 파티편성을 하는 경우
	void SetupCharacterSlotList();														
	void SetupCharacterInfoSlotList();
	void SetupCharacterLocationSlotList();
	void SetupCharacterSwapSlotList();
	void SetupPartySettingWidgetsFromLobby();																		//. 로비에서 바로 편성을 하면 1파티로 바로 세팅된다.
	void SetupPartySettingWidgetsByPartyUnique(uint16 partyUniqe, int32 stageId = 0, int32 difficultyLevel = 0, ETESCommonRemainCountType type = ETESCommonRemainCountType::None);	//. 파티 번호를 이용하여 메인 위젯 세팅
	void SetupPartySettingWidgetsBySallyForcedParty(int32 stageId = 0);												//. 강제 출격이 있는 파티는 파티 유니크 1번 인덱스로 세팅한다.
	void SetupLockedSlot(int32 numofLockedSlot);																	//. 강제 출격하는 슬롯 개수
	void SetAfterSettings(TMap<FString, int32> sceneDatas);
	//. void SetTutorial();

public:
	void SetCharacterModeling(TOptional<FTESPartyInfo> partyUniqueInfo);
	void SetCharacterSlot(int32 slotIndex, int64 characterUnique, bool useChangeAction = true);
	void SetCharacterSlotByData(int32 slotIndex, class UTESGDCharacter* gdCharacter, bool useChangeAction = true);
	void SetCameraToCharacterHeight(int32 slotIndex);

public:
	void SetDraggedCharacter(int32 characterId, FVector characterLocation);
	void SetDraggedCharacterReleased();
	void SetDraggedCharacterLocation(FVector characterLocation);

private:
	void SetCharacterSlotBySlotIndex(int32 slotIndex, int64 characterUnique, bool useChangeAction = true);
	void SetCharacterSlotByCharacterData(int32 slotIndex, class UTESGDCharacter* gdCharacter, bool useChangeAction = true);

public:
	void SetCharacterMap(TMap<int32, class UTESGDCharacter*> finalJoinedPartyMap, bool isShowingModeling = true);		//. 데이터 세팅

public:
	void SetCharacterSwapSlotClicked(int32 slotIndex, bool isClicked);
	void SetSwapComplete();
	void SetSwapEnd();

private:
	void SetCharacterSwapDefaultSetting();

public:
	void SetPartySallyForced(bool isForced);

public:
	void RefreshParticipationCharacter();
	void SetParticipationCharacter(uint16 partyId, TArray<int64> characterUnique);
	void RefreshCharacterSlotInfo();
	void SetParticipationCharacterByCharacterId(uint16 partyUnique, TArray<int32> characterIdList);
	int32 RemoveParticipationCharacterByCharacterId(uint16 partyUnique, int32 characterId);

private:
	void SetFocusToCharacter(int32 slotIndex);

private:
	void SetBattleUsedPartyUniqueIdList(TArray<uint16> battleUsedPartyUniuqeIdList);

	//. 해당 파티 캐릭터 목록 중에서 같은 아이디를 사용하고 있는 캐릭터가 있는지 확인.(중복 캐릭터가 있는지에 대한 체크)
public:
	bool CheckIfThereIsSameCharacterIdInParty(uint16 partyUnique);
	bool CheckIfThereIsSameCharacterIdInParty(GamePacket::FTESPartyCharacterInfo partyInfo, int32 fixedIndex, OUT int32& removeIndex);


	//. 튜토리얼용 캐릭터 슬롯 하이라이트 표시
public:
	void SetCharacterEmptySlotHightLight(bool isVisible);
	void SetCharacterSlotHightList(bool isVisible);

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Get Functions ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
public:
	ETESPartySettingContents GetEntryRoute() const;
	uint16 GetUsedRecentlyPartyUniqueId() const;
	uint16 GetUsedRecentlyPartyUniqueIdByContents(ETESPartySettingContents contents) const;
	TArray<uint16> GetBattleUsedPartyUniqueIdList() const;
	int32 GetCurrentSelectedStageId() const;
//.	------------------ 통상 파티에서 쓰는 함수(캐릭터 유니크로 찾음)
	TArray<uint16> GetPartyUniqueListByCharacterUnique(int64 characterUnique) const;		//. 해당 캐릭터가 몇번 몇번 파티에 속해 있는지 가져온다.(유니크)
	TArray<int64> GetCharactersUniqueByPartyUnique(uint16 partyUnique) const;				//. 해당 파티에 캐릭터가 누구 누구 있는지 가져온다.(유니크)

//. ------------------ 체험 캐릭을 찾을때 쓰는 함수(캐릭터 아이디로 찾음)
	TArray<uint16> GetPartyUniqueByCharacterId(int32 characterId) const;					//. 해당 캐릭터가 어디 파티에 속해 있는지 가져온다.(아이디)
	TArray<int32> GetCharactersIdByPartyUnique(uint16 partyUnique) const;					//. 해당 파티에 캐릭터가 누구 누구 있는지 가져온다.(아이디)
	 
public:
	int32 GetNumberOfLockedSlot() const;
	int32 GetSelectedSlotIndex() const;

public:
	TArray<class UTESUWPartySettingSwapSlotInfo*> GetCharacterSwapSlotList() const;
	TArray<class UTESUWPartySettingCharSlotInfo*> GetCharacterInfoSlot() const;
	TMap<int32, class ATESCharacterPartySetting*> GetCharacterMap() const;
	TMap<int32, AActor*> GetCharacterSlotActorMap() const;
	TArray<class UTESGDCharacter*> GetCurrentPartyGDCharacterList() const;		//. 현재 파티에 편성된 GD캐릭터로 반환하는 함수

public:
	class UTESUWPartySettingDetail* GetPartySettingDetail() const;

public:
	FORCEINLINE const ETEPartySettingWidgetId&	GetCurrentContentsType() const {	return _curWidgetID; }

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Change Functions ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	void ChangeWidget(ETEPartySettingWidgetId widgetId, bool isInited = false);

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Tick Functions ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	void TickCameraMoving(const float& deltaTime);

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ OnEvent Functions ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
public:
	UFUNCTION() void OnEvent_SwitchPartySettings(int32 slotIndex);

	UFUNCTION() void OnEvent_BackWidget();

	UFUNCTION() void OnEvent_ConfirmCharacter();

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Member Variables ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	//. UPROPERTY() ETESPartySettingEntryRoute _entryRoute = ETESPartySettingEntryRoute::None;
	UPROPERTY() ETEPartySettingWidgetId  _curWidgetID = ETEPartySettingWidgetId::None;
	UPROPERTY() int32 _selectedSlotIndex = 0;
	UPROPERTY() ETESPartySettingContents _partySettingByContents = ETESPartySettingContents::None;

private:
	UPROPERTY() TMap<FString, int32> _sceneDatas = {};

private:
    UPROPERTY() ACameraActor* _sc_sceneCamera = nullptr;
	UPROPERTY() AActor* _soa_sceneCamera_default = nullptr;
    UPROPERTY()	FVector _partySettingCameraDefaultLocation = FVector::ZeroVector;
    UPROPERTY() FVector _partySettingCameraTargetLocation = FVector::ZeroVector;
	UPROPERTY() FRotator _partySettingCameraDefaultRotation = FRotator::ZeroRotator;
	UPROPERTY() FRotator _partySettingCameraTargetRotation = FRotator::ZeroRotator;
    UPROPERTY() float _partySettingCameraDefaultFov = 0.0f;
    UPROPERTY() bool _isJoomingIn = false;										//. 줌인/아웃 상태
    UPROPERTY() bool _isFocusingToCharacter = false;							//. 현재 포커싱이 되어 있는지

private:
	TMap<int64, TArray<uint16>> _partyUniqueListByCharacterUnique = {};				//. 캐릭터가 속한 파티 유니크. (key : 캐릭터 유니크, value : 각 파티 유니크 리스트) => 한 캐릭터가 몇번 몇번 파티에 들어가 있는지. => 한 캐릭터가 최대 48개 파티까지 들어갈수 있다.
	TMap<uint16, TArray<int64>> _charactersUniqueByPartyUnique = {};				//. 한 파티에 들어있는 캐릭터들의 유니크 아이디. (key : 파티 유니크, value : 각 파티 안에 있는 캐릭터들의 유니크) => 1파티에 캐릭터 최대4명.
	TMap<int32, TArray<uint16>> _partyUniqueListByCharacterId = {};					//. 캐릭터가 속한 파티 유니크. (key : 캐릭터 아이디, value : 각 파티 유니크 리스트)
	TMap<uint16, TArray<int32>> _charactersIdByPartyUnique = {};					//. 한 파티에 들어있는 캐릭터들의 아이디.

private:
	UPROPERTY() TMap<int32, AActor*> _characterLocationSlotActorMap = {};						//. 캐릭터 위치 슬롯
	UPROPERTY() TMap<int32, class ATESCharacterPartySetting*> _characterMap = {};				//. 캐릭터 모델링(슬롯, 모델링)
	UPROPERTY() TArray<class UTESUWPartySettingCharSlotInfo*> _charSlotInfoWidgetList = {};		//. 캐릭터 정보(위젯)
	UPROPERTY() TArray<class UTESUWPartySettingSwapSlotInfo*> _characterSwapSlotList = {};		//. 캐릭터 자리 교체 슬롯
	UPROPERTY() TArray<int32> _lockedCharacterList = {};										//. 강제 출격 캐릭터 리스트(좌측부터)
	UPROPERTY() class ATESCharacterPartySetting* _draggedCharacter = nullptr;					//. 자리 교체시 드래그 할때 따라오는 임시 캐릭터

private:
	UPROPERTY() class UTESUWPartySettingMain* _mainPartySettingWidget = nullptr;
	UPROPERTY() class UTESUWTeamTagPartySettingMain* _teamTagPartySettingWidget = nullptr;
	UPROPERTY() class UTESUWMirageGardenPartySetting* _mirageGardenPartySettingWidget = nullptr;
	UPROPERTY() class UTESUWPartySettingDetail* _mainPartySettingDetailWidget = nullptr;

private:
	UPROPERTY() uint16 _recentUsedPartyUniqueId = GameDefine::PARTY_START_UNIQUE;
	UPROPERTY() int32 _clickedSlotIndex = 0;
	UPROPERTY() bool _isCurrentPartySallyForced = false;
	UPROPERTY() int32 _selectedStageId = 0;

private:
	UPROPERTY() TArray<uint16> _battleUsedPartyUniqueIdList = {};

private:
	UPROPERTY() bool _isSwapSlotClicked = false;

private:
	UPROPERTY() int32 _numofLockedSlot = 0;
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
};
