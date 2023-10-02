// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TES.h"
#include "Structs/UI/TESUWGenericBase.h"
#include "TESUWIntroMain.generated.h"

/**
 * 
 */
UCLASS()
class TES_API UTESUWIntroMain : public UTESUWGenericBase
{
	GENERATED_BODY()
	
public:
	virtual void InitWidget() override;
	virtual void ClearWidget() override;
	virtual void OpenWidget() override;
	virtual void CloseWidget() override;
	virtual void OpenDelegate() override;
	virtual void CloseDelegate() override;

private:
	void InitDim();
	void InitIntroRoot();
	void InitIntroPresets();
	void InitClientVersion();
	void InitClientType();
	void InitResourceVersion();
	void InitTouchToStart();
	void InitUserEMailAddress();
	void InitHomepageButton();
	void InitTwitterButton();
	void InitDiscordButton();
	void InitLogoutButton();
	void InitExitButton();

private:
	void ClearIntroPresets();

public:
	void SetWidget(int32 introPrestIndex);
	void AfterCompletePatch();
	void AfterCompleteLogin();
	void SetDimVisible(bool isVisible);
	void SetHomepageButtonVisibility(ESlateVisibility visibility);
	void SetTwitterButtonVisibility(ESlateVisibility visibility);
	void SetDiscordButtonVisibility(ESlateVisibility visibility);
	void SetLogoutButtonVisibility(ESlateVisibility visibility);
	void SetExitButtonVisibility(ESlateVisibility visibility);

private:
	void SetIntroPresets(int32 presetIndex, bool isVisibility);

public:
	void SetContents(int32 presetIndex);

private:
	void SwitchToNextContents(int32 presetIndex);

private:
	void SetClientVersion(FString clientVersion);
	void SetClientType(FString clientType);
	void SetResourceVersion(FString resourceVersion);
	void SetTouchToStart();

public:
	void HideAllIntroWidgets();
	void HideBottomButtons();

public:
	UFUNCTION()
	void OnEvent_NextPreset(EIntroPresetType introPresetType = EIntroPresetType::None);

	UFUNCTION()
	void OnEvent_ShowPatchDownload();

	UFUNCTION()
	void OnEvent_ShowTouchToStart();

private:
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = true))
	void OnEvent_GoToHomePage();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = true))
	void OnEvent_GoToTwitter();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = true))
	void OnEvent_GoToDiscord();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = true))
	void OnEvent_LogOut();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = true))
	void OnEvent_Exit();

//. ───────────────────────────────────────────────────────────────────────────────────────────────
//. [ Member Variables ] 
//. ───────────────────────────────────────────────────────────────────────────────────────────────
private:
	UPROPERTY() int32 _prevPresetIndex = TESConstantValue::NoneValue;
	UPROPERTY()	int32 _curPresetIndex = TESConstantValue::NoneValue;
	UPROPERTY() bool _isSwitchingToNextContents = false;

private:
	UPROPERTY() FTimerHandle th_nextSequence = {};

private:
	UPROPERTY() class UOverlay* _root_contents = nullptr;
	UPROPERTY() class UTESImage* _img_dim = nullptr;
	UPROPERTY() class UTESTextBlock* _txt_clientVersion = nullptr;
	UPROPERTY() class UTESTextBlock* _txt_clientType = nullptr;
	UPROPERTY() class UTESTextBlock* _txt_resourceVersion = nullptr;
	UPROPERTY() class UTESTextBlock* _txt_touchToStart = nullptr;
	UPROPERTY() class UTESTextBlock* _txt_userEMailAddress = nullptr;
	UPROPERTY() class UTESButton* _btn_homepage = nullptr;
	UPROPERTY() class UTESButton* _btn_twitter = nullptr;
	UPROPERTY() class UTESButton* _btn_discord = nullptr;
	UPROPERTY() class UTESButton* _btn_logout = nullptr;
	UPROPERTY() class UTESButton* _btn_exit = nullptr;

private:
	UPROPERTY()
	TArray<class UTESUWIntroPresetBase*> _introPresetWidgetList;

private:
	UPROPERTY(EditAnywhere, Category = "IntroPresets", Meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class UTESUWIntroPresetBase>> _introPresetClassList;
//. ───────────────────────────────────────────────────────────────────────────────────────────────
};
