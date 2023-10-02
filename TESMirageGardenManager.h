// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TES.h"
#include "Structs/Manager/TESManagerBase.h"
#include "TESMirageGardenManager.generated.h"

/**
 * 
 */
USTRUCT()
struct FTESMirageGardenPartyGroupData
{
	GENERATED_BODY()
public:
	UPROPERTY() uint16 _partyUniqueId = 0;
	UPROPERTY() TArray<class UTESBattlePartyData*> _battlePartyDataList = {};
};

UCLASS()
class TES_API UTESMirageGardenManager : public UTESManagerBase
{
	GENERATED_BODY()
	
//. ──────────────────────────────────────────────────────────────────────────────────────────────
//. [ Override Functions ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────
public:
	virtual void Start() override;
	virtual void Reset() override;
	virtual void Shutdown() override;
	virtual void Tick(const float& deltaTime) override;

//. ──────────────────────────────────────────────────────────────────────────────────────────────
//. [ General Functions ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────
private:
	void InitData();

private:
	void ClearManager();

public:
	bool CheckOpenMirageGardenSeason();

public:
	void SetMirageGardenData(TArray<FTESMirageGardenData> mirageGardenDataList);
	const TMap<int32, FTESMirageGardenData> GetMirageGardenData() const;
	FTESMirageGardenData GetMirageGardenDataByDifficulty(int32 difficultyLevel) const;

public:
	void UpdateMirageGardenData();

public:
	FTESDateTime ConvertTimeStampToDateTimeDetail(int64 timeStamp, int32 bias = 0);
	int64 ConvertDateTimeDetailToTimeStamp(FTESDateTime dateTime, int32 bias = 0);

private:
	void SetSeasonData(int32 seasonId);
	void SetMaximumFloorByDifficulty();

public:
	void SetDailyStamp(int64 dailyStamp);
	void SetDifficultyLevel(int32 difficultyLevel);
	void SetFloor(int32 floorTableId, int32 floorId);
	void SetUsingPartyUniqueList(uint16 partyUnique);
	void SetUsingRentalCharacterList(TArray<int32> rentalIdList);
	void SetNextCurrentlyUsingPartyUnique();			//. 현재 파티가 죽어서 다음파티로 전환할때 쓰는 함수.

public:
	void AddFloorRewardInfo(FTESRewardPopupData goodsInfo);
	void AddFloorBuffId(int32 mirageGardenBuffId);
	void AddMirageGardenPartyData(uint16 partyUnique, TArray<class UTESBattlePartyData*> battlePartyData);

public:
	const int32 GetCurrentSeasonId() const;
	/*const int64 GetCurrentSeasonStartTime() const;
	const int64 GetCurrentSeasonEndTime() const;*/
	const double GetCurrentDailyRemainTime() const;
	const double GetCurrentSeasonRemainTime() const;
	const bool CheckCurrentSeasonActive() const;
	int32 GetCurrentSeasonRentalCharacterLevel() const;

public:
	TArray<uint16> GetUsingPartyUniqueListInMirageGarden() const;
	uint16 GetCurrentUsingPartyUnique() const;		//. 현재 사용중인 파티유니크 
	int32 GetCurrentBattlePartyOrder() const;		//. 현재 전투에서 사용중인 인덱스
	TArray<FTESRewardPopupData> GetAccumulateRewardInfoList() const;
	TArray<int32> GetUsingRentalCharacterIdList() const;
	TArray<int32> GetMirageGardenOwnBuffIdList() const;
	int32 GetHighestFloorByDifficulty(int32 difficultyLevel) const;

public:
	TMap<uint16, FTESMirageGardenPartyGroupData> GetBattlePartyDataMap() const;

public:
	int32 GetRentalRemainCount(int32 difficultyLevel) const;
	int32 GetUseBattlePartyCount(int32 difficultyLevel) const;
	int32 GetCurrentDifficultyLevel() const;
	int32 GetCurrentFloor() const;
	int32 GetCurrentFloorTableId() const;

	bool GetIsCurrentStageMirageGarden() const;

public:
	void GoToNextFloor();

public:
	void ResetAccumulateRewardList();

private:
	void ResetBattlePartyData();

public:
	void SetFloorStartAnim();

//. ──────────────────────────────────────────────────────────────────────────────────────────────
//. [ OnEvent Functions ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────

//. ──────────────────────────────────────────────────────────────────────────────────────────────
//. [ Member Variables ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────
//. 서버에서 받은 데이터
	UPROPERTY() TMap<int32, FTESMirageGardenData> _currentMirageGardenDataMap = {};		//. 난이도에 별로 갖고 있는 정보들.
	UPROPERTY() TMap<int32, int32> _maximumFloorByDifficulty = {};

//. 누적 시간
	UPROPERTY() float _accumulateTick = 0.0f;

//. 전투 정보
private:
	UPROPERTY() int32 _currentUsingPartyIndex = 0;			//. 현재 사용중인 파티유니크 리스트의 인덱스.
 
//. 보상 정보
private:
	UPROPERTY() TArray<FTESRewardPopupData> _rewardInfo = {};

	UPROPERTY() TMap<uint16, FTESMirageGardenPartyGroupData> _battlePartyDataMap = {};	//. 각 파티별 전투에 있는 캐릭터들.

	UPROPERTY() TArray<class UTESBattlePartyData*> _mirageGardenBattleData = {};

//. 시즌 정보
private:
	UPROPERTY() int32 _currentOpenMirageGardenSeasonTableId = TESConstantValue::NoneValue;

	UPROPERTY() int64 _dailyStamp = 0;

	UPROPERTY()  double _remainDailyTime = 0.0f;

	UPROPERTY() int64 _seasonEndTime = 0;

	UPROPERTY() double _remainSeasonEndTime = 0.0f;

	UPROPERTY() int32 _currentDifficultyLevel = 0;

	UPROPERTY() int32 _currentFloor = 0;

	UPROPERTY() int32 _currentFloorTableId = 0;

	UPROPERTY() bool _isActiveSeason = false;

	UPROPERTY() TArray<int32> _mirageGardenOwnBuffList = {};		//. 1층부터 골라서 누적 시킨 버프 리스트(테이블 아이디)

	UPROPERTY() TMap<int32, int32> _useBattlePartyMaxCountMap = {};	//. 최대로 할수 있는 파티편성 개수(Key : 난이도 레벨, Value : 최대 카운트)

	UPROPERTY() TArray<uint16> _usingPartyUniqueList = {};			//. 이번 신기루 정원에서 쓰고 있는 파티 리스트.

	UPROPERTY() TArray<int32> _rentalCharacterIdList = {};
};
