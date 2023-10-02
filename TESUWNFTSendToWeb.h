// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TES.h"
#include "Structs/UI/TESUWGenericBase.h"
#include "TESUWNFTSendToWeb.generated.h"

/**
 * [ Note ] 인벤토리의 주 카테고리
 */
UENUM(BlueprintType)
enum class ETESInventoryMainCategory : uint8
{
	None,		//. 아무것도 아님
	Character,	//. 캐릭터
	Equip,		//. 장비
	Rune,		//. 룬
	Max		UMETA(Hidden)
};

/**
 * [ Note ] 아이템 인벤토리에서 사용하는 카테고리별 아이템 데이터
 */
USTRUCT()
struct FTESInventoryCategoryItemData
{
	GENERATED_BODY()

public:
	//. 해당 카테고리에 대한 아이디 값.
	UPROPERTY() ETESItemInventoryMainCategory _category = ETESItemInventoryMainCategory::None;

	//. 해당 카테고리가 가지고 있는 아이템 데이터의 맵.
	UPROPERTY() TMap<int32, class UTESGDGoodsItem*> _itemDataMap = {};
};

UCLASS()
class TES_API UTESUWNFTSendToWeb : public UTESUWGenericBase
{
	GENERATED_BODY()

//. ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Override Functions ] 
//. ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────	
public:
	virtual void InitWidget() override;
	virtual void ClearWidget() override;
	virtual void OpenWidget() override;
	virtual void CloseWidget() override;
	virtual void OpenDelegate() override;
	virtual void CloseDelegate() override;

//. ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Normal Functions ] 
//. ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	void InitCommonHeader();

private:
	void InitMainCategories();

private:
	void InitCharacterPanel();
	void InitCharacterProfile();
	void InitCharacterPortrait();
	void InitCharacterName();

private:
	void InitFilter();
	void InitFilterSwitcher();
	void InitFilterButtonText();
	void InitSortingOrderButton();

private:
	void InitCharacterRoleIcon();
	void InitCharacterRoleText();
	void InitCharacterAttackTypeIcon();
	void InitCharacterAttackTypeText();
	void InitCharacterRangeIcon();
	void InitCharacterRangeText();
	void InitCharacterSpecialityIcon();
	void InitCharacterSpecialityText();

private:
	void InitCharacterLevelValue();
	void InitCharacterAwakeningValue();
	void InitCharacterAwakeningRemainValue();

private:
	void InitCharacterGrade();
	void InitCharacterPrestigeEmblem();

private:
	void InitItemInfoPanel();
	void InitItemInfo();
	void InitItemName();
	void InitItemIcon();
	void InitItemRequiredLevel();
	void InitItemRequiredStat();
	void InitItemOptionPanels();
	void InitItemOptionNames();
	void InitItemOptionsValue();

private:
	void InitInventoryViewSwitcher();
	void InitInventoryView();
	void InitCharacterInventoryViewContainer();
	void InitItemInventoryViewContainer();

private:
	void InitCharacterUnlockButton();
	void InitCharacterClearButton();
	

private:
	void InitItemUnlockButton();
	void InitItemClearButton();

private:
	void InitSendToWebButton();

private:
	void ClearCommonHeader();
	void ClearCharacterInventoryViewContainer();
	void ClearItemInventoryViewContainer();
	void ClearSelectItem();

public:
	void SetWidget();

private:
	bool SetData();

private:
	void SetCommonHeader();

private:
	void SetCategoryItemData(UTESGDGoodsItem* itemGoodsData);
	void SetCategoryItemDatas(TArray<class UTESGDGoodsItem*> itemDataList);
	void AddCategoryItemData(ETESItemInventoryMainCategory categoryType, class UTESGDGoodsItem* itemData);
	void AddCategoryItemDatas(ETESItemInventoryMainCategory categoryType, TArray<class UTESGDGoodsItem*> itemDatas);
	void RemoveCategoryItemData(ETESItemInventoryMainCategory categoryType, class UTESGDGoodsItem* itemData);

private:
	void SetMainCategory(ETESInventoryMainCategory mainCategory, bool isForced = false);
	void SetMainCategoryButton(ETESInventoryMainCategory categoryType, bool isSelected);

private:
	void SetViewContents(ETESInventoryMainCategory categoryType);
	void SetCharacterViewContents();
	void SetCharacterInventoryViewContainer(TArray<class UTESGDCharacter*> characterDataList);
	void SetItemViewContents(ETESItemInventoryMainCategory categoryType);
	void SetItemViewContents(ETESItemInventoryMainCategory categoryType, TArray<class UTESGDGoodsItem*> itemDataList);
	void SetItemInventoryViewContainer(TArray<UTESGDGoodsItem*> itemDataList);

private:
	void SetCharacterProfile();
	void SetFilterContents();
	void SetFilterContents(FTESFilterData filterData);
	void SetFilterSwitcher(bool isOn);
	void SetFilterButtonText(FTESFilterData filterData);
	void SetFilterButtonText(bool isEnable, int32 buttonTextId);
	void SetFilterSortingOrderButton(FTESFilterData filterData);
	void SetFilterSortingOrderButton(bool isOn, ETESFilterSortingOrder sortingOrder);

private:
	void SetCharacterPanelVisible(bool isVisible);
	void SetCharacterPortrait(int32 characterPortraitId);
	void SetCharacterName(FString characterName);

private:
	void SetCharacterRole(ETypeCharRole roleType);
	void SetCharacterRoleIcon(ETypeCharRole roleType);
	void SetCharacterRoleText(ETypeCharRole roleType);

private:
	void SetCharacterAttackType(ETypeCharAttack attackType);
	void SetCharacterAttackTypeIcon(ETypeCharAttack attackType);
	void SetCharacterAttackTypeText(ETypeCharAttack attackType);

private:
	void SetCharacterRange(ETypeCharRange rangeType);
	void SetCharacterRangeIcon(ETypeCharRange rangeType);
	void SetCharacterRangeText(ETypeCharRange rangeType);

private:
	void SetCharacterSpeciality(ETypeCharSpeciality specialityType);
	void SetCharacterSpecialityIcon(ETypeCharSpeciality specialityType);
	void SetCharacterSpecialityText(ETypeCharSpeciality specialityType);

private:
	void SetCharacterLevelValue(int32 currentLevel, int32 maximumLevel);
	void SetCharacterAwakeningValue(int32 awakeningValue);
	void SetCharacterAwakeningRemainValue(int32 currentAwakenTryCount, int32 maximumAwakenTryCount);

private:
	void SetCharacterGrade(ETypeGrade gradeType);
	void SetCharacterPrestige(bool isPrestige);

private:
	void SetItemDetailInfo(class UTESGDGoodsItem* itemData);
	void SetItemInfoPanelVisible(bool isVisible);
	void SetItemOptionPanelVisible(int32 itemOptionIndex, bool isVisible);
	void SetItemName(FString itemNameString);
	void SetItemIcon(FString resourcePath);
	void SetItemMinimumRequiredLevelText();
	void SetItemMinimumRequiredStatValueList();
	void SetItemOptions();
	void SetItemOption(int32 itemOptionIndex
					,  ETypeStatOption itemOptionType
					,  int32 itemOptionValue
					,  bool isVisibled);

public:
	void RefreshCharacterUIButtons();

private:
	void SetSendButtonByCharacter();
	void SetSendButtonByCharacter(bool isActive);
	void SetCharacterUnlockButton();
	void SetCharacterUnlockButton(bool isActive);
	void SetCharacterClearButton();
	void SetCharacterClearButton(bool isActive);

private:
	void SetSendButtonByItem();
	void SetSendButtonByItem(bool isActive);
	void SetItemUnlockButton();
	void SetItemUnlockButton(bool isActive);
	void SetItemClearButton();
	void SetItemClearButton(bool isActive);

private:
	class UTESGDGoodsItem* GetItemCategoryData(ETESItemInventoryMainCategory categoryType, int64 itemUnique);
	TArray<class UTESGDGoodsItem*> GetItemCategoryDatas(ETESItemInventoryMainCategory categoryType);

private:
	void CloseInventoryView();

private:
	TArray<class UTESGDCharacter*> MakeFilteredCharacterDataList(TArray<class UTESGDCharacter*> source
																, TMap<int32, FTESFilterFilteringGroupData> filteringList
																, bool isFiltering);

