// Fill out your copyright notice in the Description page of Project Settings.

#include "Contents/MirageGarden/Manager/TESMirageGardenManager.h"
#include "Contents/MirageGarden/UI/TESUWMirageGardenFloorStart.h"
#include "Contents/Common/Manager/TESTimeManager.h"
#include "Contents/Common/Manager/TESPopupManager.h"
#include "Contents/Battle/Manager/TESStageManager.h"
#include "Contents/Sound/Manager/TESSoundManager.h"
#include "Contents/Common/Manager/TESSceneManager.h"
#include "Battle/Manager/TESBattlePartyManager.h"
#include "Battle/GameData/TESBattlePartyData.h"
#include "Battle/Character/TESCharacterBattle.h"
#include "Battle/Manager/TESBattleCharacterManager_New.h"

void UTESMirageGardenManager::Start()
{
	Super::Start();

	InitData();


	SetEnableTick(true);
}

void UTESMirageGardenManager::Reset()
{
	Super::Reset();

	InitData();

	ClearManager();
}

void UTESMirageGardenManager::Shutdown()
{
	Super::Shutdown();

	Reset();

	SetEnableTick(false);
}

void UTESMirageGardenManager::Tick(const float& deltaTime)
{
	Super::Tick(deltaTime);

	_accumulateTick += deltaTime;

	if (_accumulateTick >= 1.0f)
	{
		UpdateMirageGardenData();

		_accumulateTick -= 1.0f;
	}
}

void UTESMirageGardenManager::InitData()
{
	_currentOpenMirageGardenSeasonTableId = TESConstantValue::NoneValue;

	_currentDifficultyLevel = 0;

	_currentFloor = 0;

	_currentFloorTableId = 0;

	_mirageGardenOwnBuffList.Empty();

	_rentalCharacterIdList.Empty();

	CheckOpenMirageGardenSeason();

	_accumulateTick = 0.0f;
}

void UTESMirageGardenManager::ClearManager()
{
	_currentOpenMirageGardenSeasonTableId = TESConstantValue::NoneValue;

	_usingPartyUniqueList.Empty();

	_rewardInfo.Empty();

	_useBattlePartyMaxCountMap.Empty();
}

bool UTESMirageGardenManager::CheckOpenMirageGardenSeason()
{
	//. 현재 열린 오픈 시즌이 없는 경우 체크
	int64 currentTimeStamp = SharedTimeManager->GetServerStartTime();

	TArray<const FMirageGardenRow*> allMirageGarden = SharedTDBManager->GetAllRowListMirageGarden();
	
	for (const FMirageGardenRow* mirageGardenRow : allMirageGarden)
	{
		TESCHECK_CONTINUE(nullptr != mirageGardenRow);

		int64 seasonStartTime = ConvertDateTimeDetailToTimeStamp(mirageGardenRow->SeasonStartTime, TESConstantValue::KoreanTimeBias);
		_seasonEndTime = ConvertDateTimeDetailToTimeStamp(mirageGardenRow->SeasonEndTime, TESConstantValue::KoreanTimeBias);

		double remainSeasonStartTime = SharedTimeManager->GetRemainingTimeLeadingUpToTimeStamp(seasonStartTime);
		double remainSeasonEndTime = SharedTimeManager->GetRemainingTimeLeadingUpToTimeStamp(_seasonEndTime);

		//.시즌 시작 시간은 0 보다 작고
		//.시즌 종료 시간은 0 보다 커야함
		if ((remainSeasonStartTime < 0)
			&& (remainSeasonEndTime > 0))
		{
			_currentOpenMirageGardenSeasonTableId = mirageGardenRow->Id;
			UpdateMirageGardenData();

			return true;
		}
	}

	return false;
}

void UTESMirageGardenManager::SetMirageGardenData(TArray<FTESMirageGardenData> mirageGardenDataList)
{
	_currentMirageGardenDataMap.Empty();

	for (int32 index = 0; index < mirageGardenDataList.Num(); ++index)
	{
		_currentMirageGardenDataMap.Add(mirageGardenDataList[index]._difficultyLevel, mirageGardenDataList[index]);
	}

	SetMaximumFloorByDifficulty();
}

