#include "TESUtilFilter.h"
#include "TESUtilString.h"
#include "TESUtilStatus.h"
#include "Contents/RingOfStella/Manager/TESROSManager.h"

FTESFilterData TESUtilFilter::ResetFilterData(ETypeSortFilterContents contentsType)
{
	FTESFilterData filterData = {};
	filterData._contentsType = contentsType;
	filterData._sortingElementId = 0;
	filterData._isFiltering = false;
	filterData._filteringData = {};
	filterData._sortingOrder = ETESFilterSortingOrder::Descending;

	TArray_SortFilterGroup sortFilterGroupTableList = TESUtilTable::GetSortFilterGroupAllBySortFilterContentsType(contentsType);

	int32 maxCount = sortFilterGroupTableList.Num();
	if (maxCount <= 0)
	{
		TESLOG(Error, TEXT("Not Found FilterGroupTables."));

		return filterData;
	}

	for (int32 index = 0; index < maxCount; ++index)
	{
		FSortFilterGroupRow sortFilterGroupTable = (*sortFilterGroupTableList[index]);
		if (true == sortFilterGroupTable.IsEmpty())
		{
			continue;
		}

		if (ETypeSortFilterCategory::Sort == sortFilterGroupTable.T_SortFilterCategory)
		{
			TArray<int32> filterElementIdList = sortFilterGroupTable.GetColumnData<TArray<int32>>(ESortFilterGroupColumn::Section00ElementID);

			if (false == filterElementIdList.IsValidIndex(0))
			{
				TESLOG(Error, TEXT("Invalid First Element Index. SortFilterTableId : %d"), sortFilterGroupTable.Id);

				return filterData;
			}

			const FSortFilterElementRow& filterElementTable = FTDBManager::Get().Instance->GetSortFilterElementRow(filterElementIdList[0]);
			if (true == filterElementTable.IsEmpty())
			{
				break;
			}

			filterData._sortingElementId = filterElementTable.Id;
		}
	}

	return filterData;
}

bool TESUtilFilter::CheckFilteringForItemData(UTESGDGoodsItem* itemData, int32 filterElementId)
{
	if (nullptr == itemData)
	{
		TESLOG(Error, TEXT("Invalid ItemData."));

		return false;
	}

	const FSortFilterElementRow& filterElementTable = SharedTDBManager->GetSortFilterElementRow(filterElementId);
	if (true == filterElementTable.IsEmpty())
	{
		TESLOG(Error, TEXT("Invalid FilterElementTable. : %d"), filterElementId);

		return false;
	}

	ETypeSortFilterElement filterElementType = filterElementTable.T_SortFilterElement;
	switch (filterElementType)
	{
	case ETypeSortFilterElement::T_Grade:
		{
			if (filterElementTable.GradeFilter == itemData->GetItemGrade())
			{
				return true;
			}
		}
		break;

	case ETypeSortFilterElement::T_EquipSlot:
		{
			if (filterElementTable.EquipSlotFilter == itemData->GetEquipSlotType())
			{
				return true;
			}
		}
		break;

	case ETypeSortFilterElement::T_StatOption:
		{
			int32 maxCount = filterElementTable.StatOptionFilter.Num();
			if (maxCount <= 0)
			{
				TESLOG(Error, TEXT("Empty StatOptionValue. FilterElementId : %d"), filterElementId);

				break;
			}

			for (int32 index = 0; index < maxCount; ++index)
			{
				ETypeStatOption statOptionType = filterElementTable.StatOptionFilter[index];
				if (true == itemData->HasItemOption(statOptionType))
				{
					return true;
				}
			}
		}
		break;

	case ETypeSortFilterElement::T_Use:
		{
			if (filterElementTable.UseFilter == itemData->GetUseType())
			{
				return true;
			}
		}
		break;

	case ETypeSortFilterElement::T_Equip:
		{
			if (filterElementTable.EquipFilter == itemData->GetEquipType())
			{
				return true;
			}
		}
		break;

	case ETypeSortFilterElement::T_Material:
		{
			if (filterElementTable.MaterialFilter == itemData->GetMaterialType())
			{
				return true;
			}
		}
		break;

	case ETypeSortFilterElement::T_SortRuleCharacter:
	case ETypeSortFilterElement::T_SortRuleEquip:
	case ETypeSortFilterElement::T_SortRuleMisc:
	case ETypeSortFilterElement::T_SortRulePostBox:
		{
			//. 필터링에서 정렬 타입은 지원하지 않습니다.
			TESLOG(Error, TEXT("Sorting type is not supported in filtering. ElementTableId : %d"), filterElementId);
		}
		break;

	case ETypeSortFilterElement::T_CharRole:
	case ETypeSortFilterElement::T_CharAttack:
	case ETypeSortFilterElement::T_CharRange:
		{
			//. 아이템 필터링에서는 캐릭터에 대한 필터링은 지원하지 않습니다.
			TESLOG(Error, TEXT("Filtering on characters is not supported in item filtering. ElementTableId : %d"), filterElementId);
		}
		break;

	case ETypeSortFilterElement::T_Goods:
		{
			//. 재화는 아이템으로 취급하지 않으므로, 지원하지 않습니다.
			TESLOG(Error, TEXT("Since goods are not treated as items, they are not supported. ElementTableId : %d"), filterElementId);
		}
		break;

	case ETypeSortFilterElement::T_CharSpeciality:
		{
			//. 캐릭터 주특기는 아이템으로 취급하지 않으므로, 지원하지 않습니다.
			TESLOG(Error, TEXT("Since CharSpeciality is not treated as an item, it is not supported. ElementTableId : %d"), filterElementId);
		}
		break;

	default:
		{
			//. 지원하는 타입이 아닙니다.
			TESLOG(Error, TEXT("Not Supported Type. id : %d, Type : %d"), filterElementId, filterElementType);
		}
		break;
	}

	//. 일치하는 목록이 없습니다.
	return false;
}

TArray<UTESGDGoodsItem*> TESUtilFilter::MakeFilteredItemDataList(TArray<UTESGDGoodsItem*> itemDataList
														  , TMap<int32, FTESFilterFilteringGroupData> filteringData
														  , bool isFiltering)
{
	//. ───────────────────────────────────────────────────────────────────────────
	//. 필터링 조건
	//. ───────────────────────────────────────────────────────────────────────────
	//. 1. 필터 그룹들끼리는 AND 조건입니다.
	//.   - 하나라도 만족하지 않는다면 추가하지 않습니다.
	//.
	//. 2. 필터 그룹 내의 요소들끼리는 OR 조건입니다.
	//.   - 하나라도 만족한다면 조건에 충족했다고 판단합니다.
	//. ───────────────────────────────────────────────────────────────────────────
	if (true == isFiltering && filteringData.Num() > 0)
	{
		TArray<UTESGDGoodsItem*> filteredItemDatas = {};

		int32 maxCount = itemDataList.Num();
		for (int32 index = 0; index < maxCount; ++index)
		{
			UTESGDGoodsItem* itemData = itemDataList[index];
			if (false == ::IsValid(itemData))
			{
				continue;
			}

			//. 필터링의 모든 그룹들을 순회합니다.
			bool isAppendItem = true;
			for (TPair<int32, FTESFilterFilteringGroupData> child : filteringData)
			{
				bool isCheckFiltering = false;

				//. 그룹 내의 해당 요소들 중에서 만족하는 내용이 있는지를 체크합니다.
				FTESFilterFilteringGroupData filteringGroupData = child.Value;
				for (int32 filterElementId : filteringGroupData._elementIdList)
				{
					isCheckFiltering = TESUtilFilter::CheckFilteringForItemData(itemData, filterElementId);
					if (true == isCheckFiltering)
					{
						break;
					}
				}

				//. 해당 그룹에서 만족하는 데이터가 없었습니다.
				if (false == isCheckFiltering)
				{
					isAppendItem = false;
				}
			}

			//. 모든 조건에 만족하는 데이터를 추가합니다.
			if (true == isAppendItem)
			{
				filteredItemDatas.Add(itemData);
			}
		}

		TESLOG(Display, TEXT("FilteredItemDataCount : %d"), filteredItemDatas.Num());

		return filteredItemDatas;
	}
	else
	{
		return itemDataList;
	}
}

