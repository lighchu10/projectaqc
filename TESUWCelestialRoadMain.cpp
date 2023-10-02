// Fill out your copyright notice in the Description page of Project Settings.

#include "Contents/CelestialRoad/UI/TESUWCelestialRoadMain.h"
#include "Contents/Common/Manager/TESSceneManager.h"
#include "Contents/Common/Manager/TESTimeManager.h"
#include "Contents/Common/Manager/TESPopupManager.h"
#include "Contents/Common/Manager/TESUIManager.h"
#include "Contents/Common/UI/TESUWCommonHeader.h"
#include "Contents/Common/UI/TESUWCommonLevelUp.h"
#include "Contents/Common/UI/TESUWCommonToggleSwitcher.h"
#include "Contents/Common/UI/Item/TESUWItemSlot.h"
#include "Contents/Common/UI/CustomSlates/TESImage.h"
#include "Contents/Common/UI/CustomSlates/TESTextBlock.h"
#include "Contents/Common/UI/CustomSlates/TESScrollBox.h"
#include "Contents/Common/UI/CustomSlates/TESWidgetSwitcher.h"
#include "Contents/CelestialRoad/Manager/TESCelestialRoadManager.h"
#include "Contents/CelestialRoad/UI/TESUWCelestialRoadRewardSlot.h"
#include "Contents/CelestialRoad/UI/TESUWCelestialRoadMission.h"
#include "Network/TESNetworkManager.h"
#include "Contents/Sound/Manager/TESSoundManager.h"
#include "Contents/Mission/Manager/TESMissionManager.h"
#include "Structs/GameData/TESGDGoodsBase.h"
#include "Common/GameData/TESGDGoodsItem.h"
#include "Components/SizeBox.h"
#include "Components/CanvasPanel.h"
#include "TESUtils.h"
#include "TESUtilTable.h"
#include "TESUtilString.h"
#include "TESUtilTexture.h"

void UTESUWCelestialRoadMain::InitWidget()
{
	Super::InitWidget();

	InitCommonHeaderWidget();

	InitSeasonPassDailyRemainingTimeText();

	InitSeasonPassEndRemainingTimeText();

	InitSeasonPassCurrentLevelText();

	InitSeasonPassFinalRewardInfo();

	InitSeasonPassEXPProgressBar();

	InitSeasonPassCategoryToggleSwitcher();

	InitSeasonPassCategoryNotice();

	InitSeasonPassContentsSwitcher();

	InitSeasonPassRootPremimumBlock();

	InitSeasonPassMissionCategoryToggleSwitcher();

	InitSeasonPassMissionCategoryNotice();

	InitSeasonPassRewardRootCanvas();

	InitSeasonPassRewardScrollBox();

	InitRootNormalRewardClaimed();

	InitRootPremiumRewardClaimed();

	InitSeasonPassRootFixedReward();

	InitSeasonPassFixedReward();

	InitSeasonPassFixedLevel();

	InitSeasonPassPurchaseLevelUpWidgetSwitcher();

	InitSeasonPassPurchaseCelestialRoadWidgetSwitcher();

	InitSeasonPassClaimAllRoadWidgetSwitcher();
}

void UTESUWCelestialRoadMain::ClearWidget()
{
	Super::ClearWidget();

	ClearCommonHeaderWidget();

	ClearRewardSlotList();

	if (nullptr != _sb_dummyBox)
	{
		_sb_dummyBox = nullptr;
	}

	_dh_missionUpdate = {};

	GetWorld()->GetTimerManager().ClearTimer(_th_playAnim);
	_th_playAnim = {};

	GetWorld()->GetTimerManager().ClearTimer(_th_closeAnim);
	_th_closeAnim = {};

	GetWorld()->GetTimerManager().ClearTimer(_th_updateExpSound);
	_th_updateExpSound = {};
}

void UTESUWCelestialRoadMain::OpenWidget()
{
	Super::OpenWidget();

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	PlayWGAnimation(TEXT("Ani_Open"));

	if (true == ::IsValid(_commonHeaderWidget))
	{
		_commonHeaderWidget->OpenWidget();
	}
}

void UTESUWCelestialRoadMain::CloseWidget()
{
	Super::CloseWidget();

	SetVisibility(ESlateVisibility::Collapsed);

	if (true == ::IsValid(_commonHeaderWidget))
	{
		_commonHeaderWidget->CloseWidget();
	}
}

void UTESUWCelestialRoadMain::OpenDelegate()
{
	Super::OpenDelegate();

	SAFE_ADD_UOBJECT_DELEGATE(FTESUtilEvents::OnMissionUpdated, OnEvent_MissionUpdated, _dh_missionUpdate);

	SAFE_BIND_UOBJECT_DELEGATE(FTESUtilEvents::OnCelestialRoadGainExp, OnEvent_GainExp);

	SAFE_BIND_UOBJECT_DELEGATE(FTESUtilEvents::OnCelestialRoadPurchaseCompass, PlayUnlockPremiumRewardsAnim);

	SAFE_BIND_UOBJECT_DELEGATE(FTESUtilEvents::OnCelestialRoadPurchaseLevelUp, OnEvent_PurchaseLevelup);

	SAFE_BIND_UOBJECT_DELEGATE(FTESUtilEvents::OnCelestialRoadClaimReward, RefreshAll);
}

void UTESUWCelestialRoadMain::CloseDelegate()
{
	Super::CloseDelegate();

	SAFE_REMOVE_UOBJECT_DELEGATE(FTESUtilEvents::OnMissionUpdated, _dh_missionUpdate);

	SAFE_UNBIND_UOBJECT_DELEGATE(FTESUtilEvents::OnCelestialRoadGainExp);

	SAFE_UNBIND_UOBJECT_DELEGATE(FTESUtilEvents::OnCelestialRoadPurchaseCompass);

	SAFE_UNBIND_UOBJECT_DELEGATE(FTESUtilEvents::OnCelestialRoadPurchaseLevelUp);

	SAFE_UNBIND_UOBJECT_DELEGATE(FTESUtilEvents::OnCelestialRoadClaimReward);
}

void UTESUWCelestialRoadMain::NativeTick(const FGeometry& myGeometry, float deltaTime)
{
	Super::NativeTick(myGeometry, deltaTime);

	UWorld* worldContext = GetWorld();
	if (false == ::IsValid(worldContext))
	{
		return;
	}

	TickExperienceIncrease(deltaTime);														//. 레벨업 연출.

	SetDailyMissionResetTime(SharedMissionManager->GetDailyMissionTimeStamp());				//. 시즌패스 임무시간.

	SetSeasonPassResetTime(SharedCelestialRoadManger->GetSeasonPassMissionTimeStamp());		//. 시즌패스 종료 시간.

	SetSeasonPassFixedReward(GetRewardTableIdByCurrentPositionInScroll());					//. 스크롤에 따라 우측 고정 슬롯 아이템 정보 변경

	SetSeasonPassPurchaseCelestialRoadWidgetSwitcherByEndOfPurchaseableTime();				//. 나침반 구매시간 체크.
}

void UTESUWCelestialRoadMain::InitCommonHeaderWidget()
{
	_commonHeaderWidget = Cast<UTESUWCommonHeader>(GetWidgetFromName(TEXT("WBP_CommonHeader")));
	TESCHECK(nullptr != _commonHeaderWidget);

	_commonHeaderWidget->InitWidget();
	_commonHeaderWidget->OpenWidget();
	_commonHeaderWidget->SetWidget();
	_commonHeaderWidget->ApplyCenterDisplayersByDefault();
}

void UTESUWCelestialRoadMain::InitSeasonPassDailyRemainingTimeText()
{
	_txt_dailyRemainingTimeList.Empty();

	UTESTextBlock* txt_dailtyReamingTime_on = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_DailyMissionStamp_ON")));
	TESCHECK(nullptr != txt_dailtyReamingTime_on);

	UTESTextBlock* txt_dailtyReamingTime_off = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_DailyMissionStamp_OFF")));
	TESCHECK(nullptr != txt_dailtyReamingTime_off);

	_txt_dailyRemainingTimeList.Add(txt_dailtyReamingTime_off);
	_txt_dailyRemainingTimeList.Add(txt_dailtyReamingTime_on);
}

void UTESUWCelestialRoadMain::InitSeasonPassEndRemainingTimeText()
{
	_txt_seasonEndRemainingTimeList.Empty();

	_txt_seasonEndRemainingTime = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_SeasonEnd")));
	TESCHECK(nullptr != _txt_seasonEndRemainingTime);
	_txt_seasonEndRemainingTime->SetText(FText::FromString(TEXT("")));

	UTESTextBlock* txt_reamingTime_on = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_SeasonMissionStamp_ON")));
	TESCHECK(nullptr != txt_reamingTime_on);

	UTESTextBlock* txt_reamingTime_off = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_SeasonMissionStamp_OFF")));
	TESCHECK(nullptr != txt_reamingTime_off);

	_txt_seasonEndRemainingTimeList.Add(txt_reamingTime_off);
	_txt_seasonEndRemainingTimeList.Add(txt_reamingTime_on);
}

void UTESUWCelestialRoadMain::InitSeasonPassCurrentLevelText()
{
	_txt_currentSeasonPassLevel = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_CurrentLevel")));
	TESCHECK(nullptr != _txt_currentSeasonPassLevel);
	_txt_currentSeasonPassLevel->SetText(FText::FromString(TEXT("1")));
}

void UTESUWCelestialRoadMain::InitSeasonPassFinalRewardInfo()
{
	_txt_finalRewardLevel = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_LevelReward")));
	TESCHECK(nullptr != _txt_finalRewardLevel);
	_txt_finalRewardLevel->SetText(FText::FromString(TEXT("")));

	_img_finalRewardIcon = Cast<UTESImage>(GetWidgetFromName(TEXT("IMG_RewardIcon")));
	TESCHECK(nullptr != _img_finalRewardIcon);
	_img_finalRewardIcon->SetBrushFromTexture(nullptr);
}

void UTESUWCelestialRoadMain::InitSeasonPassEXPProgressBar()
{
	_pb_expProgress = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_ExperienceGauge")));
	TESCHECK(nullptr != _pb_expProgress);
	_pb_expProgress->SetPercent(0.0f);

	_pb_gainExpProgress = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_ExperienceGauge_Gain")));
	TESCHECK(nullptr != _pb_gainExpProgress);
	_pb_gainExpProgress->SetPercent(0.0f);

	_txt_expProgessLevel = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_ExperienceGaugeLevel_Current")));
	TESCHECK(nullptr != _txt_expProgessLevel);

	_txt_totalExpProgessLevel = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_ExperienceGaugeLevel_full")));
	TESCHECK(nullptr != _txt_totalExpProgessLevel);
}

