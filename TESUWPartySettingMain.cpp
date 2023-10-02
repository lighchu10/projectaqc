// Fill out your copyright notice in the Description page of Project Settings.
#include "Contents/PartySetting/UI/TESUWPartySettingMain.h"
#include "Contents/PartySetting/UI/TESUWPartySettingSwapSlotInfo.h"
#include "Contents/PartySetting/UI/TESUWPartySettingCharSlotInfo.h"
#include "Contents/PartySetting/UI/TESUWPartySettingAutomatically.h"
#include "Contents/PartySetting/UI/TESUWPartySettingCharCondition.h"
#include "Contents/PartySetting/UI/TESUWPartySettingRequiredList.h"
#include "Contents/PartySetting/UI/TESUWPartySettingSSInfoPopup.h"
#include "Contents/PartySetting/UI/TESUWPartyPresetMain.h"
#include "Contents/PartySetting/UI/TESUWPartyPresetRenamePartyPopup.h"
#include "Contents/PartySetting/Manager/TESPartySettingManager.h"
#include "Contents/PartySetting/Character/TESCharacterPartySetting.h"
#include "Contents/PartySetting/GameScene/TESLGSPartySetting.h"
#include "Adventure/Manager/TESAdventureManager.h"
#include "Sound/Manager/TESSoundManager.h"
#include "Contents/Common/Manager/TESUserDataManager.h"
#include "Contents/Common/Manager/TESSaveGameManager.h"
#include "Contents/Common/Manager/TESUIManager.h"
#include "Contents/Common/Manager/TESTimeManager.h"
#include "Contents/Common/Manager/TESPopupManager.h"
#include "Contents/Common/Manager/TESSceneManager.h"
#include "Contents/Common/Manager/TESStaminaManager.h"
#include "Contents/Common/GameData/TESGDCharacter.h"
#include "Contents/Common/UI/CustomSlates/TESTextBlock.h"
#include "Contents/Common/UI/CustomSlates/TESButton.h"
#include "Contents/Common/UI/CustomSlates/TESImage.h"
#include "Contents/Common/UI/CustomSlates/TESWrapBox.h"
#include "Contents/Common/UI/CustomSlates/TESCheckBox.h"
#include "Contents/Common/UI/CustomSlates/TESWidgetSwitcher.h"
#include "Contents/Common/UI/TESUWCommonHeader.h"
#include "Contents/Common/UI/TESUWCommonRetryCount.h"
#include "Contents/Common/UI/TESUWCommonToggleSwitcher.h"
#include "Contents/Conversation/Manager/TESConversationManager.h"
#include "Contents/Conversation/UI/TESUWConversationMain.h"
#include "Contents/MainStory/Manager/TESMainStoryManager.h"
#include "Contents/MainStory/UI/TESUWMainStoryStageInfoPopup.h"
#include "Contents/MainStory/UI/TESUWBossBattleStageInfoMain.h"
#include "Contents/MainStory/UI/TESUWMainStoryMain.h"
#include "Contents/UniqueAbyss/Manager/TESUniqueAbyssManager.h"
#include "Contents/UniqueAbyss/UI/TESUWUniqueAbyssMain.h"
#include "Contents/UniqueAbyss/UI/TESUWUniqueAbyssNoticePopup.h"
#include "Contents/UniqueAbyss/UI/TESUWUniqueAbyssInfoPopup.h"
#include "Contents/IllusionTemple/Manager/TESIllusionTempleManager.h"
#include "Contents/Lobby/PlayerController/TESPlayerControllerLobby.h"
#include "Contents/Lobby/Character/TESCharacterLobby.h"
#include "Contents/Lobby/UI/TESUWLobbyMain.h"
#include "Contents/Lobby/UI/TESUWLobbyMenuDetail.h"
#include "Contents/Sweep/UI/TESUWSweep.h"
#include "Contents/Sweep/Manager/TESSweepManager.h"
#include "Contents/Navigation/Manager/TESNavigationManager.h"
#include "Contents/Tutorial/Manager/TESNewbieForcedTutorialManager.h"
#include "Network/TESNetworkManager.h"
#include "Battle/Manager/TESStageManager.h"
#include "Battle/Manager/TESStellaStrikeManager.h"
#include "Battle/Manager/TESBattlePartyManager.h"
#include "Battle/Manager/TESTriggerEventManager.h"
#include "Components/RichTextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "TESGameInstance.h"
#include "TESUtilTexture.h"
#include "TESUtilString.h"
#include "TESUtilTable.h"
#include "TESUtilPath.h"
#include "TESUtilEvents.h"
#include "TESUtils.h"

void UTESUWPartySettingMain::InitWidget()
{
	Super::InitWidget();

	InitStageInfoPanel();

	InitPartySettingButtons();

	InitPartyRenameButton();

	InitPresetTabName();
	
	InitPartyName();

	InitPartyCombatPower();

	InitRecommandedCombatPower();

	//InitSwapCharacterSlot();

	InitPartyPresetButton();

	InitPartyEditIcon();

	InitCommonHeader();

	InitCommonRetryCount();

	InitStageInfoRoot();

	InitPartyDisperseButton();

	InitAttributionCharacterRequiredList();

	InitMaterialChecking();

	InitMaterialRequiredList();

	InitStellaStrikeRoot();

	InitStellaStrikeIcon();

	InitStellaStrikeName();

	InitAttributionIcon();

	InitStartBattleButton();

	InitBuffIconButton();

	InitRootToolTip();

	InitRootRecommandedCombatPower();

	InitReplayStory();

	InitSweepButton();

	InitSweepButtonBySweepTickCount();

	InitWidgetSwitcher();

	InitStaminaCount();
}

void UTESUWPartySettingMain::ClearWidget()
{
	Super::ClearWidget();

	ClearCharacterConditionWidgets();

	ClearMaterialConditionWidgets();

    ClearCommonHeader();

	ClearCommonRetryCount();

	GetWorld()->GetTimerManager().ClearTimer(_th_appearAnim);
	GetWorld()->GetTimerManager().ClearTimer(_th_disappearAnim);
	GetWorld()->GetTimerManager().ClearTimer(_th_fadeInAction);
}