TArray<UTESGDGoodsItem*> TESUtilFilter::MakeSortedItemDataListByEquip(TArray<UTESGDGoodsItem*> itemDataList
																	, TArray<ETypeSortRuleEquip> itemSotingRules
																	, ETESFilterSortingOrder sortingOrder)
{
	//. 정렬할 것이 없다면 하지 않습니다.
	if (itemDataList.Num() <= 1)
	{
		return itemDataList;
	}
	
	//. 정렬을 시작합니다.
	itemDataList.StableSort([&](const UTESGDGoodsItem& itemDataA, const UTESGDGoodsItem& itemDataB)			
	{
		for (int32 index = 0; index < itemSotingRules.Num(); ++index)
		{			
			ETypeSortRuleEquip equipItemSortingType = itemSotingRules[index];
			switch (equipItemSortingType)
			{
			case ETypeSortRuleEquip::EquipItemId:
				{
					if (ETESFilterSortingOrder::Descending == sortingOrder)
					{
						if (itemDataA.GetItemTableId() != itemDataB.GetItemTableId())
						{							
							return itemDataA.GetItemTableId() > itemDataB.GetItemTableId();
						}						
					}
					else
					{
						if (itemDataA.GetItemTableId() != itemDataB.GetItemTableId())
						{
							return itemDataA.GetItemTableId() < itemDataB.GetItemTableId();
						}						
					}
				}
				break;

			case ETypeSortRuleEquip::EquipPower:
				{
					if (ETESFilterSortingOrder::Descending == sortingOrder)
					{
						if (itemDataA.GetBattlePower() != itemDataB.GetBattlePower())
						{
							return itemDataA.GetBattlePower() > itemDataB.GetBattlePower();
						}						
					}
					else
					{
						if (itemDataA.GetBattlePower() != itemDataB.GetBattlePower())
						{
							return itemDataA.GetBattlePower() < itemDataB.GetBattlePower();
						}						
					}
				}
				break;

			case ETypeSortRuleEquip::EquipGrade:
				{
					if (ETESFilterSortingOrder::Descending == sortingOrder)
					{
						if (itemDataA.GetItemGrade() != itemDataB.GetItemGrade())
						{
							return itemDataA.GetItemGrade() > itemDataB.GetItemGrade();
						}						
					}
					else
					{
						if (itemDataA.GetItemGrade() != itemDataB.GetItemGrade())
						{
							return itemDataA.GetItemGrade() < itemDataB.GetItemGrade();
						}						
					}
				}
				break;

			case ETypeSortRuleEquip::EquipLevel:
				{
					if (ETESFilterSortingOrder::Descending == sortingOrder)
					{
						if (itemDataA.GetItemCurrentLevel() != itemDataB.GetItemCurrentLevel())
						{
							return itemDataA.GetItemCurrentLevel() > itemDataB.GetItemCurrentLevel();
						}						
					}
					else
					{
						if (itemDataA.GetItemCurrentLevel() != itemDataB.GetItemCurrentLevel())
						{
							return itemDataA.GetItemCurrentLevel() < itemDataB.GetItemCurrentLevel();
						}						
					}
				}
				break;

			case ETypeSortRuleEquip::EquipGetTime:
				{
					if (ETESFilterSortingOrder::Descending == sortingOrder)
					{
						if (itemDataA.GetItemUnique() != itemDataB.GetItemUnique())
						{
							return itemDataA.GetItemUnique() > itemDataB.GetItemUnique();
						}
					}
					else
					{
						if (itemDataA.GetItemUnique() != itemDataB.GetItemUnique())
						{
							return itemDataA.GetItemUnique() < itemDataB.GetItemUnique();
						}						
					}
				}
				break;

			case ETypeSortRuleEquip::EquipTier:
				{
					//. Tier를 쓰지 않는것으로 보여 일단 아무 행동을 하지 않는다.
					//. 추후 Tier를 쓰지 않는것으로 확정되어 T_SortRuleEquip 테이블 에서 EquipTier를 제거하게 되면,
					//. 이 블럭은 삭제되어야 함.
				}
				break;

			default:
				{
					TESLOG(Error, TEXT("Not Supported Type : %d"), equipItemSortingType);
				}
				break;
			}
		}

		return false;
	});

	return itemDataList;
}

TArray<UTESGDGoodsItem*> TESUtilFilter::MakeSortedItemDataListByMisc(TArray<UTESGDGoodsItem*> itemDataList
															  , TArray<ETypeSortRuleMisc> itemSotingRules
															  , ETESFilterSortingOrder sortingOrder)
{
	//. 정렬할 것이 없다면 하지 않습니다.
	if (itemDataList.Num() <= 1)
	{
		return itemDataList;
	}

	//. 정렬을 시작합니다.
	itemDataList.StableSort([&](const UTESGDGoodsItem& itemDataA, const UTESGDGoodsItem& itemDataB)
	{
		for (int32 index = 0; index < itemSotingRules.Num(); ++index)
		{
			ETypeSortRuleMisc itemSortingType = itemSotingRules[index];
			switch (itemSortingType)
			{
			case ETypeSortRuleMisc::MiscDefault:
				{
					if (ETESFilterSortingOrder::Descending == sortingOrder)
					{
						if (itemDataA.GetItemTableId() != itemDataB.GetItemTableId())
						{
							return itemDataA.GetItemTableId() > itemDataB.GetItemTableId();
						}
					}
					else
					{
						if (itemDataA.GetItemTableId() != itemDataB.GetItemTableId())
						{
							return itemDataA.GetItemTableId() < itemDataB.GetItemTableId();
						}
					}
				}
				break;

			case ETypeSortRuleMisc::MiscAmount:
				{
					if (ETESFilterSortingOrder::Descending == sortingOrder)
					{
						if (itemDataA.GetItemCount() != itemDataB.GetItemCount())
						{
							return itemDataA.GetItemCount() > itemDataB.GetItemCount();
						}
					}
					else
					{
						if (itemDataA.GetItemCount() != itemDataB.GetItemCount())
						{
							return itemDataA.GetItemCount() < itemDataB.GetItemCount();
						}
					}
				}
				break;

			case ETypeSortRuleMisc::MiscGetTime:
				{
					if (ETESFilterSortingOrder::Descending == sortingOrder)
					{
						if (itemDataA.GetItemUnique() != itemDataB.GetItemUnique())
						{
							return itemDataA.GetItemUnique() > itemDataB.GetItemUnique();
						}
					}
					else
					{
						if (itemDataA.GetItemUnique() != itemDataB.GetItemUnique())
						{
							return itemDataA.GetItemUnique() < itemDataB.GetItemUnique();
						}
					}
				}
				break;

			default:
				{
					TESLOG(Error, TEXT("Not Supported Type : %d"), itemSortingType);
				}
				break;
			}
		}

		return false;
	});

	return itemDataList;
}

