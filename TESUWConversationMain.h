// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TES.h"
#include "UI/TESUWGenericBase.h"
#include "TESUWConversationMain.generated.h"

/**
 * 
 */
UCLASS()
class TES_API UTESUWConversationMain : public UTESUWGenericBase
{
	GENERATED_BODY()

//. ──────────────────────────────────────────────────────────────────────────────────────────────
//. [ Override Functions ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────
public:
	virtual void InitWidget() override;
	virtual void ClearWidget() override;
	virtual void OpenWidget() override;
	virtual void CloseWidget() override;
	virtual void OpenDelegate() override;
	virtual void CloseDelegate() override;
	virtual void NativeTick(const FGeometry& myGeometry, float deltaTime) override;

//. ──────────────────────────────────────────────────────────────────────────────────────────────
//. [ General Functions ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────
private:
	void InitRootInfoNotify();
	void InitInfoNotify();
	void InitInfoNotifyText();
	void InitRootBottomUI();
	void InitNameText();
	void InitConversationText();
	void InitAutoButtonText();
	void InitSkipButton();
	void InitAutoButton();
	void InitMarkers();
	void InitBackground();
	void InitConversationStringList();
	void Init2DCutscene();
	void Init2DPopupImage();
	void InitAutoToggleVFX();
	void Init2DBackgroundEffectImage();
	void InitBackgroundActor();
	void InitRootChoiceList();
	void InitChoiceSizeBoxList();
	void InitChoiceTextList();
	void InitUIFXPSScreen();
	void InitBackgroundBlur();

public:
	void SetWidget();

private:
	void SetConversationInfo();

private:
	float SetInfoNotify(const int32 infoId);
	float SetBottomUI();
	void SetCharacterLocation();
	void StartConversation(float startTime);
	void SetCharacterName(const int32 characterNameId);
	void SetConversation(const int32 conversationId, const int32 justification, const float animSpeed);
	void SetConversation(const int32 currentContextIndex);
	void SetSpeakingMontage();
	void SetSkipButton();
	void SetAutoButton();
	void SetMarkers(bool setShow);
	void Set2DCutscene();
	void Set2DPopupImage(TArray<FTESConversationPopupImage> popupImageList);
	void SetBGM();
	void SetAmbientSound();
	void SetAutoToggleVisible(bool isVisible);
	void SetBackgroundImage();
	void SetBackgroundLocation();
	void SetBackgroundRotation();
	void Set2DBackgroundEffectImage(FVector4D imageEffect);
	void Set3DEffect(int32 vfxTableId, FVector vfxLocation);
	void SetCharacterEffect(int32 vfxTableId, class ATESCharacterConversation* character, FString socketName, FVector relativeLocation, float lifeTime);
	void SetRootChoice(bool isVisible);
	bool SetChoiceList();
	void ResetChoiceList();
	void SetPSScreenWidget(FString resourcePath);
	void SetVoiceSound(int32 soundId);
	void SetEffectSound(int32 soundId);
	void SetBackGroundInfo();
	void SetBackGroundBlur(float blurValue);

private:
	float GetCurrentTextSpeed();

public:
	void OnEvent_ScreenTouched(FVector touchLocation);

public:
	ETESAutoSpeedType GetAutoSpeed() const;

private:
	void CheckSceneTranslationAfterConversation();

private:
	void PlayFirstConversation();
	void PlayCurrentConversation();
	void PlayNextConversation(int32 nextConversationId);
	void GoToNextScene();
	void Hide2DPopupImage();
	
public:
	void EndConversation();

public:
	void PlayConversationByConversationId(int32 conversationId, int32 nextConversationId = 0);

private:
	void PlayNextLine();
	void OnEvent_EnterBattle();
	//void OnEvent_EndBattle(FTESBattleResultData battleResultData);

private:
	bool CheckPrintingText();

private:
	void TickAutoToggle(float deltaTime);
	void TickSkipAvailable(float deltaTime);

private:
	void DestroySceneEffect();
	void DestroyCharacterEffect();

//. ──────────────────────────────────────────────────────────────────────────────────────────────
//. [ OnEvent Functions ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────
private:
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnClicked_PrevConversation();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnClicked_AutoButton();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnClicked_SkipConversation();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnClicked_FirstChoice();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnClicked_SecondChoice();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnClicked_ThirdChoice();

//. ──────────────────────────────────────────────────────────────────────────────────────────────
//. [ Member Variables ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────
private:
	UPROPERTY() FTESConversationData _currentConversationData = {};
	UPROPERTY() TMap<int32, FTESConversationData> _fullConversationDataById = {};
	UPROPERTY() int32 _currentConversationId = 0;
	UPROPERTY() int32 _nextConversationId = 0;

private:
	class UTESUWConversationSceneChange* _fadeInOutWidget = nullptr;

private:
	UPROPERTY() ETESAutoSpeedType _autoSpeed = ETESAutoSpeedType::None;
	UPROPERTY() ETESAutoSpeedType _tempAutoSpeed = ETESAutoSpeedType::None;

private:
	UPROPERTY() class AStaticMeshActor* _backgroundActor = nullptr;

private:
	UPROPERTY()	class ATESVFX* _sceneVFX = nullptr;
	UPROPERTY()	TArray<class ATESVFX*> _characterVFXList = {};

private:
	UPROPERTY() bool _doesHaveSceneTranslation = false;		//. 회화 종료 후에 연결된 씬이 있는지
	UPROPERTY() FString _nextSceneName = TEXT("");

private:
	UPROPERTY() bool _isTextAnimated = false;
	UPROPERTY() bool _isUIOpened = false;
	UPROPERTY() float _leftTimeForNextConversation = false;
	UPROPERTY() bool _isBlocking = false;
	UPROPERTY() bool _isShowingInfo = false;
	UPROPERTY() bool _isChoiceOn = false;
	UPROPERTY() bool _isFadeInOut = false;
	UPROPERTY() bool _isSceneChanging = false;											//. 씬 전환중인가?

private:
	UPROPERTY() float _textIntervalTime = 0.0f;		//. 텍스트간 등장 간격 시간
	UPROPERTY() float _textTimeCount = 0.0f;		//. 실제로 시간을 체크하는 변수
	UPROPERTY() float _textSkipDelayTime = 0.0f;	//. 회화 대사가 시작되고 빠른 터치를 방지하기 위한 최소한의 시간.

private:
	UPROPERTY() int32 _numOfNewLine = 0;			//. 대사 개행의 숫자
	UPROPERTY() int32 _currentPlayingLine = 0;		//. 현재 진행하고 있는 줄

private:
	UPROPERTY() FTimerHandle _th_startTimer = {};
	UPROPERTY() FTimerHandle _th_playingTimer = {};
	UPROPERTY() FTimerHandle _th_fadeOutAction = {};
	UPROPERTY() FTimerHandle _th_fadeInAction = {};

#pragma region UMG Widget
private:
	UPROPERTY() class UCanvasPanel* _cp_rootInfoNotify = nullptr;
	UPROPERTY() class UCanvasPanel* _cp_rootBottomUI = nullptr;
	UPROPERTY() class UTESTextBlock* _txt_infoNotify = nullptr;
	UPROPERTY() class UTESTextBlock* _txt_characterName = nullptr;
	UPROPERTY() class UTESTextBlock* _txt_conversation = nullptr;
	UPROPERTY() class UTESTextBlock* _txt_Auto = nullptr;
	UPROPERTY() class USizeBox* _sb_conversation = nullptr;
	UPROPERTY() class UTESButton* _btn_skip = nullptr;
	UPROPERTY() class UTESButton* _btn_auto = nullptr;
	UPROPERTY() class UTESImage* _img_autoToggle = nullptr;

