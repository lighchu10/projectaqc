// Fill out your copyright notice in the Description page of Project Settings.

#include "Contents/CelestialRoad/Manager/TESCelestialRoadManager.h"
#include "Contents/Mission/GameData/TESGDMission.h"
#include "Contents/Common/Manager/TESTimeManager.h"
#include "Contents/Common/Manager/TESPopupManager.h"
#include "Contents/Common/Manager/TESSceneManager.h"
#include "Contents/CelestialRoad/UI/TESUWCelestialRoadSeasonNotice.h"
#include "Network/TESNetworkManager.h"

void UTESCelestialRoadManager::Start()
{
	Super::Start();

	SetEnableTick(true);

	SetupRewardListPerLevelData();

	_needToRenewSeason = false;

	Reset();
}

void UTESCelestialRoadManager::Reset()
{
	Super::Reset();

	_needToRenewSeason = false;
}

void UTESCelestialRoadManager::Shutdown()
{
	SetEnableTick(false);

	Reset();

	ClearRewardListPerLevelData();
}

void UTESCelestialRoadManager::Tick(const float& deltaTime)
{
	Super::Tick(deltaTime);

	//. 현재 화면이 로비가 아니라면 무시합니다.
	if (ETESPhysicalGameSceneType::Lobby != SharedSceneManager->GetCurrentPhysicalGameSceneType())
	{
		return;
	}
	
	//. 현재 시즌이 만료되면 다음 시즌 갱신전까지는 틱을 안돌린다.
	if (true == _needToRenewSeason)
	{
		return;
	}

	if (_seasonPassMissionTimeStamp > 0)
	{
		double remainTime = SharedTimeManager->GetRemainingTimeLeadingUpToTimeStamp(_seasonPassMissionTimeStamp);
		if (remainTime <= 0.0l)
		{
			_needToRenewSeason = true;
		}
	}
}

void UTESCelestialRoadManager::ClearRewardListPerLevelData()
{
	_seasonPassRewardTableIdListBySeasonId.Empty();
}

void UTESCelestialRoadManager::SetupRewardListPerLevelData()
{
	_seasonPassRewardTableIdListBySeasonId.Empty();

	TArray<const FCelestialRoadRewardRow*> seasonPassRewardRows = SharedTDBManager->GetAllRowListCelestialRoadReward();
	for (const FCelestialRoadRewardRow* rewardRow : seasonPassRewardRows)
	{
		int32 seasonPassId = rewardRow->CelestialRoadId;
		int32 rewardTableId = rewardRow->Id;

		if (true == _seasonPassRewardTableIdListBySeasonId.Contains(seasonPassId))
		{
			_seasonPassRewardTableIdListBySeasonId[seasonPassId]._seasonPassRewardIdList.Add(rewardTableId);
		}
		else
		{
			FTESCelestialRoadRewardPerLevelData rewardPerLevelData = {};
			rewardPerLevelData._seasonPassId = seasonPassId;
			rewardPerLevelData._seasonPassRewardIdList.Add(rewardTableId);

			_seasonPassRewardTableIdListBySeasonId.Add(seasonPassId, rewardPerLevelData);
		}
	}
}

void UTESCelestialRoadManager::SetSeasonPassInfo(FTESCelestialPassInfo seasonPassInfo)
{
	if (0 >= seasonPassInfo._index)
	{
		_needToRenewSeason = true;
		_seasonPassData._celestialRoadTableId = 0;
		return;
	}

	_seasonPassData._celestialRoadTableId = seasonPassInfo._index;
	_seasonPassData._accumulateExp = seasonPassInfo._exp;
	_seasonPassData._currentLevel = seasonPassInfo._level;
	_seasonPassData._lastCompassRewardLevel = seasonPassInfo._lastCompassRewardLevel;
	_seasonPassData._lastRewardLevel = seasonPassInfo._lastRewardLevel;
	_seasonPassData._isPurchased = seasonPassInfo._isPurchase;

	//. 디폴트 레벨이 1이라 서버에서 바꿔줘야 한다.
	/*if (0 == seasonPassInfo._level)
	{
		_seasonPassData._currentLevel = 0;
	}*/

	//. 여기서 시즌 시간도 설정해준다.
	SetSeasonPassEndTimeStamp(_seasonPassData._celestialRoadTableId);

	SetSeasonPassPurchaseableCompassTimeStamp(_seasonPassData._celestialRoadTableId);

	//. 갱신 완료.
	_needToRenewSeason = false;
}

