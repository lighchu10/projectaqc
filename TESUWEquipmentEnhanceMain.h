// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TES.h"
#include "Structs/UI/TESUWGenericBase.h"
#include "TESGlobalStructs.h"
#include "TESUWEquipmentEnhanceMain.generated.h"

/**
 * [ Note ] 아이템 인벤토리의 상세보기 패널의 주 카테고리
 */
UENUM(BlueprintType)
enum class ETESEquipmentInventoryView : uint8
{
	None = 0,	//. 아무것도 아님
	Empty = 1,	//. 비어있음
	Equip = 2,	//. 장비
	Rune = 3,	//. 룬
	Misc = 4,	//. 기타
	BulkSale = 5,	//. 일괄판매
	Max			UMETA(Hidden)
};

USTRUCT()
struct FTESEnhanceInventoryCategoryItemData
{
	GENERATED_BODY()

public:
	//. 해당 카테고리에 대한 아이디 값.
	UPROPERTY() ETESItemInventoryMainCategory _category = ETESItemInventoryMainCategory::None;

	//. 해당 카테고리가 가지고 있는 아이템 데이터의 맵.
	UPROPERTY() TMap<int32, class UTESGDGoodsItem*> _itemDataMap = {};
};

UCLASS()
class TES_API UTESUWEquipmentEnhanceMain : public UTESUWGenericBase
{
	GENERATED_BODY()

//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Override Functions ] 
//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
public:
	virtual void InitWidget() override;
	virtual void ClearWidget() override;
	virtual void OpenWidget() override;
	virtual void CloseWidget() override;
	virtual void OpenDelegate() override;
	virtual void CloseDelegate() override;

//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Normal Functions ] 
//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	void InitSelectedEquipmentIcon();
	void InitSelectedEquipmentName();
	void InitSelectedEquipmentGrade();
	void InitSelectedEquipmentStatNameList();
	void InitSelectedEquipmentStatBeforeValueList();
	void InitSelectedEquipmentStatAfterValueList();
	void InitSelectedEquipmentStatCompareValueList();
	void InitRootEquipName();
	void InitNotSelectedText();
	void InitWarningText();
	void InitRootStatIncrease();
	void InitRequiredMaterialValue();
	void InitStatArrow();
	void InitCommonHeader();
	void InitEnhanceButton();
	void InitSFTYDisplayer();
	void InitManaringDisplayer();
	void InitRootMaxTag();

private:
	void InitContentsInventoryView();
	void InitContentsInventoryViewSwitcher();
	void InitContentsInventoryViewContainer();

private:
	void InitItemFilter();
	void InitItemFilterSwitcher();
	void InitItemFilterButtonText();
	void InitItemFilterSortingOrderButton();

private:
	void ClearSFTYDisplayer();
	void ClearManaringDisplayer();
	void ClearCommonHeader();

public:
	void SetWidget(class UTESGDGoodsItem* itemData);

private:
	bool SetData(class UTESGDGoodsItem* itemData);
	void SetDataByFillter();
	void SetCategoryItemData(UTESGDGoodsItem* itemGoodsData);
	void SetCategoryItemDataList(TArray<class UTESGDGoodsItem*> itemDataList);
	void AddCategoryItemData(ETESItemInventoryMainCategory categoryType, class UTESGDGoodsItem* itemData);
	void AddCategoryItemDataList(ETESItemInventoryMainCategory categoryType, TArray<class UTESGDGoodsItem*> itemDatas);
	void RemoveCategoryItemData(ETESItemInventoryMainCategory categoryType, class UTESGDGoodsItem* itemData);

private:
	class UTESGDGoodsItem* GetItemCategoryData(ETESItemInventoryMainCategory categoryType, int64 itemUnique);
	TArray<class UTESGDGoodsItem*> GetItemCategoryDatas(ETESItemInventoryMainCategory categoryType);

private:
	void SetSelectedEquipmentIcon();
	void SetSelectedEquipmentName();
	void SetSelectedEquipmentGrade();
	void SetSelectedEquipmentStatName();
	void SetSelectedEquipmentStatBeforeValue();
	void SetSelectedEquipmentStatAfterValue();
	void SetSelectedEquipmentStatCompareValue();
	void SetEquipNameVisible(bool isVisible);
	void SetNotSelectedTextVisible(bool isVisible);
	void SetWarningText(int32 tableId, int32 levelIndex);
	void SetRootStatIncreaseVisible(bool isVisible);
	void SetRequiredMaterialValue();
	void SetRequiredMaterialValue(int32 tableId, int32 levelIndex);
	void SetEnhanceButtonDisable(bool isEnabled);
	void SetMaxTagVisible();

private:
	void SetCommonHeader();

private:
	void SetContentsEquipInventory();

private:
	void SetContentsInventoryView(TArray<class UTESGDGoodsItem*> itemDataListbool, bool bUseScrollToTop = true);
	void SetContentsInventoryViewSwitcher(bool isEmptied);
	void SetContentsInventoryViewContainer(TArray<UTESGDGoodsItem*> itemData, bool bUseScrollToTop = true);

private:
	void SetItemFilterContents(ETESItemInventoryMainCategory categoryType);
	void SetContentsInventoryFilter(FTESFilterData filterData);
	void SetContentsInventoryFilterSwitcher(bool isOn);
	void SetContentsInventoryFilterButtonText(FTESFilterData filterData);
	void SetContentsInventoryFilterButtonText(bool isOn, int32 buttonTextId);
	void SetContentsInventoryFilterSortingOrderButton(FTESFilterData filterData);
	void SetContentsInventoryFilterSortingOrderButton(bool isOn, ETESFilterSortingOrder sortingOrder);

public:
	void SetOpeningMessageBox(bool isOpening);

private:
	void RefreshEquipmentInfo(UTESGDGoodsItem* itemData);
	void RefreshEquipment(bool haveItem);

private:
	bool CheckIfEquipmentMaxLevel();

public:
	void PlayHideAnimation();
	void PlayShowAnimation();

private:
	TArray<class UTESGDGoodsItem*> MakeFilteredItemDataList(TArray<class UTESGDGoodsItem*> itemDataList
															, TMap<int32, FTESFilterFilteringGroupData> filteringData
															, bool isFiltering);

