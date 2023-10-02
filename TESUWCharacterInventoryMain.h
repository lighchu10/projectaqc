// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TES.h"
#include "Structs/UI/TESUWGenericBase.h"
#include "TESUWCharacterInventoryMain.generated.h"

/**
 * 
 */
UCLASS()
class TES_API UTESUWCharacterInventoryMain : public UTESUWGenericBase
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
//. [ Init Functions ] 
//. ──────────────────────────────────────────────────────────────────────────────────────────────
private:
	void InitTileView();
	void InitAttributionCategories();

private:
	void InitCharacterFilter();
	void InitCharacterFilterSwitcher();
	void InitCharacterFilterButtonText();
	void InitCharacterFilterSortingOrderButton();

private:
	void InitCommonHeader();

//. ──────────────────────────────────────────────────────────────────────────────────────────────
//. [ Clear Functions ] 
//. ──────────────────────────────────────────────────────────────────────────────────────────────
private:
	void ClearCommonHeader();

//. ──────────────────────────────────────────────────────────────────────────────────────────────
//. [ Set Functions ] 
//. ──────────────────────────────────────────────────────────────────────────────────────────────
public:
	void SetWidget();

private:
	bool SetData();

private:
	void SetMainCategory(EAttributionCategory attribution, bool isChecked);
	void SetCommonHeader();

private:
	void SetCharacterFilterContents();
	void SetCharacterFilterContents(FTESFilterData filterData);
	void SetCharacterFilterSwitcher(bool isEnabled);
	void SetCharacterFilterButtonText(FTESFilterData filterData);
	void SetCharacterFilterButtonText(bool isEnable, int32 buttonTextId);
	void SetCharacterFilterSortingOrderButton(FTESFilterData filterData);
	void SetCharacterFilterSortingOrderButton(bool isOn, ETESFilterSortingOrder sortingOrder);

private:
	void SetContents();
	void SetOwnCharacterList(EAttributionCategory attribution);

private:
	void SetAppearAnim();

//. ──────────────────────────────────────────────────────────────────────────────────────────────
//. [ Misc Functions ] 
//. ──────────────────────────────────────────────────────────────────────────────────────────────
private:
	void SetCharcterCategoryDataByAttribution(TArray<FCharacterRow> characterRowList);
	void AddCharacterCategoryDataByAttribution(EAttributionCategory attributionCategory, FCharacterRow characterData);
	void GetCharacterCategoryDataByAttribution(OUT TArray<FCharacterRow>& characterRow, EAttributionCategory attributionCategory);
	int32 GetCharacterCategoryCountByAttribution(EAttributionCategory attributionCategory);

private:
	TArray<TArray<class UTESGDCharacter*>> GetCharacterListByRow(TArray<class UTESGDCharacter*> characterRowList, int32 maxRow);

private:
	TArray<class UTESGDCharacter*> MakeFilteredCharacterDataList(TArray<class UTESGDCharacter*> source
														 , TMap<int32, FTESFilterFilteringGroupData> filteringList
														 , bool isFiltering);

	TArray<class UTESGDCharacter*> MakeSortedCharacterDataList(TArray<class UTESGDCharacter*> sourceCharacterDataList
									, int32 sortingData
									, ETESFilterSortingOrder sortingOrder);

private:
	void ChangeAttributionCategory(EAttributionCategory category);

private:
	void PlayAppearAnim(FOnTESAction finishAction);

//. ──────────────────────────────────────────────────────────────────────────────────────────────
//. [ OnEvent Functions ] 
//. ──────────────────────────────────────────────────────────────────────────────────────────────
private:
	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_ChangeAttribution(EAttributionCategory attribution);

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_CharacterFilter();

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_ChangeSortingOrder();

	UFUNCTION()
	void OnEvent_CharacterFilterApply(struct FTESFilterData filterData);

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_CharacterInventoryView_EntryInitailized(UObject* item, UUserWidget* widget);

	UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
	void OnEvent_CharacterInventoryView_EntryReleased(UUserWidget* widget);

    UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
    void OnEvent_Home();

    UFUNCTION(BlueprintCallable, Meta = (AllowPrivateAccess = "true"))
    void OnEvent_Back();

//. ──────────────────────────────────────────────────────────────────────────────────────────────
//. [ Member Variables ] 
//. ──────────────────────────────────────────────────────────────────────────────────────────────
private:
    //. 게임 데이터 캐릭터 리스트(보유)
    TArray<class UTESGDCharacter*> _ownCharacterGDList = {};

private:
	UPROPERTY() EAttributionCategory _prevMainAttributionCategory = EAttributionCategory::None;
	UPROPERTY() EAttributionCategory _currMainAttributionCategory = EAttributionCategory::None;
	UPROPERTY() FTESFilterData _currentFilterData = {};

private:
	UPROPERTY() FTimerHandle _th_appearAnim = {};		//. 등장 애니메이션 타이머 핸들
	UPROPERTY() FTimerHandle _th_disappearAnim = {};	//. 사라지는 애니메이션 타이머 핸들

private:
	UPROPERTY() bool _isPlayingAppearAnim = false;		//. 캐릭터 목록창이 출현중인가?

//. ──────────────────────────────────────────────────────────────────────────────────────────────
//. [ Member Variables ] - Only UMG
//. ──────────────────────────────────────────────────────────────────────────────────────────────
private:
	UPROPERTY() class UTESUWCommonHeader* _wbp_commonHeader = nullptr;

private:
	//. 속성에 따른 캐릭터 카테고리(필터)
	TMap<EAttributionCategory, TArray<FCharacterRow>> _attributionCategoryDataMap = {};

	//. 속성 카테고리(토글 스위쳐)
	UPROPERTY() TMap<EAttributionCategory, class UTESUWCommonToggleSwitcher*> _cts_attributionCategoryMap = {};

	//. 캐릭터 필터
	UPROPERTY() class UTESUWCommonToggleSwitcher* _cts_characterFilterSwitcher = nullptr;

	//. 캐릭터 필터 이름
	UPROPERTY() TMap<bool, class UTESTextBlock*> _txt_characterFilterTextMap = {};

	//. 캐릭터 필터 및 정렬
	UPROPERTY() TMap<bool, class UCheckBox*> _chkb_characterFilterSortingOrderMap = {};

	//. 캐릭터 타일뷰 
	UPROPERTY() class UTESTileView* _tv_characterList = nullptr;
//. ──────────────────────────────────────────────────────────────────────────────────────────────
};