TArray<UTESGDGoodsItem*> TESUtilFilter::MakeSortedItemDataList(TArray<UTESGDGoodsItem*> itemDataList
														, int32 sortingElementId
														, ETESFilterSortingOrder sortingOrder)
{
	const FSortFilterElementRow& sortingElementTable = SharedTDBManager->GetSortFilterElementRow(sortingElementId);
	if (true == sortingElementTable.IsEmpty())
	{
		TESLOG(Error, TEXT("Invalid SortingElementTable. : %d"), sortingElementId);

		return itemDataList;
	}

	ETypeSortFilterElement sortingElementType = sortingElementTable.T_SortFilterElement;
	switch (sortingElementType)
	{
	case ETypeSortFilterElement::T_SortRuleEquip:
		{
			ETypeSortRuleEquip selectedSortingRule = sortingElementTable.EquipSortRule;

			TArray<ETypeSortRuleEquip> itemSortingRules = {};
			itemSortingRules.Add(selectedSortingRule);

			int32 maxCount = static_cast<int32>(ETypeSortRuleEquip::MAX);
			for (int32 index = 1; index < maxCount; ++index)
			{
				ETypeSortRuleEquip equipSortingRule = static_cast<ETypeSortRuleEquip>(index);
				if (equipSortingRule == selectedSortingRule)
				{
					continue;
				}

				itemSortingRules.Add(equipSortingRule);
			}

			return MakeSortedItemDataListByEquip(itemDataList, itemSortingRules, sortingOrder);
		}
		break;

	case ETypeSortFilterElement::T_SortRuleMisc:
		{
			ETypeSortRuleMisc selectedSortingRule = sortingElementTable.MiscSortRule;
			TArray<ETypeSortRuleMisc> itemSortingRuls = {};

			itemSortingRuls.Add(selectedSortingRule);

			int32 maxCount = static_cast<int32>(ETypeSortRuleMisc::MAX);
			for (int32 index = 1; index < maxCount; ++index)
			{
				ETypeSortRuleMisc miscSortingRule = static_cast<ETypeSortRuleMisc>(index);
				if (miscSortingRule == selectedSortingRule)
				{
					continue;
				}

				itemSortingRuls.Add(miscSortingRule);
			}

			return MakeSortedItemDataListByMisc(itemDataList, itemSortingRuls, sortingOrder);
		}
		break;

	case ETypeSortFilterElement::T_SortRuleCharacter:
		{
			//. 아이템 정렬에서 캐릭터 정렬은 지원하지 않습니다.
			TESLOG(Error, TEXT("Character sorting is not supported in item sorting. SortingElementId : %d, SortingElementType : %d")
						, sortingElementId
						, sortingElementType);
		}
		break;

	case ETypeSortFilterElement::T_SortRulePostBox:
		{
			//. 우편은 아이템으로 취급하지 않으므로, 지원하지 않습니다.
			TESLOG(Error, TEXT("Mail is not treated as an item, so it is not supported. SortingElementId : %d, SortingElementType : %d")
						, sortingElementId
						, sortingElementType);
		}
		break;

	case ETypeSortFilterElement::T_Grade:
	case ETypeSortFilterElement::T_CharRole:
	case ETypeSortFilterElement::T_CharAttack:
	case ETypeSortFilterElement::T_CharRange:
	case ETypeSortFilterElement::T_EquipSlot:
	case ETypeSortFilterElement::T_StatOption:
	case ETypeSortFilterElement::T_Use:
	case ETypeSortFilterElement::T_Goods:
	case ETypeSortFilterElement::T_Equip:
	case ETypeSortFilterElement::T_Material:
	case ETypeSortFilterElement::T_CharSpeciality:
		{
			//. 정렬에서는 필터타입은 지원하지 않습니다.
			TESLOG(Error, TEXT("Filter type is not supported in sorting. SortingElementId : %d, SortingElementType : %d")
						, sortingElementId
						, sortingElementType);
		}
		break;

	default:
		{
			//. 정의되지 않은 타입은 지원하지 않습니다.
			TESLOG(Error, TEXT("Not Supported Type. SortingElementId : %d, SortingElementType : %d")
						, sortingElementId
						, sortingElementType);
		}
		break;
	}

	return itemDataList;
}

TArray<class UTESGDGoodsItem*> TESUtilFilter::MakeFilteredEquipableItemDataList(TArray<class UTESGDGoodsItem*> itemDataList, class UTESGDCharacter* characterData, bool isFiltering)
{
	//. ───────────────────────────────────────────────────────────────────────────
	//. 필터링 조건
	//. ───────────────────────────────────────────────────────────────────────────
	//. 1. 캐릭터의 Base 스탯과 장비 착용제한의 Base 스탯을 비교합니다.
	//.   - 하나라도 만족하지 못하면 제외됩니다.
	//. 
	//. 2. 캐릭터가 해당 장비 착용에 필요한 최소 레벨 수치보다 낮아도 제외됩니다.
	//. ───────────────────────────────────────────────────────────────────────────
	if (true == isFiltering)
	{
		TArray<UTESGDGoodsItem*> filteredItemDatas = {};

		int32 maxCount = itemDataList.Num();
		for (int32 index = 0; index < maxCount; ++index)
		{
			UTESGDGoodsItem* itemData = itemDataList[index];
			if (false == ::IsValid(itemData))
			{
				continue;
			}

			const FItemRow& itemTableRow = itemData->GetItemTable();
			if (true == itemTableRow.IsEmpty())
			{
				TESLOG(Error, TEXT("Item Table Is Invalid"));
				continue;
			}
			if (false == ::IsValid(characterData))
			{
				TESLOG(Error, TEXT("Character Data Is Invalid"));
				continue;
			}

			int32 currentCharacterLevel = characterData->GetCharacterCurrentLevel();

			//. 캐릭터 현재 레벨이 장비 아이템 최소컷보다 낮으면 거른다.
			if (currentCharacterLevel < itemTableRow.RequiredLevel)
			{
				continue;
			}

			bool isEquipable = true;

			//. 장비 착용 제한 스탯보다 낮으면 못입는다.			
			//. 캐릭터 착용제한 스탯 및 수치
			int32 startCondition = static_cast<int32>(EItemColumn::RequiredStat01);
			int32 endCondition = static_cast<int32>(EItemColumn::RequiredStatMax);

			for (int32 i = startCondition; i <= endCondition; ++i)
			{				
				FTESStatTotal statTotal = itemTableRow.GetColumnData<FTESStatTotal>(static_cast<EItemColumn>(i));				

				if (ETypeStatBase::None == statTotal._statBaseType)
					continue;

				int32 characterStatBase = characterData->GetBaseStat(statTotal._statBaseType);
				int32 characterStatOption = characterData->GetOptionStat(statTotal._statOptionType);

				//. 착용 스탯보다 작으면 걸러준다.
				if (statTotal._value > characterStatBase + characterStatOption)
				{
					isEquipable = false;
				}
			}

			if (false == isEquipable)
			{
				continue;
			}


			filteredItemDatas.Add(itemData);
		}

		TESLOG(Display, TEXT("FilteredItemDataCount : %d"), filteredItemDatas.Num());

		return filteredItemDatas;
	}
	else
	{
		return itemDataList;
	}
}

