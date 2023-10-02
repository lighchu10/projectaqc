// Fill out your copyright notice in the Description page of Project Settings.
#include "Contents/CharacterInventory/UI/TESUWCharacterInventoryMain.h"
#include "Contents/CharacterInventory/UI/TESUWCharacterInventorySlot.h"
#include "Contents/CharacterInventory/UI/TESEDCharacterInventorySlot.h"
#include "Contents/Common/Manager/TESUserDataManager.h"
#include "Contents/Common/Manager/TESSaveGameManager.h"
#include "Contents/Common/Manager/TESUIManager.h"
#include "Contents/Common/Manager/TESPopupManager.h"
#include "Contents/Common/Manager/TESSceneManager.h"
#include "Contents/Common/GameData/TESGDCharacter.h"
#include "Contents/Common/UI/CustomSlates/TESTextBlock.h"
#include "Contents/Common/UI/CustomSlates/TESWrapBox.h"
#include "Contents/Common/UI/CustomSlates/TESTileView.h"
#include "Contents/Common/UI/TESUWCommonHeader.h"
#include "Contents/Common/UI/Filter/TESUWFilter.h"
#include "Contents/Common/UI/TESUWCommonToggleSwitcher.h"
#include "Contents/Lobby/UI/TESUWLobbyMenuDetail.h"
#include "Components/WidgetSwitcher.h"
#include "Components/CheckBox.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "TESUtils.h"
#include "TESUtilTable.h"
#include "TESUtilString.h"
#include "TESUtilFilter.h"
#include "TESGameInstance.h"

void UTESUWCharacterInventoryMain::InitWidget()
{
	Super::InitWidget();

	InitTileView();

	InitAttributionCategories();

	InitCharacterFilter();

	InitCommonHeader();
}

void UTESUWCharacterInventoryMain::ClearWidget()
{
	Super::ClearWidget();

	_ownCharacterGDList.Empty();

	GetWorld()->GetTimerManager().ClearTimer(_th_appearAnim);
	GetWorld()->GetTimerManager().ClearTimer(_th_disappearAnim);

	ClearCommonHeader();
}