void UTESUWPartySettingMain::OpenWidget()
{
	Super::OpenWidget();

    if (true == ::IsValid(_commonHeaderWidget))
    {
        _commonHeaderWidget->OpenWidget();
    }

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UTESUWPartySettingMain::CloseWidget()
{
	Super::CloseWidget();

    if (true == ::IsValid(_commonHeaderWidget))
    {
        _commonHeaderWidget->CloseWidget();
    }

	SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWPartySettingMain::OpenDelegate()
{
	Super::OpenDelegate();

	_dh_partyInfo = FTESUtilEvents::OnPartyNameChanged.AddUObject(this, &UTESUWPartySettingMain::OnEvent_RefreshPartyName);

	_dh_partyChanged = FTESUtilEvents::OnPartyMemberChanged.AddUObject(this, &UTESUWPartySettingMain::OnEvent_CallbackChangedPartyMembers);

	FTESUtilEvents::OnBattleEnter.BindUObject(this, &UTESUWPartySettingMain::OnEvent_EnterBattle);

	FTESUtilEvents::OnUniqueAbyssStageOpened.BindUObject(this, &UTESUWPartySettingMain::OnEvent_OpenedUniqueAbyssSuccessfully);

	SAFE_ADD_UOBJECT_DELEGATE(FTESUtilEvents::OnSweepResultRecieved, OnEvent_SweepResultRecieved, _dh_sweepResultRecieved);

	SAFE_ADD_UOBJECT_DELEGATE(FTESUtilEvents::OnItemDataUpdated, OnEvent_ItemDataUpdated, _dh_itemDataUpdated);

	SAFE_ADD_UOBJECT_DELEGATE(FTESUtilEvents::OnCommonRewardComfirmed, OnEvent_CommonRewardComfirmed, _dh_commonRewardComfirmed);
}

void UTESUWPartySettingMain::CloseDelegate()
{
	Super::CloseDelegate();

	FTESUtilEvents::OnPartyNameChanged.Remove(_dh_partyInfo);
	_dh_partyInfo = {};

	FTESUtilEvents::OnPartyMemberChanged.Remove(_dh_partyChanged);
	_dh_partyChanged = {};

	FTESUtilEvents::OnBattleEnter.Unbind();

	FTESUtilEvents::OnUniqueAbyssStageOpened.Unbind();

	SAFE_REMOVE_UOBJECT_DELEGATE(FTESUtilEvents::OnSweepResultRecieved, _dh_sweepResultRecieved);

	SAFE_REMOVE_UOBJECT_DELEGATE(FTESUtilEvents::OnItemDataUpdated, _dh_itemDataUpdated);

	SAFE_REMOVE_UOBJECT_DELEGATE(FTESUtilEvents::OnCommonRewardComfirmed, _dh_commonRewardComfirmed);
}

void UTESUWPartySettingMain::InitStageInfoPanel()
{
	_hb_stageInfo = Cast<UHorizontalBox>(GetWidgetFromName(TEXT("HB_StageInfo")));
	TESCHECK(nullptr != _hb_stageInfo);
	_hb_stageInfo->SetVisibility(ESlateVisibility::Collapsed);

	_sp_spacer = Cast<USpacer>(GetWidgetFromName(TEXT("SP_space")));
	TESCHECK(nullptr != _sp_spacer);
}

void UTESUWPartySettingMain::InitPartySettingButtons()
{
	_hb_bottomButtons = Cast<UHorizontalBox>(GetWidgetFromName(TEXT("BottomButtons_Prim")));
	TESCHECK(nullptr != _hb_bottomButtons);
}

void UTESUWPartySettingMain::InitPartyRenameButton()
{
	_btn_partyPresetRename = Cast<UTESButton>(GetWidgetFromName(TEXT("BTN_PartyPresetRename")));
	TESCHECK(nullptr != _btn_partyPresetRename);
}

void UTESUWPartySettingMain::InitStageName()
{
	_txt_stageName = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_StageInfo")));
	TESCHECK(nullptr != _txt_stageName);
}

void UTESUWPartySettingMain::InitPresetTabName()
{
	_txt_presetTabName = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_PartyInfoTab")));
	TESCHECK(nullptr != _txt_presetTabName);
}

void UTESUWPartySettingMain::InitPartyName()
{
	_txt_partyName = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_PartyInfoName")));
	TESCHECK(nullptr != _txt_partyName);
}

void UTESUWPartySettingMain::InitPartyCombatPower()
{
	_rtxt_partyCombatPower = Cast<URichTextBlock>(GetWidgetFromName(TEXT("RTXT_PartyCombatPower")));
	TESCHECK(nullptr != _rtxt_partyCombatPower);
}

void UTESUWPartySettingMain::InitRecommandedCombatPower()
{
	_rtxt_recommandedCombatPower = Cast<URichTextBlock>(GetWidgetFromName(TEXT("RTXT_RecommandedCombatPower")));
	TESCHECK(nullptr != _rtxt_recommandedCombatPower);
	_rtxt_recommandedCombatPower->SetText(FText::FromString(TEXT("")));
}

//void UTESUWPartySettingMain::InitSwapCharacterSlot()
//{
//	_txt_swapSlot = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_ChangeSlot")));
//	TESCHECK(nullptr != _txt_swapSlot);
//
//	_txt_swapEndSlot = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_ChangeEnd")));
//	TESCHECK(nullptr != _txt_swapEndSlot);
//
//	_txt_swapEndSlot->SetVisibility(ESlateVisibility::Hidden);
//}

void UTESUWPartySettingMain::InitPartyPresetButton()
{
	_btn_partyPreset = Cast<UTESButton>(GetWidgetFromName(TEXT("BTN_PartyPresetSetting")));
	TESCHECK(nullptr != _btn_partyPreset);
}

void UTESUWPartySettingMain::InitPartyEditIcon()
{
	_img_editIcon = Cast<UTESImage>(GetWidgetFromName(TEXT("IMG_EditIcon")));
	TESCHECK(nullptr != _img_editIcon);
}

void UTESUWPartySettingMain::InitStageInfoRoot()
{
	_cp_stageInfo = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Root_StageInfo")));
	TESCHECK(nullptr != _cp_stageInfo);
	_cp_stageInfo->SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWPartySettingMain::InitPartyDisperseButton()
{
	_cts_disperseParty = Cast<UTESUWCommonToggleSwitcher>(GetWidgetFromName(TEXT("CTS_DisperseParty")));
	TESCHECK(nullptr != _cts_disperseParty);
	_cts_disperseParty->SetToggleState(false);
}

//void UTESUWPartySettingMain::InitCharacterSwapButton()
//{
//	_btn_characterSwap = Cast<UTESButton>(GetWidgetFromName(TEXT("BTN_ChangeSlot")));
//	TESCHECK(nullptr != _btn_characterSwap);
//}

void UTESUWPartySettingMain::InitAttributionCharacterRequiredList()
{
	_vb_characterAttributionRequired = Cast<UVerticalBox>(GetWidgetFromName(TEXT("VB_AttributionCharacter")));
	TESCHECK(nullptr != _vb_characterAttributionRequired);
	_vb_characterAttributionRequired->ClearChildren();
}

void UTESUWPartySettingMain::InitMaterialRequiredList()
{
	_wb_materialRequired = Cast<UTESWrapBox>(GetWidgetFromName(TEXT("WB_RequiredMaterial")));
	TESCHECK(nullptr != _wb_materialRequired);
	_wb_materialRequired->ClearChildren();
}

void UTESUWPartySettingMain::InitStellaStrikeRoot()
{
	_cp_stellaStrikeList.Empty();

	for (int32 index = 1; index <= 2; ++index)
	{
		FString rootName = FString::Printf(TEXT("StellaStrike_%d"), index);
		UCanvasPanel* stellaStrikeRoot = Cast<UCanvasPanel>(GetWidgetFromName(*rootName));
		TESCHECK_CONTINUE(nullptr != stellaStrikeRoot);
		stellaStrikeRoot->SetVisibility(ESlateVisibility::Collapsed);

		_cp_stellaStrikeList.Add(stellaStrikeRoot);
	}
}

void UTESUWPartySettingMain::InitStellaStrikeIcon()
{
	_img_stellStrikeIconList.Empty();

	for (int32 index = 1; index <= 2; ++index)
	{
		FString iconName = FString::Printf(TEXT("IMG_StellStrikeIcon_%d"), index);
		UTESImage* stellaStrikeIcon = Cast<UTESImage>(GetWidgetFromName(*iconName));
		TESCHECK_CONTINUE(nullptr != stellaStrikeIcon);

		_img_stellStrikeIconList.Add(stellaStrikeIcon);
	}
}

void UTESUWPartySettingMain::InitStellaStrikeName()
{
	_txt_stellStrikeNameList.Empty();

	for (int32 index = 1; index <= 2; ++index)
	{
		FString stellaStrikeName = FString::Printf(TEXT("TXT_StellaStrike_%d"), index);
		UTESTextBlock* stellaStrikeText = Cast<UTESTextBlock>(GetWidgetFromName(*stellaStrikeName));
		TESCHECK_CONTINUE(nullptr != stellaStrikeText);

		_txt_stellStrikeNameList.Add(stellaStrikeText);
	}
}

void UTESUWPartySettingMain::InitAttributionIcon()
{
	_img_attributionIcon.Empty();

	for (int32 index = 1; index <= 8; ++index)
	{
		FString iconName = FString::Printf(TEXT("IMG_Attribution_%d"), index);
		UTESImage* attributionIcon = Cast<UTESImage>(GetWidgetFromName(*iconName));
		TESCHECK_CONTINUE(nullptr != attributionIcon);
		attributionIcon->SetVisibility(ESlateVisibility::Collapsed);
		TESUtilTexture::SetDynamicTexture(attributionIcon, TEXT(""), false);

		_img_attributionIcon.Add(attributionIcon);
	}
}

void UTESUWPartySettingMain::InitStartBattleButton()
{
	_cts_startBattle = Cast<UTESUWCommonToggleSwitcher>(GetWidgetFromName(TEXT("CTS_StartBattle")));
	TESCHECK(nullptr != _cts_startBattle);
	_cts_startBattle->SetToggleState(false);
}

void UTESUWPartySettingMain::InitMaterialChecking()
{
	_cp_requiredMaterialChecking = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("RequiredMaterialCheck")));
	TESCHECK(nullptr != _cp_requiredMaterialChecking);
	_cp_requiredMaterialChecking->SetVisibility(ESlateVisibility::Collapsed);

	_chkb_requiredMaterial = Cast<UCheckBox>(GetWidgetFromName(TEXT("CheckBox_RequiredMaterial")));
	TESCHECK(nullptr != _chkb_requiredMaterial);
	_chkb_requiredMaterial->SetCheckedState(ECheckBoxState::Unchecked);
}

void UTESUWPartySettingMain::InitBuffIconButton()
{
	_btn_buffIcon = Cast<UTESButton>(GetWidgetFromName(TEXT("BTN_OverPower_BuffIcon")));
	TESCHECK(nullptr != _btn_buffIcon);
	_btn_buffIcon->SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWPartySettingMain::InitRootToolTip()
{
	_cp_rootToolTip = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Root_ToolTip")));
	TESCHECK(nullptr != _cp_rootToolTip);
	_cp_rootToolTip->SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWPartySettingMain::InitRootRecommandedCombatPower()
{
	_cp_recommandedCombatPowerPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("RecommandedCombatPower")));
	TESCHECK(nullptr != _cp_recommandedCombatPowerPanel);
	_cp_recommandedCombatPowerPanel->SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWPartySettingMain::InitReplayStory()
{
	_btn_replayStory = Cast<UTESButton>(GetWidgetFromName(TEXT("BTN_ReplayStory")));
	TESCHECK(true == ::IsValid(_btn_replayStory));
	_btn_replayStory->SetVisibility(ESlateVisibility::Collapsed);

	_chkb_replayStory = Cast<UTESCheckBox>(GetWidgetFromName(TEXT("CHKB_ReplayStory")));
	TESCHECK(true == ::IsValid(_chkb_replayStory));
	_chkb_replayStory->SetCheckedState(ECheckBoxState::Unchecked);
}

void UTESUWPartySettingMain::InitWidgetSwitcher()
{
	_ws_consume = Cast<UTESWidgetSwitcher>(GetWidgetFromName(TEXT("WS_Consume")));
	TESCHECK(true == ::IsValid(_ws_consume));
	_ws_consume->SetActiveWidgetIndex(0);
}

void UTESUWPartySettingMain::InitStaminaCount()
{
	_txt_staminaCount = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_StaminaCount")));
	TESCHECK(nullptr != _txt_staminaCount);
	_txt_staminaCount->SetText(FText::FromString(TEXT("")));

	_hb_stamina = Cast<UHorizontalBox>(GetWidgetFromName(TEXT("HB_Stamina")));
	TESCHECK(nullptr != _hb_stamina);
	_hb_stamina->SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWPartySettingMain::InitCommonHeader()
{
	_commonHeaderWidget = Cast<UTESUWCommonHeader>(GetWidgetFromName(TEXT("WBP_CommonHeader")));
	TESCHECK(nullptr != _commonHeaderWidget);
	_commonHeaderWidget->InitWidget();
	_commonHeaderWidget->CloseWidget();
}

void UTESUWPartySettingMain::InitCommonRetryCount()
{
	_commonRetryWidget = Cast<UTESUWCommonRetryCount>(GetWidgetFromName(TEXT("WBP_CommonRetryCount")));
	TESCHECK(nullptr != _commonRetryWidget);
	_commonRetryWidget->InitWidget();
}

void UTESUWPartySettingMain::InitSweepButton()
{
	_cts_sweep = Cast<UTESUWCommonToggleSwitcher>(GetWidgetFromName(TEXT("CTS_Sweep")));
	TESCHECK(nullptr != _cts_sweep);
	_cts_sweep->SetToggleState(false);
}

void UTESUWPartySettingMain::InitSweepButtonBySweepTickCount()
{
	//. on
	_txt_sweep_on_sweepTicketCount = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_Sweep_On_SweepTicketCount")));
	TESCHECK(nullptr != _txt_sweep_on_sweepTicketCount);
	_txt_sweep_on_sweepTicketCount->SetText(FText::FromString(TEXT("")));

	//. off
	_txt_sweep_off_sweepTicketCount = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_Sweep_Off_SweepTicketCount")));
	TESCHECK(nullptr != _txt_sweep_off_sweepTicketCount);
	_txt_sweep_off_sweepTicketCount->SetText(FText::FromString(TEXT("")));
}

void UTESUWPartySettingMain::ClearCommonHeader()
{
	if (true == ::IsValid(_commonHeaderWidget))
	{
		_commonHeaderWidget->CloseWidget();
		_commonHeaderWidget->ClearWidget();
	}
}

void UTESUWPartySettingMain::ClearCommonRetryCount()
{
	if (true == ::IsValid(_commonRetryWidget))
	{
		_commonRetryWidget->CloseWidget();
		_commonRetryWidget->ClearWidget();
	}
}

void UTESUWPartySettingMain::ClearCharacterConditionWidgets()
{
	for (UTESUWPartySettingCharCondition* characterConditionWidget : _characterConditionWidgets)
	{
		SharedUIManager->DestroyCtrWidget(characterConditionWidget);
	}

	_characterConditionWidgets.Empty();
}

void UTESUWPartySettingMain::ClearMaterialConditionWidgets()
{
	for (UTESUWPartySettingRequiredList* materialConditionWidget : _materialConditionWidgets)
	{
		SharedUIManager->DestroyCtrWidget(materialConditionWidget);
	}

	_materialConditionWidgets.Empty();
}

void UTESUWPartySettingMain::SetWidget(FTESPartyGroupInfo partyGroupInfo, FTESPartyInfo partyInfo, bool sallyForced, int32 stageId/*= 0*/, ETESCommonRemainCountType remainCountType /*= ETESCommonRemainCountType::None*/)
{
	if (false == SetData(partyGroupInfo, partyInfo, stageId))
	{
		TESLOG(Error, TEXT("There is no Data"));
		return;
	}

	SetCommonHeader();

	SetStageInfo(stageId);

	SetPresetTabName();

	SetPartyName();

	SetPartyCombatPower();

	SetRecommandedCombatPower();

	SetPartyRenameButton();

	SetPartyPresetButton();

	SetPartyEditIcon();

	SetSlotLocking();

	SetStageInfoPanel(0 == stageId ? false : true);

	SetStageInfoVisible(0 == stageId ? false : true);

	SetAppearAnim();

	SetPartySettingButtons(0 == stageId ? false : true);

	SetRequiredAttributionCharacterList(stageId);

	//SetRequiredMaterialList(stageId);

	SetStellaStrikeInfo();

	SetRequireStamina(stageId);

	SetRequiredMaterialActivation(stageId);

	SetBuffIconButton();

	SetRootToolTip(false);

	SetRootRecommandedCombatPowerPanelVisible(0 == stageId ? false : true);

	SetReplayStory(_isPlayStory);

	RefreshSweepButton();

	RefreshSweepButtonBySweepTicketCount();

	SetRemainCount(remainCountType);
}

void UTESUWPartySettingMain::SetStageInfo(int32 stageId)
{
	if (0 == stageId)
		return;

	const FStageRow& stageRow = SharedTDBManager->GetStageRow(stageId);

	//SetStageInfoVisible(true);

	if (true == stageRow.IsEmpty())
	{
		TESLOG(Error, TEXT("stageRow is Empty"));
		return;
	}

	_curStageId = stageId;

	_curStageRow = stageRow;

	//. 해당 스테이지에 편성 불가 슬롯 있는지 판별 후 세팅
	SetSlotClosing();
}

void UTESUWPartySettingMain::RefreshPartyCombatPower()
{
	SetPartyCombatPower();

	SetBuffIconButton();

	SetRequiredAttributionCharacterList(_curStageId);
}

void UTESUWPartySettingMain::ReFreshStellaStrikeInfo()
{
	SetStellaStrikeInfo();
}

void UTESUWPartySettingMain::SetCommonHeader()
{
	if (true == ::IsValid(_commonHeaderWidget))
	{
		_commonHeaderWidget->OpenWidget();
		_commonHeaderWidget->SetWidget();
		_commonHeaderWidget->ApplyCenterDisplayersByDefault();
	}
}

bool UTESUWPartySettingMain::SetData(FTESPartyGroupInfo partyGroupInfo, FTESPartyInfo partyInfo, int32 stageId)
{
	_isBackToPrevious = false;
	_isBackToHome = false;

	//. 탭을 찾는다.(현재 들어온 탭)
	TOptional<FTESPartyGroupInfo> partyGroupTabInfo = SharedUserDataManager->FindPartyGroupList(partyGroupInfo._unique);

	//. 그룹탭 이름
	if (true == partyGroupTabInfo.IsSet())
	{
		_uniqueTabId = partyGroupInfo._unique;
		FString tempString = partyGroupInfo._name;
		_partyPrestTabName = tempString.TrimStartAndEnd();
	}
	else
	{
		ETESPartyPresetTab partyPresetTab = static_cast<ETESPartyPresetTab>(partyGroupInfo._unique);

		_uniqueTabId = partyGroupInfo._unique;

		switch (partyPresetTab)
		{
		case ETESPartyPresetTab::None:
			{
				//. 강제 출격이 있는 파티는 스테이지 이름이 나와야 한다.
				_isSallyForced = true;
			}
			break;
		case ETESPartyPresetTab::FirstTab:
			{
				_partyPrestTabName = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::PartyPreset_Tab_DefaultName_1);
			}
			break;
		case ETESPartyPresetTab::SecondTab:
			{
				_partyPrestTabName = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::PartyPreset_Tab_DefaultName_2);
			}
			break;
		case ETESPartyPresetTab::ThirdTab:
			{
				_partyPrestTabName = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::PartyPreset_Tab_DefaultName_3);
			}
			break;
		case ETESPartyPresetTab::FourthTab:
			{
				_partyPrestTabName = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::PartyPreset_Tab_DefaultName_4);
			}
			break;
		case ETESPartyPresetTab::FifthTab:
			{
				_partyPrestTabName = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::PartyPreset_Tab_DefaultName_5);
			}
			break;
		case ETESPartyPresetTab::SixthTab:
			{
				_partyPrestTabName = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::PartyPreset_Tab_DefaultName_6);
			}
			break;
		default:
			{
				TESLOG(Error, TEXT("out of ETESPartyPresetTab"));
			}
			break;
		}
	}

	//. 파티정보가 있으면 세팅해준다.
	if (0 != partyInfo._unique)
	{
		_currentPartyUnique = partyInfo._unique;
		FString tempString = partyInfo._name;

		_currentPartyName = tempString.TrimStartAndEnd();

		//. 서버로부터 아무런 문자열이 안온다면 클라에서 테이블을 참조하여 세팅해준다.
		if (TEXT("") == _currentPartyName)
		{
			switch (_currentPartyUnique % 100)
			{
			case 0:
				{
					_currentPartyName = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::PartyPreset_Party_DefaultName_1);
				}
				break;
			case 1:
				{
					_currentPartyName = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::PartyPreset_Party_DefaultName_2);
				}
				break;
			case 2:
				{
					_currentPartyName = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::PartyPreset_Party_DefaultName_3);
				}
				break;
			case 3:
				{
					_currentPartyName = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::PartyPreset_Party_DefaultName_4);
				}
				break;
			case 4:
				{
					_currentPartyName = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::PartyPreset_Party_DefaultName_5);
				}
				break;
			case 5:
				{
					_currentPartyName = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::PartyPreset_Party_DefaultName_6);
				}
				break;
			case 6:
				{
					_currentPartyName = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::PartyPreset_Party_DefaultName_7);
				}
				break;
			case 7:
				{
					_currentPartyName = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::PartyPreset_Party_DefaultName_8);
				}
				break;
			default:
				{
					TESLOG(Error, TEXT("There is no string"));
				}
				break;
			}
		}

		_currentCharacterUniqueList.Empty();

		for (int index = 0; index < sizeof(partyInfo._characterUnique) / sizeof(int64); ++index)
		{
			_currentCharacterUniqueList.Add(partyInfo._characterUnique[index]);
		}
	}
	
	if (true == ::IsValid(_btn_replayStory))
	{
		_isPlayStory = false;

		FStageRow stageTable = SharedTDBManager->GetStageRow(stageId);
		if ((false == SharedUserDataManager->HasStageClearData(stageId)) || (false == stageTable.StoryMode))
		{
			_btn_replayStory->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			_btn_replayStory->SetVisibility(ESlateVisibility::Visible);
		}
	}

	return true;
}