bool TESUtilFilter::CheckFilteringForCharacterData(UTESGDCharacter* characterData, int32 filterElementId)
{
	if (nullptr == characterData)
	{
		TESLOG(Error, TEXT("Invalid CharacterData."));

		return false;
	}

	const FSortFilterElementRow& filterElementTable = SharedTDBManager->GetSortFilterElementRow(filterElementId);
	if (true == filterElementTable.IsEmpty())
	{
		TESLOG(Error, TEXT("Invalid FilterElementTable. : %d"), filterElementId);

		return false;
	}

	ETypeSortFilterElement filterElementType = filterElementTable.T_SortFilterElement;
	switch (filterElementType)
	{
	case ETypeSortFilterElement::T_Grade:
		{
			if (filterElementTable.GradeFilter == characterData->GetCharacterGrade())
			{
				return true;
			}
		}
		break;

	case ETypeSortFilterElement::T_CharRole:
		{
			if (filterElementTable.CharRoleFilter == characterData->GetCharacterRole())
			{
				return true;
			}
		}
		break;

	case ETypeSortFilterElement::T_CharAttack:
		{
			if (filterElementTable.CharAttack == characterData->GetCharacterAttackType())
			{
				return true;
			}
		}
		break;

	case ETypeSortFilterElement::T_CharRange:
		{
			if (filterElementTable.CharRangeFilter == characterData->GetCharacterRange())
			{
				return true;
			}
		}
		break;

	case ETypeSortFilterElement::T_CharSpeciality:
		{
			if (filterElementTable.CharSpeciality == characterData->GetCharacterSpeciality())
			{
				return true;
			}
		}
		break;

	case ETypeSortFilterElement::T_SortRuleCharacter:
	case ETypeSortFilterElement::T_SortRuleEquip:
	case ETypeSortFilterElement::T_SortRuleMisc:
	case ETypeSortFilterElement::T_SortRulePostBox:
		{
			//. 필터링에서 정렬 타입은 지원하지 않습니다.
			TESLOG(Error, TEXT("Sorting type is not supported in filtering. ElementTableId : %d"), filterElementId);
		}
		break;

	case ETypeSortFilterElement::T_EquipSlot:
	case ETypeSortFilterElement::T_StatOption:
	case ETypeSortFilterElement::T_Use:
	case ETypeSortFilterElement::T_Goods:
	case ETypeSortFilterElement::T_Equip:
		{
			//. 캐릭터 필터링에서는 아이템에 대한 필터링은 지원하지 않습니다.
			TESLOG(Error, TEXT("Filtering on items is not supported in character filtering. ElementTableId : %d"), filterElementId);
		}
		break;

	default:
		{
			//. 지원하지 않는 타입입니다.
			TESLOG(Error, TEXT("Not Supported Type. id : %d, Type : %d"), filterElementId, filterElementType);
		}
		break;
	}

	//. 일치하는 목록이 없습니다.
	return false;
}

TArray<UTESGDCharacter*> TESUtilFilter::MakeFilteredCharacterDataList(TArray<UTESGDCharacter*> characterDataList
	, TMap<int32, FTESFilterFilteringGroupData> filteringData
	, bool isFiltering)
{
	//. ───────────────────────────────────────────────────────────────────────────
	//. 필터링 조건
	//. ───────────────────────────────────────────────────────────────────────────
	//. 1. 필터 그룹들끼리는 AND 조건입니다.
	//.   - 하나라도 만족하지 않는다면 추가하지 않습니다.
	//.
	//. 2. 필터 그룹 내의 요소들끼리는 OR 조건입니다.
	//.   - 하나라도 만족한다면 조건에 충족했다고 판단합니다.
	//. ───────────────────────────────────────────────────────────────────────────
	if (true == isFiltering && filteringData.Num() > 0)
	{
		TArray<UTESGDCharacter*> filteredCharacterDatas = {};

		int32 maxCount = characterDataList.Num();
		for (int32 index = 0; index < maxCount; ++index)
		{
			UTESGDCharacter* characterData = characterDataList[index];
			if (false == ::IsValid(characterData))
			{
				continue;
			}

			//. 필터링의 모든 그룹들을 순회합니다.
			bool isAppendItem = true;
			for (TPair<int32, FTESFilterFilteringGroupData> child : filteringData)
			{
				bool isCheckFiltering = false;

				//. 그룹 내의 해당 요소들 중에서 만족하는 내용이 있는지를 체크합니다.
				FTESFilterFilteringGroupData filteringGroupData = child.Value;
				for (int32 filterElementId : filteringGroupData._elementIdList)
				{
					isCheckFiltering = TESUtilFilter::CheckFilteringForCharacterData(characterData, filterElementId);
					if (true == isCheckFiltering)
					{
						break;
					}
				}

				//. 해당 그룹에서 만족하는 데이터가 없었습니다.
				if (false == isCheckFiltering)
				{
					isAppendItem = false;
				}
			}

			//. 모든 조건에 만족하는 데이터를 추가합니다.
			if (true == isAppendItem)
			{
				filteredCharacterDatas.Add(characterData);
			}
		}

		TESLOG(Display, TEXT("FilteredItemDataCount : %d"), filteredCharacterDatas.Num());

		return filteredCharacterDatas;
	}
	else
	{
		return characterDataList;
	}
}