	//. 2d 컷씬 이미지
	UPROPERTY() class UTESImage* _img_2dCutscene = nullptr;
	UPROPERTY() class UTESImage* _img_rectBox = nullptr;

	//. 2d 암막 효과 이미지
	UPROPERTY() class UTESImage* _img_2dBackgroundEffect = nullptr;

	//. 2d 팝업 이미지 리스트
	UPROPERTY() TArray<class UTESImage*> _img_2dPopupList = {};
	UPROPERTY() TArray<class UTESImage*> _img_2dPopupDecoList = {};

	//. 선택지 리스트 캔버스
	UPROPERTY() class UVerticalBox* _vb_rootChoice = nullptr;
	UPROPERTY() class UHorizontalBox* _hb_rootSkip = nullptr;

	//. 선택지 버튼 리스트
	UPROPERTY() TArray<class USizeBox*> _sb_choiceList = {};

	//. 선택지 텍스트 리스트
	UPROPERTY() TArray<class UTESTextBlock*> _txt_choiceTextList = {};

	//. 출력완료 마커
	UPROPERTY()	class UTESImage* _img_triangleMarker = nullptr;
	UPROPERTY()	class UTESImage* _img_rectangleMarker = nullptr;

	UPROPERTY() class UTESImage* _img_background = nullptr;

private:
	UPROPERTY() class UTESUWUIFXPSScreen* _uiFXPSScreenWidget = nullptr;
	UPROPERTY() class UBackgroundBlur* _bb_conversationBlur = nullptr;

private:
	//. 대사(텍스트 애니메이션)
	UPROPERTY() TArray<class UTESConversationString*> _conversationStringWidgetList = {};
	UPROPERTY() TArray<FString> _splitContextArray;
#pragma endregion UMG Widget

};
