// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TES.h"
#include "Structs/UI/TESUWGenericBase.h"
#include "TESUWCelestialRoadMain.generated.h"

/**
 * 
 */
UCLASS()
class TES_API UTESUWCelestialRoadMain : public UTESUWGenericBase
{
	GENERATED_BODY()

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Override Functions ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
public:
	virtual void InitWidget() override;
	virtual void ClearWidget() override;
	virtual void OpenWidget() override;
	virtual void CloseWidget() override;
	virtual void OpenDelegate() override;
	virtual void CloseDelegate() override;
	virtual void NativeTick(const FGeometry& myGeometry, float deltaTime) override;

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Init Functions ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	void InitCommonHeaderWidget();
	void InitSeasonPassDailyRemainingTimeText();
	void InitSeasonPassEndRemainingTimeText();
	void InitSeasonPassCurrentLevelText();
	void InitSeasonPassFinalRewardInfo();
	void InitSeasonPassEXPProgressBar();

private:
	//. 시즌패스 토글 및 컨텐츠 내용물
	void InitSeasonPassRootPremimumBlock();					//. 천체의 나침반 구매해야하는 블록.
	void InitSeasonPassCategoryToggleSwitcher();			
	void InitSeasonPassCategoryNotice();
	void InitSeasonPassContentsSwitcher();					//. 대분류(보상 혹은 미션에 관련된 내용)
	void InitSeasonPassMissionCategoryToggleSwitcher();
	void InitSeasonPassMissionCategoryNotice();

private:
	//. 시즌보상
	void InitSeasonPassRewardRootCanvas();
	void InitSeasonPassRewardScrollBox();
	void InitRootNormalRewardClaimed();
	void InitRootPremiumRewardClaimed();

private:
	//. 우측 고정에 있는 보상 리스트.
	void InitSeasonPassRootFixedReward();
	void InitSeasonPassFixedReward();
	void InitSeasonPassFixedLevel();

private:
	void InitSeasonPassPurchaseLevelUpWidgetSwitcher();
	void InitSeasonPassPurchaseCelestialRoadWidgetSwitcher();
	void InitSeasonPassClaimAllRoadWidgetSwitcher();

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Clear Functions ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	void ClearCommonHeaderWidget();
	void ClearRewardSlotList();

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Tick Functions ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	//. 경험치를 증가시키는 연출을 위한 틱입니다.
	void TickExperienceIncrease(float deltaTime);

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Set Functions ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
public:
	void SetWidget(ETESCelestialRoadType categoryType);

private:
	bool SetData();

private:
	void SetDailyMissionResetTime(int64 timeStamp);
	void SetSeasonPassResetTime(int64 timeStamp);

private:
	void SetSeasonPassCurrentLevel(int32 level);
	void SetSeasonPassCurrentExpText(int32 currentExp);
	void SetSeasonPassCurrentEXPProgress(float value);
	void SetSeasonPassTotalExpText();
	void SetSeaonPassFinalRewardInfo(int32 celestialRoadTableId);
	void SetSeasonPassCategoryNotice();
	void SetSeasonPassCategoryNotice(ETESCelestialRoadType celestialRoadType);
		
private:
	//. 보상 및 임무 탭
	void SetSeasonPassPremimumBlock(bool isPurchased);
	void SetSeasonPassType(ETESCelestialRoadType categoryType);
	void SetSeasonPassCategories(ETESCelestialRoadType categoryType, bool isOn);
	void SetSeasonPassContentsSwitcher(ETESCelestialRoadType categoryType);
	void SetSeasonPassRewardList();

private:
	//. 임무 관련 탭(일일 및 시즌 임무)
	void SetSeasonPassMissionType(ETESCelestialRoadMissionType missionCategoryType);
	void SetSeasonPassMissionCategories(ETESCelestialRoadMissionType missionCategoryType, bool isOn);
	void SetSeasonPassMissionContents(ETESCelestialRoadMissionType missionCategoryType);
	void SetSeasonPassMissionContainer(ETESCelestialRoadMissionType missionCategoryType);
	void SetSeasonPassMissionCategoryNotice();
	void SetSeasonPassMissionCategoryNotice(ETESCelestialRoadMissionType missionCategoryType);

private:
	//. 우측 고정 슬롯 보상 아이템 세팅
	void SetSeasonPassFixedReward(int32 rewardTableId, bool forceRefresh = false);
	void SetNormalRewardSlot(int32 rewardTableId);
	void SetPremiumRewardSlot(int32 rewardTableId);
	void SetPremiumSlotLevel(int32 rewardTableId);
	void SetNormalRewardClaimed(int32 level);
	void SetPremiumRewardClaimed(int32 level);

private:
	//. 각종 버튼들 위젯 스위쳐
	void SetSeasonPassPurchaseLevelUpWidgetSwitcher(bool isOn);
	void SetSeasonPassPurchaseCelestialRoadWidgetSwitcher(bool isPurchased);
	void SetSeasonPassPurchaseCelestialRoadWidgetSwitcherByEndOfPurchaseableTime();
	void SetSeasonPassClaimAllRoadWidgetSwitcher(bool isOn);

private:
	void SetRewardScroll(int32 currentLevel);

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Get Functions ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
	ETypeMission GetMissionTypeByMissionCategoryType(ETESCelestialRoadMissionType missionCategoryType);