	TArray<class UTESGDGoodsItem*> MakeSortedItemDataList(TArray<class UTESGDGoodsItem*> itemDataList
															, int32 sortingElementId
															, ETESFilterSortingOrder sortingOrder);

	//. 특정 캐릭터의 아이디를 넣어주면 특정 장착 장비 중에 해당 캐릭터가 사용중인 아이템을 제외하고 리스트를 생성하여 돌려줍니다.
	TArray<class UTESGDGoodsItem*> MakeIgnoreItemDataListByUsingCharacter(TArray<class UTESGDGoodsItem*> itemDataList, int64 ignoreItemUsingCharacterUnique);

//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ OnEvent Functions ] 
//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_InventoryView_EntryInitailized(UObject* item, UUserWidget* widget);

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_InventoryView_EntryReleased(UUserWidget* widget);

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_InventoryView_EntryClicked(UObject* item);

	//. 필터 열기
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_OpenItemFilter();

	//. 필터 적용
	UFUNCTION()
	void OnEvent_ApplyEquipFilter(struct FTESFilterData filterData);

	UFUNCTION()
	void OnEvent_EnhanceItem(bool enhanceResult, FTESEnhanceItemInfo itemInfo);

	//. 필터 정렬 순서 변경
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_ChangeSotingOrder();

    UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
    void OnEvent_Home();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_Back();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_Enhance();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_EnhanceDisable();

	UFUNCTION()
	void OnEvent_ItesmSlotClicked(class UTESGDGoodsItem* selectedItemData);

//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Member Variables ] 
//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	//. 현재 아이템 데이터입니다.
	UPROPERTY() class UTESGDGoodsItem* _currentItemData = nullptr;
	UPROPERTY() int64 _currentItemUnique = 0;				//. 해당 아이템 유니크
	UPROPERTY() int32 _currentItemId = 0;					//. 해당 아이템 아이디(테이블)
	UPROPERTY() int32 _requiredMaterialValue = 0;			//. 해당 아이템 강화에 필요한 마나링 수치

	//. 선택된 아이템의 인덱스입니다.
	UPROPERTY() int64 _selectedItemUnique = 0;