void UTESUWCelestialRoadMain::InitSeasonPassRootPremimumBlock()
{
	_cp_rootPremimumSlot = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Root_BlockCelestialRoadReward")));
	TESCHECK(nullptr != _cp_rootPremimumSlot);
	_cp_rootPremimumSlot->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UTESUWCelestialRoadMain::InitSeasonPassCategoryToggleSwitcher()
{
	int32 maxCount = static_cast<int32>(ETESCelestialRoadType::Max);
	for (int32 index = 1; index < maxCount; ++index)
	{
		ETESCelestialRoadType seasonPassCategory = static_cast<ETESCelestialRoadType>(index);
		FString widgetName = FString::Printf(TEXT("CTS_SeasonPassCategory_%s"), *TESUtilString::GetEnumAsString(seasonPassCategory));

		UTESUWCommonToggleSwitcher* cts_seasonPassCategory = Cast<UTESUWCommonToggleSwitcher>(GetWidgetFromName(*widgetName));
		TESCHECK_CONTINUE(nullptr != cts_seasonPassCategory);
		cts_seasonPassCategory->SetToggleState(false);

		_cts_seasonPassCategories.Add(seasonPassCategory, cts_seasonPassCategory);
	}
}


void UTESUWCelestialRoadMain::InitSeasonPassCategoryNotice()
{
	int32 maxCount = static_cast<int32>(ETESCelestialRoadType::Max);
	for (int32 index = 1; index < maxCount; ++index)
	{
		ETESCelestialRoadType seasonPassCategory = static_cast<ETESCelestialRoadType>(index);
		FString widgetName = FString::Printf(TEXT("IMG_SeasonPassCategory_%s"), *TESUtilString::GetEnumAsString(seasonPassCategory));

		UTESImage* img_seasonPassNotice = Cast<UTESImage>(GetWidgetFromName(*widgetName));
		TESCHECK_CONTINUE(nullptr != img_seasonPassNotice);
		img_seasonPassNotice->SetVisibility(ESlateVisibility::Collapsed);

		_img_seasonPassCategoryNotices.Add(seasonPassCategory, img_seasonPassNotice);
	}
}

void UTESUWCelestialRoadMain::InitSeasonPassContentsSwitcher()
{
	_ws_seasonPassContentsSwitcher = Cast<UTESWidgetSwitcher>(GetWidgetFromName(TEXT("WS_SeasonPassContentsSwitcher")));
	TESCHECK(nullptr != _ws_seasonPassContentsSwitcher);
	_ws_seasonPassContentsSwitcher->SetActiveWidgetIndex(0);
}

void UTESUWCelestialRoadMain::InitSeasonPassMissionCategoryToggleSwitcher()
{
	int32 maxCount = static_cast<int32>(ETESCelestialRoadMissionType::Max);
	for (int32 index = 1; index < maxCount; ++index)
	{
		ETESCelestialRoadMissionType seasonPassMissionCategory = static_cast<ETESCelestialRoadMissionType>(index);
		FString widgetName = FString::Printf(TEXT("CTS_SeasonMissionCategory_%s"), *TESUtilString::GetEnumAsString(seasonPassMissionCategory));

		UTESUWCommonToggleSwitcher* cts_seasonPassMissionCategory = Cast<UTESUWCommonToggleSwitcher>(GetWidgetFromName(*widgetName));
		TESCHECK_CONTINUE(nullptr != cts_seasonPassMissionCategory);
		cts_seasonPassMissionCategory->SetToggleState(false);

		_cts_seasonPassMissionCategories.Add(seasonPassMissionCategory, cts_seasonPassMissionCategory);
	}
}

void UTESUWCelestialRoadMain::InitSeasonPassMissionCategoryNotice()
{
	int32 maxCount = static_cast<int32>(ETESCelestialRoadMissionType::Max);
	for (int32 index = 1; index < maxCount; ++index)
	{
		ETESCelestialRoadMissionType seasonPassMissionCategory = static_cast<ETESCelestialRoadMissionType>(index);
		FString widgetName = FString::Printf(TEXT("IMG_MissionCategoryNotice_%s"), *TESUtilString::GetEnumAsString(seasonPassMissionCategory));

		UTESImage* img_seasonPassMissionNotice = Cast<UTESImage>(GetWidgetFromName(*widgetName));
		TESCHECK_CONTINUE(nullptr != img_seasonPassMissionNotice);
		img_seasonPassMissionNotice->SetVisibility(ESlateVisibility::Collapsed);

		_img_seasonPassMissionCategoryNotices.Add(seasonPassMissionCategory, img_seasonPassMissionNotice);
	}
}

void UTESUWCelestialRoadMain::InitSeasonPassRewardRootCanvas()
{
	_cp_rootReward = GetWidgetFromName(TEXT("Root_Reward"));
	TESCHECK(nullptr != _cp_rootReward);
}

void UTESUWCelestialRoadMain::InitSeasonPassRewardScrollBox()
{
	_scrb_seasonPassRewardList = Cast<UTESScrollBox>(GetWidgetFromName(TEXT("SCRB_RewardList")));
	TESCHECK(nullptr != _scrb_seasonPassRewardList);
	_scrb_seasonPassRewardList->ClearChildren();
}

void UTESUWCelestialRoadMain::InitRootNormalRewardClaimed()
{
	_cp_rootNormalRewardClaimed = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Root_Claimed_NormalReward")));
	TESCHECK(nullptr != _cp_rootNormalRewardClaimed);
	_cp_rootNormalRewardClaimed->SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWCelestialRoadMain::InitRootPremiumRewardClaimed()
{
	for (int32 index = 1; index < 3; ++index)
	{
		FString widgetName = FString::Printf(TEXT("Root_Claimed_PremiumReward_%d"), index);
		UCanvasPanel* cp_rootPremiumReward = Cast<UCanvasPanel>(GetWidgetFromName(*widgetName));
		TESCHECK_CONTINUE(nullptr != cp_rootPremiumReward);
		cp_rootPremiumReward->SetVisibility(ESlateVisibility::Collapsed);

		_cp_rootPremiumRewardListClaimed.Add(cp_rootPremiumReward);
	}
}

void UTESUWCelestialRoadMain::InitSeasonPassRootFixedReward()
{
	_cp_rootFixedSlot = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Root_FixedReward")));
	TESCHECK(nullptr != _cp_rootFixedSlot);
	_cp_rootFixedSlot->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UTESUWCelestialRoadMain::InitSeasonPassFixedReward()
{
	_wbp_normalFixedItemSlot = Cast<UTESUWItemSlot>(GetWidgetFromName(TEXT("WBP_CommonFixedRewardSlot")));
	TESCHECK(nullptr != _wbp_normalFixedItemSlot);
	_wbp_normalFixedItemSlot->InitWidget();
	_wbp_normalFixedItemSlot->CloseWidget();

	for (int32 index = 1; index < 3; ++index)
	{
		FString widgetName = FString::Printf(TEXT("WBP_CelestialRoadFixedRewardSlot_%d"), index);

		UTESUWItemSlot* itemSlotWidget = Cast<UTESUWItemSlot>(GetWidgetFromName(*widgetName));
		TESCHECK_CONTINUE(nullptr != itemSlotWidget);
		itemSlotWidget->InitWidget();
		itemSlotWidget->CloseWidget();

		_preminumFixedItemSlotList.Add(itemSlotWidget);
	}
}

void UTESUWCelestialRoadMain::InitSeasonPassFixedLevel()
{
	_txt_fixedLevel = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_FixedLevel")));
	TESCHECK(nullptr != _txt_fixedLevel);
}

void UTESUWCelestialRoadMain::InitSeasonPassPurchaseLevelUpWidgetSwitcher()
{
	_ws_purchaseLevelUp = Cast<UTESWidgetSwitcher>(GetWidgetFromName(TEXT("TESWS_PurchaseLevel")));
	TESCHECK(nullptr != _ws_purchaseLevelUp);
	_ws_purchaseLevelUp->SetActiveWidgetIndex(0);
}

void UTESUWCelestialRoadMain::InitSeasonPassPurchaseCelestialRoadWidgetSwitcher()
{
	_ws_purchaseCelestialRoad = Cast<UTESWidgetSwitcher>(GetWidgetFromName(TEXT("TESWS_BuyCelestialCompass")));
	TESCHECK(nullptr != _ws_purchaseCelestialRoad);
	_ws_purchaseCelestialRoad->SetActiveWidgetIndex(0);
}

void UTESUWCelestialRoadMain::InitSeasonPassClaimAllRoadWidgetSwitcher()
{
	_ws_claimAll = Cast<UTESWidgetSwitcher>(GetWidgetFromName(TEXT("TESWS_ClaimAll")));
	TESCHECK(nullptr != _ws_claimAll);
	_ws_claimAll->SetActiveWidgetIndex(0);
}

void UTESUWCelestialRoadMain::ClearCommonHeaderWidget()
{
	if (true == ::IsValid(_commonHeaderWidget))
	{
		_commonHeaderWidget->CloseWidget();
		_commonHeaderWidget->ClearWidget();
		_commonHeaderWidget = nullptr;
	}
}

void UTESUWCelestialRoadMain::ClearRewardSlotList()
{
	for (UTESUWCelestialRoadRewardSlot* rewardSlot : _celestialRoadRewardSlotList)
	{
		if(false == ::IsValid(rewardSlot))
			continue;

		SharedUIManager->DestroyCtrWidget(rewardSlot);
		rewardSlot = nullptr;
	}

	_celestialRoadRewardSlotList.Empty();
}

void UTESUWCelestialRoadMain::TickExperienceIncrease(float deltaTime)
{
	if (false == _isLevelingUp)
	{
		return;
	}
	
	if (_experienceCurrentTime > 0.0f)
	{
		_experienceCurrentTime -= deltaTime;

		//. 연출 시간이 끝나면..
		if (_experienceCurrentTime <= 0.0f)
		{
			//. 현재 레벨이 최고레벨에 도달하면
			if (_currentLevel >= _maximumLevel)
			{
				_currentLevel = _maximumLevel;

				//. 레벨 세팅.
				SetSeasonPassCurrentLevel(_currentLevel);

				//. 경험치 게이지 세팅
				SetSeasonPassCurrentEXPProgress(1.0f);

				//. 경험치 텍스트 세팅
				SetSeasonPassCurrentExpText(_needExpTotalByLevel);

				StopWGAnimation(TEXT("Ani_EXP_Gaining"));

				//. 레벨업 팝업 연출.
				PlayLevelUpPopup();

				//. 레벨업 연출 끝.
				_isLevelingUp = false;
			}
			else
			{
				//. 현재 경험치 + 받은 경험치가 필요 경험치보다 크다면 레벨업을 합니다.(이벤트 콜백 함수에서 미리 합쳐짐)
				//. 한번에 업.
				_currentLevel = _currentSeasonPassInfo._currentLevel;

				double currentProgress = static_cast<double>(_totalExp % _needExpTotalByLevel);
				double finalProgress = currentProgress / static_cast<double>(_needExpTotalByLevel);

				_currentExpByLevel = static_cast<int32>(currentProgress);

				//. 레벨 세팅
				SetSeasonPassCurrentLevel(_currentLevel);

				//. 경험치 게이지 세팅
				SetSeasonPassCurrentEXPProgress(finalProgress);

				//. 경험치 텍스트 세팅
				SetSeasonPassCurrentExpText(_currentExpByLevel);

				//. 레벨업 연출 끝.
				_isLevelingUp = false;

				//. 레벨업 팝업 연출.
				PlayLevelUpPopup();

				RefreshAll();
			}
		}
		else
		{
			//. 레벨업 하면 초기화
			if (_currentExpByLevel >= _needExpTotalByLevel)
			{
				//. 한번에 업.
				_currentLevel = _currentSeasonPassInfo._currentLevel;

				_currentExpByLevel = 0;

				//. 레벨 세팅
				SetSeasonPassCurrentLevel(_currentLevel);

				//. 경험치 게이지 세팅
				SetSeasonPassCurrentEXPProgress(0.0f);

				//. 경험치 텍스트 세팅
				SetSeasonPassCurrentExpText(0);
			}
			else
			{
				double animTimePerTick = 0.0;
				double incrementPerTick = 0.0;

				if (_gainedExp <= _needExpTotalByLevel)
				{
					//. 획득한 exp가 1레벨보다 작으면 연출 시간을 있는 현재 경험치와 상관없이 계산을 해준다.
					animTimePerTick = _experienceTotalTime / deltaTime;
					incrementPerTick = _gainedExp / animTimePerTick;
					if (1 >= incrementPerTick)
					{
						incrementPerTick = 1;
					}
				}
				else
				{
					//. 획득한 exp가 240경험치를 넘어가게 되면 속도가 빨라지므로 목표치를 조정해준다.
					//. 레벨이 오르고 나서 경험치를 얼마나 채워야 하는지에 따라 속도가 달라지므로 계산을 해줘야 한다.
					if (0 == _gainedExp % _needExpTotalByLevel)
					{
						//. 딱 나누어 떨어지면 1렙이던 10렙이던 올라가는 속도는 같다.
						animTimePerTick = _experienceTotalTime / deltaTime;
						incrementPerTick = _gainedExp / animTimePerTick;
						if (1 >= incrementPerTick)
						{
							incrementPerTick = 1;
						}
					}
					else if (1 <= _gainedExp / _needExpTotalByLevel)
					{
						//. 1레벨 이상 오르면 실제로 오른 양만큼을 거리로 계산해야 한다.
						//. 1레벨에 필요한 총량 경험치 - 현재 경험치 보유량 = 현재 레벨업에 필요한 경험치
						//. 얻은 경험치 - 현재 레벨업에 필요한 경험치 > 1레벨에 필요한 총량 경험치 일 경우에는 2렙이상 오르는 경우.
						//. => (_gainedExp % _needExpTotalByLevel) + 현재 레벨업에 필요한 경험치 = 레벨 오르고 난 후 최종 목적지의 경험치량.
						//. 레벨 오르고 난 후 최종 목적지의 경험치량 + 현재 레벨업에 필요한 경험치 = 올라가는 속도.
						if (_gainedExp - _remainExpInCurrentLevel > _needExpTotalByLevel)
						{
							//. 2레벨 이상이 오르는 경우.
							//. 최종 목적지는 (_gainedExp % _needExpTotalByLevel) + 현재 레벨업에 필요한 경험치;
							int32 destinationExp = (_gainedExp % _needExpTotalByLevel) + _remainExpInCurrentLevel;
							animTimePerTick = _experienceTotalTime / deltaTime;
							incrementPerTick = destinationExp / animTimePerTick;
						}
						else
						{
							//. 1레벨만 오르는 경우.
							animTimePerTick = _experienceTotalTime / deltaTime;
							incrementPerTick = _gainedExp / animTimePerTick;
						}
					}
				}

				if (0 >= incrementPerTick)
				{
					incrementPerTick = 0;
				}

				//. 경험치 증가
				_currentExpByLevel += incrementPerTick;

				if (_currentLevel == _currentSeasonPassInfo._currentLevel)
				{
					//. 현재 레벨이 레벨업 한 상태에서 
					//. 틱당 획득하는 경험치를 더했더니 최대로 획득 할 수 있는 경험치보다 넘었다면?
					float currentProgress = _totalExp % _needExpTotalByLevel;
					if (_currentExpByLevel >= currentProgress)
					{
						_currentExpByLevel = static_cast<int32>(currentProgress);
						_gainedExp = 0;	//. 끝내준다.
					}
				}

				//. 현재 레벨의 경험치 출력
				SetSeasonPassCurrentExpText(_currentExpByLevel);

				//. 게이지 출력
				SetSeasonPassCurrentEXPProgress(static_cast<double>(_currentExpByLevel) / static_cast<double>(_needExpTotalByLevel));
			}
		}
	}
}

void UTESUWCelestialRoadMain::SetWidget(ETESCelestialRoadType categoryType)
{
	if (false == SetData())
	{
		CloseWidget();
		return;
	}
	
	//. 현재 레벨이 해당 패스의 최대레벨보다 작으면 true
	SetSeasonPassPurchaseLevelUpWidgetSwitcher(_currentSeasonPassInfo._currentLevel < _maximumLevel);

	//. 이미 구매 했는지에 따라 on/off
	SetSeasonPassPurchaseCelestialRoadWidgetSwitcher(_currentSeasonPassInfo._isPurchased);

	RefreshExpWidgets();

	//. 시즌패스 보상 혹은 미션 세팅.
	SetSeasonPassType(categoryType);		

	SetSeaonPassFinalRewardInfo(_currentSeasonPassInfo._celestialRoadTableId);

	//. 빨간점 표기.(보상 및 미션 둘다)
	SetSeasonPassCategoryNotice();
}

bool UTESUWCelestialRoadMain::SetData()
{
	_currentSeasonPassInfo = SharedCelestialRoadManger->GetSeasonPassInfo();

	_totalExp = SharedCelestialRoadManger->GetAccumulateExp();													//. 갖고 있는 총 exp

	_needExpTotalByLevel = TESUtilTable::GetConstantValueByInt(EConstantIntName::CelestialRoad_LvUP_EXP);		//. 각 레벨에 필요한 레벨업 exp

	_currentExpByLevel = _totalExp % _needExpTotalByLevel;														//. 현재 레벨에서 갖고 있는 exp

	_exposureLevelList = SharedCelestialRoadManger->GetSeasonPassExposureLevelList();

	_currentLevel = _currentSeasonPassInfo._currentLevel;

	_beforeLevel = _currentLevel;

	_maximumLevel = SharedCelestialRoadManger->GetMaximumRewardLevel(_currentSeasonPassInfo._celestialRoadTableId);

	return true;
}

void UTESUWCelestialRoadMain::SetDailyMissionResetTime(int64 timeStamp)
{
	if (ETESCelestialRoadType::Mission == _currentCategoryType)
	{
		bool isActive = _cts_seasonPassMissionCategories[ETESCelestialRoadMissionType::DailyMission]->IsActiveState();
		int32 index = static_cast<int32>(isActive);
		if (true == _txt_dailyRemainingTimeList.IsValidIndex(index))
		{
			if (true == ::IsValid(_txt_dailyRemainingTimeList[index]))
			{
				double remainTicks = SharedTimeManager->GetRemainingTimeLeadingUpToTimeStamp(timeStamp);

				if (0 >= remainTicks)
				{
					remainTicks = 0;
				}

				int64 seasonMissionTimeStamp = SharedCelestialRoadManger->GetSeasonPassMissionTimeStamp();

				if (timeStamp < seasonMissionTimeStamp)
				{
					FTimespan timeSpan = timeSpan.FromSeconds(remainTicks);

					/*TESLOG(Display, TEXT("TimeStamp : %lld, RemainTicks : %lf =====> Hours : %d, Minutes : %d, Seconds : %d")
								  , timeStamp, remainTicks, timeSpan.GetHours(), timeSpan.GetMinutes(), timeSpan.GetSeconds());*/

					FString formatString = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::UniqueAbyss_ChallengeTime);
					FString resetTimeString = TESUtilString::MakeFormatString(formatString, timeSpan.GetHours(), timeSpan.GetMinutes());
					_txt_dailyRemainingTimeList[index]->SetText(FText::FromString(resetTimeString));
				}
				else
				{
					double remainSeasonMissionTime = SharedTimeManager->GetRemainingTimeLeadingUpToTimeStamp(seasonMissionTimeStamp);

					if (0 >= remainSeasonMissionTime)
					{
						remainSeasonMissionTime = 0;
					}

					FTimespan timeSpan = timeSpan.FromSeconds(remainSeasonMissionTime);
					FString blankSpace = TEXT(" ");
					FString formatStringDay = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_Time_Day);
					FString formatStringHour = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_Time_Hour_2);
					FString formatStringMin = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_Time_Minute);
					FString resetDayString = TESUtilString::MakeFormatString(formatStringDay, timeSpan.GetDays());
					FString resetHourString = TESUtilString::MakeFormatString(formatStringHour, timeSpan.GetHours());
					FString resetMinString = TESUtilString::MakeFormatString(formatStringMin, timeSpan.GetMinutes());
					FString finalStringForDay = resetDayString + blankSpace + resetHourString + blankSpace + resetMinString;
					FString finalStringForHour = resetHourString + blankSpace + resetMinString;
					FString finalStringForMin = resetMinString;

					if (0 < timeSpan.GetDays())
					{
						_txt_dailyRemainingTimeList[index]->SetText(FText::FromString(finalStringForDay));
					}
					else
					{
						if (0 < timeSpan.GetHours())
						{
							_txt_dailyRemainingTimeList[index]->SetText(FText::FromString(finalStringForHour));
						}
						else
						{
							_txt_dailyRemainingTimeList[index]->SetText(FText::FromString(finalStringForMin));
						}
					}
				}
			}
		}
	}
}

