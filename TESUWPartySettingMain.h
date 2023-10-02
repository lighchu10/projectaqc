// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TES.h"
#include "Structs/UI/TESUWGenericBase.h"
#include "TESUWPartySettingMain.generated.h"

/**
 * [ Note ] 소탕 버튼에 대한 상태입니다.
 */
UENUM()
enum class ETESSweepButtonState : uint8
{
	None,			//. 아무것도 아닙니다.
	Collapsed,		//. 영역도 차지 안하고 보여주지도 않습니다.
	Activation,		//. 활성화된 상태로 보여줍니다.
	Deactivation,	//. 비활성화된 상태로 보여줍니다.
	Max
};

/**
 * 
 */
UCLASS()
class TES_API UTESUWPartySettingMain : public UTESUWGenericBase
{
	GENERATED_BODY()

//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Override Functions ] 
//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
public:
	virtual void InitWidget() override;
	virtual void ClearWidget() override;
	virtual void OpenWidget() override;
	virtual void CloseWidget() override;
	virtual void OpenDelegate() override;
	virtual void CloseDelegate() override;

//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Init Functions ] 
//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	void InitStageInfoPanel();
	void InitPartySettingButtons();
	void InitPartyRenameButton();
	void InitStageName();
	void InitPresetTabName();
	void InitPartyName();
	void InitPartyCombatPower();
	void InitRecommandedCombatPower();
	//void InitSwapCharacterSlot();
	void InitPartyPresetButton();
	void InitPartyEditIcon();
	void InitStageInfoRoot();
	void InitPartyDisperseButton();
	//void InitCharacterSwapButton();
	void InitAttributionCharacterRequiredList();
	void InitMaterialRequiredList();
	void InitStellaStrikeRoot();
	void InitStellaStrikeIcon();
	void InitStellaStrikeName();
	void InitAttributionIcon();
	void InitStartBattleButton();
	void InitMaterialChecking();
	void InitBuffIconButton();
	void InitRootToolTip();
	void InitRootRecommandedCombatPower();
	void InitReplayStory();
	void InitWidgetSwitcher();
	void InitStaminaCount();

private:
	void InitCommonHeader();
	void InitCommonRetryCount();

private:
	void InitSweepButton();
	void InitSweepButtonBySweepTickCount();

//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Clear Functions ] 
//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
public:
	void ClearCommonHeader();
	void ClearCommonRetryCount();
	void ClearCharacterConditionWidgets();
	void ClearMaterialConditionWidgets();

//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Set Functions ] 
//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
public:
	void SetWidget(FTESPartyGroupInfo partyGroupInfo, FTESPartyInfo partyInfo, bool sallyForced = false, int32 stageId = 0, ETESCommonRemainCountType remainCountType = ETESCommonRemainCountType::None);
	void SetStageInfo(int32 stageId);

public:
	void RefreshPartyCombatPower();
	void ReFreshStellaStrikeInfo();

private:
	void SetCommonHeader();

private:
	bool SetData(FTESPartyGroupInfo partyGroupInfo, FTESPartyInfo partyInfo, int32 stageId);
	void SetStageInfoPanel(bool isVisible);
	void SetPartySettingButtons(bool isMaterialRequired);
	void SetPartyRenameButton();
	void SetPresetTabName();
	void SetPartyName();
	void SetPartyCombatPower();
	void SetRecommandedCombatPower();
	void SetPartyPresetButton();
	void SetPartyEditIcon();
	void SetSlotLocking();
	void SetStageInfoVisible(bool isVisible);
	void SetStageInfoName(int32 chapterNumber, int32 stageNumber);
	void SetSlotClosing();			//. 편성 불가 슬롯
	void SetRequiredAttributionCharacterList(int32 stageId);
	void SetRequiredMaterialList(int32 stageId);
	void SetStellaStrikeInfo();
	void SetRequiredMaterialActivation(int32 stageId);
	void SetRequiredMaterialCheckState(bool isChecked);
	void SetBuffIconButton();
	void SetRootToolTip(bool isVisible);
	void SetRootRecommandedCombatPowerPanelVisible(bool isVisible);
	void SetReplayStory(bool isReplay);
	void SetRequireStamina(int32 stageId);

private:
	void SetStartBattleButton(bool isEnabled);
	void SetDispersePartyButton(bool isEnabled);
	//void SetCharacterSwapButtonEnable(bool isEnabled);

private:
	void SetSweepButton(ETESSweepButtonState sweepButtonState);
	void SetSweepButtonBySweepTicketCount(int32 sweepTicketCount);

private:
	void SetRemainCount(ETESCommonRemainCountType remainCountType);
//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Misc Functions ] 
//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
public:
	float GetCameraLocationY() const;

public:
	void CharacterSwapCanceled();
	void SetCharacterSwapEnd();

public:
	void SetCurrentGroupName();
	void SetStartAvailable(bool canBeStarted);

private:
	void SetAppearAnim();

private:
	void PlayAppearAnim(FOnTESAction finishAction);

private:
	bool CheckIfStartStage();
	void StartTriggerEvent(const FStageRow& stageRow);
	void StartBattle();

//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Refresh Functions ] 
//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
public:
	void RefreshSweepButton();
	void RefreshSweepButtonBySweepTicketCount();
	void RefreshStartBattleButton();
	void RefreshCharacterSlotInfo();
	void RefreshRequiredMaterialList();

//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Check Functions ] 
//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	//. 해당 파티 캐릭터 목록이 해당 스테이지의 파티 입장 조건에 충족한지 알아옵니다. ( true : 만족, false : 불만족 )
	bool CheckPartyConditions(TArray<int64> characterUniqueList, TArray<FTESPartyCondition> partyConditionTypes);

//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ OnEvent Functions ] 
//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_BackToPrevious();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_BackToHome();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_RenameCurrentParty();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_OpenPartyPreset();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_SwapSlot();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_DisperseCurrentParty();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_AutoSettingCurrentParty();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_OpenStageInfoDetail();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_OpenStellaStrikeInfoPopup();

public:
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_StartBattle();

private:
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_OpenToolTip();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_ToggleReplayStory();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_Sweep();