	TArray<class UTESGDCharacter*> MakeSortedCharacterDataList(TArray<class UTESGDCharacter*> sourceCharacterDataList
																, int32 sortingData
																, ETESFilterSortingOrder sortingOrder);

private:
	TArray<class UTESGDGoodsItem*> MakeFilteredItemDataList(TArray<class UTESGDGoodsItem*> itemDataList
														, TMap<int32, FTESFilterFilteringGroupData> filteringData
														, bool isFiltering);

	TArray<class UTESGDGoodsItem*> MakeSortedItemDataList(TArray<class UTESGDGoodsItem*> itemDataList
														, int32 sortingElementId
														, ETESFilterSortingOrder sortingOrder);

//. ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ OnEvent Functions ] 
//. ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	UFUNCTION()
	void OnEvent_EndMinting();

private:
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_ChangeMainCategory(ETESInventoryMainCategory category);

	//. 캐릭터 인벤토리 View
private:
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_CharacterInventoryView_EntryInitailized(UObject* item, UUserWidget* widget);

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_CharacterInventoryView_EntryReleased(UUserWidget* widget);

	UFUNCTION()
	void OnEvent_CharacterInventorySlotClicked(class UTESEDNFTCharacterInventorySlot* characterSlotData);
	//. 아이템 인벤토리 View
private:
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_ItemInventoryView_EntryInitailized(UObject* item, UUserWidget* widget);

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_ItemInventoryView_EntryReleased(UUserWidget* widget);