const TMap<int32, FTESMirageGardenData> UTESMirageGardenManager::GetMirageGardenData() const
{
	return _currentMirageGardenDataMap;
}

FTESMirageGardenData UTESMirageGardenManager::GetMirageGardenDataByDifficulty(int32 difficultyLevel) const
{
	if (true == _currentMirageGardenDataMap.Contains(difficultyLevel))
	{
		return _currentMirageGardenDataMap[difficultyLevel];
	}
	else
	{
		return {};
	}
}

void UTESMirageGardenManager::UpdateMirageGardenData()
{
	//.현재 시즌이 활성화 상태인지
	if (TESConstantValue::NoneValue == _currentOpenMirageGardenSeasonTableId)
	{
		_isActiveSeason = false;
		return;
	}
	else
	{
		SetSeasonData(_currentOpenMirageGardenSeasonTableId);

		//. 데일리때 바뀌는것들은 보상 초기화.
		_remainDailyTime = SharedTimeManager->GetRemainingTimeLeadingUpToTimeStamp(static_cast<double>(_dailyStamp));

		 //. 시즌이 바뀌는 기준은 렌탈 캐릭터들을 변경 하고 싶을때.
		_remainSeasonEndTime = SharedTimeManager->GetRemainingTimeLeadingUpToTimeStamp(_seasonEndTime);
		
		if (_remainSeasonEndTime > 0)
		{
			_isActiveSeason = true;
			return;
		}
		else
		{
			_isActiveSeason = false;
			return;
		}

	}
}

FTESDateTime UTESMirageGardenManager::ConvertTimeStampToDateTimeDetail(int64 timeStamp, int32 bias)
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

int64 UTESMirageGardenManager::ConvertDateTimeDetailToTimeStamp(FTESDateTime dateTime, int32 bias)
{
	FString timeString = FString::Printf(TEXT("%d %d %d %d %d %d"), dateTime._year, dateTime._month, dateTime._date, dateTime._hour, dateTime._min, dateTime._sec);
	FDateTime outDateTime;
	FDateTime::Parse(timeString, outDateTime);
	int64 timeStamp = outDateTime.ToUnixTimestamp() - (bias * 3600);

	return timeStamp;
}

void UTESMirageGardenManager::SetSeasonData(int32 seasonId)
{
	_currentOpenMirageGardenSeasonTableId = seasonId;
	const FMirageGardenRow& mirageGardenRow = SharedTDBManager->GetMirageGardenRow(_currentOpenMirageGardenSeasonTableId);
	
	if (false == mirageGardenRow.IsEmpty())
	{
		/*_currentSeasonStartTime = ConvertDateTimeDetailToTimeStamp(mirageGardenRow.SeasonStartTime, TESConstantValue::KoreanTimeBias);
		_currentSeasonEndTime = ConvertDateTimeDetailToTimeStamp(mirageGardenRow.SeasonEndTime, TESConstantValue::KoreanTimeBias);*/

		int32 maxCount = mirageGardenRow.DifficultyList.Num();
		for (int32 index = 0; index < maxCount; ++index)
		{
			int32 difficultyId = mirageGardenRow.DifficultyList[index];
			const FMirageGardenDifficultyRow& gardenDifficultyRow = SharedTDBManager->GetMirageGardenDifficultyRow(difficultyId);
			if (false == gardenDifficultyRow.IsEmpty())
			{
				//. 난이도마다 최대 카운트개수를 넣어준다.
				_useBattlePartyMaxCountMap.Add(difficultyId, gardenDifficultyRow.PartyCount_Max);
			}
		}
	}
}