	UFUNCTION()
	void OnEvent_SweepResultRecieved(struct FTESSweepResultData sweepResultData);

	UFUNCTION()
	void OnEvent_OpenedUniqueAbyssSuccessfully();

	UFUNCTION()
	void OnEvent_RefreshPartyName(uint16 partyUnique, FString partyName);

	UFUNCTION()
	void OnEvent_CallbackChangedPartyMembers();

public:
	void OnEvent_BackWidget();

private:
	UFUNCTION() void OnEvent_EnterBattle();

	UFUNCTION() void OnEvent_OpenUniqueAbyssStage();

	UFUNCTION() void OnEvent_ItemDataUpdated();

	UFUNCTION() void OnEvent_CommonRewardComfirmed();

//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Member Variables ] 
//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	UPROPERTY(EditAnywhere, Category = "CameraLocation", Meta = (AllowPrivateAccess = "true"))
	float _cameraLocationY = 0.0f;

private:
	FDelegateHandle _dh_partyInfo = {};
	FDelegateHandle _dh_partyChanged = {};
	FDelegateHandle _dh_sweepResultRecieved = {};
	FDelegateHandle _dh_itemDataUpdated = {};
	FDelegateHandle _dh_commonRewardComfirmed = {};

private:
	UPROPERTY() FTimerHandle _th_fadeInAction = {};
	UPROPERTY() FTimerHandle _th_appearAnim = {};
	UPROPERTY() FTimerHandle _th_disappearAnim = {};

private:
	//. 현재 파티의 해당되는 탭 정보
	UPROPERTY() uint8 _uniqueTabId = 0;
	UPROPERTY() FString _partyPrestTabName = TEXT("");

private:
	//. 현재 파티 정보
	UPROPERTY() uint16 _currentPartyUnique = 0;
	UPROPERTY() FString _currentPartyName = TEXT("");
	UPROPERTY() TArray<int64> _currentCharacterUniqueList = {};

private:
	UPROPERTY() int32 _curStageId = 0;
	/*UPROPERTY() */FStageRow _curStageRow = {};
	UPROPERTY() bool _isStarted = false;
	UPROPERTY() bool _isToolTipOpened = false;

private:
	UPROPERTY() class UTESUWCommonHeader* _commonHeaderWidget = nullptr;
	UPROPERTY() class UTESUWCommonRetryCount* _commonRetryWidget = nullptr;

private:
	UPROPERTY() bool _isBackToPrevious = false;
	UPROPERTY() bool _isDispersingParty = false;
	UPROPERTY() bool _isAutoSettingParty = false;
	UPROPERTY() bool _isCharacterSwapping = false;
	UPROPERTY() bool _isBackToHome = false;

private:
	//. 캐릭터 목록창이 출현중인가?
	UPROPERTY() bool _isPlayingAppearAnim = false;

