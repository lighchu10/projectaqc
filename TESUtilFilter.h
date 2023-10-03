// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Common/GameData/TESGDGoodsItem.h"
#include "Common/GameData/TESGDCharacter.h"
#include "Common/GameData/TESGDMail.h"

/** 
 * [ Note ] 정렬과 필터에 관련된 함수들을 모아놓은 유틸리티입니다.
 * 
 * - (주의) namespace에서 static 함수를 오버로딩하는 경우, 함수를 정의하는 순서에 영향을 받으므로, 반드시 함수 전방선언이 필요합니다.
 **/
namespace TESUtilFilter
{
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Common Functions ] 
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
	//. [ Note ] 필터 데이터의 초기값을 돌려줍니다.
	FTESFilterData ResetFilterData(ETypeSortFilterContents contentsType);

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Item Filter Functions ] 
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────

	//. [ Note ] 해당 아이템이 해당 필터 리스트에 해당하는지를 알려줍니다.
	bool CheckFilteringForItemData(class UTESGDGoodsItem* itemData, int32 filterElementId);

	//. [ Note ] 아이템에 대한 필터링 데이터를 넣어주면 필터링된 리스트로 돌려줍니다.
	TArray<class UTESGDGoodsItem*> MakeFilteredItemDataList(TArray<class UTESGDGoodsItem*> itemDataList
														  , TMap<int32, FTESFilterFilteringGroupData> filteringData
														  , bool isFiltering);

	//. [ Note ] 장착 아이템에 대한 정렬 기준을 넣어주면 정렬된 리스트로 돌려줍니다.
	TArray<class UTESGDGoodsItem*> MakeSortedItemDataListByEquip(TArray<class UTESGDGoodsItem*> itemDataList
															   , TArray<ETypeSortRuleEquip> itemSotingRules
															   , ETESFilterSortingOrder sortingOrder);

	//. [ Note ] 기타 아이템에 대한 정렬 기준을 넣어주면 정렬된 리스트로 돌려줍니다.
	TArray<class UTESGDGoodsItem*> MakeSortedItemDataListByMisc(TArray<class UTESGDGoodsItem*> itemDataList
															  , TArray<ETypeSortRuleMisc> itemSotingRules
															  , ETESFilterSortingOrder sortingOrder);

	//. [ Note ] 아이템 데이터와 정렬 기준을 넣어주면 정렬된 아이템 데이터 리스트로 돌려줍니다.
	TArray<class UTESGDGoodsItem*> MakeSortedItemDataList(TArray<class UTESGDGoodsItem*> itemDataList
														, int32 sortingElementId
														, ETESFilterSortingOrder sortingOrder);

	//. [ Note ] 장착 아이템 데이터와 해당 캐릭터의 스탯을 비교하여 장비 착용 제한에 안걸리는 아이템 데이터 리스트를 돌려줍니다.
	TArray<class UTESGDGoodsItem*> MakeFilteredEquipableItemDataList(TArray<class UTESGDGoodsItem*> itemDataList
														, class UTESGDCharacter* characterData
														, bool isFiltering);

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Character Filter Functions ] 
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
	//. [ Note ] 해당 캐릭터가 해당 필터 리스트에 해당하는지를 알려줍니다.
	bool CheckFilteringForCharacterData(class UTESGDCharacter* characterData, int32 filterElementId);

	//. [ Note ] 캐릭터에 대한 필터링 데이터를 넣어주면 필터링된 리스트로 돌려줍니다.
	TArray<class UTESGDCharacter*> MakeFilteredCharacterDataList(TArray<class UTESGDCharacter*> characterDataList
															   , TMap<int32, FTESFilterFilteringGroupData> filteringData
															   , bool isFiltering);

	//. [ Note ] 캐릭터 아이템에 대한 정렬 기준을 넣어주면 정렬된 리스트로 돌려줍니다.
	TArray<class UTESGDCharacter*> MakeSortedCharacterDataList(TArray<class UTESGDCharacter*> characterDataList
															 , TArray<ETypeSortRuleCharacter> sotingRules
															 , ETESFilterSortingOrder sortingOrder);

	//. [ Note ]  캐릭터 데이터와 정렬 기준을 넣어주면 정렬된 캐릭터 데이터 리스트로 돌려줍니다.
	TArray<class UTESGDCharacter*> MakeSortedCharacterDataList(TArray<class UTESGDCharacter*> characterDataList
															 , int32 sortingElementId
															 , ETESFilterSortingOrder sortingOrder);

	//. [ Note ] 제작 및 생산 시설의 정렬을 위한 함수입니다.
	bool CheckFacilitySortingOrderByCharacter(ETypeStatBase baseStatType, const class UTESGDCharacter& characterDataA, const class UTESGDCharacter& characterDataB, ETESFilterSortingOrder sortingOrder);

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ MailBox Filter Functions ] 
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
	//. [ Note ] 장착 아이템에 대한 정렬 기준을 넣어주면 정렬된 리스트로 돌려줍니다.
	TArray<class UTESGDMail*> MakeSortedMailDataListByMailBox(TArray<class UTESGDMail*> mailDatas
															, TArray<ETypeSortRulePostBox> itemSotingRules
															, ETESFilterSortingOrder sortingOrder);

	//. [ Note ] 아이템 데이터와 정렬 기준을 넣어주면 정렬된 아이템 데이터 리스트로 돌려줍니다.
	TArray<class UTESGDMail*> MakeSortedMailDataList(TArray<class UTESGDMail*> mailDatas
												   , int32 sortingElementId
												   , ETESFilterSortingOrder sortingOrder);

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ ROS Star Filter Functions ] 
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
#pragma region >> ROS Star Filter & Sorter <<
	//. [ Note ] 해당 아이템이 해당 필터 리스트에 해당하는지를 알려줍니다.
	bool CheckFilteringForROSStarData(int32 rosStarId, int32 filterElementId);

	//. [ Note ] 별의 고리에서 별들에 대한 필터링 데이터를 넣어주면 필터링된 리스트로 돌려줍니다.
	TArray<int32> MakeFilteredROSStarDatas(TArray<int32> rosStarDatas
										 , TMap<int32, FTESFilterFilteringGroupData> filteringData
										 , bool isFiltering);

	//. [ Note ] 별의 고리에서 별들에 대한 데이터와 정렬 기준을 넣어주면 정렬된 데이터 리스트로 돌려줍니다.
	TArray<int32> MakeSortedROSStarDatas(TArray<int32> rosStarDatas
									   , int32 sortingElementId
									   , ETESFilterSortingOrder sortingOrder);


	//. [ Note ] 별의 고리에서 별들에 대한 정렬 기준을 넣어주면 정렬된 리스트로 돌려줍니다.
	TArray<int32> MakeSortedROSStarDatasByROSStar(TArray<int32> rosStarDatas
										        , TArray<ETypeSortRuleStar> sotingRules
										        , ETESFilterSortingOrder sortingOrder);

#pragma endregion
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
}
