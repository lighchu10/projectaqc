// Fill out your copyright notice in the Description page of Project Settings.
#include "Contents/Common/UI/EquipmentEnhance/TESUWEquipmentEnhanceMain.h"
#include "Contents/Common/UI/EquipmentEnhance/TESUWEquipmentEnhanceMessageBox.h"
#include "Contents/Common/UI/CustomSlates/TESTextBlock.h"
#include "Contents/Common/UI/CustomSlates/TESImage.h"
#include "Contents/Common/UI/CustomSlates/TESTileView.h"
#include "Contents/Common/UI/CustomSlates/TESButton.h"
#include "Contents/Common/UI/TESUWGoodsDisplayer.h"
#include "Contents/Common/UI/TESUWCommonToggleSwitcher.h"
#include "Contents/Common/UI/TESUWCommonHeader.h"
#include "Contents/Common/UI/Filter/TESUWFilter.h"
#include "Contents/Common/Manager/TESUserDataManager.h"
#include "Contents/Common/Manager/TESSaveGameManager.h"
#include "Contents/Common/Manager/TESPopupManager.h"
#include "Contents/Common/Manager/TESSceneManager.h"
#include "Contents/Common/GameData/TESGDGoodsItem.h"
#include "Contents/CharacterInformation/Manager/TESCharacterInformationManager.h"
#include "Contents/CharacterInformation/UI/TESEDCharInfoContentsItemSlot.h"
#include "Contents/CharacterInformation/UI/TESUWCharInfoContentsItemSlot.h"
#include "Contents/Help/UI/TESUWHelpButton.h"
#include "Contents/Tutorial/Manager/TESTutorialManager.h"
#include "Components/WidgetSwitcher.h"
#include "Components/CheckBox.h"
#include "Components/VerticalBox.h"
#include "TESGameInstance.h"
#include "TESUtilTexture.h"
#include "TESUtilString.h"
#include "TESUtilFilter.h"
#include "TESUtilStatus.h"
#include "TESUtilPath.h"
#include "TESUtils.h"

void UTESUWEquipmentEnhanceMain::InitWidget()
{
	Super::InitWidget();

	InitSelectedEquipmentIcon();

	InitSelectedEquipmentName();

	InitSelectedEquipmentGrade();

	InitSelectedEquipmentStatNameList();

	InitSelectedEquipmentStatBeforeValueList();

	InitSelectedEquipmentStatAfterValueList();

	InitSelectedEquipmentStatCompareValueList();

	InitContentsInventoryView();

	InitItemFilter();

	InitRootEquipName();

	InitNotSelectedText();

	InitWarningText();

	InitRootStatIncrease();

	InitRequiredMaterialValue();

	InitStatArrow();

	InitEnhanceButton();

	InitCommonHeader();
	
	InitSFTYDisplayer();

	InitManaringDisplayer();

	InitRootMaxTag();
}

void UTESUWEquipmentEnhanceMain::ClearWidget()
{
	Super::ClearWidget();

	ClearSFTYDisplayer();

	ClearManaringDisplayer();

	ClearCommonHeader();
}

void UTESUWEquipmentEnhanceMain::OpenWidget()
{
	Super::OpenWidget();

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if (true == ::IsValid(_wbp_manaringDisplayerWidget))
	{
		_wbp_manaringDisplayerWidget->OpenWidget();
	}
}

void UTESUWEquipmentEnhanceMain::CloseWidget()
{
	Super::CloseWidget();

	SetVisibility(ESlateVisibility::Collapsed);

	if (true == ::IsValid(_wbp_manaringDisplayerWidget))
	{
		_wbp_manaringDisplayerWidget->CloseWidget();
	}
}

void UTESUWEquipmentEnhanceMain::OpenDelegate()
{
	Super::OpenDelegate();

	FTESUtilEvents::OnCommonFilterApply.BindUObject(this, &UTESUWEquipmentEnhanceMain::OnEvent_ApplyEquipFilter);

	FTESUtilEvents::OnCharInfoContentsEquipItemSlotClicked.BindUObject(this, &UTESUWEquipmentEnhanceMain::OnEvent_ItesmSlotClicked);

	FTESUtilEvents::OnEnhanceItem.Remove(_dh_itemEnhancedCompletely);
	_dh_itemEnhancedCompletely = FTESUtilEvents::OnEnhanceItem.AddUObject(this, &UTESUWEquipmentEnhanceMain::OnEvent_EnhanceItem);
}

void UTESUWEquipmentEnhanceMain::CloseDelegate()
{
	Super::CloseDelegate();

	FTESUtilEvents::OnCommonFilterApply.Unbind();

	FTESUtilEvents::OnCharInfoContentsEquipItemSlotClicked.Unbind();

	FTESUtilEvents::OnEnhanceItem.Remove(_dh_itemEnhancedCompletely);
}

void UTESUWEquipmentEnhanceMain::InitSelectedEquipmentIcon()
{
	_img_selectedEquipment = Cast<UTESImage>(GetWidgetFromName(TEXT("IMG_SelectedEquipment")));
	TESCHECK(nullptr != _img_selectedEquipment);
}

void UTESUWEquipmentEnhanceMain::InitSelectedEquipmentName()
{
	_txt_selectedEquipmentName = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_SelectedEuipment")));
	TESCHECK(nullptr != _txt_selectedEquipmentName);
}

void UTESUWEquipmentEnhanceMain::InitSelectedEquipmentGrade()
{
	_img_selectedEquipmentGrade = Cast<UTESImage>(GetWidgetFromName(TEXT("IMG_ItemGrade")));
	TESCHECK(nullptr != _img_selectedEquipmentGrade);
}

void UTESUWEquipmentEnhanceMain::InitSelectedEquipmentStatNameList()
{
	_txt_statNameList.Empty();

	for (int32 index = 1; index <= 3; ++index)
	{
		FString itemName = FString::Printf(TEXT("TXT_StatName_%d"), index);
		UTESTextBlock* statNameText = Cast<UTESTextBlock>(GetWidgetFromName(*itemName));
		TESCHECK_CONTINUE(nullptr != statNameText);

		_txt_statNameList.Add(statNameText);
	}
}

void UTESUWEquipmentEnhanceMain::InitSelectedEquipmentStatBeforeValueList()
{
	_txt_statBeforeValue.Empty();

	for (int32 index = 1; index <= 3; ++index)
	{
		FString itemStat = FString::Printf(TEXT("TXT_StatBeforeValue_%d"), index);
		UTESTextBlock* statValueText = Cast<UTESTextBlock>(GetWidgetFromName(*itemStat));
		TESCHECK_CONTINUE(nullptr != statValueText);

		_txt_statBeforeValue.Add(statValueText);
	}
}

void UTESUWEquipmentEnhanceMain::InitSelectedEquipmentStatAfterValueList()
{
	_txt_statAfterValue.Empty();

	for (int32 index = 1; index <= 3; ++index)
	{
		FString itemStat = FString::Printf(TEXT("TXT_StatAfterValue_%d"), index);
		UTESTextBlock* statValueText = Cast<UTESTextBlock>(GetWidgetFromName(*itemStat));
		TESCHECK_CONTINUE(nullptr != statValueText);

		_txt_statAfterValue.Add(statValueText);
	}

	_scb_statAfterValue = Cast<UScaleBox>(GetWidgetFromName(TEXT("SCB_StatAfterValue")));
	TESCHECK(nullptr != _scb_statAfterValue);
}

void UTESUWEquipmentEnhanceMain::InitSelectedEquipmentStatCompareValueList()
{
	_txt_StatCompareValue.Empty();

	for (int32 index = 1; index <= 3; ++index)
	{
		FString itemStat = FString::Printf(TEXT("TXT_StatAfterValueCompare_%d"), index);
		UTESTextBlock* statValueText = Cast<UTESTextBlock>(GetWidgetFromName(*itemStat));
		TESCHECK_CONTINUE(nullptr != statValueText);

		_txt_StatCompareValue.Add(statValueText);
	}

	_scb_statCompareValue = Cast<UScaleBox>(GetWidgetFromName(TEXT("SCB_StatAfterCompareValue")));
	TESCHECK(nullptr != _scb_statCompareValue);
}