	UFUNCTION()
	void OnEvent_ItemInventorySlotClicked(class UTESGDGoodsItem* itemSlotData);
	//. 필터
private:
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_Filter();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_ChangeSortingOrder();

	UFUNCTION()
	void OnEvent_FilterApply(struct FTESFilterData filterData);

private:
	//. 캐릭터 상세정보(캐릭터 정보창)
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_CharacterDetailView();

	//. 아이템 상세정보(아이템 인벤토리)
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_ItemDetailView();

	//. 캐릭터 잠금 해제 버튼
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_CharacterUnlock();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_CharacterUnlockComplete(bool isLock);

	//. 캐릭터 클리어 버튼
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_CharacterClear();

	//. 아이템 잠금 해제 버튼
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_ItemUnlock();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_ItemUnlockComplete(int64 itemUnique);

	//. 아이템 클리어 버튼
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_ItemClear();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_ItemClearComplete(TArray<class UTESGDGoodsItem*> itemDatas);

	//. 캐릭터를 웹으로 전송
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_SendToWebByCharacter();

	//. 아이템을 웹으로 전송
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_SendToWebByItem();

	//. 웹으로 이동
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_GoToWebSite();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_Back();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_Tutorial();
	
//. ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Member Variables ] 
//. ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	//. 민팅이 완료 되었을때
	FDelegateHandle _dh_endMintingObject = {};

	//. 아이템 탈착에 대한 딜리게이트 핸들입니다.
	FDelegateHandle _dh_itemUnEquipComplete = {};

	//. 아이템 락에 대한 딜리게이트 핸들입니다.
	FDelegateHandle _dh_itemLockComplete = {};

private:
	UPROPERTY() class UTESGDCharacter* _currentCharacterData = nullptr;
	UPROPERTY() class UTESGDGoodsItem* _currentItemData = nullptr;
	UPROPERTY()	FTESFilterData _currentFilterData = {};

private:
	//. 이전 카테고리 아이디 값.
	UPROPERTY() ETESInventoryMainCategory	_previousMainCategory = ETESInventoryMainCategory::None;

	//. 현재 카테고리 아이디 값.
	UPROPERTY() ETESInventoryMainCategory	_currentMainCategory = ETESInventoryMainCategory::None;

private:
	UPROPERTY() class UTESEDNFTCharacterInventorySlot* _prevSelectedSlot = nullptr;

private:
	//. 선택된 아이템의 인덱스입니다.
	UPROPERTY() int64 _selectedItemUnique = 0;

	//. 인벤토리뷰에서 현재 아이템 슬롯 리스트의 데이터들입니다.
	UPROPERTY()
	TMap<int64, class UTESEDCharInfoContentsItemSlot*> _currentInventoryItemSlotDatas = {};

	//. 캐릭터 클리어, 언락, 전송 버튼
private:
	UPROPERTY() class UTESUWCommonToggleSwitcher* _cts_unlockCharacterButton = nullptr;
	UPROPERTY() class UTESUWCommonToggleSwitcher* _cts_clearCharacterButton = nullptr;
	UPROPERTY() class UTESUWCommonToggleSwitcher* _cts_sendToWebButtonByCharacter = nullptr;

