// Fill out your copyright notice in the Description page of Project Settings.

#include "Contents/Conversation/UI/TESUWConversationMain.h"
#include "Contents/Conversation/UI/TESUWStorySkipPopup.h"
#include "Contents/Conversation/Manager/TESConversationManager.h"
#include "Contents/Conversation/UI/TESUWConversationSceneChange.h"
#include "Contents/Conversation/Character/TESCharacterConversation.h"
#include "Contents/Common/UI/LevelSequence/TESUWLevelSequence.h"
#include "Contents/Common/UI/CustomSlates/TESConversationString.h"
#include "Contents/Common/UI/CustomSlates/TESTextBlock.h"
#include "Contents/Common/UI/CustomSlates/TESButton.h"
#include "Contents/Common/UI/CustomSlates/TESImage.h"
#include "Contents/Common/Manager/TESPopupManager.h"
#include "Contents/Common/Manager/TESSceneManager.h"
#include "Contents/Common/Manager/TESUIManager.h"
#include "Contents/Common/Manager/TESUIFXManager.h"
#include "Contents/Common/Manager/TESSaveGameManager.h"
#include "Contents/Common/Manager/TESUserDataManager.h"
#include "Contents/Common/Manager/TESLevelSequenceManager.h"
#include "Contents/Common/UI/UIFX/TESUWUIFXPSScreen.h"
#include "Contents/Battle/Manager/TESVFXManager.h"
#include "Contents/Shop/UI/ShopMenu/TESUWNpcMenu.h"
#include "Contents/Battle/Manager/TESStageManager.h"
#include "Contents/Common/Manager/TESUserDataManager.h"
#include "Battle/Actor/TESVFX.h"
#include "Sound/Manager/TESSoundManager.h"
#include "Lobby/PlayerController/TESPlayerControllerLobby.h"
#include "Lobby/Character/TESCharacterLobby.h"
#include "Lobby/Character/TESCharacterNPC.h"
#include "Lobby/Character/TESCharacterMovableNPC.h"
#include "Lobby/Manager/TESLobbyManager.h"
#include "Components/CanvasPanel.h"
#include "Components/BackgroundBlur.h"
#include "TESUtils.h"
#include "TESUtilPath.h"
#include "TESUtilTable.h"
#include "TESUtilString.h"
#include "TESUtilTexture.h"
#include "TESGameInstance.h"


void UTESUWConversationMain::InitWidget()
{
	Super::InitWidget();

	InitRootInfoNotify();

	InitInfoNotify();

	InitInfoNotifyText();

	InitRootBottomUI();

	InitNameText();

	InitAutoButtonText();

	InitSkipButton();

	InitAutoButton();

	InitMarkers();

	InitBackground();

	InitConversationStringList();

	Init2DCutscene();

	Init2DPopupImage();

	InitAutoToggleVFX();

	InitBackgroundActor();

	Init2DBackgroundEffectImage();

	InitRootChoiceList();

	InitChoiceSizeBoxList();

	InitChoiceTextList();

	InitUIFXPSScreen();

	InitBackgroundBlur();
}

void UTESUWConversationMain::ClearWidget()
{
	Super::ClearWidget();

	GetWorld()->GetTimerManager().ClearTimer(_th_startTimer);
	_th_startTimer = {};

	GetWorld()->GetTimerManager().ClearTimer(_th_playingTimer);
	_th_playingTimer = {};

	GetWorld()->GetTimerManager().ClearTimer(_th_fadeOutAction);
	_th_fadeOutAction = {};

	GetWorld()->GetTimerManager().ClearTimer(_th_fadeInAction);
	_th_fadeInAction = {};

	for (UTESConversationString* stringWidget : _conversationStringWidgetList)
	{
		TESCHECK_CONTINUE(stringWidget);
		stringWidget->SetEmpty();
	}

	if (true == ::IsValid(_uiFXPSScreenWidget))
	{
		_uiFXPSScreenWidget->ClearUIFX();
		_uiFXPSScreenWidget = nullptr;
	}

	_fadeInOutWidget = nullptr;

	_fullConversationDataById.Empty();

	_conversationStringWidgetList.Empty();

	DestroyCharacterEffect();

	DestroySceneEffect();
}