void UTESUWPartySettingMain::SetStageInfoPanel(bool isVisible)
{
	TESCHECK(nullptr != _hb_stageInfo);
	TESCHECK(nullptr != _sp_spacer);

	_hb_stageInfo->SetVisibility(true == isVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	_sp_spacer->SetVisibility(true == isVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UTESUWPartySettingMain::SetPartySettingButtons(bool isMaterialRequired)
{
	if (false == isMaterialRequired)
	{
		//. 채우기로 바꿔준다.(자동->채우기)
		UHorizontalBoxSlot* horizontalBoxslot = Cast<UHorizontalBoxSlot>(_hb_bottomButtons->Slot);
		TESCHECK(nullptr != horizontalBoxslot);
		FSlateChildSize slateChildSize = {};
		slateChildSize.SizeRule = ESlateSizeRule::Fill;

		horizontalBoxslot->SetSize(slateChildSize);
	}
}

void UTESUWPartySettingMain::SetPartyRenameButton()
{
	TESCHECK(nullptr != _btn_partyPresetRename);

	if (false == _isSallyForced)
	{
		_btn_partyPresetRename->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		_btn_partyPresetRename->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UTESUWPartySettingMain::SetPresetTabName()
{
	TESCHECK(nullptr != _txt_presetTabName);

	if (false == _isSallyForced)
	{
		_txt_presetTabName->SetGlobalString(_partyPrestTabName);
	}
	else
	{
		_txt_presetTabName->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UTESUWPartySettingMain::SetPartyName()
{
	TESCHECK(nullptr != _txt_partyName);

	if (true == _isSallyForced)
	{
		_txt_partyName->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		TOptional<FTESPartyInfo> partyInfo = SharedUserDataManager->FindPartyList(_currentPartyUnique);

		if (false == partyInfo.IsSet())
		{
			//. 아무것도 없으면 디폴트에 세팅한 것으로 출력한다.
			_txt_partyName->SetGlobalString(_currentPartyName);
		}
		else
		{
			//. 정보에 이름이 없으면 디폴트로 세팅 해준다.
			FString tempString = partyInfo.GetValue()._name;
			FString partyName = tempString.TrimStartAndEnd();
			if (TEXT("") != partyName)
			{
				_txt_partyName->SetGlobalString(partyName);
			}
			else
			{
				_txt_partyName->SetGlobalString(_currentPartyName);
			}
		}
	}
}

void UTESUWPartySettingMain::SetPartyCombatPower()
{
	TOptional<FTESPartyInfo> partyInfo = SharedUserDataManager->FindPartyList(_currentPartyUnique);

	_currentCharacterUniqueList.Empty();

	if (false == partyInfo.IsSet())
	{
		//. 아무것도 없으면 디폴트로 냅둔다.
		FString formatString = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Party_Power);
		FString combatPower = TESUtilString::MakeFormatString(formatString, 0);

		TESCHECK(nullptr != _rtxt_partyCombatPower);
		_rtxt_partyCombatPower->SetText(FText::FromString(*combatPower));

		//. 파티가 존재하지 않으므로 파티해산 버튼 및 자리교체 버튼을 비활성화 시킨다.
		SetDispersePartyButton(false);
		SetStartBattleButton(false);
		//SetCharacterSwapButtonEnable(false);
	}
	else
	{
		FTESPartyInfo currentPartyInfo = partyInfo.GetValue();
		int32 numOfParty = sizeof(currentPartyInfo._characterUnique) / sizeof(int64);

		int32 partyTotalCombatPower = 0;

		for (int32 index = 0; index < numOfParty; ++index)
		{
			TOptional<UTESGDCharacter*> gdCharacter = SharedUserDataManager->FindHaveCharacter(currentPartyInfo._characterUnique[index]);
			if (true == gdCharacter.IsSet())
			{
				partyTotalCombatPower += gdCharacter.GetValue()->GetCharacterBattlePower();
			}
			else
			{
				if (TESConstantValue::NoneValue == currentPartyInfo._characterUnique[index])
				{
					//. 체험캐릭 같은 경우에는 테이블에 입력된 정보로 가져온다.
					//. 하지만 여기선 체험 캐릭이 없으므로 냅둔다.
				}
			}

			_currentCharacterUniqueList.Add(currentPartyInfo._characterUnique[index]);
		}

		FString formatString = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Party_Power);
		FString combatPower = TESUtilString::MakeFormatString(formatString, partyTotalCombatPower);

		TESCHECK(nullptr != _rtxt_partyCombatPower);
		_rtxt_partyCombatPower->SetText(FText::FromString(*combatPower));

		//. 파티가 존재하지 않으므로 파티해산 버튼 및 자리교체 버튼을 비활성화 시킨다.
		if (0 == partyTotalCombatPower)
		{
			SetDispersePartyButton(false);
			SetStartBattleButton(false);
			//SetCharacterSwapButtonEnable(false);
		}
		else
		{
			SetDispersePartyButton(true);
			SetStartBattleButton(true);
			//SetCharacterSwapButtonEnable(true);
		}
	}
}

void UTESUWPartySettingMain::SetRecommandedCombatPower()
{
	const FStageRow& stageRow = SharedTDBManager->GetStageRow(_curStageId);
	if (false == stageRow.IsEmpty())
	{
		FString formatString = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Party_Suggest_Power);
		FString recommandedPower = TESUtilString::MakeFormatString(formatString, stageRow.SuggestPower);

		TESCHECK(nullptr != _rtxt_recommandedCombatPower);
		_rtxt_recommandedCombatPower->SetText(FText::FromString(*recommandedPower));
	}
}

void UTESUWPartySettingMain::SetPartyPresetButton()
{ 
	//. 강제 출격이 있으면 비활성화 시켜준다.
	TESCHECK(nullptr != _btn_partyPreset);
	if (false == _isSallyForced)
	{
		_btn_partyPreset->SetIsEnabled(true);
	}
	else
	{
		_btn_partyPreset->SetIsEnabled(false);
	}
}

void UTESUWPartySettingMain::SetPartyEditIcon()
{
	//. 강제 출격이 있으면 비활성화 시켜준다.
	TESCHECK(nullptr != _img_editIcon);
	if (false == _isSallyForced)
	{
		_img_editIcon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		_img_editIcon->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UTESUWPartySettingMain::SetSlotLocking()
{
	TArray<UTESUWPartySettingSwapSlotInfo*> swapSlotInfo = SharedPartySettingManager->GetCharacterSwapSlotList();
	TArray<UTESUWPartySettingCharSlotInfo*> characterSlotInfo = SharedPartySettingManager->GetCharacterInfoSlot();

	int32 numofLockedSlot = SharedPartySettingManager->GetNumberOfLockedSlot();

	//. 강제 출격이 된 슬롯이 있는지 확인하고, 있으면 처리해준다.
	for (int32 index = 0; index < numofLockedSlot; ++index)
	{
		if (nullptr != swapSlotInfo[index])
		{
			swapSlotInfo[index]->SetSlotLocking(true);
		}

		if (nullptr != characterSlotInfo[index])
		{
			characterSlotInfo[index]->SetSlotLocking(true);
		}
	}
}

void UTESUWPartySettingMain::SetStageInfoVisible(bool isVisible)
{
	TESCHECK(nullptr != _cp_stageInfo);
	_cp_stageInfo->SetVisibility(true == isVisible ?
								ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UTESUWPartySettingMain::SetStageInfoName(int32 chapterNumber, int32 stageNumber)
{
	//. 챕터넘버 스테이지넘버
	FString partyGroupName = TESUtilString::MakeFormatString(*TESUtilTable::GetGlobalString(GetWorld(), EGStringID::MainStory_ChapterPartySetting_Format), chapterNumber, stageNumber);
	TESCHECK(nullptr != _txt_stageName)
	_txt_stageName->SetText(FText::FromString(*partyGroupName));
}

void UTESUWPartySettingMain::SetSlotClosing()
{
	//. 편성불가 슬롯은 오른쪽부터 시작한다.
	//. 강제 출격에서만 일어난다.
	if (true == _isSallyForced)
	{
		const FStageRow& stageRow = SharedTDBManager->GetStageRow(_curStageId);

		if (true == stageRow.IsEmpty())
		{
			TESLOG(Error, TEXT("Stage Row is Empty"));
			return;
		}

		int32 numOfSlotClosed = TESConstantValue::MaxPartyCount - stageRow.UseCharacterCount;
		TArray<UTESUWPartySettingSwapSlotInfo*> swapSlotInfo = SharedPartySettingManager->GetCharacterSwapSlotList();
		TArray<UTESUWPartySettingCharSlotInfo*> characterSlotInfo = SharedPartySettingManager->GetCharacterInfoSlot();

		int32 maxCount = swapSlotInfo.Num() - 1;	
		for (int32 index = 0; index < numOfSlotClosed; ++index)
		{
			if (true == swapSlotInfo.IsValidIndex(maxCount - index))
			{
				if (nullptr != swapSlotInfo[maxCount - index])
				{
					swapSlotInfo[maxCount - index]->SetSlotClosed(true);
				}
			}
		}

		maxCount = characterSlotInfo.Num() - 1;
		for (int32 index = 0; index < numOfSlotClosed; ++index)
		{
			if (true == characterSlotInfo.IsValidIndex(maxCount - index))
			{
				if (nullptr != characterSlotInfo[maxCount - index])
				{
					characterSlotInfo[maxCount - index]->SetSlotClosed(true);
				}
			}
		}
	}
}

void UTESUWPartySettingMain::SetRequiredAttributionCharacterList(int32 stageId)
{
	if (0 == stageId)
		return;

	const FStageRow& stageRow = SharedTDBManager->GetStageRow(stageId);

	if (true == stageRow.IsEmpty())
	{
		TESLOG(Error, TEXT("Stage Row is Empty"));
		return;
	}

	TESCHECK(nullptr != _vb_characterAttributionRequired);
	_vb_characterAttributionRequired->ClearChildren();

	TArray<FTESPartyCondition> partyCondtionList = stageRow.PartyCondition;

	int32 maxCount = partyCondtionList.Num();

	//. 내가 세팅된 캐릭터와 비교해서 조건에 맞는지 확인한 후 불충족하면 빨간색으로 바꿔준다.
	ClearCharacterConditionWidgets();
	for (int32 index = 0; index < maxCount; ++index)
	{
		if (false == partyCondtionList.IsValidIndex(index))
		{
			TESLOG(Error, TEXT("party condition is invalid"));
			continue;
		}

		UTESUWPartySettingCharCondition* characterConditionWidget = SharedUIManager->CreateCtrWidget<UTESUWPartySettingCharCondition>(TEXT("WBP_PartySettingBattleStartConditionSlot_01"), TEXT("Common/WBP/PartySetting"));
		TESCHECK_CONTINUE(nullptr != characterConditionWidget);

		bool isSatisfied = false;

		isSatisfied = TESUtils::CheckPartyConditionOnStage(_currentCharacterUniqueList, partyCondtionList[index]);

		characterConditionWidget->InitWidget();
		characterConditionWidget->OpenWidget();
		characterConditionWidget->SetWidget(partyCondtionList[index], isSatisfied);

		_characterConditionWidgets.Add(characterConditionWidget);

		TESCHECK_CONTINUE(nullptr != _vb_characterAttributionRequired);
		_vb_characterAttributionRequired->AddChild(characterConditionWidget);
	}
}

void UTESUWPartySettingMain::SetRequiredMaterialList(int32 stageId)
{
	if (0 == stageId)
		return;

	const FStageRow& stageRow = SharedTDBManager->GetStageRow(stageId);

	if (true == stageRow.IsEmpty())
	{
		TESLOG(Error, TEXT("Stage Row is Empty"));
		return;
	}

	if (true == ::IsValid(_wb_materialRequired))
	{
		_wb_materialRequired->ClearChildren();
	}

	//. 현재 내가 보유하고 있는 아이템과 여기에 있는 아이템을 비교해서 하나라도 만족을 안하면 빨간색으로 표기
	ClearMaterialConditionWidgets();
	TArray<FTESGoodsItem> materialContionList = stageRow.MaterialCondition;
	int32 maxCount = materialContionList.Num();

	for (int32 index = 0; index < maxCount; ++index)
	{
		int64 goodsAmount = SharedUserDataManager->GetGoodsTotalCount(stageRow.MaterialCondition[index]._goodsType
																	, stageRow.MaterialCondition[index]._itemId);
		int32 needMaterialCount = stageRow.MaterialCondition[index]._count;
		bool isEnough = true;

		if (goodsAmount < needMaterialCount)
		{
			isEnough = false;
		}

		if (ETypeContents::UniqueAbyss == stageRow.T_Contents)
		{
			TArray<int32> activeStageList = SharedUniqueAbyssManager->GetActiveStageList();
			for (int32 activeStage : activeStageList)
			{
				if (activeStage == stageId)
				{
					isEnough = true;
				}
			}
		}

		UTESUWPartySettingRequiredList* materialConditionWidget = SharedUIManager->CreateCtrWidget<UTESUWPartySettingRequiredList>(TEXT("WBP_PartySettingBattleStartConditionSlot_02"), TEXT("Common/WBP/PartySetting"));
		TESCHECK_CONTINUE(nullptr != materialConditionWidget);

		materialConditionWidget->InitWidget();
		materialConditionWidget->OpenWidget();
		materialConditionWidget->SetWidget(_curStageId, index, goodsAmount, isEnough);

		_materialConditionWidgets.Add(materialConditionWidget);

		TESCHECK_CONTINUE(nullptr != _wb_materialRequired);
		_wb_materialRequired->AddChild(materialConditionWidget);
	}
}

void UTESUWPartySettingMain::SetStellaStrikeInfo()
{
	TArray<class UTESGDCharacter*> gdCharacterList = {};

	TOptional<FTESPartyInfo> partyInfo = SharedUserDataManager->FindPartyList(_currentPartyUnique);

	if (false == partyInfo.IsSet())
	{
		if (true == _cp_stellaStrikeList.IsValidIndex(0))
		{
			_cp_stellaStrikeList[0]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}

		if (true == _txt_stellStrikeNameList.IsValidIndex(0))
		{
			_txt_stellStrikeNameList[0]->SetGlobalString(TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Party_Change_Not_StellaStrike));
		}

		if (true == _img_stellStrikeIconList.IsValidIndex(0))
		{
			_img_stellStrikeIconList[0]->SetVisibility(ESlateVisibility::Collapsed);
		}

		return;
	}

	FTESPartyInfo currentPartyInfo = partyInfo.GetValue();
	int32 numOfParty = sizeof(currentPartyInfo._characterUnique) / sizeof(int64);

	for (int32 index = 0; index < numOfParty; ++index)
	{
		TOptional<UTESGDCharacter*> gdCharacter = SharedUserDataManager->FindHaveCharacter(currentPartyInfo._characterUnique[index]);
		if (true == gdCharacter.IsSet())
		{
			gdCharacterList.Add(gdCharacter.GetValue());
		}
	}

	//. 초기화
	for (int32 index = 0; index < _cp_stellaStrikeList.Num(); ++index)
	{
		_cp_stellaStrikeList[index]->SetVisibility(ESlateVisibility::Collapsed);
	}

	for (int32 index = 0; index < _img_attributionIcon.Num(); ++index)
	{
		_img_attributionIcon[index]->SetVisibility(ESlateVisibility::Collapsed);
		TESUtilTexture::SetDynamicTexture(_img_attributionIcon[index], TEXT(""), false);
	}

	TArray<FStellaStrikeSkillRow> stellaStrikeRowList =	TESUtilTable::GetStellaStrikeSkillTableFromCurrentPartySetting(gdCharacterList);

	//. 활성화된 스텔라 스트라이크를 세팅한다.
	int32 maxCount = stellaStrikeRowList.Num();

	if (0 == maxCount /*||*/ /*TESConstantValue::MaxPartyCount > gdCharacterList.Num()*/)
	{
		if (true == _cp_stellaStrikeList.IsValidIndex(0))
		{
			_cp_stellaStrikeList[0]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}

		if (true == _txt_stellStrikeNameList.IsValidIndex(0))
		{
			_txt_stellStrikeNameList[0]->SetGlobalString(TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Party_Change_Not_StellaStrike));
		}

		if (true == _img_stellStrikeIconList.IsValidIndex(0))
		{
			_img_stellStrikeIconList[0]->SetVisibility(ESlateVisibility::Collapsed);
		}

		return;
	}

	int32 attributionIndex = 0;
	for (int32 index = 0; index < maxCount; ++index)
	{
		if (true == stellaStrikeRowList.IsValidIndex(index))
		{
			//. 위젯 활성화한다.
			if (true == _cp_stellaStrikeList.IsValidIndex(index))
			{
				_cp_stellaStrikeList[index]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}

			//. 아이콘 이미지를 테이블에 등록되어 있는 경로로 설정해준다.
			if (true == _img_stellStrikeIconList.IsValidIndex(index))
			{
				FString iconPath = stellaStrikeRowList[index].StellaStrikeSkillIconPath;
				_img_stellStrikeIconList[index]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				TESUtilTexture::SetDynamicTexture(_img_stellStrikeIconList[index], iconPath);
			}
			//. 스텔라 스트라이크 이름을 설정한다.
			if (true == _txt_stellStrikeNameList.IsValidIndex(index))
			{
				_txt_stellStrikeNameList[index]->SetText(FText::FromString(TESUtilTable::GetGlobalString(GetWorld(), stellaStrikeRowList[index].StellaStrikeName)));
			}

			//. 속성 적용
			for (int32 secondIndex = 0; secondIndex < gdCharacterList.Num(); ++secondIndex)
			{
				if (stellaStrikeRowList[index].StellaStrikeSkillType == ETypeStellaStrikeSkill::MixSS)
				{
					break;
				}

				if (gdCharacterList[secondIndex]->GetCharacterAttribute() == stellaStrikeRowList[index].PartyAttributeCondition[0])
				{
					if (true == _img_attributionIcon.IsValidIndex(attributionIndex))
					{
						_img_attributionIcon[attributionIndex]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

						FString resourcePath = TESUtilPath::GetCharacterAttributionIconResourcePath(gdCharacterList[secondIndex]->GetCharacterAttribute());

						TESUtilTexture::SetDynamicTexture(_img_attributionIcon[attributionIndex], resourcePath);
						++attributionIndex;
					}
				}
			}
		}
		//. 속성 아이콘은 총8개. 한줄당 4개씩이므로 다음 for문을 돌때는 4로 고정 시켜준다.
		attributionIndex = TESConstantValue::MaxPartyCount;
	}
}

void UTESUWPartySettingMain::SetRequiredMaterialActivation(int32 stageId)
{
	const FStageRow& stageTableId = SharedTDBManager->GetStageRow(stageId);
	if (false == stageTableId.IsEmpty())
	{
		//. 유니크 어비스일때만 적용
		if (ETypeContents::UniqueAbyss == stageTableId.T_Contents)
		{
			TESCHECK(nullptr != _cp_requiredMaterialChecking);
			_cp_requiredMaterialChecking->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			TArray<int32> activeStageList = SharedUniqueAbyssManager->GetActiveStageList();
			for (int32 activeStage : activeStageList)
			{
				if (activeStage == stageId)
				{
					//. 현재 활성화가 되어 있으면 체크박스
					SetRequiredMaterialCheckState(true);
					return;
				}
			}
			SetRequiredMaterialCheckState(false);
		}
	}
}

void UTESUWPartySettingMain::SetRequiredMaterialCheckState(bool isChecked)
{
	/*
	TESCHECK(nullptr != _chkb_requiredMaterial);
	_chkb_requiredMaterial->SetCheckedState(true == isChecked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	*/

	if (true == ::IsValid(_txt_staminaCount))
	{
		if(true == isChecked)
		{ 
			FString requireMaterialCheck = TEXT("-");
			_txt_staminaCount->SetText(FText::FromString(requireMaterialCheck));
		}
	}
}

void UTESUWPartySettingMain::SetBuffIconButton()
{
	//. 현재 전투력이 추천 전투력보다 크거나 같으면 압도 버프를 활성화 시켜준다.
	TESCHECK(nullptr != _btn_buffIcon);

	const FStageRow& stageRow = SharedTDBManager->GetStageRow(_curStageId);
	if (true  == stageRow.IsEmpty())
	{
		TESLOG(Error, TEXT("stage row is Invalid"));
		return;
	}
	TOptional<FTESPartyInfo> partyInfo = SharedUserDataManager->FindPartyList(_currentPartyUnique);

	//. 파티 정보가 없으므로 가려준다.
	if (false == partyInfo.IsSet())
	{
		_btn_buffIcon->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	FTESPartyInfo currentPartyInfo = partyInfo.GetValue();
	int32 numOfParty = sizeof(currentPartyInfo._characterUnique) / sizeof(int64);

	int32 partyTotalCombatPower = 0;

	for (int32 index = 0; index < numOfParty; ++index)
	{
		TOptional<UTESGDCharacter*> gdCharacter = SharedUserDataManager->FindHaveCharacter(currentPartyInfo._characterUnique[index]);
		if (true == gdCharacter.IsSet())
		{
			partyTotalCombatPower += gdCharacter.GetValue()->GetCharacterBattlePower();
		}
	}

	if(0 < stageRow.SuggestPower)
	{ 
		if (partyTotalCombatPower >= stageRow.SuggestPower)
		{
			_btn_buffIcon->SetVisibility(ESlateVisibility::Visible);
			SharedBattlePartyManager->SetOverPower(true);
		}
		else
		{
			_btn_buffIcon->SetVisibility(ESlateVisibility::Collapsed);
			SharedBattlePartyManager->SetOverPower(false);
		}
	}
	else
	{
		_btn_buffIcon->SetVisibility(ESlateVisibility::Collapsed);
		SharedBattlePartyManager->SetOverPower(false);
	}
}

void UTESUWPartySettingMain::SetRootToolTip(bool isVisible)
{
	TESCHECK(nullptr != _cp_rootToolTip);
	_cp_rootToolTip->SetVisibility(true == isVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UTESUWPartySettingMain::SetRootRecommandedCombatPowerPanelVisible(bool isVisible)
{
	TESCHECK(nullptr != _cp_recommandedCombatPowerPanel);
	_cp_recommandedCombatPowerPanel->SetVisibility(true == isVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UTESUWPartySettingMain::SetReplayStory(bool isReplay)
{
	if (true == ::IsValid(_chkb_replayStory))
	{
		_chkb_replayStory->SetCheckedState((true == isReplay) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	}

	SharedTriggerEventManager->SetReplayStory(isReplay);
}

void UTESUWPartySettingMain::SetRequireStamina(int32 stageId)
{
	if (0 == stageId)
		return;

	const FStageRow& stageRow = SharedTDBManager->GetStageRow(stageId);

	if (true == stageRow.IsEmpty())
	{
		TESLOG(Error, TEXT("Stage Row is Empty"));
		return;
	}

	int32 stamina = 0;
	if (true == stageRow.MaterialCondition.IsValidIndex(0))
	{
		stamina = stageRow.MaterialCondition[0]._count;
	}

	if (0 == stamina)
	{
		if (true == ::IsValid(_hb_stamina))
		{
			_hb_stamina->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	else
	{
		if (true == ::IsValid(_hb_stamina))
		{
			_hb_stamina->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}

		if (true == ::IsValid(_txt_staminaCount))
		{
			_txt_staminaCount->SetText(FText::FromString(FString::FromInt(stamina)));
		}
	}

}

void UTESUWPartySettingMain::SetStartBattleButton(bool isEnabled)
{
	if (true == ::IsValid(_cts_startBattle))
	{
		_cts_startBattle->SetToggleState(isEnabled);
	}
}

void UTESUWPartySettingMain::SetDispersePartyButton(bool isEnabled)
{
	if (true == ::IsValid(_cts_disperseParty))
	{
		_cts_disperseParty->SetToggleState(isEnabled);
	}
}

//void UTESUWPartySettingMain::SetCharacterSwapButtonEnable(bool isEnabled)
//{
//	TESCHECK(nullptr != _btn_characterSwap);
//	_btn_characterSwap->SetIsEnabled(isEnabled);
//}

void UTESUWPartySettingMain::SetSweepButton(ETESSweepButtonState sweepButtonState)
{
	switch (sweepButtonState)
	{
	case ETESSweepButtonState::Activation:
		{
			if (true == ::IsValid(_cts_sweep))
			{
				_cts_sweep->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				_cts_sweep->SetToggleState(true);
			}
		}
		break;

	case ETESSweepButtonState::Deactivation:
		{
			if (true == ::IsValid(_cts_sweep))
			{
				_cts_sweep->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				_cts_sweep->SetToggleState(false);
			}
		}
		break;

	case ETESSweepButtonState::Collapsed:
		{
			if (true == ::IsValid(_cts_sweep))
			{
				_cts_sweep->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
		break;

	default:
		{
			TESLOG(Error, TEXT("Not Supported Type : %s"), *TESUtilString::GetEnumAsString(sweepButtonState));
		}
		break;
	}
}

void UTESUWPartySettingMain::SetSweepButtonBySweepTicketCount(int32 sweepTicketCount)
{
	if (true == ::IsValid(_txt_sweep_on_sweepTicketCount))
	{
		_txt_sweep_on_sweepTicketCount->SetText(FText::FromString(FString::FormatAsNumber(sweepTicketCount)));
	}

	if (true == ::IsValid(_txt_sweep_off_sweepTicketCount))
	{
		_txt_sweep_off_sweepTicketCount->SetText(FText::FromString(FString::FormatAsNumber(sweepTicketCount)));
	}
}

void UTESUWPartySettingMain::SetRemainCount(ETESCommonRemainCountType remainCountType)
{
	switch (remainCountType)
	{
		case ETESCommonRemainCountType::IllusionTemple:
			{
				if (true == ::IsValid(_ws_consume))
				{
					_ws_consume->SetActiveWidgetIndex(1);
				}

				if (true == ::IsValid(_btn_buffIcon))
				{
					_btn_buffIcon->SetVisibility(ESlateVisibility::Hidden);
				}

				if (true == ::IsValid(_cp_recommandedCombatPowerPanel))
				{
					_cp_recommandedCombatPowerPanel->SetVisibility(ESlateVisibility::Hidden);
				}

				if (true == ::IsValid(_cp_stageInfo))
				{
					_cp_stageInfo->SetVisibility(ESlateVisibility::Hidden);
				}

				if (true == ::IsValid(_commonRetryWidget))
				{
					FTESCommonRetryCountData commonRetryCountData = {};
					commonRetryCountData._isCanReChargeCount = false;
					commonRetryCountData._type = ETESCommonRemainCountType::IllusionTemple;
					commonRetryCountData._remainCount = SharedIllusionTempleManager->GetCurrentRemainCount();
					commonRetryCountData._maxCount = TESUtilTable::GetConstantValueByInt(EConstantIntName::IllusionTemple_BattleCount);
					commonRetryCountData._remainTime = SharedIllusionTempleManager->GetCurrentRemainTime();

					_commonRetryWidget->InitWidget();
					_commonRetryWidget->OpenWidget();
					_commonRetryWidget->SetWidget(commonRetryCountData);
				}
			}
			break;
		default:
			{
				if (true == ::IsValid(_ws_consume))
				{
					_ws_consume->SetActiveWidgetIndex(0);
				}

				if (true == ::IsValid(_cp_recommandedCombatPowerPanel))
				{
					if(false == _curStageRow.IsEmpty())
					{ 
						_cp_recommandedCombatPowerPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
					}
					else
					{
						_cp_recommandedCombatPowerPanel->SetVisibility(ESlateVisibility::Collapsed);
					}
				}

				if (true == ::IsValid(_cp_stageInfo))
				{
					_cp_stageInfo->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				}

				SetBuffIconButton();
			}
			break;
	}
}

float UTESUWPartySettingMain::GetCameraLocationY() const
{
	return _cameraLocationY;
}

void UTESUWPartySettingMain::CharacterSwapCanceled()
{
	if (true == _isCharacterSwapping)
	{
		_isCharacterSwapping = false;

		TArray<UTESUWPartySettingCharSlotInfo*> characterSlotInfo = SharedPartySettingManager->GetCharacterInfoSlot();

		//. 캐릭터 정보창을 다 켜준다.
		for (UTESUWPartySettingCharSlotInfo* characterSlot : characterSlotInfo)
		{
			if(nullptr != characterSlot)
				characterSlot->OpenWidget();
		}

		TArray<UTESUWPartySettingSwapSlotInfo*> swapSlotInfo = SharedPartySettingManager->GetCharacterSwapSlotList();

		/*if (nullptr != _txt_swapSlot && nullptr != _txt_swapEndSlot)
		{
			_txt_swapEndSlot->SetVisibility(ESlateVisibility::Hidden);
			_txt_swapSlot->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}*/

		for (int32 index = 0; index < swapSlotInfo.Num(); ++index)
		{
			//. 화살표를 없애준다.
			TESCHECK_CONTINUE(nullptr != swapSlotInfo[index]);
			swapSlotInfo[index]->SetRootMainVisibile(false);
		}

		SharedPartySettingManager->SetSwapComplete();
	}
}

void UTESUWPartySettingMain::SetCharacterSwapEnd()
{
	CharacterSwapCanceled();

	//. 서버에 갱신 시켜주는 요청을 한다.
	FTESPartyCharacterInfo partyCharacterInfo;

	partyCharacterInfo._unique = _currentPartyUnique;

	for (int32 index = 0; index < TESConstantValue::MaxPartyCount; ++index)
	{
		partyCharacterInfo._characterUnique[index] = _currentCharacterUniqueList[index];
	}

	SharedNetworkManager->CLGW_QRY_UPDATE_PARTY(partyCharacterInfo);
}

void UTESUWPartySettingMain::SetCurrentGroupName()
{
	//. 탭을 찾는다.(현재 들어온 탭)
	TOptional<FTESPartyGroupInfo> partyGroupTabInfo = SharedUserDataManager->FindPartyGroupList(_uniqueTabId);

	//. 그룹탭 이름
	if (true == partyGroupTabInfo.IsSet())
	{
		FString tempString = partyGroupTabInfo.GetValue()._name;
		_partyPrestTabName = tempString.TrimStartAndEnd();
	}

	SetPresetTabName();
}

void UTESUWPartySettingMain::SetStartAvailable(bool canBeStarted)
{
	_isStarted = !canBeStarted;
}

void UTESUWPartySettingMain::SetAppearAnim()
{
	if (true == _isPlayingAppearAnim)
	{
		return;
	}

	_isPlayingAppearAnim = true;

	PlayAppearAnim(FOnTESAction::CreateLambda([=]()
	{		
		_isPlayingAppearAnim = false;
	}));
}

void UTESUWPartySettingMain::PlayAppearAnim(FOnTESAction finishAction)
{
	//. 추후 위젯 애니메이션 작업하고 나면 주석 풀어야 함.
	float finishTime = PlayWGAnimation(TEXT("AppearUI"));
	finishTime = FMath::Clamp(finishTime, 0.01f, finishTime);

	GetWorld()->GetTimerManager().ClearTimer(_th_appearAnim);
	GetWorld()->GetTimerManager().SetTimer(_th_appearAnim, FTimerDelegate::CreateLambda([=]()
	{
		finishAction.ExecuteIfBound();
	}), finishTime, false);
}

bool UTESUWPartySettingMain::CheckIfStartStage()
{
	TOptional<FTESPartyInfo> partyUnique = SharedUserDataManager->FindPartyList(_currentPartyUnique);

	if (false == partyUnique.IsSet())
	{
		//. 메세지 띄워준다.
		SharedPopupManager->ShowMessageBox(TEXT("Can't start without party member. Please add party member.")
			, FLinearColor::Red
			, 3.0f);
		return false;
	}
	else
	{
		bool isEmpty = true;

		//. 파티에 아무도 없으면 리턴
		for (int32 index = 0; index < TESConstantValue::MaxPartyCount; ++index)
		{
			if (0 == partyUnique.GetValue()._characterUnique[index])
			{
				continue;
			}

			isEmpty = false;
		}

		if (true == isEmpty)
		{
			SharedPopupManager->ShowMessageBox(TEXT("Can't start without party member. Please add party member.")
											, FLinearColor::Red
											, 3.0f);

			return false;
		}

		return true;
	}
}

void UTESUWPartySettingMain::StartTriggerEvent(const FStageRow& stageRow)
{
	const FTriggerEventRow& triggerEvent = SharedTDBManager->GetTriggerEventRow(stageRow.CovnersationBeforeStageBegin);
	if (true == triggerEvent.IsEmpty())
	{
		TESLOG(Error, TEXT("trigger event table is invalid"));
		return;
	}


	//. 대화에 대한 정보를 세팅합니다.
	ETESPartySettingContents contents = SharedPartySettingManager->GetEntryRoute();
	SharedSaveGameManager->SetLastPlayedPartyIdByContents(contents, _currentPartyUnique);

	FTESConversationInitData initData = {};
	initData._startFrom = ETESConversationStartFrom::PartySetting;
	initData._stageId = _curStageId;
	initData._nextTriggerEventId = triggerEvent.NextEventId;
	bool isPlayed = false;

	SharedUniqueAbyssManager->ClearData();
	SharedPartySettingManager->ClearScene();
	SharedTriggerEventManager->TriggerEventActivation(stageRow.CovnersationBeforeStageBegin, nullptr, isPlayed, initData);
}

void UTESUWPartySettingMain::StartBattle()
{
	SharedSaveGameManager->SetLastPlayedMainStoryStage(_curStageId);


	ETESPartySettingContents contents = SharedPartySettingManager->GetEntryRoute();
	SharedSaveGameManager->SetLastPlayedPartyIdByContents(contents, _currentPartyUnique);

	//. 전투 진입을 진행합니다.
	SharedStageManager->Reset();
	SharedStageManager->SetUseGameFlow(true);
	SharedStageManager->SetStage(_curStageId);
	SharedUniqueAbyssManager->ClearData();
	SharedPartySettingManager->ClearScene();
	SharedStellaStrikeManager->SetStellaStrikeDatas();
	_isStarted = false;

	//. 화면 전환...
	//. ( "파티세팅 -> 전투"로 이동시에, DontUserTrace를 하여 PartySetting의 발자취를 남기지 않도록 합니다. )
	//.   - 이유 : 전투 복귀 이후 유저 발자취 목록으로 파티세팅이 남아있으면 이전으로 이동할 위치가 파티세팅이 되기 때문입니다.
	FTESSceneTransitionData sceneTransitionData = {};
	sceneTransitionData._physicalSceneType = ETESPhysicalGameSceneType::LoadingBattle;
	sceneTransitionData._logicalSceneType = ETESLogicalGameSceneType::LoadingBattle;
    sceneTransitionData._fadeAtrribute._fadeType = ETESSceneFadeType::WidgetAnimation;
    sceneTransitionData._fadeAtrribute._fadeColor = FColor::Black;
	sceneTransitionData._isDontUserTrace = true;
	SharedSceneManager->ChangeGameScene(sceneTransitionData);
}

void UTESUWPartySettingMain::RefreshSweepButton()
{
	//. 소탕이 불가능한 스테이지인 경우, 소탕 버튼 자체를 보여주지 않습니다.
	if (false == _curStageRow.IsSweepable)
	{
		SetSweepButton(ETESSweepButtonState::Collapsed);

		return;
	}

	//. 해당 스테이지의 컨텐츠 타입에 따라 각각 다른 처리를 합니다.
	ETypeContents stageContentsType = _curStageRow.T_Contents;
	switch (stageContentsType)
	{
	case ETypeContents::MainStory:
		{
			//. 모든 도전과제를 클리어하지 않았을 경우, 소탕 버튼을 비활성화합니다.
			if (false == TESUtils::IsAllClearStageAchivement(_curStageRow.Id))
			{
				SetSweepButton(ETESSweepButtonState::Deactivation);

				return;
			}

			//. 파티 입장 조건을 충족하지 않아 만족하지 않는 경우, 소탕 버튼을 비활성화합니다.
			if (false == CheckPartyConditions(_currentCharacterUniqueList, _curStageRow.PartyCondition))
			{
				SetSweepButton(ETESSweepButtonState::Deactivation);

				return;
			}

			//. 소탕권이 없거나, 재료가 부족하여 소탕 1회도 부족한 경우, 소탕 버튼을 비활성화합니다.
			if (false == SharedSweepManager->CheckSweepableCount(_curStageId, 1))
			{
				SetSweepButton(ETESSweepButtonState::Deactivation);

				return;
			}

			//. 모든 조건을 만족하는 경우, 소탕 버튼을 보여줍니다.
			SetSweepButton(ETESSweepButtonState::Activation);
		}
		break;

	case ETypeContents::UniqueAbyss:
		{
			//. 소탕이 불가능한 스테이지인 경우, 소탕 버튼 자체를 보여주지 않습니다.
			if (false == _curStageRow.IsSweepable)
			{
				SetSweepButton(ETESSweepButtonState::Collapsed);

				return;
			}

			//. 모든 도전과제를 클리어하지 않았을 경우, 소탕 버튼을 비활성화합니다.
			if (false == TESUtils::IsAllClearStageAchivement(_curStageRow.Id))
			{
				SetSweepButton(ETESSweepButtonState::Deactivation);

				return;
			}

			//. 파티 입장 조건을 충족하지 않아 만족하지 않는 경우, 소탕 버튼을 비활성화합니다.
			if (false == CheckPartyConditions(_currentCharacterUniqueList, _curStageRow.PartyCondition))
			{
				SetSweepButton(ETESSweepButtonState::Deactivation);

				return;
			}

			//. 소탕권이 없거나, 재료가 부족하여 소탕 1회도 부족한 경우, 소탕 버튼을 비활성화합니다.
			if (false == SharedSweepManager->CheckSweepableCount(_curStageId, 1))
			{
				SetSweepButton(ETESSweepButtonState::Deactivation);

				return;
			}
			
			//. 해당 유니크 어비스에 스테이지의 도전횟수를 부족한 경우, 소탕 버튼을 비활성화합니다.
			int32 remainChallengeCount = SharedUniqueAbyssManager->GetRemainChallengeCountByStageId(_curStageId);
			if (remainChallengeCount <= 0)
			{
				SetSweepButton(ETESSweepButtonState::Deactivation);

				return;
			}

			//. 해당 유니크 어비스의 스테이지가 만료된 스테이지인지 경우, 소탕 버튼을 비활성화 합니다.
			bool isExpired = SharedUniqueAbyssManager->CheckIfExpiredUniqueAbyssByStageId(_curStageId);
			if (true == isExpired)
			{
				SetSweepButton(ETESSweepButtonState::Deactivation);

				return;
			}

			//. 모든 조건을 만족하는 경우, 소탕 버튼을 보여줍니다.
			SetSweepButton(ETESSweepButtonState::Activation);
		}
		break;

	//. 지원하지 않는 컨텐츠 타입인 경우, 메세지를 출력 후 보여주지 않습니다.
	default:
		{
			TESLOG(Error, TEXT("Not Supported Stage Contents Type : %s"), *TESUtilString::GetEnumAsString(stageContentsType));

			SetSweepButton(ETESSweepButtonState::Collapsed);
		}
		break;
	}
}

void UTESUWPartySettingMain::RefreshSweepButtonBySweepTicketCount()
{
	SetSweepButtonBySweepTicketCount(SharedSweepManager->GetHaveSweepTickCount());
}

void UTESUWPartySettingMain::RefreshStartBattleButton()
{
	//. 해당 스테이지가 유니크 어비스인 경우....
	const FStageRow& stageTableId = SharedTDBManager->GetStageRow(_curStageId);
	if (false == stageTableId.IsEmpty())
	{
		if (ETypeContents::UniqueAbyss == stageTableId.T_Contents)
		{
			//. 해당 유니크 어비스에 스테이지의 도전횟수가 부족한 경우, 전투시작 버튼을 비활성화 합니다.
			int32 challengeableCount = SharedUniqueAbyssManager->GetRemainChallengeCountByStageId(_curStageId);
			if (challengeableCount <= 0)
			{
				SetStartBattleButton(false);

				return;
			}
		}
	}

	//. 해당 파티 유니크를 사용하는 파티 정보를 찾습니다.
	TOptional<FTESPartyInfo> partyInfo = SharedUserDataManager->FindPartyList(_currentPartyUnique);

	//. 해당 파티 정보가 존재하는 경우, 해당 파티의 전투력을 확인합니다.
	//. ( ※ 파티에 유닛 정보가 없는 경우, 비어있는 파티 슬롯일 수도 있습니다. )
	if (true == partyInfo.IsSet())
	{
		int32 partyTotalCombatPower = 0;

		FTESPartyInfo currentPartyInfo = partyInfo.GetValue();
		int32 numOfParty = sizeof(currentPartyInfo._characterUnique) / sizeof(int64);
		for (int32 index = 0; index < numOfParty; ++index)
		{
			TOptional<UTESGDCharacter*> gdCharacter = SharedUserDataManager->FindHaveCharacter(currentPartyInfo._characterUnique[index]);
			if (true == gdCharacter.IsSet())
			{
				partyTotalCombatPower += gdCharacter.GetValue()->GetCharacterBattlePower();
			}
		}

		//. 전투력이 0이 아닌 경우, 전투 시작 버튼을 활성화 상태로 만듭니다.
		if (partyTotalCombatPower > 0)
		{
			SetStartBattleButton(true);
		}
		//. 전투력이 0인 경우, 전투시작 버튼을 비활성화 상태로 세팅합니다.
		else
		{
			SetStartBattleButton(false);
		}
	}
	//. 파티가 존재하지 않는 경우, 전투 버튼은 비활성화 상태로 만듭니다.
	else
	{
		SetStartBattleButton(false);
	}
}

void UTESUWPartySettingMain::RefreshCharacterSlotInfo()
{
	SharedPartySettingManager->RefreshCharacterSlotInfo();
}

void UTESUWPartySettingMain::RefreshRequiredMaterialList()
{
	SetRequiredMaterialList(_curStageId);
}

bool UTESUWPartySettingMain::CheckPartyConditions(TArray<int64> characterUniqueList, TArray<FTESPartyCondition> partyConditionTypes)
{
	//. 파티 입장 조건 하나하나를 순회하면서 조건에 충족하는지 확인합니다.
	for (FTESPartyCondition partyConditionType : partyConditionTypes)
	{
		//. 단, 하나라도 만족하지 않는다면 충족하지 않는다는 것을 알립니다.
		if (false == TESUtils::CheckPartyConditionOnStage(_currentCharacterUniqueList, partyConditionType))
		{
			return false;
		}
	}

	//. 모두 만족하였다면 만족한 것을 알립니다.
	return true;
}

void UTESUWPartySettingMain::OnEvent_BackToPrevious()
{
	//. 팝업이 출현하는 중이라면 무시합니다.
	if (true == _isPlayingAppearAnim)
	{
		return;
	}

	//. 팝업이 사라지는 중이라면 무시합니다.
	if (true == _isPlayingDisappearAnim)
	{
		return;
	}

	if (true == SharedSceneManager->IsRegularRoute())
	{
		_isBackToPrevious = true;
		_isDispersingParty = false;
		_isAutoSettingParty = false;

		TMap<int32, ATESCharacterPartySetting*> characterMap = SharedPartySettingManager->GetCharacterMap();
		GamePacket::FTESPartyCharacterInfo partyInfo;
		partyInfo._unique = GameDefine::PARTY_START_UNIQUE;

		if (0 >= characterMap.Num())
		{
			SharedNetworkManager->CLGW_QRY_UPDATE_PARTY(partyInfo);

			return;
		}

		//. 서버에 파티 변경을 요청한다.
		partyInfo._unique = _currentPartyUnique;

		for (int32 index = 1; index <= characterMap.Num(); ++index)
		{
			if (nullptr != characterMap[index])
			{
				partyInfo._characterUnique[index - 1] = characterMap[index]->GetCharacterUnique();
			}
			else
			{
				partyInfo._characterUnique[index - 1] = 0;
			}
		}

		SharedNetworkManager->CLGW_QRY_UPDATE_PARTY(partyInfo);
	}
}

void UTESUWPartySettingMain::OnEvent_BackToHome()
{
	//. 팝업이 출현하는 중이라면 무시합니다.
	if (true == _isPlayingAppearAnim)
	{
		return;
	}

	//. 팝업이 사라지는 중이라면 무시합니다.
	if (true == _isPlayingDisappearAnim)
	{
		return;
	}

	//. 화면 관리자의 홈을 호출합니다.
	SharedSceneManager->OnEvent_ContentsHome();

	_isBackToHome = true;
	_isDispersingParty = false;
	_isAutoSettingParty = false;

	TMap<int32, ATESCharacterPartySetting*> characterMap = SharedPartySettingManager->GetCharacterMap();
	GamePacket::FTESPartyCharacterInfo partyInfo;
	partyInfo._unique = GameDefine::PARTY_START_UNIQUE;

	if (0 >= characterMap.Num())
	{
		SharedNetworkManager->CLGW_QRY_UPDATE_PARTY(partyInfo);

		return;
	}

	//. 서버에 파티 변경을 요청한다.
	partyInfo._unique = _currentPartyUnique;

	for (int32 index = 1; index <= characterMap.Num(); ++index)
	{
		if (nullptr != characterMap[index])
		{
			partyInfo._characterUnique[index - 1] = characterMap[index]->GetCharacterUnique();
		}
		else
		{
			partyInfo._characterUnique[index - 1] = 0;
		}
	}

	SharedNetworkManager->CLGW_QRY_UPDATE_PARTY(partyInfo);
}

void UTESUWPartySettingMain::OnEvent_RenameCurrentParty()
{
	TOptional<FTESPartyInfo> partyInfo = SharedUserDataManager->FindPartyList(_currentPartyUnique);
	uint16 currentPartyUnique = 0;
	FString currentPartyName = _currentPartyName;

	if (true == partyInfo.IsSet())
	{
		currentPartyUnique = partyInfo.GetValue()._unique;
		FString tempString = partyInfo.GetValue()._name;

		currentPartyName = tempString.TrimStartAndEnd();
		if (TEXT("") == currentPartyName)
		{
			//. 파티는 만들어져있는데 이름이 빈칸이면 디폴트 이름으로 세팅한다.
			currentPartyName = _currentPartyName;
		}
	}

	//. 파티 이름 변경
	UTESUWPartyPresetRenamePartyPopup* partyPresetRenamePopup = Cast<UTESUWPartyPresetRenamePartyPopup>(SharedPopupManager->CreatePopupWidget(TEXT("WBP_PartyPresetRenamePartyPopup"), TEXT("Common/WBP/PartySetting")));
	TESCHECK(nullptr != partyPresetRenamePopup);
	partyPresetRenamePopup->InitWidget();
	partyPresetRenamePopup->OpenWidget();
	partyPresetRenamePopup->SetWidget(currentPartyUnique, currentPartyName);
}

void UTESUWPartySettingMain::OnEvent_OpenPartyPreset()
{
	//. 파티 프리셋 이동
	UTESUWPartyPresetMain* partyPresetMain = Cast<UTESUWPartyPresetMain>(SharedPopupManager->CreatePopupWidget(TEXT("WBP_PartyPresetMain"), TEXT("Common/WBP/PartySetting")));
	TESCHECK(nullptr != partyPresetMain);
	partyPresetMain->InitWidget();
	partyPresetMain->OpenWidget();
	partyPresetMain->SetWidget(_currentPartyUnique);
}

void UTESUWPartySettingMain::OnEvent_SwapSlot()
{
	TMap<int32, ATESCharacterPartySetting*> characterMap = SharedPartySettingManager->GetCharacterMap();
	_currentCharacterUniqueList.Empty();

	for (TPair<int32, ATESCharacterPartySetting*> child : characterMap)
	{
		if (nullptr != child.Value)
		{
			_currentCharacterUniqueList.Add(child.Value->GetCharacterId());
		}
		else
		{
			_currentCharacterUniqueList.Add(0);
		}
	}

	TArray<UTESUWPartySettingSwapSlotInfo*> swapSlotInfo = SharedPartySettingManager->GetCharacterSwapSlotList();
	TArray<UTESUWPartySettingCharSlotInfo*> characterSlotInfo = SharedPartySettingManager->GetCharacterInfoSlot();

	int32 numofLockedSlot = SharedPartySettingManager->GetNumberOfLockedSlot();

	if (TESConstantValue::MaxPartyCount == numofLockedSlot)
	{
		FString message = *TESUtilTable::GetGlobalString(GetWorld(), EGStringID::ForcedCharacter_Stage_Party_NoSlot_Change_Tooltip);
		SharedPopupManager->ShowMessageBox(message, FLinearColor::White);

		return;
	}

	//. 기본 세팅을 해준다.
	//. 캐릭터가 있는곳은 화살표를 띄우고 없는곳은 안띄운다.
	if (false == _isCharacterSwapping)
	{
		/*if (nullptr != _txt_swapSlot && nullptr != _txt_swapEndSlot)
		{
			_txt_swapEndSlot->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			_txt_swapSlot->SetVisibility(ESlateVisibility::Hidden);
		}*/

		for (int32 index = 0; index < swapSlotInfo.Num(); ++index)
		{
			//. 화살표를 띄운다.(클릭 가능한 상태로 만든다.)
			TESCHECK_CONTINUE(nullptr != swapSlotInfo[index]);
			swapSlotInfo[index]->SetRootMainVisibile(true);
			swapSlotInfo[index]->SetButtonVisible(true);
			swapSlotInfo[index]->ShowSelectedArrow(true);

			if (0 == _currentCharacterUniqueList[index])
			{
				//. 캐릭터가 없는 경우에는 빈 슬롯으로 처리 시킨다.
				swapSlotInfo[index]->SetButtonVisible(false);
				swapSlotInfo[index]->ShowSelectedArrow(false);
			}
		}

		//. 강제 출격이 된 슬롯이 있는지 확인하고, 있으면 처리해준다.
		for (int32 index = 0; index < numofLockedSlot; ++index)
		{
			swapSlotInfo[index]->SetSlotLocking(true);
		}

		//. 캐릭터 정보창을 다 꺼준다.
		for (UTESUWPartySettingCharSlotInfo* characterSlot : characterSlotInfo)
		{
			characterSlot->CloseWidget();
		}

		//. 세팅이 끝나면 교체중 상태로 변경
		_isCharacterSwapping = true;
	}
	else
	{
		//. 교체 완료 버튼 눌렀을때
		SetCharacterSwapEnd();
	}
}

void UTESUWPartySettingMain::OnEvent_DisperseCurrentParty()
{
	_isDispersingParty = true;

	//. 파티 변경을 알리는 팝업
	FTESMessageBoxInitData messageBoxInitData = {};
	messageBoxInitData._messageBoxType = ETESMessageBoxType::Warning;
	messageBoxInitData._messageBoxSubType = ETESMessageBoxSubType::TwoButton;
	messageBoxInitData._messageHeaderString = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Party_Disperse_PopUp_Title);
	messageBoxInitData._messageContextString = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Party_Disperse_PopUp_Message);
	messageBoxInitData._messageButton1String = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_Cancel);
	messageBoxInitData._messageButton1Callback = nullptr;
	messageBoxInitData._messageButton2String = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_Confirm);
	messageBoxInitData._messageButton2Callback = FOnTESAction::CreateLambda([&]()
	{
		//. 파티를 해산 시킨다.
		FTESPartyCharacterInfo partyCharacterInfo;

		partyCharacterInfo._unique = _currentPartyUnique;

		for (int32 index = 0; index < TESConstantValue::MaxPartyCount; ++index)
		{
			partyCharacterInfo._characterUnique[index] = _currentCharacterUniqueList[index];
		}

		int32 numOfLockedSlot = SharedPartySettingManager->GetNumberOfLockedSlot();

		//. 전부다 0으로 초기화해준다. -> 강제 출격이 있는 파티는 해당 출격하는 슬롯들만 제외하고 초기화 해준다.
		for (int32 index = 0; index < TESConstantValue::MaxPartyCount; ++index)
		{
			if (index < numOfLockedSlot)
				continue;

			partyCharacterInfo._characterUnique[index] = 0;
		}

		SharedNetworkManager->CLGW_QRY_UPDATE_PARTY(partyCharacterInfo);
	});

	SharedPopupManager->ShowMessageBox(messageBoxInitData);
}

void UTESUWPartySettingMain::OnEvent_AutoSettingCurrentParty()
{
	_isAutoSettingParty = true;

	UTESUWPartySettingAutomatically* partyAutoSettingWidget = SharedPopupManager->CreatePopupWidget<UTESUWPartySettingAutomatically>(TEXT("WBP_PartySettingAutomatically"), TEXT("Common/WBP/PartySetting"));
	if (false == ::IsValid(partyAutoSettingWidget))
	{
		TESLOG(Error, TEXT("Invalid partyAutoSettingWidget."));

		return;
	}

	partyAutoSettingWidget->InitWidget();
	partyAutoSettingWidget->OpenWidget();
	partyAutoSettingWidget->SetWidget(_currentPartyUnique, _curStageId);
}

void UTESUWPartySettingMain::OnEvent_OpenStageInfoDetail()
{
	if (0 == _curStageId)
		return;

	const FStageRow& stageRow = SharedTDBManager->GetStageRow(_curStageId);

	if (true == stageRow.IsEmpty())
		return;

	UTESUWMainStoryStageInfoPopup* mainStoryStageInfoPopup = SharedPopupManager->CreatePopupWidget<UTESUWMainStoryStageInfoPopup>(TEXT("WBP_MainStoryStageInfoPopup"), TEXT("MainStory/WBP/StageInformation"));
	if (false == ::IsValid(mainStoryStageInfoPopup))
	{
		TESLOG(Error, TEXT("Invalid mainStoryStageInfoWidget."));

		return;
	}

	mainStoryStageInfoPopup->InitWidget();
	mainStoryStageInfoPopup->OpenWidget();
	mainStoryStageInfoPopup->SetWidget(_curStageId);
}

void UTESUWPartySettingMain::OnEvent_OpenStellaStrikeInfoPopup()
{
	UTESUWPartySettingSSInfoPopup* stellaStrikeInfoPopup = SharedPopupManager->CreatePopupWidget<UTESUWPartySettingSSInfoPopup>(TEXT("WBP_PartyPreset_StellaStrikeInfoPopup"), TEXT("Common/WBP/PartySetting"));
	if (false == ::IsValid(stellaStrikeInfoPopup))
	{
		TESLOG(Error, TEXT("Invalid stellaStrikeInfoPopup."));

		return;
	}

	stellaStrikeInfoPopup->InitWidget();
	stellaStrikeInfoPopup->OpenWidget();
	stellaStrikeInfoPopup->SetWidget();
}

void UTESUWPartySettingMain::OnEvent_StartBattle()
{
	if (true == SharedSceneManager->IsSwitchingGameScene())
	{
		return;
	}

	if (true == _isStarted)
	{
		return;
	}

	if (0 >= _curStageId)
	{
		return;
	}

	//. 파티내에 같은 캐릭터 아이디가 있는지 확인(중복 체크)
	if (true == SharedPartySettingManager->CheckIfThereIsSameCharacterIdInParty(_currentPartyUnique))
	{
		FTESMessageBoxInitData messageBoxInitData = {};
		messageBoxInitData._messageBoxType = ETESMessageBoxType::Normal;
		messageBoxInitData._messageBoxSubType = ETESMessageBoxSubType::OneButton;
		messageBoxInitData._messageHeaderString = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_Notice);
		messageBoxInitData._messageContextString = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_Party_CantSameChar);
		messageBoxInitData._messageButton2String = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_Confirm);
		messageBoxInitData._messageButton2Callback = nullptr;

		SharedPopupManager->ShowMessageBox(messageBoxInitData);
		return;
	}
	

	//. 파티편성이 스테이지 입장조건에 맞는지 확인한다.
	//. 파티 입장 조건이 알맞지 않은 경우, "파티 입장 조건을 충족해주십시오."라는 메세지를 출력합니다.
	if (false == CheckPartyConditions(_currentCharacterUniqueList, _curStageRow.PartyCondition))
	{
		FTESMessageBoxInitData messageBoxInitData = {};
		messageBoxInitData._messageBoxType = ETESMessageBoxType::Normal;
		messageBoxInitData._messageBoxSubType = ETESMessageBoxSubType::OneButton;
		messageBoxInitData._messageHeaderString = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_Notice);
		messageBoxInitData._messageContextString = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_PartyCondition_insatiable);
		messageBoxInitData._messageButton2String = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_Confirm);
		messageBoxInitData._messageButton2Callback = nullptr;

		SharedPopupManager->ShowMessageBox(messageBoxInitData);

		return;
	}

	//. 재료가 있는지 확인한다.
	const FStageRow& stageTableId = SharedTDBManager->GetStageRow(_curStageId);
	if (true == stageTableId.IsEmpty())
	{	
		TESLOG(Error, TEXT("Stage Table Id is Invalid"));
		return;
	}

	//. 유니크 어비스 스테이지면 상태 체크를 하고 팝업을 띄우거나 스테이지를 진행한다.
	if (ETypeContents::UniqueAbyss == stageTableId.T_Contents)
	{
		//. 해당 유니크 어비스에 스테이지의 도전횟수가 부족한 경우, 센터 메세지("도전 횟수를 모두 소진하였습니다.")를 출력합니다.
		int32 challengeableCount = SharedUniqueAbyssManager->GetRemainChallengeCountByStageId(_curStageId);
		if (challengeableCount <= 0)
		{
			SharedPopupManager->ShowMessageBox(TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Sweep_UniqueAbyss_NotEnter));

			return;
		}

		_isStarted = true;

		//. 먼저 노티스 팝업을 띄우고, 만약 재료가 들어가있는 상태라면 팝업을 바로 띄우지 않고 바로 배틀시작
		TArray<int32> uniqueAbyssActiveStageList = SharedUniqueAbyssManager->GetActiveStageList();
		for (int32 activeList : uniqueAbyssActiveStageList)
		{
			//. 현재 활성화가 되어 있으면 바로 시작.
			if (activeList == _curStageId)
			{
				//. 시작하기전에 최종적으로 해당 유니크어비스가 종료되었는지 확인한다.
				int32 uniqueAbyssId = SharedUniqueAbyssManager->GetUniqueAbyssIdByStageId(_curStageId);

				TOptional<FTESUniqueAbyssInfo> uniqueAbyssInfo = SharedUniqueAbyssManager->GetUniqeAbyssInfoByAbyssId(uniqueAbyssId);

				if (true == uniqueAbyssInfo.IsSet())
				{
					//. 남은시간을 확인하고 만료가 되었으면 종료팝업을 띄우고 메인으로 보내준다.
					int64 remainingTime = SharedTimeManager->GetRemainingTimeLeadingUpToTimeStamp(uniqueAbyssInfo.GetValue()._expireTimeStamp);

					if (0.0f >= remainingTime)
					{
						const FUniqueAbyssRow& abyssTableRow = SharedTDBManager->GetUniqueAbyssRow(uniqueAbyssId);
						if (true == abyssTableRow.IsEmpty())
						{
							TESLOG(Error, TEXT("Unique Abyss Table is Invalid"));
							return;
						}

						FString uniqueAbyssName = TESUtilTable::GetGlobalString(GetWorld(), abyssTableRow.UniqueAbyssName);

						FString header = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_Notice);
						FString context = TESUtilString::MakeFormatString(*TESUtilTable::GetGlobalString(GetWorld(), EGStringID::UniqueAbyss_EndNotice), uniqueAbyssName);
						FTESMessageBoxInitData messageBoxInitData = {};
						messageBoxInitData._messageBoxType = ETESMessageBoxType::Warning;
						messageBoxInitData._messageBoxSubType = ETESMessageBoxSubType::OneButton;
						messageBoxInitData._messageHeaderString = header;
						messageBoxInitData._messageContextString = context;
						messageBoxInitData._messageButton2String = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_Confirm);
						messageBoxInitData._messageButton2Callback = FOnTESAction::CreateLambda([&]()
						{
								//. 유니크어비스 메인화면으로 보내줍니다.
								FTESSceneTransitionData sceneTransitionData = {};
								sceneTransitionData._physicalSceneType = SharedSceneManager->GetCurrentPhysicalGameSceneType();
								sceneTransitionData._logicalSceneType = ETESLogicalGameSceneType::UniqueAbyss;
								sceneTransitionData._fadeAtrribute._fadeType = ETESSceneFadeType::WidgetAnimation;
								sceneTransitionData._fadeAtrribute._fadeColor = FColor::Black;
								SharedSceneManager->ChangeGameScene(sceneTransitionData);
						});

						SharedPopupManager->ShowMessageBox(messageBoxInitData);
					}
					else
					{
						//. 그외에는 정상적으로 시작 가능합니다.
						SharedNetworkManager->CLGW_QRY_START_STAGE(_curStageId, _currentPartyUnique);
					}
				}

				return;
			}
		}

		//. 활성화가 되어 있지 않으면 팝업을 띄워준다.
		UTESUWUniqueAbyssNoticePopup* noticePopup = SharedPopupManager->CreatePopupWidget<UTESUWUniqueAbyssNoticePopup>(TEXT("WBP_UniqueAbyssNoticeWindow"), TEXT("MainStory/WBP/UniqueAbyss"));
		if (false == ::IsValid(noticePopup))
		{
			TESLOG(Error, TEXT("Invalid noticePopup."));

			return;
		}

		noticePopup->InitWidget();
		noticePopup->OpenWidget();
		noticePopup->SetWidget(_curStageId, _currentPartyUnique);

		_isStarted = false;

		return;

	}
	else if (ETypeContents::IllusionTemple == stageTableId.T_Contents)
	{
		int32 challengeableCount = SharedIllusionTempleManager->GetCurrentRemainCount();
		if (challengeableCount <= 0)
		{
			SharedPopupManager->ShowMessageBox(TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Sweep_UniqueAbyss_NotEnter));

			return;
		}

		FTESMessageBoxInitData messageBoxInitData = {};
		messageBoxInitData._messageBoxType = ETESMessageBoxType::Warning;
		messageBoxInitData._messageBoxSubType = ETESMessageBoxSubType::TwoButton;
		messageBoxInitData._messageHeaderString = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::TeamTag_BattleStart_Popup);
		messageBoxInitData._messageContextString = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::TeamTag_BattleStart_Message);
		messageBoxInitData._messageButton1String = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_Cancel);
		messageBoxInitData._messageButton1Callback = nullptr;
		messageBoxInitData._messageButton2String = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_BattleStart);
		messageBoxInitData._messageButton2Callback = FOnTESAction::CreateLambda([&]()
		{
			_isStarted = true;

			int32 illusionTempleId = SharedIllusionTempleManager->GetCurrentSelectedIllusionTempleId();

			SharedNetworkManager->CLGW_QRY_ILLUSION_TEMPLE_START(illusionTempleId, _currentPartyUnique);
		});

		SharedPopupManager->ShowMessageBox(messageBoxInitData);
	}
	else if (ETypeContents::HeavenTower == stageTableId.T_Contents)
	{
		_isStarted = true;

		//. 메인 스토리일 경우에는 바로 진행해준다.
		SharedNetworkManager->CLGW_QRY_START_STAGE(_curStageId, _currentPartyUnique);
	}
	else if (ETypeContents::MainStory == stageTableId.T_Contents)
	{
		_isStarted = true;

		int32 maxCount = stageTableId.MaterialCondition.Num();
		for (int32 index = 0; index < maxCount; ++index)
		{
			int64 goodsAmount = SharedUserDataManager->GetGoodsTotalCount(stageTableId.MaterialCondition[index]._goodsType
																		, stageTableId.MaterialCondition[index]._itemId);
			int32 needMaterialCount = stageTableId.MaterialCondition[index]._count;

			if (goodsAmount < needMaterialCount)
			{
				SharedStaminaManager->OpenPurchaseStaminaWidget();

				_isStarted = false;

				return;
			}
		}

		//. 메인 스토리일 경우에는 바로 진행해준다.
		SharedNetworkManager->CLGW_QRY_START_STAGE(_curStageId, _currentPartyUnique);
	}
}