	//. 아이템 클리어, 언락, 전송 버튼
private:
	UPROPERTY() class UTESUWCommonToggleSwitcher* _cts_unlockItemButton = nullptr;
	UPROPERTY() class UTESUWCommonToggleSwitcher* _cts_clearItemButton = nullptr;
	UPROPERTY() class UTESUWCommonToggleSwitcher* _cts_sendToWebButtonByItem = nullptr;

#pragma region UMG Widget
private:
	UPROPERTY() class UTESUWCommonHeader* _wbp_commonHeader = nullptr;

private:
	UPROPERTY() TMap<ETESInventoryMainCategory, class UTESUWCommonToggleSwitcher*> _cts_mainCategoryMap = {};
	UPROPERTY() TMap<ETESItemInventoryMainCategory, struct FTESInventoryCategoryItemData> _categoryItemDataMap = {};

private:
	UPROPERTY() class UWidgetSwitcher* _ws_inventoryViewSwitcher = nullptr;	//. 캐릭터, 아이템 위젯 스위쳐
	UPROPERTY() class UTESTileView* _tv_characterInventoryViewContainer = nullptr;
	UPROPERTY() class UTESTileView* _tv_itemInventoryViewContainer = nullptr;

private:
	//. 필터
	UPROPERTY() class UTESUWCommonToggleSwitcher* _cts_filterSwitcher = nullptr;

	//. 필터 이름
	UPROPERTY() TMap<bool, class UTESTextBlock*> _txt_characterFilterTextMap = {};

	//. 필터 및 정렬
	UPROPERTY() TMap<bool, class UCheckBox*> _chkb_characterFilterSortingOrderMap = {};

private:
	UPROPERTY() class UCanvasPanel* _cp_characterPanel = nullptr;
	UPROPERTY() class UTESImage* _img_characterPortrait = nullptr;			//. 캐릭터 초상화
	UPROPERTY() class UTESTextBlock* _txt_characterName = nullptr;			//. 캐릭터 이름

private:
	//. 해당 캐릭터에 대한 역할
	UPROPERTY() class UTESImage* _img_characterRole = nullptr;
	UPROPERTY() class UTESTextBlock* _txt_characterRole = nullptr;

	//. 해당 캐릭터에 대한 공격타입
	UPROPERTY() class UTESImage* _img_characterAttackType = nullptr;
	UPROPERTY() class UTESTextBlock* _txt_characterAttackType = nullptr;

	//. 해당 캐릭터에 대한 공격 범위
	UPROPERTY() class UTESImage* _img_characterRange = nullptr;
	UPROPERTY() class UTESTextBlock* _txt_characterRange = nullptr;

	//. 해당 캐릭터에 대한 특성
	UPROPERTY() class UTESImage* _img_characterSpeciality = nullptr;
	UPROPERTY() class UTESTextBlock* _txt_characterSpecialty = nullptr;

private:
	UPROPERTY() class UTESTextBlock* _txt_characterCurrentLevel = nullptr;
	UPROPERTY() class UTESTextBlock* _txt_characterMaximumLevel = nullptr;
	UPROPERTY() class UTESTextBlock* _txt_characterAwakeningValue = nullptr;
	UPROPERTY() class UTESTextBlock* _txt_characterAwakeningRemainValue = nullptr;

private:
	UPROPERTY() class UTESImage* _img_characterGrade = nullptr;					//. 캐릭터 등급
	UPROPERTY() class UTESImage* _img_characterPrestigeEmblem = nullptr;		//. 캐릭터 프레스티지 엠블렘

private:
	UPROPERTY() class UCanvasPanel* _cp_itemPanel = nullptr;

private:
	UPROPERTY() class UTESImage* _img_itemIcon = nullptr;
	UPROPERTY() class UTESTextBlock* _txt_itemName = nullptr;
	UPROPERTY() class UTESTextBlock* _txt_itemRequiredLevel = nullptr;

private:
	TMap<ETypeStatBase, class UTESTextBlock*> _txt_requiredStatMap = {};
	TArray<class UCanvasPanel*> _cp_mainOptionPanelList = {};
	TArray<class UTESTextBlock*> _txt_mainOptionNameList = {};
	TArray<class UTESTextBlock*> _txt_mainOptionValueList = {};

#pragma endregion UMG Widget
};
