// Fill out your copyright notice in the Description page of Project Settings.
#include "Contents/Intro/UI/TESUWIntroMain.h"
#include "Contents/Intro/UI/TESUWIntroPresetBase.h"
#include "Contents/Intro/UI/TESUWIntroPresetLanguage.h"
#include "Contents/Intro/UI/TESUWIntroPresetLogin.h"
#include "Contents/Common/Manager/TESPopupManager.h"
#include "Contents/Common/Manager/TESUIManager.h"
#include "Contents/Common/Manager/TESSceneManager.h"
#include "Contents/Common/UI/CustomSlates/TESImage.h"
#include "Contents/Common/UI/CustomSlates/TESTextBlock.h"
#include "Contents/Common/UI/CustomSlates/TESButton.h"
#include "Contents/Common/Manager/TESSaveGameManager.h"
#include "Contents/Common/Manager/TESUserDataManager.h"
#include "Contents/Common/Manager/TESGameResourceManager.h"
#include "Contents/Common/UI/CustomSlates/TESTextBlock.h"
#include "Contents/Common/UI/CustomSlates/TESImage.h"
#include "Contents/Common/Manager/TESLevelSequenceManager.h"
#include "Contents/Network/TESNetworkManager.h"
#include "Contents/Battle/Manager/TESStageManager.h"
#include "Contents/Intro/UI/TESUWIntroPresetBase.h"
#include "Contents/Intro/UI/TESUWIntroPresetLanguage.h"
#include "Contents/Intro/UI/TESUWIntroPresetLogin.h"
#include "Contents/Intro/UI/TESUWIntroPresetTitle.h"
#include "Contents/Patch/Manager/TESPatchManager.h"
#include "Contents/Patch/UI/TESUWPatchMain.h"
#include "Battle/Manager/TESTriggerEventManager.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/OverlaySlot.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Components/Overlay.h"
#include "TESGameInstance.h"
#include "TESUtils.h"
#include "TESUtilNetwork.h"


void UTESUWIntroMain::InitWidget()
{
	Super::InitWidget();

	InitDim();

	InitIntroRoot();

	InitIntroPresets();

	InitClientVersion();

	InitClientType();

	InitResourceVersion();

	InitTouchToStart();

	InitUserEMailAddress();

	InitHomepageButton();

	InitTwitterButton();

	InitDiscordButton();

	InitLogoutButton();

	InitExitButton();
}

void UTESUWIntroMain::ClearWidget()
{
	Super::ClearWidget();

	GetWorld()->GetTimerManager().ClearTimer(th_nextSequence);
	th_nextSequence = {};
}

void UTESUWIntroMain::OpenWidget()
{
	Super::OpenWidget();

	SetVisibility(ESlateVisibility::Visible);
}