void UTESUWPartySettingMain::OnEvent_OpenToolTip()
{
	_isToolTipOpened = !_isToolTipOpened;

	if (true == _isToolTipOpened)
	{
		SetRootToolTip(true);
	}
	else
	{
		SetRootToolTip(false);
	}
}

void UTESUWPartySettingMain::OnEvent_ToggleReplayStory()
{
	if (false == SharedUserDataManager->HasStageClearData(_curStageId))
	{
		return;
	}
	else
	{
		_isPlayStory = (true == _isPlayStory) ? false : true;

		SetReplayStory(_isPlayStory);
	}
}

void UTESUWPartySettingMain::OnEvent_Sweep()
{
	//. 소탕 가능한 스테이지가 아니라면 무시합니다.
	//. ( ※ 해당 경우는 발생하지 않을 것 같습니다만, 혹시라도 UI에서 뚫리더라도 코드적으로 막기 위해 방어코드를 넣어둡니다. )
	if (false == _curStageRow.IsSweepable)
	{
		return;
	}

	//. 도전과제 미달성시, "도전 과제를 모두 달성해주십시오."라는 메세지를 출력합니다.
	if (false == TESUtils::IsAllClearStageAchivement(_curStageRow.Id))
	{
		SharedPopupManager->ShowMessageBox(TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Sweep_Not_Possible_Text_1));

		return;
	}

	//. 파티 입장 조건이 알맞지 않은 경우, "파티 입장 조건을 충족해주십시오."라는 메세지를 출력합니다.
	if (false == CheckPartyConditions(_currentCharacterUniqueList, _curStageRow.PartyCondition))
	{
		SharedPopupManager->ShowMessageBox(TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Sweep_Not_Possible_Text_2));

		return;
	}

	//. 스테이지 입장 재료 부족시, "입장 재료가 부족합니다."라는 메세지를 출력합니다. (1회분!!)
	if (false == SharedSweepManager->CheckStageMaterial(_curStageId, 1))
	{
		SharedPopupManager->ShowMessageBox(TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Sweep_Not_Possible_Text_3));

		return;
	}

	//. 소탕권 1개가 부족한 경우, "기억의 정수가 부족합니다."라는 메세지를 출력합니다. (1회분!!)
	if (false == SharedSweepManager->CheckSweepTicket(1))
	{
		SharedPopupManager->ShowMessageBox(TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Sweep_Not_Possible_Text_4));

		return;
	}

	//. 스테이지 컨텐츠 타입에 따라 소탕 버튼에 대한 각각 다른 행동을 합니다.
	ETypeContents stageContentsType = _curStageRow.T_Contents;
	switch (stageContentsType)
	{
	case ETypeContents::MainStory:
		{
			//. 소탕에 대한 정보를 구성합니다.
			FTESSweepAttribute sweepAttribute = {};
			sweepAttribute._stageId = _curStageId;
			sweepAttribute._uniqueAbyssId = 0;
			sweepAttribute._partyIndex = _currentPartyUnique;
			sweepAttribute._partyCharacterUniques = _currentCharacterUniqueList;

			//. 정상적으로 소탕 위젯을 열 수 있는 상황인 경우, 열어줍니다.
			UTESUWSweep* sweepWidget = SharedPopupManager->CreatePopupWidget<UTESUWSweep>(TEXT("WBP_Sweep"), TEXT("Sweep/WBP"));
			TESCHECK(nullptr != sweepWidget);
			sweepWidget->InitWidget();
			sweepWidget->OpenWidget();
			sweepWidget->SetWidget(sweepAttribute);
		}
		break;

	case ETypeContents::UniqueAbyss:
		{
			int32 uniqueAbyssId = SharedUniqueAbyssManager->GetUniqueAbyssIdByStageId(_curStageId);

			//. 해당 유니크 어비스에 스테이지의 도전횟수가 부족한 경우, 센터 메세지("도전 횟수를 모두 소진하였습니다.")를 출력합니다.
			int32 challengeableCount = SharedUniqueAbyssManager->GetRemainChallengeCountByStageId(_curStageId);
			if (challengeableCount <= 0)
			{
				SharedPopupManager->ShowMessageBox(TESUtilTable::GetGlobalString(GetWorld(), EGStringID::TeamTag_BattleCount_Message));

				return;
			}

			//. 해당 유니크 어비스의 스테이지가 만료된 스테이지인지 경우, 유니크 어비스 화면으로 이동한다는 메세지 박스를 출력합니다.
			bool isExpired = SharedUniqueAbyssManager->CheckIfExpiredUniqueAbyssByStageId(_curStageId);
			if (true == isExpired)
			{
				const FUniqueAbyssRow& uniqueAbyssRow = SharedTDBManager->GetUniqueAbyssRow(uniqueAbyssId);
				if (true == uniqueAbyssRow.IsEmpty())
				{
					TESLOG(Error, TEXT("Invalid UniqueAbyssRow : %d"), uniqueAbyssId);

					return;
				}

				FString uniqueAbyssName = TESUtilTable::GetGlobalString(GetWorld(), uniqueAbyssRow.UniqueAbyssName);

				FString header = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_Notice);
				FString context = TESUtilString::MakeFormatString(*TESUtilTable::GetGlobalString(GetWorld(), EGStringID::UniqueAbyss_EndNotice), uniqueAbyssName);
				FTESMessageBoxInitData messageBoxData = {};
				messageBoxData._messageBoxType = ETESMessageBoxType::Warning;
				messageBoxData._messageBoxSubType = ETESMessageBoxSubType::OneButton;
				messageBoxData._messageHeaderString = header;
				messageBoxData._messageContextString = context;
				messageBoxData._messageButton2String = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_Confirm);
				messageBoxData._messageButton2Callback = FOnTESAction::CreateLambda([=]()
				{
					FTESSceneTransitionData sceneTransitionData = {};
					sceneTransitionData._physicalSceneType = SharedSceneManager->GetCurrentPhysicalGameSceneType();
					sceneTransitionData._logicalSceneType = ETESLogicalGameSceneType::UniqueAbyss;
					sceneTransitionData._fadeAtrribute._fadeType = ETESSceneFadeType::WidgetAnimation;
					sceneTransitionData._fadeAtrribute._fadeColor = FColor::Black;
					SharedSceneManager->ChangeGameScene(sceneTransitionData);
				});

				SharedPopupManager->ShowMessageBox(messageBoxData);

				return;
			}

			//. 소탕에 대한 정보를 구성합니다.
			FTESSweepAttribute sweepAttribute = {};
			sweepAttribute._stageId = _curStageId;
			sweepAttribute._uniqueAbyssId = uniqueAbyssId;
			sweepAttribute._partyIndex = _currentPartyUnique;
			sweepAttribute._partyCharacterUniques = _currentCharacterUniqueList;

			//. 정상적으로 소탕 위젯을 열 수 있는 상황인 경우, 열어줍니다.
			UTESUWSweep* sweepWidget = SharedPopupManager->CreatePopupWidget<UTESUWSweep>(TEXT("WBP_Sweep"), TEXT("Sweep/WBP"));
			TESCHECK(nullptr != sweepWidget);
			sweepWidget->InitWidget();
			sweepWidget->OpenWidget();
			sweepWidget->SetWidget(sweepAttribute);
		}
		break;

	default:
		{
			TESLOG(Error, TEXT("Not Supported Stage Contents Type : %s"), *TESUtilString::GetEnumAsString(stageContentsType));
		}
		break;
	}
}

