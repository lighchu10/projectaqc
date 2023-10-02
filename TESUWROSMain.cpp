// Fill out your copyright notice in the Description page of Project Settings.
#include "Contents/RingOfStella/UI/TESUWROSMain.h"
#include "Contents/RingOfStella/UI/TESUWROSLevelUp.h"
#include "Contents/RingOfStella/UI/TESUWROSMainStarSlot.h"
#include "Contents/RingOfStella/UI/TESEDROSMainStarSlot.h"
#include "Contents/RingOfStella/UI/TESUWROSMainBonusStatSlot.h"
#include "Contents/RingOfStella/Manager/TESROSManager.h"
#include "Contents/Common/UI/TESUWCommonHeader.h"
#include "Contents/Common/UI/TESUWCommonToggleSwitcher.h"
#include "Contents/Common/UI/CustomSlates/TESWidgetSwitcher.h"
#include "Contents/Common/UI/CustomSlates/TESImage.h"
#include "Contents/Common/UI/CustomSlates/TESTextBlock.h"
#include "Contents/Common/UI/CustomSlates/TESListView.h"
#include "Contents/Common/UI/CustomSlates/TESWrapBox.h"
#include "Contents/Common/Manager/TESUIManager.h"
#include "Contents/Common/Manager/TESPopupManager.h"
#include "Contents/Common/Manager/TESUserDataManager.h"
#include "Contents/Common/Manager/TESSaveGameManager.h"
#include "Contents/Common/UI/Filter/TESUWFilter.h"
#include "TESUtilFilter.h"
#include "TESUtilString.h"
#include "TESUtilTable.h"
#include "TESUtilPath.h"
#include "TESUtils.h"

void UTESUWROSMain::InitWidget()
{
	Super::InitWidget();

	InitCommonHeaderWidget();

	InitStarFilter();

	InitStarIcon();

	InitStarLevel();

	InitStarTier();

	InitStarCategoryName();

	InitStarName();

	InitFlavorText();

	InitStarPower();

	InitStarTierCategories();

	InitStarTierCategoriesNotice();

	InitStarViewSwitcher();

	InitStarViewContents();

	InitStarViewEmptyText();

	InitBonusStatView();

	InitStarLevelUpNotice();

	InitStarLevelUpWidgetSwitcher();
}

void UTESUWROSMain::ClearWidget()
{
	Super::ClearWidget();

	GetWorld()->GetTimerManager().ClearTimer(_th_appear);

	GetWorld()->GetTimerManager().ClearTimer(_th_disappear);

	ClearCommonHeaderWidget();

	ClearStarListView();

	ClearBonusStatView();
}