	//. 우측 고정형 슬롯 몇레벨 보상 보여줄지 판단하는 함수.
	int32 GetRewardTableIdByCurrentPositionInScroll();

	int32 CheckCurrentLevelInCurrentScrollOffset(float rightScrollOffset);

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ General Functions ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	void PlayUnlockPremiumRewardsAnim();

public:
	void PlayExpGainingAnim();
	void PlayExpGainingSound();
	void PlayLevelUpPopup();

public:
	void RefreshAll();

private:
	void RefreshExpWidgets();

private:
	void ShowExpiredMessage();

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ OnEvent Functions ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_ChangeSeasonPassCategory(ETESCelestialRoadType categoryType);						//. 보상 및 미션 탭 클릭시 호출 되는 함수.

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_ChangeMissionCategory(ETESCelestialRoadMissionType missionCategoryType);			//. 일일 및 시즌 임무 탭 클릭시 호출 되는 함수.

private:
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_PurchaseLevelEXP();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_CannotPurchaseLevelEXPAnymore();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_PurchasePremiumSeasonPsss();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_AlreadyPurchasedPremiumSeasonPsss();

private:
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_ClaimAll();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_AlreadyClaimAll();

	UFUNCTION()
	void OnEvent_MissionUpdated();

	UFUNCTION()
	void OnEvent_Close();

private:
	UFUNCTION()
	void OnEvent_PurchaseLevelup();

	UFUNCTION()
	void OnEvent_GainExp(int32 gainedExp);

private:
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_ShowFixedItemInfo();

private:
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_Home();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_Back();

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Member Variables ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	FDelegateHandle _dh_missionUpdate = {};
	//FDelegateHandle _dh_commonRewardConfirm = {};
	UPROPERTY() FTimerHandle _th_playAnim = {};
	UPROPERTY() FTimerHandle _th_closeAnim = {};

	//. 점수 연출시 소리 업데이트 타이머
	UPROPERTY() FTimerHandle _th_updateExpSound = {};

private:
	//. 보상 및 미션 카테고리
	UPROPERTY() ETESCelestialRoadType _previousCategoryType = ETESCelestialRoadType::None;
	UPROPERTY() ETESCelestialRoadType _currentCategoryType = ETESCelestialRoadType::None;

private:
	//. 일일 및 시즌 미션 카테고리
	UPROPERTY() ETESCelestialRoadMissionType _previousMissionCategoryType = ETESCelestialRoadMissionType::None;
	UPROPERTY() ETESCelestialRoadMissionType _currentMissionCategoryType = ETESCelestialRoadMissionType::None;

private:
	UPROPERTY() int32 _needExpTotalByLevel = 0;			//. 한 레벨업 당 필요한 총량 경험치.(일정함)
	UPROPERTY() int32 _totalExp = 0;					//. 현재 계정이 갖고 있는 시즌패스의 총 경험치량
	UPROPERTY() int32 _currentExpByLevel = 0;			//. 시즌패스의 현재 레벨의 경험치량.
	UPROPERTY() int32 _gainedExp = 0;					//. 미션 수행으로 얻은 경험치량
	UPROPERTY() int32 _remainExpInCurrentLevel = 0;		//. 남은 경험치(현재 레벨 기준)

private:
	UPROPERTY() TArray<int32> _exposureLevelList = {};
	UPROPERTY() int32 _fixedRewardTableId = 0;			//. 우측 슬롯 리워드 아이템 아이디.
	UPROPERTY() int32 _finalRewardItemId = 0;			//. 맨 우측 스페셜 아이템 아이디.

private:
	UPROPERTY() bool _isChangeContents = false;
	UPROPERTY() bool _isLevelingUp = false;			//. 패스 경험치 연출중
	UPROPERTY() bool _isPurchasingCompass = false;	//. 나침반 구매 후 발생하는 연출중

private:
	UPROPERTY() FTESCelestialRoadData _currentSeasonPassInfo = {};
	UPROPERTY() int32 _currentLevel = 0;			//. 현재 레벨.
	UPROPERTY() int32 _beforeLevel = 0;				//. 경험치 및 레벨구매를 하기 전의 레벨.
	UPROPERTY() int32 _maximumLevel = 0;			//. 한 시즌패스에서 최고 레벨.

private:
	UPROPERTY() class UTESUWItemSlot* _wbp_normalFixedItemSlot = nullptr;			//. 우측 고정 노말 보상.

private:
	UPROPERTY() TArray<class UTESUWItemSlot*> _preminumFixedItemSlotList = {};		//. 우측 고정 프리미엄 보상.

private:
	UPROPERTY() class UTESUWCelestialRoadMission* _currentMissionContentsWidget = nullptr;		//. 미션 위젯

private:
	UPROPERTY() TArray<class UTESUWCelestialRoadRewardSlot*> _celestialRoadRewardSlotList = {};

private:
	//. 현재 경험치 연출의 시간
	UPROPERTY()	float _experienceCurrentTime = 0.0f;