	//. 캐릭터 목록창이 사라지는 중인가?
	UPROPERTY() bool _isPlayingDisappearAnim = false;
	
private:
	UPROPERTY() bool _isSallyForced = false;				//. 강제 출격 파티인지
	UPROPERTY() bool _isPlayStory = false;					//. 스토리 리플레이

#pragma region UMG Widget
private:
	UPROPERTY()	TArray<class UCanvasPanel*> _cp_stellaStrikeList = {};			//. 스텔라 스트라이크 정보
	UPROPERTY() TArray<class UTESImage*> _img_stellStrikeIconList = {};			//. 스텔라 스트라이크 아이콘
	UPROPERTY() TArray<class UTESTextBlock*> _txt_stellStrikeNameList = {};		//. 스텔라 스트라이크 이름
	UPROPERTY() TArray<class UTESImage*> _img_attributionIcon = {};				//. 속성

private:
	UPROPERTY() class UHorizontalBox* _hb_stageInfo = nullptr;					//. 스테이지 정보 묶음.
	UPROPERTY() class USpacer* _sp_spacer = nullptr;							//. 스테이지 정보 사이에 있음 스페이스.
	UPROPERTY() class UHorizontalBox* _hb_bottomButtons = nullptr;				//. 파티편성 버튼들 묶음
	UPROPERTY() class UTESButton* _btn_partyPresetRename = nullptr;				//. 파티 프리셋 이름바꾸기 버튼	
	UPROPERTY()	class UTESTextBlock* _txt_stageName = nullptr;					//. 강제 출격일때 나오는 이름(스테이지 이름)
	UPROPERTY()	class UTESTextBlock* _txt_presetTabName = nullptr;				//. 파티 그룹 이름
	UPROPERTY()	class UTESTextBlock* _txt_partyName = nullptr;					//. 파티 이름
	UPROPERTY()	class URichTextBlock* _rtxt_partyCombatPower = nullptr;			//. 파티 전투력
	UPROPERTY()	class URichTextBlock* _rtxt_recommandedCombatPower = nullptr;	//. 해당 스테이지 권장 전투력
	//UPROPERTY() class UTESTextBlock* _txt_swapSlot = nullptr;					//. 교체 슬롯
	//UPROPERTY() class UTESTextBlock* _txt_swapEndSlot = nullptr;				//. 교체 슬롯
	UPROPERTY() class UTESButton* _btn_partyPreset = nullptr;					//. 파티 프리셋 버튼
	UPROPERTY() class UTESImage* _img_editIcon = nullptr;						//. 파티 이름 교체 아이콘
	UPROPERTY()	class UCanvasPanel* _cp_stageInfo = nullptr;					//. 스테이지 정보
	//UPROPERTY() class UTESButton* _btn_characterSwap = nullptr;					//. 캐릭터 자리 교체 버튼
	UPROPERTY() class UTESWrapBox* _wb_materialRequired = nullptr;				//. 필요 재료 리스트
	UPROPERTY() class UVerticalBox* _vb_characterAttributionRequired = nullptr;	//. 필요한 속성 캐릭터 리스트
	UPROPERTY() class UCanvasPanel* _cp_requiredMaterialChecking = nullptr;		//. 이미 투입했는지 안했는지에 대한 체크박스 패널
	UPROPERTY() class UCheckBox* _chkb_requiredMaterial = nullptr;				//. 이미 투입했는지.
	UPROPERTY() class UCanvasPanel* _cp_recommandedCombatPowerPanel = nullptr;	//. 추천 전투력 패널
	UPROPERTY() class UTESButton* _btn_replayStory = nullptr;
	UPROPERTY() class UTESCheckBox* _chkb_replayStory = nullptr;				//. 스토리 리플레이
	UPROPERTY() class UTESWidgetSwitcher* _ws_consume = nullptr;				//. 시작 시 소비 정보 위젯 스위처
	UPROPERTY() class UTESTextBlock* _txt_staminaCount = nullptr;
	UPROPERTY() class UHorizontalBox* _hb_stamina = nullptr;

private:
	UPROPERTY() class UCanvasPanel* _cp_rootToolTip = nullptr;					//. 압도버프 설명
	UPROPERTY() class UTESButton* _btn_buffIcon = nullptr;						//. 압도버프 아이콘 버튼

private:
	UPROPERTY() class UTESUWCommonToggleSwitcher* _cts_disperseParty = nullptr; //. 파티 해산 버튼 스윗쳐
	UPROPERTY() class UTESUWCommonToggleSwitcher* _cts_startBattle = nullptr;	//. 전투 시작 버튼 스윗쳐

private:
	UPROPERTY() class UTESUWCommonToggleSwitcher* _cts_sweep = nullptr;			//. 소탕 버튼 스윗쳐
	UPROPERTY() class UTESTextBlock* _txt_sweep_on_sweepTicketCount = nullptr;			//. 소탕 버튼 ON 상태일때 아이템 개수
	UPROPERTY() class UTESTextBlock* _txt_sweep_off_sweepTicketCount = nullptr;		//. 소탕 버튼 OFF 상태일때 아이템 개수

private:
	UPROPERTY() TArray<class UTESUWPartySettingCharCondition*> _characterConditionWidgets = {};
	UPROPERTY() TArray<class UTESUWPartySettingRequiredList*> _materialConditionWidgets = {};
#pragma endregion UMG Widget
};