void UTESUWPartySettingMain::OnEvent_SweepResultRecieved(FTESSweepResultData sweepResultData)
{
	RefreshSweepButton();

	RefreshSweepButtonBySweepTicketCount();

	RefreshStartBattleButton();
}

void UTESUWPartySettingMain::OnEvent_OpenedUniqueAbyssSuccessfully()
{
	SharedNetworkManager->CLGW_QRY_START_STAGE(_curStageId, _currentPartyUnique);
}

void UTESUWPartySettingMain::OnEvent_RefreshPartyName(uint16 partyUnique, FString partyName)
{
	if (_currentPartyUnique == partyUnique)
	{
		//. 이름만 변경된다.
		_currentPartyName = partyName;

		TESCHECK(nullptr != _txt_partyName);
		_txt_partyName->SetText(FText::FromString(*_currentPartyName));
	}
}

void UTESUWPartySettingMain::OnEvent_CallbackChangedPartyMembers()
{
	//. 프리셋이 열려있으면 바로 리턴.
	UTESUWPartyPresetMain* partyPresetMain = SharedPopupManager->FindPopupWidget<UTESUWPartyPresetMain>();
	if (nullptr != partyPresetMain)
    {
		RefreshPartyCombatPower();

		RefreshStartBattleButton();

		ReFreshStellaStrikeInfo();

		return;
	}

	//. 파티해산 버튼을 눌렀을때는 아직 나가는게 아니므로 리턴.
	if (true == _isDispersingParty)
    {
		_isDispersingParty = false;

		//. 닫을때 현재 파티에 있는 상태로 업데이트를 해줘야 한다.
		SharedPartySettingManager->SetPartyPresetCharacterModelingByPartyUnique(_currentPartyUnique);

		CharacterSwapCanceled();

		RefreshPartyCombatPower();

		RefreshStartBattleButton();

		ReFreshStellaStrikeInfo();

		return;
	}

	//. 자동편성 버튼을 눌렀을때도 리턴 시켜준다.
	if (true == _isAutoSettingParty)
    {
		_isAutoSettingParty = false;

		CharacterSwapCanceled();

		RefreshPartyCombatPower();

		RefreshStartBattleButton();

		ReFreshStellaStrikeInfo();
		
		SharedSoundManager->Play2DSound(GetWorld(), EConstantIntName::UISE_PartySet);

		return;
	}

	if (true == _isBackToPrevious)
	{
		SharedSceneManager->OnEvent_ContentsBack();

		_isBackToPrevious = false;
	}

	//. 로비로 한번에 돌아간다.
	if (true == _isBackToHome)
	{
        SharedSceneManager->OnEvent_ContentsHome();

		_isBackToHome = false;
	}
}