void UTESMirageGardenManager::SetMaximumFloorByDifficulty()
{
	for (TPair<int32, FTESMirageGardenData> child : _currentMirageGardenDataMap)
	{
		const FMirageGardenDifficultyRow& mirageGardenDifficultyRow = SharedTDBManager->GetMirageGardenDifficultyRow(child.Value._difficultyLevel);
		if (true == mirageGardenDifficultyRow.IsEmpty())
		{
			continue;
		}

		TArray<int32> mirageGardenFloorIdList =	mirageGardenDifficultyRow.DifficultyStageId;

		int32 beforeFloor = 0;
		int32 nextFloor = 0;
		int32 highestFloor = 0;

		for (int32 index = 0; index < mirageGardenFloorIdList.Num(); ++index)
		{
			const FMirageGardenFloorRow& mirageGardenFloorRow = SharedTDBManager->GetMirageGardenFloorRow(mirageGardenFloorIdList[index]);
			if (true == mirageGardenFloorRow.IsEmpty())
			{
				continue;
			}

			nextFloor = mirageGardenFloorRow.Floor;
			highestFloor = FMath::Max(beforeFloor, nextFloor);
			beforeFloor = mirageGardenFloorRow.Floor;
		}

		_maximumFloorByDifficulty.Add(child.Key, highestFloor);
	}
}

void UTESMirageGardenManager::SetDailyStamp(int64 dailyStamp)
{
	_dailyStamp = dailyStamp;

	_remainDailyTime = SharedTimeManager->GetRemainingTimeLeadingUpToTimeStamp(static_cast<double>(dailyStamp));
}

void UTESMirageGardenManager::SetDifficultyLevel(int32 difficultyLevel)
{
	_currentDifficultyLevel = difficultyLevel;
}

void UTESMirageGardenManager::SetFloor(int32 floorTableId, int32 floorId)
{
	_currentFloorTableId = floorTableId;

	_currentFloor = floorId;
}

void UTESMirageGardenManager::SetUsingPartyUniqueList(uint16 partyUnique)
{
	if (false == _usingPartyUniqueList.Contains(partyUnique))
	{
		_usingPartyUniqueList.Add(partyUnique);
	}
}

void UTESMirageGardenManager::SetUsingRentalCharacterList(TArray<int32> rentalIdList)
{
	_rentalCharacterIdList.Empty();

	for (int32 index = 0; index < rentalIdList.Num(); ++index)
	{
		_rentalCharacterIdList.Add(rentalIdList[index]);
	}
}

void UTESMirageGardenManager::SetNextCurrentlyUsingPartyUnique()
{
	++_currentUsingPartyIndex;
}

void UTESMirageGardenManager::AddFloorRewardInfo(FTESRewardPopupData goodsInfo)
{
	//. 추후에 어떻게 할지에 따라 로직이 바뀔거라 일단 보류.
	_rewardInfo.Add(goodsInfo);
}

void UTESMirageGardenManager::AddFloorBuffId(int32 mirageGardenBuffId)
{
	//. 매층 버프를 선택할때마다 누적 시킨다.
	_mirageGardenOwnBuffList.Add(mirageGardenBuffId);
}

void UTESMirageGardenManager::AddMirageGardenPartyData(uint16 partyUnique, TArray<class UTESBattlePartyData*> battlePartyData)
{
	_mirageGardenBattleData.Empty();

	for (int32 index = 0; index < battlePartyData.Num(); ++index)
	{
		UTESBattlePartyData* mirageGardenPartyData = NewObject<UTESBattlePartyData>(this);
		TESCHECK_CONTINUE(nullptr != mirageGardenPartyData);

		FTESCharacterSpawnInfo spawnInfo = {};
		
		if (true == ::IsValid(battlePartyData[index]))
		{
			spawnInfo._teamSide = ETeamSide::TeamSideA;
			spawnInfo._characterUnique = battlePartyData[index]->GetCharacterUnique();
			spawnInfo._characterId = battlePartyData[index]->GetCharacterTableId();
			spawnInfo._level = battlePartyData[index]->GetCurrentLevel();
			spawnInfo._awaken = battlePartyData[index]->GetCharacterAwaken();
			spawnInfo._exp = battlePartyData[index]->GetCurrentExp();
			
			mirageGardenPartyData->SetPartyData(spawnInfo);
			mirageGardenPartyData->SetDeckPartyId(battlePartyData[index]->GetDeckPartyId());		//. 생성된 순서

			mirageGardenPartyData->SetBattlePartyId(battlePartyData[index]->GetBattlePartyId());	//. 변경된 순서
			float characterHpRatio = 0.0f;
			float characterApValue = 0.0f;
			if (true == ::IsValid(battlePartyData[index]->GetCharacter()))
			{
				characterHpRatio = battlePartyData[index]->GetCharacter()->GetHPRatio();
				characterApValue = battlePartyData[index]->GetCharacter()->GetCurrentAP();
			}

			mirageGardenPartyData->SetHpRatio(characterHpRatio);
			mirageGardenPartyData->SetAp(characterApValue);
			ECharacterState characterState = ECharacterState::Locomotion;

			if (0.0f >= characterHpRatio)
			{
				characterState = ECharacterState::Dead;
			}

			mirageGardenPartyData->SetCharacterState(characterState);
		}
		
		_mirageGardenBattleData.Add(mirageGardenPartyData);
	}

	if (true == _battlePartyDataMap.Contains(partyUnique))
	{
		_battlePartyDataMap[partyUnique]._battlePartyDataList = _mirageGardenBattleData;
	}
	else
	{
		FTESMirageGardenPartyGroupData groupData = {};
		groupData._partyUniqueId = partyUnique;
		groupData._battlePartyDataList = _mirageGardenBattleData;
		_battlePartyDataMap.Add(partyUnique, groupData);
	}
}

