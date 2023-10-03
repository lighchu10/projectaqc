// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TES.h"
#include "Structs/UI/TESUWPopupBase.h"
#include "TDB/Type_SortFilterContents.h"
#include "TDB/Type_SortFilterCategory.h"
#include "TESUWFilter.generated.h"

/**
 * [ Note ] 필터 팝업에서 사용하는 "요소 박스 그룹" 위젯의 배열 타입
 *          ( 언리얼은 다중 배열을 지원하지 않습니다. )
 */
USTRUCT()
struct FTESFilterElementBoxGroupWidgetArray
{
	GENERATED_BODY()

public:
	//. 필터 카테고리에 대한 필터 박스 그룹 위젯의 리스트
	UPROPERTY() TArray<class UTESUWFilterElementBoxGroup*> _elementBoxGroupWidgets = {};
};

/**
 * [ Note ] 필터 팝업에서 사용하는 "요소 박스" 위젯의 맵 타입
 *          ( 언리얼은 다중 배열을 지원하지 않습니다. )
 */
USTRUCT()
struct FTESFilterElementBoxWidgetMap
{
	GENERATED_BODY()

public:
	//. 필터 카테고리에 대한 필터 요소 박스 위젯의 맵
	UPROPERTY() TMap<int32, class UTESUWFilterElementBox*> _elementBoxWidgetMap = {};
};

/**
 * 
 */
UCLASS()
class TES_API UTESUWFilter : public UTESUWPopupBase
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

//. ──────────────────────────────────────────────────────────────────────────────────────────────
//. [ Normal Functions ] 
//. ──────────────────────────────────────────────────────────────────────────────────────────────	
private:
	void InitFilterCategories();
	void InitFilterContents();
	void InitSortingOrderButtons();

public:
	//. 최초 진입지점입니다.
	void SetWidget(ETypeSortFilterContents contentsType);

private:
	//. 필터 팝업에서 필요로하는 데이터를 세팅하는 함수입니다.
	bool SetData(ETypeSortFilterContents contentsType);

private:
	//. 필터 팝업의 컨텐츠에 대한 위젯들을 생성합니다.
	void SetFilterContents(ETypeSortFilterContents contentsType);

	//. 필터 팝업의 컨텐츠에 대한 요소 그룹 위젯들을 생성합니다.
	void SetFilterContentsElementGroup(ETypeSortFilterContents contentsType
									 , ETypeSortFilterCategory categoryType
									 , FSortFilterGroupRow sortFilterGroupTable);

private:
	//. 필터 팝업에서 카테고리 버튼의 체크상태를 세팅해주는 함수입니다.
	void SetFilterCategoryButton(ETypeSortFilterCategory categoryType, bool isSelected);

	//. 필터 팝업에서 컨텐츠의 보임 여부를 세팅해주는 함수입니다.
	void SetFilterContentsVisiblity(ETypeSortFilterCategory categoryType, bool isVisibled);

	//. 필터 팝업에서 필터 요소 버튼을 세팅해주는 함수입니다.
	void SetFilterContentsElementBox(ETypeSortFilterCategory categoryType, int32 elementId, bool isSelected);

	//. 필터 팝업에서 정령 차순 버튼을 세팅해주는 함수입니다.
	void SetSortingOrderButton(ETESFilterSortingOrder sortingOrderBy, bool isSelected);

private:
	//. 필터 팝업에서 해당 카테고리의 필터 요소 박스 그룹 위젯 맵에 해당 위젯을 등록합니다.
	void AddElementBoxGroupWidget(ETypeSortFilterCategory category, class UTESUWFilterElementBoxGroup* elementBoxGroupWidget);

	//. 필터 팝업에서 해당 카테고리의 필터 요소 박스 위젯 맵에 해당 위젯을 등록합니다.
	void AddElementBoxWidget(ETypeSortFilterCategory category, class UTESUWFilterElementBox* elementBoxWidget);

private:
	//. 필터링 목록으로 데이터를 추가해주는 함수입니다.
	void AddFilteringData(int32 elementGroupIndex, int32 elementId);

	//. 필터링 목록에 있는 데이터를 제거해주는 함수입니다.
	void RemoveFilteringData(int32 elementGroupIndex, int32 elementId);

private:
	//.
	void ResetFilterData(FTESFilterData filterData);