void UTESUWPartySettingMain::OnEvent_BackWidget()
{
	OnEvent_BackToPrevious();
}

void UTESUWPartySettingMain::OnEvent_EnterBattle()
{
	//. 씬 이동 전에 튜토리얼중이면 파티편성 튜토리얼을 완료 시킨다.
	ETESNewbieForcedTutorialManagerType currentTutorialType = SharedNewbieForcedTutorialManager->CheckCurrentPlayingTutorialType();

	if (ETESNewbieForcedTutorialManagerType::None != currentTutorialType)
	{
		switch (currentTutorialType)
		{
		case ETESNewbieForcedTutorialManagerType::PartySettingTutorial:
		case ETESNewbieForcedTutorialManagerType::SecondBattleTutorial:
		case ETESNewbieForcedTutorialManagerType::SecondBattleTutorialEnd:
			{
				currentTutorialType = ETESNewbieForcedTutorialManagerType::PartySettingTutorial;
			}
			break;
		case ETESNewbieForcedTutorialManagerType::LastBattleTutorial:
			{
				currentTutorialType = ETESNewbieForcedTutorialManagerType::LastBattleTutorial;
			}
			break;
		default:
			break;
		}

		SharedNewbieForcedTutorialManager->SetNextTutorialWidgetByTutorialType(currentTutorialType);
	}

	//. 회화가 있으면 회화를 먼저 실행한다.
	const FStageRow& stageRow = SharedTDBManager->GetStageRow(_curStageId);
	if (false == stageRow.IsEmpty())
	{
		//. 전투 시작전에 회화나 컷씬이 있으면 실행한다.
		if (0 != stageRow.CovnersationBeforeStageBegin)
		{
			const FTriggerEventRow& triggerEvent = SharedTDBManager->GetTriggerEventRow(stageRow.CovnersationBeforeStageBegin);
			if (true == triggerEvent.IsEmpty())
			{
				TESLOG(Error, TEXT("trigger event table is invalid"));
				return;
			}

			if (ETypeTriggerEvent::Conversation == triggerEvent.T_TriggerEvent)
			{
				//. 이미 한번 읽었는지 확인.
				if ((true == SharedTriggerEventManager->GetReplayStory()) || (false == SharedSaveGameManager->HasGameEventCompleteDataByConversation(triggerEvent.ConversationId)))
				{
					bool canBeStarted = CheckIfStartStage();

					if (true == canBeStarted)
					{
						//. 대화에 대한 정보를 세팅합니다.
						StartTriggerEvent(stageRow);
					}
				}
				//. 이미 읽거나 반복허용이 안되는 회화라면 바로 배틀로 넘어간다.
				else 
				{
					bool canBeStarted = CheckIfStartStage();

					if (true == canBeStarted)
					{
						StartBattle();
					}
				}
			}
			else if (ETypeTriggerEvent::ConversationCutscene == triggerEvent.T_TriggerEvent)
			{
				//. 이미 한번 읽었는지 확인.
				if ((true == SharedTriggerEventManager->GetReplayStory()) || (false == SharedSaveGameManager->HasGameEventCompleteDataByConversation(triggerEvent.Cutscene3DId)))
				{
					bool canBeStarted = CheckIfStartStage();

					if (true == canBeStarted)
					{
						//. 대화에 대한 정보를 세팅합니다.
						StartTriggerEvent(stageRow);
					}
				}
				//. 이미 읽거나 반복허용이 안되는 회화라면 바로 배틀로 넘어간다.
				else
				{
					bool canBeStarted = CheckIfStartStage();

					if (true == canBeStarted)
					{
						StartBattle();
					}
				}
			}
			else
			{
				bool canBeStarted = CheckIfStartStage();

				if (true == canBeStarted)
				{
					StartBattle();
				}
			}
		}
		else
		{
			bool canBeStarted = CheckIfStartStage();

			if (true == canBeStarted)
			{
				StartBattle();
			}
		}
	}
	else
	{
		TESLOG(Error, TEXT("Stage Table is Invalid"));
	}
}

void UTESUWPartySettingMain::OnEvent_OpenUniqueAbyssStage()
{
	//. 오픈에 성공했으므로 전투시작
	OnEvent_EnterBattle();
}

void UTESUWPartySettingMain::OnEvent_ItemDataUpdated()
{
	RefreshSweepButton();

	RefreshSweepButtonBySweepTicketCount();

	RefreshRequiredMaterialList();
}

void UTESUWPartySettingMain::OnEvent_CommonRewardComfirmed()
{
	RefreshSweepButton();

	RefreshSweepButtonBySweepTicketCount();

	RefreshRequiredMaterialList();

	RefreshStartBattleButton();

	RefreshCharacterSlotInfo();
}