const int32 UTESMirageGardenManager::GetCurrentSeasonId() const
{
	return _currentOpenMirageGardenSeasonTableId;
}

const double UTESMirageGardenManager::GetCurrentDailyRemainTime() const
{
	return _remainDailyTime;
}

//const int64 UTESMirageGardenManager::GetCurrentSeasonStartTime() const
//{
//	return _currentSeasonStartTime;
//}
//
//const int64 UTESMirageGardenManager::GetCurrentSeasonEndTime() const 
//{
//	return _currentSeasonEndTime;
//}

const double UTESMirageGardenManager::GetCurrentSeasonRemainTime() const
{
	return _remainSeasonEndTime;
}

const bool UTESMirageGardenManager::CheckCurrentSeasonActive() const
{
	return _isActiveSeason;
}

int32 UTESMirageGardenManager::GetCurrentSeasonRentalCharacterLevel() const
{
	const FMirageGardenRow& mirageGardenRow = SharedTDBManager->GetMirageGardenRow(_currentOpenMirageGardenSeasonTableId);
	if (true == mirageGardenRow.IsEmpty())
	{
		return TESConstantValue::RentalCharacterLevel;
	}

	return mirageGardenRow.GardenRentalCharactersLv;
}

TArray<uint16> UTESMirageGardenManager::GetUsingPartyUniqueListInMirageGarden() const
{
	return _usingPartyUniqueList;
}

uint16 UTESMirageGardenManager::GetCurrentUsingPartyUnique() const
{
	if (true == _usingPartyUniqueList.IsValidIndex(_currentUsingPartyIndex))
	{
		return _usingPartyUniqueList[_currentUsingPartyIndex];
	}
	else
	{
		return 0;
	}
}

int32 UTESMirageGardenManager::GetCurrentBattlePartyOrder() const
{
	return _currentUsingPartyIndex;
}

TArray<FTESRewardPopupData> UTESMirageGardenManager::GetAccumulateRewardInfoList() const
{
	return _rewardInfo;
}

TArray<int32> UTESMirageGardenManager::GetUsingRentalCharacterIdList() const
{
	return _rentalCharacterIdList;
}

TArray<int32> UTESMirageGardenManager::GetMirageGardenOwnBuffIdList() const
{
	//. 현재 갖고 있는 버프들 리스트.(테이블 아이디)
	return _mirageGardenOwnBuffList;
}

int32 UTESMirageGardenManager::GetHighestFloorByDifficulty(int32 difficultyLevel) const
{
	if (true == _maximumFloorByDifficulty.Contains(difficultyLevel))
	{
		return _maximumFloorByDifficulty[difficultyLevel];
	}
	else
	{
		return 0;
	}
}

TMap<uint16, FTESMirageGardenPartyGroupData> UTESMirageGardenManager::GetBattlePartyDataMap() const
{
	return _battlePartyDataMap;
}

int32 UTESMirageGardenManager::GetRentalRemainCount(int32 difficultyLevel) const
{
	if (true == _currentMirageGardenDataMap.Contains(difficultyLevel))
	{
		return _currentMirageGardenDataMap[difficultyLevel]._rentalCountRemain;
	}
	else
	{
		return 0;
	}
}