void UTESCelestialRoadManager::SetSeasonPassInfo(FTESCelestialRoadData seasonPassInfo)
{
	_seasonPassData = seasonPassInfo;
}

void UTESCelestialRoadManager::AddSeasonPassAccumulateExp(int32 exp)
{
	_seasonPassData._accumulateExp += exp;

	int32 needExp = TESUtilTable::GetConstantValueByInt(EConstantIntName::CelestialRoad_LvUP_EXP);

	//. 레벨업을 하면 갱신해준다.
	//. 나눠서 나온 몫만큼 레벨업 시켜준다.
	int32 currentLevel = _seasonPassData._accumulateExp / needExp;
	if (0 >= currentLevel)
		return;

	_seasonPassData._currentLevel = currentLevel;
}

void UTESCelestialRoadManager::SetSeasonPassLevel(int32 seasonPassLevel)
{
	_seasonPassData._currentLevel = seasonPassLevel;
}

void UTESCelestialRoadManager::SetIsRewardClaimedAll(bool isClaimedAll)
{
	_isAllRewardClaimed = isClaimedAll;
}

void UTESCelestialRoadManager::SetSeasonPassEndTimeStamp(int32 celestialRoadTableId)
{
	const FCelestialRoadRow& seasonPassRow = SharedTDBManager->GetCelestialRoadRow(celestialRoadTableId);
	if (true == seasonPassRow.IsEmpty())
		return;

	_seasonPassMissionTimeStamp = ConvertDateTimeDetailToTimeStamp(seasonPassRow.CelestialRoadEndTime, TESConstantValue::KoreanTimeBias);

	//. 갱신
	_needToRenewSeason = false;
}

void UTESCelestialRoadManager::SetSeasonPassPurchaseableCompassTimeStamp(int32 celestialRoadTableId)
{
	const FCelestialRoadRow& seasonPassRow = SharedTDBManager->GetCelestialRoadRow(celestialRoadTableId);
	if (true == seasonPassRow.IsEmpty())
		return;

	_seasonPassPurchaseableCompassTimeStamp = ConvertDateTimeDetailToTimeStamp(seasonPassRow.CelestialCompassSaleEndTime, TESConstantValue::KoreanTimeBias);
}

const int64& UTESCelestialRoadManager::GetSeasonPassMissionTimeStamp() const
{
	return _seasonPassMissionTimeStamp;
}

const int64& UTESCelestialRoadManager::GetSeasonPassPurchaseableCompassTimeStamp() const
{
	return _seasonPassPurchaseableCompassTimeStamp;
}

int32 UTESCelestialRoadManager::GetCurrentSeasonPassId() const
{
	return _seasonPassData._celestialRoadTableId;
}

int32 UTESCelestialRoadManager::GetCurrentSeasonPassLevel() const
{
	return _seasonPassData._currentLevel;
}

int32 UTESCelestialRoadManager::GetSeasonPassRewardTableIdByLevel(int32 seasonpassLevel)
{
	TArray<const FCelestialRoadRewardRow*> rewardRows = SharedTDBManager->GetAllRowListCelestialRoadReward();
	for (const FCelestialRoadRewardRow* seasonPassRewardRow : rewardRows)
	{
		if (_seasonPassData._celestialRoadTableId == seasonPassRewardRow->CelestialRoadId)
		{
			int32 celestialRoadLevel = seasonPassRewardRow->CelestialRoadLv;
			if (seasonpassLevel == celestialRoadLevel)
			{
				return seasonPassRewardRow->Id;
			}
		}
	}

	return 0;
}

int32 UTESCelestialRoadManager::GetAccumulateExp() const
{
	return _seasonPassData._accumulateExp;
}

int32 UTESCelestialRoadManager::GetMaximumRewardLevel(int32 currentSeasonPassId) const
{
	int32 maxiumLevel = 0;

	if (true == _seasonPassRewardTableIdListBySeasonId.Contains(currentSeasonPassId))
	{
		TArray<int32> rewardIdList = _seasonPassRewardTableIdListBySeasonId[currentSeasonPassId]._seasonPassRewardIdList;

		for (int32 index = 0; index < rewardIdList.Num(); ++index)
		{
			const FCelestialRoadRewardRow& rewardRow = SharedTDBManager->GetCelestialRoadRewardRow(rewardIdList[index]);
			if(true == rewardRow.IsEmpty())
				continue;

			if (maxiumLevel < rewardRow.CelestialRoadLv)
			{
				maxiumLevel = rewardRow.CelestialRoadLv;
			}
		}

	}

	return maxiumLevel;
}

