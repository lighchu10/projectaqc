// Fill out your copyright notice in the Description page of Project Settings.
#include "Contents/NFT/UI/TESUWNFTSendToWeb.h"
#include "Contents/NFT/UI/TESUWNFTMenuMain.h"
#include "Contents/NFT/UI/TESUWObjectClearPopup.h"
#include "Contents/NFT/UI/TESEDNFTCharacterInventorySlot.h"
#include "Contents/NFT/UI/TESUWNFTCharacterInventorySlot.h"
#include "Contents/NFT/UI/TESUWNFTNetworkSelectionPopup.h"
#include "Contents/NFT/UI/TESUWNFTInProgressPopup.h"
#include "Contents/Lobby/Manager/TESLobbyInputManager.h"
#include "Contents/ItemInventory/Manager/TESItemInventoryManager.h"
#include "Contents/ItemInventory/UI/TESUWItemInventoryMain.h"
#include "Contents/ItemInventory/UI/TESEDItemInventorySlot.h"
#include "Contents/ItemInventory/UI/TESUWItemInventorySlot.h"
#include "Contents/ItemInventory/UI/TESUWItemInventoryDetailBase.h"
#include "Contents/ItemInventory/UI/TESUWItemInventoryDetailEmpty.h"
#include "Contents/ItemInventory/UI/TESUWItemInventoryCapacityExpand.h"
#include "Contents/ItemInventory/UI/TESUWItemInventoryDetailBulkSale.h"
#include "Contents/PartySetting/Manager/TESPartySettingManager.h"
#include "Contents/Facility/Manager/TESFacilityManager.h"
#include "Contents/CharacterInformation/UI/TESUWCharInfoMain.h"
#include "Contents/CharacterInformation/UI/TESEDCharInfoContentsItemSlot.h"
#include "Contents/CharacterInformation/UI/TESUWCharInfoContentsItemSlot.h"
#include "Contents/CharacterInformation/Manager/TESCharacterInformationManager.h"
#include "Contents/CharacterInformation/UI/TESUWCharInfoMain.h"
#include "Contents/Common/Manager/TESSaveGameManager.h"
#include "Contents/Common/Manager/TESUserDataManager.h"
#include "Contents/Common/Manager/TESPopupManager.h"
#include "Contents/Common/Manager/TESSceneManager.h"
#include "Contents/Common/UI/CustomSlates/TESTextBlock.h"
#include "Contents/Common/UI/CustomSlates/TESTileView.h"
#include "Contents/Common/UI/TESUWCommonHeader.h"
#include "Contents/Common/UI/Filter/TESUWFilter.h"
#include "Contents/Common/UI/TESUWCommonToggleSwitcher.h"
#include "Components/WidgetSwitcher.h"
#include "NetWork/TESNetworkManager.h"
#include "Tutorial/Manager/TESTutorialManager.h"
#include "TESGameInstance.h"
#include "TESUtils.h"
#include "TESUtilPath.h"
#include "TESUtilFilter.h"
#include "TESUtilTexture.h"
#include "TESUtilString.h"
#include "TESUtilStatus.h"

void UTESUWNFTSendToWeb::InitWidget()
{
	Super::InitWidget();

	InitCommonHeader();

	InitMainCategories();

	InitFilter();

	InitCharacterPanel();

	InitCharacterProfile();

	InitInventoryViewSwitcher();

	InitInventoryView();

	InitItemInfoPanel();

	InitItemInfo();

	InitCharacterUnlockButton();

	InitCharacterClearButton();

	InitItemUnlockButton();

	InitItemClearButton();

	InitSendToWebButton();
}

void UTESUWNFTSendToWeb::ClearWidget()
{
	Super::ClearWidget();

	ClearCommonHeader();

	ClearCharacterInventoryViewContainer();

	ClearItemInventoryViewContainer();
}