int32 UTESMirageGardenManager::GetUseBattlePartyCount(int32 difficultyLevel) const
{
	if (true == _useBattlePartyMaxCountMap.Contains(difficultyLevel))
	{
		return _useBattlePartyMaxCountMap[difficultyLevel];
	}
	else
	{
		return 0;
	}
}

int32 UTESMirageGardenManager::GetCurrentDifficultyLevel() const
{
	return _currentDifficultyLevel;
}

int32 UTESMirageGardenManager::GetCurrentFloor() const
{
	return _currentFloor;
}

int32 UTESMirageGardenManager::GetCurrentFloorTableId() const
{
	return _currentFloorTableId;
}

bool UTESMirageGardenManager::GetIsCurrentStageMirageGarden() const
{
	int32 stageId = SharedStageManager->GetStageId();
	const FStageRow& stageTable = SharedTDBManager->GetStageRow(stageId);

	if (true == stageTable.IsEmpty())
		return false;

	return (ETypeContents::MirageGarden == stageTable.T_Contents) ? true : false;
}

void UTESMirageGardenManager::GoToNextFloor()
{
	const FMirageGardenFloorRow& floorRow = SharedTDBManager->GetMirageGardenFloorRow(_currentFloorTableId);
	if (true == floorRow.IsEmpty())
		return;

	//. 재진입에 따른 BGM도 페이드 아웃을 합니다.
	//SharedSoundManager->FadeOutBGM();

	TESLOG(Display, TEXT("After Floor Id : %d"), _currentFloorTableId);

	//. 전투 진입을 진행합니다.
	SharedStageManager->DestroyMirageGardenAllWarpGate();
	SharedStageManager->Reset();
	SharedStageManager->SetUseGameFlow(true);
	SharedStageManager->SetStage(floorRow.FloorStage);
	SharedStageManager->SetRestart(true);				//. 다시시작은 아니지만 다음층으로 가기 위해 해야한다.

	FTESSceneTransitionData sceneTransitionData = {};
	sceneTransitionData._physicalSceneType = ETESPhysicalGameSceneType::LoadingBattle;
	sceneTransitionData._logicalSceneType = ETESLogicalGameSceneType::LoadingBattle;
	sceneTransitionData._fadeAtrribute._fadeType = ETESSceneFadeType::WidgetAnimation;
	sceneTransitionData._fadeAtrribute._fadeColor = FColor::Black;
	sceneTransitionData._isNeedTransitionMap = true;
	sceneTransitionData._sceneArguments._booleanValues.Add(TESConstantValue::Battle_GSKey_WhetherHideToGiveUp, false);
	sceneTransitionData._sceneArguments._booleanValues.Add(TESConstantValue::Battle_GSKey_IsComeFromMireageGardenBattle, true);
	SharedSceneManager->ChangeGameScene(sceneTransitionData);
}

void UTESMirageGardenManager::ResetAccumulateRewardList()
{
	_rewardInfo.Empty();

	ResetBattlePartyData();
}

void UTESMirageGardenManager::ResetBattlePartyData()
{
	_battlePartyDataMap.Empty();

	_mirageGardenBattleData.Empty();

	_currentUsingPartyIndex = 0;

	_mirageGardenOwnBuffList.Empty();
}

void UTESMirageGardenManager::SetFloorStartAnim()
{
	bool isMirageGarden = SharedStageManager->IsSameContentsType(ETypeContents::MirageGarden);

	if (true == isMirageGarden)
	{
		UTESUWMirageGardenFloorStart* mirageGardenFloorStartAnimWidget = SharedPopupManager->CreatePopupWidget<UTESUWMirageGardenFloorStart>(TEXT("WBP_MirageGardenFloorStart"), TEXT("MirageGarden/WBP"));
		if (true == ::IsValid(mirageGardenFloorStartAnimWidget))
		{
			mirageGardenFloorStartAnimWidget->InitWidget();
			mirageGardenFloorStartAnimWidget->OpenWidget();
			mirageGardenFloorStartAnimWidget->SetWidget(_currentFloor);
		}
	}
}