bool UTESCelestialRoadManager::GetAllRewardIsClaimed() const
{
	//. 전부 시즌패스 보상을 전부 수령했는지?
	return _isAllRewardClaimed;
}

bool UTESCelestialRoadManager::CheckAnyRewardsCanClaim()
{
	if (true == _needToRenewSeason)
		return false;

	if (_seasonPassData._currentLevel > _seasonPassData._lastRewardLevel)
	{
		if (GetMaximumRewardLevel(_seasonPassData._celestialRoadTableId) < _seasonPassData._currentLevel)
		{
			return false;
		}

		return true;
	}

	if (true == _seasonPassData._isPurchased)
	{
		if (_seasonPassData._currentLevel > _seasonPassData._lastCompassRewardLevel)
		{
			return true;
		}
	}
	else
	{
		return false;
	}
	
	return false;
	
}

bool UTESCelestialRoadManager::CheckIfSeasonNeedToBeRenew() const
{
	return _needToRenewSeason;
}

TArray<int32> UTESCelestialRoadManager::GetSeasonPassExposureLevelList() const
{
	const FCelestialRoadRow& seasonPassRow = SharedTDBManager->GetCelestialRoadRow(_seasonPassData._celestialRoadTableId);
	if (true == seasonPassRow.IsEmpty())
	{
		return {};
	}

	return seasonPassRow.FixedExposureLevel;
}

FTESCelestialRoadData UTESCelestialRoadManager::GetSeasonPassInfo() const
{
	return _seasonPassData;
}

TArray<int32> UTESCelestialRoadManager::GetSeasonPassRewardTableIdListBySeasonPassId(int32 currentSeasonPassId) const
{
	if (true == _seasonPassRewardTableIdListBySeasonId.Contains(currentSeasonPassId))
	{
		return _seasonPassRewardTableIdListBySeasonId[currentSeasonPassId]._seasonPassRewardIdList;
	}

	return {};
}

bool UTESCelestialRoadManager::OpenCelestialRoadNoticePopup(TDelegate<void()> onNextAction)
{
	//. 팝업을 띄워준다.
	if (true == CheckIfSeasonNeedToBeRenew())
	{
		return false;
	}

	UTESUWCelestialRoadSeasonNotice* noticePopup = SharedPopupManager->CreatePopupWidget<UTESUWCelestialRoadSeasonNotice>(TEXT("WBP_CelestialNewSeasonNotice"), TEXT("CelestialRoad/WBP"));
	if (true == ::IsValid(noticePopup))
	{
		noticePopup->InitWidget();
		noticePopup->OpenWidget();
		noticePopup->SetWidget(GetCurrentSeasonPassId());
		noticePopup->SetNextActionCallback(onNextAction);
	}

	return true;
}

FTESDateTime UTESCelestialRoadManager::ConvertTimeStampToDateTimeDetail(int64 timeStamp, int32 bias)
{
	FDateTime dateTime = FDateTime::FromUnixTimestamp(timeStamp - (bias * 3600));
	FTESDateTime dateTimeDetail;
	dateTimeDetail._year = dateTime.GetYear();
	dateTimeDetail._month = dateTime.GetMonth();
	dateTimeDetail._date = dateTime.GetDay();
	dateTimeDetail._hour = dateTime.GetHour();
	dateTimeDetail._min = dateTime.GetMinute();
	dateTimeDetail._sec = dateTime.GetSecond();

	return dateTimeDetail;
}

int64 UTESCelestialRoadManager::ConvertDateTimeDetailToTimeStamp(FTESDateTime dateTime, int32 bias)
{
	FString timeString = FString::Printf(TEXT("%d %d %d %d %d %d"), dateTime._year, dateTime._month, dateTime._date, dateTime._hour, dateTime._min, dateTime._sec);
	FDateTime outDateTime;
	FDateTime::Parse(timeString, outDateTime);
	int64 timeStamp = outDateTime.ToUnixTimestamp() - (bias * 3600);

	return timeStamp;
}