void UTESUWEquipmentEnhanceMain::InitRootEquipName()
{
	_cp_rootEquipName = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Root_EquipmentName")));
	TESCHECK(nullptr != _cp_rootEquipName);
	_cp_rootEquipName->SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWEquipmentEnhanceMain::InitNotSelectedText()
{
	_cp_notSelectedText = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Root_NotSelectedText")));
	TESCHECK(nullptr != _cp_notSelectedText);
	_cp_notSelectedText->SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWEquipmentEnhanceMain::InitWarningText()
{
	_cp_warningMessage = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Root_Warning")));
	TESCHECK(nullptr != _cp_warningMessage);
	_cp_warningMessage->SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWEquipmentEnhanceMain::InitRootStatIncrease()
{
	_cp_statIncrease = Cast<UVerticalBox>(GetWidgetFromName(TEXT("Root_StatCompare")));
	TESCHECK(nullptr != _cp_statIncrease);
	_cp_statIncrease->SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWEquipmentEnhanceMain::InitRequiredMaterialValue()
{
	_txt_requiredMaterialValue = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_Cost")));
	TESCHECK(nullptr != _txt_requiredMaterialValue);
	_txt_requiredMaterialValue->SetText(FText::FromString(TEXT("")));
}

void UTESUWEquipmentEnhanceMain::InitStatArrow()
{
	_img_statArrowList.Empty();

	for (int32 index = 1; index <= 3; ++index)
	{
		FString arrowImg = FString::Printf(TEXT("IMG_StatArrow_%d"), index);
		UTESImage* img_statArrow = Cast<UTESImage>(GetWidgetFromName(*arrowImg));
		TESCHECK_CONTINUE(nullptr != img_statArrow);

		_img_statArrowList.Add(img_statArrow);
	}
}

void UTESUWEquipmentEnhanceMain::InitCommonHeader()
{
	_wbp_commonHeader = Cast<UTESUWCommonHeader>(GetWidgetFromName(TEXT("WBP_CommonHeader")));
	TESCHECK(nullptr != _wbp_commonHeader);
	_wbp_commonHeader->InitWidget();
	_wbp_commonHeader->CloseWidget();
}

void UTESUWEquipmentEnhanceMain::InitEnhanceButton()
{
	_cts_enhanceButton = Cast<UTESUWCommonToggleSwitcher>(GetWidgetFromName(TEXT("CTS_EnhanceSwitcher")));
	TESCHECK(nullptr != _cts_enhanceButton);
	_cts_enhanceButton->SetToggleState(true);
}

void UTESUWEquipmentEnhanceMain::InitSFTYDisplayer()
{
	_wbp_sftyDisplayerWidget = Cast<UTESUWGoodsDisplayer>(GetWidgetFromName(TEXT("WBP_SFTYDisplayer")));
	TESCHECK(nullptr != _wbp_sftyDisplayerWidget);
	_wbp_sftyDisplayerWidget->InitWidget();
	_wbp_sftyDisplayerWidget->OpenWidget();
	_wbp_sftyDisplayerWidget->SetWidget();
}

void UTESUWEquipmentEnhanceMain::InitManaringDisplayer()
{
	_wbp_manaringDisplayerWidget = Cast<UTESUWGoodsDisplayer>(GetWidgetFromName(TEXT("WBP_ManaringDisplayer")));
	TESCHECK(nullptr != _wbp_manaringDisplayerWidget);
	_wbp_manaringDisplayerWidget->InitWidget();
	_wbp_manaringDisplayerWidget->OpenWidget();
	_wbp_manaringDisplayerWidget->SetWidget();
}

void UTESUWEquipmentEnhanceMain::InitRootMaxTag()
{
	_cp_maxTag = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Root_MaxTag")));
	TESCHECK(nullptr != _cp_maxTag);
	_cp_maxTag->SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWEquipmentEnhanceMain::InitContentsInventoryView()
{
	InitContentsInventoryViewSwitcher();

	InitContentsInventoryViewContainer();
}

void UTESUWEquipmentEnhanceMain::InitContentsInventoryViewSwitcher()
{
	_ws_contentsInventoryViewSwitcher = Cast<UWidgetSwitcher>(GetWidgetFromName(TEXT("WS_ContentsInventoryView")));
	TESCHECK(nullptr != _ws_contentsInventoryViewSwitcher);
	_ws_contentsInventoryViewSwitcher->SetActiveWidgetIndex(0);
}

void UTESUWEquipmentEnhanceMain::InitContentsInventoryViewContainer()
{
	_tv_itemInventoryViewContainer = Cast<UTESTileView>(GetWidgetFromName(TEXT("TV_ItemInventoryViewContainer")));
	TESCHECK(nullptr != _tv_itemInventoryViewContainer);
	_tv_itemInventoryViewContainer->ClearListItems();
}

void UTESUWEquipmentEnhanceMain::InitItemFilter()
{
	InitItemFilterSwitcher();

	InitItemFilterButtonText();

	InitItemFilterSortingOrderButton();
}

void UTESUWEquipmentEnhanceMain::InitItemFilterSwitcher()
{
	_cts_itemFilterSwitcher = Cast<UTESUWCommonToggleSwitcher>(GetWidgetFromName(TEXT("CTS_ItemFilterSwitcher")));
	TESCHECK(nullptr != _cts_itemFilterSwitcher);
	_cts_itemFilterSwitcher->SetToggleState(false);
}

void UTESUWEquipmentEnhanceMain::InitItemFilterButtonText()
{
	//. [ON] 상태일때의 필터 버튼 텍스트
	UTESTextBlock* txt_filter_on = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_Filter_ON")));
	TESCHECK(nullptr != txt_filter_on);
	_txt_itemFilterTextMap.Add(true, txt_filter_on);

	//. [OFF] 상태일때의 필터 버튼 텍스트
	UTESTextBlock* txt_filter_off = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_Filter_OFF")));
	TESCHECK(nullptr != txt_filter_off);
	_txt_itemFilterTextMap.Add(false, txt_filter_off);
}

void UTESUWEquipmentEnhanceMain::InitItemFilterSortingOrderButton()
{
	//. [ON] 상태일때의 정렬 버튼
	UCheckBox* chkb_on = Cast<UCheckBox>(GetWidgetFromName(TEXT("CHKB_Sorter_ON")));
	TESCHECK(nullptr != chkb_on);
	_chkb_itemFilterSortingOrderMap.Add(true, chkb_on);

	//. [OFF] 상태일때의 정렬 버튼
	UCheckBox* chkb_off = Cast<UCheckBox>(GetWidgetFromName(TEXT("CHKB_Sorter_OFF")));
	TESCHECK(nullptr != chkb_off);
	_chkb_itemFilterSortingOrderMap.Add(false, chkb_off);
}

void UTESUWEquipmentEnhanceMain::ClearSFTYDisplayer()
{
	if (true == ::IsValid(_wbp_sftyDisplayerWidget))
	{
		_wbp_sftyDisplayerWidget->CloseWidget();
		_wbp_sftyDisplayerWidget->ClearWidget();
	}
}

void UTESUWEquipmentEnhanceMain::ClearManaringDisplayer()
{
	if (true == ::IsValid(_wbp_manaringDisplayerWidget))
	{
		_wbp_manaringDisplayerWidget->CloseWidget();
		_wbp_manaringDisplayerWidget->ClearWidget();
	}
}

void UTESUWEquipmentEnhanceMain::ClearCommonHeader()
{
	TESCHECK(nullptr != _wbp_commonHeader);
	_wbp_commonHeader->CloseWidget();
	_wbp_commonHeader->ClearWidget();
	_wbp_commonHeader = nullptr;
}

void UTESUWEquipmentEnhanceMain::SetWidget(UTESGDGoodsItem* itemData)
{
	if (false == SetData(itemData))
	{
		RefreshEquipment(false);

		SetContentsEquipInventory();

		SetEnhanceButtonDisable(false);
	}
	else
	{
		RefreshEquipment(true);

		SetContentsEquipInventory();
	}

	SetCommonHeader();

	SharedTutorialManager->ShowTutorialPopupAfterCheck(ETESTutorialContentsType::EquipmentEnhance);
}

bool UTESUWEquipmentEnhanceMain::SetData(UTESGDGoodsItem* itemData)
{
	if (false == ::IsValid(itemData))
	{
		//. 쵸피를 통해 왔을때는 빈 창을 띄워준다.
		_currentItemData = nullptr;

		//. 아이템 카테고리별 목록을 추출합니다.
		TArray<UTESGDGoodsItem*> haveItemList = SharedUserDataManager->FindItemDataAll();
		SetCategoryItemDataList(haveItemList);

		SetDataByFillter();

		return false;
	}

	_currentItemData = itemData;
	_currentItemUnique = itemData->GetItemUnique();
	_currentItemId = itemData->GetItemTableId();

	_selectedItemUnique = _currentItemUnique;

	//. 아이템 카테고리별 목록을 추출합니다.
	TArray<UTESGDGoodsItem*> haveItemList = SharedUserDataManager->FindItemDataAll();
	SetCategoryItemDataList(haveItemList);

	SetDataByFillter();

	return true;
}

void UTESUWEquipmentEnhanceMain::SetDataByFillter()
{
	TOptional<FTESFilterData> filterData = SharedSaveGameManager->GetFilterData(ETypeSortFilterContents::EnhanceGear);
	if (false == filterData.IsSet())
	{
		TESLOG(Error, TEXT("Invalid FilterData."));

		return;
	}

	_currentFilterData = filterData.GetValue();
}

void UTESUWEquipmentEnhanceMain::SetCategoryItemData(UTESGDGoodsItem* itemGoodsData)
{
	if (false == ::IsValid(itemGoodsData))
	{
		TESLOG(Error, TEXT("Invalid ItemData."));

		return;
	}

	for (TPair<ETESItemInventoryMainCategory, FTESEnhanceInventoryCategoryItemData> child : _categoryItemDataMap)
	{
		int64 itemUnique = itemGoodsData->GetUniqueID();
		if (true == child.Value._itemDataMap.Contains(itemUnique))
		{
			child.Value._itemDataMap[itemUnique] = itemGoodsData;
		}
	}
}

void UTESUWEquipmentEnhanceMain::SetCategoryItemDataList(TArray<class UTESGDGoodsItem*> itemDataList)
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

void UTESUWEquipmentEnhanceMain::AddCategoryItemData(ETESItemInventoryMainCategory categoryType, UTESGDGoodsItem* itemData)
{
	if (false == ::IsValid(itemData))
	{
		TESLOG(Error, TEXT("Invalid ItemData."));

		return;
	}

	if (true == _categoryItemDataMap.Contains(categoryType))
	{
		_categoryItemDataMap[categoryType]._itemDataMap.Add(itemData->GetItemUnique(), itemData);
	}
	else
	{
		FTESEnhanceInventoryCategoryItemData categoryData = {};

		categoryData._category = categoryType;

		categoryData._itemDataMap.Add(itemData->GetItemUnique(), itemData);

		_categoryItemDataMap.Add(categoryType, categoryData);
	}
}

void UTESUWEquipmentEnhanceMain::AddCategoryItemDataList(ETESItemInventoryMainCategory categoryType, TArray<class UTESGDGoodsItem*> itemDatas)
{
	for (UTESGDGoodsItem* itemData : itemDatas)
	{
		AddCategoryItemData(categoryType, itemData);
	}
}

void UTESUWEquipmentEnhanceMain::RemoveCategoryItemData(ETESItemInventoryMainCategory categoryType, UTESGDGoodsItem* itemData)
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

UTESGDGoodsItem* UTESUWEquipmentEnhanceMain::GetItemCategoryData(ETESItemInventoryMainCategory categoryType, int64 itemUnique)
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

TArray<class UTESGDGoodsItem*> UTESUWEquipmentEnhanceMain::GetItemCategoryDatas(ETESItemInventoryMainCategory categoryType)
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

void UTESUWEquipmentEnhanceMain::SetSelectedEquipmentIcon()
{
	TESCHECK(nullptr != _img_selectedEquipment);

	const FItemRow& itemTableId = SharedTDBManager->GetItemRow(_currentItemId);
	if (true == itemTableId.IsEmpty())
	{
		_img_selectedEquipment->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	
	_img_selectedEquipment->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	TESUtilTexture::SetDynamicGenericTexture(_img_selectedEquipment, itemTableId.IconPath2);
}

void UTESUWEquipmentEnhanceMain::SetSelectedEquipmentName()
{
	TESCHECK(nullptr != _txt_selectedEquipmentName);
	const FItemRow& itemTableId = SharedTDBManager->GetItemRow(_currentItemId);
	if (true == itemTableId.IsEmpty())
	{
		_txt_selectedEquipmentName->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	_txt_selectedEquipmentName->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	FString itemName = TESUtilTable::GetGlobalString(GetWorld(), itemTableId.NameForGlobalString);
	
	_txt_selectedEquipmentName->SetText(FText::FromString(*itemName));
}

void UTESUWEquipmentEnhanceMain::SetSelectedEquipmentGrade()
{
	TESCHECK(nullptr != _img_selectedEquipmentGrade);

	const FItemRow& itemTableId = SharedTDBManager->GetItemRow(_currentItemId);
	if (true == itemTableId.IsEmpty())
	{
		_img_selectedEquipmentGrade->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	_img_selectedEquipmentGrade->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	TESUtilTexture::SetDynamicTexture(_img_selectedEquipmentGrade, TESUtilPath::GetCharacterGradeImagePath(itemTableId.GradeType, ETESGradeTextureType::Left));
}

void UTESUWEquipmentEnhanceMain::SetSelectedEquipmentStatName()
{
	//. 레벨, 강화로 인한 상승되는 스텟, 전투력 순..
	const FItemRow& itemTableId = SharedTDBManager->GetItemRow(_currentItemId);
	if (true == itemTableId.IsEmpty())
	{
		return;
	}

	if (true == _txt_statNameList.IsValidIndex(0))
	{
		//. 레벨. 
		FString levelString = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Equip_Popup_Level);
		_txt_statNameList[0]->SetText(FText::FromString(*levelString));
	}

	if (true == _txt_statNameList.IsValidIndex(1))
	{
		//. 강화로 인한 상승되는 스텟
		FString statOptionName = TESUtilStatus::MakeStatOptionNameString(GetWorld(), itemTableId.EnhanceMainStatType);
		_txt_statNameList[1]->SetText(FText::FromString(*statOptionName));
	}

	if (true == _txt_statNameList.IsValidIndex(2))
	{
		//. 전투력
		FString battlePowerString = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Equip_Popup_BattlePower);
		_txt_statNameList[2]->SetText(FText::FromString(*battlePowerString));
	}
}

void UTESUWEquipmentEnhanceMain::SetSelectedEquipmentStatBeforeValue()
{
	if (false == ::IsValid(_currentItemData))
	{
		return;
	}

	if (true == _txt_statBeforeValue.IsValidIndex(0))
	{
		//. 아이템의 현재 레벨. 
		int32 currentItemLevel = _currentItemData->GetItemCurrentLevel();
		_txt_statBeforeValue[0]->SetText(FText::FromString(FString::FromInt(currentItemLevel)));
	}

	if (true == _txt_statBeforeValue.IsValidIndex(1))
	{
		//. 현재 메인 옵션의 수치
		int32 currentMainOptionValue =	_currentItemData->GetItemOptionValue(0);
		_txt_statBeforeValue[1]->SetText(FText::FromString(FString::FromInt(currentMainOptionValue)));
	}

	if (true == _txt_statBeforeValue.IsValidIndex(2))
	{
		//. 전투력
		int32 currentBattlePower = _currentItemData->GetBattlePower();
		_txt_statBeforeValue[2]->SetText(FText::FromString(FString::FromInt(currentBattlePower)));
	}
}

void UTESUWEquipmentEnhanceMain::SetSelectedEquipmentStatAfterValue()
{
	if (false == ::IsValid(_currentItemData))
		return;

	//. 해당 아이템의 다음 레벨을 비교하기 위한 더미 데이터를 만들어줍니다.
	UTESGDGoodsItem* dummyItemData = NewObject<UTESGDGoodsItem>();
	if (false == ::IsValid(dummyItemData))
	{
		TESLOG(Error, TEXT("Invalid dummyItemData."));

		return;
	}

	dummyItemData->SetItemDataDummy(_currentItemData->GetItemTableId(), _currentItemData->GetItemCount());
	dummyItemData->SetItemLevel(_currentItemData->GetItemCurrentLevel() + 1);

	TArray<FTESItemOption> dummyItemOptionList = {};
	dummyItemOptionList = _currentItemData->GetItemOptionDatas();

	const FItemRow& itemTableId = SharedTDBManager->GetItemRow(_currentItemId);
	if (true == itemTableId.IsEmpty())
	{
		TESLOG(Error, TEXT("item Table Id is Invalid"));
		return;
	}

	TArray<int32> increasingStatValueList = itemTableId.EnhanceMainStatValue;

	//. 강화 수치 인덱스(2레벨부터 시작이지만 1레벨 만큼 위의 수치를 보여줘야 해서 -1를 해준다.)
	int32 increaseIndex = _currentItemData->GetItemCurrentLevel() - 1;

	if (true == increasingStatValueList.IsValidIndex(increaseIndex) && true == increasingStatValueList.IsValidIndex(increaseIndex - 1))
	{
		int32 increasedMainStatValue = increasingStatValueList[increaseIndex] - increasingStatValueList[increaseIndex - 1];

		dummyItemOptionList[0]._value += increasedMainStatValue;
	}
	//. 1레벨->2레벨로 갈때
	else if (0 == increaseIndex)
	{
		dummyItemOptionList[0]._value += increasingStatValueList[0];
	}

	dummyItemData->SetItemOptions(&dummyItemOptionList[0]);

	int32 maximumLevel = _currentItemData->GetItemMaximumLevel();

	if (true == _txt_statAfterValue.IsValidIndex(0))
	{
		//. 아이템의 강화 후 레벨. 강화 후에는 무조건 1레벨씩만 증가한다.
		TESCHECK(nullptr != _scb_statAfterValue);
		int32 currentItemLevel = _currentItemData->GetItemCurrentLevel();
		int32 afterItemLevel = dummyItemData->GetItemCurrentLevel();
		_txt_statAfterValue[0]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		_txt_statAfterValue[0]->SetText(FText::FromString(FString::FromInt(afterItemLevel)));
		_scb_statAfterValue->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		//. 현재 레벨이 맥시멈 레벨이면 Collapsed 시켜준다
		if (currentItemLevel == maximumLevel)
		{
			_txt_statAfterValue[0]->SetVisibility(ESlateVisibility::Collapsed);
			_scb_statAfterValue->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (true == _txt_statAfterValue.IsValidIndex(1))
	{
		//. 현재 메인 옵션의 수치
		int32 currentMainOptionValue = _currentItemData->GetItemOptionValue(0);

		//. 강화 성공 했을시 증가량분
		int32 afterMainOptionValue = 0;
		if (true == increasingStatValueList.IsValidIndex(increaseIndex) && true == increasingStatValueList.IsValidIndex(increaseIndex - 1))
		{
			//. 다음 강화 수치에서 증가량분만 보여준다.
			afterMainOptionValue = increasingStatValueList[increaseIndex] - increasingStatValueList[increaseIndex - 1];
			_txt_statAfterValue[1]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			_txt_statAfterValue[1]->SetText(FText::FromString(FString::FromInt(currentMainOptionValue + afterMainOptionValue)));
			_scb_statAfterValue->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		//. 1레벨->2레벨로 갈때
		else if (0 == increaseIndex)
		{
			afterMainOptionValue = increasingStatValueList[0];
			_txt_statAfterValue[1]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			_txt_statAfterValue[1]->SetText(FText::FromString(FString::FromInt(currentMainOptionValue + afterMainOptionValue)));
			
		}
		//. 아이템 레벨이 맥시멈이면 Collapsed 해준다.
		if (maximumLevel == increaseIndex + 1)
		{
			_txt_statAfterValue[1]->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (true == _txt_statAfterValue.IsValidIndex(2))
	{
		//. 전투력
		int32 afterBattlePower = dummyItemData->GetBattlePower();
		_txt_statAfterValue[2]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		_txt_statAfterValue[2]->SetText(FText::FromString(FString::FromInt(afterBattlePower)));

		//. 아이템 레벨이 맥시멈이면 Collapsed 해준다.
		if (maximumLevel == increaseIndex + 1)
		{
			_txt_statAfterValue[2]->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UTESUWEquipmentEnhanceMain::SetSelectedEquipmentStatCompareValue()
{
	if (false == ::IsValid(_currentItemData))
		return;

	const FItemRow& itemTableId = SharedTDBManager->GetItemRow(_currentItemId);
	if (true == itemTableId.IsEmpty())
	{
		TESLOG(Error, TEXT("item Table Id is Invalid"));
		return;
	}

	TArray<int32> increasingStatValueList = itemTableId.EnhanceMainStatValue;

	int32 maximumLevel = _currentItemData->GetItemMaximumLevel();

	if (true == _txt_StatCompareValue.IsValidIndex(0))
	{
		//. 강화 후 아이템 레벨은 +1.
		int32 compareLevelValue = 1;
		FString formatString = TESUtilString::MakeFormatString(TEXT("(+{0})"), compareLevelValue);

		if (true == ::IsValid(_txt_StatCompareValue[0]))
		{
			_txt_StatCompareValue[0]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			_txt_StatCompareValue[0]->SetText(FText::FromString(formatString));
		}

		if (true == ::IsValid(_scb_statCompareValue))
		{
			_scb_statCompareValue->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}

		if (true == _img_statArrowList.IsValidIndex(0))
		{
			if (true == ::IsValid(_img_statArrowList[0]))
			{
				_img_statArrowList[0]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
		}

		int32 currentItemLevel = _currentItemData->GetItemCurrentLevel();

		//. 현재 레벨이 맥시멈 레벨이면 Collapsed 시킨다.
		if (currentItemLevel == maximumLevel)
		{
			if (true == ::IsValid(_txt_StatCompareValue[0])) 
			{
				_txt_StatCompareValue[0]->SetVisibility(ESlateVisibility::Collapsed);
			}

			if (true == _img_statArrowList.IsValidIndex(0))
			{
				_img_statArrowList[0]->SetVisibility(ESlateVisibility::Collapsed);
			}

			if (true == ::IsValid(_scb_statCompareValue)) 
			{
				_scb_statCompareValue->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}

	if (true == _txt_StatCompareValue.IsValidIndex(1))
	{
		int32 increaseIndex = _currentItemData->GetItemCurrentLevel() - 1;
		int32 afterMainOptionValue = 0;
		if (true == increasingStatValueList.IsValidIndex(increaseIndex) && true == increasingStatValueList.IsValidIndex(increaseIndex - 1))
		{
			afterMainOptionValue = increasingStatValueList[increaseIndex] - increasingStatValueList[increaseIndex - 1];
			FString formatString = TESUtilString::MakeFormatString(TEXT("(+{0})"), afterMainOptionValue);
			if (true == ::IsValid(_txt_StatCompareValue[1]))
			{
				_txt_StatCompareValue[1]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				_txt_StatCompareValue[1]->SetText(FText::FromString(formatString));
			}
		}
		else if (0 == increaseIndex)
		{
			afterMainOptionValue = increasingStatValueList[0];
			FString formatString = TESUtilString::MakeFormatString(TEXT("(+{0})"), afterMainOptionValue);
			if (true == ::IsValid(_txt_StatCompareValue[1]))
			{
				_txt_StatCompareValue[1]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				_txt_StatCompareValue[1]->SetText(FText::FromString(formatString));
			}
		}

		if (true == _img_statArrowList.IsValidIndex(1))
		{
			if (true == ::IsValid(_img_statArrowList[1]))
			{
				_img_statArrowList[1]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
		}

		//. 아이템 레벨이 맥시멈이면 Collapsed 해준다.
		if (maximumLevel == increaseIndex + 1)
		{
			if (true == ::IsValid(_txt_StatCompareValue[1]))
			{
				_txt_StatCompareValue[1]->SetVisibility(ESlateVisibility::Collapsed);
			}
			
			if (true == _img_statArrowList.IsValidIndex(1))
			{
				if (true == ::IsValid(_img_statArrowList[1])) 
				{
					_img_statArrowList[1]->SetVisibility(ESlateVisibility::Collapsed);
				}
			}
		}
	}

	if (true == _txt_StatCompareValue.IsValidIndex(2))
	{
		//. 해당 아이템의 다음 레벨을 비교하기 위한 더미 데이터를 만들어줍니다.
		UTESGDGoodsItem* dummyItemData = NewObject<UTESGDGoodsItem>();
		if (false == ::IsValid(dummyItemData))
		{
			TESLOG(Error, TEXT("Invalid _currentItemData."));

			return;
		}

		dummyItemData->SetItemDataDummy(_currentItemData->GetItemTableId(), _currentItemData->GetItemCount());
		dummyItemData->SetItemLevel(_currentItemData->GetItemCurrentLevel() + 1);

		TArray<FTESItemOption> dummyItemOptionList = {};
		dummyItemOptionList = _currentItemData->GetItemOptionDatas();

		//. 강화 수치 인덱스(2레벨부터 시작이지만 1레벨 만큼 위의 수치를 보여줘야 해서 -1를 해준다.)
		int32 increaseIndex = _currentItemData->GetItemCurrentLevel() - 1;

		if (true == increasingStatValueList.IsValidIndex(increaseIndex) && true == increasingStatValueList.IsValidIndex(increaseIndex - 1))
		{
			int32 increasedMainStatValue = increasingStatValueList[increaseIndex] - increasingStatValueList[increaseIndex - 1];

			if (true == dummyItemOptionList.IsValidIndex(0))
			{
				dummyItemOptionList[0]._value += increasedMainStatValue;
			}
		}
		//. 1레벨->2레벨로 갈때
		else if (0 == increaseIndex)
		{
			if (true == dummyItemOptionList.IsValidIndex(0)) 
			{
				dummyItemOptionList[0]._value += increasingStatValueList[0];
			}
		}

		dummyItemData->SetItemOptions(&dummyItemOptionList[0]);


		//. 전투력
		int32 beforeBattlePower = _currentItemData->GetBattlePower();
		int32 afterBattlePower = dummyItemData->GetBattlePower();
		int32 increasedBattlePower = afterBattlePower - beforeBattlePower;

		FString formatString = TESUtilString::MakeFormatString(TEXT("(+{0})"), increasedBattlePower);

		if (true == _txt_StatCompareValue.IsValidIndex(2))
		{
			if (true == ::IsValid(_txt_StatCompareValue[2]))
			{
				_txt_StatCompareValue[2]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				_txt_StatCompareValue[2]->SetText(FText::FromString(formatString));
			}
		}

		if (true == _img_statArrowList.IsValidIndex(2))
		{
			if (true == ::IsValid(_img_statArrowList[2]))
			{
				_img_statArrowList[2]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
		}
		//. 맥시멈 레벨이면 Collapsed 시켜준다.
		if (increaseIndex == increasingStatValueList.Num())
		{
			if (true == ::IsValid(_txt_StatCompareValue[2]))
			{
				_txt_StatCompareValue[2]->SetVisibility(ESlateVisibility::Collapsed);
			}

			if (true == _img_statArrowList.IsValidIndex(2))
			{
				if (true == ::IsValid(_img_statArrowList[2]))
				{
					_img_statArrowList[2]->SetVisibility(ESlateVisibility::Collapsed);
				}
			}
		}
	}
}

void UTESUWEquipmentEnhanceMain::SetEquipNameVisible(bool isVisible)
{
	TESCHECK(nullptr != _cp_rootEquipName);
	_cp_rootEquipName->SetVisibility(true == isVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UTESUWEquipmentEnhanceMain::SetNotSelectedTextVisible(bool isVisible)
{
	TESCHECK(nullptr != _cp_notSelectedText);
	_cp_notSelectedText->SetVisibility(true == isVisible ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
}

void UTESUWEquipmentEnhanceMain::SetWarningText(int32 tableId, int32 levelIndex)
{
	TESCHECK(nullptr != _cp_warningMessage);

	const FConstantStringIntListRow& enhancementSuccessRateList = SharedTDBManager->GetConstantStringIntListRow(tableId);
	TArray<int32> successRateList = enhancementSuccessRateList.Value;

	if (true == successRateList.IsValidIndex(levelIndex))
	{
		//. 성공확률이 100%라면 문구를 가려준다.
		if (100 == successRateList[levelIndex])
		{
			_cp_warningMessage->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			_cp_warningMessage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
	else
	{
		_cp_warningMessage->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UTESUWEquipmentEnhanceMain::SetRootStatIncreaseVisible(bool isVisible)
{
	TESCHECK(nullptr != _cp_statIncrease);
	_cp_statIncrease->SetVisibility(true == isVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UTESUWEquipmentEnhanceMain::SetRequiredMaterialValue()
{
	TESCHECK(nullptr != _txt_requiredMaterialValue);

	const FItemRow& itemTableId = SharedTDBManager->GetItemRow(_currentItemId);
	if (true == itemTableId.IsEmpty())
	{
		return;
	}

	TESCHECK(nullptr != _currentItemData);
	int32 currentIndex = _currentItemData->GetItemCurrentLevel() - 1;

	switch (itemTableId.GradeType)
	{
	case ETypeGrade::C:
		{
			int32 tableId = static_cast<int32>(EConstantStringIntListName::CGradeEquipEnhanceCost);
			SetRequiredMaterialValue(tableId, currentIndex);

			tableId = static_cast<int32>(EConstantStringIntListName::CGradeEnhanceSuccessRate);
			SetWarningText(tableId, currentIndex);
		}
		break;
	case ETypeGrade::UC:
		{
			int32 tableId = static_cast<int32>(EConstantStringIntListName::UCGradeEquipEnhanceCost);
			SetRequiredMaterialValue(tableId, currentIndex);

			tableId = static_cast<int32>(EConstantStringIntListName::UCGradeEnhanceSuccessRate);
			SetWarningText(tableId, currentIndex);
		}
		break;
	case ETypeGrade::R:
		{
			int32 tableId = static_cast<int32>(EConstantStringIntListName::RGradeEquipEnhanceCost);
			SetRequiredMaterialValue(tableId, currentIndex);

			tableId = static_cast<int32>(EConstantStringIntListName::RGradeEnhanceSuccessRate);
			SetWarningText(tableId, currentIndex);
		}
		break;
	case ETypeGrade::SR:
		{
			int32 tableId = static_cast<int32>(EConstantStringIntListName::SGradeREquipEnhanceCost);
			SetRequiredMaterialValue(tableId, currentIndex);

			tableId = static_cast<uint32>(EConstantStringIntListName::SRGradeEnhanceSuccessRate);
			SetWarningText(tableId, currentIndex);
		}
		break;
	case ETypeGrade::SSR:
		{
			int32 tableId = static_cast<int32>(EConstantStringIntListName::SSRGradeEquipEnhanceCost);
			SetRequiredMaterialValue(tableId, currentIndex);

			tableId = static_cast<int32>(EConstantStringIntListName::SSRGradeEnhanceSuccessRate);
			SetWarningText(tableId, currentIndex);
		}
		break;
	default:
		{
			TESLOG(Error, TEXT("Equipment Grade Type is invalid"));
		}
		break;
	}
}

void UTESUWEquipmentEnhanceMain::SetRequiredMaterialValue(int32 tableId, int32 levelIndex)
{
	const FConstantStringIntListRow& enhancementCostList = SharedTDBManager->GetConstantStringIntListRow(tableId);

	if (true == enhancementCostList.IsEmpty())
	{
		TESLOG(Error, TEXT("item enhancementCostList Id is Invalid"));
		return;
	}

	TArray<int32> costList = enhancementCostList.Value;

	if (true == costList.IsValidIndex(levelIndex))
	{
		_requiredMaterialValue = costList[levelIndex];
		_txt_requiredMaterialValue->SetText(FText::FromString(FString::FromInt(costList[levelIndex])));
	}
	else
	{
		_requiredMaterialValue = 0;
		_txt_requiredMaterialValue->SetText(FText::FromString(FString::FromInt(0)));
	}
}

void UTESUWEquipmentEnhanceMain::SetEnhanceButtonDisable(bool isEnabled)
{
	//. 장비 레벨이 맥시멈이면 강화 버튼 비활성화.
	if (true == ::IsValid(_cts_enhanceButton))
	{
		_cts_enhanceButton->SetToggleState(isEnabled);
	}
}

void UTESUWEquipmentEnhanceMain::SetMaxTagVisible()
{
	TESCHECK(nullptr != _currentItemData);
	bool isMaxLevel = false;
	if (_currentItemData->GetItemMaximumLevel() == _currentItemData->GetItemCurrentLevel() ||
		_currentItemData->GetItemMaximumLevel() == _currentItemData->GetItemCurrentLevel() + 1)
	{
		//. 아이템이 최대 레벨이거나 최대 레벨 바로 전이면 MAX태그가 떠야한다.
		isMaxLevel = true;
	}
		
	TESCHECK(nullptr != _cp_maxTag);
	_cp_maxTag->SetVisibility(true == isMaxLevel ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UTESUWEquipmentEnhanceMain::SetCommonHeader()
{
	TESCHECK(nullptr != _wbp_commonHeader);

	_wbp_commonHeader->OpenWidget();
    _wbp_commonHeader->SetWidget();

	_wbp_commonHeader->ApplyCenterDisplayersByControlType(ETESCommonHeaderCenterDisplayControlType::Custom);
	_wbp_commonHeader->ApplyRightDisplayersByControlType(ETESCommonHeaderRightDisplayControlType::Custom);

	FTESGoodsDisplayerParameters rightGoodsDisplayerParameters;
	rightGoodsDisplayerParameters._goodsDisplayType = ETESGoodsDisplayType::Manaring;
	_wbp_commonHeader->SetRightGoodsDisplayer(ETESCommonHeaderRightGoodsIndex::Goods1, rightGoodsDisplayerParameters);

	//_wbp_commonHeader->ApplyCenterDisplayersByDefault();
}

void UTESUWEquipmentEnhanceMain::SetContentsEquipInventory()
{
	//. 필터에 관련된 필터 버튼과 정렬 버튼 등을 세팅합니다.
	SetItemFilterContents(ETESItemInventoryMainCategory::Equip);
	
	SetContentsInventoryView(GetItemCategoryDatas(ETESItemInventoryMainCategory::Equip));
}

void UTESUWEquipmentEnhanceMain::SetContentsInventoryView(TArray<class UTESGDGoodsItem*> itemDataList, bool bUseScrollToTop/* = true*/)
{
	//. 아이템 리스트가 존재하다면 필터 후 정렬하고, 출력합니다.
	//TArray<UTESGDGoodsItem*> itemDataLists = MakeIgnoreItemDataListByUsingCharacter(itemDataList, );

	if (itemDataList.Num() > 0)
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

			//. 인벤토리 뷰로 교체합니다.
			SetContentsInventoryViewSwitcher(false);

			//. 아이템을 출력합니다.
			SetContentsInventoryViewContainer(sortedItemDataList, bUseScrollToTop);
		}
		//. 필터링된 데이터가 없을 경우, 보유한 아이템이 없다는 메세지를 출력해줍니다.
		else
		{
			//. 비어있는 텍스트 문구로 교체합니다.
			SetContentsInventoryViewSwitcher(true);
		}
	}
	//. 아이템 리스트가 존재하지 않는다면, 보유한 아이템이 없다는 메세지를 출력해줍니다.
	else
	{
		//. 비어있는 텍스트 문구로 교체합니다.
		SetContentsInventoryViewSwitcher(true);
	}
}

void UTESUWEquipmentEnhanceMain::SetContentsInventoryViewSwitcher(bool isEmptied)
{
	if (true == ::IsValid(_ws_contentsInventoryViewSwitcher))
	{
		_ws_contentsInventoryViewSwitcher->SetActiveWidgetIndex(isEmptied ? 0 : 1);
	}
}

void UTESUWEquipmentEnhanceMain::SetContentsInventoryViewContainer(TArray<UTESGDGoodsItem*> itemDataList, bool bUseScrollToTop)
{
	if (false == ::IsValid(_tv_itemInventoryViewContainer))
	{
		return;
	}

	//. 모든 내용을 비웁니다.
	_tv_itemInventoryViewContainer->ClearListItems();

	//. 보여진 데이터들에 대한 것도 모두 비운 후 갱신합니다.
	_currentInventoryItemSlotDatas.Empty();

	//. 그리고, 다시 채웁니다 :)
	int32 maxCount = itemDataList.Num();
	for (int32 index = 0; index < maxCount; ++index)
	{
		UTESEDCharInfoContentsItemSlot* itemSlotData = NewObject<UTESEDCharInfoContentsItemSlot>();
		if (false == ::IsValid(itemSlotData))
		{
			TESLOG(Error, TEXT("Invalid ItemSlotData."));

			continue;
		}

		//. 아이템 데이터입니다.
		UTESGDGoodsItem* itemData = itemDataList[index];

		itemSlotData->SetItemData(itemData);

		itemSlotData->SetItemSelected(itemData->GetItemUnique() == _selectedItemUnique);

		itemSlotData->SetItemUsingCharVisibled(true);

		itemSlotData->SetItemStatusVisibled(true);

		_currentInventoryItemSlotDatas.Add(itemData->GetItemUnique(), itemSlotData);

		_tv_itemInventoryViewContainer->AddItem(itemSlotData);
	}

	//. 타일뷰의 내용이 새로 갱신되었으므로, 스크롤을 처음으로 둘지, 여부를 세팅합니다.
	if (true == bUseScrollToTop)
	{
		_tv_itemInventoryViewContainer->ScrollToTop();
	}

		/*UTESEDItemInventorySlot* inventorySlotEntryData = NewObject<UTESEDItemInventorySlot>();
		if (false == ::IsValid(inventorySlotEntryData))
		{
			TESLOG(Error, TEXT("Invalid InventorySlotEntryData."));

			continue;
		}*/

	//	//. 아이템 데이터입니다.
	//	UTESGDGoodsItem* itemGoodsData = itemDataList[index];

	//	//. 아이템 데이터를 아이템 슬롯 위젯에 세팅합니다.
	//	inventorySlotEntryData->SetGoodsData(itemGoodsData);

	//	//. 아이템 슬롯의 비활성화 여부를 세팅합니다.
	//	switch (_currentDetailViewType)
	//	{
	//	case ETESEquipmentInventoryView::BulkSale:
	//		{
	//			//. ─────────────────────────────────────────────────
	//			//. 다음의 경우, 일괄 판매중일때에는 비활성화 처리합니다.
	//			//. ─────────────────────────────────────────────────
	//			//. 1. 잠긴 아이템일 경우
	//			//. 2. 사용중인 아이템일 경우
	//			//. 3. 판매하는 아이템이 아닐 경우
	//			//. ─────────────────────────────────────────────────
	//			inventorySlotEntryData->SetItemDisabled(true == itemGoodsData->IsLocked() ||
	//				true == itemGoodsData->IsUsing() ||
	//				false == itemGoodsData->IsSellItem());
	//		}
	//		break;

	//	default:
	//		{
	//			//. 일괄 판매중이 아닐 경우에는 그냥 비활성화 시키지 않습니다.
	//			inventorySlotEntryData->SetItemDisabled(false);
	//		}
	//		break;
	//	}

	//	//. 타일뷰에 아이템 슬롯을 추가합니다.
	//	_tv_itemInventoryViewContainer->AddItem(inventorySlotEntryData);
	//}

	////. 타일뷰의 내용이 새로 갱신되었으므로, 스크롤을 처음으로 둘지, 여부를 세팅합니다.
	//if (true == bUseScrollToTop)
	//{
	//	_tv_itemInventoryViewContainer->ScrollToTop();
	//}
}

void UTESUWEquipmentEnhanceMain::SetItemFilterContents(ETESItemInventoryMainCategory categoryType)
{
	/*ETypeSortFilterContents filterContents = ETypeSortFilterContents::None;
	switch (categoryType)
	{
	case ETESItemInventoryMainCategory::Equip:		filterContents = ETypeSortFilterContents::Equip;	break;
	case ETESItemInventoryMainCategory::Rune:		filterContents = ETypeSortFilterContents::Rune;		break;
	case ETESItemInventoryMainCategory::Misc:		filterContents = ETypeSortFilterContents::Misc;		break;
	default:
		{
			FString categoryTypeString = TESUtilString::GetEnumAsString<ETESItemInventoryMainCategory>(TEXT("ETESItemInventoryMainCategory"), categoryType);
			TESLOG(Error, TEXT("Not Supported Category : %s"), *categoryTypeString);
			return;
		}
		break;
	}

	TOptional<FTESFilterData> filterData = SharedSaveGameManager->GetFilterData(filterContents);
	if (false == filterData.IsSet())
	{
		TESLOG(Error, TEXT("Invalid FilterData."));

		return;
	}*/

	//. 해당 필터의 데이터로 필터 버튼 컨텐츠들을 세팅합니다.
	SetContentsInventoryFilter(_currentFilterData);
}

void UTESUWEquipmentEnhanceMain::SetContentsInventoryFilter(FTESFilterData filterData)
{
	//. 아이템 인벤토리에서 사용하는 필터 데이터도 갱신해줍니다.
	_currentFilterData = filterData;

	//. 현재의 필터여부에 따라 필터 버튼을 교체해줍니다.
	SetContentsInventoryFilterSwitcher(_currentFilterData._isFiltering);

	//. 현재의 필터 텍스트를 세팅합니다.
	SetContentsInventoryFilterButtonText(_currentFilterData);

	//. 현재의 정렬 순서로 정렬 박스를 출력해줍니다.
	SetContentsInventoryFilterSortingOrderButton(_currentFilterData._isFiltering, _currentFilterData._sortingOrder);
}

void UTESUWEquipmentEnhanceMain::SetContentsInventoryFilterSwitcher(bool isOn)
{
	if (true == ::IsValid(_cts_itemFilterSwitcher))
	{
		_cts_itemFilterSwitcher->SetToggleState(isOn);
	}
}

void UTESUWEquipmentEnhanceMain::SetContentsInventoryFilterButtonText(FTESFilterData filterData)
{
	const FSortFilterElementRow& filterElementTable = SharedTDBManager->GetSortFilterElementRow(filterData._sortingElementId);
	if (true == filterElementTable.IsEmpty())
	{
		SetContentsInventoryFilterButtonText(filterData._isFiltering, TESConstantValue::NoneValue);
	}
	else
	{
		SetContentsInventoryFilterButtonText(filterData._isFiltering, filterElementTable.SortFilterNameID);
	}
}

void UTESUWEquipmentEnhanceMain::SetContentsInventoryFilterButtonText(bool isOn, int32 buttonTextId)
{
	if (true == _txt_itemFilterTextMap.Contains(isOn))
	{
		FString str = TESUtilTable::GetGlobalString(GetWorld(), buttonTextId);
		_txt_itemFilterTextMap[isOn]->SetText(FText::FromString(*str));
	}
}

void UTESUWEquipmentEnhanceMain::SetContentsInventoryFilterSortingOrderButton(FTESFilterData filterData)
{
	SetContentsInventoryFilterSortingOrderButton(filterData._isFiltering, filterData._sortingOrder);
}

void UTESUWEquipmentEnhanceMain::SetContentsInventoryFilterSortingOrderButton(bool isOn, ETESFilterSortingOrder sortingOrder)
{
	if (true == _chkb_itemFilterSortingOrderMap.Contains(isOn))
	{
		//. Unchecked : 내림차순, checked : 오름차순
		_chkb_itemFilterSortingOrderMap[isOn]->SetCheckedState(sortingOrder == ETESFilterSortingOrder::Descending ?
																ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	}
}

void UTESUWEquipmentEnhanceMain::SetOpeningMessageBox(bool isOpening)
{
	_isOpeningMessageBox = isOpening;
}

void UTESUWEquipmentEnhanceMain::RefreshEquipmentInfo(UTESGDGoodsItem* itemData)
{
	_currentItemData = itemData;
	_currentItemUnique = itemData->GetItemUnique();
	_currentItemId = itemData->GetItemTableId();

	//. 아이템 카테고리별 목록을 추출합니다.
	TArray<UTESGDGoodsItem*> haveItemList = SharedUserDataManager->FindItemDataAll();
	SetCategoryItemDataList(haveItemList);

	RefreshEquipment(nullptr != itemData);
}

void UTESUWEquipmentEnhanceMain::RefreshEquipment(bool haveItem)
{
	SetSelectedEquipmentIcon();

	SetSelectedEquipmentName();

	SetSelectedEquipmentGrade();

	SetSelectedEquipmentStatName();

	SetSelectedEquipmentStatBeforeValue();

	SetSelectedEquipmentStatAfterValue();

	SetSelectedEquipmentStatCompareValue();

	SetRequiredMaterialValue();

	SetNotSelectedTextVisible(haveItem);

	SetRootStatIncreaseVisible(haveItem);

	SetEquipNameVisible(haveItem);

	SetEnhanceButtonDisable(!CheckIfEquipmentMaxLevel());

	SetMaxTagVisible();
}

bool UTESUWEquipmentEnhanceMain::CheckIfEquipmentMaxLevel()
{
	//. 비워져 있는 상태라면 가려준다.
	if (false == ::IsValid(_currentItemData))
	{
		return false;
	}

	//. 현재 레벨이 장비 최대 레벨인지.
	if (_currentItemData->GetItemCurrentLevel() >= _currentItemData->GetItemMaximumLevel())
	{
		return true;
	}

	return false;
}

void UTESUWEquipmentEnhanceMain::PlayHideAnimation()
{
	PlayWGAnimation(TEXT("Ani_VanishStats"));
}

void UTESUWEquipmentEnhanceMain::PlayShowAnimation()
{
	PlayWGAnimation(TEXT("Ani_ShowStats"));
}

TArray<class UTESGDGoodsItem*> UTESUWEquipmentEnhanceMain::MakeFilteredItemDataList(TArray<class UTESGDGoodsItem*> itemDataList, TMap<int32, FTESFilterFilteringGroupData> filteringData, bool isFiltering)
{
	return TESUtilFilter::MakeFilteredItemDataList(itemDataList, filteringData, isFiltering);
}

TArray<class UTESGDGoodsItem*> UTESUWEquipmentEnhanceMain::MakeSortedItemDataList(TArray<class UTESGDGoodsItem*> itemDataList, int32 sortingElementId, ETESFilterSortingOrder sortingOrder)
{
	return TESUtilFilter::MakeSortedItemDataList(itemDataList, sortingElementId, sortingOrder);
}

TArray<class UTESGDGoodsItem*> UTESUWEquipmentEnhanceMain::MakeIgnoreItemDataListByUsingCharacter(TArray<class UTESGDGoodsItem*> itemDataList, int64 ignoreItemUsingCharacterUnique)
{
	TArray<UTESGDGoodsItem*> itemDatas = itemDataList;
	for (UTESGDGoodsItem* itemData : itemDatas)
	{
		if (false == ::IsValid(itemData))
		{
			continue;
		}

		if (ignoreItemUsingCharacterUnique == itemData->GetItemUsingCharacterUnique())
		{
			itemDatas.Remove(itemData);

			break;
		}
	}

	return itemDatas;
}

void UTESUWEquipmentEnhanceMain::OnEvent_InventoryView_EntryInitailized(UObject* item, UUserWidget* widget)
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

void UTESUWEquipmentEnhanceMain::OnEvent_InventoryView_EntryReleased(UUserWidget* widget)
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

void UTESUWEquipmentEnhanceMain::OnEvent_InventoryView_EntryClicked(UObject* item)
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

	UTESEDCharInfoContentsItemSlot* selectedSlotData = Cast<UTESEDCharInfoContentsItemSlot>(item);
	if (false == ::IsValid(selectedSlotData))
	{
		return;
	}

	UTESGDGoodsItem* selectedItemData = selectedSlotData->GetItemData();
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

	//. 선택된 인덱스를 갱신하여줍니다.
	_selectedItemUnique = itemUnique;
	
	//. 해당 아이템에 맞춰서 세팅을 다시 해줍니다.
	RefreshEquipmentInfo(selectedItemData);
}

void UTESUWEquipmentEnhanceMain::OnEvent_OpenItemFilter()
{
	////. 위젯을 열고 있는 중이라면 무시합니다.
	//if (true == _isOpeningWidget)
	//{
	//	return;
	//}

	////. 위젯을 닫고 있는 중이라면 아이템 필터 위젯을 열지 않습니다.
	//if (true == _isClosingWidget)
	//{
	//	return;
	//}

	/*ETypeSortFilterContents filterContentsType = ETypeSortFilterContents::None;
	switch (_currentMainCategory)
	{
	case ETESEquipmentInventoryView::Equip:		filterContentsType = ETypeSortFilterContents::Equip; break;
	case ETESEquipmentInventoryView::Rune:		filterContentsType = ETypeSortFilterContents::Rune;	break;
	case ETESEquipmentInventoryView::Misc:		filterContentsType = ETypeSortFilterContents::Misc;	break;
	default:
		{
			TESLOG(Error, TEXT("Not Supported Category : %d"), _currentMainCategory);

			return;
		}
		break;
	}*/

	UTESUWFilter* filterWidget = SharedPopupManager->CreatePopupWidget<UTESUWFilter>(TEXT("WBP_Filter"), TEXT("Common/WBP/Filter"));
	if (false == ::IsValid(filterWidget))
	{
		TESLOG(Error, TEXT("Invalid FilterWidget."));

		return;
	}

	filterWidget->InitWidget();
	filterWidget->OpenWidget();
	filterWidget->SetWidget(ETypeSortFilterContents::EnhanceGear);
}

void UTESUWEquipmentEnhanceMain::OnEvent_ApplyEquipFilter(struct FTESFilterData filterData)
{
	_currentFilterData = filterData;

	SetContentsInventoryFilter(_currentFilterData);

	SetContentsInventoryView(GetItemCategoryDatas(ETESItemInventoryMainCategory::Equip));
}

void UTESUWEquipmentEnhanceMain::OnEvent_EnhanceItem(bool enhanceResult, FTESEnhanceItemInfo itemInfo)
{
	//. 좌측 장비 UI 갱신
	RefreshEquipment(true);

	//. 우측 아이템 리스트 갱신
	SetContentsInventoryFilter(_currentFilterData);

	SetContentsInventoryView(GetItemCategoryDatas(ETESItemInventoryMainCategory::Equip));
}

void UTESUWEquipmentEnhanceMain::OnEvent_ChangeSotingOrder()
{
	////. 위젯을 열고 있는 중이라면 무시합니다.
	//if (true == _isOpeningWidget)
	//{
	//	return;
	//}

	////. 위젯을 닫고 있는 중이라면 무시합니다.
	//if (true == _isClosingWidget)
	//{
	//	return;
	//}

	//. 현재의 정렬 순서를 가져와서 반전 시킵니다.
	ETESFilterSortingOrder currentSortingOrder = _currentFilterData._sortingOrder;
	switch (currentSortingOrder)
	{
	case ETESFilterSortingOrder::Descending:		currentSortingOrder = ETESFilterSortingOrder::Ascending;		break;
	case ETESFilterSortingOrder::Ascending:			currentSortingOrder = ETESFilterSortingOrder::Descending;		break;
	default:
		{
			TESLOG(Error, TEXT("Invalid SortingOrder. : %d"), currentSortingOrder);

			return;	
		}
		break;
	}
	
	//. 아이템 인벤토리의 필터 데이터를 갱신합니다.
	_currentFilterData._sortingOrder = currentSortingOrder;

	//. 데이터 매니져의 필터 데이터를 갱신합니다.
	SharedSaveGameManager->SetFilterData(_currentFilterData._contentsType, _currentFilterData);

	//. 정렬된 리스트를 컨테이너에 출력합니다.
	SetContentsInventoryView(GetItemCategoryDatas(ETESItemInventoryMainCategory::Equip));

	//. 현재의 정렬 순서로 정렬 박스를 출력해줍니다.
	SetContentsInventoryFilterSortingOrderButton(_currentFilterData._isFiltering, _currentFilterData._sortingOrder);
}

void UTESUWEquipmentEnhanceMain::OnEvent_Home()
{
    SharedSceneManager->OnEvent_ContentsHome();
}

void UTESUWEquipmentEnhanceMain::OnEvent_Back()
{
    SharedSceneManager->OnEvent_ContentsBack();

	//. SharedPopupManager->DestroyPopupWidget(this);
}

void UTESUWEquipmentEnhanceMain::OnEvent_Enhance()
{
	//if (true == _isOpeningMessageBox)
	//{
	//	return;
	//}

	TESCHECK(nullptr != _currentItemData);

	//. 마나링이 부족하면 강화 할수 없다는 문구를 띄워준다.
	int64 currentManaring = SharedUserDataManager->GetGoodsTotalCountByResourceType(ETypeResource::Manaring);

	//. 현재 가지고 있는 마나링이 필요한 마나링보다 작으면 문구를 띄워준다.
	if (currentManaring < _requiredMaterialValue)
	{
		SharedPopupManager->ShowMessageBox(TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Equip_Enchane_Error_Mararing));
		return;
	}

	//. _isOpeningMessageBox = true;

	UTESUWEquipmentEnhanceMessageBox* messageBoxWidget = SharedPopupManager->CreatePopupWidget<UTESUWEquipmentEnhanceMessageBox>(TEXT("WBP_EquipmentEnhance_MessageBox"), TEXT("EquipmentEnhance/WBP"));
	if (false == ::IsValid(messageBoxWidget))
	{
		return;
	}
	
	messageBoxWidget->InitWidget();
	messageBoxWidget->OpenWidget();
	messageBoxWidget->SetWidget(_currentItemData, _currentItemData->GetItemUnique(), _requiredMaterialValue);
}

void UTESUWEquipmentEnhanceMain::OnEvent_EnhanceDisable()
{
	if(true == ::IsValid(_currentItemData))
	{ 
		//. 더이상 강화 할수 없습니다.(이미 최대 강화)
		SharedPopupManager->ShowMessageBox(TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Equip_Enchane_Error_MaxLevel));
	}
	else
	{
		SharedPopupManager->ShowMessageBox(TESUtilTable::GetGlobalString(GetWorld(), EGStringID::Equip_Enchane_Error_NoEquip));
	}
	return;
}

void UTESUWEquipmentEnhanceMain::OnEvent_ItesmSlotClicked(UTESGDGoodsItem* selectedItemData)
{
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
		_currentInventoryItemSlotDatas[_selectedItemUnique]->SetItemSelected(false);

		UTESUWCharInfoContentsItemSlot* itemSlotWidget = Cast<UTESUWCharInfoContentsItemSlot>(_tv_itemInventoryViewContainer->GetEntryWidgetFromItem(_currentInventoryItemSlotDatas[_selectedItemUnique]));
		if (true == ::IsValid(itemSlotWidget))
		{
			itemSlotWidget->SetItemSelected(false);
		}
	}

	//. 선택된 인덱스를 갱신하여줍니다.
	_selectedItemUnique = itemUnique;

	_currentInventoryItemSlotDatas[_selectedItemUnique]->SetItemSelected(true);

	UTESUWCharInfoContentsItemSlot* itemSlotWidget = Cast<UTESUWCharInfoContentsItemSlot>(_tv_itemInventoryViewContainer->GetEntryWidgetFromItem(_currentInventoryItemSlotDatas[_selectedItemUnique]));
	if (true == ::IsValid(itemSlotWidget))
	{
		itemSlotWidget->SetItemSelected(true);
	}

	//. 선택된 인덱스를 갱신하여줍니다.
	_selectedItemUnique = itemUnique;

	//. 해당 아이템에 맞춰서 세팅을 다시 해줍니다.
	RefreshEquipmentInfo(selectedItemData);
}