TArray<UTESGDCharacter*> TESUtilFilter::MakeSortedCharacterDataList(TArray<UTESGDCharacter*> characterDataList
																  , TArray<ETypeSortRuleCharacter> sotingRules
																  , ETESFilterSortingOrder sortingOrder)
{
	//. 정렬할 것이 없다면 하지 않습니다.
	if (characterDataList.Num() <= 1)
	{
		return characterDataList;
	}

	//. 정렬을 시작합니다.
	characterDataList.StableSort([&](const UTESGDCharacter& characterDataA, const UTESGDCharacter& characterDataB)
	{
		for (int32 index = 0; index < sotingRules.Num(); ++index)
		{
			ETypeSortRuleCharacter sortingType = sotingRules[index];
			switch (sortingType)
			{
			case ETypeSortRuleCharacter::CharPower:
				{
					if (ETESFilterSortingOrder::Descending == sortingOrder)
					{
						if(characterDataA.GetCharacterBattlePower() > characterDataB.GetCharacterBattlePower())
						{
							return true;
						}
						else if (characterDataA.GetCharacterBattlePower() < characterDataB.GetCharacterBattlePower())
						{
							return false;
						}
						else
						{
							continue;
						}
					}
					else
					{
						if (characterDataA.GetCharacterBattlePower() < characterDataB.GetCharacterBattlePower())
						{
							return true;
						}
						else if (characterDataA.GetCharacterBattlePower() > characterDataB.GetCharacterBattlePower())
						{
							return false;
						}
						else
						{
							continue;
						}
					}
				}
				break;

			case ETypeSortRuleCharacter::CharGrade:
				{
					if (ETESFilterSortingOrder::Descending == sortingOrder)
					{
						if (characterDataA.GetCharacterGrade() > characterDataB.GetCharacterGrade())
						{
							return true;
						}
						else if (characterDataA.GetCharacterGrade() < characterDataB.GetCharacterGrade())
						{
							return false;
						}
						else
						{
							continue;
						}
					}
					else
					{
						if (characterDataA.GetCharacterGrade() < characterDataB.GetCharacterGrade())
						{
							return true;
						}
						else if (characterDataA.GetCharacterGrade() > characterDataB.GetCharacterGrade())
						{
							return false;
						}
						else
						{
							continue;
						}
					}
				}
				break;

			case ETypeSortRuleCharacter::CharLevel:
				{
					if (ETESFilterSortingOrder::Descending == sortingOrder)
					{
						if (characterDataA.GetCharacterCurrentLevel() > characterDataB.GetCharacterCurrentLevel())
						{
							return true;
						}
						else if (characterDataA.GetCharacterCurrentLevel() < characterDataB.GetCharacterCurrentLevel())
						{
							return false;
						}
						else
						{
							continue;
						}
					}
					else
					{
						if (characterDataA.GetCharacterCurrentLevel() < characterDataB.GetCharacterCurrentLevel())
						{
							return true;
						}
						else if (characterDataA.GetCharacterCurrentLevel() > characterDataB.GetCharacterCurrentLevel())
						{
							return false;
						}
						else
						{
							continue;
						}
					}
				}
				break;

			/*case ETypeSortRuleCharacter::CharRank:
				{
					if (ETESFilterSortingOrder::Descending == sortingOrder)
					{
						return characterDataA.GetCharacterRank() > characterDataB.GetCharacterRank();
					}
					else
					{
						return characterDataA.GetCharacterRank() < characterDataB.GetCharacterRank();
					}
				}
				break;*/

			case ETypeSortRuleCharacter::CharAwakening:
				{
					if (ETESFilterSortingOrder::Descending == sortingOrder)
					{
						if (characterDataA.GetCharacterCurrentAwakenLevel() > characterDataB.GetCharacterCurrentAwakenLevel())
						{
							return true;
						}
						else if (characterDataA.GetCharacterCurrentAwakenLevel() < characterDataB.GetCharacterCurrentAwakenLevel())
						{
							return false;
						}
						else
						{
							continue;
						}
					}
					else
					{
						if (characterDataA.GetCharacterCurrentAwakenLevel() < characterDataB.GetCharacterCurrentAwakenLevel())
						{
							return true;
						}
						else if (characterDataA.GetCharacterCurrentAwakenLevel() > characterDataB.GetCharacterCurrentAwakenLevel())
						{
							return false;
						}
						else
						{
							continue;
						}
					}
				}
				break;

			case ETypeSortRuleCharacter::CharAttack:
				{
					int32 valueA = TESUtilStatus::CalculateStatValueByTotalStatContents(&characterDataA, ETypeStatOption::PhysicalAttack_Value) + TESUtilStatus::CalculateStatValueByTotalStatContents(&characterDataA, ETypeStatOption::MagicalAttack_Value);
					int32 valueB = TESUtilStatus::CalculateStatValueByTotalStatContents(&characterDataB, ETypeStatOption::PhysicalAttack_Value) + TESUtilStatus::CalculateStatValueByTotalStatContents(&characterDataB, ETypeStatOption::MagicalAttack_Value);

					if (ETESFilterSortingOrder::Descending == sortingOrder)
					{
						if (valueA > valueB)
						{
							return true;
						}
						else if (valueA < valueB)
						{
							return false;
						}
						else
						{
							continue;
						}
					}
					else
					{
						if (valueA < valueB)
						{
							return true;
						}
						else if (valueA > valueB)
						{
							return false;
						}
						else
						{
							continue;
						}
					}
				}
				break;

			case ETypeSortRuleCharacter::CharDef:
				{
					int32 valueA = TESUtilStatus::CalculateStatValueByTotalStatContents(&characterDataA, ETypeStatOption::PhysicalDefence_Value) + TESUtilStatus::CalculateStatValueByTotalStatContents(&characterDataA, ETypeStatOption::MagicalDefence_Value);
					int32 valueB = TESUtilStatus::CalculateStatValueByTotalStatContents(&characterDataB, ETypeStatOption::PhysicalDefence_Value) + TESUtilStatus::CalculateStatValueByTotalStatContents(&characterDataB, ETypeStatOption::MagicalDefence_Value);

					if (ETESFilterSortingOrder::Descending == sortingOrder)
					{
						if (valueA > valueB)
						{
							return true;
						}
						else if (valueA < valueB)
						{
							return false;
						}
						else
						{
							continue;
						}
					}
					else
					{
						if (valueA < valueB)
						{
							return true;
						}
						else if (valueA > valueB)
						{
							return false;
						}
						else
						{
							continue;
						}
					}
				}
				break;

			case ETypeSortRuleCharacter::CharHP:
				{
					int32 valueA = TESUtilStatus::CalculateStatValueByTotalStatContents(&characterDataA, ETypeStatOption::HP_Value);
					int32 valueB = TESUtilStatus::CalculateStatValueByTotalStatContents(&characterDataB, ETypeStatOption::HP_Value);

					if (ETESFilterSortingOrder::Descending == sortingOrder)
					{
						if (valueA > valueB)
						{
							return true;
						}
						else if (valueA < valueB)
						{
							return false;
						}
						else
						{
							continue;
						}
					}
					else
					{
						if (valueA < valueB)
						{
							return true;
						}
						else if (valueA > valueB)
						{
							return false;
						}
						else
						{
							continue;
						}
					}
				}
				break;

			case ETypeSortRuleCharacter::CharCriRate:
				{
					int32 valueA = TESUtilStatus::CalculateStatValueByTotalStatContents(&characterDataA, ETypeStatOption::PhysicalCriticalRate_Value) + TESUtilStatus::CalculateStatValueByTotalStatContents(&characterDataA, ETypeStatOption::MagicalCriticalRate_Value);
					int32 valueB = TESUtilStatus::CalculateStatValueByTotalStatContents(&characterDataB, ETypeStatOption::PhysicalCriticalRate_Value) + TESUtilStatus::CalculateStatValueByTotalStatContents(&characterDataB, ETypeStatOption::MagicalCriticalRate_Value);

					if (ETESFilterSortingOrder::Descending == sortingOrder)
					{
						if (valueA > valueB)
						{
							return true;
						}
						else if (valueA < valueB)
						{
							return false;
						}
						else
						{
							continue;
						}
					}
					else
					{
						if (valueA < valueB)
						{
							return true;
						}
						else if (valueA > valueB)
						{
							return false;
						}
						else
						{
							continue;
						}
					}
				}
				break;

			case ETypeSortRuleCharacter::CharCriDamage:
				{
					int32 valueA = TESUtilStatus::CalculateStatValueByTotalStatContents(&characterDataA, ETypeStatOption::PhysicalCriticalDamage_Value) + TESUtilStatus::CalculateStatValueByTotalStatContents(&characterDataA, ETypeStatOption::MagicalCriticalDamage_Value);
					int32 valueB = TESUtilStatus::CalculateStatValueByTotalStatContents(&characterDataB, ETypeStatOption::PhysicalCriticalDamage_Value) + TESUtilStatus::CalculateStatValueByTotalStatContents(&characterDataB, ETypeStatOption::MagicalCriticalDamage_Value);

					if (ETESFilterSortingOrder::Descending == sortingOrder)
					{
						if (valueA > valueB)
						{
							return true;
						}
						else if (valueA < valueB)
						{
							return false;
						}
						else
						{
							continue;
						}
					}
					else
					{
						if (valueA < valueB)
						{
							return true;
						}
						else if (valueA > valueB)
						{
							return false;
						}
						else
						{
							continue;
						}
					}
				}
				break;

			case ETypeSortRuleCharacter::CharDefault:
				{
					if (ETESFilterSortingOrder::Descending == sortingOrder)
					{
						if (characterDataA.GetCharacterTableId() > characterDataB.GetCharacterTableId())
						{
							return true;
						}
						else if (characterDataA.GetCharacterTableId() < characterDataB.GetCharacterTableId())
						{
							return false;
						}
						else
						{
							continue;
						}
					}
					else
					{
						if (characterDataA.GetCharacterTableId() < characterDataB.GetCharacterTableId())
						{
							return true;
						}
						else if (characterDataA.GetCharacterTableId() > characterDataB.GetCharacterTableId())
						{
							return false;
						}
						else
						{
							continue;
						}
					}
					
				}
				break;

			case ETypeSortRuleCharacter::CharGetTime:
				{		
					if (ETESFilterSortingOrder::Descending == sortingOrder)
					{
						if (characterDataA.GetCharacterCreateTime() > characterDataB.GetCharacterCreateTime())
						{
							return true;
						}
						else if (characterDataA.GetCharacterCreateTime() < characterDataB.GetCharacterCreateTime())
						{
							return false;
						}
						else
						{
							continue;
						}
					}
					else
					{
						if (characterDataA.GetCharacterCreateTime() < characterDataB.GetCharacterCreateTime())
						{
							return true;
						}
						else if (characterDataA.GetCharacterCreateTime() > characterDataB.GetCharacterCreateTime())
						{
							return false;
						}
						else
						{
							continue;
						}
					}
				}
				break;

			case ETypeSortRuleCharacter::CharSTR:
				{
					int32 valueA = TESUtilStatus::CalculateStatValueByTotalStatContents(&characterDataA, ETypeStatOption::STR_Value);
					int32 valueB = TESUtilStatus::CalculateStatValueByTotalStatContents(&characterDataB, ETypeStatOption::STR_Value);

					if (ETESFilterSortingOrder::Descending == sortingOrder)
					{
						if (valueA > valueB)
						{
							return true;
						}
						else if (valueA < valueB)
						{
							return false;
						}
						else
						{
							continue;
						}
					}
					else
					{
						if (valueA < valueB)
						{
							return true;
						}
						else if (valueA > valueB)
						{
							return false;
						}
						else
						{
							continue;
						}
					}
				}
				break;

			case ETypeSortRuleCharacter::CharDEX:
				{
					int32 valueA = TESUtilStatus::CalculateStatValueByTotalStatContents(&characterDataA, ETypeStatOption::DEX_Value);
					int32 valueB = TESUtilStatus::CalculateStatValueByTotalStatContents(&characterDataB, ETypeStatOption::DEX_Value);

					if (ETESFilterSortingOrder::Descending == sortingOrder)
					{
						if (valueA > valueB)
						{
							return true;
						}
						else if (valueA < valueB)
						{
							return false;
						}
						else
						{
							continue;
						}
					}
					else
					{
						if (valueA < valueB)
						{
							return true;
						}
						else if (valueA > valueB)
						{
							return false;
						}
						else
						{
							continue;
						}
					}
				}
				break;

			case ETypeSortRuleCharacter::CharCON:
				{
					int32 valueA = TESUtilStatus::CalculateStatValueByTotalStatContents(&characterDataA, ETypeStatOption::CON_Value);
					int32 valueB = TESUtilStatus::CalculateStatValueByTotalStatContents(&characterDataB, ETypeStatOption::CON_Value);

					if (ETESFilterSortingOrder::Descending == sortingOrder)
					{
						if (valueA > valueB)
						{
							return true;
						}
						else if (valueA < valueB)
						{
							return false;
						}
						else
						{
							continue;
						}
					}
					else
					{
						if (valueA < valueB)
						{
							return true;
						}
						else if (valueA > valueB)
						{
							return false;
						}
						else
						{
							continue;
						}
					}
				}
				break;

			case ETypeSortRuleCharacter::CharINT:
				{
					int32 valueA = TESUtilStatus::CalculateStatValueByTotalStatContents(&characterDataA, ETypeStatOption::INT_Value);
					int32 valueB = TESUtilStatus::CalculateStatValueByTotalStatContents(&characterDataB, ETypeStatOption::INT_Value);

					if (ETESFilterSortingOrder::Descending == sortingOrder)
					{
						if (valueA > valueB)
						{
							return true;
						}
						else if (valueA < valueB)
						{
							return false;
						}
						else
						{
							continue;
						}
					}
					else
					{
						if (valueA < valueB)
						{
							return true;
						}
						else if (valueA > valueB)
						{
							return false;
						}
						else
						{
							continue;
						}
					}
				}
				break;
			case ETypeSortRuleCharacter::CharWIS:
				{
					int32 valueA = TESUtilStatus::CalculateStatValueByTotalStatContents(&characterDataA, ETypeStatOption::WIS_Value);
					int32 valueB = TESUtilStatus::CalculateStatValueByTotalStatContents(&characterDataB, ETypeStatOption::WIS_Value);

					if (ETESFilterSortingOrder::Descending == sortingOrder)
					{
						if (valueA > valueB)
						{
							return true;
						}
						else if (valueA < valueB)
						{
							return false;
						}
						else
						{
							continue;
						}
					}
					else
					{
						if (valueA < valueB)
						{
							return true;
						}
						else if (valueA > valueB)
						{
							return false;
						}
						else
						{
							continue;
						}
					}
				}
				break;

			case ETypeSortRuleCharacter::CharWoods:
				{
					return CheckFacilitySortingOrderByCharacter(ETypeStatBase::Woods, characterDataA, characterDataB, sortingOrder);
				}
				break;

			case ETypeSortRuleCharacter::CharMineral:
				{
					return CheckFacilitySortingOrderByCharacter(ETypeStatBase::Mineral, characterDataA, characterDataB, sortingOrder);
				}
				break;

			case ETypeSortRuleCharacter::CharLeather:
				{
					return CheckFacilitySortingOrderByCharacter(ETypeStatBase::Leather, characterDataA, characterDataB, sortingOrder);
				}
				break;

			case ETypeSortRuleCharacter::CharManaring:
				{
					return CheckFacilitySortingOrderByCharacter(ETypeStatBase::Manaring, characterDataA, characterDataB, sortingOrder);
				}
				break;
			case ETypeSortRuleCharacter::CharCrafting:
				{
					return CheckFacilitySortingOrderByCharacter(ETypeStatBase::Crafting, characterDataA, characterDataB, sortingOrder);
				}
				break;

			default:
				{
					TESLOG(Error, TEXT("Not Supported Type : %d"), sortingType);
				}
				break;
			}
		}

		return false;
	});

	return characterDataList;
}