void UTESUWCelestialRoadMain::SetSeasonPassResetTime(int64 timeStamp)
{
	double remainTicks = SharedTimeManager->GetRemainingTimeLeadingUpToTimeStamp(timeStamp);
	if (0 >= remainTicks)
	{
		remainTicks = 0;
	}

	FTimespan timeSpan = timeSpan.FromSeconds(remainTicks);

	//. 현재 남은 시간에 따라 표기가 바뀌어야 한다.
	//. 1일 이상 : n일 n시간 n분
	//. 1일 미만 : n시간 n분
	//. 1시간 미만 : n분
	FString blankSpace = TEXT(" ");
	FString newLine = TEXT("\n");
	FString formatStringDay = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_Time_Day);
	FString formatStringHour = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_Time_Hour_2);
	FString formatStringMin = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_Time_Minute);
	FString resetDayString = TESUtilString::MakeFormatString(formatStringDay, timeSpan.GetDays());
	FString resetHourString = TESUtilString::MakeFormatString(formatStringHour, timeSpan.GetHours());
	FString resetMinString = TESUtilString::MakeFormatString(formatStringMin, timeSpan.GetMinutes());
	FString finalStringForDay = resetDayString + newLine + resetHourString + blankSpace + resetMinString;
	FString finalStringForHour = resetHourString + blankSpace + resetMinString;
	FString finalStringForMin = resetMinString;

	if (ETESCelestialRoadType::Mission == _currentCategoryType)
	{
		bool isActive = _cts_seasonPassMissionCategories[ETESCelestialRoadMissionType::SeasonMission]->IsActiveState();
		int32 index = static_cast<int32>(isActive);
		if (true == _txt_seasonEndRemainingTimeList.IsValidIndex(index))
		{
			if (true == ::IsValid(_txt_seasonEndRemainingTimeList[index]))
			{
				if (0 < timeSpan.GetDays())
				{
					_txt_seasonEndRemainingTimeList[index]->SetText(FText::FromString(finalStringForDay));
				}
				else
				{
					if (0 < timeSpan.GetHours())
					{
						_txt_seasonEndRemainingTimeList[index]->SetText(FText::FromString(finalStringForHour));
					}
					else
					{
						_txt_seasonEndRemainingTimeList[index]->SetText(FText::FromString(finalStringForMin));
					}
				}
			}
		}
	}

	if (true == ::IsValid(_txt_seasonEndRemainingTime))
	{
		if (0 < timeSpan.GetDays())
		{
			FString remainingTimeForDay = resetDayString + blankSpace + resetHourString + blankSpace + resetMinString;
			_txt_seasonEndRemainingTime->SetText(FText::FromString(remainingTimeForDay));
		}
		else
		{
			if (0 < timeSpan.GetHours())
			{
				_txt_seasonEndRemainingTime->SetText(FText::FromString(finalStringForHour));
			}
			else
			{
				_txt_seasonEndRemainingTime->SetText(FText::FromString(finalStringForMin));
			}
		}
	}
}