void UTESUWCharacterInventoryMain::OpenWidget()
{
	Super::OpenWidget();

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UTESUWCharacterInventoryMain::CloseWidget()
{
	Super::CloseWidget();

	SetVisibility(ESlateVisibility::Collapsed);
}

void UTESUWCharacterInventoryMain::OpenDelegate()
{
	Super::OpenDelegate();

	FTESUtilEvents::OnCommonFilterApply.BindUObject(this, &UTESUWCharacterInventoryMain::OnEvent_CharacterFilterApply);
}

void UTESUWCharacterInventoryMain::CloseDelegate()
{
	Super::CloseDelegate();

	FTESUtilEvents::OnCommonFilterApply.Unbind();
}

void UTESUWCharacterInventoryMain::InitTileView()
{
	_tv_characterList = Cast<UTESTileView>(GetWidgetFromName(TEXT("TV_CharacterList")));
	TESCHECK(nullptr != _tv_characterList);
}

void UTESUWCharacterInventoryMain::InitAttributionCategories()
{
	int32 maxCount = static_cast<int32>(EAttributionCategory::Max);
	for (int32 index = 1; index < maxCount; ++index)
	{
		FString widgetName = FString::Printf(TEXT("CTS_MainCategory_%d"), index);
		UTESUWCommonToggleSwitcher* cts_mainCategory = Cast<UTESUWCommonToggleSwitcher>(GetWidgetFromName(*widgetName));
		TESCHECK_CONTINUE(nullptr != cts_mainCategory);

		EAttributionCategory category = static_cast<EAttributionCategory>(index);
		_cts_attributionCategoryMap.Add(category, cts_mainCategory);
	}
}

void UTESUWCharacterInventoryMain::InitCharacterFilter()
{
	InitCharacterFilterSwitcher();

	InitCharacterFilterButtonText();

	InitCharacterFilterSortingOrderButton();
}

void UTESUWCharacterInventoryMain::InitCharacterFilterSwitcher()
{
	_cts_characterFilterSwitcher = Cast<UTESUWCommonToggleSwitcher>(GetWidgetFromName(TEXT("CTS_CharacterFilterSwitcher")));
	TESCHECK(nullptr != _cts_characterFilterSwitcher);
	_cts_characterFilterSwitcher->SetToggleState(false);
}

void UTESUWCharacterInventoryMain::InitCharacterFilterButtonText()
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

void UTESUWCharacterInventoryMain::InitCharacterFilterSortingOrderButton()
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

void UTESUWCharacterInventoryMain::InitCommonHeader()
{
	_wbp_commonHeader = Cast<UTESUWCommonHeader>(GetWidgetFromName(TEXT("WBP_CommonHeader")));
	TESCHECK(nullptr != _wbp_commonHeader);
	_wbp_commonHeader->InitWidget();
	_wbp_commonHeader->CloseWidget();
}

void UTESUWCharacterInventoryMain::ClearCommonHeader()
{
	if (true == ::IsValid(_wbp_commonHeader))
	{
		_wbp_commonHeader->CloseWidget();
		_wbp_commonHeader->ClearWidget();
		_wbp_commonHeader = nullptr;
	}
}

void UTESUWCharacterInventoryMain::SetWidget()
{
	if (false == SetData())
	{
		CloseWidget();

		return;
	}

	//.
	SetCommonHeader();

	//. 기본 세팅은 All로 지정
	ChangeAttributionCategory(EAttributionCategory::All);

	//.
	SetCharacterFilterContents();

	//.
	SetAppearAnim();
}

bool UTESUWCharacterInventoryMain::SetData()
{
	//. 보유 캐릭터 아이디는 DB로부터 가져온다.
	TArray<UTESGDCharacter*> characterInfoList = SharedUserDataManager->FindHaveCharacterAll();
	TArray<int32> playableCharacterIdList = {};
	TArray<FCharacterRow> characterTableRow = {};
	for (UTESGDCharacter* charInfo : characterInfoList)
	{
		int32 charTableId = charInfo->GetCharacterTableId();
		if (false == playableCharacterIdList.Contains(charTableId))
		{
			playableCharacterIdList.Add(charTableId);
			characterTableRow.Add(charInfo->GetCharacterTable());
		}
	}

	//. 캐릭터별로 속성 세팅
	SetCharcterCategoryDataByAttribution(characterTableRow);

	//. 보유 캐릭터 아이디는 DB로부터 가져온다.
	//. TArray<UTESGDCharacter*> characterInfoList = SharedUserDataManager->FindHaveCharacterAll();
	for (UTESGDCharacter* charInfo : characterInfoList)
	{
		charInfo->SetOwnCharacter(true);

		_ownCharacterGDList.Add(charInfo);
	}

	return true;
}

void UTESUWCharacterInventoryMain::SetMainCategory(EAttributionCategory attribution, bool isChecked)
{
	if (EAttributionCategory::None == attribution)
	{
		return;
	}

	if (false == _cts_attributionCategoryMap.Contains(attribution))
	{
		TESLOG(Error, TEXT("Invalid attribution. %d"), attribution);

		return;
	}

	_cts_attributionCategoryMap[attribution]->SetToggleState(isChecked);
}

void UTESUWCharacterInventoryMain::SetCommonHeader()
{
	if (true == ::IsValid(_wbp_commonHeader))
	{
		_wbp_commonHeader->OpenWidget();
		_wbp_commonHeader->SetWidget();
	}
}

void UTESUWCharacterInventoryMain::SetCharacterFilterContents()
{
	ETypeSortFilterContents filterContents = ETypeSortFilterContents::Character;

	TOptional<FTESFilterData> filterData = SharedSaveGameManager->GetFilterData(filterContents);

	if (false == filterData.IsSet())
	{
		return;
	}

	SetCharacterFilterContents(filterData.GetValue());
}

void UTESUWCharacterInventoryMain::SetCharacterFilterContents(FTESFilterData filterData)
{
	//. 필터 데이터도 갱신해줍니다.
	_currentFilterData = filterData;

	//. 현재 필터여부에 따라 필터 버튼을 교체해줍니다.
	SetCharacterFilterSwitcher(_currentFilterData._isFiltering);

	//. 현재 필터 텍스트를 세팅합니다.
	SetCharacterFilterButtonText(_currentFilterData);

	//. 현재 정렬 순서로 정렬 박스를 출력해줍니다.
	SetCharacterFilterSortingOrderButton(_currentFilterData._isFiltering, _currentFilterData._sortingOrder);
}

void UTESUWCharacterInventoryMain::SetCharacterFilterSwitcher(bool isEnabled)
{
	if (true == ::IsValid(_cts_characterFilterSwitcher))
	{
		_cts_characterFilterSwitcher->SetToggleState(isEnabled);
	}
}

void UTESUWCharacterInventoryMain::SetCharacterFilterButtonText(FTESFilterData filterData)
{
	const FSortFilterElementRow& filterElementTable = SharedTDBManager->GetSortFilterElementRow(filterData._sortingElementId);

	if (true == filterElementTable.IsEmpty())
	{
		SetCharacterFilterButtonText(filterData._isFiltering, TESConstantValue::NoneValue);
	}
	else
	{
		SetCharacterFilterButtonText(filterData._isFiltering, filterElementTable.SortFilterNameID);
	}
}

void UTESUWCharacterInventoryMain::SetCharacterFilterButtonText(bool isEnable, int32 buttonTextId)
{
	if (true == _txt_characterFilterTextMap.Contains(isEnable))
	{
		FString str = TESUtilTable::GetGlobalString(GetWorld(), buttonTextId);
		_txt_characterFilterTextMap[isEnable]->SetText(FText::FromString(*str));
	}
}

void UTESUWCharacterInventoryMain::SetCharacterFilterSortingOrderButton(FTESFilterData filterData)
{
	SetCharacterFilterSortingOrderButton(filterData._isFiltering, filterData._sortingOrder);
}

void UTESUWCharacterInventoryMain::SetCharacterFilterSortingOrderButton(bool isOn, ETESFilterSortingOrder sortingOrder)
{
	if (true == _chkb_characterFilterSortingOrderMap.Contains(isOn))
	{
		//. Unchecked : 내림차순, checked : 오름차순
		_chkb_characterFilterSortingOrderMap[isOn]->SetCheckedState(sortingOrder == ETESFilterSortingOrder::Descending ?
			ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	}
}

void UTESUWCharacterInventoryMain::SetContents()
{
	//. 현재 활성화 된 속성으로 캐릭터 리스트를 필터링 해준다.(보유 캐릭터)
	SetOwnCharacterList(_currMainAttributionCategory);
}

void UTESUWCharacterInventoryMain::SetOwnCharacterList(EAttributionCategory attribution)
{
	TESCHECK(nullptr != _tv_characterList);
	_tv_characterList->ClearListItems();

	//. 속성 카테고리에 맞는 리스트를 가져온다.
	TArray<FCharacterRow> characterRowListByAttribution = {};

	GetCharacterCategoryDataByAttribution(OUT characterRowListByAttribution, attribution);

	//. 속성 필터
	TArray<UTESGDCharacter*> filteredIdListByAttribution = {};

	for (FCharacterRow characterRowData : characterRowListByAttribution)
	{
		for (UTESGDCharacter* ownCharacterList : _ownCharacterGDList)
		{
			//. 2021.11.29. p2e로 방향성이 변경되면서 캐릭터 중복 허용작업 중. 문제가 될수 있는 부분이지만 기획이 변경될거라 보고 넘어갑니다.
			if (ownCharacterList->GetCharacterTableId() == characterRowData.Id)
			{
				filteredIdListByAttribution.Add(ownCharacterList);
			}
		}
	}

	//. 필터링된 캐릭터 리스트를 얻어옵니다.(필터 옵션창)
	TArray<UTESGDCharacter*> filteredCharacterListByFilterOption = MakeFilteredCharacterDataList(filteredIdListByAttribution
																								, _currentFilterData._filteringData
																								, _currentFilterData._isFiltering);

	//. 필터링된 캐릭터 리스트를 정렬하여 얻어옵니다.
	TArray<UTESGDCharacter*> sortedCharacterDataList = MakeSortedCharacterDataList(filteredCharacterListByFilterOption
																				, _currentFilterData._sortingElementId
																				, _currentFilterData._sortingOrder);
	
	for (UTESGDCharacter* characterData : sortedCharacterDataList)
	{
		UTESEDCharacterInventorySlot* slotEntryData = NewObject<UTESEDCharacterInventorySlot>(this);

		if (false == ::IsValid(slotEntryData))
		{
			TESLOG(Error, TEXT("Invalid EntryData."));

			continue;
		}

		slotEntryData->SetCharacterData(characterData);

		slotEntryData->SetVisibilityByCharacterGrade(true);

		slotEntryData->SetVisibilityByCharacterAttribute(true);

		slotEntryData->SetVisibilityByCharacterAwakenLevel(true);

		slotEntryData->SetSortRule(_currentFilterData._sortingElementId);

		_tv_characterList->AddItem(slotEntryData);
	}
}

void UTESUWCharacterInventoryMain::SetAppearAnim()
{
	if (true == _isPlayingAppearAnim)
	{
		return;
	}

	_isPlayingAppearAnim = true;

	PlayAppearAnim(FOnTESAction::CreateLambda([=]()
	{
		SetContents();
		_isPlayingAppearAnim = false;
	}));
}

void UTESUWCharacterInventoryMain::SetCharcterCategoryDataByAttribution(TArray<FCharacterRow> characterRowList)
{
	//. 초기화
	_attributionCategoryDataMap.Empty();

	//. 캐릭터 속성별로 세팅
	int32 maxCount = characterRowList.Num();
	for (int32 index = 0; index < maxCount; ++index)
	{
		//. All 속성에는 모든 캐릭을 다 넣어준다.
		AddCharacterCategoryDataByAttribution(EAttributionCategory::All, characterRowList[index]);

		ETypeCharAttribute characterAttributeType = characterRowList[index].T_CharAttribute;
		switch (characterAttributeType)
		{
		case ETypeCharAttribute::Red:
			{
				AddCharacterCategoryDataByAttribution(EAttributionCategory::RedAttribution, characterRowList[index]);
			}
			break;

		case ETypeCharAttribute::Green:
			{
				AddCharacterCategoryDataByAttribution(EAttributionCategory::GreenAttribution, characterRowList[index]);
			}
			break;

		case ETypeCharAttribute::Blue:
			{
				AddCharacterCategoryDataByAttribution(EAttributionCategory::BlueAttribution, characterRowList[index]);
			}
			break;

		case ETypeCharAttribute::Black:
			{
				AddCharacterCategoryDataByAttribution(EAttributionCategory::BlackAttribution, characterRowList[index]);
			}
			break;

		case ETypeCharAttribute::White:
			{
				AddCharacterCategoryDataByAttribution(EAttributionCategory::WhiteAttribution, characterRowList[index]);
			}
			break;

		//. 2022.05.13.
		//. 캐릭터 데이터를 속성별로 분류하기 위한 작업을 하는 코드입니다만, 게시판과 같은 캐릭터의 데이터는 속성이 None으로 충분히 될 수 있습니다.
		//. 이 경우, 잘못된 속성이 아니므로 그냥 아무런 로그도 출력하지 않도록 주석 처리되었습니다.
		//. default:
		//. 	{
		//. 		TESLOG(Error, TEXT("There is no attribution : %s")
		//. 					, *TESUtilString::GetEnumAsString<ETypeCharAttribute>(TEXT("ETypeCharAttribute"), characterAttributeType));
		//. 	}
		//. 	break;
		}
	}
}

void UTESUWCharacterInventoryMain::AddCharacterCategoryDataByAttribution(EAttributionCategory attributionCategory, FCharacterRow characterData)
{
	if (true == _attributionCategoryDataMap.Contains(attributionCategory))
	{
		_attributionCategoryDataMap[attributionCategory].Add(characterData);
	}
	else
	{
		TArray<FCharacterRow> characterRowList = {};
		characterRowList.Add(characterData);
		_attributionCategoryDataMap.Add(attributionCategory, characterRowList);
	}
}

void UTESUWCharacterInventoryMain::GetCharacterCategoryDataByAttribution(OUT TArray<FCharacterRow>& characterRow, EAttributionCategory attributionCategory)
{
	if (true == _attributionCategoryDataMap.Contains(attributionCategory))
	{
		characterRow = _attributionCategoryDataMap[attributionCategory];
	}
	else
	{
		TESLOG(Error, TEXT("There is no Category : %d"), attributionCategory);
	}
}

int32 UTESUWCharacterInventoryMain::GetCharacterCategoryCountByAttribution(EAttributionCategory attributionCategory)
{
	if (_attributionCategoryDataMap.Contains(attributionCategory))
	{
		return _attributionCategoryDataMap[attributionCategory].Num();
	}

	return 0;
}

TArray<TArray<UTESGDCharacter*>> UTESUWCharacterInventoryMain::GetCharacterListByRow(TArray<UTESGDCharacter*> characterRowList, int32 maxRow)
{
	//. 1행당 7개의 캐릭터까지 들어갈수 있으므로, 그에 맞게 캐릭터 세팅
	TArray<TArray<UTESGDCharacter*>> rowCharacterIdList;

	TArray<UTESGDCharacter*> lastRowNotOwnCharIdList = characterRowList;

	//. 보유(혹은 미보유)캐릭터 리스트를 지역변수에 담아두고
	//. 하나씩 2중배열에 담아준다.(첫번째는 행, 두번째는 열)
	for (int index = 0; index <= maxRow; ++index)
	{
		TArray<UTESGDCharacter*> rowCharList;
		for (int idIndex = 0; idIndex < TESConstantValue::MaxCountForCharacterListInRow; ++idIndex)
		{
			if (lastRowNotOwnCharIdList.Num() <= 0)
				break;

			rowCharList.Add(lastRowNotOwnCharIdList[0]);

			//. 전부 제거하면 마지막줄만 남게된다.
			lastRowNotOwnCharIdList.RemoveAt(0);
		}
		rowCharacterIdList.Add(rowCharList);
	}

	return rowCharacterIdList;
}

TArray<UTESGDCharacter*> UTESUWCharacterInventoryMain::MakeFilteredCharacterDataList(TArray<UTESGDCharacter*> source
																				   , TMap<int32, FTESFilterFilteringGroupData> filteringList
																				   , bool isFiltering)
{
	return TESUtilFilter::MakeFilteredCharacterDataList(source, filteringList, isFiltering);
}

TArray<UTESGDCharacter*> UTESUWCharacterInventoryMain::MakeSortedCharacterDataList(TArray<UTESGDCharacter*> sourceCharacterDataList, int32 sortingData, ETESFilterSortingOrder sortingOrder)
{
	return TESUtilFilter::MakeSortedCharacterDataList(sourceCharacterDataList, sortingData, sortingOrder);
}

void UTESUWCharacterInventoryMain::ChangeAttributionCategory(EAttributionCategory category)
{
	//. 같은 카테고리를 교체하려고하면 무시
	if (category == _currMainAttributionCategory)
	{
		return;
	}

	//. 아이디 값을 교체합니다.
	_prevMainAttributionCategory = _currMainAttributionCategory;
	_currMainAttributionCategory = category;

	//. 이전 메인 카테고리를 "선택되지 않은 상태"로 변경합니다.
	SetMainCategory(_prevMainAttributionCategory, false);

	//. 이전 메인 카테고리를 "선택된 상태"로 변경합니다.
	SetMainCategory(_currMainAttributionCategory, true);

	//. 데이터 및 필터 세팅
	//SetCharacterFilterContents();
}

void UTESUWCharacterInventoryMain::PlayAppearAnim(FOnTESAction finishAction)
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

void UTESUWCharacterInventoryMain::OnEvent_ChangeAttribution(EAttributionCategory attribution)
{
	if (attribution == _currMainAttributionCategory)
		return;

	//. 클릭한 속성으로 바꿔준다.
	ChangeAttributionCategory(attribution);

	//. 필터 적용중인지 아닌지 세팅
	SetCharacterFilterSwitcher(_currentFilterData._isFiltering);

	//. 바뀐 속성에 따라 필터링 해준다.
	SetContents();
}

void UTESUWCharacterInventoryMain::OnEvent_CharacterFilter()
{
	UTESUWFilter* filterWidget = SharedPopupManager->CreatePopupWidget<UTESUWFilter>(TEXT("WBP_Filter"), TEXT("Common/WBP/Filter"));
	if (false == ::IsValid(filterWidget))
	{
		TESLOG(Error, TEXT("Invalid FilterWidget."));

		return;
	}

	ETypeSortFilterContents filterContentsType = ETypeSortFilterContents::Character;

	filterWidget->InitWidget();
	filterWidget->OpenWidget();
	filterWidget->SetWidget(filterContentsType);
}

void UTESUWCharacterInventoryMain::OnEvent_ChangeSortingOrder()
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
	SetContents();

	//. 현재의 정렬 순서로 정렬 박스를 출력해줍니다.
	SetCharacterFilterSortingOrderButton(_currentFilterData._isFiltering, _currentFilterData._sortingOrder);
}

void UTESUWCharacterInventoryMain::OnEvent_CharacterFilterApply(FTESFilterData filterData)
{
	_currentFilterData = filterData;

	SetCharacterFilterContents(_currentFilterData);

	//. 데이터 매니져의 필터 데이터를 갱신합니다.
	SharedSaveGameManager->SetFilterData(_currentFilterData._contentsType, _currentFilterData);

	//. 적용 혹은 롤백이 된 세팅을 적용
	SetContents();
}

void UTESUWCharacterInventoryMain::OnEvent_CharacterInventoryView_EntryInitailized(UObject* item, UUserWidget* widget)
{
#if WITH_EDITOR	
	if (false == TESUtils::IsEditorPlayingGame(GetWorld()))
	{
		return;
	}
#endif //. WITH_EDITOR

	UTESUWCharacterInventorySlot* characterSettingInvenSlotWidget = Cast<UTESUWCharacterInventorySlot>(widget);
	if (false == ::IsValid(characterSettingInvenSlotWidget))
	{
		return;
	}

	characterSettingInvenSlotWidget->InitWidget();
	characterSettingInvenSlotWidget->OpenWidget();
	characterSettingInvenSlotWidget->SetWidget(Cast<UTESEDCharacterInventorySlot>(item));
}

void UTESUWCharacterInventoryMain::OnEvent_CharacterInventoryView_EntryReleased(UUserWidget* widget)
{
#if WITH_EDITOR	
	if (false == TESUtils::IsEditorPlayingGame(GetWorld()))
	{
		return;
	}
#endif //. WITH_EDITOR

	UTESUWCharacterInventorySlot* characterSettingWidget = Cast<UTESUWCharacterInventorySlot>(widget);
	if (false == ::IsValid(characterSettingWidget))
	{
		return;
	}

	characterSettingWidget->CloseWidget();
	characterSettingWidget->ClearWidget();
}

void UTESUWCharacterInventoryMain::OnEvent_Home()
{
    if (true == _isPlayingAppearAnim)
    {
        return;
    }

    SharedSceneManager->OnEvent_ContentsHome();
}

void UTESUWCharacterInventoryMain::OnEvent_Back()
{
    if (true == _isPlayingAppearAnim)
    {
        return;
    }

    SharedSceneManager->OnEvent_ContentsBack();
}