	//. 경험치 연출 시간
	UPROPERTY()	float _experienceTotalTime = 0.0f;

#pragma region UMG Widget
private:
	UPROPERTY() class UTESUWCommonHeader* _commonHeaderWidget = nullptr;
	UPROPERTY() class UTESTextBlock* _txt_currentSeasonPassLevel = nullptr;
	UPROPERTY() class UTESTextBlock* _txt_finalRewardLevel = nullptr;
	UPROPERTY() class UTESImage* _img_finalRewardIcon = nullptr;
	UPROPERTY() class UTESTextBlock* _txt_expProgessLevel = nullptr;
	UPROPERTY() class UTESTextBlock* _txt_totalExpProgessLevel = nullptr;
	UPROPERTY() class UProgressBar* _pb_expProgress = nullptr;
	UPROPERTY() class UProgressBar* _pb_gainExpProgress = nullptr;

private:
	UPROPERTY() class UTESTextBlock* _txt_seasonEndRemainingTime = nullptr;
	UPROPERTY() TArray<class UTESTextBlock*> _txt_dailyRemainingTimeList = {};
	UPROPERTY() TArray<class UTESTextBlock*> _txt_seasonEndRemainingTimeList = {};

private:
	UPROPERTY() class UTESTextBlock* _txt_fixedLevel = nullptr;

private:
	UPROPERTY() class UWidget* _cp_rootReward = {};
	UPROPERTY() class UTESScrollBox* _scrb_seasonPassRewardList = {};
	UPROPERTY() class USizeBox* _sb_dummyBox = nullptr;

private:
	UPROPERTY() class UTESWidgetSwitcher* _ws_purchaseLevelUp = nullptr;
	UPROPERTY() class UTESWidgetSwitcher* _ws_purchaseCelestialRoad = nullptr;
	UPROPERTY() class UTESWidgetSwitcher* _ws_claimAll = nullptr;
	
private:
	//. 보상 및 미션 탭.
	UPROPERTY() TMap<ETESCelestialRoadType, class UTESUWCommonToggleSwitcher*> _cts_seasonPassCategories = {};
	UPROPERTY() TMap<ETESCelestialRoadType, class UTESImage*> _img_seasonPassCategoryNotices = {};

private:
	//. 우측 고정 슬롯 패널
	UPROPERTY() class UCanvasPanel* _cp_rootFixedSlot = nullptr;

private:
	//. 내용물.(보상 혹은 미션)
	UPROPERTY() class UCanvasPanel* _cp_rootPremimumSlot = nullptr;		//. 프리미엄 슬롯 블락.
	UPROPERTY() class UTESWidgetSwitcher* _ws_seasonPassContentsSwitcher = nullptr;

private:
	UPROPERTY() class UCanvasPanel* _cp_rootNormalRewardClaimed = nullptr;
	UPROPERTY() TArray<class UCanvasPanel*> _cp_rootPremiumRewardListClaimed = {};

private:
	//. 미션 탭(일일미션 및 시즌미션)
	UPROPERTY() TMap<ETESCelestialRoadMissionType, class UTESUWCommonToggleSwitcher*> _cts_seasonPassMissionCategories = {};
	UPROPERTY() TMap<ETESCelestialRoadMissionType, class UTESImage*> _img_seasonPassMissionCategoryNotices = {};
#pragma endregion UMG Widget
};
