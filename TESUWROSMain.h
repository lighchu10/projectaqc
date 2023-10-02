// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TES.h"
#include "Structs/UI/TESUWGenericBase.h"
#include "TESUWROSMain.generated.h"

/**
 * 
 */
UCLASS()
class TES_API UTESUWROSMain : public UTESUWGenericBase
{
	GENERATED_BODY()

//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Override Functions ] 
//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────
public:
	virtual void InitWidget() override;
	virtual void ClearWidget() override;
	virtual void OpenWidget() override;
	virtual void CloseWidget() override;

public:
	virtual void OpenDelegate() override;
	virtual void CloseDelegate() override;

//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Init Functions ] 
//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	void InitCommonHeaderWidget();

private:
	void InitStarFilter();
	void InitStarSwitcher();
	void InitStarFilterButtonText();
	void InitStarSortingOrderButton();

private:
	void InitStarIcon();
	void InitStarLevel();
	void InitStarTier();
	void InitStarCategoryName();
	void InitStarName();

private:
	void InitFlavorText();

private:
	void InitStarPower();

private:
	void InitStarTierCategories();
	void InitStarTierCategoriesNotice();

private:
	void InitStarViewSwitcher();
	void InitStarViewContents();
	void InitStarViewEmptyText();

private:
	void InitBonusStatView();

private:
	void InitStarLevelUpNotice();
	void InitStarLevelUpWidgetSwitcher();
	
//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Clear Functions ] 
//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	void ClearCommonHeaderWidget();

private:
	void ClearStarListView();
	void ClearBonusStatView();

//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Set Functions ] 
//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────
public:
	void SetWidget(const int32& rosStarId);

private:
	bool SetData(const int32& rosStarId);

private:
	void SetCommonHeaderWidget();

private:
    void SetStarTierCategories(const int32& retteraStarLevel);
    void SetStarTierCategoriesNotice(const int32& retteraStarLevel);

private:
	void SetStarFilter(const ETypeSortFilterContents& filterContentsType);
	void SetStarFilter(const FTESFilterData& filterData);
	void SetStarFilterSwitcher(bool isOn);
	void SetStarFilterButtonText(FTESFilterData filterData);
	void SetStarFilterButtonText(bool isOn, int32 buttonTextId);
	void SetStarFilterSortingOrderButton(FTESFilterData filterData);
	void SetStarFilterSortingOrderButton(bool isOn, ETESFilterSortingOrder sortingOrder);

private:
	void SetStarIcon(const FString& resourcePath);
	void SetStarLevel(const int32& starLevel);
	void SetStarTier(const int32& starTier);
	void SetStarCategoryName(const FString& starCategoryName);
	void SetStarName(const FString& starName);

private:
	void SetFlavorText(const FString& context);

private:
	void SetStarPower(const int32& starPower);

private:
	void SetStarTierCategory(const int32& tier, const bool& isOn);

private:
	void SetStarView(const int32& rosTierId, const bool& bUseScrollToTop = false);
	void SetStarView(TArray<int32> rosStarDatas, const bool& bUseScrollToTop = false);
	void SetStarViewSwitcher(bool isEmptied);
	void SetStarViewContents(TArray<int32> rosStarDatas, const bool& bUseScrollToTop = false);
	void SetStarViewEmptyText(const FString& str);

private:
	void SetBonusStatView(const int32& rosStarId);

private:
	void SetStarLevelUpNotice(const bool& isOn);
	void SetStarLevelUpWidgetSwitcher(const bool& isMaxLevel);

//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Filter Functions ] 
//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	TArray<int32> MakeFilteredROSStarDatas(TArray<int32> rosStarDatas
									     , TMap<int32, FTESFilterFilteringGroupData> filteringData
									     , bool isFiltering);


	TArray<int32> MakeSortedROSStarDatas(TArray<int32> rosStarDatas
									   , int32 sortingElementId
									   , ETESFilterSortingOrder sortingOrder);

//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Misc Functions ] 
//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────
public:
	void SelectTier(const int32& rosTierId);