TArray<UTESGDCharacter*> TESUtilFilter::MakeSortedCharacterDataList(TArray<UTESGDCharacter*> characterDataList
																  , int32 sortingElementId
																  , ETESFilterSortingOrder sortingOrder)
{
	const FSortFilterElementRow& sortingElementTable = SharedTDBManager->GetSortFilterElementRow(sortingElementId);
	if (true == sortingElementTable.IsEmpty())
	{
		TESLOG(Error, TEXT("Invalid SortingElementTable. : %d"), sortingElementId);

		return characterDataList;
	}

	ETypeSortFilterElement sortingElementType = sortingElementTable.T_SortFilterElement;
	switch (sortingElementType)
	{
	case ETypeSortFilterElement::T_SortRuleCharacter:
		{
			ETypeSortRuleCharacter selectedSortingRule = sortingElementTable.CharacterSortRule;
			TArray<ETypeSortRuleCharacter> sortingRuls = {};

			sortingRuls.Add(selectedSortingRule);

			int32 maxCount = static_cast<int32>(ETypeSortRuleCharacter::MAX);
			for (int32 index = 1; index < maxCount; ++index)
			{
				ETypeSortRuleCharacter sortingRule = static_cast<ETypeSortRuleCharacter>(index);
				if (sortingRule == selectedSortingRule)
				{
					continue;
				}

				sortingRuls.Add(sortingRule);
			}

			return MakeSortedCharacterDataList(characterDataList, sortingRuls, sortingOrder);
		}
		break;

	case ETypeSortFilterElement::T_SortRuleEquip:
	case ETypeSortFilterElement::T_SortRuleMisc:
	case ETypeSortFilterElement::T_SortRulePostBox:
	case ETypeSortFilterElement::T_Grade:
	case ETypeSortFilterElement::T_CharRole:
	case ETypeSortFilterElement::T_CharAttack:
	case ETypeSortFilterElement::T_CharRange:
	case ETypeSortFilterElement::T_EquipSlot:
	case ETypeSortFilterElement::T_StatOption:
	case ETypeSortFilterElement::T_Use:
	case ETypeSortFilterElement::T_Goods:
	case ETypeSortFilterElement::T_Equip:
	case ETypeSortFilterElement::T_Material:
		{
			//. 캐릭터 정렬에서 필터에 대한 타입은 지원하지 않습니다.
			TESLOG(Error, TEXT("Types for filters in character sort are not supported. ElementId : %d, ElementType : %d")
						, sortingElementId
						, sortingElementType);
		}
		break;

	default:
		{
			//. 정의되지 않은 타입은 지원하지 않습니다.
			TESLOG(Error, TEXT("Not Supported Type. SortingElementId : %d, SortingElementType : %d")
						, sortingElementId
						, sortingElementType);
		}
		break;
	}

	return characterDataList;
}