private:
	//. 카테고리를 변경해주는 함수입니다.
	void ChangeFilterCategory(ETypeSortFilterCategory filterCategory);

	void ChangeFilterSortingType(ETypeSortFilterCategory filterCategory);
	void ChangeFilterSortingType(int32 sortingElementId);
	void ChangeFilterSortingOrder(ETESFilterSortingOrder sortingOrderBy);

private:
	bool IsFiltering() const;

//. ──────────────────────────────────────────────────────────────────────────────────────────────
//. [ OnEvent Functions ] 
//. ──────────────────────────────────────────────────────────────────────────────────────────────
private:
	UFUNCTION()
	void OnEvent_SortingTypeChanged(int32 sectionElementId);

	UFUNCTION()
	void OnEvent_FilterElementBoxAppend(ETypeSortFilterCategory categoryType, class UTESUWFilterElementBox* filterElementBoxWidget);

	UFUNCTION()
	void OnEvent_FilteringDataAdded(int32 elementGroupIndex, int32 elementId);

	UFUNCTION()
	void OnEvent_FilteringDataRemoved(int32 elementGroupIndex, int32 elementId);

private:
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true", ToolTip = "FilterCategoryId (1 : Filter, 2 : Sort)"))
	void OnEvent_FilterCategoryChanged(uint8 filterCategoryId);

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_SortingOrderChanged(ETESFilterSortingOrder sortingOrder);

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_ResetFilter();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_Apply();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_Close();

//. ──────────────────────────────────────────────────────────────────────────────────────────────
//. [ Override OnEvent Functions ] 
//. ──────────────────────────────────────────────────────────────────────────────────────────────
public:
	virtual void OnEvent_ContentsBack() override;

//. ──────────────────────────────────────────────────────────────────────────────────────────────
//. [ Member Variables ] 
//. ──────────────────────────────────────────────────────────────────────────────────────────────
private:
	//. 필터 팝업의 컨텐츠 타입입니다.
	ETypeSortFilterContents	_currentContentsType = ETypeSortFilterContents::None;

	//. 이전에 선택된 카테고리의 아이디값입니다.
	ETypeSortFilterCategory _previousFilterCategory = ETypeSortFilterCategory::None;

	//. 이번에 선택된 카테고리의 아이디값입니다.
	ETypeSortFilterCategory _currentFilterCategory = ETypeSortFilterCategory::None;

	//. 이전에 선택된 정렬 차순의 아이디값입니다. 
	ETESFilterSortingOrder _previousSortingOrder = ETESFilterSortingOrder::None;

	//. 이번에 선택된 정렬 차순의 아이디값입니다.
	ETESFilterSortingOrder _currentSortingOrder = ETESFilterSortingOrder::None;

	//. 이전에 정렬값으로 지정되었던 아이디 값입니다.
	int32 _previousSortingElementId = 0;

	//. 이번에 정렬값으로 지정되었던 아이디 값입니다.
	int32 _currentSortingElementId = 0;

private:
	//. 현재의 필터 데이터입니다. ( 필터 팝업 내에서 사용하는 필터데이터입니다!! )
	UPROPERTY() FTESFilterData	_currentFilterData = {};

private:
	//. 카테고리에 대한 체크 버튼들
	UPROPERTY() TMap<ETypeSortFilterCategory, class UTESUWCommonToggleSwitcher*> _cts_filterCategoryButtonMap = {};

	//. 카테고리에 대한 스크롤 박스들
	UPROPERTY() TMap<ETypeSortFilterCategory, class UScrollBox*> _scrb_contentsMap = {};

	//. 카테고리에 대한 필터 그룹 위젯 맵
	UPROPERTY() TMap<ETypeSortFilterCategory, struct FTESFilterElementBoxGroupWidgetArray> _elementBoxGroupWidgetMap = {};

	//. 카테고리에 대한 필터 박스 위젯 맵
	UPROPERTY() TMap<ETypeSortFilterCategory, struct FTESFilterElementBoxWidgetMap> _elementBoxWidgetMap = {};
	//UPROPERTY() TMap<int32, class UTESUWFilterElementBox*> _filterElementBoxMap = {};

	//. 정렬 차순에 대한 체크 버튼들
	UPROPERTY() TMap<ETESFilterSortingOrder, class UTESUWCommonToggleSwitcher*> _cts_sortingOrderButtonMap = {};

	//. 카테고리에 대한 컨텐츠 변경을 해주는 스윗쳐입니다.
	UPROPERTY() class UWidgetSwitcher* _ws_contentsSwitcher = nullptr;
//. ──────────────────────────────────────────────────────────────────────────────────────────────
};
