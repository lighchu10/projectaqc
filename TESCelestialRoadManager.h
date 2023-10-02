// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TES.h"
#include "Structs/Manager/TESManagerBase.h"
#include "TESCelestialRoadManager.generated.h"

/**
 * [ Note ] 빠른 탐색을 하기 위해 레벨업당 별에 대한 효과 데이터를 그룹핑해 놓은 데이터입니다.
 */
USTRUCT()
struct FTESCelestialRoadRewardPerLevelData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY() int32 _seasonPassId = 0;
	UPROPERTY() TArray<int32> _seasonPassRewardIdList = {};
};

UCLASS()
class TES_API UTESCelestialRoadManager : public UTESManagerBase
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
//. [ Clear Functions ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	void ClearRewardListPerLevelData();

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Set Functions ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	void SetupRewardListPerLevelData();

public:
	//. 시즌패스 정보를 세팅합니다.
	void SetSeasonPassInfo(FTESCelestialPassInfo seasonPassInfo);

	//. 랩핑한 구조체로 바로 세팅.
	void SetSeasonPassInfo(FTESCelestialRoadData seasonPassInfo);

	//. 미션패스 경험치를 더해준다.(현재 + 추가분)
	void AddSeasonPassAccumulateExp(int32 exp);

	//. 미션패스 레벨을 세팅.(대입)
	void SetSeasonPassLevel(int32 seasonPassLevel);

	void SetIsRewardClaimedAll(bool isClaimedAll);

private:
	void SetSeasonPassEndTimeStamp(int32 celestialRoadTableId);
	void SetSeasonPassPurchaseableCompassTimeStamp(int32 celestialRoadTableId);

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Get Functions ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
public:
	const int64& GetSeasonPassMissionTimeStamp() const;
	const int64& GetSeasonPassPurchaseableCompassTimeStamp() const;
	int32 GetCurrentSeasonPassId() const;
	int32 GetCurrentSeasonPassLevel() const;
	int32 GetSeasonPassRewardTableIdByLevel(int32 seasonpassLevel);
	int32 GetAccumulateExp() const;
	int32 GetMaximumRewardLevel(int32 currentSeasonPassId) const;
	bool GetAllRewardIsClaimed() const;
	bool CheckAnyRewardsCanClaim();
	bool CheckIfSeasonNeedToBeRenew() const;
	TArray<int32> GetSeasonPassExposureLevelList() const;

public:
	FTESCelestialRoadData GetSeasonPassInfo() const;

public:
	TArray<int32> GetSeasonPassRewardTableIdListBySeasonPassId(int32 currentSeasonPassId) const;

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ General Functions ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
	bool OpenCelestialRoadNoticePopup(TDelegate<void()> onNextAction);

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Convert Functions ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
public:
	FTESDateTime ConvertTimeStampToDateTimeDetail(int64 timeStamp, int32 bias = 0);
	int64 ConvertDateTimeDetailToTimeStamp(FTESDateTime dateTime, int32 bias = 0);

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Member Variables ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	//. 해당 시즌패스 정보.
	UPROPERTY() FTESCelestialRoadData _seasonPassData = {};

private:
	//. 해당 시즌에 있는 1렙~만렙까지 보상들의 테이블 아이디.(키값 : 시즌 넘버. 밸류값 : celestialroadReward Table Id List)
	UPROPERTY() TMap<int32, FTESCelestialRoadRewardPerLevelData> _seasonPassRewardTableIdListBySeasonId = {};

private:
	//. 일일미션의 타임스탬프에 대한 변수입니다.
	UPROPERTY() int64 _seasonPassMissionTimeStamp = 0;					//. 시즌패스 미션 및 시즌패스가 끝나는 시간.
	UPROPERTY() int64 _seasonPassPurchaseableCompassTimeStamp = 0;		//. 시즌패스 나침반 구매 가능한 시간.

private:
	//. 노말 및 프리미엄 보상을 전부 수령했는지?
	UPROPERTY() bool _isAllRewardClaimed = false;

private:
	UPROPERTY() bool _needToRenewSeason = false;		//. 해당 만료시간이 지나 다음 시즌패스로 갱신이 필요할때 true.
};