void UTESUWIntroMain::CloseWidget()
{
	Super::CloseWidget();

	SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWIntroMain::OpenDelegate()
{
	Super::OpenDelegate();	
}

void UTESUWIntroMain::CloseDelegate()
{
	Super::CloseDelegate();	
}

void UTESUWIntroMain::InitDim()
{
	_img_dim = Cast<UTESImage>(GetWidgetFromName(TEXT("IMG_Dim")));
	TESCHECK(nullptr != _img_dim);

	_img_dim->SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWIntroMain::InitIntroRoot()
{
	_root_contents = Cast<UOverlay>(GetWidgetFromName(TEXT("Root_Contents")));
	TESCHECK(nullptr != _root_contents);
}

void UTESUWIntroMain::InitIntroPresets()
{
	TESCHECK(nullptr != _root_contents);
	_introPresetWidgetList.Empty();
	_root_contents->ClearChildren();

	int32 maxCount = _introPresetClassList.Num();
	for (int32 index = 0; index < maxCount; ++index)
	{
		UTESUWIntroPresetBase* introPresetWidget = CreateWidget<UTESUWIntroPresetBase>(GetWorld(), _introPresetClassList[index].Get());
		TESCHECK_CONTINUE(nullptr != introPresetWidget);

		introPresetWidget->InitWidget();
		introPresetWidget->CloseWidget();

		//. 프리셋 리스트에 넣어준다.
		_introPresetWidgetList.Add(introPresetWidget);

		//. 화면에 뿌려질 UI들을 붙여준다.
		_root_contents->AddChild(introPresetWidget);

		//UCanvasPanelSlot* presetWidgetSlot = Cast<UCanvasPanelSlot>(introPresetWidget->Slot);
		UOverlaySlot* presetWidgetSlot = Cast<UOverlaySlot>(introPresetWidget->Slot);
		TESCHECK_CONTINUE(nullptr != presetWidgetSlot);
		presetWidgetSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		presetWidgetSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
		/*presetWidgetSlot->SetAutoSize(true);
		presetWidgetSlot->SetAlignment(FVector2D(0.5f, 0.5f));*/
	}
}

void UTESUWIntroMain::InitClientVersion()
{
	_txt_clientVersion = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_ClientVersion")));
	TESCHECK(nullptr != _txt_clientVersion);

	_txt_clientVersion->SetText(FText::FromString(TEXT("")));
}

void UTESUWIntroMain::InitClientType()
{
	_txt_clientType = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_ClientType")));
	TESCHECK(nullptr != _txt_clientType);

	_txt_clientType->SetText(FText::FromString(TEXT("")));
}

void UTESUWIntroMain::InitResourceVersion()
{
	_txt_resourceVersion = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_ResourceVersion")));
	TESCHECK(nullptr != _txt_resourceVersion);
	_txt_resourceVersion->SetText(FText::FromString(TEXT("")));
}

void UTESUWIntroMain::InitTouchToStart()
{
	_txt_touchToStart = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_TouchToStart")));
	TESCHECK(nullptr != _txt_touchToStart);
	_txt_touchToStart->SetText(FText::FromString(TEXT("")));
}

void UTESUWIntroMain::InitUserEMailAddress()
{
	_txt_userEMailAddress = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_EmailAddress")));
	TESCHECK(nullptr != _txt_userEMailAddress);
	_txt_userEMailAddress->SetText(FText::FromString(TEXT("")));
}

void UTESUWIntroMain::InitHomepageButton()
{
	_btn_homepage = Cast<UTESButton>(GetWidgetFromName(TEXT("BTN_Homepage")));
	TESCHECK(nullptr != _btn_homepage);
	_btn_homepage->OnPressed.AddDynamic(this, &UTESUWIntroMain::OnEvent_GoToHomePage);
	_btn_homepage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UTESUWIntroMain::InitTwitterButton()
{
	_btn_twitter = Cast<UTESButton>(GetWidgetFromName(TEXT("BTN_Twitter")));
	TESCHECK(nullptr != _btn_twitter);
	_btn_twitter->OnPressed.AddDynamic(this, &UTESUWIntroMain::OnEvent_GoToTwitter);
	_btn_twitter->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UTESUWIntroMain::InitDiscordButton()
{
	_btn_discord = Cast<UTESButton>(GetWidgetFromName(TEXT("BTN_Discord")));
	TESCHECK(nullptr != _btn_discord);
	_btn_discord->OnPressed.AddDynamic(this, &UTESUWIntroMain::OnEvent_GoToDiscord);
	_btn_discord->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UTESUWIntroMain::InitLogoutButton()
{
	_btn_logout = Cast<UTESButton>(GetWidgetFromName(TEXT("BTN_Logout")));
	TESCHECK(nullptr != _btn_logout);
	_btn_logout->OnPressed.AddDynamic(this, &UTESUWIntroMain::OnEvent_LogOut);
	_btn_logout->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UTESUWIntroMain::InitExitButton()
{
	_btn_exit = Cast<UTESButton>(GetWidgetFromName(TEXT("BTN_Exit")));
	TESCHECK(nullptr != _btn_exit);
	_btn_exit->OnPressed.AddDynamic(this, &UTESUWIntroMain::OnEvent_Exit);
	_btn_exit->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UTESUWIntroMain::ClearIntroPresets()
{
	for (UTESUWIntroPresetBase* introPresetBase : _introPresetWidgetList)
	{
		if(false == ::IsValid(introPresetBase))
			continue;

		introPresetBase->CloseWidget();
		introPresetBase->ClearWidget();
	}

	_introPresetWidgetList.Empty();
}

void UTESUWIntroMain::SetWidget(int32 introPresetIndex)
{
	if (EIntroPresetType::Login == static_cast<EIntroPresetType>(introPresetIndex))
	{
		SetFinishActionCallback(FOnActionAnimFinished::CreateUObject(this, &UTESUWIntroMain::OnEvent_ShowPatchDownload));

		PlayWGAnimation(TEXT("ShowLogo"));
	}
	else
	{
		OnEvent_NextPreset(static_cast<EIntroPresetType>(introPresetIndex));
	}
}

void UTESUWIntroMain::AfterCompletePatch()
{
	UTESUWPatchMain* patchWidget = SharedUIManager->FindGenericWidget<UTESUWPatchMain>();
	SharedUIManager->DestroyGenericWidget(patchWidget);

	//. 첫번째 프리셋으로 세팅 해준다. (로그인)
	SetContents(0);

	//. 클라이언트 타입을 세팅합니다.
	FString typeName = TitleAPPTypeName;
	SetClientType(typeName);

	//. 클라이언트 버전을 세팅합니다.
	FString versionName = FString::Printf(TEXT("app v.%d"), GameDefine::APP_VERSION);
	SetClientVersion(versionName);

	//. 리소스 버전을 세팅합니다.	
	SetResourceVersion(SharedPatchManager->GetCurrentPatchVersionName());

	//. 회화에 필요한 카메라 쉐이크 에셋을 로드합니다.
	SharedLevelSequenceManager->LoadCameraShakes();
}

void UTESUWIntroMain::AfterCompleteLogin()
{
	if (true == ::IsValid(_txt_userEMailAddress))
	{
		FString accountLoginId = SharedUserDataManager->GetAccountLoginId();
		_txt_userEMailAddress->SetText(FText::FromString(*accountLoginId));
	}

	SetFinishActionCallback(FOnActionAnimFinished::CreateUObject(this, &UTESUWIntroMain::OnEvent_ShowTouchToStart));
	PlayWGAnimation(TEXT("ShowBottomButtons"));
}

void UTESUWIntroMain::SetDimVisible(bool isVisible)
{
	TESCHECK(nullptr != _img_dim);

	_img_dim->SetVisibility(true == isVisible ?
							ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UTESUWIntroMain::SetHomepageButtonVisibility(ESlateVisibility visibility)
{
	TESCHECK(nullptr != _btn_homepage);
	_btn_homepage->SetVisibility(visibility);
}

void UTESUWIntroMain::SetTwitterButtonVisibility(ESlateVisibility visibility)
{
	TESCHECK(nullptr != _btn_twitter);
	_btn_twitter->SetVisibility(visibility);
}

void UTESUWIntroMain::SetDiscordButtonVisibility(ESlateVisibility visibility)
{
	TESCHECK(nullptr != _btn_discord);
	_btn_discord->SetVisibility(visibility);
}

void UTESUWIntroMain::SetLogoutButtonVisibility(ESlateVisibility visibility)
{
	TESCHECK(nullptr != _btn_logout);
	_btn_logout->SetVisibility(visibility);
}

void UTESUWIntroMain::SetExitButtonVisibility(ESlateVisibility visibility)
{
	TESCHECK(nullptr != _btn_exit);
	_btn_exit->SetVisibility(visibility);
}

void UTESUWIntroMain::SetIntroPresets(int32 presetIndex, bool isVisibility)
{
	if (presetIndex < 0 || presetIndex >= _introPresetWidgetList.Num())
	{
		return;
	}

	TESCHECK(nullptr != _introPresetWidgetList[presetIndex]);

	if (true == isVisibility)
	{
		//. 오픈되는 위젯 세팅
		_introPresetWidgetList[presetIndex]->OpenWidget();
		_introPresetWidgetList[presetIndex]->SetWidget(this);
	}
	else
	{
		//. 닫는다.
		_introPresetWidgetList[presetIndex]->CloseWidget();
	}
}

void UTESUWIntroMain::SetContents(int32 presetIndex)
{
	if (presetIndex == _curPresetIndex)
	{
        TESLOG(Warning, TEXT("Is Same PresetIndex. CurrIndex : %d, NewIndex : %d"), _curPresetIndex, presetIndex);

		return;
	}

	//. 인덱스 교체
	_prevPresetIndex = _curPresetIndex;
	_curPresetIndex = presetIndex;

	//. 지나간 위젯은 꺼주고 새로 등장한 위젯은 켜준다.
	SetIntroPresets(_prevPresetIndex, false);
	SetIntroPresets(_curPresetIndex, true);

	if (_prevPresetIndex + 1 == static_cast<int32>(EIntroPresetType::PlayerSelection))
	{
		//HideBottomButtons();
		SetIntroPresets(_prevPresetIndex, true);
	}
}

void UTESUWIntroMain::SwitchToNextContents(int32 presetIndex)
{
	if (true == SharedSceneManager->IsSwitchingGameScene())
	{
		return;
	}

	if (true == _isSwitchingToNextContents)
	{
		return;
	}

	int32 maxPresetIndex = _introPresetWidgetList.Num();

	//. 현재 인덱스가 마지막 인덱스보다 커지면 다음 씬으로 넘어간다.
	if (presetIndex >= maxPresetIndex)
	{
		return;
	}

	SetContents(presetIndex);

	_isSwitchingToNextContents = true;

	float delayTime = 0.3f;

	//. 광클릭을 막아준다.
	GetWorld()->GetTimerManager().ClearTimer(th_nextSequence);
	GetWorld()->GetTimerManager().SetTimer(th_nextSequence, FTimerDelegate::CreateLambda([&]()
	{
		_isSwitchingToNextContents = false;
	}), delayTime, false);
}

void UTESUWIntroMain::SetClientVersion(FString clientVersion)
{
	if (true == ::IsValid(_txt_clientVersion))
	{
		_txt_clientVersion->SetText(FText::FromString(clientVersion));
	}
}

void UTESUWIntroMain::SetClientType(FString clientType)
{
	if (true == ::IsValid(_txt_clientType))
	{
		_txt_clientType->SetText(FText::FromString(clientType));
	}
}

void UTESUWIntroMain::SetResourceVersion(FString resourceVersion)
{
	if (true == ::IsValid(_txt_resourceVersion))
	{
		_txt_resourceVersion->SetText(FText::FromString(resourceVersion));
	}
}

void UTESUWIntroMain::SetTouchToStart()
{
	SetHomepageButtonVisibility(ESlateVisibility::Visible);
	SetTwitterButtonVisibility(ESlateVisibility::Visible);
	SetDiscordButtonVisibility(ESlateVisibility::Visible);
	SetLogoutButtonVisibility(ESlateVisibility::Visible);
	SetExitButtonVisibility(ESlateVisibility::Visible);
	
	if (true == ::IsValid(_txt_touchToStart))
	{
		_txt_touchToStart->SetText(FText::FromString(TEXT("Click to Start")));
		_txt_touchToStart->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	PlayWGAnimationLoop(TEXT("StartTxtBlink"));

	UTESUWIntroPresetTitle* introPresetTitleWidget = Cast<UTESUWIntroPresetTitle>(_introPresetWidgetList[_curPresetIndex]);
	if (true == ::IsValid(introPresetTitleWidget))
	{
		introPresetTitleWidget->SetColliderButtonReadied();
	}
}

void UTESUWIntroMain::HideAllIntroWidgets()
{
	int32 maxCount = _introPresetWidgetList.Num();
	for (int32 index = 0; index < maxCount; ++index)
	{
		_introPresetWidgetList[index]->CloseWidget();
	}
}

void UTESUWIntroMain::HideBottomButtons()
{
	//. 하단 버튼 막음
	SetHomepageButtonVisibility(ESlateVisibility::SelfHitTestInvisible);
	SetTwitterButtonVisibility(ESlateVisibility::SelfHitTestInvisible);
	SetDiscordButtonVisibility(ESlateVisibility::SelfHitTestInvisible);
	SetLogoutButtonVisibility(ESlateVisibility::SelfHitTestInvisible);
	SetExitButtonVisibility(ESlateVisibility::SelfHitTestInvisible);

	//. 깜빡임 애니메이션 멈춤
	StopWGAnimation(TEXT("StartTxtBlink"));

	//. 숨김 애니메이션
	PlayWGAnimation(TEXT("HideBottomButtons"));
}

void UTESUWIntroMain::OnEvent_NextPreset(EIntroPresetType introPresetType /*= EIntroPresetType::None*/)
{
	if (introPresetType == EIntroPresetType::None)
	{
		SwitchToNextContents(_curPresetIndex + 1);
	}
	else
	{
		int32 presetIndex = static_cast<int32>(introPresetType);
		SwitchToNextContents(presetIndex - 1);
	}
}

void UTESUWIntroMain::OnEvent_ShowPatchDownload()
{
#if !WITH_EDITOR	
	if (false == SharedPatchManager->GetIsFullResource()) 
	{
		if (true == SharedPatchManager->GetIsSuccessReadManifest())
		{
			if (false == SharedPatchManager->GetIsPatchComplete())
			{
				if (false == SharedPatchManager->GetIsInstalling())
				{
					SharedPatchManager->ClearInitInstallData();
					SharedPatchManager->BeginPatch();
				}
			}
			else
			{
				AfterCompletePatch();
			}
		}
		else
		{
			FSimpleDelegate readFailManifestCallbackButton = FOnTESAction::CreateLambda([this]() {
				FGenericPlatformMisc::RequestExit(false);
				});

			SharedPopupManager->ShowMessageBox(TEXT("Fail to read patch manifest"), readFailManifestCallbackButton, TEXT("확인"));
		}
	}
	else
	{
		AfterCompletePatch();
	}
#else //!WITH_EDITOR
	AfterCompletePatch();
#endif
}

void UTESUWIntroMain::OnEvent_ShowTouchToStart()
{
#if WITH_EDITOR
	SetTouchToStart();
#else
	if (true == SharedPatchManager->GetIsFullResource() ||
		true == SharedPatchManager->GetIsPatchComplete() || 
		true == SharedPatchManager->GetIsValidPatch())
	{
		SetTouchToStart();		
	}
	else
	{
		TESLOG(Error, TEXT("No FullResource, No PatchComplete, No ValidPatch"));
	}
#endif
}

void UTESUWIntroMain::OnEvent_GoToHomePage()
{
	FPlatformProcess::LaunchURL(StellaFantasyAddress, NULL, NULL);
}

void UTESUWIntroMain::OnEvent_GoToTwitter()
{
	FPlatformProcess::LaunchURL(TwitterAddress, NULL, NULL);
}

void UTESUWIntroMain::OnEvent_GoToDiscord()
{
	FPlatformProcess::LaunchURL(DiscordAddress, NULL, NULL);
}

void UTESUWIntroMain::OnEvent_LogOut()
{
	FSimpleDelegate confirmButtonCallBack = FOnTESAction::CreateLambda([&]() {
		
		//. 하단 버튼 숨김
		HideBottomButtons();

		//. 자동로그인 정보 삭제후
		SharedSaveGameManager->SetAutoLoginData(FTESAutoLoginData());

		//.게이트웨이 연결끊기
		SharedNetworkManager->DisconnectForReconnectLoginServer();

		//.로그인 서버 연결
		SharedNetworkManager->ReconnectLoginServer();

		//. 첫번째 프리셋으로 세팅 해준다. (로그인)
		SetContents(0);
	});
	
	FTESMessageBoxContextLogoutParameters logoutParameter = {};
	logoutParameter._context1 = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Title_Signout_Desc);
	logoutParameter._context2 = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Title_Signout_Account);	
	logoutParameter._account = SharedUserDataManager->GetAccountLoginId();
	
	FTESMessageBoxInitData messageBoxInitData = {};
	messageBoxInitData._messageBoxType = ETESMessageBoxType::Normal;
	messageBoxInitData._messageBoxSubType = ETESMessageBoxSubType::TwoButton;
	messageBoxInitData._messageHeaderString = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Title_Signout);
	messageBoxInitData._messageContextType = ETESMessageBoxContextType::Logout;
	messageBoxInitData._messageContextParams = &logoutParameter;
	messageBoxInitData._messageButton1String = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_Cancel);
	messageBoxInitData._messageButton1Callback = nullptr;
	messageBoxInitData._messageButton2String = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_Confirm);
	messageBoxInitData._messageButton2Callback = confirmButtonCallBack;
	SharedPopupManager->ShowMessageBox(messageBoxInitData);
}

void UTESUWIntroMain::OnEvent_Exit()
{
	FSimpleDelegate confirmButtonCallBack = FOnTESAction::CreateLambda([=]() {
		TESUtils::QuitGameApp(GetWorld());
	});

	FTESMessageBoxInitData messageBoxInitData = {};
	messageBoxInitData._messageBoxType = ETESMessageBoxType::Normal;
	messageBoxInitData._messageBoxSubType = ETESMessageBoxSubType::TwoButton;
	messageBoxInitData._messageHeaderString = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Title_Exit_Program);
	messageBoxInitData._messageContextString = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Title_Exit_Program_Desc);
	messageBoxInitData._messageButton1String = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_Cancel);
	messageBoxInitData._messageButton1Callback = nullptr;
	messageBoxInitData._messageButton2String = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_Confirm);
	messageBoxInitData._messageButton2Callback = confirmButtonCallBack;
	SharedPopupManager->ShowMessageBox(messageBoxInitData);
}