void UTESUWConversationMain::OpenWidget()
{
	Super::OpenWidget();

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UTESUWConversationMain::CloseWidget()
{
	Super::CloseWidget();

	SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWConversationMain::OpenDelegate()
{
	Super::OpenDelegate();

	FTESUtilEvents::OnNextLine.BindUObject(this, &UTESUWConversationMain::PlayNextLine);

	FTESUtilEvents::OnBattleEnter.BindUObject(this, &UTESUWConversationMain::OnEvent_EnterBattle);

	//FTESUtilEvents::OnBattleResult.BindUObject(this, &UTESUWConversationMain::OnEvent_EndBattle);
}

void UTESUWConversationMain::CloseDelegate()
{
	Super::CloseDelegate();

	FTESUtilEvents::OnNextLine.Unbind();

	//FTESUtilEvents::OnBattleResult.Unbind();
}

void UTESUWConversationMain::NativeTick(const FGeometry& myGeometry, float deltaTime)
{
	Super::NativeTick(myGeometry, deltaTime);

	TickAutoToggle(deltaTime);

	TickSkipAvailable(deltaTime);
}

void UTESUWConversationMain::InitRootInfoNotify()
{
	_textSkipDelayTime = 0.0f;

	_cp_rootInfoNotify = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Root_InfoNotify")));
	TESCHECK(nullptr != _cp_rootInfoNotify);
	_cp_rootInfoNotify->SetVisibility(ESlateVisibility::Collapsed);

	_hb_rootSkip = Cast<UHorizontalBox>(GetWidgetFromName(TEXT("HB_Skip")));
	TESCHECK(nullptr != _hb_rootSkip);
	_hb_rootSkip->SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWConversationMain::InitInfoNotify()
{
	_isUIOpened = false;
}

void UTESUWConversationMain::InitInfoNotifyText()
{
	_txt_infoNotify = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_InfoNotify")));
	TESCHECK(nullptr != _txt_infoNotify);
	_txt_infoNotify->SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWConversationMain::InitRootBottomUI()
{
	_cp_rootBottomUI = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Root_BottomUI")));
	TESCHECK(nullptr != _cp_rootBottomUI);
	_cp_rootBottomUI->SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWConversationMain::InitNameText()
{
	_txt_characterName = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_Name")));
	TESCHECK(nullptr != _txt_characterName);
	_txt_characterName->SetText(FText::FromString(TEXT("")));
	_txt_characterName->SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWConversationMain::InitConversationText()
{
	_txt_conversation = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_Conversation")));
	TESCHECK(nullptr != _txt_conversation);
	_txt_conversation->SetText(FText::FromString(TEXT("")));
	_txt_conversation->SetVisibility(ESlateVisibility::Collapsed);
	
	_isTextAnimated = false;
}

void UTESUWConversationMain::InitAutoButtonText()
{
	_txt_Auto = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_Auto")));
	TESCHECK(nullptr != _txt_Auto);
	_txt_Auto->SetText(FText::FromString(TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Conversation_Button_Manual)));

	//. 오토가 꺼져있을때는 None
	_autoSpeed = ETESAutoSpeedType::None;
}

void UTESUWConversationMain::InitSkipButton()
{
	_btn_skip = Cast<UTESButton>(GetWidgetFromName(TEXT("BTN_SkipConversation")));
	TESCHECK(nullptr != _btn_skip);
	_btn_skip->SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWConversationMain::InitAutoButton()
{
	_btn_auto = Cast<UTESButton>(GetWidgetFromName(TEXT("BTN_AUTO")));
	TESCHECK(nullptr != _btn_auto);
	_btn_auto->SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWConversationMain::InitMarkers()
{
	_img_triangleMarker = Cast<UTESImage>(GetWidgetFromName(TEXT("IMG_TriangleMarker")));
	TESCHECK(nullptr != _img_triangleMarker);
	_img_triangleMarker->SetVisibility(ESlateVisibility::Collapsed);

	_img_rectangleMarker = Cast<UTESImage>(GetWidgetFromName(TEXT("IMG_RectangleMarker")));
	TESCHECK(nullptr != _img_rectangleMarker);
	_img_rectangleMarker->SetVisibility(ESlateVisibility::Collapsed);

	//. 오토 기본 상태일때는 다음 대사가 나오기까지 2초 걸린다.
	_leftTimeForNextConversation = TESConstantValue::LeftTimeToNextConversation;
}

void UTESUWConversationMain::InitBackground()
{
	_img_background = Cast<UTESImage>(GetWidgetFromName(TEXT("IMG_BackBG")));
	TESCHECK(nullptr != _img_background);
	_img_background->SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWConversationMain::InitConversationStringList()
{
	UTESConversationString* conversationStringWidget = Cast<UTESConversationString>(GetWidgetFromName(TEXT("TESConversationBox")));
	TESCHECK(nullptr != conversationStringWidget);
	conversationStringWidget->Init();
	_conversationStringWidgetList.Add(conversationStringWidget);

	UTESConversationString* conversationStringWidget2 = Cast<UTESConversationString>(GetWidgetFromName(TEXT("TESConversationBox_2")));
	TESCHECK(nullptr != conversationStringWidget2);
	conversationStringWidget2->Init();
	_conversationStringWidgetList.Add(conversationStringWidget2);

	UTESConversationString* conversationStringWidget3 = Cast<UTESConversationString>(GetWidgetFromName(TEXT("TESConversationBox_3")));
	TESCHECK(nullptr != conversationStringWidget3);
	conversationStringWidget3->Init();
	_conversationStringWidgetList.Add(conversationStringWidget3);

	UTESConversationString* conversationStringWidget4 = Cast<UTESConversationString>(GetWidgetFromName(TEXT("TESConversationBox_4")));
	TESCHECK(nullptr != conversationStringWidget4);
	conversationStringWidget4->Init();
	_conversationStringWidgetList.Add(conversationStringWidget4);
}

void UTESUWConversationMain::Init2DCutscene()
{
	_img_2dCutscene = Cast<UTESImage>(GetWidgetFromName(TEXT("IMG_2DCutScene")));
	TESCHECK(nullptr != _img_2dCutscene);
	_img_2dCutscene->SetVisibility(ESlateVisibility::Collapsed);

	_img_rectBox = Cast<UTESImage>(GetWidgetFromName(TEXT("IMG_RectBox")));
	TESCHECK(nullptr != _img_rectBox);
	_img_rectBox->SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWConversationMain::Init2DPopupImage()
{
	for (int32 index = 1; index <= 3; ++index)
	{
		FString imagePath = FString::Printf(TEXT("IMG_2DPopupImage_%d"), index);
		UTESImage* img_2dPopup = Cast<UTESImage>(GetWidgetFromName(*imagePath));
		TESCHECK(nullptr != img_2dPopup);
		img_2dPopup->SetVisibility(ESlateVisibility::Collapsed);
		_img_2dPopupList.Add(img_2dPopup);
		
		FString decoImagePath = FString::Printf(TEXT("IMG_2DPopupDeco_%d"), index);
		UTESImage* _img_2dPopupDeco = Cast<UTESImage>(GetWidgetFromName(*decoImagePath));
		TESCHECK(nullptr != _img_2dPopupDeco);
		_img_2dPopupDeco->SetVisibility(ESlateVisibility::Collapsed);

		_img_2dPopupDecoList.Add(_img_2dPopupDeco);
	}
}

void UTESUWConversationMain::InitAutoToggleVFX()
{
	_img_autoToggle = Cast<UTESImage>(GetWidgetFromName(TEXT("IMG_ToggleVFX")));
	TESCHECK(nullptr != _img_autoToggle);
	_img_autoToggle->SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWConversationMain::Init2DBackgroundEffectImage()
{
	_img_2dBackgroundEffect = Cast<UTESImage>(GetWidgetFromName(TEXT("IMG_Black")));
	TESCHECK(nullptr != _img_2dBackgroundEffect);
	_img_2dBackgroundEffect->SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWConversationMain::InitBackgroundActor()
{
	_backgroundActor = TESUtils::FindActorByName<AStaticMeshActor>(GetWorld(), TEXT("ConversationBackgroundPlane"));
	TESCHECK(nullptr != _backgroundActor);
}

void UTESUWConversationMain::InitRootChoiceList()
{
	_vb_rootChoice = Cast<UVerticalBox>(GetWidgetFromName(TEXT("Root_ChoiceList")));
	TESCHECK(nullptr != _vb_rootChoice);
	_vb_rootChoice->SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWConversationMain::InitChoiceSizeBoxList()
{
	for (int32 index = 1; index <= 3; ++index)
	{
		FString path = FString::Printf(TEXT("SB_Choice_%d"), index);
		USizeBox* sb_choice = Cast<USizeBox>(GetWidgetFromName(*path));
		TESCHECK(nullptr != sb_choice);
		sb_choice->SetVisibility(ESlateVisibility::Collapsed);

		_sb_choiceList.Add(sb_choice);
	}
}

void UTESUWConversationMain::InitChoiceTextList()
{
	for (int32 index = 1; index <= 3; ++index)
	{
		FString path = FString::Printf(TEXT("TXT_Choice_%d"), index);
		UTESTextBlock* txt_choice = Cast<UTESTextBlock>(GetWidgetFromName(*path));
		TESCHECK(nullptr != txt_choice);
		txt_choice->SetText(FText::FromString(TEXT("")));

		_txt_choiceTextList.Add(txt_choice);
	}
}

void UTESUWConversationMain::InitUIFXPSScreen()
{
	_uiFXPSScreenWidget = Cast<UTESUWUIFXPSScreen>(GetWidgetFromName(TEXT("UIFX_PS_Screen_Main")));
	if (true == ::IsValid(_uiFXPSScreenWidget))
	{
		_uiFXPSScreenWidget->InitUIFX();
		_uiFXPSScreenWidget->CloseUIFX();
	}
}

void UTESUWConversationMain::InitBackgroundBlur()
{
	_bb_conversationBlur = Cast<UBackgroundBlur>(GetWidgetFromName(TEXT("BB_Blur")));
	if (true == ::IsValid(_bb_conversationBlur))
	{
		_bb_conversationBlur->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UTESUWConversationMain::SetWidget()
{
	CheckSceneTranslationAfterConversation();

	PlayFirstConversation();

	SetBGM();

	SetAmbientSound();
}

void UTESUWConversationMain::SetConversationInfo()
{
	float textSpeed = 0.0f;
	textSpeed = GetCurrentTextSpeed();

	if (0.0f == textSpeed)
	{
		textSpeed = 1.0f;
	}

	_isBlocking = true;

	//. 얘기하는 대화가 아니라 이펙트 효과음과 텍스트만 나올 경우에는 스킵을 안한다.
	if (0 == _currentConversationData._characterNameId && 0 == _currentConversationData._talkId)
	{
		SharedConversationManager->SetConversationEnd();

		PlayNextConversation(_currentConversationData._nextConversationId);

		return;
	}

	ResetChoiceList();		//. 선택지를 일단 가려준다.
	SetCharacterName(_currentConversationData._characterNameId);
	SetConversation(_currentConversationData._talkId, _currentConversationData._talkPoint, _currentConversationData._talkSpeed / textSpeed);
	SetSpeakingMontage();
	Set2DCutscene();
	Set2DPopupImage(_currentConversationData._popupImageList);
	Set2DBackgroundEffectImage(_currentConversationData._2dBackgroundEffectColor);
	Set3DEffect(_currentConversationData._vfxId, _currentConversationData._vfxLocation);
	SetVoiceSound(_currentConversationData._voiceId);
	SetEffectSound(_currentConversationData._soundEffectId);
	SetPSScreenWidget(_currentConversationData._psScreenUIFXPath);

	TESLOG(Display, TEXT("Current Conversation Id : %d"), _currentConversationData._conversationTableId);
}

float UTESUWConversationMain::SetInfoNotify(const int32 infoId)
{
	//. 정보를 보여준다.
	TESCHECK(nullptr != _cp_rootInfoNotify, 0.0f);
	_cp_rootInfoNotify->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	//. 스킵버튼을 가려준다.
	TESCHECK(nullptr != _hb_rootSkip, 0.0f);
	_hb_rootSkip->SetVisibility(ESlateVisibility::Collapsed);

	//. 채팅UI를 가려준다.
	TESCHECK(nullptr != _cp_rootBottomUI, 0.0f);
	_cp_rootBottomUI->SetVisibility(ESlateVisibility::Collapsed);

	//. 이름 초기화
	TESCHECK(nullptr != _txt_characterName, 0.0f);
	_txt_characterName->SetText(FText::FromString(TEXT("")));

	//. 내용을 비워준다.
	for (int index = 0; index < _numOfNewLine; ++index)
	{
		TESCHECK(nullptr != _conversationStringWidgetList[index], 0.0f);
		_conversationStringWidgetList[index]->SetEmpty();
	}

	//. 텍스트 세팅
	TESCHECK(nullptr != _txt_infoNotify, 0.0f);
	_txt_infoNotify->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	_txt_infoNotify->SetGlobalString(infoId);

	_isShowingInfo = true;

	return PlayWGAnimation(TEXT("InfoNotify_Ani"));
}

void UTESUWConversationMain::SetCharacterLocation()
{
	//. 테이블로부터 대화하는 캐릭터 숫자
	int32 totalSpeakingChar = _currentConversationData._speakingCharacters.Num();

	if (0 >= totalSpeakingChar)
		return;

	//. 7번 돌면서 각자 위치에 맞게 배치해준다.
	for (int charIndex = 1; charIndex <= totalSpeakingChar; ++charIndex)
	{
		SharedConversationManager->SetAttachToActor(charIndex);
	}
}

float UTESUWConversationMain::SetBottomUI()
{
	TESCHECK(nullptr != _cp_rootBottomUI, 0.0f);

	if (ESlateVisibility::SelfHitTestInvisible == _cp_rootBottomUI->GetVisibility())
		return 0.0f;

	_cp_rootBottomUI->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	SetSkipButton();

	SetAutoButton();

	//OnClicked_AutoButton();

	return PlayWGAnimation(TEXT("ConversationMai_On_Ani"));
}

void UTESUWConversationMain::StartConversation(float startTime)
 {
	if (0.0f == startTime)
	{
		_isShowingInfo = false;

		SetConversationInfo();

		return;
	}

	//. 애니메이션이 끝나고 회화를 시작한다.
	GetWorld()->GetTimerManager().ClearTimer(_th_startTimer);
	GetWorld()->GetTimerManager().SetTimer(_th_startTimer, FTimerDelegate::CreateLambda([=]()
	{
		//. UI 연출 진행했는지
		_isUIOpened = true;
		_isShowingInfo = false;

		SetConversationInfo();
	}), startTime, false);
}

void UTESUWConversationMain::SetCharacterName(const int32 characterNameId)
{
	TESCHECK(nullptr != _txt_characterName);
	if (0 == characterNameId)
	{
		_txt_characterName->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	_txt_characterName->SetText(FText::FromString(TESUtilTable::GetGlobalString(GetWorld(), characterNameId)));
	_txt_characterName->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UTESUWConversationMain::SetConversation(const int32 conversationId, const int32 justification, const float animSpeed)
{
	FString context = TESUtilTable::GetConversationString(GetWorld(), conversationId);
	_splitContextArray = {};
	
	context.ParseIntoArray(OUT _splitContextArray, TEXT("\n"));

	//. 세팅하기 전에 먼저 전에 있던 내용을 비워준다.
	for (int index = 0; index < _numOfNewLine; ++index)
	{
		if (true == _conversationStringWidgetList.IsValidIndex(index))
		{
			TESCHECK(nullptr != _conversationStringWidgetList[index]);
			_conversationStringWidgetList[index]->SetEmpty();
		}
	}

	//. 개행이 몇개 있는지 카운트를 하고 개수에 따라 랩박스를 바인딩 한다.
	_numOfNewLine = _splitContextArray.Num();

	//. for문 돌려서 개수만큼만 할당해준다.
	//. 개행이 없으면 전체문장 하나 들어오고, 하나가 있으면 2개, 2개가 있으면 3개가 들어온다.
	for (int index = 0; index < _splitContextArray.Num(); ++index)
	{
		if (true == _conversationStringWidgetList.IsValidIndex(index))
		{
			TESCHECK(nullptr != _conversationStringWidgetList[index]);
			_conversationStringWidgetList[index]->SetConversationString(_splitContextArray[index], animSpeed);
		}
	}

	FVector location = {};
	ACameraActor* conversationCamera = SharedConversationManager->GetCameraActor();
	if (true == ::IsValid(conversationCamera))
	{
		//. 일단은 카메라 위치를 기준으로 한다.
		location = conversationCamera->GetActorLocation();
	}

	SetConversation(0);

	SharedLevelSequenceManager->PlayCameraShake(_currentConversationData._cameraShake, location, true);
	
	_currentPlayingLine = 0;
	_isTextAnimated = true;
}

void UTESUWConversationMain::SetConversation(const int32 currentContextIndex)
{
	//. 현재 줄이 등록되어 있는 대사의 줄보다 많으면 더이상 실행하지 않고 마커를 등록한다.
	if (currentContextIndex >= _numOfNewLine)
	{
		SetMarkers(true);

		//. 입모양도 더이상 실행시켜주지 않는다.
		//. 매니저로부터 대화하는 캐릭터들
		//. 테이블로부터 대화하는 캐릭터 숫자
		//. 카메라 이동 및 캐릭터 이동을 끝낸다.
		//. 캐릭터 애니메이션 및 모퍼값도 바꿔준다.
		SharedConversationManager->SetCharacterToEnd();

		//. 선택지가 있으면 선택지를 띄워준다.
		_isChoiceOn = SetChoiceList();
		
		return;
	}

	//. 첫번째 글씨부터 시작
	if (true == _conversationStringWidgetList.IsValidIndex(currentContextIndex))
	{
		TESCHECK(nullptr != _conversationStringWidgetList[currentContextIndex]);
		_conversationStringWidgetList[currentContextIndex]->PlayTextAnim(0);
	}
}

void UTESUWConversationMain::SetSpeakingMontage()
{
	//. 테이블로부터 대화하는 캐릭터 숫자
	int32 totalSpeakingChar = _currentConversationData._speakingCharacters.Num();

	if (0 >= totalSpeakingChar)
		return;

	//. 매니저로부터 대화하는 캐릭터들
	TMap<int32, ATESCharacterConversation*> conversationCharacters = SharedConversationManager->GetCurrentSpeakingCharacters();

	if (0 >= conversationCharacters.Num())
		return;

	//. 캐릭터 슬롯은 총 7개가 있지만, 실제로는 데이터는 1~4번, 5~7번까지 두개로 나뉘어진다.
	//. for문을 총 7번 돌고 그 안에 데이터가 있을 경우에만 몽타주를 실행한다.
	for (int charIndex = 1; charIndex <= totalSpeakingChar; ++charIndex)
	{
		int32 characterId = _currentConversationData._speakingCharacters[charIndex]._characterId;

		if(0 == characterId)
			continue;

		FString montage = _currentConversationData._speakingCharacters[charIndex]._montage;
		FString finalMontagePath = montage;

		//. CHR_AM_99998_EmotionKnee_Pillow
		//. CHR_AM_99999_EmotionKnee_Pillow
	
		//. 남자 / 여자 주인공을 판단한다.
		if (TESConstantValue::MaleHeroCharacterId == characterId)
		{
			//. 여자일때만 바꿔준다.
			if (GENDER_TYPE::GENDER_TYPE_FEMALE == SharedUserDataManager->GetUserGender())
			{
				finalMontagePath = TESUtilString::ChageMontagePathByGender(montage);
			}
		}

		TArray<FTESCharacterMorpher> characterMopherList = _currentConversationData._speakingCharactersMorphers[charIndex];

		//. 해당 캐릭터에 맞는 몽타주를 실행한다.
		//. 해당 캐릭터에 맞는 모퍼값도 같이 실행한다.
		if (true == conversationCharacters.Contains(charIndex))
		{
			if (nullptr != conversationCharacters[charIndex])
			{
				conversationCharacters[charIndex]->SetCharacterMorpher(characterMopherList);
				conversationCharacters[charIndex]->PlaySpeakingMontage(finalMontagePath);

				//. 캐릭터들마다 입력된 이펙트들을 적용시킨다.
				int32 vfxId = _currentConversationData._speakingCharacterEffects[charIndex]._vfxId;
				FString characterSocketName = _currentConversationData._speakingCharacterEffects[charIndex]._socketName;
				FVector relativeLocation = {};
				relativeLocation.X = _currentConversationData._speakingCharacterEffects[charIndex]._offsetX;
				relativeLocation.Y = _currentConversationData._speakingCharacterEffects[charIndex]._offsetY;
				relativeLocation.Z = _currentConversationData._speakingCharacterEffects[charIndex]._offsetZ;

				float vfxLifeTime = _currentConversationData._speakingCharacterEffects[charIndex]._lifeTime;
				SetCharacterEffect(vfxId, conversationCharacters[charIndex], characterSocketName, relativeLocation, vfxLifeTime);
			}
		}
	}
}

void UTESUWConversationMain::SetSkipButton()
{
	TESCHECK(nullptr != _btn_skip);
	_btn_skip->SetVisibility(ESlateVisibility::Visible);
}

void UTESUWConversationMain::SetAutoButton()
{
	TESCHECK(nullptr != _btn_auto);
	_btn_auto->SetVisibility(ESlateVisibility::Visible);

	TESCHECK(nullptr != _hb_rootSkip);
	_hb_rootSkip->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	switch (_autoSpeed)
	{
	case ETESAutoSpeedType::None:
		{
			_autoSpeed = ETESAutoSpeedType::None;
			SharedSaveGameManager->SetConversationControlMode(ETESAutoSpeedType::None);
			_txt_Auto->SetText(FText::FromString(TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Conversation_Button_Manual)));
			SetAutoToggleVisible(false);
		}
		break;
	case ETESAutoSpeedType::Default:
		{
			_autoSpeed = ETESAutoSpeedType::Default;
			SharedSaveGameManager->SetConversationControlMode(ETESAutoSpeedType::Default);
			TESCHECK(nullptr != _txt_Auto);
			_txt_Auto->SetText(FText::FromString(TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Conversation_Button_Auto)));
			SetAutoToggleVisible(true);
			
		}
		break;
	case ETESAutoSpeedType::Double:
		{
			_autoSpeed = ETESAutoSpeedType::Double;
			SharedSaveGameManager->SetConversationControlMode(ETESAutoSpeedType::Double);
			TESCHECK(nullptr != _txt_Auto);
			_txt_Auto->SetText(FText::FromString(TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Conversation_Button_Autox2)));
			SetAutoToggleVisible(true);
		}
		break;
	case ETESAutoSpeedType::Triple:
		{
			_autoSpeed = ETESAutoSpeedType::Triple;
			SharedSaveGameManager->SetConversationControlMode(ETESAutoSpeedType::Triple);
			TESCHECK(nullptr != _txt_Auto);
			_txt_Auto->SetText(FText::FromString(TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Conversation_Button_Autox3)));
			SetAutoToggleVisible(true);
		}
		break;
	default:
		{
			TESLOG(Error, TEXT("There is no auto speed Type"));
		}
		break;
	}
}

void UTESUWConversationMain::SetMarkers(bool setShow)
{
	if (true == setShow)
	{
		TESCHECK(nullptr != _img_triangleMarker);
		_img_triangleMarker->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		TESCHECK(nullptr != _img_rectangleMarker);
		_img_rectangleMarker->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		//. 루프 애니메이션 재생
		PlayWGAnimation("Loop", 0.0f, 0);
	}
	else
	{
		TESCHECK(nullptr != _img_triangleMarker);
		_img_triangleMarker->SetVisibility(ESlateVisibility::Collapsed);

		TESCHECK(nullptr != _img_rectangleMarker);
		_img_rectangleMarker->SetVisibility(ESlateVisibility::Collapsed);

		StopWGAnimation("Loop");

		//. 현재 오토 상태에 따라 넘어가는 시간을 세팅 해준다.
		switch (_autoSpeed)
		{
		case ETESAutoSpeedType::Default:
			_leftTimeForNextConversation = 2.0f;
			break;
		case ETESAutoSpeedType::Double:
			_leftTimeForNextConversation = 1.0f;
			break;
		case ETESAutoSpeedType::Triple:
			_leftTimeForNextConversation = 0.75f;
			break;
		default:
			{
				//TESLOG(Error, TEXT("No auto type"));
			}
			break;
		}
	}
}

void UTESUWConversationMain::Set2DCutscene()
{
	TESCHECK(nullptr != _img_2dCutscene);
	TESCHECK(nullptr != _img_rectBox);

	GENDER_TYPE genderType = SharedUserDataManager->GetUserGender();
	switch (genderType)
	{
	case GENDER_TYPE::GENDER_TYPE_MALE:
		{
			if (TEXT("") == _currentConversationData._2dCutsceneForMale)
			{
				_img_rectBox->SetVisibility(ESlateVisibility::Collapsed);
				_img_2dCutscene->SetVisibility(ESlateVisibility::Collapsed);
				return;
			}

			FString resourcePath = TESUtilPath::Get2DCutsceneImagePath(_currentConversationData._2dCutsceneForMale);
			_img_2dCutscene->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			_img_rectBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			TESUtilTexture::SetDynamicTexture(_img_2dCutscene, resourcePath);
		}
		break;
	case GENDER_TYPE::GENDER_TYPE_FEMALE:
		{
			if (TEXT("") == _currentConversationData._2dCutsceneForFeMale)
			{
				_img_2dCutscene->SetVisibility(ESlateVisibility::Collapsed);
				_img_rectBox->SetVisibility(ESlateVisibility::Collapsed);
				return;
			}

			FString resourcePath = TESUtilPath::Get2DCutsceneImagePath(_currentConversationData._2dCutsceneForFeMale);

			_img_2dCutscene->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			_img_rectBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			TESUtilTexture::SetDynamicTexture(_img_2dCutscene, resourcePath);
		}
		break;
	default:
		{
			if (TEXT("") == _currentConversationData._2dCutsceneForFeMale)
			{
				_img_2dCutscene->SetVisibility(ESlateVisibility::Collapsed);
				_img_rectBox->SetVisibility(ESlateVisibility::Collapsed);
				return;
			}

			FString resourcePath = TESUtilPath::Get2DCutsceneImagePath(_currentConversationData._2dCutsceneForFeMale);

			_img_2dCutscene->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			_img_rectBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			TESUtilTexture::SetDynamicTexture(_img_2dCutscene, resourcePath);
		}
	}
}

void UTESUWConversationMain::Set2DPopupImage(TArray<FTESConversationPopupImage> popupImageList)
{
	//. 현재는 이미지 하나만 있지만, 추후에 연출을 넣기 위해서는 wbp로 만들어서 애니메이션까지 재생 시켜야한다.
	for (int32 index = 0; index < popupImageList.Num(); ++index)
	{
		if (true == popupImageList.IsValidIndex(index))
		{
			if (true == _img_2dPopupList.IsValidIndex(index) && true == _img_2dPopupDecoList.IsValidIndex(index))
			{
				if (TEXT("") == popupImageList[index]._resourceName)
				{
					_img_2dPopupList[index]->SetVisibility(ESlateVisibility::Collapsed);
					_img_2dPopupDecoList[index]->SetVisibility(ESlateVisibility::Collapsed);
				}
				else
				{
					FString resourcePath = TESUtilPath::Get2DCutsceneImagePath(popupImageList[index]._resourceName);
					FVector2D imageLocation = FVector2D(popupImageList[index]._x, popupImageList[index]._y);

					_img_2dPopupList[index]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
					_img_2dPopupDecoList[index]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
					_img_2dPopupList[index]->SetRenderTranslation(imageLocation);							//. 이미지 좌표 설정
					_img_2dPopupList[index]->SetBrushSize(FVector2D(popupImageList[index]._sizeX, popupImageList[index]._sizeY));
					_img_2dPopupDecoList[index]->SetRenderTranslation(imageLocation);
					_img_2dPopupDecoList[index]->SetBrushSize(FVector2D(popupImageList[index]._sizeX, popupImageList[index]._sizeY));

					TESUtilTexture::SetDynamicTexture(_img_2dPopupList[index], resourcePath);				//. 이미지 교체
				}
			}
		}
	}
}

void UTESUWConversationMain::SetBGM()
{
	if (0 >= _currentConversationData._bgmId)
		return;
	
	//SharedSoundManager->FadeOutBGM();
	SharedSoundManager->PlayBGM(GetWorld(), _currentConversationData._bgmId);
}

void UTESUWConversationMain::SetAmbientSound()
{
	if (0 >= _currentConversationData._ambientSoundId)
		return;

	//SharedSoundManager->FadeOutBGM();
	SharedSoundManager->PlayAmbientSound(GetWorld(), _currentConversationData._ambientSoundId);
}

void UTESUWConversationMain::SetAutoToggleVisible(bool isVisible)
{
	TESCHECK(nullptr != _img_autoToggle);
	_img_autoToggle->SetVisibility(true == isVisible ? 
									ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UTESUWConversationMain::SetBackgroundImage()
{
	TESCHECK(nullptr != _backgroundActor);

	FString resourcePath = TESUtilPath::GetBackgroundImagePath(_currentConversationData._backgroundResourceName);

	if (FString() == resourcePath)
	{
		return;
	}

	UMaterialInterface* materialInterface = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, *resourcePath));
	TESCHECK(nullptr != materialInterface);
	_backgroundActor->GetStaticMeshComponent()->SetMaterial(0, materialInterface);
}

void UTESUWConversationMain::SetBackgroundLocation()
{
	TESCHECK(nullptr != _backgroundActor);

	//. 위치 초기화.
	/*_backgroundActor->SetActorLocation(FVector(-140.0f, -454.0f, 500140.0f));
	_backgroundActor->SetActorRotation(FRotator(0.0f, 0.0f, 90.0f));*/

	if (FVector::ZeroVector == _currentConversationData._backgroundLocation)
		return;
	
	_backgroundActor->SetActorLocation(_currentConversationData._backgroundLocation);
}

void UTESUWConversationMain::SetBackgroundRotation()
{
	if (FRotator::ZeroRotator == _currentConversationData._backgroundRotation)
		return;

	//. 회전값 변경.(기획팀에서 회전값을 xyz순으로 하고 싶어함)
	FRotator temp = {};
	temp.Pitch = _currentConversationData._backgroundRotation.Roll;
	temp.Yaw = _currentConversationData._backgroundRotation.Pitch;
	temp.Roll = _currentConversationData._backgroundRotation.Yaw;

	TESCHECK(nullptr != _backgroundActor);
	_backgroundActor->SetActorRotation(temp);
}

void UTESUWConversationMain::Set2DBackgroundEffectImage(FVector4D imageEffect)
{
	TESCHECK(nullptr != _img_2dBackgroundEffect);
	if (0.0f == imageEffect._a)
	{
		_img_2dBackgroundEffect->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	FLinearColor color = {};
	color.R = imageEffect._r;
	color.G = imageEffect._g;
	color.B = imageEffect._b;
	color.A = imageEffect._a;

	_img_2dBackgroundEffect->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	_img_2dBackgroundEffect->SetColorAndOpacity(color);
}

void UTESUWConversationMain::Set3DEffect(int32 vfxTableId, FVector vfxLocation)
{
	if (0 == vfxTableId)
		return;

	FVector effectLocation = TESConstantValue::ConversationEffectDefaultLocation + vfxLocation;

	_sceneVFX = SharedVFXManager->PerformEffect(vfxTableId, effectLocation, FRotator(), TESConstantValue::Conversation3DEffectLoopTime);
}

void UTESUWConversationMain::SetCharacterEffect(int32 vfxTableId, ATESCharacterConversation* character, FString socketName, FVector relativeLocation, float lifeTime)
{
	if (0 >= vfxTableId)
		return;

	ATESVFX* characterVFX = SharedVFXManager->PerformEffect(vfxTableId, character, socketName, relativeLocation, lifeTime);
	TESCHECK(nullptr != characterVFX);

	_characterVFXList.Add(characterVFX);
}

void UTESUWConversationMain::SetRootChoice(bool isVisible)
{
	TESCHECK(nullptr != _vb_rootChoice);
	_vb_rootChoice->SetVisibility(true == isVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

bool UTESUWConversationMain::SetChoiceList()
{
	bool IsEnabled = false;
	SetRootChoice(IsEnabled);
	
	//. 하나의 선택지.
	for (int32 index = 0; index < _currentConversationData._dialougeChoiceList.Num(); ++index)
	{
		if (true == _currentConversationData._dialougeChoiceList.IsValidIndex(index))
		{
			if (0 != _currentConversationData._dialougeChoiceList[index]._globalstring)
			{
				IsEnabled = true;
				SetRootChoice(IsEnabled);

				if (true == _sb_choiceList.IsValidIndex(index) && true == _txt_choiceTextList.IsValidIndex(index))
				{
					if (true == ::IsValid(_sb_choiceList[index]) && true == ::IsValid(_txt_choiceTextList[index]))
					{
						_sb_choiceList[index]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
						_txt_choiceTextList[index]->SetGlobalStringConversation(_currentConversationData._dialougeChoiceList[index]._globalstring);
					}
				}
			}
			else
			{
				if (true == _sb_choiceList.IsValidIndex(index))
				{
					if (true == ::IsValid(_sb_choiceList[index]))
					{
						_sb_choiceList[index]->SetVisibility(ESlateVisibility::Collapsed);
					}
				}
			}
		}
	}

	//. 선택지가 한개라도 있으면 위젯 애님을 실행시킨다.
	if (true == IsEnabled)
	{
		PlayWGAnimation(TEXT("ChoiceList_On_Ani"));
	}

	return IsEnabled;
}

void UTESUWConversationMain::ResetChoiceList()
{
	//. 전부 꺼준다.
	SetRootChoice(false);
}

void UTESUWConversationMain::SetPSScreenWidget(FString resourcePath)
{
	if (FString() == resourcePath)
	{
		_uiFXPSScreenWidget->CloseUIFX();
		_uiFXPSScreenWidget->ClearUIFX();
		return;
	}

	if (true == ::IsValid(_uiFXPSScreenWidget))
	{
		_uiFXPSScreenWidget->InitUIFX();
		_uiFXPSScreenWidget->OpenUIFX();
		_uiFXPSScreenWidget->SetUIFXPSScreen(resourcePath);
	}
}

void UTESUWConversationMain::SetVoiceSound(int32 soundId)
{
	if (0 == soundId)
		return;

	SharedSoundManager->Play2DSound(GetWorld(), soundId);
}

void UTESUWConversationMain::SetEffectSound(int32 soundId)
{
	if (0 == soundId)
		return;

	SharedSoundManager->Play2DSound(GetWorld(), soundId);
}

void UTESUWConversationMain::SetBackGroundInfo()
{
	if (0 != _currentConversationData._conversationTableId)
	{
		//. 현재 진행할 회화 아이디 세팅
		SharedConversationManager->SetCurrentConversationId(_currentConversationData._conversationTableId);

		//. 바뀐 데이터가 있으면 세팅을 다시 해준다.
		SharedConversationManager->UpdateConversation(_currentConversationData._conversationTableId);
	}
	else
	{
		return;
	}

	//. 배경을 미리 바꿔준다.
	SetBackgroundImage();
	SetBackgroundLocation();
	SetBackgroundRotation();
	SetBackGroundBlur(_currentConversationData._backgroundBlurValue);
}

void UTESUWConversationMain::SetBackGroundBlur(float blurValue)
{
	if (true == ::IsValid(_bb_conversationBlur))
	{
		if (0 >= blurValue)
		{
			_bb_conversationBlur->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			_bb_conversationBlur->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			_bb_conversationBlur->SetBlurStrength(blurValue);
		}
	}
}

float UTESUWConversationMain::GetCurrentTextSpeed()
{
	float textSpeed = 0.0f;

	switch (_autoSpeed)
	{
	case ETESAutoSpeedType::None:
		{
			if (ETESAutoSpeedType::None == _autoSpeed)
			{
				//. 수동 상태일때는 기본 속도로 설정
				textSpeed = 1.0f;
			}
		}
		break;
	case ETESAutoSpeedType::Default:
		{
			textSpeed = static_cast<float>(_autoSpeed);
		}
		break;
	case ETESAutoSpeedType::Double:
		{
			textSpeed = static_cast<float>(_autoSpeed);
		}
		break;
	case ETESAutoSpeedType::Triple:
		{
			textSpeed = static_cast<float>(_autoSpeed);
		}
		break;
	default:
		{
			TESLOG(Error, TEXT("There is No Type For Auto Speed"));
		}
		break;
	}

	return textSpeed;
}

void UTESUWConversationMain::OnEvent_ScreenTouched(FVector touchLocation)
{
	if (true == _isShowingInfo)
		return;

	if (true == _isFadeInOut)
		return;

	if (true == _isBlocking)
		return;

	//. 텍스트 애니메이션이 재생 중이라면...
	if (true == CheckPrintingText())
	{
		for (int index = 0; index < _numOfNewLine; ++index)
		{
			if (true == _conversationStringWidgetList.IsValidIndex(index))
			{
				TESCHECK(nullptr != _conversationStringWidgetList[index]);

				_conversationStringWidgetList[index]->StopTextAnim();
			}
		}

		SetMarkers(true);

		//. 카메라 이동 및 캐릭터 이동을 끝낸다.
		//. 캐릭터 애니메이션 및 모퍼값도 바꿔준다.
		SharedConversationManager->SetConversationEnd();

		//. 선택지가 있으면 선택지를 켜준다.
		_isChoiceOn = SetChoiceList();
	}
	else
	{
		//. 선택지가 없을때는 다음 대사로 진행
		if (false == _isChoiceOn)
		{
			//. 텍스트 애니메이션이 진행되었으면 다음 대사로 넘어간다.
			if (true == _isTextAnimated)
			{
				//. 다음회화가 바로 다음이 아니라 점프할 아이디가 현재 아이디보다 크면 점프. 아니면 다음껄로 실행
				if (_currentConversationId > _nextConversationId)
				{
					PlayNextConversation(_currentConversationData._nextConversationId);
				}
				else
				{
					PlayNextConversation(_nextConversationId);
				}
			}
		}
		//. 선택지가 있으면 막아준다.
		else
		{
			return;
		}
	}
}

ETESAutoSpeedType UTESUWConversationMain::GetAutoSpeed() const
{
	return _autoSpeed;
}

void UTESUWConversationMain::CheckSceneTranslationAfterConversation()
{
	_fullConversationDataById = SharedConversationManager->GetConversationDataMap();

	TArray<int32> keyList = {};

	if (0 >= _fullConversationDataById.Num())
		return;

	_fullConversationDataById.GenerateKeyArray(keyList);

	if (0 >= keyList.Num())
		return;

	int32 lastIndex = keyList.Num() - 1;
	FString nextSceneId = TEXT("");
	if (true == _fullConversationDataById.Contains(keyList[lastIndex]))
	{
		nextSceneId = _fullConversationDataById.Find(keyList[lastIndex])->_sceneId;
	}

	//. 마지막 대사에 씬 이동이 있는지 체크하고 있으면 해당 내용을 저장한다.
	if (TEXT("") != nextSceneId)
	{
		_doesHaveSceneTranslation = true;
		_nextSceneName = nextSceneId;
	}
	else
	{
		_doesHaveSceneTranslation = false;
		_nextSceneName = TEXT("");
	}
}

void UTESUWConversationMain::PlayFirstConversation()
{
	if (true == SharedConversationManager->CheckIfPossibleToPlay())
	{
		_currentConversationData = SharedConversationManager->GetFirstConversationData();
		_currentConversationId = _currentConversationData._conversationTableId;

		ETESAutoSpeedType conversationMode = SharedSaveGameManager->GetConversationControlMode();
		switch (conversationMode)
		{
		case ETESAutoSpeedType::None:
			{
				_autoSpeed = ETESAutoSpeedType::None;
			}
			break;
		case ETESAutoSpeedType::Default:
			{
				_autoSpeed = ETESAutoSpeedType::Default;
			}
			break;
		case ETESAutoSpeedType::Double:
			{
				_autoSpeed = ETESAutoSpeedType::Double;
			}
			break;
		case  ETESAutoSpeedType::Triple:
			{
				_autoSpeed = ETESAutoSpeedType::Triple;
			}
			break;
		default:
			{
				_autoSpeed = ETESAutoSpeedType::None;
			}
			break;
		}


		if (0 >= _currentConversationId)
		{
			TESLOG(Error, TEXT("Wrong conversation Id"));
			return;
		}
	}

	SetBackGroundInfo();
	PlayCurrentConversation();
}

void UTESUWConversationMain::PlayCurrentConversation()
{
	if (0 == _currentConversationData._conversationTableId)
		return;

	//. 대화가 처음 시작할때 안내 문구가 있으면 실행시킨다.
	if (0 != _currentConversationData._infoStringId)
	{
		float startNotifyTime = SetInfoNotify(_currentConversationData._infoStringId);
		
		float delayTime = 1.5f;

		_tempAutoSpeed = _autoSpeed;
		_autoSpeed = ETESAutoSpeedType::None;

		TESLOG(Display, TEXT("Current Conversation Id : %d"), _currentConversationData._conversationTableId);

		GetWorld()->GetTimerManager().ClearTimer(_th_playingTimer);
		GetWorld()->GetTimerManager().SetTimer(_th_playingTimer, FTimerDelegate::CreateLambda([&]()
		{
			_autoSpeed = _tempAutoSpeed;

			float startConversationTime = SetBottomUI();

			StartConversation(startConversationTime);

			TESCHECK(nullptr != _cp_rootInfoNotify);
			_cp_rootInfoNotify->SetVisibility(ESlateVisibility::Collapsed);
		}), startNotifyTime + delayTime, false);
	}
	else
	{
		if (true == _isUIOpened)
		{
			//. 한번이라도 UI가 세팅이 되어있으면 다른 연출이 필요 없이 대화를 진행한다.
			//. 안내문구가 따로 없으면 바로 회화를 시작한다.
			SetConversationInfo();
		}
		else
		{
			//. 노티파이 없이 처음 대화가 진행하면 UI 연출후 진행한다.
			float startConversationTime = SetBottomUI();

			StartConversation(startConversationTime);
		}
	}
}

void UTESUWConversationMain::PlayNextConversation(int32 nextConversationId)
{
	if (true == _isShowingInfo)
		return;

	if (true == _isFadeInOut)
		return;

	//. 스토리 스킵 팝업이 띄워져 있으면 다음 대사로 못넘어간다.
	int32 popupCount = SharedPopupManager->GetPopupWidgetCount();
	if (1 <= popupCount)
	{
		return;
	}

	if (0 == nextConversationId)
	{
		EndConversation();
		return;
	}

	//TESLOG(Error, TEXT("next conversation id : %d"), nextConversationId);

	FTESConversationData beforeConversationData = _currentConversationData;

	_currentConversationData = SharedConversationManager->GetConversationDataById(nextConversationId);

	_currentConversationId = nextConversationId;

	_nextConversationId = 0;

	//. 더이상 대사 및 대화가 없을 경우에는 회화를 끝내준다.
	if (0 == _currentConversationData._conversationTableId)
	{
		EndConversation();
		return;
	}
	
	if (TEXT("") == beforeConversationData._conversationIntervalFadeOut)
	{
		//. 캐릭터 이펙트를 지워준다.
		DestroyCharacterEffect();

		DestroySceneEffect();

		Hide2DPopupImage();

		//. 다음 대사로 가는 조건이 다 만족하면 다음 대사를 실행한다.
		SetBackGroundInfo();
		PlayCurrentConversation();

		//. BGM이 바뀔수도 있다.
		SetBGM();

		//. 환경음도 바꿔준다.
		SetAmbientSound();

		SetMarkers(false);
	}
	else
	{
		float fadeOutTime = 0.0f;

		if (nullptr == _fadeInOutWidget)
		{
			_fadeInOutWidget = SharedUIManager->CreateGenericWidget<UTESUWConversationSceneChange>(TEXT("WBP_FadeInOutSet"), TEXT("Common/WBP"));
			if (true == ::IsValid(_fadeInOutWidget))
			{
				_fadeInOutWidget->InitWidget();
				_fadeInOutWidget->OpenWidget();
				_fadeInOutWidget->SetWidget();
				_fadeInOutWidget->AddToViewport(static_cast<int32>(ETESWidgetLayerType::UIPopup));
				fadeOutTime = _fadeInOutWidget->PlaySceneFadeOut(beforeConversationData._conversationIntervalFadeOut);
				fadeOutTime = FMath::Clamp(fadeOutTime, 0.001f, fadeOutTime);

				TESLOG(Error, TEXT("fade out time : %f"), fadeOutTime);
				
				_isFadeInOut = true;

				//. 페이드 아웃 할때 BGM도 같이 해준다.
				if (0 != _currentConversationData._bgmId)
				{
					SharedSoundManager->FadeOutBGM(beforeConversationData._bgmFadeOutTime);
					SharedSoundManager->StopAmbientSound();
				}

				//. Fade In / Out 연출이 있으면 연출을 전부 한 후에 실행한다.
				//. 애니메이션이 끝나고 회화를 시작한다.
				GetWorld()->GetTimerManager().ClearTimer(_th_fadeOutAction);
				GetWorld()->GetTimerManager().SetTimer(_th_fadeOutAction, FTimerDelegate::CreateLambda([=]()
					{
						float fadeInTime = 0.0f;

						if (true == ::IsValid(_fadeInOutWidget))
						{
							
							fadeInTime = _fadeInOutWidget->PlaySceneFadeIn(_currentConversationData._conversationIntervalFadeIn);
							fadeInTime = FMath::Clamp(fadeInTime, 0.001f, fadeInTime);

							//. 캐릭터 이펙트를 지워준다.
							DestroyCharacterEffect();

							//. 스크린 이펙트도 지워준다.
							DestroySceneEffect();

							Hide2DPopupImage();

							//. 다음 대사로 가는 조건이 다 만족하면 다음 대사를 실행한다.
							SetBackGroundInfo();
							PlayCurrentConversation();

							GetWorld()->GetTimerManager().ClearTimer(_th_fadeInAction);

							SetMarkers(false);

							GetWorld()->GetTimerManager().SetTimer(_th_fadeInAction, FTimerDelegate::CreateLambda([=]()
								{
									//. BGM이 바뀔수도 있다.
									SetBGM();

									//. 환경음도 바꿔준다.
									SetAmbientSound();

									//. 페이드 인 아웃이 전부 끝나면 클릭으로 넘어갈수 있게 해준다.
									_isFadeInOut = false;

									_fadeInOutWidget->CloseWidget();
									_fadeInOutWidget->ClearWidget();
									_fadeInOutWidget->RemoveFromViewport();
									SharedUIManager->DestroyGenericWidget(_fadeInOutWidget);
									_fadeInOutWidget = nullptr;

								}), fadeInTime + 1.0f, false);
						}
					}), fadeOutTime, false);
			}
		}
		
	}
}

void UTESUWConversationMain::EndConversation()
{
	//. fade in / out
	if (true == _isBlocking)
		return;

	//. 대화내용을 전부 지워준다.
	_fullConversationDataById.Empty();
	_currentConversationData.Reset();

	//. 회화창을 정리해준다.
	for (int index = 0; index < _numOfNewLine; ++index)
	{
		if (true == _conversationStringWidgetList.IsValidIndex(index))
		{
			_conversationStringWidgetList[index]->SetEmpty();
		}
	}

	DestroySceneEffect();

	DestroyCharacterEffect();

	Hide2DPopupImage();

	GoToNextScene();
}

void UTESUWConversationMain::GoToNextScene()
{
	if (true == _isSceneChanging)
		return;

	//. 회화 레벨로 들어 왔을 경우에는 바로 종료 시켜준다.
	if (false == SharedSceneManager->IsRegularRoute() || true == SharedSceneManager->IsDevelopHelperJumping())
	{
		TESUtils::QuitGameApp(GetWorld());

		return;
	}

	//. 회화에서 다음으로 연결되는 씬이 있으면 그 씬으로 이동 시켜준다.
	if (true == _doesHaveSceneTranslation)
	{
		if (TEXT("") != _nextSceneName)
		{
			_nextSceneName = _nextSceneName.Replace(TEXT("Scene"), TEXT(""));

			ETESPhysicalGameSceneType physicalSceneType = TESUtilString::GetStringAsEnum<ETESPhysicalGameSceneType>(TEXT("ETESPhysicalGameSceneType"), _nextSceneName);
			if (ETESPhysicalGameSceneType::None != physicalSceneType)
			{
				SharedSoundManager->FadeOutBGM(TESConstantValue::DefaultBGMFadeOutTime);
				SharedSoundManager->StopAmbientSound();
				SharedConversationManager->SetToNextScene(physicalSceneType);

				_isSceneChanging = true;
			}
		}
	}
	else
	{
		SharedConversationManager->EndConversation();
		//SharedUIManager->DestroyGenericWidget(this);
	}
}

void UTESUWConversationMain::Hide2DPopupImage()
{
	for (int32 index = 0; index < _currentConversationData._popupImageList.Num(); ++index)
	{
		if (true == _currentConversationData._popupImageList.IsValidIndex(index))
		{
			if (true == _img_2dPopupList.IsValidIndex(index) && true == _img_2dPopupDecoList.IsValidIndex(index))
			{
				_img_2dPopupList[index]->SetVisibility(ESlateVisibility::Collapsed);
				_img_2dPopupDecoList[index]->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
}

void UTESUWConversationMain::PlayConversationByConversationId(int32 conversationId, int32 nextConversationId)
{
	//. 한번에 점프 하는 경우.
	_currentConversationId = conversationId;
	_nextConversationId = nextConversationId;
	PlayNextConversation(_currentConversationId);
}

void UTESUWConversationMain::PlayNextLine()
{
	//. 다음줄을 실행하기 위해 개행시켜준다.
	++_currentPlayingLine;

	//. 다음줄에 있는 대사 실행
	SetConversation(_currentPlayingLine);
}

void UTESUWConversationMain::OnEvent_EnterBattle()
{
	SharedStageManager->Reset();
	SharedStageManager->SetUseGameFlow(true);
	FTESConversationInitData initData = SharedConversationManager->GetConversationInitData();
	SharedSaveGameManager->SetLastPlayedMainStoryStage(initData._stageId);
	SharedStageManager->SetStage(initData._stageId);

	FTESSceneTransitionData sceneTransitionData = {};
	sceneTransitionData._physicalSceneType = ETESPhysicalGameSceneType::LoadingBattle;
	sceneTransitionData._logicalSceneType = ETESLogicalGameSceneType::LoadingBattle;
	sceneTransitionData._fadeAtrribute._fadeType = ETESSceneFadeType::ConversationSceneChange;
	sceneTransitionData._fadeAtrribute._widgetFadeInAnimName = _currentConversationData._sceneTransitionOut._fadeIn;
	sceneTransitionData._fadeAtrribute._widgetFadeOutAnimName = _currentConversationData._sceneTransitionOut._fadeOut;
	SharedSceneManager->ChangeGameScene(sceneTransitionData);
}

//void UTESUWConversationMain::OnEvent_EndBattle(FTESBattleResultData battleResultData)
//{
//	//. 로비로 강제 이동.
//	FTESSceneTransitionData sceneTransitionData = {};
//	sceneTransitionData._physicalSceneType = ETESPhysicalGameSceneType::LoadingLobby;
//	sceneTransitionData._logicalSceneType = ETESLogicalGameSceneType::LoadingLobby;
//	sceneTransitionData._fadeAtrribute._fadeType = ETESSceneFadeType::WidgetAnimation;
//	sceneTransitionData._fadeAtrribute._fadeColor = FColor::Black;
//
//	SharedSceneManager->ChangeGameScene(sceneTransitionData);
//}

bool UTESUWConversationMain::CheckPrintingText()
{
	for (int index = 0; index < _numOfNewLine; ++index)
	{
		if (true == _conversationStringWidgetList.IsValidIndex(index))
		{
			TESCHECK(nullptr != _conversationStringWidgetList[index], false);

			if (true == _conversationStringWidgetList[index]->IsPlayingTextAnimation())
			{
				return true;
			}
		}
	}

	return false;
}

void UTESUWConversationMain::TickAutoToggle(float deltaTime)
{
	switch (_autoSpeed)
	{
	case ETESAutoSpeedType::None:
		{
			//. 수동 상태일때는 리턴
			return;
		}
		break;
	case ETESAutoSpeedType::Default:
		{
			if (true == CheckPrintingText())
			{
				return;
			}
			//. 텍스트 애니메이션이 끝나면 2초 카운트 후 넘어간다.
			else
			{
				if (0.0f < _leftTimeForNextConversation)
				{
					_leftTimeForNextConversation -= deltaTime;

					if (0.0f >= _leftTimeForNextConversation)
					{
						if (false == _isChoiceOn)
						{
							_leftTimeForNextConversation = 2.0f;

							//. 다음회화가 바로 다음이 아니라 점프할 아이디가 현재 아이디보다 크면 점프. 아니면 다음껄로 실행
							if (_currentConversationId > _nextConversationId)
							{
								PlayNextConversation(_currentConversationData._nextConversationId);
							}
							else
							{
								PlayNextConversation(_nextConversationId);
							}
						}
					}
				}
			}
		}
		break;
	case ETESAutoSpeedType::Double:
		if (true == CheckPrintingText())
		{
			return;
		}
		//. 텍스트 애니메이션이 끝나면 1초 카운트 후 넘어간다.
		else
		{
			if (0.0f < _leftTimeForNextConversation)
			{
				_leftTimeForNextConversation -= deltaTime;

				if (0.0f >= _leftTimeForNextConversation)
				{
					if (false == _isChoiceOn)
					{
						_leftTimeForNextConversation = 1.0f;

						//. 다음회화가 바로 다음이 아니라 점프할 아이디가 현재 아이디보다 크면 점프. 아니면 다음껄로 실행
						if (_currentConversationId > _nextConversationId)
						{
							PlayNextConversation(_currentConversationData._nextConversationId);
						}
						else
						{
							PlayNextConversation(_nextConversationId);
						}
					}
				}
			}
		}
		break;
	case ETESAutoSpeedType::Triple:
		if (true == CheckPrintingText())
		{
			return;
		}
		//. 텍스트 애니메이션이 끝나면 0.75초 카운트 후 넘어간다.
		else
		{
			if (0.0f < _leftTimeForNextConversation)
			{
				_leftTimeForNextConversation -= deltaTime;

				if (0.0f >= _leftTimeForNextConversation)
				{
					if (false == _isChoiceOn) 
					{
						_leftTimeForNextConversation = 0.75f;

						//. 다음회화가 바로 다음이 아니라 점프할 아이디가 현재 아이디보다 크면 점프. 아니면 다음껄로 실행
						if (_currentConversationId > _nextConversationId)
						{
							PlayNextConversation(_currentConversationData._nextConversationId);
						}
						else
						{
							PlayNextConversation(_nextConversationId);
						}
					}
				}
			}
		}
		break;
	default:
		{
			TESLOG(Error, TEXT("Error auto speed type"));
		}
		break;
	}
}

void UTESUWConversationMain::TickSkipAvailable(float deltaTime)
{
	if (false == _isBlocking)
		return;

	_textSkipDelayTime += deltaTime;

	if (TESConstantValue::ConversationSkipDelayTime <= _textSkipDelayTime)
	{
		_textSkipDelayTime = 0.0f;
		_isBlocking = false;
		return;
	}
}

void UTESUWConversationMain::DestroySceneEffect()
{
	if (nullptr != _sceneVFX)
	{
		_sceneVFX->HideAtWorld();
		_sceneVFX = nullptr;
	}

	if (true == ::IsValid(_uiFXPSScreenWidget))
	{
		_uiFXPSScreenWidget->CloseUIFX();
		_uiFXPSScreenWidget->ClearUIFX();
	}
}

void UTESUWConversationMain::DestroyCharacterEffect()
{
	for (ATESVFX* vfxList : _characterVFXList)
	{
		vfxList->HideAtWorld();
		vfxList = nullptr;
	}

	_characterVFXList.Empty();
}

void UTESUWConversationMain::OnClicked_PrevConversation()
{
	//. 이전 대화보기 기능은 현재 들어가지 않는다.
	//TESLOG_S(Error);
}

void UTESUWConversationMain::OnClicked_AutoButton()
{
	int32 autoSpeed = static_cast<int32>(_autoSpeed);

	//. 3배속에서 클릭했을때만 수동으로 바꿔주고, 나머지 상황에서는 +1씩 올려준다.
	if (_autoSpeed == ETESAutoSpeedType::Triple || _autoSpeed == ETESAutoSpeedType::Max)
	{
		_autoSpeed = ETESAutoSpeedType::None;
	}
	else
	{
		autoSpeed += 1;
		_autoSpeed = static_cast<ETESAutoSpeedType>(autoSpeed);
	}

	SetAutoButton();
}

void UTESUWConversationMain::OnClicked_SkipConversation()
{
	if (true == _isBlocking)
		return;

	if (0 == _currentConversationData._skipSummaryId)
	{
		EndConversation();
	}
	else
	{
		UTESUWStorySkipPopup* skipPopup = SharedPopupManager->CreatePopupWidget<UTESUWStorySkipPopup>(TEXT("WBP_StorySkipPopup"), TEXT("Dialogue/WBP"));
		if (true == ::IsValid(skipPopup))
		{
			skipPopup->InitWidget();
			skipPopup->OpenWidget();
			skipPopup->SetWidget(_currentConversationData._skipSummaryId, ETESConversationType::Conversation);
		}
	}
}

void UTESUWConversationMain::OnClicked_FirstChoice()
{
	if(true == _currentConversationData._dialougeChoiceList.IsValidIndex(0))
	{ 
		//. 첫번째 선택지를 선택하면 테이블에 입력된 회화 아이디로 점프한다.
		PlayConversationByConversationId(_currentConversationData._dialougeChoiceList[0]._conversationId,
										_currentConversationData._dialougeChoiceList[0]._nextConversationId);
	}
}

void UTESUWConversationMain::OnClicked_SecondChoice()
{
	if (true == _currentConversationData._dialougeChoiceList.IsValidIndex(1))
	{
		//. 두번째 선택지를 선택하면 테이블에 입력된 회화 아이디로 점프한다.
		PlayConversationByConversationId(_currentConversationData._dialougeChoiceList[1]._conversationId,
										_currentConversationData._dialougeChoiceList[1]._nextConversationId);
	}
}

void UTESUWConversationMain::OnClicked_ThirdChoice()
{
	if (true == _currentConversationData._dialougeChoiceList.IsValidIndex(2))
	{
		//. 세번째 선택지를 선택하면 테이블에 입력된 회화 아이디로 점프한다.
		PlayConversationByConversationId(_currentConversationData._dialougeChoiceList[2]._conversationId,
										_currentConversationData._dialougeChoiceList[2]._nextConversationId);
	}
}