void UTESUWCelestialRoadMain::SetSeasonPassCurrentLevel(int32 level)
{
	TESCHECK(nullptr != _txt_currentSeasonPassLevel);
	
	_txt_currentSeasonPassLevel->SetText(FText::FromString(TESUtilString::MakeFormatString(TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Navigation_ContentsSubName_UniqueAbyss_Number), level)));
}

void UTESUWCelestialRoadMain::SetSeasonPassCurrentExpText(int32 currentExp)
{
	TESCHECK(nullptr != _txt_expProgessLevel);
	_txt_expProgessLevel->SetText(FText::FromString(FString::FromInt(currentExp)));
}

void UTESUWCelestialRoadMain::SetSeasonPassCurrentEXPProgress(float value)
{
	TESCHECK(nullptr != _pb_expProgress);
	_pb_expProgress->SetPercent(value);
}

void UTESUWCelestialRoadMain::SetSeasonPassTotalExpText()
{
	TESCHECK(nullptr != _txt_totalExpProgessLevel);
	_txt_totalExpProgessLevel->SetText(FText::FromString(FString::FromInt(_needExpTotalByLevel)));
}

void UTESUWCelestialRoadMain::SetSeaonPassFinalRewardInfo(int32 celestialRoadTableId)
{
	const FCelestialRoadRow& celestialRoadRow = SharedTDBManager->GetCelestialRoadRow(celestialRoadTableId);
	if (true == celestialRoadRow.IsEmpty())
		return;

	int32 finalRewardLevel = celestialRoadRow.KeyItemRewardLevel;
	FString finalRewardIconPath = TEXT("");
	_finalRewardItemId = celestialRoadRow.KeyItemId;

	UTESGDGoodsBase* finalRewardItemBase = TESUtils::CreateGoodsData(ETypeGoods::Item, 0, _finalRewardItemId);
	if (true == ::IsValid(finalRewardItemBase))
	{
		UTESGDGoodsItem* itemGoodsData = Cast<UTESGDGoodsItem>(finalRewardItemBase);
		if (true == ::IsValid(itemGoodsData))
		{
			finalRewardIconPath = itemGoodsData->GetItemBigIconPath();
		}
	}

	TESCHECK(nullptr != _txt_finalRewardLevel);
	TESCHECK(nullptr != _img_finalRewardIcon);

	_txt_finalRewardLevel->SetText(FText::FromString(TESUtilString::MakeFormatString(TEXT("Lv. {0}"), finalRewardLevel)));
	TESUtilTexture::SetDynamicTexture(_img_finalRewardIcon, finalRewardIconPath);
}

void UTESUWCelestialRoadMain::SetSeasonPassCategoryNotice()
{
	//. 둘다 체크한다.
	SetSeasonPassCategoryNotice(ETESCelestialRoadType::Reward);

	SetSeasonPassCategoryNotice(ETESCelestialRoadType::Mission);
}