void UTESUWNFTSendToWeb::OpenWidget()
{
	Super::OpenWidget();

	if (true == ::IsValid(_wbp_commonHeader))
	{
		_wbp_commonHeader->OpenWidget();
	}

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UTESUWNFTSendToWeb::CloseWidget()
{
	Super::CloseWidget();

	if (true == ::IsValid(_wbp_commonHeader))
	{
		_wbp_commonHeader->CloseWidget();
	}

	SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWNFTSendToWeb::OpenDelegate()
{
	Super::OpenDelegate();

	FTESUtilEvents::OnCommonFilterApply.BindUObject(this, &UTESUWNFTSendToWeb::OnEvent_FilterApply);

	FTESUtilEvents::OnSendToWebCharacterSlotClicked.BindUObject(this, &UTESUWNFTSendToWeb::OnEvent_CharacterInventorySlotClicked);

	FTESUtilEvents::OnCharInfoContentsEquipItemSlotClicked.BindUObject(this, &UTESUWNFTSendToWeb::OnEvent_ItemInventorySlotClicked);

	FTESUtilEvents::OnCharInfoLockCharacter.BindUObject(this, &UTESUWNFTSendToWeb::OnEvent_CharacterUnlockComplete);

	FTESUtilEvents::OnNFTRefreshButtons.BindUObject(this, &UTESUWNFTSendToWeb::RefreshCharacterUIButtons);

	_dh_endMintingObject = FTESUtilEvents::OnMintingObject.AddUObject(this, &UTESUWNFTSendToWeb::OnEvent_EndMinting);

	_dh_itemUnEquipComplete = FTESUtilEvents::OnItemUnEquipComplete.AddUObject(this, &UTESUWNFTSendToWeb::OnEvent_ItemClearComplete);

	_dh_itemLockComplete = FTESUtilEvents::OnItemLockedComplete.AddUObject(this, &UTESUWNFTSendToWeb::OnEvent_ItemUnlockComplete);
}

void UTESUWNFTSendToWeb::CloseDelegate()
{
	Super::CloseDelegate();

	FTESUtilEvents::OnCommonFilterApply.Unbind();

	FTESUtilEvents::OnSendToWebCharacterSlotClicked.Unbind();

	FTESUtilEvents::OnCharInfoContentsEquipItemSlotClicked.Unbind();

	FTESUtilEvents::OnCharInfoLockCharacter.Unbind();

	FTESUtilEvents::OnNFTRefreshButtons.Unbind();

	FTESUtilEvents::OnMintingObject.Remove(_dh_endMintingObject);

	FTESUtilEvents::OnItemUnEquipComplete.Remove(_dh_itemUnEquipComplete);

	FTESUtilEvents::OnItemLockedComplete.Remove(_dh_itemLockComplete);
}

void UTESUWNFTSendToWeb::InitCommonHeader()
{
	_wbp_commonHeader = Cast<UTESUWCommonHeader>(GetWidgetFromName(TEXT("WBP_CommonHeader")));
	TESCHECK(nullptr != _wbp_commonHeader);
	_wbp_commonHeader->InitWidget();
	_wbp_commonHeader->CloseWidget();
}

void UTESUWNFTSendToWeb::InitMainCategories()
{
	int32 maxCount = static_cast<int32>(ETESInventoryMainCategory::Max);
	for (int32 index = 1; index < maxCount; ++index)
	{
		FString widgetName = FString::Printf(TEXT("CTS_%d"), index);
		UTESUWCommonToggleSwitcher* cts_mainCategory = Cast<UTESUWCommonToggleSwitcher>(GetWidgetFromName(*widgetName));
		TESCHECK_CONTINUE(nullptr != cts_mainCategory);

		ETESInventoryMainCategory category = static_cast<ETESInventoryMainCategory>(index);
		_cts_mainCategoryMap.Add(category, cts_mainCategory);
	}
}

void UTESUWNFTSendToWeb::InitCharacterPanel()
{
	_cp_characterPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Root_RightPanel_Character")));
	TESCHECK(nullptr != _cp_characterPanel);
	_cp_characterPanel->SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWNFTSendToWeb::InitCharacterProfile()
{
	InitCharacterPortrait();

	InitCharacterName();

	InitCharacterRoleIcon();

	InitCharacterRoleText();

	InitCharacterAttackTypeIcon();

	InitCharacterAttackTypeText();

	InitCharacterRangeIcon();

	InitCharacterRangeText();

	InitCharacterSpecialityIcon();

	InitCharacterSpecialityText();

	InitCharacterLevelValue();

	InitCharacterAwakeningValue();

	InitCharacterAwakeningRemainValue();

	InitCharacterGrade();

	InitCharacterPrestigeEmblem();
}

void UTESUWNFTSendToWeb::InitCharacterPortrait()
{
	_img_characterPortrait = Cast<UTESImage>(GetWidgetFromName(TEXT("IMG_ProfileCharPortrait_BG")));
	TESCHECK(nullptr != _img_characterPortrait);
	_img_characterPortrait->SetBrushFromTexture(nullptr);
}

void UTESUWNFTSendToWeb::InitCharacterName()
{
	_txt_characterName = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_CharacterName")));
	TESCHECK(nullptr != _txt_characterName);
	_txt_characterName->SetText(FText::FromString(TEXT("")));
}

void UTESUWNFTSendToWeb::InitFilter()
{
	InitFilterSwitcher();

	InitFilterButtonText();

	InitSortingOrderButton();
}

void UTESUWNFTSendToWeb::InitFilterSwitcher()
{
	_cts_filterSwitcher = Cast<UTESUWCommonToggleSwitcher>(GetWidgetFromName(TEXT("CTS_FilterSwitcher")));
	TESCHECK(nullptr != _cts_filterSwitcher);
	_cts_filterSwitcher->SetToggleState(false);
}

void UTESUWNFTSendToWeb::InitFilterButtonText()
{
	//. [ON] 상태일때의 필터 버튼 텍스트
	UTESTextBlock* txt_filter_on = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_Filter_ON")));
	TESCHECK(nullptr != txt_filter_on);
	_txt_characterFilterTextMap.Add(true, txt_filter_on);

	//. [OFF] 상태일때의 필터 버튼 텍스트
	UTESTextBlock* txt_filter_off = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_Filter_OFF")));
	TESCHECK(nullptr != txt_filter_off);
	_txt_characterFilterTextMap.Add(false, txt_filter_off);
}

void UTESUWNFTSendToWeb::InitSortingOrderButton()
{
	//. [ON] 상태일때의 정렬 버튼
	UCheckBox* chkb_on = Cast<UCheckBox>(GetWidgetFromName(TEXT("CHKB_Sorter_ON")));
	TESCHECK(nullptr != chkb_on);
	chkb_on->SetVisibility(ESlateVisibility::HitTestInvisible);
	_chkb_characterFilterSortingOrderMap.Add(true, chkb_on);

	//. [OFF] 상태일때의 정렬 버튼
	UCheckBox* chkb_off = Cast<UCheckBox>(GetWidgetFromName(TEXT("CHKB_Sorter_OFF")));
	TESCHECK(nullptr != chkb_off);
	chkb_off->SetVisibility(ESlateVisibility::HitTestInvisible);
	_chkb_characterFilterSortingOrderMap.Add(false, chkb_off);
}

void UTESUWNFTSendToWeb::InitCharacterRoleIcon()
{
	_img_characterRole = Cast<UTESImage>(GetWidgetFromName(TEXT("IMG_CharacterRole")));
	TESCHECK(nullptr != _img_characterRole);
	_img_characterRole->SetBrushFromTexture(nullptr);
}

void UTESUWNFTSendToWeb::InitCharacterRoleText()
{
	_txt_characterRole = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_CharacterRole")));
	TESCHECK(nullptr != _txt_characterRole);
	_txt_characterRole->SetText(FText::FromString(TEXT("")));
}

void UTESUWNFTSendToWeb::InitCharacterAttackTypeIcon()
{
	_img_characterAttackType = Cast<UTESImage>(GetWidgetFromName(TEXT("IMG_CharacterAttackType")));
	TESCHECK(nullptr != _img_characterAttackType);
	_img_characterAttackType->SetBrushFromTexture(nullptr);
}

void UTESUWNFTSendToWeb::InitCharacterAttackTypeText()
{
	_txt_characterAttackType = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_CharacterAttackType")));
	TESCHECK(nullptr != _txt_characterAttackType);
	_txt_characterAttackType->SetText(FText::FromString(TEXT("")));
}

void UTESUWNFTSendToWeb::InitCharacterRangeIcon()
{
	_img_characterRange = Cast<UTESImage>(GetWidgetFromName(TEXT("IMG_CharacterRange")));
	TESCHECK(nullptr != _img_characterRange);
	_img_characterRange->SetBrushFromTexture(nullptr);
}

void UTESUWNFTSendToWeb::InitCharacterRangeText()
{
	_txt_characterRange = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_CharacterRange")));
	TESCHECK(nullptr != _txt_characterRange);
	_txt_characterRange->SetText(FText::FromString(TEXT("")));
}

void UTESUWNFTSendToWeb::InitCharacterSpecialityIcon()
{
	_img_characterSpeciality = Cast<UTESImage>(GetWidgetFromName(TEXT("IMG_CharacterSpecialty")));
	TESCHECK(nullptr != _img_characterSpeciality);
	_img_characterSpeciality->SetBrushFromTexture(nullptr);
}

void UTESUWNFTSendToWeb::InitCharacterSpecialityText()
{
	_txt_characterSpecialty = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_CharacterSpecialty")));
	TESCHECK(nullptr != _txt_characterSpecialty);
	_txt_characterSpecialty->SetText(FText::FromString(TEXT("")));
}

void UTESUWNFTSendToWeb::InitCharacterLevelValue()
{
	//. 캐릭터 현재 레벨
	_txt_characterCurrentLevel = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_CurrentLevel")));
	TESCHECK(nullptr != _txt_characterCurrentLevel);
	_txt_characterCurrentLevel->SetText(FText::FromString(TEXT("")));

	//. 캐릭터 맥스 레벨
	_txt_characterMaximumLevel = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_MaxLevel")));
	TESCHECK(nullptr != _txt_characterMaximumLevel);
	_txt_characterMaximumLevel->SetText(FText::FromString(TEXT("")));
}

void UTESUWNFTSendToWeb::InitCharacterAwakeningValue()
{
	_txt_characterAwakeningValue = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_CharacterAwakeningValue")));
	TESCHECK(nullptr != _txt_characterAwakeningValue);
	_txt_characterAwakeningValue->SetText(FText::FromString(TEXT("")));
}

void UTESUWNFTSendToWeb::InitCharacterAwakeningRemainValue()
{
	_txt_characterAwakeningRemainValue = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_CharacterAwakenRemainValue")));
	TESCHECK(nullptr != _txt_characterAwakeningRemainValue);
	_txt_characterAwakeningRemainValue->SetText(FText::FromString(TEXT("")));
}

void UTESUWNFTSendToWeb::InitItemInfoPanel()
{
	_cp_itemPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Root_RightPanel_EquipmentAndRune")));
	TESCHECK(nullptr != _cp_itemPanel);
	_cp_itemPanel->SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWNFTSendToWeb::InitItemInfo()
{
	InitItemName();

	InitItemIcon();

	InitItemRequiredLevel();

	InitItemRequiredStat();

	InitItemOptionPanels();

	InitItemOptionNames();

	InitItemOptionsValue();

	InitCharacterGrade();

	InitCharacterPrestigeEmblem();
}

void UTESUWNFTSendToWeb::InitItemName()
{
	_txt_itemName = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_ItemName")));
	TESCHECK(nullptr != _txt_itemName);
	_txt_itemName->SetText(FText::FromString(TEXT("")));
}

void UTESUWNFTSendToWeb::InitItemIcon()
{
	_img_itemIcon = Cast<UTESImage>(GetWidgetFromName(TEXT("IMG_ItemIcon")));
	TESCHECK(nullptr != _img_itemIcon);
	_img_itemIcon->SetBrushFromTexture(nullptr);
}

void UTESUWNFTSendToWeb::InitItemRequiredLevel()
{
	_txt_itemRequiredLevel = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_RequiredLevel")));
	TESCHECK(nullptr != _txt_itemRequiredLevel);
	_txt_itemRequiredLevel->SetText(FText::FromString(TEXT("0")));
}

void UTESUWNFTSendToWeb::InitItemRequiredStat()
{
	_txt_requiredStatMap.Empty();

	//	STR = 27, // 캐릭터 기본 힘
	//	DEX = 28, // 캐릭터 기본 민첩
	//	CON = 29, // 캐릭터 기본 체력
	//	INT = 30, // 캐릭터 기본 지력
	//	WIS = 31, // 캐릭터 기본 지혜

	//. 스탯의 종류는 10개.
	for (int32 index = 1; index <= 5; ++index)
	{
		FString statText = FString::Printf(TEXT("TXT_Stat_%d"), index);
		UTESTextBlock* txt_requiredStat = Cast<UTESTextBlock>(GetWidgetFromName(*statText));
		TESCHECK_CONTINUE(nullptr != txt_requiredStat);
		txt_requiredStat->SetText(FText::FromString(TEXT("0")));
		int32 numofEnum = index + 26;
		ETypeStatBase statBase = static_cast<ETypeStatBase>(numofEnum);
		
		_txt_requiredStatMap.Add(statBase, txt_requiredStat);
	}
}

void UTESUWNFTSendToWeb::InitItemOptionPanels()
{
	_cp_mainOptionPanelList.Empty();

	//. 스탯의 종류는 5개.
	for (int32 index = 1; index <= 5; ++index)
	{
		FString panelText = FString::Printf(TEXT("StatOption_%d"), index);
		UCanvasPanel* cp_option = Cast<UCanvasPanel>(GetWidgetFromName(*panelText));
		TESCHECK_CONTINUE(nullptr != cp_option);
		cp_option->SetVisibility(ESlateVisibility::Collapsed);

		_cp_mainOptionPanelList.Add(cp_option);
	}
}

void UTESUWNFTSendToWeb::InitItemOptionNames()
{
	_txt_mainOptionNameList.Empty();

	//. 메인 + 서브 옵션까지 총 7개까지 있지만 여기서 표기는 최대 5개까지 밖에 안된다.
	for (int32 index = 1; index <= 5; ++index)
	{
		FString statOptionName = FString::Printf(TEXT("TXT_StatOptionName_%d"), index);

		UTESTextBlock* txt_mainOptionName = Cast<UTESTextBlock>(GetWidgetFromName(*statOptionName));
		TESCHECK_CONTINUE(nullptr != txt_mainOptionName);
		txt_mainOptionName->SetText(FText::FromString(TEXT("")));

		_txt_mainOptionNameList.Add(txt_mainOptionName);
	}
}

void UTESUWNFTSendToWeb::InitItemOptionsValue()
{
	_txt_mainOptionValueList.Empty();

	//. 메인 + 서브 옵션까지 총 7개까지 있지만 여기서 표기는 최대 5개까지 밖에 안된다.
	for (int32 index = 1; index <= 5; ++index)
	{
		FString statOptionName = FString::Printf(TEXT("TXT_StatOptionValue_%d"), index);

		UTESTextBlock* txt_mainOptionValue = Cast<UTESTextBlock>(GetWidgetFromName(*statOptionName));
		TESCHECK_CONTINUE(nullptr != txt_mainOptionValue);
		txt_mainOptionValue->SetText(FText::FromString(TEXT("0")));

		_txt_mainOptionValueList.Add(txt_mainOptionValue);
	}
}

void UTESUWNFTSendToWeb::InitInventoryViewSwitcher()
{
	_ws_inventoryViewSwitcher = Cast<UWidgetSwitcher>(GetWidgetFromName(TEXT("WidgetSwitcher_List")));
	TESCHECK(nullptr != _ws_inventoryViewSwitcher);
	_ws_inventoryViewSwitcher->SetActiveWidgetIndex(0);
}

void UTESUWNFTSendToWeb::InitInventoryView()
{
	InitCharacterInventoryViewContainer();

	InitItemInventoryViewContainer();
}

void UTESUWNFTSendToWeb::InitCharacterInventoryViewContainer()
{
	_tv_characterInventoryViewContainer = Cast<UTESTileView>(GetWidgetFromName(TEXT("TV_Character")));
	TESCHECK(nullptr != _tv_characterInventoryViewContainer);
	_tv_characterInventoryViewContainer->ClearListItems();

	//_tv_characterInventoryViewContainer->OnItemClicked().AddUObject(this, &UTESUWNFTSendToWeb::OnEvent_SlotClicked);
}

void UTESUWNFTSendToWeb::InitItemInventoryViewContainer()
{
	_tv_itemInventoryViewContainer = Cast<UTESTileView>(GetWidgetFromName(TEXT("TV_Equipment")));
	TESCHECK(nullptr != _tv_itemInventoryViewContainer);
	_tv_itemInventoryViewContainer->ClearListItems();
}

void UTESUWNFTSendToWeb::InitCharacterUnlockButton()
{
	//. 추후 캐릭터 잠금 버튼 기능 생기면 작업할 예정
	_cts_unlockCharacterButton = Cast<UTESUWCommonToggleSwitcher>(GetWidgetFromName(TEXT("CTS_UnlockCharacter")));
	TESCHECK(nullptr != _cts_unlockCharacterButton);
	_cts_unlockCharacterButton->SetToggleState(false);
}

void UTESUWNFTSendToWeb::InitCharacterClearButton()
{
	_cts_clearCharacterButton = Cast<UTESUWCommonToggleSwitcher>(GetWidgetFromName(TEXT("CTS_ClearCharacter")));
	TESCHECK(nullptr != _cts_clearCharacterButton);
	_cts_clearCharacterButton->SetToggleState(false);
}

void UTESUWNFTSendToWeb::InitItemUnlockButton()
{
	_cts_unlockItemButton = Cast<UTESUWCommonToggleSwitcher>(GetWidgetFromName(TEXT("CTS_UnlockItem")));
	TESCHECK(nullptr != _cts_unlockItemButton);
	_cts_unlockItemButton->SetToggleState(false);
}

void UTESUWNFTSendToWeb::InitItemClearButton()
{
	_cts_clearItemButton = Cast<UTESUWCommonToggleSwitcher>(GetWidgetFromName(TEXT("CTS_ClearItem")));
	TESCHECK(nullptr != _cts_clearItemButton);
	_cts_clearItemButton->SetToggleState(false);
}

void UTESUWNFTSendToWeb::InitSendToWebButton()
{
	_cts_sendToWebButtonByCharacter = Cast<UTESUWCommonToggleSwitcher>(GetWidgetFromName(TEXT("CTS_SendToWebByCharacter")));
	TESCHECK(nullptr != _cts_sendToWebButtonByCharacter);
	_cts_sendToWebButtonByCharacter->SetToggleState(false);

	_cts_sendToWebButtonByItem = Cast<UTESUWCommonToggleSwitcher>(GetWidgetFromName(TEXT("CTS_SendToWebByItem")));
	TESCHECK(nullptr != _cts_sendToWebButtonByItem);
	_cts_sendToWebButtonByItem->SetToggleState(false);
}

void UTESUWNFTSendToWeb::InitCharacterGrade()
{
	_img_characterGrade = Cast<UTESImage>(GetWidgetFromName(TEXT("IMG_CharacterGrade")));
	TESCHECK(nullptr != _img_characterGrade);
	_img_characterGrade->SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWNFTSendToWeb::InitCharacterPrestigeEmblem()
{
	_img_characterPrestigeEmblem = Cast<UTESImage>(GetWidgetFromName(TEXT("IMG_PrestigeEmblem")));
	TESCHECK(nullptr != _img_characterPrestigeEmblem);
	_img_characterPrestigeEmblem->SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWNFTSendToWeb::ClearCommonHeader()
{
	TESCHECK(nullptr != _wbp_commonHeader);
	_wbp_commonHeader->CloseWidget();
	_wbp_commonHeader->ClearWidget();
}

void UTESUWNFTSendToWeb::ClearCharacterInventoryViewContainer()
{
	if (true == ::IsValid(_tv_characterInventoryViewContainer))
	{
		_tv_characterInventoryViewContainer->ClearListItems();
	}
}

void UTESUWNFTSendToWeb::ClearItemInventoryViewContainer()
{
	if (true == ::IsValid(_tv_itemInventoryViewContainer))
	{
		_tv_itemInventoryViewContainer->ClearListItems();
	}
}

void UTESUWNFTSendToWeb::ClearSelectItem()
{
	_selectedItemUnique = 0;
}

void UTESUWNFTSendToWeb::SetWidget()
{
	if (false == SetData())
	{
		CloseWidget();

		return;
	}

	SetCommonHeader();

	SetMainCategory(ETESInventoryMainCategory::Character);

	SetCharacterUnlockButton();

	SetCharacterClearButton();

	SetItemUnlockButton();

	SetItemClearButton();
}

bool UTESUWNFTSendToWeb::SetData()
{
	//. 아이템 카테고리별 목록을 추출합니다.
	TArray<UTESGDGoodsItem*> haveItemList = SharedUserDataManager->FindItemDataAll();
	SetCategoryItemDatas(haveItemList);

	return true;
}

void UTESUWNFTSendToWeb::SetCommonHeader()
{
	TESCHECK(nullptr != _wbp_commonHeader);

	_wbp_commonHeader->SetWidget();
}

void UTESUWNFTSendToWeb::SetCategoryItemData(UTESGDGoodsItem* itemData)
{
	if (false == ::IsValid(itemData))
	{
		TESLOG(Error, TEXT("Invalid ItemData."));

		return;
	}

	for (TPair<ETESItemInventoryMainCategory, FTESInventoryCategoryItemData> child : _categoryItemDataMap)
	{
		int64 itemUnique = itemData->GetUniqueID();
		if (true == child.Value._itemDataMap.Contains(itemUnique))
		{
			child.Value._itemDataMap[itemUnique] = itemData;
		}
	}
}

void UTESUWNFTSendToWeb::SetCategoryItemDatas(TArray<class UTESGDGoodsItem*> itemDataList)
{
	//. 모두 비웁니다.
	_categoryItemDataMap.Empty();

	//. 그리고 다시 채웁니다.
	int32 maxCount = itemDataList.Num();
	for (int32 index = 0; index < maxCount; ++index)
	{
		UTESGDGoodsItem* gameDataItem = itemDataList[index];
		if (false == ::IsValid(gameDataItem))
		{
			TESLOG(Error, TEXT("Invalid ItemData. : %d"), index);

			continue;
		}

		const FItemRow& itemTable = gameDataItem->GetItemTable();
		if (false == itemTable.IsShow) continue;

		ETypeItem itemType = itemTable.ItemType;
		switch (itemType)
		{
		case ETypeItem::Equip:
			{
				switch (itemTable.EquipType)
				{
				case ETypeEquip::Equipment: AddCategoryItemData(ETESItemInventoryMainCategory::Equip, gameDataItem);	break;
				case ETypeEquip::Rune:		AddCategoryItemData(ETESItemInventoryMainCategory::Rune, gameDataItem);		break;
				default:
					{
						TESLOG(Error, TEXT("Not Supported Type. : %d"), itemTable.EquipType);
					}
					break;
				}
			}
			break;

		case ETypeItem::Use:
		case ETypeItem::Material:
			{
				AddCategoryItemData(ETESItemInventoryMainCategory::Misc, gameDataItem);
			}
			break;

		default:
			{
				TESLOG(Error, TEXT("Not Supported Type : %d"), itemType);
			}
			break;
		}
	}
}

void UTESUWNFTSendToWeb::AddCategoryItemData(ETESItemInventoryMainCategory category, UTESGDGoodsItem* itemData)
{
	if (false == ::IsValid(itemData))
	{
		TESLOG(Error, TEXT("Invalid ItemData."));

		return;
	}

	if (true == _categoryItemDataMap.Contains(category))
	{
		_categoryItemDataMap[category]._itemDataMap.Add(itemData->GetItemUnique(), itemData);
	}
	else
	{
		FTESInventoryCategoryItemData categoryData = {};

		categoryData._category = category;

		categoryData._itemDataMap.Add(itemData->GetItemUnique(), itemData);

		_categoryItemDataMap.Add(category, categoryData);
	}
}

void UTESUWNFTSendToWeb::AddCategoryItemDatas(ETESItemInventoryMainCategory categoryType, TArray<class UTESGDGoodsItem*> itemDatas)
{
	for (UTESGDGoodsItem* itemData : itemDatas)
	{
		AddCategoryItemData(categoryType, itemData);
	}
}

void UTESUWNFTSendToWeb::RemoveCategoryItemData(ETESItemInventoryMainCategory categoryType, UTESGDGoodsItem* itemData)
{
	if (false == ::IsValid(itemData))
	{
		TESLOG(Error, TEXT("Invalid ItemData."));

		return;
	}

	if (true == _categoryItemDataMap.Contains(categoryType))
	{
		_categoryItemDataMap[categoryType]._itemDataMap.Remove(itemData->GetItemUnique());
	}
	else
	{
		TESLOG(Error, TEXT("Not Found Category. : %d"), categoryType);
	}
}

void UTESUWNFTSendToWeb::SetMainCategory(ETESInventoryMainCategory categoryType, bool isForced)
{
	//. 같은 카테고리를 교체하려고하면 무시합니다!!
	//. (단, 강제 전환이 아닌 경우에만 체크합니다.)
	if ((false == isForced) && (categoryType == _currentMainCategory))
	{
		return;
	}

	//. 이전 카테고리의 아이디 값을 교체합니다.
	_previousMainCategory = _currentMainCategory;

	//. 다음 카테고리의 아이디 값을 교체합니다.
	_currentMainCategory = categoryType;

	//. 이전 카테고리 버튼의 상태를 "선택되지 않은 상태"로 변경합니다.
	SetMainCategoryButton(_previousMainCategory, false);

	//. 현재 카테고리 버튼의 상태를 "선택된 상태"로 변경합니다.
	SetMainCategoryButton(_currentMainCategory, true);

	//. 필터
	SetFilterContents();

	SetViewContents(_currentMainCategory);
}

void UTESUWNFTSendToWeb::SetMainCategoryButton(ETESInventoryMainCategory category, bool isSelected)
{
	if (ETESInventoryMainCategory::None == category)
	{
		return;
	}

	if (false == _cts_mainCategoryMap.Contains(category))
	{
		TESLOG(Error, TEXT("Invalid Category. %d"), category);

		return;
	}

	_cts_mainCategoryMap[category]->SetToggleState(isSelected);
}

void UTESUWNFTSendToWeb::SetViewContents(ETESInventoryMainCategory categoryType)
{
	TESCHECK(nullptr != _ws_inventoryViewSwitcher);
	switch (categoryType)
	{
	case ETESInventoryMainCategory::Character:
		{
			_ws_inventoryViewSwitcher->SetActiveWidget(0);

			//. 캐릭터
			SetCharacterViewContents();
		}
		break;
	case ETESInventoryMainCategory::Equip:
		{
			//. 장비
			_ws_inventoryViewSwitcher->SetActiveWidgetIndex(1);

			SetItemViewContents(ETESItemInventoryMainCategory::Equip);
		}
		break;
	case ETESInventoryMainCategory::Rune:
		{
			//. 룬
			_ws_inventoryViewSwitcher->SetActiveWidgetIndex(2);

			SetItemViewContents(ETESItemInventoryMainCategory::Rune);
		}
		break;
	default:
		{
			TESLOG(Error, TEXT("There is wrong category : %d"), categoryType);
		}
		break;
	}
}

void UTESUWNFTSendToWeb::SetCharacterViewContents()
{
	//. 보유 캐릭터 아이디는 DB로부터 가져온다.
	TArray<UTESGDCharacter*> ownCharacterInfoList = SharedUserDataManager->FindHaveCharacterAll();

	//. 필터링된 캐릭터 리스트를 얻어옵니다.(필터 옵션창)
	TArray<UTESGDCharacter*> filteredCharacterListByFilterOption = MakeFilteredCharacterDataList(ownCharacterInfoList
																								, _currentFilterData._filteringData
																								, _currentFilterData._isFiltering);

	//. 필터링된 캐릭터 리스트를 정렬하여 얻어옵니다.
	TArray<UTESGDCharacter*> sortedCharacterDataList = MakeSortedCharacterDataList(filteredCharacterListByFilterOption
																				, _currentFilterData._sortingElementId
																				, _currentFilterData._sortingOrder);

	SetCharacterInventoryViewContainer(sortedCharacterDataList);
}

void UTESUWNFTSendToWeb::SetCharacterInventoryViewContainer(TArray<class UTESGDCharacter*> characterDataList)
{
	if (false == ::IsValid(_tv_characterInventoryViewContainer))
	{
		return;
	}

	//. 모든 내용을 지워줍니다.
	_tv_characterInventoryViewContainer->ClearListItems();

	//. 다시 채워준다.
	int32 maxCount = characterDataList.Num();
	for (int32 index = 0; index < maxCount; ++index)
	{
		UTESEDNFTCharacterInventorySlot* characterSlotEntryData = NewObject<UTESEDNFTCharacterInventorySlot>();
		if (false == ::IsValid(characterSlotEntryData))
		{
			TESLOG(Error, TEXT("Invalid characterSlotEntryData."));

			continue;
		}

		UTESGDCharacter* characterData = characterDataList[index];
		TESCHECK_CONTINUE(nullptr != characterData);
		const FCharacterRow& characterTable = characterData->GetCharacterTable();
		if (false == characterTable.IsEmpty())
		{
			if (false == characterTable.IsNFTAble)
			{
				continue;
			}
		}
		
		if (nullptr != characterData)
		{
			bool isSelected = false;

			characterSlotEntryData->SetData(characterData, isSelected, _currentFilterData._sortingElementId);
			_tv_characterInventoryViewContainer->AddItem(characterSlotEntryData);
		}
	}
}

void UTESUWNFTSendToWeb::SetItemViewContents(ETESItemInventoryMainCategory categoryType)
{
	SetItemViewContents(categoryType, GetItemCategoryDatas(categoryType));
}

void UTESUWNFTSendToWeb::SetItemViewContents(ETESItemInventoryMainCategory categoryType, TArray<class UTESGDGoodsItem*> itemDataList)
{
	//. 필터링된 아이템 리스트를 얻어옵니다.
	TArray<UTESGDGoodsItem*> filteredItemDataList = MakeFilteredItemDataList(itemDataList
																			, _currentFilterData._filteringData
																			, _currentFilterData._isFiltering);

	//. 필터링된 데이터가 있을 경우, 정렬 후 출력합니다.
	if (true == filteredItemDataList.Num() > 0)
	{
		//. 필터링된 아이템 리스트를 정렬하여 얻어옵니다.
		TArray<UTESGDGoodsItem*> sortedItemDataList = MakeSortedItemDataList(filteredItemDataList
																			, _currentFilterData._sortingElementId
																			, _currentFilterData._sortingOrder);

		SetItemInventoryViewContainer(sortedItemDataList);
	}
	else
	{
		SetItemInventoryViewContainer({});
	}
}

void UTESUWNFTSendToWeb::SetItemInventoryViewContainer(TArray<UTESGDGoodsItem*> itemDataList)
{
	if (false == ::IsValid(_tv_itemInventoryViewContainer))
	{
		return;
	}

	_currentInventoryItemSlotDatas.Empty();

	_tv_itemInventoryViewContainer->ClearListItems();

	for (UTESGDGoodsItem* itemData : itemDataList)
	{
		UTESEDCharInfoContentsItemSlot* itemSlotData = NewObject<UTESEDCharInfoContentsItemSlot>();
		if (false == ::IsValid(itemSlotData))
		{
			TESLOG(Error, TEXT("Invalid ItemSlotData."));

			continue;
		}

		const FItemRow& itemRow = itemData->GetItemTable();
		if (false == itemRow.IsEmpty())
		{
			if (false == itemRow.IsNFTAble)
			{
				continue;
			}
		}

		itemSlotData->SetItemData(itemData);

		itemSlotData->SetItemSelected(itemData->GetItemUnique() == _selectedItemUnique);

		itemSlotData->SetItemUsingCharVisibled(true);

		_currentInventoryItemSlotDatas.Add(itemData->GetItemUnique(), itemSlotData);

		_tv_itemInventoryViewContainer->AddItem(itemSlotData);
	}
}

void UTESUWNFTSendToWeb::SetCharacterPanelVisible(bool isVisible)
{
	TESCHECK(nullptr != _cp_characterPanel);
	_cp_characterPanel->SetVisibility(true == isVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UTESUWNFTSendToWeb::SetCharacterProfile()
{
	SetCharacterPanelVisible(true);

	//. 해당 캐릭터의 초상화를 세팅합니다.
	SetCharacterPortrait(_currentCharacterData->GetCharacterPortraitId());

	//. 해당 캐릭터의 이름을 세팅합니다.
	SetCharacterName(_currentCharacterData->GetCharacterName(GetWorld()));

	//. 해당 캐릭터의 역할을 세팅합니다.
	SetCharacterRole(_currentCharacterData->GetCharacterRole());

	//. 해당 캐릭터의 공격타입을 세팅합니다.
	SetCharacterAttackType(_currentCharacterData->GetCharacterAttackType());

	//. 해당 캐릭터의 범위를 세팅합니다.
	SetCharacterRange(_currentCharacterData->GetCharacterRange());

	//. 해당 캐릭터의 특화를 세팅합니다.
	SetCharacterSpeciality(_currentCharacterData->GetCharacterSpeciality());

	//. 해당 캐릭터의 레벨을 세팅합니다.
	SetCharacterLevelValue(_currentCharacterData->GetCharacterCurrentLevel(), _currentCharacterData->GetCharacterMaximumLevel());

	//. 해당 캐릭터의 각성을 세팅합니다.
	SetCharacterAwakeningValue(_currentCharacterData->GetCharacterCurrentAwakenLevel());

	//. 해당 캐릭터의 각성 남은 횟수를 표기합니다.
	SetCharacterAwakeningRemainValue(_currentCharacterData->GetCharacterCurrentAwakenTryCount()
								   , _currentCharacterData->GetCharacterMaximumAwakenTryCount());

	SetCharacterGrade(_currentCharacterData->GetCharacterGrade());

	SetCharacterPrestige(_currentCharacterData->IsPrestigeCharacter());
}

void UTESUWNFTSendToWeb::SetFilterContents()
{
	ETypeSortFilterContents filterContents = ETypeSortFilterContents::None;

	switch (_currentMainCategory)
	{
	case ETESInventoryMainCategory::Character:
		{
			filterContents = ETypeSortFilterContents::Character;
		}
		break;
	case ETESInventoryMainCategory::Equip:
		{
			filterContents = ETypeSortFilterContents::Equip;
		}
		break;
	case ETESInventoryMainCategory::Rune:
		{
			filterContents = ETypeSortFilterContents::Rune;
		}
		break;
	default:
		{
			TESLOG(Error, TEXT("wrong type : %d"), _currentMainCategory);
		}
		break;
	}

	TOptional<FTESFilterData> filterData = SharedSaveGameManager->GetFilterData(filterContents);

	if (false == filterData.IsSet())
	{
		return;
	}

	SetFilterContents(filterData.GetValue());
}

void UTESUWNFTSendToWeb::SetFilterContents(FTESFilterData filterData)
{
	//. 필터 데이터도 갱신해줍니다.
	_currentFilterData = filterData;

	//. 현재의 필터여부에 따라 필터 버튼을 교체해줍니다.
	SetFilterSwitcher(_currentFilterData._isFiltering);

	//. 현재 필터 텍스트를 세팅합니다.
	SetFilterButtonText(_currentFilterData);

	//. 현재 정렬 순서로 정렬 박스를 출력해줍니다.
	SetFilterSortingOrderButton(_currentFilterData._isFiltering, _currentFilterData._sortingOrder);
}

void UTESUWNFTSendToWeb::SetFilterSwitcher(bool isOn)
{
	if (true == ::IsValid(_cts_filterSwitcher))
	{
		_cts_filterSwitcher->SetToggleState(isOn);
	}
}

void UTESUWNFTSendToWeb::SetFilterButtonText(FTESFilterData filterData)
{
	const FSortFilterElementRow& filterElementTable = SharedTDBManager->GetSortFilterElementRow(filterData._sortingElementId);

	if (true == filterElementTable.IsEmpty())
	{
		SetFilterButtonText(filterData._isFiltering, TESConstantValue::NoneValue);
	}
	else
	{
		SetFilterButtonText(filterData._isFiltering, filterElementTable.SortFilterNameID);
	}
}

void UTESUWNFTSendToWeb::SetFilterButtonText(bool isEnable, int32 buttonTextId)
{
	if (true == _txt_characterFilterTextMap.Contains(isEnable))
	{
		FString str = TESUtilTable::GetGlobalString(GetWorld(), buttonTextId);
		_txt_characterFilterTextMap[isEnable]->SetText(FText::FromString(*str));
	}
}

void UTESUWNFTSendToWeb::SetFilterSortingOrderButton(FTESFilterData filterData)
{
	SetFilterSortingOrderButton(filterData._isFiltering, filterData._sortingOrder);
}

void UTESUWNFTSendToWeb::SetFilterSortingOrderButton(bool isOn, ETESFilterSortingOrder sortingOrder)
{
	if (true == _chkb_characterFilterSortingOrderMap.Contains(isOn))
	{
		//. Unchecked : 내림차순, checked : 오름차순
		_chkb_characterFilterSortingOrderMap[isOn]->SetCheckedState(sortingOrder == ETESFilterSortingOrder::Descending ?
			ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	}
}

void UTESUWNFTSendToWeb::SetCharacterPortrait(int32 characterPortraitId)
{
	FString resourcePath = TESUtilPath::GetCharacterPortraitPath(characterPortraitId, ECharPortraitType::Common_256x256);
	TESUtilTexture::SetDynamicTexture(_img_characterPortrait, resourcePath);
}

void UTESUWNFTSendToWeb::SetCharacterName(FString characterName)
{
	if (true == ::IsValid(_txt_characterName))
	{
		_txt_characterName->SetText(FText::FromString(characterName));
	}
}

void UTESUWNFTSendToWeb::SetCharacterRole(ETypeCharRole roleType)
{
	SetCharacterRoleIcon(roleType);

	SetCharacterRoleText(roleType);
}

void UTESUWNFTSendToWeb::SetCharacterRoleIcon(ETypeCharRole roleType)
{
	TESUtilTexture::SetDynamicTexture(_img_characterRole, TESUtilPath::GetCharacterRoleIconResourcePath(roleType));
}

void UTESUWNFTSendToWeb::SetCharacterRoleText(ETypeCharRole roleType)
{
	if (true == ::IsValid(_txt_characterRole))
	{
		_txt_characterRole->SetText(FText::FromString(TESUtils::GetCharacterRoleString(GetWorld(), roleType)));
	}
}

void UTESUWNFTSendToWeb::SetCharacterAttackType(ETypeCharAttack attackType)
{
	SetCharacterAttackTypeIcon(attackType);

	SetCharacterAttackTypeText(attackType);
}

void UTESUWNFTSendToWeb::SetCharacterAttackTypeIcon(ETypeCharAttack attackType)
{
	TESUtilTexture::SetDynamicTexture(_img_characterAttackType, TESUtilPath::GetCharacterAttackTypeIconResourcePath(attackType));
}

void UTESUWNFTSendToWeb::SetCharacterAttackTypeText(ETypeCharAttack attackType)
{
	if (true == ::IsValid(_txt_characterAttackType))
	{
		_txt_characterAttackType->SetText(FText::FromString(TESUtils::GetCharacterAttackTypeString(GetWorld(), attackType)));
	}
}

void UTESUWNFTSendToWeb::SetCharacterRange(ETypeCharRange rangeType)
{
	SetCharacterRangeIcon(rangeType);

	SetCharacterRangeText(rangeType);
}

void UTESUWNFTSendToWeb::SetCharacterRangeIcon(ETypeCharRange rangeType)
{
	TESUtilTexture::SetDynamicTexture(_img_characterRange, TESUtilPath::GetCharacterRangeIconResourcePath(rangeType));
}

void UTESUWNFTSendToWeb::SetCharacterRangeText(ETypeCharRange rangeType)
{
	if (true == ::IsValid(_txt_characterRange))
	{
		_txt_characterRange->SetText(FText::FromString(TESUtils::GetCharacterRangeTypeString(GetWorld(), rangeType)));
	}
}

void UTESUWNFTSendToWeb::SetCharacterSpeciality(ETypeCharSpeciality specialityType)
{
	SetCharacterSpecialityIcon(specialityType);

	SetCharacterSpecialityText(specialityType);
}

void UTESUWNFTSendToWeb::SetCharacterSpecialityIcon(ETypeCharSpeciality specialityType)
{
	TESUtilTexture::SetDynamicTexture(_img_characterSpeciality, TESUtilPath::GetCharacterSpecialityTypeIconResourcePath(specialityType));
}

void UTESUWNFTSendToWeb::SetCharacterSpecialityText(ETypeCharSpeciality specialityType)
{
	if (true == ::IsValid(_txt_characterSpecialty))
	{
		_txt_characterSpecialty->SetText(FText::FromString(TESUtils::GetCharacterSpecialityTypeString(GetWorld(), specialityType)));
	}
}

void UTESUWNFTSendToWeb::SetCharacterLevelValue(int32 currentLevel, int32 maximumLevel)
{
	if (true == ::IsValid(_txt_characterCurrentLevel))
	{
		_txt_characterCurrentLevel->SetText(FText::FromString(FString::Printf(TEXT("%d"), currentLevel)));
	}

	if (true == ::IsValid(_txt_characterMaximumLevel))
	{
		_txt_characterMaximumLevel->SetText(FText::FromString(FString::Printf(TEXT("%d"), maximumLevel)));
	}
}

void UTESUWNFTSendToWeb::SetCharacterAwakeningValue(int32 awakeningValue)
{
	if (true == ::IsValid(_txt_characterAwakeningValue))
	{
		_txt_characterAwakeningValue->SetText(FText::FromString(FString::FromInt(awakeningValue)));
	}
}

void UTESUWNFTSendToWeb::SetCharacterAwakeningRemainValue(int32 currentAwakenTryCount, int32 maximumAwakenTryCount)
{
	if (true == ::IsValid(_txt_characterAwakeningRemainValue))
	{
        int32 remainTryCount = maximumAwakenTryCount - currentAwakenTryCount;

		remainTryCount = FMath::Clamp(remainTryCount, 0, remainTryCount);

		_txt_characterAwakeningRemainValue->SetText(FText::FromString(FString::FromInt(remainTryCount)));
	}
}

void UTESUWNFTSendToWeb::SetCharacterGrade(ETypeGrade gradeType)
{
	if (true == ::IsValid(_img_characterGrade))
	{
		if (ETypeGrade::None == gradeType)
		{
			_img_characterGrade->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			_img_characterGrade->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			TESUtilTexture::SetDynamicTexture(_img_characterGrade, TESUtilPath::GetCharacterGradeImagePath(gradeType, ETESGradeTextureType::Right));
		}
	}
}

void UTESUWNFTSendToWeb::SetCharacterPrestige(bool isPrestige)
{
	ESlateVisibility visibility = (true == isPrestige) ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed;

	if (true == ::IsValid(_img_characterPrestigeEmblem))
	{
		_img_characterPrestigeEmblem->SetVisibility(visibility);
	}
}

void UTESUWNFTSendToWeb::SetItemDetailInfo(class UTESGDGoodsItem* itemData)
{
	if (false == ::IsValid(itemData))
	{
		TESLOG(Error, TEXT("item Data is invalid"));
		return;
	}

	SetItemInfoPanelVisible(true);

	SetItemName(itemData->GetItemName());

	SetItemIcon(itemData->GetItemIconPath());

	SetItemMinimumRequiredLevelText();

	SetItemMinimumRequiredStatValueList();

	SetItemOptions();
}

void UTESUWNFTSendToWeb::SetItemInfoPanelVisible(bool isVisible)
{
	TESCHECK(nullptr != _cp_itemPanel);
	_cp_itemPanel->SetVisibility(true == isVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UTESUWNFTSendToWeb::SetItemOptionPanelVisible(int32 itemOptionIndex, bool isVisible)
{
	if (true == _cp_mainOptionPanelList.IsValidIndex(itemOptionIndex))
	{
		_cp_mainOptionPanelList[itemOptionIndex]->SetVisibility(true == isVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	}
}

void UTESUWNFTSendToWeb::SetItemName(FString itemNameString)
{
	if (true == ::IsValid(_txt_itemName))
	{
		_txt_itemName->SetText(FText::FromString(itemNameString));
	}
}

void UTESUWNFTSendToWeb::SetItemIcon(FString resourcePath)
{
	TESCHECK(nullptr != _img_itemIcon);
	TESUtilTexture::SetDynamicTexture(_img_itemIcon, resourcePath);
}

void UTESUWNFTSendToWeb::SetItemMinimumRequiredLevelText()
{
	TESCHECK(nullptr != _currentItemData);

	const FItemRow& itemTable = _currentItemData->GetItemTable();
	if (true == itemTable.IsEmpty())
	{
		TESLOG(Error, TEXT("item table is invalid"));
		return;
	}

	int32 requiredLevel = itemTable.RequiredLevel;

	TESCHECK(nullptr != _txt_itemRequiredLevel);
	_txt_itemRequiredLevel->SetText(FText::FromString(FString::FromInt(requiredLevel)));
}

void UTESUWNFTSendToWeb::SetItemMinimumRequiredStatValueList()
{
	TESCHECK(nullptr != _currentItemData);
	const FItemRow& itemTableRow = _currentItemData->GetItemTable();
	if (true == itemTableRow.IsEmpty())
	{
		TESLOG(Error, TEXT("Item Table Is Invalid"));
		return;
	}

	for (TPair<ETypeStatBase, class UTESTextBlock*> child : _txt_requiredStatMap)
	{
		if (true == ::IsValid(child.Value))
		{
			child.Value->SetText(FText::FromString(TEXT("0")));
		}
	}
	

	int32 startCondition = static_cast<int32>(EItemColumn::RequiredStat01);
	int32 endCondition = static_cast<int32>(EItemColumn::RequiredStatMax);

	for (int32 i = startCondition; i <= endCondition; ++i)
	{
		FTESStatTotal statTotal = itemTableRow.GetColumnData<FTESStatTotal>(static_cast<EItemColumn>(i));
		if (statTotal._statBaseType == ETypeStatBase::None)
		{
			continue;
		}

		if (true == _txt_requiredStatMap.Contains(statTotal._statBaseType))
		{
			_txt_requiredStatMap[statTotal._statBaseType]->SetText(FText::FromString(FString::FromInt(statTotal._value)));
		}
	}
}

void UTESUWNFTSendToWeb::SetItemOptions()
{
	TESCHECK(nullptr != _currentItemData);
	TArray<FTESItemOption> itemOptionList = _currentItemData->GetItemOptionDatas();

	//. 보여줄수 있는 개수가 최대 5개 옵션까지 보여준다.
	for (int32 index = 0; index < 5; ++index)
	{
		if (false == itemOptionList.IsValidIndex(index))
		{
			continue;
		}

		int16 itemOptionType = itemOptionList[index]._stat;
		int32 itemOptionValue = itemOptionList[index]._value;

		if (itemOptionType <= 0)
		{
			SetItemOptionPanelVisible(index, false);
			SetItemOption(index, static_cast<ETypeStatOption>(itemOptionType), itemOptionValue, false);
		}
		else
		{
			SetItemOptionPanelVisible(index, true);
			SetItemOption(index, static_cast<ETypeStatOption>(itemOptionType), itemOptionValue, true);
		}

	}
}

void UTESUWNFTSendToWeb::SetItemOption(int32 itemOptionIndex
									,  ETypeStatOption itemOptionType
									,  int32 itemOptionValue
									,  bool isVisibled)
{
	if (true == _txt_mainOptionNameList.IsValidIndex(itemOptionIndex))
	{
		FString statOptionName = TESUtilStatus::MakeStatOptionNameString(GetWorld(), itemOptionType);
		_txt_mainOptionNameList[itemOptionIndex]->SetText(FText::FromString(*statOptionName));
	}

	if (true == _txt_mainOptionValueList.IsValidIndex(itemOptionIndex))
	{
		_txt_mainOptionValueList[itemOptionIndex]->SetText(FText::FromString(FString::Printf(TEXT("%d"), itemOptionValue)));
	}
}

void UTESUWNFTSendToWeb::RefreshCharacterUIButtons()
{
	SetCharacterUnlockButton();
	SetCharacterClearButton();
	SetSendButtonByCharacter();
}

void UTESUWNFTSendToWeb::SetSendButtonByCharacter()
{
	//. 해당 캐릭터의 언락과 클리어가 전부 완료된 상태면 On
	//. 현재 시점에서는 캐릭터 락/언락 기능이 없어서 전부 클리어 되어 있는지만 확인한다.
	TESCHECK(nullptr != _cts_unlockCharacterButton);
	TESCHECK(nullptr != _cts_clearCharacterButton);
	TESCHECK(nullptr != _currentCharacterData);

	bool isUnlockActiveStateOn = _cts_unlockCharacterButton->IsActiveState();

	bool isClearButtonActiveStateOn = _cts_clearCharacterButton->IsActiveState();

	bool isCharacterAbleToNFT = false;
	const FCharacterRow& characterRow = _currentCharacterData->GetCharacterTable();
	if (false == characterRow.IsEmpty())
	{
		isCharacterAbleToNFT = characterRow.IsNFTAble;
	}

	if (false == isClearButtonActiveStateOn && false == isUnlockActiveStateOn && true == isCharacterAbleToNFT)
	{
		//SetSendButtonByCharacter(true);
		_cts_sendToWebButtonByCharacter->SetToggleState(true);
	}
	else
	{
		SetSendButtonByCharacter(false);
	}
}

void UTESUWNFTSendToWeb::SetSendButtonByCharacter(bool isActive)
{
	if (true == ::IsValid(_cts_sendToWebButtonByCharacter))
	{
		_cts_sendToWebButtonByCharacter->SetToggleState(isActive);
	}
}

void UTESUWNFTSendToWeb::SetCharacterUnlockButton()
{
	//. 현재 캐릭터 잠금 기능이 없으므로 추후에 생기면 개발 예정.
	SetCharacterUnlockButton(false);

	if (true == ::IsValid(_currentCharacterData))
	{
		bool isCharacterLocked = _currentCharacterData->GetCharacterLocked();

		SetCharacterUnlockButton(true == isCharacterLocked ? true : false);
	}
}

void UTESUWNFTSendToWeb::SetCharacterUnlockButton(bool isActive)
{
	if (true == ::IsValid(_cts_unlockCharacterButton))
	{
		_cts_unlockCharacterButton->SetToggleState(isActive);
	}
}

void UTESUWNFTSendToWeb::SetCharacterClearButton()
{
	//. 캐릭터를 웹에 전송하기 위해서는 사전작업을 체크해야 한다.
	//. 캐릭터가 진행중인 생산활동이 있는지.
	//. 캐릭터가 어느 파티편성에라도 속해 있는지.
	//. 캐릭터가 별의 성소에 등록이 되어 있는지.
	//. 캐릭터가 장착중인 아이템이 있는지.
	//. 위에 조건중 하나라도 만족하지 않으면 False로 설정해야 한다.

	if (true == ::IsValid(_currentCharacterData))
	{
		//. 진행중인 생산 활동이 있는지 확인
		int32 isAssigned = SharedFacilityManager->GetAssignedPartyId(_currentCharacterData->GetCharacterUnique());

		if (0 != isAssigned)
		{
			SetCharacterClearButton(true);
			return;
		}

		//. 파티편성에 등록이 되어 있는지.
		TArray<uint16> partyUniqueList = SharedPartySettingManager->GetPartyUniqueListByCharacterUnique(_currentCharacterData->GetCharacterUnique());
		if (0 < partyUniqueList.Num())
		{
			SetCharacterClearButton(true);
			return;
		}

		//. 별의 성소에 이미 등록 되어 있는지 확인.
		TOptional<FTESStellaStrikePartyInfo> stellaStrikePartyInfo = SharedUserDataManager->FindStellaStrikePartyInfoByCharacterUnique(_currentCharacterData->GetCharacterUnique());

		//. 다른 별의 성소의 슬롯에 참여중인 캐릭터라면 False
		if (true == stellaStrikePartyInfo.IsSet())
		{
			SetCharacterClearButton(true);

			return;
		}

		//. 캐릭터가 장착중인 아이템이 있는지 확인
		int32 maxCount = static_cast<int32>(ETypeEquipment::MAX);
		for (int32 index = 1; index < maxCount; ++index)
		{
			ETypeEquipment equipmentType = static_cast<ETypeEquipment>(index);
			UTESGDGoodsItem* slotItemData =	_currentCharacterData->GetEuipmentSlotItemData(equipmentType);
			if (nullptr != slotItemData)
			{
				SetCharacterClearButton(true);
				return;
			}
		}

		SetCharacterClearButton(false);
	}
}

void UTESUWNFTSendToWeb::SetCharacterClearButton(bool isActive)
{
	if (true == ::IsValid(_cts_clearCharacterButton))
	{
		_cts_clearCharacterButton->SetToggleState(isActive);
	}
}

void UTESUWNFTSendToWeb::SetSendButtonByItem()
{
	TESCHECK(nullptr != _cts_clearItemButton);
	TESCHECK(nullptr != _cts_unlockItemButton);
	TESCHECK(nullptr != _currentItemData);

	//. 해당 아이템의 언락과 클리어가 전부 완료된 상태면 On
	bool isClearButtonActiveStateOn = _cts_clearItemButton->IsActiveState();

	bool isUnlockActiveStateOn = _cts_unlockItemButton->IsActiveState();

	bool isItemAbleToNFT = false;
	const FItemRow& itemRow = _currentItemData->GetItemTable();

	if (false == itemRow.IsEmpty())
	{
		isItemAbleToNFT = itemRow.IsNFTAble;
	}

	if (false == isClearButtonActiveStateOn && false == isUnlockActiveStateOn && true == isItemAbleToNFT)
	{
		SetSendButtonByItem(true);
	}
	else
	{
		SetSendButtonByItem(false);
	}
}

void UTESUWNFTSendToWeb::SetSendButtonByItem(bool isActive)
{
	if (true == ::IsValid(_cts_clearItemButton))
	{
		_cts_sendToWebButtonByItem->SetToggleState(isActive);
	}
}

void UTESUWNFTSendToWeb::SetItemUnlockButton()
{
	//. 현재 아이템이 잠금 상태인지 확인하고 잠금 상태이면 활성화 한다.
	if (true == ::IsValid(_currentItemData))
	{
		bool isItemLocked = _currentItemData->IsLocked();

		SetItemUnlockButton(true == isItemLocked ? true : false);
	}
}

void UTESUWNFTSendToWeb::SetItemUnlockButton(bool isActive)
{
	if (true == ::IsValid(_cts_clearItemButton))
	{
		_cts_unlockItemButton->SetToggleState(isActive);
	}
}

void UTESUWNFTSendToWeb::SetItemClearButton()
{
	//. 현재 아이템이 장착중인 캐릭이 없으면 버튼을 비활성화 한다.
	if (true == ::IsValid(_currentItemData))
	{
		if (0 == _currentItemData->GetItemUsingCharacterUnique())
		{
			SetItemClearButton(false);
		}
		else
		{
			SetItemClearButton(true);
		}
	}
	else
	{
		SetItemClearButton(false);
	}
}

void UTESUWNFTSendToWeb::SetItemClearButton(bool isActive)
{
	if (true == ::IsValid(_cts_clearItemButton))
	{
		_cts_clearItemButton->SetToggleState(isActive);
	}
}

UTESGDGoodsItem* UTESUWNFTSendToWeb::GetItemCategoryData(ETESItemInventoryMainCategory categoryType, int64 itemUnique)
{
	if (false == _categoryItemDataMap.Contains(categoryType))
	{
		TESLOG(Error, TEXT("Not Found Category. : %d"), categoryType);

		return nullptr;
	}

	TMap<int32, UTESGDGoodsItem*> itemDataMap = _categoryItemDataMap[categoryType]._itemDataMap;
	if (false == itemDataMap.Contains(itemUnique))
	{
		TESLOG(Error, TEXT("Not Found ItemData. Category : %d, ItemUnique : %d"), categoryType, itemUnique);

		return nullptr;
	}

	return itemDataMap[itemUnique];
}

TArray<class UTESGDGoodsItem*> UTESUWNFTSendToWeb::GetItemCategoryDatas(ETESItemInventoryMainCategory categoryType)
{
	if (true == _categoryItemDataMap.Contains(categoryType))
	{
		TArray<UTESGDGoodsItem*> itemDatas = {};

		for (TPair<int32, class UTESGDGoodsItem*> child : _categoryItemDataMap[categoryType]._itemDataMap)
		{
			itemDatas.Add(child.Value);
		}

		return itemDatas;
	}

	return {};
}

void UTESUWNFTSendToWeb::CloseInventoryView()
{
	//. 캐릭터 상세보기 패널을 닫아줍니다.
	SetCharacterPanelVisible(false);

	//. 아이템 상세보기 패널을 닫아줍니다.
	SetItemInfoPanelVisible(false);

	//. 선택된 아이템의 정보도 해제하여 줍니다.
	ClearSelectItem();
}

TArray<class UTESGDCharacter*> UTESUWNFTSendToWeb::MakeFilteredCharacterDataList(TArray<class UTESGDCharacter*> source, TMap<int32, FTESFilterFilteringGroupData> filteringList, bool isFiltering)
{
	return TESUtilFilter::MakeFilteredCharacterDataList(source, filteringList, isFiltering);
}

TArray<class UTESGDCharacter*> UTESUWNFTSendToWeb::MakeSortedCharacterDataList(TArray<class UTESGDCharacter*> sourceCharacterDataList, int32 sortingData, ETESFilterSortingOrder sortingOrder)
{
	return TESUtilFilter::MakeSortedCharacterDataList(sourceCharacterDataList, sortingData, sortingOrder);
}

TArray<class UTESGDGoodsItem*> UTESUWNFTSendToWeb::MakeFilteredItemDataList(TArray<class UTESGDGoodsItem*> itemDataList, TMap<int32, FTESFilterFilteringGroupData> filteringData, bool isFiltering)
{
	return TESUtilFilter::MakeFilteredItemDataList(itemDataList, filteringData, isFiltering);
}

TArray<class UTESGDGoodsItem*> UTESUWNFTSendToWeb::MakeSortedItemDataList(TArray<class UTESGDGoodsItem*> itemDataList, int32 sortingElementId, ETESFilterSortingOrder sortingOrder)
{
	return TESUtilFilter::MakeSortedItemDataList(itemDataList, sortingElementId, sortingOrder);
}

void UTESUWNFTSendToWeb::OnEvent_EndMinting()
{
	FTESMessageBoxInitData messageBoxInitData = {};
	messageBoxInitData._messageBoxType = ETESMessageBoxType::Normal;
	messageBoxInitData._messageBoxSubType = ETESMessageBoxSubType::OneButton;
	messageBoxInitData._messageHeaderString = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_Notice);
	messageBoxInitData._messageContextString = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::P2E_Request_Processed_Item);
	messageBoxInitData._messageButton2String = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_Confirm);
	messageBoxInitData._messageButton2Callback = FOnTESAction::CreateLambda([=]()
	{
		OnEvent_ChangeMainCategory(_currentMainCategory);
	});

	SharedPopupManager->ShowMessageBox(messageBoxInitData);
}

void UTESUWNFTSendToWeb::OnEvent_ChangeMainCategory(ETESInventoryMainCategory category)
{
	CloseInventoryView();

	if (ETESInventoryMainCategory::Equip == category || ETESInventoryMainCategory::Rune == category)
	{
		SetData();
	}

	SetMainCategory(category, true);
}

void UTESUWNFTSendToWeb::OnEvent_CharacterInventoryView_EntryInitailized(UObject* item, UUserWidget* widget)
{
#if WITH_EDITOR	
	if (false == TESUtils::IsEditorPlayingGame(GetWorld()))
	{
		return;
	}
#endif //. WITH_EDITOR

	UTESUWNFTCharacterInventorySlot* characterSettingInvenSlotWidget = Cast<UTESUWNFTCharacterInventorySlot>(widget);
	if (false == ::IsValid(characterSettingInvenSlotWidget))
	{
		return;
	}

	characterSettingInvenSlotWidget->InitWidget();
	characterSettingInvenSlotWidget->OpenWidget();
	characterSettingInvenSlotWidget->SetWidget(Cast<UTESEDNFTCharacterInventorySlot>(item));

}

void UTESUWNFTSendToWeb::OnEvent_CharacterInventoryView_EntryReleased(UUserWidget* widget)
{
#if WITH_EDITOR	
	if (false == TESUtils::IsEditorPlayingGame(GetWorld()))
	{
		return;
	}
#endif //. WITH_EDITOR

	UTESUWNFTCharacterInventorySlot* characterSettingWidget = Cast<UTESUWNFTCharacterInventorySlot>(widget);
	if (false == ::IsValid(characterSettingWidget))
	{
		return;
	}

	characterSettingWidget->CloseWidget();
	characterSettingWidget->ClearWidget();
}

void UTESUWNFTSendToWeb::OnEvent_CharacterInventorySlotClicked(UTESEDNFTCharacterInventorySlot* characterSlotData)
{
#if WITH_EDITOR	
	if (false == TESUtils::IsEditorPlayingGame(GetWorld()))
	{
		return;
	}
#endif //. WITH_EDITOR

	_currentCharacterData = characterSlotData->GetSlotGDCharacter();

	if (_prevSelectedSlot == characterSlotData)
	{
		return;
	}

	if (false == ::IsValid(_currentCharacterData))
	{
		TESLOG(Error, TEXT("Invalid CharacterData."));

		return;
	}

	//. 이전에 눌려져있는 VFX를 꺼준다.
	if (nullptr != _prevSelectedSlot)
	{
		_prevSelectedSlot->SetSelectedVFX(false);
	}

	//. 현재 누른 VFX를 켜준다.
	characterSlotData->SetSelectedVFX(true);

	SetCharacterProfile();

	RefreshCharacterUIButtons();

	_prevSelectedSlot = characterSlotData;
}

void UTESUWNFTSendToWeb::OnEvent_ItemInventoryView_EntryInitailized(UObject* item, UUserWidget* widget)
{
#if WITH_EDITOR	
	if (false == TESUtils::IsEditorPlayingGame(GetWorld()))
	{
		return;
	}
#endif //. WITH_EDITOR

	UTESUWCharInfoContentsItemSlot* itemSlotWidget = Cast<UTESUWCharInfoContentsItemSlot>(widget);
	if (false == ::IsValid(itemSlotWidget))
	{
		return;
	}

	itemSlotWidget->InitWidget();
	itemSlotWidget->OpenWidget();
	itemSlotWidget->SetWidget(Cast<UTESEDCharInfoContentsItemSlot>(item));
}

void UTESUWNFTSendToWeb::OnEvent_ItemInventoryView_EntryReleased(UUserWidget* widget)
{
#if WITH_EDITOR	
	if (false == TESUtils::IsEditorPlayingGame(GetWorld()))
	{
		return;
	}
#endif //. WITH_EDITOR

	UTESUWCharInfoContentsItemSlot* itemSlotWidget = Cast<UTESUWCharInfoContentsItemSlot>(widget);
	if (false == ::IsValid(itemSlotWidget))
	{
		return;
	}

	itemSlotWidget->CloseWidget();
	itemSlotWidget->ClearWidget();
}

void UTESUWNFTSendToWeb::OnEvent_ItemInventorySlotClicked(UTESGDGoodsItem* selectedItemData)
{
#if WITH_EDITOR	
	if (false == TESUtils::IsEditorPlayingGame(GetWorld()))
	{
		return;
	}
#endif //. WITH_EDITOR

	if (false == ::IsValid(_tv_itemInventoryViewContainer))
	{
		return;
	}

	if (false == ::IsValid(selectedItemData))
	{
		return;
	}

	//. 같은 아이템을 누를 경우, 아무것도 하지 않습니다.
	int64 itemUnique = selectedItemData->GetItemUnique();
	if (itemUnique == _selectedItemUnique)
	{
		return;
	}

	//. 이전에 선택된 위젯을 찾아 선택된 상태를 해제하여 줍니다.
	if (true == _currentInventoryItemSlotDatas.Contains(_selectedItemUnique))
	{
		if (true == ::IsValid(_currentInventoryItemSlotDatas[_selectedItemUnique]))
		{
			_currentInventoryItemSlotDatas[_selectedItemUnique]->SetItemSelected(false);

			UTESUWCharInfoContentsItemSlot* itemSlotWidget = Cast<UTESUWCharInfoContentsItemSlot>(_tv_itemInventoryViewContainer->GetEntryWidgetFromItem(_currentInventoryItemSlotDatas[_selectedItemUnique]));
			if (true == ::IsValid(itemSlotWidget))
			{
				itemSlotWidget->SetItemSelected(false);
			}
		}
	}

	_selectedItemUnique = itemUnique;

	if (true == _currentInventoryItemSlotDatas.Contains(_selectedItemUnique))
	{
		if (true == ::IsValid(_currentInventoryItemSlotDatas[_selectedItemUnique]))
		{
			_currentInventoryItemSlotDatas[_selectedItemUnique]->SetItemSelected(true);
		}
	}

	UTESUWCharInfoContentsItemSlot* itemSlotWidget = Cast<UTESUWCharInfoContentsItemSlot>(_tv_itemInventoryViewContainer->GetEntryWidgetFromItem(_currentInventoryItemSlotDatas[_selectedItemUnique]));
	if (true == ::IsValid(itemSlotWidget))
	{
		itemSlotWidget->SetItemSelected(true);
	}

	//. 선택된 인덱스를 갱신하여줍니다.
	_selectedItemUnique = itemUnique;

	//. 선택된 아이템을 갱신해줍니다.
	_currentItemData = selectedItemData;

	SetItemDetailInfo(_currentItemData);

	SetItemUnlockButton();

	SetItemClearButton();

	SetSendButtonByItem();
}

void UTESUWNFTSendToWeb::OnEvent_Filter()
{
	UTESUWFilter* filterWidget = SharedPopupManager->CreatePopupWidget<UTESUWFilter>(TEXT("WBP_Filter"), TEXT("Common/WBP/Filter"));
	if (false == ::IsValid(filterWidget))
	{
		TESLOG(Error, TEXT("Invalid FilterWidget."));

		return;
	}

	switch (_currentMainCategory)
	{
	case ETESInventoryMainCategory::Character:
		{
			ETypeSortFilterContents filterContentsType = ETypeSortFilterContents::Character;
			filterWidget->InitWidget();
			filterWidget->OpenWidget();
			filterWidget->SetWidget(filterContentsType);
		}
		break;
	case ETESInventoryMainCategory::Equip:
		{
			ETypeSortFilterContents filterContentsType = ETypeSortFilterContents::Equip;
			filterWidget->InitWidget();
			filterWidget->OpenWidget();
			filterWidget->SetWidget(filterContentsType);
		}
		break;
	case ETESInventoryMainCategory::Rune:
		{
			ETypeSortFilterContents filterContentsType = ETypeSortFilterContents::Rune;

			filterWidget->InitWidget();
			filterWidget->OpenWidget();
			filterWidget->SetWidget(filterContentsType);
		}
		break;
	default:
		{
			TESLOG(Error, TEXT("wrong type : %d"), _currentMainCategory);
		}
		break;
	}
}

void UTESUWNFTSendToWeb::OnEvent_ChangeSortingOrder()
{
	//. 현재의 정렬 순서를 가져와서 반전 시킵니다.
	ETESFilterSortingOrder currentSortingOrder = _currentFilterData._sortingOrder;
	switch (currentSortingOrder)
	{
	case ETESFilterSortingOrder::Descending:
		{
			currentSortingOrder = ETESFilterSortingOrder::Ascending;
		}
		break;

	case ETESFilterSortingOrder::Ascending:
		{
			currentSortingOrder = ETESFilterSortingOrder::Descending;
		}
		break;

	default:
		{
			TESLOG(Error, TEXT("Invalid SortingOrder. : %d"), currentSortingOrder);

			return;
		}
		break;
	}

	//. 캐릭터의 필터 데이터를 갱신합니다.
	_currentFilterData._sortingOrder = currentSortingOrder;

	//. 데이터 매니져의 필터 데이터를 갱신합니다.
	SharedSaveGameManager->SetFilterData(_currentFilterData._contentsType, _currentFilterData);

	//. 정렬된 리스트를 컨테이너에 출력합니다.
	SetViewContents(_currentMainCategory);

	//. 현재의 정렬 순서로 정렬 박스를 출력해줍니다.
	SetFilterSortingOrderButton(_currentFilterData._isFiltering, _currentFilterData._sortingOrder);
}

void UTESUWNFTSendToWeb::OnEvent_FilterApply(struct FTESFilterData filterData)
{
	_currentFilterData = filterData;

	SetFilterContents(_currentFilterData);

	//. 데이터 매니져의 필터 데이터를 갱신합니다.
	SharedSaveGameManager->SetFilterData(_currentFilterData._contentsType, _currentFilterData);

	//. 적용 혹은 롤백이 된 세팅을 적용
	SetViewContents(_currentMainCategory);
}

void UTESUWNFTSendToWeb::OnEvent_CharacterDetailView()
{
    FTESSceneTransitionData sceneTransitionData = {};
    sceneTransitionData._physicalSceneType = SharedSceneManager->GetCurrentPhysicalGameSceneType();
    sceneTransitionData._logicalSceneType = ETESLogicalGameSceneType::CharacterInformation;
    sceneTransitionData._sceneArguments._int64Values.Add(TEXT("CharacterUnique"), _currentCharacterData->GetCharacterUnique());
    sceneTransitionData._remoteSceneURL = TEXT("");
    sceneTransitionData._fadeAtrribute._fadeType = ETESSceneFadeType::WidgetAnimation;
    sceneTransitionData._fadeAtrribute._fadeColor = FColor::Black;

    SharedSceneManager->ChangeGameScene(sceneTransitionData);

	//. CloseWidget();
	//. 
	//. UTESUWLobbyMain* lobbyMainWidget = SharedPopupManager->FindPopupWidget<UTESUWLobbyMain>();
	//. if (true == ::IsValid(lobbyMainWidget))
	//. {
	//. 	lobbyMainWidget->CloseWidget();
	//. }
	//. 
	//. if (true == ::IsValid(_currentCharacterData))
	//. {
	//. 	SharedCharacterInformationManager->InitScene(ETESCharacterInformationEntryRoute::None);
	//. 	SharedCharacterInformationManager->EnterScene(_currentCharacterData->GetCharacterUnique());
	//. }

	//. 캐릭터 정보 위젯 생성
	//UTESUWCharInfoMain* characterInfoWidget = Cast<UTESUWCharInfoMain>(SharedPopupManager->CreatePopupWidget(TEXT("WBP_CharInfoMain"), TEXT("Common/WBP/CharacterInformation")));
	//TESCHECK(nullptr != characterInfoWidget);
	//characterInfoWidget->InitWidget();
	//characterInfoWidget->OpenWidget();
	//characterInfoWidget->SetWidget(_currentCharacterData);

	////. 카메라 이동
	//ATESPlayerControllerLobby* playerControllerLobby = Cast<ATESPlayerControllerLobby>(GetWorld()->GetFirstPlayerController());
	//TESCHECK(nullptr != playerControllerLobby);
	//playerControllerLobby->ChangeViewTarget(TEXT("CharacterCameraActor"));			//. 캐릭터 정보창에 있는 카메라로 이동
	//playerControllerLobby->SetEnableInputByCharacter(false);
}

void UTESUWNFTSendToWeb::OnEvent_ItemDetailView()
{
	SharedLobbyInputManager->InputItemInventoryOpen(_currentItemData->GetItemUnique());
}

void UTESUWNFTSendToWeb::OnEvent_CharacterUnlock()
{
	SharedNetworkManager->CLGW_QRY_LOCK_CHARACTER(_currentCharacterData->GetCharacterUnique(), false);
}

void UTESUWNFTSendToWeb::OnEvent_CharacterUnlockComplete(bool isLock)
{
	SetCharacterViewContents();

	RefreshCharacterUIButtons();

	FString lockString = TESUtilTable::GetGlobalString(GetWorld(), (true == isLock) ? EGStringID::Char_Info_Locked : EGStringID::Char_Info_Unlocked);
	SharedPopupManager->ShowMessageBox(lockString);
}

void UTESUWNFTSendToWeb::OnEvent_CharacterClear()
{
	UTESUWObjectClearPopup* objectClearPopup = SharedPopupManager->CreatePopupWidget<UTESUWObjectClearPopup>(TEXT("WBP_P2E_ObjectClear_Popup"), TEXT("P2E/WBP"));
	if (false == ::IsValid(objectClearPopup))
	{
		TESLOG(Error, TEXT("Invalid objectClearPopup."));

		return;
	}

	objectClearPopup->InitWidget();
	objectClearPopup->OpenWidget();
	objectClearPopup->SetWidget(_currentCharacterData, nullptr);
}

void UTESUWNFTSendToWeb::OnEvent_ItemUnlock()
{
	//. 아이템 잠금을 처리해달라고 서버에게 요청합니다 :)
	TESCHECK(nullptr != _currentItemData);

	bool isLocked = !_currentItemData->IsLocked();
	SharedNetworkManager->CLGW_QRY_LOCK_ITEM(_currentItemData->GetItemUnique(), isLocked);
}

void UTESUWNFTSendToWeb::OnEvent_ItemUnlockComplete(int64 itemUnique)
{
	//. 아이템 데이터로부터 갱신합니다.
	TOptional<UTESGDGoodsItem*> itemDataOptional = SharedUserDataManager->FindItemData(itemUnique);
	if (false == itemDataOptional.IsSet())
	{
		TESLOG(Error, TEXT("Not Found ItemData."));

		return;
	}

	//. 해당 아이템이 유효하지 않다면 무시합니다.
	UTESGDGoodsItem* itemData = itemDataOptional.GetValue();
	if (false == ::IsValid(itemData))
	{
		TESLOG(Error, TEXT("Invalid ItemData."));

		return;
	}

	//. 방금 잠금 완료된 것이 현재 아이템하고 다르다면 무시합니다.
	if (_currentItemData->GetItemUnique() != itemData->GetItemUnique())
	{
		return;
	}

	//. 아이템 데이터 업데이트.
	_currentItemData = itemData;

	//. 아이템 잠긴 상태를 UI에 갱신.
	SetItemUnlockButton();

	//. 갱신
	SetSendButtonByItem();

	//. refresh
	SetViewContents(_currentMainCategory);
}

void UTESUWNFTSendToWeb::OnEvent_ItemClear()
{
	UTESUWObjectClearPopup* objectClearPopup = SharedPopupManager->CreatePopupWidget<UTESUWObjectClearPopup>(TEXT("WBP_P2E_ObjectClear_Popup"), TEXT("P2E/WBP"));
	if (false == ::IsValid(objectClearPopup))
	{
		TESLOG(Error, TEXT("Invalid objectClearPopup."));

		return;
	}

	objectClearPopup->InitWidget();
	objectClearPopup->OpenWidget();
	objectClearPopup->SetWidget(nullptr, _currentItemData);
}

void UTESUWNFTSendToWeb::OnEvent_ItemClearComplete(TArray<class UTESGDGoodsItem*> itemDatas)
{
	//. 장착 완료 이벤트가 도착하였는데, 변경된 장착 아이템 데이터가 없다면 무시합니다.
	if (itemDatas.Num() <= 0)
	{
		TESLOG(Error, TEXT("Empty UnEquipCompleteData."));
		return;
	}

	SetItemClearButton();

	SetSendButtonByItem();

	//. refresh
	SetViewContents(_currentMainCategory);
}

void UTESUWNFTSendToWeb::OnEvent_SendToWebByCharacter()
{
	//. 밀봉이 되어 있으면 공용 팝업을 띄워준다.
	TESCHECK(nullptr != _currentCharacterData);
	if (true == _currentCharacterData->GetCharacterSealed())
	{
		FTESMessageBoxInitData messageBoxInitData = {};
		messageBoxInitData._messageBoxType = ETESMessageBoxType::Normal;
		messageBoxInitData._messageBoxSubType = ETESMessageBoxSubType::TwoButton;
		messageBoxInitData._messageHeaderString = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::P2E_SendtoWeb_SendtoWeb_Popup_Title);
		messageBoxInitData._messageContextString = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::P2E_SendtoWeb_Sendtoweb_Popup_Desc_1);
		messageBoxInitData._messageButton1String = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_Cancel);
		messageBoxInitData._messageButton1Callback = nullptr;
		messageBoxInitData._messageButton2String = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_Confirm);
		messageBoxInitData._messageButton2Callback = FOnTESAction::CreateLambda([&]()
		{
			TESCHECK(nullptr != _currentCharacterData);
			//TESLOG(Error, TEXT("main Net : %d"), _currentCharacterData->GetCharacterNFTMainType());
			SharedNetworkManager->CLGW_QRY_SFTYASSET_CHARACTER(_currentCharacterData->GetCharacterUnique(), _currentCharacterData->GetCharacterNFTMainType());

			UTESUWNFTInProgressPopup* progressPopup = SharedPopupManager->CreatePopupWidget<UTESUWNFTInProgressPopup>(TEXT("WBP_P2E_InProgress_Popup"), TEXT("P2E/WBP"));
			if (true == ::IsValid(progressPopup))
			{
				progressPopup->InitWidget();
				progressPopup->OpenWidget();
				progressPopup->SetWidget();
			}
		});

		SharedPopupManager->ShowMessageBox(messageBoxInitData);
	}
	//. 밀봉이 안되어 있는 캐릭은 민팅 팝업을 띄워준다.
	else
	{
		UTESUWNFTNetworkSelectionPopup* networkSelectionPopup = SharedPopupManager->CreatePopupWidget<UTESUWNFTNetworkSelectionPopup>(TEXT("WBP_P2E_NetworkSelection_Popup"), TEXT("P2E/WBP"));
		TESCHECK(nullptr != networkSelectionPopup);
		networkSelectionPopup->InitWidget();
		networkSelectionPopup->OpenWidget();
		networkSelectionPopup->SetWidget(_currentCharacterData, nullptr);
	}
}

void UTESUWNFTSendToWeb::OnEvent_SendToWebByItem()
{
	TESCHECK(nullptr != _currentItemData);
	//. 해당 아이템이 이미 밀봉 되어 있으면 공용팝업으로 처리 해준다.
	if (true == _currentItemData->GetItemSealed())
	{
		FTESMessageBoxInitData messageBoxInitData = {};
		messageBoxInitData._messageBoxType = ETESMessageBoxType::Normal;
		messageBoxInitData._messageBoxSubType = ETESMessageBoxSubType::TwoButton;
		messageBoxInitData._messageHeaderString = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::P2E_SendtoWeb_SendtoWeb_Popup_Title);
		messageBoxInitData._messageContextString = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::P2E_SendtoWeb_Sendtoweb_Popup_Desc_1);
		messageBoxInitData._messageButton1String = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_Cancel);
		messageBoxInitData._messageButton1Callback = nullptr;
		messageBoxInitData._messageButton2String = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Common_Confirm);
		messageBoxInitData._messageButton2Callback = FOnTESAction::CreateLambda([&]()
		{
			TESCHECK(nullptr != _currentItemData);
			//TESLOG(Error, TEXT("main Net : %d"), _currentItemData->GetItemNFTType());
			SharedNetworkManager->CLGW_QRY_SFTYASSET_EQUIP(_currentItemData->GetItemUnique(), _currentItemData->GetItemNFTType());
			UTESUWNFTInProgressPopup* progressPopup = SharedPopupManager->CreatePopupWidget<UTESUWNFTInProgressPopup>(TEXT("WBP_P2E_InProgress_Popup"), TEXT("P2E/WBP"));
			if (true == ::IsValid(progressPopup))
			{
				progressPopup->InitWidget();
				progressPopup->OpenWidget();
				progressPopup->SetWidget();
			}
		});

		SharedPopupManager->ShowMessageBox(messageBoxInitData);
	}
	//. 아닐 경우에는 네트워크 선택 팝업창을 띄워준다.
	else
	{
		UTESUWNFTNetworkSelectionPopup* networkSelectionPopup = SharedPopupManager->CreatePopupWidget<UTESUWNFTNetworkSelectionPopup>(TEXT("WBP_P2E_NetworkSelection_Popup"), TEXT("P2E/WBP"));
		TESCHECK(nullptr != networkSelectionPopup);
		networkSelectionPopup->InitWidget();
		networkSelectionPopup->OpenWidget();
		networkSelectionPopup->SetWidget(nullptr, _currentItemData);
	}
}

void UTESUWNFTSendToWeb::OnEvent_GoToWebSite()
{
	FPlatformProcess::LaunchURL(TESMarketplaceAddress, NULL, NULL);
}

void UTESUWNFTSendToWeb::OnEvent_Back()
{
    //. FTESUtilEvents::OnNFTContentsChanged.ExecuteIfBound(ETESNFTContentsType::NFTMenuMain);

	SharedSceneManager->OnEvent_ContentsBack();

	//. UTESUWNFTMenuMain* mainMenu = SharedPopupManager->FindPopupWidget<UTESUWNFTMenuMain>();
	//. if (nullptr != mainMenu)
	//. {
	//. 	mainMenu->OpenWidget();
	//. }
	//. 
	//. SharedPopupManager->DestroyPopupWidget(this);
}

void UTESUWNFTSendToWeb::OnEvent_Tutorial()
{
	SharedTutorialManager->ShowTutorialPopupAfterCheck(ETESTutorialContentsType::Minting);
}