//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Refresh Functions ] 
//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────
public:
	void RefreshAll();
	void RefreshStarView();
	void RefreshStarLevelUpNotice();
	void RefreshStarTierCategoriesNotice();

//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Play Functions ] 
//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────
public:
	void PlayAppearAnim(TDelegate<void()> onNextAction);
	void PlayDisappearAnim(TDelegate<void()> onNextAction);

//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ OnEvent Functions ] 
//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_StarListView_EntryInitailized(class UObject* item, class UUserWidget* widget);

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_StarListView_EntryReleased(class UUserWidget* widget);

private:
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_OpenStarFilter();

	UFUNCTION()
	void OnEvent_ApplyStarFilter(struct FTESFilterData commonFilterData);

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_ChangeSotingOrder();

private:
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_TierSelected(int32 tier);

private:
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_LevelUp();

public:
	UFUNCTION()
	void OnEvent_ItemDataUpdated();

private:
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_Back();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_Home();

//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Member Variables ] - Datas
//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	FRingOfStellaStarRow _rosStarRow = {};

private:
	UPROPERTY() int32 _previousROSTierId = TESConstantValue::NoneValue;
	UPROPERTY() int32 _currentROSTierId = TESConstantValue::NoneValue;
	UPROPERTY() int32 _currentROSStarId = TESConstantValue::NoneValue;

private:
	//. 현재 필터링 되고 있는 필터 데이터.
	UPROPERTY() struct FTESFilterData _currentFilterData = {};

//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Member Variables ] - Handles
//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	FTimerHandle _th_appear = {};
	FTimerHandle _th_disappear = {};

//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Member Variables ] - UMG
//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────
private:
	UPROPERTY() class UTESUWCommonHeader* _commonHeaderWidget = nullptr;

private:
	UPROPERTY() class UTESUWCommonToggleSwitcher* _cts_starFilterSwitcher = nullptr;
	UPROPERTY() TMap<bool, class UTESTextBlock*> _txt_starFilterTextMap = {};
	UPROPERTY() TMap<bool, class UCheckBox*> _chkb_starFilterSortingOrderMap = {};

private:
	UPROPERTY() class UTESImage* _img_starIcon = nullptr;
	UPROPERTY() class UTESTextBlock* _txt_starLevel = nullptr;
	UPROPERTY() class UTESTextBlock* _txt_starTier = nullptr;
	UPROPERTY() class UTESTextBlock* _txt_starCategoryName = nullptr;
	UPROPERTY() class UTESTextBlock* _txt_starName = nullptr;

private:
	UPROPERTY() class UTESTextBlock* _txt_flavor = nullptr;

private:
	UPROPERTY() class UTESTextBlock* _txt_starPower = nullptr;

private:
	UPROPERTY() class UTESWidgetSwitcher* _ws_levelUp = nullptr;

private:
	UPROPERTY() TMap<int32, class UTESUWCommonToggleSwitcher*> _cts_tierCategories = {};
	UPROPERTY() TMap<int32, class UTESImage*> _img_tierCategories_select = {};
	UPROPERTY() TMap<int32, class UTESImage*> _img_tierCategories_notice = {};

private:
	UPROPERTY() class UWidgetSwitcher* _ws_starView = nullptr;
	UPROPERTY()	class UTESListView* _lv_starView = nullptr;
	UPROPERTY()	class UTESTextBlock* _txt_starViewEmpty = nullptr;
	UPROPERTY() TArray<class UTESEDROSMainStarSlot*> _rosDetailStarSlotDatas = {};

private:
	UPROPERTY() class UTESWrapBox* _wb_bonusStatView = nullptr;
	UPROPERTY() TArray<class UTESUWROSMainBonusStatSlot*> _rosDetailBonusStatSlotWidgets = {};

private:
	UPROPERTY() class UTESImage* _img_starLevelUpNotice = nullptr;
//. ───────────────────────────────────────────────────────────────────────────────────────────────────────────
};