void UTESUWCelestialRoadMain::SetSeasonPassCategoryNotice(ETESCelestialRoadType celestialRoadType)
{
	//. 각각 체크하는 로직이 달라서 분기를 태운다.
	switch (celestialRoadType)
	{
	case ETESCelestialRoadType::Reward:
		{
			//. 보상이 하나라도 안받은게 있으면 빨간점 On
			bool isClaimed = SharedCelestialRoadManger->GetAllRewardIsClaimed();

			if (true == _img_seasonPassCategoryNotices.Contains(celestialRoadType))
			{
				if (true == ::IsValid(_img_seasonPassCategoryNotices[celestialRoadType]))
				{
					_img_seasonPassCategoryNotices[celestialRoadType]->SetVisibility(true == isClaimed ? ESlateVisibility::Collapsed: ESlateVisibility::SelfHitTestInvisible);
				}
			}
		}
		break;
	case ETESCelestialRoadType::Mission:
		{
			bool isReceivable = false;

			if (true == SharedMissionManager->HasAnyMissionRewardsToReceive(ETypeMission::SeasonPassDailyMission))
			{
				isReceivable = true;
			}

			if (true == SharedMissionManager->HasAnyMissionRewardsToReceive(ETypeMission::SeasonPassAchievement))
			{
				isReceivable = true;
			}
			
			/*if (true == ::IsValid(_currentMissionContentsWidget))
			{
				if (true == _currentMissionContentsWidget->HasAnyMissionRewardsToReceive((SharedMissionManager->GetMissionDatas(ETypeMission::SeasonPassAchievement))))
				{
					isReceivable = true;
				}
			}*/

			//if (true == ::IsValid(_currentMissionContentsWidget))
			//{
			//	//. 시즌패스 일일임무.
			//	if (true == _currentMissionContentsWidget->HasAnyMissionRewardsToReceive((SharedMissionManager->GetMissionDatas(ETypeMission::SeasonPassDailyMission))))
			//	{
			//		isReceivable = true;
			//	}

			//	if (true == _currentMissionContentsWidget->HasAnyMissionRewardsToReceive((SharedMissionManager->GetMissionDatas(ETypeMission::SeasonPassAchievement))))
			//	{
			//		isReceivable = true;
			//	}
			//}

			if (true == _img_seasonPassCategoryNotices.Contains(celestialRoadType))
			{
				if (true == ::IsValid(_img_seasonPassCategoryNotices[celestialRoadType]))
				{
					_img_seasonPassCategoryNotices[celestialRoadType]->SetVisibility(true == isReceivable ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
				}
			}
		}
		break;
	}
}

void UTESUWCelestialRoadMain::SetSeasonPassPremimumBlock(bool isPurchased)
{
	//. 천체의 나침반을 구매하였으면 collapsed. 아니면 HitTest로 설정한다.
	TESCHECK(nullptr != _cp_rootPremimumSlot);
	_cp_rootPremimumSlot->SetVisibility(true == isPurchased ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible);
}

void UTESUWCelestialRoadMain::SetSeasonPassType(ETESCelestialRoadType categoryType)
{
	if (categoryType == _currentCategoryType)
	{
		TESLOG(Display, TEXT("Is Same CategoryType : %d"), categoryType);

		return;
	}

	_previousCategoryType = _currentCategoryType;

	_currentCategoryType = categoryType;

	//. 카테고리 탭.
	SetSeasonPassCategories(_previousCategoryType, false);

	SetSeasonPassCategories(_currentCategoryType, true);

	//. 해당 카테고리 내용.
	SetSeasonPassContentsSwitcher(_currentCategoryType);

	switch (_currentCategoryType)
	{
	case ETESCelestialRoadType::Reward:
		{
			SetSeasonPassRewardList();
		}
		break;
	case ETESCelestialRoadType::Mission:
		{
			SetSeasonPassMissionType(ETESCelestialRoadMissionType::DailyMission);
		}
		break;
	default:
		{
			TESLOG(Error, TEXT("Wrong CelestialRoad Type"));
		}
		break;
	}
}

void UTESUWCelestialRoadMain::SetSeasonPassCategories(ETESCelestialRoadType categoryType, bool isOn)
{
	if (ETESCelestialRoadType::None == categoryType)
	{
		return;
	}

	if (false == _cts_seasonPassCategories.Contains(categoryType))
	{
		TESLOG(Error, TEXT("Not Found CategoryType : %d"), categoryType);

		return;
	}

	if (false == ::IsValid(_cts_seasonPassCategories[categoryType]))
	{
		TESLOG(Error, TEXT("Invalid _cts_seasonPassCategories"));

		return;
	}

	_cts_seasonPassCategories[categoryType]->SetToggleState(isOn);
}

void UTESUWCelestialRoadMain::SetSeasonPassContentsSwitcher(ETESCelestialRoadType categoryType)
{
	if (true == ::IsValid(_ws_seasonPassContentsSwitcher))
	{
		int32 index = static_cast<int32>(categoryType) - 1;
		_ws_seasonPassContentsSwitcher->SetActiveWidgetIndex(index);
	}
}

void UTESUWCelestialRoadMain::SetSeasonPassRewardList()
{
	//. 보상 내용물.
	_sb_dummyBox = nullptr;
	_sb_dummyBox = NewObject<USizeBox>(this);
	TESCHECK(nullptr != _sb_dummyBox);
	_sb_dummyBox->SetWidthOverride(184.0f);

	if (true == ::IsValid(_scrb_seasonPassRewardList))
	{
		ClearRewardSlotList();

		_scrb_seasonPassRewardList->ClearChildren();

		//. 먼저 사이즈 박스를 insert 해준다.
		_scrb_seasonPassRewardList->AddChild(_sb_dummyBox);

		TArray<int32> seasonPassRewardTableIdList = SharedCelestialRoadManger->GetSeasonPassRewardTableIdListBySeasonPassId(_currentSeasonPassInfo._celestialRoadTableId);
		bool isRewardClaimedAll = true;

		for (int32 index = 0; index < seasonPassRewardTableIdList.Num(); ++index)
		{
			UTESUWCelestialRoadRewardSlot* rewardSlotWidget = SharedUIManager->CreateCtrWidget<UTESUWCelestialRoadRewardSlot>(TEXT("WBP_CelestialRoadRewardSlot"), TEXT("CelestialRoad/WBP"));
			if (true == ::IsValid(rewardSlotWidget))
			{
				rewardSlotWidget->InitWidget();
				rewardSlotWidget->OpenWidget();
				rewardSlotWidget->SetWidget(seasonPassRewardTableIdList[index]);


				if (false == rewardSlotWidget->isRewardClaimedAll())
				{
					//. 모든 보상중에 하나라도 안받은게 있으면 false 처리.(빨간점)
					isRewardClaimedAll = false;
				}

				_celestialRoadRewardSlotList.Add(rewardSlotWidget);
				_scrb_seasonPassRewardList->AddChild(rewardSlotWidget);
			}
		}

		SharedCelestialRoadManger->SetIsRewardClaimedAll(isRewardClaimedAll);
	}

	FTESCelestialRoadData seasonPassData = SharedCelestialRoadManger->GetSeasonPassInfo();
	
	SetSeasonPassPremimumBlock(seasonPassData._isPurchased);

	//. 전부 받았으면 비활성화 처리. 한개라도 받을게 있으면 활성화 처리.
	SetSeasonPassClaimAllRoadWidgetSwitcher(SharedCelestialRoadManger->GetAllRewardIsClaimed());
	
	//. 현재 레벨에 맞게 스크롤 해준다.
	SetRewardScroll(_currentSeasonPassInfo._currentLevel);

	//. 
	SetSeasonPassFixedReward(GetRewardTableIdByCurrentPositionInScroll(), true);
}

void UTESUWCelestialRoadMain::SetSeasonPassMissionType(ETESCelestialRoadMissionType missionCategoryType)
{
	if (missionCategoryType == _currentMissionCategoryType)
	{
		TESLOG(Display, TEXT("Is Same CategoryType : %d"), missionCategoryType);

		//. 수령 버튼만 갱신.
		if (true == ::IsValid(_currentMissionContentsWidget))
		{
			bool isReceivable = _currentMissionContentsWidget->HasAnyMissionRewardsToReceive(SharedMissionManager->GetMissionDatas(GetMissionTypeByMissionCategoryType(missionCategoryType)));
			//. 받을만한게 있다 == 수령을 다 못했다. 
			SetSeasonPassClaimAllRoadWidgetSwitcher(!isReceivable);
		}

		return;
	}

	_previousMissionCategoryType = _currentMissionCategoryType;

	_currentMissionCategoryType = missionCategoryType;

	//. 카테고리 탭.
	SetSeasonPassMissionCategories(_previousMissionCategoryType, false);

	SetSeasonPassMissionCategories(_currentMissionCategoryType, true);

	//. 해당 카테고리 내용.(미션인 경우에는 일일미션이거나 시즌미션)
	SetSeasonPassMissionContents(_currentMissionCategoryType);

	//. 빨간점.(일일 및 시즌임무 둘다)
	SetSeasonPassMissionCategoryNotice();
}

void UTESUWCelestialRoadMain::SetSeasonPassMissionCategories(ETESCelestialRoadMissionType missionCategoryType, bool isOn)
{
	if (ETESCelestialRoadMissionType::None == missionCategoryType)
	{
		return;
	}

	if (false == _cts_seasonPassMissionCategories.Contains(missionCategoryType))
	{
		TESLOG(Error, TEXT("Not Found CategoryType : %d"), missionCategoryType);

		return;
	}

	if (false == ::IsValid(_cts_seasonPassMissionCategories[missionCategoryType]))
	{
		TESLOG(Error, TEXT("Invalid _cts_seasonPassMissionCategories"));

		return;
	}

	_cts_seasonPassMissionCategories[missionCategoryType]->SetToggleState(isOn);
}

void UTESUWCelestialRoadMain::SetSeasonPassMissionContents(ETESCelestialRoadMissionType missionCategoryType)
{
	SetSeasonPassMissionContainer(missionCategoryType);
}

void UTESUWCelestialRoadMain::SetSeasonPassMissionContainer(ETESCelestialRoadMissionType missionCategoryType)
{
	//. 이전 컨텐츠 위젯이 존재하는 경우, 위젯을 닫아주고 해제를 합니다.
	if (true == ::IsValid(_currentMissionContentsWidget))
	{
		_currentMissionContentsWidget->CloseWidget();
		_currentMissionContentsWidget->ClearWidget();
		_currentMissionContentsWidget = nullptr;
	}
	
	//. 새로운 컨텐츠 위젯을 가져옵니다.
	_currentMissionContentsWidget = Cast<UTESUWCelestialRoadMission>(GetWidgetFromName(TEXT("WBP_CelestialRoadMission")));
	if (false == ::IsValid(_currentMissionContentsWidget))
	{
		TESLOG(Error, TEXT("Invalid ContentsWidget. CategoryType : %s")
			, *TESUtilString::GetEnumAsString<ETESCelestialRoadMissionType>(TEXT("ETESCelestialRoadMissionType"), missionCategoryType));

		return;
	}

	//. 일일 혹은 시즌 임무 카테고리에 따라 가져온다.
	if (false == ::IsValid(_currentMissionContentsWidget))
	{
		TESLOG(Error, TEXT("Invalid DailyMissionWidget."));

		return;
	}

	_currentMissionContentsWidget->InitWidget();
	_currentMissionContentsWidget->OpenWidget();
	_currentMissionContentsWidget->SetWidget(SharedMissionManager->GetMissionDatas(GetMissionTypeByMissionCategoryType(missionCategoryType)));

	bool isReceivable = _currentMissionContentsWidget->HasAnyMissionRewardsToReceive(SharedMissionManager->GetMissionDatas(GetMissionTypeByMissionCategoryType(missionCategoryType)));
	//. 받을만한게 있다 == 수령을 다 못했다. 
	SetSeasonPassClaimAllRoadWidgetSwitcher(!isReceivable);
}

void UTESUWCelestialRoadMain::SetSeasonPassMissionCategoryNotice()
{
	//. 둘다 체크해준다.
	SetSeasonPassMissionCategoryNotice(ETESCelestialRoadMissionType::DailyMission);

	SetSeasonPassMissionCategoryNotice(ETESCelestialRoadMissionType::SeasonMission);
}

void UTESUWCelestialRoadMain::SetSeasonPassMissionCategoryNotice(ETESCelestialRoadMissionType missionCategoryType)
{
	ETypeMission missionType = GetMissionTypeByMissionCategoryType(missionCategoryType);
	bool receivable = _currentMissionContentsWidget->HasAnyMissionRewardsToReceive(SharedMissionManager->GetMissionDatas(missionType));

	if (true == _img_seasonPassMissionCategoryNotices.Contains(missionCategoryType))
	{
		if (true == ::IsValid(_img_seasonPassMissionCategoryNotices[missionCategoryType]))
		{
			_img_seasonPassMissionCategoryNotices[missionCategoryType]->SetVisibility(true == receivable ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
		}
	}
}

void UTESUWCelestialRoadMain::SetSeasonPassFixedReward(int32 rewardTableId, bool forceRefresh/* = false*/)
{
	if (0 >= rewardTableId)
		return;

	if (false == forceRefresh)
	{
		if (_fixedRewardTableId == rewardTableId)
			return;
	}

	_fixedRewardTableId = rewardTableId;

	SetNormalRewardSlot(rewardTableId);

	SetPremiumRewardSlot(rewardTableId);

	SetPremiumSlotLevel(rewardTableId);
}

void UTESUWCelestialRoadMain::SetNormalRewardSlot(int32 rewardTableId)
{
	const FCelestialRoadRewardRow& rewardRow = SharedTDBManager->GetCelestialRoadRewardRow(rewardTableId);
	if (true == rewardRow.IsEmpty())
		return;

	TArray<FTESGoodsItem> goodItemList = rewardRow.NormalItemId;

	if (false == goodItemList.IsValidIndex(0))
		return;

	TESCHECK(nullptr != _wbp_normalFixedItemSlot);
	FTESItemSlotData itemSlotdata = {};

	UTESGDGoodsBase* goodsData = TESUtils::CreateGoodsData(goodItemList[0]._goodsType, goodItemList[0]._count, goodItemList[0]._itemId);
	if (true == ::IsValid(goodsData))
	{
		itemSlotdata._goodsDataBase = goodsData;
		itemSlotdata._isVisibleByItemCount = true;
		itemSlotdata._isVisibleByStatus = false;
		itemSlotdata._isVisibleByNFT = false;
		itemSlotdata._isDisabled = false;
		itemSlotdata._isSelected = false;
		itemSlotdata._isSelectable = false;
		itemSlotdata._isUseToolTip = true;
		itemSlotdata._isUseButtonEvent = true;
		itemSlotdata._isUseNotify = false;

		_wbp_normalFixedItemSlot->OpenWidget();
		_wbp_normalFixedItemSlot->SetWidget(itemSlotdata);
	}
}

void UTESUWCelestialRoadMain::SetPremiumRewardSlot(int32 rewardTableId)
{
	const FCelestialRoadRewardRow& rewardRow = SharedTDBManager->GetCelestialRoadRewardRow(rewardTableId);
	if (true == rewardRow.IsEmpty())
		return;

	TArray<FTESGoodsItem> goodItemList = rewardRow.PremiumItemId;

	for (int32 index = 0; index < goodItemList.Num(); ++index)
	{
		FTESItemSlotData itemSlotdata = {};

		UTESGDGoodsBase* goodsData = TESUtils::CreateGoodsData(goodItemList[index]._goodsType, goodItemList[index]._count, goodItemList[index]._itemId);
		if (true == ::IsValid(goodsData))
		{
			itemSlotdata._goodsDataBase = goodsData;
			itemSlotdata._isVisibleByItemCount = true;
			itemSlotdata._isVisibleByStatus = false;
			itemSlotdata._isVisibleByNFT = false;
			itemSlotdata._isDisabled = false;
			itemSlotdata._isSelected = false;
			itemSlotdata._isSelectable = false;
			itemSlotdata._isUseToolTip = true;
			itemSlotdata._isUseButtonEvent = true;
			itemSlotdata._isUseNotify = false;

			if (true == _preminumFixedItemSlotList.IsValidIndex(index))
			{
				if (true == ::IsValid(_preminumFixedItemSlotList[index]))
				{
					_preminumFixedItemSlotList[index]->OpenWidget();
					_preminumFixedItemSlotList[index]->SetWidget(itemSlotdata);
				}
			}
		}
	}
}

void UTESUWCelestialRoadMain::SetPremiumSlotLevel(int32 rewardTableId)
{
	const FCelestialRoadRewardRow& rewardRow = SharedTDBManager->GetCelestialRoadRewardRow(rewardTableId);
	if (true == rewardRow.IsEmpty())
		return;

	int32 level = rewardRow.CelestialRoadLv;
	TESCHECK(nullptr != _txt_fixedLevel);
	_txt_fixedLevel->SetText(FText::FromString(FString::FromInt(level)));

	SetNormalRewardClaimed(level);

	SetPremiumRewardClaimed(level);
}

void UTESUWCelestialRoadMain::SetNormalRewardClaimed(int32 level)
{
	FTESCelestialRoadData seasonPassData = SharedCelestialRoadManger->GetSeasonPassInfo();
	
	if (seasonPassData._lastRewardLevel >= level)
	{
		//. 받았다.
		_cp_rootNormalRewardClaimed->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		_cp_rootNormalRewardClaimed->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UTESUWCelestialRoadMain::SetPremiumRewardClaimed(int32 level)
{
	FTESCelestialRoadData seasonPassData = SharedCelestialRoadManger->GetSeasonPassInfo();
	if (false == seasonPassData._isPurchased)
		return;

	for (int32 index = 0; index < _cp_rootPremiumRewardListClaimed.Num(); ++index)
	{
		if (seasonPassData._lastCompassRewardLevel >= level)
		{
			//. 받았다.
			_cp_rootPremiumRewardListClaimed[index]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			_cp_rootPremiumRewardListClaimed[index]->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UTESUWCelestialRoadMain::SetSeasonPassPurchaseLevelUpWidgetSwitcher(bool isOn)
{
	TESCHECK(nullptr != _ws_purchaseLevelUp);
	_ws_purchaseLevelUp->SetActiveWidgetIndex(true == isOn ? 0 : 1);
}

void UTESUWCelestialRoadMain::SetSeasonPassPurchaseCelestialRoadWidgetSwitcher(bool isPurchased)
{
	TESCHECK(nullptr != _ws_purchaseCelestialRoad);
	if (true == isPurchased)
	{
		//. 구매 했을때는 비활성화 처리
		_ws_purchaseCelestialRoad->SetActiveWidgetIndex(1);
	}
	else
	{
		SetSeasonPassPurchaseCelestialRoadWidgetSwitcherByEndOfPurchaseableTime();
	}
}

void UTESUWCelestialRoadMain::SetSeasonPassPurchaseCelestialRoadWidgetSwitcherByEndOfPurchaseableTime()
{
	//. 나침반 판매 기간이 종료 했는지 아닌지에 따라 버튼 활성화 / 비활성화 처리.
	double remainingTime = SharedTimeManager->GetRemainingTimeLeadingUpToTimeStamp(SharedCelestialRoadManger->GetSeasonPassPurchaseableCompassTimeStamp());
	if (0 >= remainingTime)
	{
		//. 구매 하지 않았지만, 구매 가능한 시간이 지났으면 비활성화 처리를 해야한다.
		_ws_purchaseCelestialRoad->SetActiveWidgetIndex(1);
	}
	else
	{
		//. 활성화.
		if (true == _currentSeasonPassInfo._isPurchased)
		{
			//. 구매 했을때는 비활성화 처리
			_ws_purchaseCelestialRoad->SetActiveWidgetIndex(1);
		}
		else
		{
			_ws_purchaseCelestialRoad->SetActiveWidgetIndex(0);
		}

	}
}

void UTESUWCelestialRoadMain::SetSeasonPassClaimAllRoadWidgetSwitcher(bool isClaimedAll)
{
	TESCHECK(nullptr != _ws_claimAll);
	_ws_claimAll->SetActiveWidgetIndex(true == isClaimedAll ? 1 : 0);
}

void UTESUWCelestialRoadMain::SetRewardScroll(int32 currentLevel)
{
	//. 현재 레벨에 맞는 스크롤 위치 판단하기.
	TESCHECK(nullptr != _scrb_seasonPassRewardList);
	_scrb_seasonPassRewardList->ForceLayoutPrepass();
	FVector2D totalsize = _scrb_seasonPassRewardList->GetDesiredSize();
	FVector2D slotSize = FVector2D::ZeroVector;
	if (true == ::IsValid(_scrb_seasonPassRewardList->GetChildAt(1)))
	{
		slotSize = _scrb_seasonPassRewardList->GetChildAt(1)->GetDesiredSize();
	}
	//TESLOG(Error, TEXT("total size : %f"), totalsize.X);

	int32 finalLevel = currentLevel - 5;			//. 기획팀에서 처음 위치를 잡을때 현재 레벨을 맨 왼쪽이 아니라 현재 레벨보다 3~5정도 낮은 슬롯을 보여달라고 하셨음.
	float finalOffset = slotSize.X * finalLevel;
	_scrb_seasonPassRewardList->SetScrollOffset(finalOffset);
}

ETypeMission UTESUWCelestialRoadMain::GetMissionTypeByMissionCategoryType(ETESCelestialRoadMissionType missionCategoryType)
{
	switch (missionCategoryType)
	{
	case ETESCelestialRoadMissionType::DailyMission:
		{
			return ETypeMission::SeasonPassDailyMission;
		}
	case ETESCelestialRoadMissionType::SeasonMission:
		{
			return ETypeMission::SeasonPassAchievement;
		}
	default:
		{
			return ETypeMission::None;
		}
	}
}

int32 UTESUWCelestialRoadMain::GetRewardTableIdByCurrentPositionInScroll()
{
	//. 조건에 따라서 fix reward를 보여준다.
	//. 현재 어디레벨까지 스크롤을 처리하고, 그에 맞는 레벨에 fixed reward를 보여줘야한다.
	if (true == ::IsValid(_scrb_seasonPassRewardList))
	{
		//. 스크롤박스의 크기.
		FVector2D totalsize = _scrb_seasonPassRewardList->GetDesiredSize();
		float gap = 0.0f;	//. 해상도에 따라 달라지는 크기의 갭.

		float endScroll = _scrb_seasonPassRewardList->GetScrollOffsetOfEnd();	//. 현재 해상도에 따른 스크롤 박스의 크기.

		float scrollOffset = _scrb_seasonPassRewardList->GetScrollOffset();		//. 스크롤박스의 좌측 offset 
		//TESLOG(Error, TEXT("Left scrollOffset : %f"), scrollOffset);
		gap = totalsize.X - endScroll;

		FVector2D slotSize = FVector2D::ZeroVector;
		if (true == ::IsValid(_cp_rootFixedSlot))
		{
			//. 우측 고정슬롯의 크기(해상도에 따라 바뀜)
			slotSize = _cp_rootFixedSlot->GetCachedGeometry().GetLocalSize();
			//TESLOG(Error, TEXT(" Local slotSize : %f"), slotSize.X);
		}

		float rightScrollOffset = gap - slotSize.X;
		rightScrollOffset += scrollOffset;			//. 우측 기준점.
		
		//TESLOG(Error, TEXT("Right scrollOffset : %f"), rightScrollOffset);

		int32 sectionLevel = CheckCurrentLevelInCurrentScrollOffset(rightScrollOffset);

		//TESLOG(Error, TEXT("sectionLevel : %d"), sectionLevel);
		
		return SharedCelestialRoadManger->GetSeasonPassRewardTableIdByLevel(sectionLevel);
	}
	
	return 0;
}

int32 UTESUWCelestialRoadMain::CheckCurrentLevelInCurrentScrollOffset(float rightScrollOffset)
{
	//. 10, 20, 30, 40, 50.
	//. expose 레벨에 따라 스크롤 오프셋을 구하고,(절대값을 구해야한다.)
	//. 위 함수를 쓸때 Array를 순회하면서 체크를 하고 트루가 뜨면 해당 레벨을 바로 리턴.

	bool isInRange = false;		//. 현재 스크롤이 어디에 있는지 확인하는 불값.(ex. 1~10레벨 사이)
	float rewardSlotLocalSize = 0.0f;
	//. 보상 슬롯 각 하나당 크기(해상도에 따라 바뀜)(가로 길이만)
	if (true == ::IsValid(_scrb_seasonPassRewardList))
	{
		if (true == ::IsValid(_scrb_seasonPassRewardList->GetChildAt(_currentSeasonPassInfo._currentLevel)))
		{
			rewardSlotLocalSize = _scrb_seasonPassRewardList->GetChildAt(_currentSeasonPassInfo._currentLevel)->GetCachedGeometry().GetLocalSize().X;
		}
	}

	float startPoint = 0.0f;			//. 체크 지점 start
	float lengthOfExposure = 0.0f;		//. 체크 지점 max
	for (int32 index = 0; index < _exposureLevelList.Num(); ++index)
	{
		startPoint = lengthOfExposure;
		int32 exposureLevel = _exposureLevelList[index];														//. 첫번째 슬롯에는 sizeBox가 들어가서 + 1을 해줘야 한다.
		lengthOfExposure = rewardSlotLocalSize * exposureLevel - (rewardSlotLocalSize / 2);						//. 사이즈 x 숫자 = 해당 레벨까지의 길이 - 한슬롯의 크기의 반.(체크 지점)

		isInRange = FMath::IsWithinInclusive(rightScrollOffset, startPoint, lengthOfExposure);
		if (true == isInRange)
		{
			return _exposureLevelList[index];
		}
	}

	//. 만렙일때는 만렙으로 고정 시켜준다.

	if (_maximumLevel <= _currentSeasonPassInfo._currentLevel)
	{
		return _maximumLevel;
	}
	
	return 0;
}

void UTESUWCelestialRoadMain::PlayUnlockPremiumRewardsAnim()
{
	_isPurchasingCompass = true;

	_currentSeasonPassInfo = SharedCelestialRoadManger->GetSeasonPassInfo();

	OnEvent_ChangeSeasonPassCategory(ETESCelestialRoadType::Reward);

	TESCHECK(nullptr != _cp_rootPremimumSlot);
	_cp_rootPremimumSlot->SetVisibility(ESlateVisibility::Collapsed);
	PlayWGAnimation(TEXT("Ani_PremiumRewardUnlocked"));

	float finishTime = PlayWGAnimation(TEXT("Ani_PremiumRewardUnlocked"));
	GetWorld()->GetTimerManager().ClearTimer(_th_playAnim);
	GetWorld()->GetTimerManager().SetTimer(_th_playAnim, FTimerDelegate::CreateLambda([=]()
		{
			//. 보상 받을수 있게 빨간점 표시 갱신해준다.
			_isPurchasingCompass = false;
			SharedCelestialRoadManger->SetIsRewardClaimedAll(false);
			RefreshAll();
		}), finishTime, false);
}

void UTESUWCelestialRoadMain::PlayExpGainingAnim()
{
	_experienceCurrentTime = PlayWGAnimation(TEXT("Ani_EXP_Gaining"));

	_experienceTotalTime = 1.0f;		//. 연출시간은 1초로 정하기로 약속.

	_isLevelingUp = true;				//. 틱 레벨업 연출 시작.

	PlayExpGainingSound();
}

void UTESUWCelestialRoadMain::PlayExpGainingSound()
{
	if (false == _isLevelingUp)
		return;

	GetWorld()->GetTimerManager().SetTimer(_th_updateExpSound
		, FTimerDelegate::CreateUObject(this, &UTESUWCelestialRoadMain::PlayExpGainingSound)
		, 0.3f, true);

	int32 soundId = TESUtilTable::GetConstantValueByInt(EConstantIntName::UISE_ScoreBarUp);
	const FSoundRow& soundTable = SharedTDBManager->GetSoundRow(soundId);
	if (true == soundTable.IsEmpty())
		return;

	SharedSoundManager->Play2DSoundAtOnce(GetWorld(), soundTable.SoundPath);
}

void UTESUWCelestialRoadMain::PlayLevelUpPopup()
{
	GetWorld()->GetTimerManager().ClearTimer(_th_updateExpSound);

	//. 레벨업을 할때만 팝업을 켜준다.
	if (_currentLevel > _beforeLevel)
	{
		//. 레벨업 위젯을 출력하기 위한 데이터를 구성합니다.
		FTESCommonLevelUpInitData levelUpInitData = {};
		levelUpInitData._levelUpType = ETESCommonLevelUpType::Normal;
		levelUpInitData._beforeLevelValue = _beforeLevel;
		levelUpInitData._afterLevelValue = _currentLevel;
		levelUpInitData._onFinishAction = FOnTESAction::CreateLambda([=]
			{
				//. 해당 위젯을 새로운 정보로 모두 갱신합니다.
				RefreshAll();

				_beforeLevel = _currentLevel;
			});

		//. 구성된 정보로 레벨 연출을 출력합니다.
		UTESUWCommonLevelUp* commonLevelUpWidget = SharedPopupManager->CreatePopupWidget<UTESUWCommonLevelUp>(TEXT("WBP_CelestialRoadLevelUp"), TEXT("CelestialRoad/WBP"));
		TESCHECK(nullptr != commonLevelUpWidget);
		commonLevelUpWidget->InitWidget();
		commonLevelUpWidget->OpenWidget();
		commonLevelUpWidget->SetWidget(levelUpInitData);
	}
	//. 단순히 경험치만 올랐으면 갱신만..
	else
	{
		//. 해당 위젯을 새로운 정보로 모두 갱신합니다.
		RefreshAll();
	}
}

void UTESUWCelestialRoadMain::RefreshAll()
{
	switch (_currentCategoryType)
	{
	case ETESCelestialRoadType::Reward:
		{
			SetSeasonPassRewardList();
			
		}
		break;
	case ETESCelestialRoadType::Mission:
		{
			//. 해당 카테고리 내용.(미션인 경우에는 일일미션이거나 시즌미션)
			SetSeasonPassMissionContents(_currentMissionCategoryType);

			//. 빨간점.(일일 및 시즌임무 둘다)
			SetSeasonPassMissionCategoryNotice();
		}
		break;
	default:
		{
			TESLOG(Error, TEXT("Wrong CelestialRoad Type"));
		}
		break;
	}

	//. 빨간점.(둘다 체크)
	SetSeasonPassCategoryNotice();
	
	RefreshExpWidgets();

	SetSeasonPassPurchaseCelestialRoadWidgetSwitcher(_currentSeasonPassInfo._isPurchased);

	SetSeasonPassPurchaseLevelUpWidgetSwitcher(_currentSeasonPassInfo._currentLevel < _maximumLevel);
}

void UTESUWCelestialRoadMain::RefreshExpWidgets()
{
	SetSeasonPassTotalExpText();

	if (_maximumLevel <= _currentLevel)
	{
		//. 한번 더 갱신해준다.
		SetSeasonPassCurrentLevel(_maximumLevel);

		SetSeasonPassCurrentExpText(_needExpTotalByLevel);

		SetSeasonPassCurrentEXPProgress(1.0f);
	}
	else
	{
		float currentProgress = _totalExp % _needExpTotalByLevel;
		float finalProgress = currentProgress / static_cast<float>(_needExpTotalByLevel);

		SetSeasonPassCurrentLevel(_currentLevel);

		SetSeasonPassCurrentExpText(_totalExp % _needExpTotalByLevel);

		SetSeasonPassCurrentEXPProgress(finalProgress);
	}
}

void UTESUWCelestialRoadMain::ShowExpiredMessage()
{
	if (true == _isChangeContents)
		return;

	//. 시즌패스가 끝난 상태에서 이것저것 행동 했을때 메세지 띄우고 로비로 보낸다.
	SharedPopupManager->ShowMessageBox(TESUtilTable::GetGlobalString(GetWorld(), EGStringID::CelestialRoad_SeasonEnd_Message1));

	_isChangeContents = true;

	GetWorld()->GetTimerManager().ClearTimer(_th_closeAnim);
	GetWorld()->GetTimerManager().SetTimer(_th_closeAnim, FTimerDelegate::CreateLambda([=]()
		{
			_isChangeContents = false;
			OnEvent_Back();
		}), 2.0f, false);
}

void UTESUWCelestialRoadMain::OnEvent_ChangeSeasonPassCategory(ETESCelestialRoadType categoryType)
{
	//. 시즌패스가 만료되면 로비로 보낸다.
	if (true == SharedCelestialRoadManger->CheckIfSeasonNeedToBeRenew())
	{
		ShowExpiredMessage();
		return;
	}

	SetSeasonPassType(categoryType);
}

void UTESUWCelestialRoadMain::OnEvent_ChangeMissionCategory(ETESCelestialRoadMissionType missionCategoryType)
{
	//. 시즌패스가 만료되면 로비로 보낸다.
	if (true == SharedCelestialRoadManger->CheckIfSeasonNeedToBeRenew())
	{
		ShowExpiredMessage();
		return;
	}

	SetSeasonPassMissionType(missionCategoryType);
}

void UTESUWCelestialRoadMain::OnEvent_PurchaseLevelEXP()
{
	//. 시즌패스가 만료되면 로비로 보낸다.
	if (true == SharedCelestialRoadManger->CheckIfSeasonNeedToBeRenew())
	{
		ShowExpiredMessage();

		return;
	}

	if (true == _isPurchasingCompass)
		return;
	
	//. 공용 팝업 띄워준다.(구매)
	//. 최대레벨까지 구매 가능하므로 (최대 - 현재) 레벨 만큼까지만 허용해야한다.
	const FCelestialRoadRow& seasonPassRow = SharedTDBManager->GetCelestialRoadRow(_currentSeasonPassInfo._celestialRoadTableId);
	if (true == seasonPassRow.IsEmpty())
	{
		return;
	}

	int32 purchaseableCount = 1;

	FTESPurchaseBoxParameters purchaseBoxParameters = {};
	purchaseBoxParameters._purchaseType = ETESCommonPurchaseBoxType::GeneralPurchase;
	purchaseBoxParameters._fromItemId = seasonPassRow.CelestialRoadLvSale._itemId;						//. 소모 아이템
	purchaseBoxParameters._toItemId = seasonPassRow.CelestialRoadLvItemId;								//. 구매 아이템
	purchaseBoxParameters._quantityConsumedPerPiece = seasonPassRow.CelestialRoadLvSale._count;			//. 한개당 사는데 필요한 재화. (Ex. 에스트 5000개)
	purchaseBoxParameters._limitedPurchaseableCurrentCount = 50 - SharedCelestialRoadManger->GetCurrentSeasonPassLevel();
	purchaseBoxParameters._limitedPurchaseableMaximumCount = SharedCelestialRoadManger->GetMaximumRewardLevel(seasonPassRow.Id);
	purchaseBoxParameters._onPurchaseAction = TDelegate<void(int32)>::CreateLambda([=](int32 purchaseCount)
		{
			if (true == SharedCelestialRoadManger->CheckIfSeasonNeedToBeRenew())
			{
				ShowExpiredMessage();

				return;
			}

			SharedNetworkManager->CLGW_QRY_CELESTIAL_PASS_PURCHASE_LEVEL(purchaseCount);
		});

	SharedPopupManager->ShowPurchaseBox(purchaseBoxParameters);
}

void UTESUWCelestialRoadMain::OnEvent_CannotPurchaseLevelEXPAnymore()
{
	//. 시즌패스가 만료되면 로비로 보낸다.
	if (true == SharedCelestialRoadManger->CheckIfSeasonNeedToBeRenew())
	{
		ShowExpiredMessage();
		return;
	}

	SharedPopupManager->ShowMessageBox(TESUtilTable::GetGlobalString(GetWorld(), EGStringID::CelestialRoad_LvMax_Message));
}

void UTESUWCelestialRoadMain::OnEvent_PurchasePremiumSeasonPsss()
{
	//. 시즌패스가 만료되면 로비로 보낸다.
	if (true == SharedCelestialRoadManger->CheckIfSeasonNeedToBeRenew())
	{
		ShowExpiredMessage();
		return;
	}

	//. 공용 팝업 띄워준다.(구매)
	//. 아이템 아이디는 테이블에서 불러온다.
	const FCelestialRoadRow& seasonPassRow = SharedTDBManager->GetCelestialRoadRow(_currentSeasonPassInfo._celestialRoadTableId);
	if (true == seasonPassRow.IsEmpty())
	{
		return;
	}

	int32 purchaseableCount = 0;
	if (false == _currentSeasonPassInfo._isPurchased)
	{
		purchaseableCount = 1;
	}
	else
	{
		SharedPopupManager->ShowMessageBox(TESUtilTable::GetGlobalString(GetWorld(), EGStringID::CelestialRoad_CelestialCompass_Message1));
		return;
	}

	FTESPurchaseBoxParameters purchaseBoxParameters = {};
	purchaseBoxParameters._purchaseType = ETESCommonPurchaseBoxType::GeneralPurchase;
	purchaseBoxParameters._fromItemId = seasonPassRow.CelestialCompassSale._itemId;						//. 소모 아이템
	purchaseBoxParameters._toItemId = seasonPassRow.CelestialCompassItemId;								//. 구매 아이템
	purchaseBoxParameters._quantityConsumedPerPiece = seasonPassRow.CelestialCompassSale._count;		//. 한개당 사는데 필요한 재화. (Ex. 에스트 5000개)
	purchaseBoxParameters._limitedPurchaseableMaximumCount = 1;
	purchaseBoxParameters._limitedPurchaseableCurrentCount = 1;
	purchaseBoxParameters._onPurchaseAction = TDelegate<void(int32)>::CreateLambda([=](int32 purchaseCount)
	{
		//. 시즌패스가 만료되면 로비로 보낸다.
		if (true == SharedCelestialRoadManger->CheckIfSeasonNeedToBeRenew())
		{
			ShowExpiredMessage();
			return;
		}

		double remainingTime = SharedTimeManager->GetRemainingTimeLeadingUpToTimeStamp(SharedCelestialRoadManger->GetSeasonPassPurchaseableCompassTimeStamp());
		if (0 >= remainingTime)
		{
			//. 구매 기간이 지났으면 기간이 지났다는 메세지.
			SharedPopupManager->ShowMessageBox(TESUtilTable::GetGlobalString(GetWorld(), EGStringID::CelestialRoad_CelestialCompass_Message2));
			return;
		}

		SharedNetworkManager->CLGW_QRY_CELESTIAL_PASS_PURCHASE_COMPASS();
	});

	SharedPopupManager->ShowPurchaseBox(purchaseBoxParameters);
}

void UTESUWCelestialRoadMain::OnEvent_AlreadyPurchasedPremiumSeasonPsss()
{
	//. 시즌패스가 만료되면 로비로 보낸다.
	if (true == SharedCelestialRoadManger->CheckIfSeasonNeedToBeRenew())
	{
		ShowExpiredMessage();
		return;
	}

	if (true == _isPurchasingCompass)
		return;

	double remainingTime = SharedTimeManager->GetRemainingTimeLeadingUpToTimeStamp(SharedCelestialRoadManger->GetSeasonPassPurchaseableCompassTimeStamp());
	if (0 >= remainingTime)
	{
		//. 구매 기간이 지났으면 기간이 지났다는 메세지.
		SharedPopupManager->ShowMessageBox(TESUtilTable::GetGlobalString(GetWorld(), EGStringID::CelestialRoad_CelestialCompass_Message2));
	}
	else
	{
		//. 이미 샀으면 구매 했다는 센터 메세지
		SharedPopupManager->ShowMessageBox(TESUtilTable::GetGlobalString(GetWorld(), EGStringID::CelestialRoad_CelestialCompass_Message1));
	}
}

void UTESUWCelestialRoadMain::OnEvent_ClaimAll()
{
	//. 시즌패스가 만료되면 로비로 보낸다.
	if (true == SharedCelestialRoadManger->CheckIfSeasonNeedToBeRenew())
	{
		ShowExpiredMessage();
		return;
	}

	if (true == _isPurchasingCompass)
		return;

	switch (_currentCategoryType)
	{
	case ETESCelestialRoadType::Reward:
		{
			//. 보상 한번에 받기
			if (true == _currentSeasonPassInfo._isPurchased)
			{
				if (_currentSeasonPassInfo._currentLevel > _currentSeasonPassInfo._lastCompassRewardLevel)
				{
					SharedNetworkManager->CLGW_QRY_CELESTIAL_PASS_REWARD();
					return;
				}
			}

			if (_currentSeasonPassInfo._currentLevel > _currentSeasonPassInfo._lastRewardLevel)
			{
				SharedNetworkManager->CLGW_QRY_CELESTIAL_PASS_REWARD();
			}
		}
		break;
	case ETESCelestialRoadType::Mission:
		{
			//. 완료된 미션들 한번에 받기.
			if (false == ::IsValid(_currentMissionContentsWidget))
			{
				return;
			}

			TArray<UTESGDMission*> missionDataList = SharedMissionManager->GetMissionDatas(GetMissionTypeByMissionCategoryType(_currentMissionCategoryType));
			if (false == _currentMissionContentsWidget->HasAnyMissionRewardsToReceive(missionDataList))
			{
				return;
			}

			//. 일일임무인지 시즌임무인지 분기로 나눠야함.
			switch (_currentMissionCategoryType)
			{
			case ETESCelestialRoadMissionType::DailyMission:
				{
					SharedNetworkManager->CLGW_QRY_CELESTIAL_PASS_MISSION_ALL_REWARD(ETypeMission::SeasonPassDailyMission);
				}
				break;
			case ETESCelestialRoadMissionType::SeasonMission:
				{
					SharedNetworkManager->CLGW_QRY_CELESTIAL_PASS_MISSION_ALL_REWARD(ETypeMission::SeasonPassAchievement);
				}
				break;
			default:
				{
					TESLOG(Error, TEXT("_currentMissionCategoryType type is invalid"));
					return;
				}
				break;
			}
		}
		break;
	default:
		{
			TESLOG(Error, TEXT("WrongType"));
		}
		break;
	}
}

void UTESUWCelestialRoadMain::OnEvent_AlreadyClaimAll()
{
	//. 시즌패스가 만료되면 로비로 보낸다.
	if (true == SharedCelestialRoadManger->CheckIfSeasonNeedToBeRenew())
	{
		ShowExpiredMessage();
		return;
	}

	FString finalString = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::CelestialRoad_ReceiveAll_Message);
	SharedPopupManager->ShowMessageBox(finalString);
}

void UTESUWCelestialRoadMain::OnEvent_MissionUpdated()
{
	//. 미션 목록이 업데이트되는 이벤트를  받으면, 미션 카테고리 컨텐츠들이 목록을 업데이트 할 수 있도록 그 이벤트를 전달해줍니다.
	switch (_currentMissionCategoryType)
	{
	case ETESCelestialRoadMissionType::DailyMission:
		{
			UTESUWCelestialRoadMission* missionContentsWigdget = Cast<UTESUWCelestialRoadMission>(_currentMissionContentsWidget);
			if (true == ::IsValid(missionContentsWigdget))
			{
				missionContentsWigdget->OnEvent_MissionUpdated(SharedMissionManager->GetMissionDatas(GetMissionTypeByMissionCategoryType(_currentMissionCategoryType)));
			}
		}
		break;

	case ETESCelestialRoadMissionType::SeasonMission:
		{
			UTESUWCelestialRoadMission* missionContentsWigdget = Cast<UTESUWCelestialRoadMission>(_currentMissionContentsWidget);
			if (true == ::IsValid(missionContentsWigdget))
			{
				missionContentsWigdget->OnEvent_MissionUpdated(SharedMissionManager->GetMissionDatas(GetMissionTypeByMissionCategoryType(_currentMissionCategoryType)));
			}
		}
		break;
	}
}

void UTESUWCelestialRoadMain::OnEvent_Close()
{
	if (true == _isChangeContents)
	{
		return;
	}

	SharedSceneManager->OnEvent_ContentsBack();
}

void UTESUWCelestialRoadMain::OnEvent_PurchaseLevelup()
{
	//. 레벨업 연출을 해준다.
	_currentSeasonPassInfo = SharedCelestialRoadManger->GetSeasonPassInfo();

	if (_currentSeasonPassInfo._currentLevel >= _maximumLevel)
	{
		_currentSeasonPassInfo._currentLevel = _maximumLevel;
	}

	_totalExp = _currentSeasonPassInfo._accumulateExp;

	_gainedExp = _needExpTotalByLevel;

	_remainExpInCurrentLevel = _needExpTotalByLevel - _currentExpByLevel;

	SharedCelestialRoadManger->SetIsRewardClaimedAll(false);

	PlayExpGainingAnim();
}

void UTESUWCelestialRoadMain::OnEvent_GainExp(int32 gainedExp)
{
	//. 이미 만렙이면 아무런 연출을 하지 않는다.
	if (_currentLevel >= _maximumLevel)
		return;

	TESLOG(Error, TEXT("gained exp : %d"), gainedExp);

	//. 얻은 exp 연출을 진행한다.
	//. 레벨이 올랐을때는 레벨업 연출도 진행한다.
	if (gainedExp >= _needExpTotalByLevel || gainedExp + _currentExpByLevel >= _needExpTotalByLevel)
	{
		//. 레벨업 조건이 만족하면 레벨업 연출은 해준다.
		//. 레벨업을 했으므로 보상 받아야할게 생겼다.
		SharedCelestialRoadManger->SetIsRewardClaimedAll(false);
	}
	
	_totalExp += gainedExp;

	_gainedExp = gainedExp;

	_remainExpInCurrentLevel = _needExpTotalByLevel - _currentExpByLevel;

	_currentSeasonPassInfo = SharedCelestialRoadManger->GetSeasonPassInfo();

	if (_currentSeasonPassInfo._currentLevel >= _maximumLevel)
	{
		_currentSeasonPassInfo._currentLevel = _maximumLevel;
	}

	PlayExpGainingAnim();
}

void UTESUWCelestialRoadMain::OnEvent_ShowFixedItemInfo()
{
	if (true == _isPurchasingCompass)
		return;

	FTESInfomationPopupInitData popupInitData = {};
	popupInitData._informationPopupType = EInfomationPopupType::Item;
	popupInitData._infomationDataId = _finalRewardItemId;

	SharedPopupManager->ShowInfomationPopup(popupInitData);
}

void UTESUWCelestialRoadMain::OnEvent_Home()
{
	SharedSceneManager->OnEvent_ContentsHome();
}

void UTESUWCelestialRoadMain::OnEvent_Back()
{
	OnEvent_Close();
}