bool TESUtilFilter::CheckFacilitySortingOrderByCharacter(ETypeStatBase baseStatType, const UTESGDCharacter& characterDataA, const UTESGDCharacter& characterDataB, ETESFilterSortingOrder sortingOrder)
{
	TArray<ETypePassiveSkillEffect> validSkillTypes = {};

	switch (baseStatType)
	{
	case ETypeStatBase::Woods:
		validSkillTypes.Add(ETypePassiveSkillEffect::WoodsProduce);
		validSkillTypes.Add(ETypePassiveSkillEffect::LuckyWoodsProduce);
		break;
	case ETypeStatBase::Leather:
		validSkillTypes.Add(ETypePassiveSkillEffect::LeatherProduce);
		validSkillTypes.Add(ETypePassiveSkillEffect::LuckyLeatherProduce);
		break;
	case ETypeStatBase::Mineral:
		validSkillTypes.Add(ETypePassiveSkillEffect::MineralProduce);
		validSkillTypes.Add(ETypePassiveSkillEffect::LuckyMineralProduce);
		break;
	case ETypeStatBase::Manaring:
		validSkillTypes.Add(ETypePassiveSkillEffect::ManaringProduce);
		validSkillTypes.Add(ETypePassiveSkillEffect::LuckyManaringProduce);
		break;
	case ETypeStatBase::Crafting:
		validSkillTypes.Add(ETypePassiveSkillEffect::LuckyCrafting);
		validSkillTypes.Add(ETypePassiveSkillEffect::ChoLuckyCrafting);
		break;
	}

	TArray<int32> characteDataASkillIds = characterDataA.GetPassiveSkillIds();
	TArray<int32> characteDataBSkillIds = characterDataB.GetPassiveSkillIds();

	int32 characteDataAValidSkillNum = 0;
	int32 characteDataBValidSkillNum = 0;

	for (int32 i = 0; i < characteDataASkillIds.Num(); ++i)
	{
		const FPassiveSkillRow& passiveSkill = SharedTDBManager->GetPassiveSkillRow(characteDataASkillIds[i]);
		if (true == passiveSkill.IsEmpty())
		{
			continue;
		}

		if (validSkillTypes.Contains(passiveSkill.PassiveEffectType))
		{
			characteDataAValidSkillNum += 1;
		}
	}

	for (int32 i = 0; i < characteDataBSkillIds.Num(); ++i)
	{
		const FPassiveSkillRow& passiveSkill = SharedTDBManager->GetPassiveSkillRow(characteDataBSkillIds[i]);
		if (true == passiveSkill.IsEmpty())
		{
			continue;
		}

		if (validSkillTypes.Contains(passiveSkill.PassiveEffectType))
		{
			characteDataBValidSkillNum += 1;
		}
	}

	int32 characterDataATotalStat = characterDataA.GetBaseStat(baseStatType) + characterDataA.GetOptionStat(TESUtilStatus::GetStatBaseAsStatOption(baseStatType));
	int32 characterDataBTotalStat = characterDataB.GetBaseStat(baseStatType) + characterDataB.GetOptionStat(TESUtilStatus::GetStatBaseAsStatOption(baseStatType));

	if (ETESFilterSortingOrder::Descending == sortingOrder)
	{
		if (characterDataATotalStat > characterDataBTotalStat)
		{
			return true;
		}
		else if (characterDataATotalStat < characterDataBTotalStat)
		{
			return false;
		}
		else
		{
			return characteDataAValidSkillNum > characteDataBValidSkillNum;
		}
	}
	else
	{
		if (characterDataATotalStat < characterDataBTotalStat)
		{
			return true;
		}
		else if (characterDataATotalStat > characterDataBTotalStat)
		{
			return false;
		}
		else
		{
			return characteDataAValidSkillNum < characteDataBValidSkillNum;
		}
	}
}


TArray<UTESGDMail*> TESUtilFilter::MakeSortedMailDataListByMailBox(TArray<UTESGDMail*> mailDatas
																 , TArray<ETypeSortRulePostBox> mailSotingRules
																 , ETESFilterSortingOrder sortingOrder)
{
	//. 정렬할 것이 없다면 하지 않습니다.
	if (mailDatas.Num() <= 1)
	{
		return mailDatas;
	}

	//. 정렬을 시작합니다.
	mailDatas.Sort([&](const UTESGDMail& mailDataA, const UTESGDMail& mailDataB)
	{
		for (int32 index = 0; index < mailSotingRules.Num(); ++index)
		{
			ETypeSortRulePostBox sortingType = mailSotingRules[index];
			switch (sortingType)
			{
			case ETypeSortRulePostBox::PostBoxGetTime:
				{
					if (ETESFilterSortingOrder::Descending == sortingOrder)
					{
						return mailDataA.GetMailUnique() > mailDataB.GetMailUnique();
					}
					else
					{
						return mailDataA.GetMailUnique() < mailDataB.GetMailUnique();
					}
				}
				break;
			}
		}

		return false;
	});

	return mailDatas;
}


TArray<UTESGDMail*> TESUtilFilter::MakeSortedMailDataList(TArray<UTESGDMail*> mailDatas
														, int32 sortingElementId
														, ETESFilterSortingOrder sortingOrder)
{

	const FSortFilterElementRow& sortingElementTable = SharedTDBManager->GetSortFilterElementRow(sortingElementId);
	if (true == sortingElementTable.IsEmpty())
	{
		TESLOG(Error, TEXT("Invalid SortingElementTable. : %d"), sortingElementId);

		return mailDatas;
	}

	ETypeSortFilterElement sortingElementType = sortingElementTable.T_SortFilterElement;
	switch (sortingElementType)
	{
	case ETypeSortFilterElement::T_SortRulePostBox:
		{
			ETypeSortRulePostBox selectedSortingRule = sortingElementTable.PostBoxSortRule;

			TArray<ETypeSortRulePostBox> sortingRules = {};
			sortingRules.Add(selectedSortingRule);

			int32 maxCount = static_cast<int32>(ETypeSortRulePostBox::MAX);
			for (int32 index = 1; index < maxCount; ++index)
			{
				ETypeSortRulePostBox sortingRule = static_cast<ETypeSortRulePostBox>(index);
				if (sortingRule == selectedSortingRule)
				{
					continue;
				}

				sortingRules.Add(sortingRule);
			}

			return MakeSortedMailDataListByMailBox(mailDatas, sortingRules, sortingOrder);
		}
		break;

	default:
		{
			//. 정의되지 않은 타입은 지원하지 않습니다.
			TESLOG(Error, TEXT("Not Supported Type. SortingElementId : %d, SortingElementType : %d")
				, sortingElementId
				, sortingElementType);
		}
		break;
	}

	return mailDatas;
}

