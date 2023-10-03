// Fill out your copyright notice in the Description page of Project Settings.
#include "Contents/Common/UI/Filter/TESUWFilter.h"
#include "Contents/Common/UI/Filter/TESUWFilterElementBoxGroup.h"
#include "Contents/Common/UI/Filter/TESUWFilterElementBox.h"
#include "Contents/Common/UI/CustomSlates/TESScrollBox.h"
#include "Contents/Common/UI/TESUWCommonToggleSwitcher.h"
#include "Contents/Common/Manager/TESSaveGameManager.h"
#include "Contents/Common/Manager/TESPopupManager.h"
#include "Contents/Common/Manager/TESUIManager.h"
#include "Components/CheckBox.h"
#include "Components/WidgetSwitcher.h"
#include "TESUtils.h"
#include "TESUtilFilter.h"
#include "TESUtilString.h"
#include "TESGameInstance.h"

void UTESUWFilter::InitWidget()
{
	Super::InitWidget();

	InitFilterCategories();

	InitFilterContents();

	InitSortingOrderButtons();
}

void UTESUWFilter::ClearWidget()
{
	Super::ClearWidget();
}

void UTESUWFilter::OpenWidget()
{
	Super::OpenWidget();
	
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UTESUWFilter::CloseWidget()
{
	Super::CloseWidget();

	SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWFilter::OpenDelegate()
{
	Super::OpenDelegate();

	FTESUtilEvents::OnCommonFilterElementBoxAppend.BindUObject(this, &UTESUWFilter::OnEvent_FilterElementBoxAppend);

	FTESUtilEvents::OnCommonFilterSortingTypeChanged.BindUObject(this, &UTESUWFilter::OnEvent_SortingTypeChanged);

	FTESUtilEvents::OnCommonFilterFilteringListAdded.BindUObject(this, &UTESUWFilter::OnEvent_FilteringDataAdded);

	FTESUtilEvents::OnCommonFilterFilteringListRemoved.BindUObject(this, &UTESUWFilter::OnEvent_FilteringDataRemoved);
}

void UTESUWFilter::CloseDelegate()
{
	FTESUtilEvents::OnCommonFilterElementBoxAppend.Unbind();

	FTESUtilEvents::OnCommonFilterSortingTypeChanged.Unbind();

	FTESUtilEvents::OnCommonFilterFilteringListAdded.Unbind();

	FTESUtilEvents::OnCommonFilterFilteringListRemoved.Unbind();
}

void UTESUWFilter::InitFilterCategories()
{
	int32 maxCount = static_cast<int32>(ETypeSortFilterCategory::MAX);
	for (int32 index = 1; index < maxCount; ++index)
	{
		ETypeSortFilterCategory categoryType = static_cast<ETypeSortFilterCategory>(index);
		FString categoryTypeString = TESUtilString::GetEnumAsString<ETypeSortFilterCategory>(TEXT("ETypeSortFilterCategory"), categoryType);

		FString widgetName = FString::Printf(TEXT("CTS_FilterCategory_%s"), *categoryTypeString);
		UTESUWCommonToggleSwitcher* cts_filterCategoryButton = Cast<UTESUWCommonToggleSwitcher>(GetWidgetFromName(*widgetName));
		TESCHECK_CONTINUE(nullptr != cts_filterCategoryButton);

		cts_filterCategoryButton->InitWidget();
		cts_filterCategoryButton->OpenWidget();
		cts_filterCategoryButton->SetToggleState(false);

		_cts_filterCategoryButtonMap.Add(categoryType, cts_filterCategoryButton);
	}
}

void UTESUWFilter::InitFilterContents()
{
	int32 maxCount = static_cast<int32>(ETypeSortFilterCategory::MAX);

	for (int32 index = 1; index < maxCount; ++index)
	{
		FString widgetName = FString::Printf(TEXT("SCRB_Contents_%d"), index);

		UTESScrollBox* scrb_contents = Cast<UTESScrollBox>(GetWidgetFromName(*widgetName));
		TESCHECK_CONTINUE(nullptr != scrb_contents);

		scrb_contents->SetVisibility(ESlateVisibility::Collapsed);
		scrb_contents->ClearChildren();

		ETypeSortFilterCategory filterCategory = static_cast<ETypeSortFilterCategory>(index);
		_scrb_contentsMap.Add(filterCategory, scrb_contents);
	}
}

void UTESUWFilter::InitSortingOrderButtons()
{
	int32 maxCount = static_cast<int32>(ETESFilterSortingOrder::Max);
	for (int32 index = 1; index < maxCount; ++index)
	{
		ETESFilterSortingOrder sortingOrderType = static_cast<ETESFilterSortingOrder>(index);
		FString sortingOrderTypeString = TESUtilString::GetEnumAsString<ETESFilterSortingOrder>(TEXT("ETESFilterSortingOrder"), sortingOrderType);

		FString widgetName = FString::Printf(TEXT("CTS_SortingOrderButton_%s"), *sortingOrderTypeString);
		UTESUWCommonToggleSwitcher* cts_sortingOrderButton = Cast<UTESUWCommonToggleSwitcher>(GetWidgetFromName(*widgetName));
		TESCHECK_CONTINUE(nullptr != cts_sortingOrderButton);

		cts_sortingOrderButton->InitWidget();
		cts_sortingOrderButton->OpenWidget();
		cts_sortingOrderButton->SetToggleState(false);

		_cts_sortingOrderButtonMap.Add(sortingOrderType, cts_sortingOrderButton);
	}
}

void UTESUWFilter::SetWidget(ETypeSortFilterContents contentsType)
{
	if (false == SetData(contentsType))
	{
		CloseWidget();

		return;
	}

	//. 필터 컨텐츠에 해당하는 데이터를 미리 생성합니다.
	SetFilterContents(contentsType);

	//. 처음에 들어오면 무조건 "정렬"탭이 선택된 상태로 시작합니다.
	ChangeFilterCategory(ETypeSortFilterCategory::Sort);

	//. 처음에 들어오면 정렬 순서는 이전에 지정되어 있던 그대로 복원합니다.
	ChangeFilterSortingOrder(_currentFilterData._sortingOrder);
}

bool UTESUWFilter::SetData(ETypeSortFilterContents contentsType)
{
	//. 열어진 필터의 컨텐츠 타입을 저장합니다.
	_currentContentsType = contentsType;

	//. 필터 팝업에 대한 필터 데이터가 있다면 가져옵니다.
	TOptional<FTESFilterData> filterData = SharedSaveGameManager->GetFilterData(contentsType);
	if (true == filterData.IsSet())
	{	
		_currentFilterData = filterData.GetValue();
	}

	//. 모든 시련을 통과하였으니 팝업을 사용하셔도 됩니다. :)
	return true;
}

void UTESUWFilter::SetFilterContents(ETypeSortFilterContents contentsType)
{
	TArray_SortFilterGroup contentsSortFilterTables = TESUtilTable::GetSortFilterGroupAllBySortFilterContentsType(contentsType);
	int32 maxCount = contentsSortFilterTables.Num();
	for (int32 index = 0; index < maxCount; ++index)
	{
		const FSortFilterGroupRow* sortFilterGroupTable = contentsSortFilterTables[index];
		if (nullptr == sortFilterGroupTable)
		{
			continue;
		}

		ETypeSortFilterCategory categoryType = sortFilterGroupTable->T_SortFilterCategory;
		switch (categoryType)
		{
		case ETypeSortFilterCategory::Filter:
		case ETypeSortFilterCategory::Sort:
			{
				SetFilterContentsElementGroup(contentsType, categoryType, (*sortFilterGroupTable));
			}
			break;

		default:
			{
				TESLOG(Error, TEXT("Not Supported Type : %d"), categoryType);
			}
			break;
		}
	}
}

void UTESUWFilter::SetFilterContentsElementGroup(ETypeSortFilterContents contentsType
											   , ETypeSortFilterCategory categoryType
											   , FSortFilterGroupRow sortFilterGroupTable)
{
	if (true == sortFilterGroupTable.IsEmpty())
	{
		TESLOG(Error, TEXT("Invalid SortFilterGroupTable."));

		return;
	}

	if (false == _scrb_contentsMap.Contains(categoryType))
	{
		TESLOG(Error, TEXT("Invalid Category Contents ScrollBox"));

		return;
	}

	int32 elementGroupIndex = 0;

	int32 startGroupIndex = static_cast<int32>(ESortFilterGroupColumn::Section00Id);
	int32 endGroupIndex = static_cast<int32>(ESortFilterGroupColumn::Section00ElementID);
	int32 gapIndex = (endGroupIndex - startGroupIndex) + 1;

	int32 beginIndex = static_cast<int32>(ESortFilterGroupColumn::Section00Id);
	int32 endIndex = static_cast<int32>(ESortFilterGroupColumn::SectionMaxId);
	for (int32 index = beginIndex; index < endIndex; index += gapIndex)
	{
		//. 소트필터 그룹 테이블의 섹션 아이디를 먼저 읽어옵니다.
		int32 sectionGroupNameId = sortFilterGroupTable.GetColumnData<int32>(index);

		//. 비어있는 섹션은 생성하지 않습니다.
		if (sectionGroupNameId <= 0)
		{
			continue;
		}

		//. 섹션 요소 아이디 리스트를 획득합니다.
		TArray<int32> sectionElementIdIdList = sortFilterGroupTable.GetColumnData<TArray<int32>>(index + 1);

		//. 섹션 데이터를 구성합니다.
		FTESFilterElementBoxGroupInitData elementBoxGroupInitData = {};
		elementBoxGroupInitData._elementGroupIndex = elementGroupIndex;
		elementBoxGroupInitData._contentsType = contentsType;
		elementBoxGroupInitData._categoryType = categoryType;
		elementBoxGroupInitData._elementGroupNameId = sectionGroupNameId;
		elementBoxGroupInitData._elementIdList = sectionElementIdIdList;
		elementBoxGroupInitData._applyfilterData = _currentFilterData;

		//. 섹션 위젯을 세팅합니다.
		UTESUWFilterElementBoxGroup* elementBoxGroupWidget = nullptr;
		elementBoxGroupWidget = SharedUIManager->CreateCtrWidget<UTESUWFilterElementBoxGroup>(TEXT("WBP_FilterElementBoxGroup"), TEXT("Common/WBP/Filter"));
		if (false == ::IsValid(elementBoxGroupWidget))
		{
			TESLOG(Error, TEXT("Invalid SectionGroupWidget."));

			continue;
		}

		elementBoxGroupWidget->InitWidget();
		elementBoxGroupWidget->OpenWidget();
		elementBoxGroupWidget->SetWidget(elementBoxGroupInitData);

		//. 박스 그룹 위젯을 해당 카테고리에 추가합니다.
		AddElementBoxGroupWidget(categoryType, elementBoxGroupWidget);

		//. 해당 스크롤 박스에 생성한 위젯을 붙입니다.
		_scrb_contentsMap[categoryType]->AddChild(elementBoxGroupWidget);

		//. 그룹 인덱스를 증가 시켜줍니다.
		++elementGroupIndex;
	}
}

void UTESUWFilter::SetFilterCategoryButton(ETypeSortFilterCategory categoryType, bool isSelected)
{
	if (true == _cts_filterCategoryButtonMap.Contains(categoryType))
	{
		_cts_filterCategoryButtonMap[categoryType]->SetToggleState(isSelected);
	}
}

void UTESUWFilter::SetFilterContentsVisiblity(ETypeSortFilterCategory categoryType, bool isVisibled)
{
	//. 카테고리에 대한 컨텐츠가 생성되지 않은 경우에는 무시합니다.
	if (false == _scrb_contentsMap.Contains(categoryType))
	{
		return;
	}

	//. 카테고리에 대한 컨텐츠가 생성되었으나, 유효하지 않은 경우에는 무시합니다.
	if (false == ::IsValid(_scrb_contentsMap[categoryType]))
	{
		return;
	}

	//. 카테고리에 대한 컨텐츠를 보여줍니다.
	_scrb_contentsMap[categoryType]->SetVisibility(isVisibled? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

	//. 맨 처음으로... :)
	_scrb_contentsMap[categoryType]->ScrollToStart();
}

void UTESUWFilter::SetFilterContentsElementBox(ETypeSortFilterCategory categoryType, int32 elementId, bool isSelected)
{
	if (true == _elementBoxWidgetMap.Contains(categoryType))
	{
		TMap<int32, class UTESUWFilterElementBox*> elementBoxWidgetMap = _elementBoxWidgetMap[categoryType]._elementBoxWidgetMap;
		if (true == elementBoxWidgetMap.Contains(elementId))
		{
			if (true == ::IsValid(elementBoxWidgetMap[elementId]))
			{
				elementBoxWidgetMap[elementId]->SetCheckedState(isSelected);
				elementBoxWidgetMap[elementId]->SetVisibility(isSelected ? ESlateVisibility::HitTestInvisible : ESlateVisibility::SelfHitTestInvisible);
			}
		}
	}
}

void UTESUWFilter::SetSortingOrderButton(ETESFilterSortingOrder sortingOrder, bool isSelected)
{
	if (ETESFilterSortingOrder::None == sortingOrder)
	{
		return;
	}

	if (true == _cts_sortingOrderButtonMap.Contains(sortingOrder))
	{
		_cts_sortingOrderButtonMap[sortingOrder]->SetToggleState(isSelected);
	}
	else
	{
		TESLOG(Error, TEXT("Not Found Type : %d"), sortingOrder);
	}
}

void UTESUWFilter::AddElementBoxGroupWidget(ETypeSortFilterCategory category, UTESUWFilterElementBoxGroup* elementBoxGroupWidget)
{
	if (false == ::IsValid(elementBoxGroupWidget))
	{
		TESLOG(Error, TEXT("Invalid ElementBoxGroupWidget."));

		return;
	}

	if (true == _elementBoxGroupWidgetMap.Contains(category))
	{
		_elementBoxGroupWidgetMap[category]._elementBoxGroupWidgets.Add(elementBoxGroupWidget);
	}
	else
	{
		FTESFilterElementBoxGroupWidgetArray elementBoxGroupWidgetArray = {};

		elementBoxGroupWidgetArray._elementBoxGroupWidgets.Add(elementBoxGroupWidget);

		_elementBoxGroupWidgetMap.Add(category, elementBoxGroupWidgetArray);
	}
}

void UTESUWFilter::AddElementBoxWidget(ETypeSortFilterCategory category, UTESUWFilterElementBox* elementBoxWidget)
{
	if (false == ::IsValid(elementBoxWidget))
	{
		TESLOG(Error, TEXT("Invalid ElementBoxWidget."));

		return;
	}

	int32 elementId = elementBoxWidget->GetElementId();

	if (true == _elementBoxWidgetMap.Contains(category))
	{	
		_elementBoxWidgetMap[category]._elementBoxWidgetMap.Add(elementId, elementBoxWidget);
	}
	else
	{
		FTESFilterElementBoxWidgetMap elementBoxWidgetMap = {};
		elementBoxWidgetMap._elementBoxWidgetMap.Add(elementId, elementBoxWidget);

		_elementBoxWidgetMap.Add(category, elementBoxWidgetMap);
	}
}

void UTESUWFilter::AddFilteringData(int32 elementGroupIndex, int32 elementId)
{
	if (true == _currentFilterData._filteringData.Contains(elementGroupIndex))
	{
		_currentFilterData._filteringData[elementGroupIndex]._elementIdList.Add(elementId);
	}
	else
	{
		FTESFilterFilteringGroupData filteringGroupData = {};
		filteringGroupData._elementGroupIndex = elementGroupIndex;
		filteringGroupData._elementIdList.Add(elementId);

		_currentFilterData._filteringData.Add(elementGroupIndex, filteringGroupData);
	}
}

void UTESUWFilter::RemoveFilteringData(int32 elementGroupIndex, int32 elementId)
{
	//. 필터링하고 있는 데이터가 없습니다.
	if (false == _currentFilterData._filteringData.Contains(elementGroupIndex))
	{
		return;
	}

	//. 해당 그룹에 대한 필터링 그룹을 찾고, 그룹에 있는 아이디 리스트에서 제거합니다.
	_currentFilterData._filteringData[elementGroupIndex]._elementIdList.Remove(elementId);

	//. 해당 그룹에 대한 필터링 리스트가 모두 제거되면 그룹 데이터 자체를 제거합니다.
	if (_currentFilterData._filteringData[elementGroupIndex]._elementIdList.Num() <= 0)
	{
		_currentFilterData._filteringData.Remove(elementGroupIndex);
	}
}

void UTESUWFilter::ResetFilterData(FTESFilterData filterData)
{
	for (TPair<ETypeSortFilterCategory, FTESFilterElementBoxGroupWidgetArray> child : _elementBoxGroupWidgetMap)
	{
		FTESFilterElementBoxGroupWidgetArray elementBoxGroupWidgetArray = child.Value;
		for (UTESUWFilterElementBoxGroup* elementBoxGroupWidget : elementBoxGroupWidgetArray._elementBoxGroupWidgets)
		{
			if (true == ::IsValid(elementBoxGroupWidget))
			{
				elementBoxGroupWidget->ResetFilterData(filterData);
			}
		}
	}
}

void UTESUWFilter::ChangeFilterCategory(ETypeSortFilterCategory filterCategory)
{
	if (filterCategory == _currentFilterCategory)
	{
		return;
	}

	//. 카테고리의 상태값을 변경합니다.
	_previousFilterCategory = _currentFilterCategory;
	_currentFilterCategory = filterCategory;

	//. 이전에 선택되어 있던 카테고리의 체크를 해제해 줍니다.
	SetFilterCategoryButton(_previousFilterCategory, false);

	//. 이번에 선택되어야 하는 카테고리를 체크 상태로 변경합니다.
	SetFilterCategoryButton(_currentFilterCategory, true);

	//. 이전에 보여줬던 컨텐츠를 "OFF" 시킵니다.
	SetFilterContentsVisiblity(_previousFilterCategory, false);

	//. 이번에 보여줬던 컨텐츠를 "ON" 시킵니다.
	SetFilterContentsVisiblity(_currentFilterCategory, true);

	//. 정렬 인덱스를 현재 필터의 값으로 세팅합니다. (단, 정렬 탭일 경우에만요... :))
	ChangeFilterSortingType(_currentFilterCategory);
}

void UTESUWFilter::ChangeFilterSortingType(ETypeSortFilterCategory filterCategory)
{
	if (ETypeSortFilterCategory::Sort == filterCategory)
	{
		_previousSortingElementId = TESConstantValue::NoneValue;
		_currentSortingElementId = TESConstantValue::NoneValue;

		ChangeFilterSortingType(_currentFilterData._sortingElementId);
	}	
}

void UTESUWFilter::ChangeFilterSortingType(int32 sortingElementId)
{
	if (_currentSortingElementId == sortingElementId)
	{
		return;
	}

	//. 타입을 교체 해줍니다.
	_previousSortingElementId = _currentSortingElementId;
	_currentSortingElementId = sortingElementId;

	//. 이전에 체크되어 있던 박스를 "OFF"합니다.
	SetFilterContentsElementBox(ETypeSortFilterCategory::Sort, _previousSortingElementId, false);

	//. 이번에 체크되어야 하는 박스를 "ON"합니다.
	SetFilterContentsElementBox(ETypeSortFilterCategory::Sort, _currentSortingElementId, true);

	//. 정렬 아이디를 교체해줍니다.
	_currentFilterData._sortingElementId = sortingElementId;
}

void UTESUWFilter::ChangeFilterSortingOrder(ETESFilterSortingOrder sortingOrder)
{
	if (sortingOrder == _currentSortingOrder)
	{
		return;
	}

	//. 아이디 값을 변경합니다.
	_previousSortingOrder = _currentSortingOrder;
	_currentSortingOrder = sortingOrder;

	//. 이전 것을 "OFF"
	SetSortingOrderButton(_previousSortingOrder, false);

	//. 새 것을 "ON"
	SetSortingOrderButton(_currentSortingOrder, true);

	//. 데이터 교체.
	_currentFilterData._sortingOrder = _currentSortingOrder;
}

bool UTESUWFilter::IsFiltering() const
{
	FTESFilterData tempfilterData = _currentFilterData;
	tempfilterData = TESUtilFilter::ResetFilterData(_currentFilterData._contentsType);

	//. 정렬이 기본 정렬과 다르다면 필터링 되고 있는 상태.
	if (tempfilterData._sortingElementId != _currentFilterData._sortingElementId)
	{
		return true;
	}
	
	//. 필터링 리스트가 하나라도 존재하면 필터링 되고 있는 상태.
	if (_currentFilterData._filteringData.Num() > 0)
	{
		return true;
	}

	//. 정렬 순서를 초기상태를 다르게 했다면 필터링되고 있는 상태.
	if (tempfilterData._sortingOrder != _currentFilterData._sortingOrder)
	{
		return true;
	}

	//. 초기 상태와 전부 동일하다면 필터링 되고 있지 않은 상태.
	return false;
}

void UTESUWFilter::OnEvent_SortingTypeChanged(int32 sortingElementId)
{
	ChangeFilterSortingType(sortingElementId);
}

void UTESUWFilter::OnEvent_FilterElementBoxAppend(ETypeSortFilterCategory categoryType, UTESUWFilterElementBox* elementBoxWidget)
{
	AddElementBoxWidget(categoryType, elementBoxWidget);
}

void UTESUWFilter::OnEvent_FilteringDataAdded(int32 elementGroupIndex, int32 elementId)
{
	AddFilteringData(elementGroupIndex, elementId);
}

void UTESUWFilter::OnEvent_FilteringDataRemoved(int32 elementGroupIndex, int32 elementId)
{
	RemoveFilteringData(elementGroupIndex, elementId);
}

void UTESUWFilter::OnEvent_FilterCategoryChanged(uint8 filterCategoryId)
{
	ETypeSortFilterCategory filterCategory = static_cast<ETypeSortFilterCategory>(filterCategoryId);
	ChangeFilterCategory(filterCategory);
}

void UTESUWFilter::OnEvent_SortingOrderChanged(ETESFilterSortingOrder sortingOrder)
{
	ChangeFilterSortingOrder(sortingOrder);
}

void UTESUWFilter::OnEvent_ResetFilter()
{
	//. 필터 팝업 내에서 사용하는 필터 데이터만 적용합니다.
	//. ( 세이브 데이터로 저장하지 않음. )
	_currentFilterData = TESUtilFilter::ResetFilterData(_currentFilterData._contentsType);

	//. 해당 필터 요소 그룹들을 필터 데이터로 갱신합니다.
	ResetFilterData(_currentFilterData);

	//. 정렬 타입도 초기화 타입에 맞게 교체해줍니다. ( 단, 정렬 카테고리 일 경우에만!! )
	ChangeFilterSortingType(_currentFilterCategory);

	//. 정렬 순서 체크버튼들도 다시 세팅합니다.
	ChangeFilterSortingOrder(_currentFilterData._sortingOrder);
}

void UTESUWFilter::OnEvent_Apply()
{
	if (true == IsClosingContents())
	{
		return;
	}

	//. 필터링 중입니다.
	_currentFilterData._isFiltering = IsFiltering();

	//. 해당 필터의 구성으로 진짜 필터 데이터에 반영합니다.
	SharedSaveGameManager->SetFilterData(_currentContentsType, _currentFilterData);

	//. 각 컨텐츠에게 필터 데이터가 적용되었음을 알립니다.
	FTESUtilEvents::OnCommonFilterApply.ExecuteIfBound(_currentFilterData);

	//. 필터 팝업을 닫습니다.
	DestroyWidget();
}

void UTESUWFilter::OnEvent_Close()
{
	//. 적용 없이 그냥 필터 팝업을 닫습니다.
	DestroyWidget();
}

void UTESUWFilter::OnEvent_ContentsBack()
{
	Super::OnEvent_ContentsBack();

	OnEvent_Close();
}