	//. 현재 필터링 되고 있는 필터 데이터입니다.
	UPROPERTY() struct FTESFilterData _currentFilterData = {};

private:
	//. 인벤토리뷰에서 현재 아이템 슬롯 리스트의 데이터들입니다.
	UPROPERTY()
	TMap<int64, class UTESEDCharInfoContentsItemSlot*> _currentInventoryItemSlotDatas = {};

	UPROPERTY() TMap<ETESItemInventoryMainCategory, struct FTESEnhanceInventoryCategoryItemData> _categoryItemDataMap = {};

private:
	//. 메세지 박스 위젯을 열고 있는 중인가요?
	UPROPERTY() bool _isOpeningMessageBox = false;

	////. 위젯을 닫고 있는 중인가요?
	//UPROPERTY() bool _isClosingWidget = false;

private:
	//. 아이템 강화후 대한 델리게이트 핸들입니다.
	FDelegateHandle _dh_itemEnhancedCompletely = {};

#pragma region UMG Widget
private:
	//. 강화된 수치들
	UPROPERTY() TArray<class UTESTextBlock*> _txt_statNameList = {};		//. ex)레벨, 공격력, 방어력, 전투력 등
	UPROPERTY() TArray<class UTESTextBlock*> _txt_statBeforeValue = {};		//. 강화하기 전 수치(현재 수치)
	UPROPERTY() TArray<class UTESTextBlock*> _txt_statAfterValue = {};		//. 강화한 후 수치
	UPROPERTY() TArray<class UTESTextBlock*> _txt_StatCompareValue = {};	//. 증가된 수치
	UPROPERTY() TArray<class UTESImage*> _img_statArrowList = {};				//. 사이에 있는 화살표 이미지

private:
	//. 강화에 필요한 재화 수치
	UPROPERTY() class UTESTextBlock* _txt_requiredMaterialValue = nullptr;

private:
	UPROPERTY() class UScaleBox* _scb_statAfterValue = nullptr;
	UPROPERTY() class UScaleBox* _scb_statCompareValue = nullptr;

private:
	//. 강화할 아이템
	UPROPERTY()	class UImage* _img_selectedEquipment = nullptr;
	UPROPERTY()	class UImage* _img_selectedEquipmentGrade = nullptr;
	UPROPERTY() class UTESTextBlock* _txt_selectedEquipmentName = nullptr;

	UPROPERTY() class UCanvasPanel* _cp_rootEquipName = nullptr;
	UPROPERTY() class UVerticalBox* _cp_statIncrease = nullptr;
	UPROPERTY() class UCanvasPanel* _cp_notSelectedText = nullptr;
	UPROPERTY() class UCanvasPanel* _cp_warningMessage = nullptr;
	UPROPERTY() class UCanvasPanel* _cp_maxTag = nullptr;

private:
	//. 강화에서의 필터 스윗쳐입니다.
	UPROPERTY() class UTESUWCommonToggleSwitcher* _cts_itemFilterSwitcher = nullptr;

	//. 강화에서의 필터 텍스트 맵입니다.
	UPROPERTY() TMap<bool, class UTESTextBlock*> _txt_itemFilterTextMap = {};

	//. 강화에서의 필터 차순 버튼 맵입니다.
	UPROPERTY() TMap<bool, class UCheckBox*> _chkb_itemFilterSortingOrderMap = {};

	//. 강화 버튼 스위쳐입니다.
	UPROPERTY() class UTESUWCommonToggleSwitcher* _cts_enhanceButton = nullptr;

private:
	UPROPERTY() class UTESUWCommonHeader* _wbp_commonHeader = nullptr;

private:
	UPROPERTY() class UTESUWGoodsDisplayer* _wbp_sftyDisplayerWidget = nullptr;
	UPROPERTY() class UTESUWGoodsDisplayer* _wbp_manaringDisplayerWidget = nullptr;

private:
	//. 장비 인벤토리 컨텐츠에 대한 컨텐츠 스윗쳐 입니다. (0 : 아이템 인벤토리, 1 : 해당하는 장비 아이템이 없음 텍스트 )
	UPROPERTY() class UWidgetSwitcher* _ws_contentsInventoryViewSwitcher = nullptr;

	//. 장비 인벤토리 컨텐츠에 대한 타일뷰입니다.
	UPROPERTY() class UTESTileView* _tv_itemInventoryViewContainer = nullptr;
#pragma endregion UMG Widget
};