bool TESUtilFilter::CheckFilteringForROSStarData(int32 rosStarId, int32 filterElementId)
{
	const FSortFilterElementRow& filterElementTable = SharedTDBManager->GetSortFilterElementRow(filterElementId);
	if (true == filterElementTable.IsEmpty())
	{
		TESLOG(Error, TEXT("Invalid FilterElementTable. : %d"), filterElementId);

		return false;
	}

	ETypeSortFilterElement filterElementType = filterElementTable.T_SortFilterElement;
	switch (filterElementType)
	{
	case ETypeSortFilterElement::T_FilterRuleStar:
		{
			const FRingOfStellaStarRow& rosStarRow = SharedTDBManager->GetRingOfStellaStarRow(rosStarId);
			if (true == rosStarRow.IsEmpty())
			{
				TESLOG(Error, TEXT("Invalid ROS Star Row : %d"), rosStarId);

				return false;
			}

			switch (filterElementTable.StarFilter)
			{
			case ETypeFilterRuleStar::T_CharAttribute:
				{
					if (ETypeCharAttribute::None != rosStarRow.TargetAttributeType)
					{
						return true;
					}
				}
				break;

			case ETypeFilterRuleStar::T_CharRange:
				{
					if (ETypeCharRange::None != rosStarRow.TargetRangeType)
					{
						return true;
					}
				}
				break;

			case ETypeFilterRuleStar::T_CharRole:
				{
					if (ETypeCharRole::None != rosStarRow.TargetRoleType)
					{
						return true;
					}
				}
				break;

			case ETypeFilterRuleStar::T_CharSpeciality:
				{
					if (ETypeCharSpeciality::None != rosStarRow.TargetSpecialityType)
					{
						return true;
					}
				}
				break;

			default:
				{
					//. 지원하는 타입이 아닙니다.
					TESLOG(Error, TEXT("Not Supported FilterRuleStar Type. filterElementId : %d, filterElementType : %d, StarFilterType : %s")
								, filterElementId, filterElementType, *TESUtilString::GetEnumAsString(filterElementTable.StarFilter));
				}
				break;
			}
		}
		break;

	default:
		{
			//. 지원하는 타입이 아닙니다.
			TESLOG(Error, TEXT("Not Supported Type. id : %d, Type : %d"), filterElementId, filterElementType);
		}
		break;
	}

	//. 일치하는 목록이 없습니다.
	return false;
}

TArray<int32> TESUtilFilter::MakeFilteredROSStarDatas(TArray<int32> rosStarDatas
													, TMap<int32, FTESFilterFilteringGroupData> filteringData
													, bool isFiltering)
{
	if (true == isFiltering && filteringData.Num() > 0)
	{
		TArray<int32> filteredItemDatas = {};

		int32 maxCount = rosStarDatas.Num();
		for (int32 index = 0; index < maxCount; ++index)
		{
			int32 rosStarId = rosStarDatas[index];

			//. 필터링의 모든 그룹들을 순회합니다.
			bool isAppendItem = true;
			for (TPair<int32, FTESFilterFilteringGroupData> child : filteringData)
			{
				bool isCheckFiltering = false;

				//. 그룹 내의 해당 요소들 중에서 만족하는 내용이 있는지를 체크합니다.
				FTESFilterFilteringGroupData filteringGroupData = child.Value;
				for (int32 filterElementId : filteringGroupData._elementIdList)
				{
					isCheckFiltering = TESUtilFilter::CheckFilteringForROSStarData(rosStarId, filterElementId);
					if (true == isCheckFiltering)
					{
						break;
					}
				}

				//. 해당 그룹에서 만족하는 데이터가 없었습니다.
				if (false == isCheckFiltering)
				{
					isAppendItem = false;
				}
			}

			//. 모든 조건에 만족하는 데이터를 추가합니다.
			if (true == isAppendItem)
			{
				filteredItemDatas.Add(rosStarId);
			}
		}

		TESLOG(Display, TEXT("FilteredItemDataCount : %d"), filteredItemDatas.Num());

		return filteredItemDatas;
	}
	else
	{
		return rosStarDatas;
	}

}

TArray<int32> TESUtilFilter::MakeSortedROSStarDatas(TArray<int32> rosStarDatas
											   , int32 sortingElementId
											   , ETESFilterSortingOrder sortingOrder)
{
	const FSortFilterElementRow& sortingElementTable = SharedTDBManager->GetSortFilterElementRow(sortingElementId);
	if (true == sortingElementTable.IsEmpty())
	{
		TESLOG(Error, TEXT("Invalid SortingElementTable. : %d"), sortingElementId);

		return rosStarDatas;
	}

	ETypeSortFilterElement sortingElementType = sortingElementTable.T_SortFilterElement;
	switch (sortingElementType)
	{
	case ETypeSortFilterElement::T_SortRuleStar:
		{
			ETypeSortRuleStar selectedSortingRule = sortingElementTable.StarSortRule;

			TArray<ETypeSortRuleStar> sortingRules = {};
			sortingRules.Add(selectedSortingRule);

			int32 maxCount = static_cast<int32>(ETypeSortRuleStar::MAX);
			for (int32 index = 1; index < maxCount; ++index)
			{
				ETypeSortRuleStar sortingRule = static_cast<ETypeSortRuleStar>(index);
				if (sortingRule == selectedSortingRule)
				{
					continue;
				}

				sortingRules.Add(sortingRule);
			}

			return MakeSortedROSStarDatasByROSStar(rosStarDatas, sortingRules, sortingOrder);
		}
		break;

	default:
		{
			//. 정의되지 않은 타입은 지원하지 않습니다.
			TESLOG(Error, TEXT("Not Supported Type. SortingElementId : %d, SortingElementType : %d")
						, sortingElementId
						, sortingElementType);
		}
		break;
	}

	return rosStarDatas;
}

TArray<int32> TESUtilFilter::MakeSortedROSStarDatasByROSStar(TArray<int32> rosStarDatas
														, TArray<ETypeSortRuleStar> sotingRules
														, ETESFilterSortingOrder sortingOrder)
{
	//. 정렬할 것이 없다면 하지 않습니다.
	if (rosStarDatas.Num() <= 1)
	{
		return rosStarDatas;
	}

	//. 정렬을 시작합니다.
	rosStarDatas.StableSort([&](const int32 rosStarDataA, const int32 rosStarDataB)
	{
		for (int32 index = 0; index < sotingRules.Num(); ++index)
		{
			ETypeSortRuleStar sortingType = sotingRules[index];
			switch (sortingType)
			{
			case ETypeSortRuleStar::StarDefault:
				{
					const FRingOfStellaStarRow& rosStarRowA = SharedTDBManager->GetRingOfStellaStarRow(rosStarDataA);
					if (true == rosStarRowA.IsEmpty())
					{
						TESLOG(Error, TEXT("Invalid ROS Star Row : %d"), rosStarDataA);

						return false;
					}

					const FRingOfStellaStarRow& rosStarRowB = SharedTDBManager->GetRingOfStellaStarRow(rosStarDataB);
					if (true == rosStarRowB.IsEmpty())
					{
						TESLOG(Error, TEXT("Invalid ROS Star Row : %d"), rosStarDataB);

						return false;
					}

					if (ETESFilterSortingOrder::Descending == sortingOrder)
					{
						return rosStarRowA.Id > rosStarRowB.Id;
					}
					else
					{
						return rosStarRowA.Id < rosStarRowB.Id;
					}
				}
				break;

			case ETypeSortRuleStar::StarLevel:
				{
					int32 rosStarALevel = SharedROSManager->GetCurrentStarLevel(rosStarDataA);
					int32 rosStarBLevel = SharedROSManager->GetCurrentStarLevel(rosStarDataB);

					if (ETESFilterSortingOrder::Descending == sortingOrder)
					{
						return rosStarALevel > rosStarBLevel;
					}
					else
					{
						return rosStarALevel < rosStarBLevel;
					}
				}
				break;

			default:
				{
					TESLOG(Error, TEXT("Not Supported Type : %d"), sortingType);
				}
				break;
			}
		}

		return false;
	});

	return rosStarDatas;
}