void UTESUWROSMain::OpenWidget()
{
	Super::OpenWidget();

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UTESUWROSMain::CloseWidget()
{
	Super::CloseWidget();

	SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWROSMain::OpenDelegate()
{
	Super::OpenDelegate();

	SAFE_BIND_UOBJECT_DELEGATE(FTESUtilEvents::OnCommonFilterApply, OnEvent_ApplyStarFilter);
}

void UTESUWROSMain::CloseDelegate()
{
    Super::CloseDelegate();

	SAFE_UNBIND_UOBJECT_DELEGATE(FTESUtilEvents::OnCommonFilterApply);
}

void UTESUWROSMain::InitCommonHeaderWidget()
{
	_commonHeaderWidget = Cast<UTESUWCommonHeader>(GetWidgetFromName(TEXT("WBP_CommonHeader")));
	TESCHECK(nullptr != _commonHeaderWidget);
	_commonHeaderWidget->InitWidget();
	_commonHeaderWidget->CloseWidget();
}

void UTESUWROSMain::InitStarFilter()
{
	InitStarSwitcher();

	InitStarFilterButtonText();

	InitStarSortingOrderButton();
}

void UTESUWROSMain::InitStarSwitcher()
{
	_cts_starFilterSwitcher = Cast<UTESUWCommonToggleSwitcher>(GetWidgetFromName(TEXT("CTS_StarFilterSwitcher")));
	TESCHECK(nullptr != _cts_starFilterSwitcher);
	_cts_starFilterSwitcher->SetToggleState(false);
}

void UTESUWROSMain::InitStarFilterButtonText()
{
	//. [ON] 상태일때의 필터 버튼 텍스트
	UTESTextBlock* txt_filter_on = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_Filter_ON")));
	TESCHECK(nullptr != txt_filter_on);
	_txt_starFilterTextMap.Add(true, txt_filter_on);

	//. [OFF] 상태일때의 필터 버튼 텍스트
	UTESTextBlock* txt_filter_off = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_Filter_OFF")));
	TESCHECK(nullptr != txt_filter_off);
	_txt_starFilterTextMap.Add(false, txt_filter_off);
}

void UTESUWROSMain::InitStarSortingOrderButton()
{
	//. [ON] 상태일때의 정렬 버튼
	UCheckBox* chkb_on = Cast<UCheckBox>(GetWidgetFromName(TEXT("CHKB_Sorter_ON")));
	TESCHECK(nullptr != chkb_on);
	_chkb_starFilterSortingOrderMap.Add(true, chkb_on);

	//. [OFF] 상태일때의 정렬 버튼
	UCheckBox* chkb_off = Cast<UCheckBox>(GetWidgetFromName(TEXT("CHKB_Sorter_OFF")));
	TESCHECK(nullptr != chkb_off);
	_chkb_starFilterSortingOrderMap.Add(false, chkb_off);
}

void UTESUWROSMain::InitStarIcon()
{
	_img_starIcon = Cast<UTESImage>(GetWidgetFromName(TEXT("IMG_StarIcon")));
	TESCHECK(nullptr != _img_starIcon);
	_img_starIcon->SetDynamicEmptyTexture();
}

void UTESUWROSMain::InitStarLevel()
{
	_txt_starLevel = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_StarLv")));
	TESCHECK(nullptr != _txt_starLevel);
	_txt_starLevel->SetText(FText::FromString(TEXT("")));
}

void UTESUWROSMain::InitStarTier()
{
	_txt_starTier = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_StarTier")));
	TESCHECK(nullptr != _txt_starTier);
	_txt_starTier->SetText(FText::FromString(TEXT("")));
}

void UTESUWROSMain::InitStarCategoryName()
{
	_txt_starCategoryName = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_Category")));
	TESCHECK(nullptr != _txt_starCategoryName);
	_txt_starCategoryName->SetText(FText::FromString(TEXT("")));
}

void UTESUWROSMain::InitStarName()
{
	_txt_starName = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_StarName")));
	TESCHECK(nullptr != _txt_starName);
	_txt_starName->SetText(FText::FromString(TEXT("")));
}

void UTESUWROSMain::InitFlavorText()
{
	_txt_flavor = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_Flavor")));
	TESCHECK(nullptr != _txt_flavor);
	_txt_flavor->SetText(FText::FromString(TEXT("")));
}

void UTESUWROSMain::InitStarPower()
{
	_txt_starPower = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_StarPower")));
	TESCHECK(nullptr != _txt_starPower);
	_txt_starPower->SetText(FText::FromString(TEXT("")));
}

void UTESUWROSMain::InitStarTierCategories()
{
	TArray<const FRingOfStellaTierRow*> ringOfStellaTierRows = SharedTDBManager->GetAllRowListRingOfStellaTier();
	for (const FRingOfStellaTierRow* rosTierRow : ringOfStellaTierRows)
	{
		if (nullptr == rosTierRow)
		{
			continue;
		}

		int32 rosTier = rosTierRow->Tier;

		//. 티어 카테고리 토글 스윗쳐
		{
			FString widgetName = FString::Printf(TEXT("CTS_TierCategory_%d"), rosTier);
			UTESUWCommonToggleSwitcher* cts = Cast<UTESUWCommonToggleSwitcher>(GetWidgetFromName(*widgetName));
			TESCHECK_CONTINUE(nullptr != cts);
			cts->SetToggleState(false);

			_cts_tierCategories.Add(rosTierRow->Id, cts);
		}

		//. 티어 카테고리 선택 이미지
		{
			FString widgetName = FString::Printf(TEXT("IMG_TierSelect_%d"), rosTier);
			UTESImage* img_starTierCategories_select = Cast<UTESImage>(GetWidgetFromName(*widgetName));
			TESCHECK_CONTINUE(nullptr != img_starTierCategories_select);
			img_starTierCategories_select->SetVisibility(ESlateVisibility::Collapsed);

			_img_tierCategories_select.Add(rosTierRow->Id, img_starTierCategories_select);
		}
	}
}

void UTESUWROSMain::InitStarTierCategoriesNotice()
{
    TArray<const FRingOfStellaTierRow*> ringOfStellaTierRows = SharedTDBManager->GetAllRowListRingOfStellaTier();
    for (const FRingOfStellaTierRow* rosTierRow : ringOfStellaTierRows)
    {
        if (nullptr == rosTierRow)
        {
            continue;
        }

        int32 rosTier = rosTierRow->Tier;

        FString widgetName = FString::Printf(TEXT("IMG_TierCategory_Notice_%d"), rosTier);
        UTESImage* img_starTierCategories_notice = Cast<UTESImage>(GetWidgetFromName(*widgetName));
		if (false == ::IsValid(img_starTierCategories_notice))
		{
			TESLOG(Error, TEXT("Invalid Tier Category Notice. : %d"), rosTier);

			continue;
		}

		img_starTierCategories_notice->SetVisibility(ESlateVisibility::Collapsed);
		
		_img_tierCategories_notice.Add(rosTierRow->Id, img_starTierCategories_notice);
    }
}

void UTESUWROSMain::InitStarViewSwitcher()
{
	_ws_starView = Cast<UWidgetSwitcher>(GetWidgetFromName(TEXT("WS_StarView")));
	TESCHECK(nullptr != _ws_starView);
	_ws_starView->SetActiveWidgetIndex(0);
}

void UTESUWROSMain::InitStarViewContents()
{
	_lv_starView = Cast<UTESListView>(GetWidgetFromName(TEXT("LV_StarView")));
	TESCHECK(nullptr != _lv_starView);
	_lv_starView->ClearListItems();
}

void UTESUWROSMain::InitStarViewEmptyText()
{
	_txt_starViewEmpty = Cast<UTESTextBlock>(GetWidgetFromName(TEXT("TXT_StarView_Empty")));
	TESCHECK(nullptr != _txt_starViewEmpty);
	_txt_starViewEmpty->SetText(FText::FromString(TEXT("")));
}

void UTESUWROSMain::InitBonusStatView()
{
	_wb_bonusStatView = Cast<UTESWrapBox>(GetWidgetFromName(TEXT("WB_BonusStatView")));
	TESCHECK(nullptr != _wb_bonusStatView);
	_wb_bonusStatView->ClearChildren();
}

void UTESUWROSMain::InitStarLevelUpNotice()
{
	_img_starLevelUpNotice = Cast<UTESImage>(GetWidgetFromName(TEXT("IMG_Notice")));
	TESCHECK(nullptr != _img_starLevelUpNotice);
	_img_starLevelUpNotice->SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWROSMain::InitStarLevelUpWidgetSwitcher()
{
	_ws_levelUp = Cast<UTESWidgetSwitcher>(GetWidgetFromName(TEXT("WS_LevelUp")));
	TESCHECK(nullptr != _ws_levelUp);
	_ws_levelUp->SetActiveWidgetIndex(0);
}

void UTESUWROSMain::ClearCommonHeaderWidget()
{
	TESCHECK(nullptr != _commonHeaderWidget);
	_commonHeaderWidget->CloseWidget();
	_commonHeaderWidget->ClearWidget();
	_commonHeaderWidget = nullptr;
}

void UTESUWROSMain::ClearStarListView()
{
	for (UTESEDROSMainStarSlot* rosDetailStarSlotData : _rosDetailStarSlotDatas)
	{
		if (true == ::IsValid(rosDetailStarSlotData))
		{
			rosDetailStarSlotData->RemoveFromRoot();
			rosDetailStarSlotData->ConditionalBeginDestroy();
			rosDetailStarSlotData = nullptr;
		}
	}

	_rosDetailStarSlotDatas.Empty();
}

void UTESUWROSMain::ClearBonusStatView()
{
	for (UTESUWROSMainBonusStatSlot* rosDetailBonusStatSlotWidget : _rosDetailBonusStatSlotWidgets)
	{
		SharedUIManager->DestroyCtrWidget(rosDetailBonusStatSlotWidget);
	}

	_rosDetailBonusStatSlotWidgets.Empty();
}

void UTESUWROSMain::SetWidget(const int32& rosStarId)
{
	if (false == SetData(rosStarId))
	{
		CloseWidget();

		return;
	}

	int32 currentRetteraStarLevel = SharedROSManager->GetCurrentRetteraStarLevel();

	SetCommonHeaderWidget();

	SetStarTierCategories(currentRetteraStarLevel);

	SetStarTierCategoriesNotice(currentRetteraStarLevel);

	SetStarFilter(ETypeSortFilterContents::RingOfStella);

	SetStarIcon(TESUtilPath::GetRingOfStarIconResourcePath(_currentROSStarId));

	SetStarLevel(SharedROSManager->GetCurrentStarLevel(_currentROSStarId));

	SetStarTier(SharedROSManager->GetROSTierByROSStarId(_currentROSStarId));

	SetStarCategoryName(TESUtilTable::GetGlobalString(GetWorld(), _rosStarRow.CategoryName));

	SetStarName(TESUtilTable::GetGlobalString(GetWorld(), _rosStarRow.StarName));

	SetFlavorText(TESUtilTable::GetGlobalString(GetWorld(), _rosStarRow.FlavorText));

	SetStarPower(SharedROSManager->GetROSStarPowerValue(_currentROSStarId));

	SetBonusStatView(_currentROSStarId);

	SelectTier(SharedROSManager->GetROSTierIdByROSStarId(rosStarId));

	SetStarLevelUpNotice(SharedROSManager->CheckLevelupable(rosStarId));

	SetStarLevelUpWidgetSwitcher(SharedROSManager->CheckIfMaximumLevel(rosStarId));
}

bool UTESUWROSMain::SetData(const int32& rosStarId)
{
	const FRingOfStellaStarRow& rosStarRow = SharedTDBManager->GetRingOfStellaStarRow(rosStarId);
	if (true == rosStarRow.IsEmpty())
	{
		TESLOG(Error, TEXT("Invalid RingOfStellaStarRow : %d"), rosStarId);

		return false;
	}

	_rosStarRow = rosStarRow;

	_previousROSTierId = TESConstantValue::NoneValue;

	_currentROSTierId = TESConstantValue::NoneValue;

	_currentROSStarId = rosStarId;

	return true;
}

void UTESUWROSMain::SetCommonHeaderWidget()
{
	if (true == ::IsValid(_commonHeaderWidget))
	{
		_commonHeaderWidget->OpenWidget();
		_commonHeaderWidget->SetWidget();
	}
}

void UTESUWROSMain::SetStarTierCategories(const int32& retteraStarLevel)
{
	TArray<const FRingOfStellaTierRow*> ringOfStellaTierRows = SharedTDBManager->GetAllRowListRingOfStellaTier();
	for (const FRingOfStellaTierRow* rosTierRow : ringOfStellaTierRows)
	{
		if (nullptr == rosTierRow)
		{
			continue;
		}

		int32 rosTierId = rosTierRow->Id;

		if (false == _cts_tierCategories.Contains(rosTierId))
		{
			continue;
		}

		if (false == ::IsValid(_cts_tierCategories[rosTierId]))
		{
			continue;
		}

		//. 유저의 리테라 레벨이 티어를 여는 조건 하는 여부에 따라 활성화 비활성화 여부를 결정합니다.
		_cts_tierCategories[rosTierId]->SetToggleState(retteraStarLevel >= rosTierRow->OpenReterraLevel);
	}
}

void UTESUWROSMain::SetStarTierCategoriesNotice(const int32& retteraStarLevel)
{
    TArray<const FRingOfStellaTierRow*> ringOfStellaTierRows = SharedTDBManager->GetAllRowListRingOfStellaTier();
    for (const FRingOfStellaTierRow* rosTierRow : ringOfStellaTierRows)
    {
        if (nullptr == rosTierRow)
        {
            continue;
        }

        int32 rosTierId = rosTierRow->Id;

        if (false == _img_tierCategories_notice.Contains(rosTierId))
        {
            continue;
        }

        if (false == ::IsValid(_img_tierCategories_notice[rosTierId]))
        {
            continue;
        }

		if (retteraStarLevel >= rosTierRow->OpenReterraLevel)
		{
            bool isExistLevelupableTierStars = SharedROSManager->CheckIfThereIsAnyLevelupableStarByTierStars(rosTierRow->StarList);

			_img_tierCategories_notice[rosTierId]->SetVisibility(isExistLevelupableTierStars ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
		}
		else
		{
			_img_tierCategories_notice[rosTierId]->SetVisibility(ESlateVisibility::Collapsed);
		}
    }
}

void UTESUWROSMain::SetStarFilter(const ETypeSortFilterContents& filterContentsType)
{
	TOptional<FTESFilterData> filterData = SharedSaveGameManager->GetFilterData(filterContentsType);
	if (false == filterData.IsSet())
	{
		TESLOG(Error, TEXT("Invalid FilterData. %d"), filterContentsType);

		return;
	}

	//. 해당 필터의 데이터로 필터 버튼 컨텐츠들을 세팅합니다.
	SetStarFilter(filterData.GetValue());
}

void UTESUWROSMain::SetStarFilter(const FTESFilterData& filterData)
{
	//. 아이템 인벤토리에서 사용하는 필터 데이터도 갱신해줍니다.
	_currentFilterData = filterData;

	//. 현재의 필터여부에 따라 필터 버튼을 교체해줍니다.
	SetStarFilterSwitcher(_currentFilterData._isFiltering);

	//. 현재의 필터 텍스트를 세팅합니다.
	SetStarFilterButtonText(_currentFilterData);

	//. 현재의 정렬 순서로 정렬 박스를 출력해줍니다.
	SetStarFilterSortingOrderButton(_currentFilterData._isFiltering, _currentFilterData._sortingOrder);
}

void UTESUWROSMain::SetStarFilterSwitcher(bool isOn)
{
	if (true == ::IsValid(_cts_starFilterSwitcher))
	{
		_cts_starFilterSwitcher->SetToggleState(isOn);
	}
}

void UTESUWROSMain::SetStarFilterButtonText(FTESFilterData filterData)
{
	const FSortFilterElementRow& filterElementTable = SharedTDBManager->GetSortFilterElementRow(filterData._sortingElementId);
	if (true == filterElementTable.IsEmpty())
	{
		SetStarFilterButtonText(filterData._isFiltering, TESConstantValue::NoneValue);
	}
	else
	{
		SetStarFilterButtonText(filterData._isFiltering, filterElementTable.SortFilterNameID);
	}
}

void UTESUWROSMain::SetStarFilterButtonText(bool isOn, int32 buttonTextId)
{
	if (true == _txt_starFilterTextMap.Contains(isOn))
	{
		FString str = TESUtilTable::GetGlobalString(GetWorld(), buttonTextId);
		_txt_starFilterTextMap[isOn]->SetText(FText::FromString(*str));
	}
}

void UTESUWROSMain::SetStarFilterSortingOrderButton(FTESFilterData filterData)
{
	SetStarFilterSortingOrderButton(filterData._isFiltering, filterData._sortingOrder);
}

void UTESUWROSMain::SetStarFilterSortingOrderButton(bool isOn, ETESFilterSortingOrder sortingOrder)
{
	if (true == _chkb_starFilterSortingOrderMap.Contains(isOn))
	{
		//. Checked : 내림차순, Unchecked : 오름차순
		_chkb_starFilterSortingOrderMap[isOn]->SetCheckedState(sortingOrder == ETESFilterSortingOrder::Descending ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	}
}

void UTESUWROSMain::SetStarIcon(const FString& resourcePath)
{
	if (true == ::IsValid(_img_starIcon))
	{
		_img_starIcon->SetDynamicTexture(resourcePath);
	}
}

void UTESUWROSMain::SetStarLevel(const int32& starLevel)
{
	if (true == ::IsValid(_txt_starLevel))
	{
		_txt_starLevel->SetText(FText::FromString(FString::Printf(TEXT("Lv.%d"), starLevel)));
	}
}

void UTESUWROSMain::SetStarTier(const int32& starTier)
{
	if (true == ::IsValid(_txt_starTier))
	{
		FString formatString = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::RingOfStella_Tier_);
		FString tierString = TESUtilString::MakeFormatString(formatString, starTier);
		_txt_starTier->SetText(FText::FromString(tierString));
	}
}

void UTESUWROSMain::SetStarCategoryName(const FString& starCategoryName)
{
	if (true == ::IsValid(_txt_starCategoryName))
	{
		_txt_starCategoryName->SetText(FText::FromString(starCategoryName));
	}
}

void UTESUWROSMain::SetStarName(const FString& starName)
{
	if (true == ::IsValid(_txt_starName))
	{
		_txt_starName->SetText(FText::FromString(starName));
	}
}

void UTESUWROSMain::SetFlavorText(const FString& context)
{
	if (true == ::IsValid(_txt_flavor))
	{
		_txt_flavor->SetText(FText::FromString(context));
	}
}

void UTESUWROSMain::SetStarPower(const int32& starPower)
{
	if (true == ::IsValid(_txt_starPower))
	{
		_txt_starPower->SetText(FText::FromString(FString::FormatAsNumber(starPower)));
	}
}

void UTESUWROSMain::SetStarTierCategory(const int32& rosTierId, const bool& isOn)
{
	if (TESConstantValue::NoneValue == rosTierId)
	{
		return;
	}

	if (false == _img_tierCategories_select.Contains(rosTierId))
	{
		TESLOG(Error, TEXT("Not Found Tier : %d"), rosTierId);

		return;
	}

	if (false == ::IsValid(_img_tierCategories_select[rosTierId]))
	{
		TESLOG(Error, TEXT("Invalid TierCategory. : %d"), rosTierId);

		return;
	}

	_img_tierCategories_select[rosTierId]->SetVisibility(isOn ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UTESUWROSMain::SetStarView(const int32& rosTierId, const bool& bUseScrollToTop /*= false*/)
{
	const FRingOfStellaTierRow& rosTierRow = SharedTDBManager->GetRingOfStellaTierRow(rosTierId);
	if (true == rosTierRow.IsEmpty())
	{
		TESLOG(Error, TEXT("Invalid RingOfStellaTierRow : %d"), rosTierId);

		return;
	}

	SetStarView(rosTierRow.StarList, bUseScrollToTop);
}

void UTESUWROSMain::SetStarView(TArray<int32> rosStarDatas, const bool& bUseScrollToTop /*= false*/)
{
	//. 데이터 목록이 존재하다면 필터 후 정렬하고, 출력합니다.
	if (rosStarDatas.Num() > 0)
	{
		//. 필터링된 데이터 목록을 얻어옵니다.
		TArray<int32> filteredStarDatas = MakeFilteredROSStarDatas(rosStarDatas
																  , _currentFilterData._filteringData
																  , _currentFilterData._isFiltering);

		//. 필터링된 데이터가 있을 경우, 정렬 후 출력합니다.
		if (true == filteredStarDatas.Num() > 0)
		{
			//. 필터링된 데이터 목록을 정렬하여 얻어옵니다.
			TArray<int32> sortedStarDatas = MakeSortedROSStarDatas(filteredStarDatas
																    , _currentFilterData._sortingElementId
																    , _currentFilterData._sortingOrder);

			//. 데이터 목록을 보여주기 위해 리스트뷰로 변경해줍니다.
			SetStarViewSwitcher(false);

			//. 데이터를 출력합니다.
			SetStarViewContents(sortedStarDatas, bUseScrollToTop);
		}
		//. 필터링된 데이터가 없을 경우, 목록이 없다는 메세지를 출력해줍니다.
		else
		{
			//. 비어있는 텍스트 문구로 교체합니다.
			SetStarViewSwitcher(true);

			//. 아무것도 출력될 것이 없다는 텍스트를 출력합니다.
			SetStarViewEmptyText(TESUtilTable::GetGlobalString(GetWorld(), EGStringID::RingOfStella_No_Filter));
		}
	}
	//. 데이터 목록이 존재하지 않는다면, 보유한 목록이 없다는 메세지를 출력해줍니다.
	else
	{
		//. 비어있는 텍스트 문구로 교체합니다.
		SetStarViewSwitcher(true);

		//. 아무것도 출력될 것이 없다는 텍스트를 출력합니다.
		SetStarViewEmptyText(TESUtilTable::GetGlobalString(GetWorld(), EGStringID::RingOfStella_No_Filter));
	}
}

void UTESUWROSMain::SetStarViewSwitcher(bool isEmptied)
{
	if (true == ::IsValid(_ws_starView))
	{
		_ws_starView->SetActiveWidgetIndex(isEmptied ? 1 : 0);
	}
}

void UTESUWROSMain::SetStarViewContents(TArray<int32> rosStarDatas, const bool& bUseScrollToTop /*= false*/)
{
	if (false == ::IsValid(_lv_starView))
	{
		return;
	}

	ClearStarListView();

	_lv_starView->ClearListItems();

	int32 selectedROSStarId = SharedROSManager->GetSelectedROSStarId();
	//. TESLOG(Display, TEXT("%d"), selectedROSStarId);

	for (int32 rosStarId : rosStarDatas)
	{
		UTESEDROSMainStarSlot* rosDetailStarSlotData = NewObject<UTESEDROSMainStarSlot>();
		TESCHECK_CONTINUE(nullptr != rosDetailStarSlotData);
		rosDetailStarSlotData->SetStarId(rosStarId);
		rosDetailStarSlotData->SetStarTierId(SharedROSManager->GetROSTierIdByROSStarId(rosStarId));
		rosDetailStarSlotData->SetStarSelected(rosStarId == selectedROSStarId);

		_rosDetailStarSlotDatas.Add(rosDetailStarSlotData);

		_lv_starView->AddItem(rosDetailStarSlotData);
	}

	if (true == bUseScrollToTop)
	{
		_lv_starView->ScrollToTop();
	}
}

void UTESUWROSMain::SetStarViewEmptyText(const FString& str)
{
	if (true == ::IsValid(_txt_starViewEmpty))
	{
		_txt_starViewEmpty->SetText(FText::FromString(str));
	}
}

void UTESUWROSMain::SetBonusStatView(const int32& rosStarId)
{
	if (false == ::IsValid(_wb_bonusStatView))
	{
		return;
	}

	ClearBonusStatView();

	_wb_bonusStatView->ClearChildren();

	int32 rosStarLevel = SharedROSManager->GetCurrentStarLevel(rosStarId);
	const FRingOfStellaEffectRow& rosEffectRow = SharedROSManager->GetROSEffectRow(rosStarId, rosStarLevel);
	if (true == rosEffectRow.IsEmpty())
	{
		TESLOG(Error, TEXT("Invalid ROS Effect Row : %d"), _currentROSStarId);

		return;
	}

	for (FTESStatOption statOption : rosEffectRow.LevelStatOption)
	{
		//. 스텟 옵션이 None인 경우에는 추가되지 않도록 예외처리합니다.
		if (ETypeStatOption::None == statOption._statOptionType)
		{
			continue;
		}

		UTESUWROSMainBonusStatSlot* rosDetailBonusStatSlotWidget = SharedUIManager->CreateCtrWidget<UTESUWROSMainBonusStatSlot>(TEXT("WBP_ROSMainBonusStatSlot"), TEXT("RingOfStella/WBP"));
		TESCHECK_CONTINUE(nullptr != rosDetailBonusStatSlotWidget);
		rosDetailBonusStatSlotWidget->InitWidget();
		rosDetailBonusStatSlotWidget->OpenWidget();
		rosDetailBonusStatSlotWidget->SetWidget(statOption);

		_rosDetailBonusStatSlotWidgets.Add(rosDetailBonusStatSlotWidget);

		_wb_bonusStatView->AddChild(rosDetailBonusStatSlotWidget);
	}
}

void UTESUWROSMain::SetStarLevelUpNotice(const bool& isOn)
{
	if (true == ::IsValid(_img_starLevelUpNotice))
	{
		_img_starLevelUpNotice->SetVisibility(isOn ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	}
}

void UTESUWROSMain::SetStarLevelUpWidgetSwitcher(const bool& isMaxLevel)
{
	if (true == ::IsValid(_ws_levelUp))
	{
		_ws_levelUp->SetActiveWidgetIndex(isMaxLevel);
	}
}

TArray<int32> UTESUWROSMain::MakeFilteredROSStarDatas(TArray<int32> rosStarDatas
												 , TMap<int32, FTESFilterFilteringGroupData> filteringData
												 , bool isFiltering)
{
	return TESUtilFilter::MakeFilteredROSStarDatas(rosStarDatas, filteringData, isFiltering);
}

TArray<int32> UTESUWROSMain::MakeSortedROSStarDatas(TArray<int32> rosStarDatas
												  , int32 sortingElementId
												  , ETESFilterSortingOrder sortingOrder)
{
	return TESUtilFilter::MakeSortedROSStarDatas(rosStarDatas, sortingElementId, sortingOrder);
}

void UTESUWROSMain::SelectTier(const int32& rosTierId)
{
	if (_currentROSTierId == rosTierId)
	{
		return;
	}

	_previousROSTierId = _currentROSTierId;

	_currentROSTierId = rosTierId;

	SetStarTierCategory(_previousROSTierId, false);

	SetStarTierCategory(_currentROSTierId, true);

	SetStarView(_currentROSTierId);
}

void UTESUWROSMain::RefreshAll()
{
	SetWidget(_currentROSStarId);
}

void UTESUWROSMain::RefreshStarView()
{
	SetStarView(_currentROSTierId);
}

void UTESUWROSMain::RefreshStarLevelUpNotice()
{
	SetStarLevelUpNotice(SharedROSManager->CheckLevelupable(_currentROSStarId));
}

void UTESUWROSMain::RefreshStarTierCategoriesNotice()
{
    SetStarTierCategoriesNotice(SharedROSManager->GetCurrentRetteraStarLevel());
}

void UTESUWROSMain::PlayAppearAnim(TDelegate<void()> onNextAction)
{
	float finishTime = PlayWGAnimation(TEXT("AppearContents"));
	finishTime = FMath::Clamp(finishTime, 0.01f, finishTime);

	GetWorld()->GetTimerManager().ClearTimer(_th_appear);
	GetWorld()->GetTimerManager().SetTimer(_th_appear, FTimerDelegate::CreateLambda([=]()
	{
		onNextAction.ExecuteIfBound();
	}), finishTime, false);
}

void UTESUWROSMain::PlayDisappearAnim(TDelegate<void()> onNextAction)
{
	float finishTime = PlayWGAnimation(TEXT("DisappearContents"));
	finishTime = FMath::Clamp(finishTime, 0.01f, finishTime);

	GetWorld()->GetTimerManager().ClearTimer(_th_disappear);
	GetWorld()->GetTimerManager().SetTimer(_th_disappear, FTimerDelegate::CreateLambda([=]()
	{
		onNextAction.ExecuteIfBound();
	}), finishTime, false);
}

void UTESUWROSMain::OnEvent_StarListView_EntryInitailized(UObject* item, UUserWidget* widget)
{
#if WITH_EDITOR	
	if (false == TESUtils::IsEditorPlayingGame(GetWorld()))
	{
		return;
	}
#endif //. WITH_EDITOR

	UTESUWROSMainStarSlot* rosDetailStarSlotWidget = Cast<UTESUWROSMainStarSlot>(widget);
	TESCHECK(nullptr != rosDetailStarSlotWidget);
	
	rosDetailStarSlotWidget->InitWidget();
	rosDetailStarSlotWidget->OpenWidget();
	rosDetailStarSlotWidget->SetWidget(Cast<UTESEDROSMainStarSlot>(item));
}

void UTESUWROSMain::OnEvent_StarListView_EntryReleased(UUserWidget* widget)
{
#if WITH_EDITOR	
	if (false == TESUtils::IsEditorPlayingGame(GetWorld()))
	{
		return;
	}
#endif //. WITH_EDITOR

	UTESUWROSMainStarSlot* rosDetailStarSlotWidget = Cast<UTESUWROSMainStarSlot>(widget);
	TESCHECK(nullptr != rosDetailStarSlotWidget);
	rosDetailStarSlotWidget->CloseWidget();
	rosDetailStarSlotWidget->ClearWidget();
}

void UTESUWROSMain::OnEvent_OpenStarFilter()
{
    if (true == SharedROSManager->IsROSStarting())
    {
        return;
    }

	if (true == SharedROSManager->IsStarTierUpLeveling())
	{
		return;
	}

	UTESUWFilter* filterWidget = SharedPopupManager->CreatePopupWidget<UTESUWFilter>(TEXT("WBP_Filter"), TEXT("Common/WBP/Filter"));
	TESCHECK(nullptr != filterWidget);

	filterWidget->InitWidget();
	filterWidget->OpenWidget();
	filterWidget->SetWidget(_currentFilterData._contentsType);
}

void UTESUWROSMain::OnEvent_ApplyStarFilter(FTESFilterData filterData)
{
	SetStarFilter(filterData);

	RefreshStarView();
}

void UTESUWROSMain::OnEvent_ChangeSotingOrder()
{
    if (true == SharedROSManager->IsROSStarting())
    {
        return;
    }

	if (true == SharedROSManager->IsStarTierUpLeveling())
	{
		return;
    }

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

	//. 현재의 정렬 순서로 정렬 박스를 출력해줍니다.
	SetStarFilterSortingOrderButton(_currentFilterData._isFiltering, _currentFilterData._sortingOrder);

	//. 정렬에 맞는 내용으로 다시 갱신합니다.
	RefreshStarView();
}

void UTESUWROSMain::OnEvent_TierSelected(int32 tier)
{
    if (true == SharedROSManager->IsROSStarting())
    {
        return;
    }

	if (true == SharedROSManager->IsStarTierUpLeveling())
	{
		return;
    }

	const FRingOfStellaTierRow& rosTierRow = SharedROSManager->GetROSTierRowByROSTier(tier);
	if(true == rosTierRow.IsEmpty())
	{
		TESLOG(Error, TEXT("Invalid ROS Tier Row : %d"), tier);

		return;
	}

	int32 retteraStarLevel = SharedROSManager->GetCurrentRetteraStarLevel();
	if (retteraStarLevel >= rosTierRow.OpenReterraLevel)
	{
		SelectTier(rosTierRow.Id);
	}
	else
	{
		FString formatString = TESUtilTable::GetGlobalString(GetWorld(), EGStringID::RingOfStella_Tier_OpenCondition);
		FString messageString = TESUtilString::MakeFormatString(formatString, rosTierRow.OpenReterraLevel);
		SharedPopupManager->ShowMessageBox(messageString);
	}
}

void UTESUWROSMain::OnEvent_LevelUp()
{
    if (true == SharedROSManager->IsROSStarting())
    {
        return;
    }

	if (true == SharedROSManager->IsStarTierUpLeveling())
	{
		return;
    }

	//. TESLOG(Display, TEXT("StarId : %d, TierId : %d"), _currentROSStarId, _currentROSTierId);

	UTESUWROSLevelUp* rosLevelUpWidget = SharedPopupManager->CreatePopupWidget<UTESUWROSLevelUp>(TEXT("WBP_ROSLevelUp"), TEXT("RingOfStella/WBP"));
	TESCHECK(nullptr != rosLevelUpWidget);
	rosLevelUpWidget->InitWidget();
	rosLevelUpWidget->OpenWidget();
	rosLevelUpWidget->SetWidget(_currentROSStarId, SharedROSManager->GetROSTierByROSStarId(_currentROSStarId));
}

void UTESUWROSMain::OnEvent_ItemDataUpdated()
{
    RefreshStarView();

    RefreshStarLevelUpNotice();

	RefreshStarTierCategoriesNotice();
}

void UTESUWROSMain::OnEvent_Back()
{
	GoToPrevious();
}

void UTESUWROSMain::OnEvent_Home()
{
	GoToHome();
}
