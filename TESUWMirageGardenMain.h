// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TES.h"
#include "Structs/UI/TESUWGenericBase.h"
#include "TESUWMirageGardenMain.generated.h"

/**
 * 
 */
UCLASS()
class TES_API UTESUWMirageGardenMain : public UTESUWGenericBase
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

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ General Functions ] 
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	void InitCommonHeader();
	void InitMirageGardenDifficultyInfoWidget();

private:
	void ClearCommonHeader();

public:
	void SetWidget();

private:
	void SetCommonHeader();
	void SetMirageGardenDifficultyInfoWidget();

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ OnEvent Functions ] 
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_Back();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_Home();

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Member Variables ] 
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	UPROPERTY() class UTESUWCommonHeader* _wbp_commonHeader = nullptr;
	UPROPERTY() class UTESUWMirageGardenDifficultyInfo* _wbp_mirageGardenDifficultyInfo = nullptr;

private:
	UPROPERTY() int32 _currentMirageGardenId = 0;			//. 현재 시즌 아이디.
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
};
