// Fill out your copyright notice in the Description page of Project Settings.
#include "Contents/PartySetting/Manager/TESPartySettingManager.h"
#include "Contents/PartySetting/Character/TESCharacterPartySetting.h"
#include "Contents/PartySetting/UI/TESUWPartySettingMain.h"
#include "Contents/PartySetting/UI/TESUWPartySettingDetail.h"
#include "Contents/PartySetting/UI/TESUWPartySettingSwapSlotInfo.h"
#include "Contents/PartySetting/UI/TESUWPartySettingCharSlotInfo.h"
#include "Contents/MirageGarden/UI/TESUWMirageGardenPartySetting.h"
#include "Contents/MirageGarden/Manager/TESMirageGardenManager.h"
#include "Contents/Tutorial/Manager/TESNewbieForcedTutorialManager.h"
#include "Contents/TeamTag/UI/TESUWTeamTagPartySettingMain.h"
#include "Contents/Common/Manager/TESUIManager.h"
#include "Contents/Common/Manager/TESPopupManager.h"
#include "Contents/Common/Manager/TESUserDataManager.h"
#include "Contents/Common/Manager/TESSaveGameManager.h"
#include "Contents/Sound/Manager/TESSoundManager.h"
#include "Tutorial/Manager/TESTutorialManager.h"
#include "Lobby/PlayerController/TESPlayerControllerLobby.h"
#include "Camera/CameraActor.h"
#include "Components/WidgetComponent.h"
#include "TESGameInstance.h"
#include "TESUtils.h"

void UTESPartySettingManager::Start()
{
	Super::Start();

    SetEnableTick(true);
}

void UTESPartySettingManager::Reset()
{
	Super::Reset();

	ClearScene();

    _recentUsedPartyUniqueId = GameDefine::PARTY_START_UNIQUE;

	_partySettingByContents = ETESPartySettingContents::None;

	_partyUniqueListByCharacterUnique.Empty();
	_charactersUniqueByPartyUnique.Empty();
	_partyUniqueListByCharacterId.Empty();
	_charactersIdByPartyUnique.Empty();
}

void UTESPartySettingManager::Shutdown()
{
	Super::Shutdown();

	SetEnableTick(false);

	Reset();
}

void UTESPartySettingManager::Tick(const float& deltaTime)
{
	Super::Tick(deltaTime);

	TickCameraMoving(deltaTime);
}

void UTESPartySettingManager::InitScene(/*ETESPartySettingEntryRoute entryRoute,*/ TMap<FString, int32> sceneDatas /*= {}*/)
{
	//. _entryRoute = entryRoute;

	_sceneDatas = sceneDatas;

	_curWidgetID = ETEPartySettingWidgetId::Main;

	InitSceneCameraActor();

	InitSceneCameraOffsetActor();

	SetupCharacterSlotList();
}

void UTESPartySettingManager::EnterScene(int32 partyUnique, int32 stageId, int32 numofLockedSlot, int32 difficultyLevel)
{
	//. 해당 파티 세팅에서 사용하는 카메라를 세팅합니다.
	SetSceneCameraActor(_soa_sceneCamera_default);

	//. 해당 파티 세팅에서 사용할 모델링을 세팅합니다.
	SetPartyPresetCharacterModelingByPartyUnique(partyUnique);

	//. 해당 파티 세팅에서 사용할 위젯들을 세팅합니다.
    SetupPartySettingWidgetsByPartyUnique(partyUnique, stageId, difficultyLevel);

	//. 해당 파티 세팅에서는 슬롯을 몇개나 잠궈야하는지를 세팅합니다.
	SetupLockedSlot(numofLockedSlot);

	//. 기본 세팅 이후에 세팅해야하는 것이 더 있는 경우, 해당 함수에서 그에 대한 처리를 진행합니다.
	SetAfterSettings(_sceneDatas);

	//. SetTutorial();
}

void UTESPartySettingManager::ClearScene()
{
    if (true == ::IsValid(_draggedCharacter))
    {
        _draggedCharacter->Remove();
        _draggedCharacter = nullptr;
    }

    ClearMainWidget();

	ClearTeamTagMainWidget();

	ClearMirageGardenWidget();

    ClearDetailWidget();

    ClearCharSlotWidgets();

	ClearCharacterActors();

    _curWidgetID = ETEPartySettingWidgetId::None;

    _characterLocationSlotActorMap.Empty();

    _lockedCharacterList.Empty();

    _numofLockedSlot = 0;

    _isSwapSlotClicked = false;

    _draggedCharacter = nullptr;

	_selectedSlotIndex = 0;
}

void UTESPartySettingManager::InitSceneCameraActor()
{
	_sc_sceneCamera = TESUtils::FindActorByName<ACameraActor>(GetWorld(), TESConstantValue::SceneCameraName_PartySetting, true);
    TESCHECK(nullptr != _sc_sceneCamera);
    //._partySettingCameraDefaultLocation = _sc_sceneCamera->GetActorLocation();
    //. _partySettingCameraDefaultFov = _sc_sceneCamera->GetCameraComponent()->FieldOfView;
}

void UTESPartySettingManager::InitSceneCameraOffsetActor()
{
	_soa_sceneCamera_default = TESUtils::FindActorByName<AActor>(GetWorld(), TEXT("SOA_PartySettingCamera_Default"), true);
    TESCHECK(nullptr != _soa_sceneCamera_default);
    _partySettingCameraDefaultLocation = _soa_sceneCamera_default->GetActorLocation();
	_partySettingCameraDefaultRotation = _soa_sceneCamera_default->GetActorRotation();
}

void UTESPartySettingManager::ClearMainWidget()
{
	SharedUIManager->DestroyGenericWidget(_mainPartySettingWidget);

    _mainPartySettingWidget = nullptr;
}

void UTESPartySettingManager::ClearTeamTagMainWidget()
{
	SharedUIManager->DestroyGenericWidget(_teamTagPartySettingWidget);

	_teamTagPartySettingWidget = nullptr;
}

void UTESPartySettingManager::ClearMirageGardenWidget()
{
	SharedUIManager->DestroyGenericWidget(_mirageGardenPartySettingWidget);

	_mirageGardenPartySettingWidget = nullptr;
}

void UTESPartySettingManager::ClearDetailWidget()
{
    SharedUIManager->DestroyGenericWidget(_mainPartySettingDetailWidget);

    _mainPartySettingDetailWidget = nullptr;
}

void UTESPartySettingManager::ClearCharSlotWidgets()
{
    for (UTESUWPartySettingCharSlotInfo* child : _charSlotInfoWidgetList)
    {
		if (true == ::IsValid(child))
		{
			child->ClearWidget();
		}
    }

    _charSlotInfoWidgetList.Empty();
}

void UTESPartySettingManager::ClearSwapSlotWidgets()
{
    for (UTESUWPartySettingSwapSlotInfo* child : _characterSwapSlotList)
    {
		if (true == ::IsValid(child))
		{
			child->ClearWidget();
		}
    }

    _characterSwapSlotList.Empty();
}

void UTESPartySettingManager::ClearCharacterActors()
{
    for (TPair<int32, ATESCharacterPartySetting*> child : _characterMap)
    {
		if (true == ::IsValid(child.Value))
		{
			child.Value->Remove();
		}
    }

	_characterMap.Empty();
}

void UTESPartySettingManager::SetSceneCameraActor(AActor* offsetActor)
{
	if (true == ::IsValid(_sc_sceneCamera))
	{
		_sc_sceneCamera->SetActorTransform(offsetActor->GetTransform());
	}
}

void UTESPartySettingManager::SetPartyPresetCharacterModelingFromLobby()
{
	//. 파티편성으로 들어오는 경우에는 무조건 1번탭에 1번 파티로 들어온다.
	_recentUsedPartyUniqueId = GameDefine::PARTY_START_UNIQUE;		//. 1번탭 1번 파티 유니크는 100.

	TOptional<FTESPartyInfo> partyUnique = SharedUserDataManager->FindPartyList(_recentUsedPartyUniqueId);

	SetCharacterModeling(partyUnique);
}

void UTESPartySettingManager::SetPartyPresetCharacterModelingByPartyUnique(uint16 partyUnique)
{
	_recentUsedPartyUniqueId = partyUnique;

	TOptional<FTESPartyInfo> partyUniqueInfo = SharedUserDataManager->FindPartyList(_recentUsedPartyUniqueId);

	SetCharacterModeling(partyUniqueInfo);
}

void UTESPartySettingManager::SetPartyCharacterModelingBySallyForced(uint16 partyUnique)
{
	//. 강제 출격 파티는 1번 파티 유니크로 들어온다.
	TOptional<FTESPartyInfo> sallyForcedPartyUnique = SharedUserDataManager->FindPartyList(partyUnique);

	SetCharacterModeling(sallyForcedPartyUnique);
}

void UTESPartySettingManager::SetupCharacterSlotList()
{
	SetupCharacterInfoSlotList();

	SetupCharacterLocationSlotList();

	SetupCharacterSwapSlotList();
}

void UTESPartySettingManager::SetupCharacterInfoSlotList()
{
	_charSlotInfoWidgetList.Empty();

	for (int32 slotIndex = 1; slotIndex <= TESConstantValue::MaxPartyCount; ++slotIndex)
	{
		FString charSlotName = FString::Printf(TEXT("CharInfoSlot_%d"), slotIndex);
		AActor* charSlotActor = TESUtils::FindActorByName<AActor>(GetWorld(), charSlotName);
		TESCHECK_CONTINUE(nullptr != charSlotActor);

		TArray<UWidgetComponent*> widgetComponents;
		charSlotActor->GetComponents<UWidgetComponent>(widgetComponents);

		UTESUWPartySettingCharSlotInfo* charSlotInfoWidget = nullptr;
		for (int32 childrenIndex = 0; childrenIndex < widgetComponents.Num(); ++childrenIndex)
		{
			UWidgetComponent* charSlotInfoWidgetComponent = widgetComponents[childrenIndex];
			charSlotInfoWidget = Cast<UTESUWPartySettingCharSlotInfo>(charSlotInfoWidgetComponent->GetUserWidgetObject());
			TESCHECK_CONTINUE(nullptr != charSlotInfoWidget);

			charSlotInfoWidget->InitWidget();
			charSlotInfoWidget->OpenWidget();
			charSlotInfoWidget->SetWidget(slotIndex);
			_charSlotInfoWidgetList.Add(charSlotInfoWidget);
		}
	}
}

void UTESPartySettingManager::SetupCharacterLocationSlotList()
{
	_characterLocationSlotActorMap.Empty();

	for (int32 slotIndex = 1; slotIndex <= TESConstantValue::MaxPartyCount; ++slotIndex)
	{
		FString charSlotName = FString::Printf(TEXT("CharacterLocationSlot_%d"), slotIndex);
		AActor* charSlotActor = TESUtils::FindActorByName<AActor>(GetWorld(), charSlotName);
		TESCHECK_CONTINUE(nullptr != charSlotActor);
		_characterLocationSlotActorMap.Add(slotIndex, charSlotActor);
	}
}

void UTESPartySettingManager::SetupCharacterSwapSlotList()
{
	_characterSwapSlotList.Empty();

	for (int32 slotIndex = 1; slotIndex <= TESConstantValue::MaxPartyCount; ++slotIndex)
	{
		FString charSlotName = FString::Printf(TEXT("SwapSlotInfo_%d"), slotIndex);
		AActor* charSlotActor = TESUtils::FindActorByName<AActor>(GetWorld(), charSlotName);
		TESCHECK_CONTINUE(nullptr != charSlotActor);

		TArray<UWidgetComponent*> widgetComponents;
		charSlotActor->GetComponents<UWidgetComponent>(widgetComponents);

		UTESUWPartySettingSwapSlotInfo* characterSwapSlotWidget = nullptr;
		for (int32 childrenIndex = 0; childrenIndex < widgetComponents.Num(); ++childrenIndex)
		{
			UWidgetComponent* charSlotInfoWidgetComponent = widgetComponents[childrenIndex];
			characterSwapSlotWidget = Cast<UTESUWPartySettingSwapSlotInfo>(charSlotInfoWidgetComponent->GetUserWidgetObject());
			TESCHECK_CONTINUE(nullptr != characterSwapSlotWidget);

			characterSwapSlotWidget->InitWidget();
			characterSwapSlotWidget->OpenWidget();
			characterSwapSlotWidget->SetWidget(slotIndex);
			_characterSwapSlotList.Add(characterSwapSlotWidget);
		}
	}
}

void UTESPartySettingManager::SetupPartySettingWidgetsFromLobby()
{
	//. 로비에서 들어올때는 기획상 1번탭에 1번 파티로 들어와야 한다.
	SetupPartySettingWidgetsByPartyUnique(GameDefine::PARTY_START_UNIQUE);
}

void UTESPartySettingManager::SetupPartySettingWidgetsByPartyUnique(uint16 partyUniqe, int32 stageId/* = 0*/, int32 difficultyLevel/* = 0;*/, ETESCommonRemainCountType type /*= ETESCommonRemainCountType::None*/)
{
	//. 강제 출격인지 아닌지에 대한 판단
	if (TESConstantValue::SallyForcedPartyUnique == partyUniqe)
	{
		_isCurrentPartySallyForced = true;
	}
	else
	{
		_isCurrentPartySallyForced = false;
	}

	_selectedStageId = stageId;

	TOptional<FTESPartyInfo> partyInfo = SharedUserDataManager->FindPartyList(partyUniqe);
	int32 partyTabId = partyUniqe / 100;
	TOptional<FTESPartyGroupInfo> partyGroupInfo = SharedUserDataManager->FindPartyGroupList(partyTabId);

	FTESPartyGroupInfo tempPartyTabInfo = {};
	FTESPartyInfo tempPartyInfo = {};
	if (false == partyGroupInfo.IsSet())
	{
		//. 아무런 정보가 없을때는 공백으로 넣어준다. 위젯에서 알아서 세팅함. 유니크만 넣어준다.
		tempPartyTabInfo._unique = partyTabId;
	}
	else
	{
		tempPartyTabInfo._unique = partyTabId;
		tempPartyTabInfo = partyGroupInfo.GetValue();
	}

	//. 파티 정보도 세팅해준다.
	if (true == partyInfo.IsSet())
	{
		tempPartyInfo._unique = partyUniqe;
		tempPartyInfo = partyInfo.GetValue();
	}
	else
	{
		tempPartyInfo._unique = partyUniqe;
	}

	const FStageRow& stageTable = SharedTDBManager->GetStageRow(_selectedStageId);

	switch (stageTable.T_Contents)
	{
	case ETypeContents::MainStory:
		_partySettingByContents = ETESPartySettingContents::FromMainStory;
		break;
	case ETypeContents::UniqueAbyss:
		_partySettingByContents = ETESPartySettingContents::FromUniqueAbyss;
		break;
	case ETypeContents::TeamTagBattle:
		_partySettingByContents = ETESPartySettingContents::FromTeamTagBattle;
		break;
	case ETypeContents::MirageGarden:
		_partySettingByContents = ETESPartySettingContents::FromMirageGarden;
		break;
	case ETypeContents::IllusionTemple:
		_partySettingByContents = ETESPartySettingContents::FromIllusionTemple;
		break;
	case ETypeContents::HeavenTower:
		_partySettingByContents = ETESPartySettingContents::FromHeavenTower;
		break;
	default:
		_partySettingByContents = ETESPartySettingContents::FromLobby;
		break;
	}

	if (ETESPartySettingContents::FromTeamTagBattle == _partySettingByContents)
	{
		if (true == ::IsValid(_teamTagPartySettingWidget))
		{
			_teamTagPartySettingWidget->OpenWidget();
			_teamTagPartySettingWidget->SetWidget(tempPartyTabInfo, tempPartyInfo, _isCurrentPartySallyForced, stageId);
		}
		else
		{
			_teamTagPartySettingWidget = SharedUIManager->CreateGenericWidget<UTESUWTeamTagPartySettingMain>(TEXT("WBP_TeamTagPartySettingMain"), TEXT("TeamTag/WBP"));
			TESCHECK(nullptr != _teamTagPartySettingWidget);

			_teamTagPartySettingWidget->InitWidget();
			_teamTagPartySettingWidget->OpenWidget();
			_teamTagPartySettingWidget->SetWidget(tempPartyTabInfo, tempPartyInfo, _isCurrentPartySallyForced, stageId);
			_teamTagPartySettingWidget->AddToViewport();
		}
	}
	else if (ETESPartySettingContents::FromMirageGarden == _partySettingByContents)
	{
		if (true == ::IsValid(_mirageGardenPartySettingWidget))
		{
			_mirageGardenPartySettingWidget->OpenWidget();
			_mirageGardenPartySettingWidget->SetWidget(tempPartyTabInfo, tempPartyInfo, _isCurrentPartySallyForced, stageId, difficultyLevel);
		}
		else
		{
			_mirageGardenPartySettingWidget = SharedUIManager->CreateGenericWidget<UTESUWMirageGardenPartySetting>(TEXT("WBP_MirageGardenPartySettingMain"), TEXT("MirageGarden/WBP"));
			TESCHECK(nullptr != _mirageGardenPartySettingWidget);

			_mirageGardenPartySettingWidget->InitWidget();
			_mirageGardenPartySettingWidget->OpenWidget();
			_mirageGardenPartySettingWidget->SetWidget(tempPartyTabInfo, tempPartyInfo, _isCurrentPartySallyForced, stageId, difficultyLevel);
			_mirageGardenPartySettingWidget->AddToViewport();
		}

		SharedMirageGardenManger->SetDifficultyLevel(difficultyLevel);
	}
	else if (ETESPartySettingContents::FromIllusionTemple == _partySettingByContents)
	{
		//. 메인 위젯 세팅
		if (true == ::IsValid(_mainPartySettingWidget))
		{
			_mainPartySettingWidget->OpenWidget();
			_mainPartySettingWidget->SetWidget(tempPartyTabInfo, tempPartyInfo, _isCurrentPartySallyForced, stageId);
		}
		else
		{
			_mainPartySettingWidget = SharedUIManager->CreateGenericWidget<UTESUWPartySettingMain>(TEXT("WBP_PartySettingMain"), TEXT("Common/WBP/PartySetting"));
			TESCHECK(nullptr != _mainPartySettingWidget);

			_mainPartySettingWidget->InitWidget();
			_mainPartySettingWidget->OpenWidget();
			_mainPartySettingWidget->SetWidget(tempPartyTabInfo, tempPartyInfo, _isCurrentPartySallyForced, stageId, ETESCommonRemainCountType::IllusionTemple);
			_mainPartySettingWidget->AddToViewport();
		}
	}
	else if (ETESPartySettingContents::FromHeavenTower == _partySettingByContents)
	{
		//. 메인 위젯 세팅
		if (true == ::IsValid(_mainPartySettingWidget))
		{
			_mainPartySettingWidget->OpenWidget();
			_mainPartySettingWidget->SetWidget(tempPartyTabInfo, tempPartyInfo, _isCurrentPartySallyForced, stageId);
		}
		else
		{
			_mainPartySettingWidget = SharedUIManager->CreateGenericWidget<UTESUWPartySettingMain>(TEXT("WBP_PartySettingMain"), TEXT("Common/WBP/PartySetting"));
			TESCHECK(nullptr != _mainPartySettingWidget);

			_mainPartySettingWidget->InitWidget();
			_mainPartySettingWidget->OpenWidget();
			_mainPartySettingWidget->SetWidget(tempPartyTabInfo, tempPartyInfo, _isCurrentPartySallyForced, stageId);
			_mainPartySettingWidget->AddToViewport();
		}
	}
	else
	{ 
		//. 메인 위젯 세팅
		if(true == ::IsValid(_mainPartySettingWidget))
		{
			_mainPartySettingWidget->OpenWidget();
			_mainPartySettingWidget->SetWidget(tempPartyTabInfo, tempPartyInfo, _isCurrentPartySallyForced, stageId);
		}
		else
		{
			_mainPartySettingWidget = SharedUIManager->CreateGenericWidget<UTESUWPartySettingMain>(TEXT("WBP_PartySettingMain"), TEXT("Common/WBP/PartySetting"));
			TESCHECK(nullptr != _mainPartySettingWidget);

			_mainPartySettingWidget->InitWidget();
			_mainPartySettingWidget->OpenWidget();
			_mainPartySettingWidget->SetWidget(tempPartyTabInfo, tempPartyInfo, _isCurrentPartySallyForced, stageId);
			_mainPartySettingWidget->AddToViewport();
		}
	}

	//. 상세보기 위젯 세팅
	if (nullptr == _mainPartySettingDetailWidget)
	{
		_mainPartySettingDetailWidget = SharedUIManager->CreateGenericWidget<UTESUWPartySettingDetail>(TEXT("WBP_PartySettingDetail"), TEXT("Common/WBP/PartySetting"));
		TESCHECK(nullptr != _mainPartySettingDetailWidget);

		_mainPartySettingDetailWidget->InitWidget();
		_mainPartySettingDetailWidget->CloseWidget();
		_mainPartySettingDetailWidget->AddToViewport();
	}
	else
	{
		_mainPartySettingDetailWidget->CloseWidget();
	}
}

void UTESPartySettingManager::SetupPartySettingWidgetsBySallyForcedParty(int32 stageId/* = 0*/)
{
	//. 강제 출격 파티는 1번 파티로 고정한다.
	//. 1번 파티는 스테이지 정보에 들어올때 생성 및 수정한다.
	SetupPartySettingWidgetsByPartyUnique(TESConstantValue::SallyForcedPartyUnique, stageId);
}

void UTESPartySettingManager::SetupLockedSlot(int32 numofLockedSlot)
{
	_numofLockedSlot = numofLockedSlot;
}

void UTESPartySettingManager::SetAfterSettings(TMap<FString, int32> sceneDatas)
{
    //. 만약, 다른 곳에서 되돌아온 경우라면...
    if (true == sceneDatas.Contains(TESConstantValue::PartySettingParameterKey_SelectedSlotIndex))
    {
		int32 selectedSlotIndex = sceneDatas[TESConstantValue::PartySettingParameterKey_SelectedSlotIndex];

		//. 이미 포커스 되어 있는 상태로...
		//. 씬에서는 캐릭터를 줌인 혹은 줌 아웃을 해준다.
		OnEvent_SwitchPartySettings(selectedSlotIndex);
    }
}

//void UTESPartySettingManager::SetTutorial()
//{
//	if (ETESPartySettingEntryRoute::InMainStory == _entryRoute || ETESPartySettingEntryRoute::InUniqueAbyss == _entryRoute)
//	{
//		SharedTutorialManager->ShowTutorialPopupAfterCheck(ETESTutorialContentsType::PartySetting);
//	}
//}

void UTESPartySettingManager::SetCharacterModeling(TOptional<FTESPartyInfo> partyUniqueInfo)
{
	if (true == partyUniqueInfo.IsSet())
	{
		for (int32 index = 0; index < TESConstantValue::MaxPartyCount; ++index)
		{
			SetCharacterSlot(index + 1, partyUniqueInfo.GetValue()._characterUnique[index], false);
		}
	}
	else
	{
		for (int32 index = 0; index < TESConstantValue::MaxPartyCount; ++index)
		{
			SetCharacterSlot(index + 1, 0); 
		}
	}
}

void UTESPartySettingManager::SetCharacterSlot(int32 slotIndex, int64 characterUnique, bool useChangeAction /*= true*/)
{
	//. 캐릭터 슬롯 모델링 및 정보 교체
	//. 대여캐릭터가 있을때.
	if (TESConstantValue::NoneValue == characterUnique)
	{
		//. CharacterMap은 현재 나와 있는 모델링이라서 다른 파티로 교체 할때 문제가 생긴다.
		//. 대여 캐릭이 있는 경우에는 캐릭터 아이디로 한다.
		TArray<int32> characterIdList =	GetCharactersIdByPartyUnique(_recentUsedPartyUniqueId);
		if(true == characterIdList.IsValidIndex(slotIndex - 1))
		{
			//. 대여 캐릭터는 현재 신기루 정원에서만 쓰고 있어서, 테이블을 MirageGarden 테이블에서 로드해온다.
			//. 추후에 다른 컨텐츠에서도 대여 캐릭을 쓸 경우에는 해당 테이블에서 불러와야 한다.
			FTESCharacterInfo characterInfo = {};
			characterInfo._unique = -1;
			characterInfo._index = characterIdList[slotIndex - 1];
			characterInfo._level = SharedMirageGardenManger->GetCurrentSeasonRentalCharacterLevel();
			characterInfo._lock = false;
			characterInfo._seal = false;
			characterInfo._awaken = 0;

			UTESGDCharacter* gameDataCharacter = NewObject<UTESGDCharacter>(this);
			if (true == ::IsValid(gameDataCharacter))
			{
				gameDataCharacter->SetCharacterData(characterInfo);
				SetCharacterSlotByData(slotIndex, gameDataCharacter, useChangeAction);
			}
			else
			{
				SetCharacterSlotBySlotIndex(slotIndex, 0, useChangeAction);
			}
		}
		else
		{
			SetCharacterSlotBySlotIndex(slotIndex, 0, useChangeAction);
		}
	}
	else
	{
		SetCharacterSlotBySlotIndex(slotIndex, characterUnique, useChangeAction);
	}

}

void UTESPartySettingManager::SetCharacterSlotByData(int32 slotIndex, UTESGDCharacter* gdCharacter, bool useChangeAction /*= true*/)
{
	SetCharacterSlotByCharacterData(slotIndex, gdCharacter, useChangeAction);
}

void UTESPartySettingManager::SetCameraToCharacterHeight(int32 slotIndex)
{
    TMap<int32, AActor*> characterSlotLocationMap = GetCharacterSlotActorMap();

	if (true == characterSlotLocationMap.Contains(slotIndex))
	{
		//. 캐릭터가 바뀌었을때 높이 맞춰주는 함수
		_partySettingCameraTargetLocation = characterSlotLocationMap[slotIndex]->GetActorLocation();
		_partySettingCameraTargetRotation = _partySettingCameraDefaultRotation;
				
		_partySettingCameraTargetLocation.X = -210.0f;
		_partySettingCameraTargetLocation.Z += 7.0f;
		
		switch (slotIndex)
		{
		case 1:
			{
				_partySettingCameraTargetLocation.Y = -140.0f;
				_partySettingCameraTargetRotation.Yaw = -20.0f;
			}
			break;
		case 2:
			{
				_partySettingCameraTargetLocation.Y = -40.0f;
				_partySettingCameraTargetRotation.Yaw = -15.0f;
			}
			break;
		case 3:
			{
				_partySettingCameraTargetLocation.Y = 40.0f;
				_partySettingCameraTargetRotation.Yaw = -5.0f;
			}
			break;
		case 4:
			{
				_partySettingCameraTargetLocation.Y = 140.0f;
				_partySettingCameraTargetRotation.Yaw = 0.0f;
			}
			break;
		}

		TMap<int32, class ATESCharacterPartySetting*> characterMap = GetCharacterMap();

		if (true == characterMap.Contains(slotIndex))
		{
			if (nullptr != characterMap[slotIndex])
			{
				float eyeHeight = characterMap[slotIndex]->GetLobbyEyeHeight();

				//TESLOG(Error, TEXT("eye Height : %f"), eyeHeight);
				_partySettingCameraTargetLocation.Z += eyeHeight;
				_isFocusingToCharacter = true;
				_isJoomingIn = true;
			}
			else
			{
				//. 빈 슬롯일 경우에는 임의로 높이를 맞춰준다.
				_partySettingCameraTargetLocation.Z += 160.0f;
				_isFocusingToCharacter = true;
				_isJoomingIn = true;
			}
		}
		else
		{
			TESLOG_S(Error);
		}
	}
	else
	{
		TESLOG(Error, TEXT("Not Found Slot Index : %d"), slotIndex);
	}
}

void UTESPartySettingManager::SetDraggedCharacter(int32 characterId, FVector characterLocation)
{
	//. 드래그용 캐릭터를 만들어준다. => 마우스 위치에 생성
	//. 마우스 커서 위치에 만들어져야 한다.
	FActorSpawnParameters spawnInfo;
	spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	FRotator rotation;
	rotation.Yaw += 180.f;
	ATESCharacterPartySetting* dragChacter = GetWorld()->SpawnActor<ATESCharacterPartySetting>(characterLocation, rotation, spawnInfo);
	TESCHECK(nullptr != dragChacter);
	_draggedCharacter = dragChacter;
	_draggedCharacter->Init(characterId);
}

void UTESPartySettingManager::SetDraggedCharacterReleased()
{
	//. 몇번째 슬롯인지에 따라 로직 처리 한다.(자리교체 등)
	//. 드래그된 캐릭터는 지워준다.
	if (true == ::IsValid(_draggedCharacter))
	{
		_draggedCharacter->Remove();
		_draggedCharacter = nullptr;
	}

	for (TPair<int32, ATESCharacterPartySetting*> child : _characterMap)
	{
		if (nullptr != child.Value)
		{
			child.Value->SetCharacterReleased(true);
		}
	}
}

void UTESPartySettingManager::SetDraggedCharacterLocation(FVector characterLocation)
{
	if (true == ::IsValid(_draggedCharacter))
	{
		_draggedCharacter->SetCharacterLocation(characterLocation);
	}
}

void UTESPartySettingManager::SetCharacterSlotBySlotIndex(int32 slotIndex, int64 characterUnique, bool useChangeAction /*= true*/)
{
	if (true == _characterMap.Contains(slotIndex))
	{
		if (nullptr != _characterMap[slotIndex])
		{
			if (_characterMap[slotIndex]->GetCharacterUnique() == characterUnique)
			{
				if (true == ::IsValid(_characterMap[slotIndex]->GetCharacterData()))
				{
					int32 characterLevel = _characterMap[slotIndex]->GetCharacterData()->GetCharacterCurrentLevel();
					_charSlotInfoWidgetList[slotIndex - 1]->SetCharacterSlotInfo(_characterMap[slotIndex]->GetCharacterId(), characterLevel, characterUnique);
				}
			}
		}
	}

	//. 슬롯을 비울려고 하는 경우, nullptr로 들어옵니다.
	if (true == _characterMap.Contains(slotIndex) && nullptr != _characterMap[slotIndex])
	{
		//. 이전에 있던 모델을 제거해줍니다.
		_characterMap[slotIndex]->Remove();
		_characterMap.Remove(slotIndex);
	}

	//. 변경할 데이터가 존재할 경우에만!!
	if (0 < characterUnique)
	{
		UWorld* worldContext = GetWorld();
		if (false == ::IsValid(worldContext))
		{
			TESLOG(Error, TEXT("Invalid WorldContext."));

			return;
		}

		FVector location = FVector::ZeroVector;

		FActorSpawnParameters spawnInfo;
		spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		ATESCharacterPartySetting* partySettingCharacter = nullptr;
		if (_characterLocationSlotActorMap.Contains(slotIndex))
		{
			FVector characterLocation = _characterLocationSlotActorMap[slotIndex]->GetActorLocation();
			FRotator characterRotation = _characterLocationSlotActorMap[slotIndex]->GetActorRotation();
			partySettingCharacter = GetWorld()->SpawnActor<ATESCharacterPartySetting>(characterLocation, characterRotation, spawnInfo);
		}

		TESCHECK(nullptr != partySettingCharacter);
		partySettingCharacter->Init(characterUnique);
		partySettingCharacter->PlayChangeVFX();

		TESCHECK(nullptr != partySettingCharacter);
		int32 characterId = partySettingCharacter->GetCharacterTableId();
		int32 characterLevel = partySettingCharacter->GetCurrentLevel();

		if (true == useChangeAction)
		{
			//. 파이팅 애니
			const FCharacterRow& characterTable = SharedTDBManager->GetCharacterRow(characterId);
			TESCHECK(false == characterTable.IsEmpty());
			partySettingCharacter->PlayMontage(characterTable.AnimLobbyPick);
		}

		if (_charSlotInfoWidgetList.Num() < slotIndex || 0 == slotIndex)
		{
			TESLOG(Error, TEXT("Index is out of array. %d"), slotIndex);
			return;
		}

		//. 캐릭터 정보 입력
		_charSlotInfoWidgetList[slotIndex - 1]->SetCharacterSlotInfo(characterId, characterLevel, characterUnique);

		//. 캐릭터 맵에 추가
		_characterMap.Add(slotIndex, partySettingCharacter);
	}
	else
	{
		if (_charSlotInfoWidgetList.Num() < slotIndex || 0 == slotIndex)
		{
			TESLOG(Error, TEXT("Index is out of array. %d"), slotIndex);
			return;
		}

		//. 캐릭터 정보 입력
		_charSlotInfoWidgetList[slotIndex - 1]->SetCharacterSlotInfo(0, 0, 0);

		//. 빈공간인 상태면 그대로 냅두고, 원래 있었던 캐릭이 위치하고 있었으면 없애준다.
		_characterMap.Add(slotIndex, nullptr);
	}

	if (0 != characterUnique)
	{
		int32 soundId = TESUtilTable::GetConstantValueByInt(EConstantIntName::UISE_PartySet);
		const FSoundRow& soundTable = SharedTDBManager->GetSoundRow(soundId);
		if (true == soundTable.IsEmpty())
			return;
		
		SharedSoundManager->Play2DSoundAtOnce(GetWorld(), soundTable.SoundPath);
	}
}

void UTESPartySettingManager::SetCharacterSlotByCharacterData(int32 slotIndex, UTESGDCharacter* gdCharacter, bool useChangeAction /*= true*/)
{
	if (true == _characterMap.Contains(slotIndex))
	{
		if (nullptr != _characterMap[slotIndex])
		{
			if (true == ::IsValid(gdCharacter))
			{
				if (_characterMap[slotIndex]->GetCharacterId() == gdCharacter->GetCharacterTableId())
				{
					
					int32 characterLevel = gdCharacter->GetCharacterCurrentLevel();
					int64 characterUnique = gdCharacter->GetCharacterUnique();
					_charSlotInfoWidgetList[slotIndex - 1]->SetCharacterSlotInfo(_characterMap[slotIndex]->GetCharacterId(), characterLevel, characterUnique);
				}
			}
			
		}
	}

	//. 슬롯을 비울려고 하는 경우, nullptr로 들어옵니다.
	if (true == _characterMap.Contains(slotIndex) && nullptr != _characterMap[slotIndex])
	{
		//. 이전에 있던 모델을 제거해줍니다.
		_characterMap[slotIndex]->Remove();
		_characterMap.Remove(slotIndex);
	}
	int32 characterId = 0;

	if (true == ::IsValid(gdCharacter))
	{
		characterId = gdCharacter->GetCharacterTableId();
	}

	//. 변경할 데이터가 존재할 경우에만!!
	if (0 != characterId)
	{
		UWorld* worldContext = GetWorld();
		if (false == ::IsValid(worldContext))
		{
			TESLOG(Error, TEXT("Invalid WorldContext."));

			return;
		}

		FVector location = FVector::ZeroVector;

		FActorSpawnParameters spawnInfo;
		spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		ATESCharacterPartySetting* partySettingCharacter = nullptr;
		if (_characterLocationSlotActorMap.Contains(slotIndex))
		{
			FVector characterLocation = _characterLocationSlotActorMap[slotIndex]->GetActorLocation();
			FRotator characterRotation = _characterLocationSlotActorMap[slotIndex]->GetActorRotation();
			partySettingCharacter = GetWorld()->SpawnActor<ATESCharacterPartySetting>(characterLocation, characterRotation, spawnInfo);
		}

		TESCHECK(nullptr != partySettingCharacter);
		partySettingCharacter->InitCharacterByCharacterData(gdCharacter);
		partySettingCharacter->PlayChangeVFX();

		TESCHECK(nullptr != partySettingCharacter);
		int32 characterLevel = partySettingCharacter->GetCurrentLevel();

		if (true == useChangeAction)
		{
			//. 파이팅 애니
			const FCharacterRow& characterTable = SharedTDBManager->GetCharacterRow(characterId);
			TESCHECK(false == characterTable.IsEmpty());
			partySettingCharacter->PlayMontage(characterTable.AnimLobbyPick);
		}

		if (_charSlotInfoWidgetList.Num() < slotIndex || 0 == slotIndex)
		{
			TESLOG(Error, TEXT("Index is out of array. %d"), slotIndex);
			return;
		}

		//. 캐릭터 정보 입력
		_charSlotInfoWidgetList[slotIndex - 1]->SetCharacterSlotInfo(gdCharacter);

		//. 캐릭터 맵에 추가
		_characterMap.Add(slotIndex, partySettingCharacter);
	}
	else
	{
		if (_charSlotInfoWidgetList.Num() < slotIndex || 0 == slotIndex)
		{
			TESLOG(Error, TEXT("Index is out of array. %d"), slotIndex);
			return;
		}

		//. 캐릭터 정보 입력
		_charSlotInfoWidgetList[slotIndex - 1]->SetCharacterSlotInfo(0, 0, 0);

		//. 빈공간인 상태면 그대로 냅두고, 원래 있었던 캐릭이 위치하고 있었으면 없애준다.
		_characterMap.Add(slotIndex, nullptr);
	}

	if (0 != characterId)
	{
		int32 soundId = TESUtilTable::GetConstantValueByInt(EConstantIntName::UISE_PartySet);
		const FSoundRow& soundTable = SharedTDBManager->GetSoundRow(soundId);
		if (true == soundTable.IsEmpty())
			return;

		SharedSoundManager->Play2DSoundAtOnce(GetWorld(), soundTable.SoundPath);
	}
}

void UTESPartySettingManager::SetCharacterMap(TMap<int32, class UTESGDCharacter*> finalJoinedPartyMap, bool isShowingModeling)
{
	
	//. 최종적으로 참여한 파티로 다시 세팅
	//. 있는 데이터들을 전부 지워준다.
	for (TPair<int32, ATESCharacterPartySetting*> child : _characterMap)
	{
		if (nullptr != child.Value)
		{
			child.Value->Remove();
		}
	}

	_characterMap.Empty();

	for (TPair<int32, UTESGDCharacter*> child : finalJoinedPartyMap)
		{
			if (true == ::IsValid(child.Value))
			{
				//. 캐릭터 유니크
				if (0 != child.Value->GetCharacterUnique())
				{
					UWorld* worldContext = GetWorld();
					if (false == ::IsValid(worldContext))
					{
						TESLOG(Error, TEXT("Invalid WorldContext."));

						return;
					}

					FVector characterLocation = _characterLocationSlotActorMap[child.Key]->GetActorLocation();
					FRotator characterRotation = _characterLocationSlotActorMap[child.Key]->GetActorRotation();

					FActorSpawnParameters spawnInfo;
					spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
					ATESCharacterPartySetting* partySettingCharacter = GetWorld()->SpawnActor<ATESCharacterPartySetting>(characterLocation, characterRotation, spawnInfo);
					TESCHECK(nullptr != partySettingCharacter);
					//. 캐릭터 유니크가 -1이면 테이블 아이디로 만들어준다.(렌탈)
					if (TESConstantValue::NoneValue == child.Value->GetCharacterUnique())
					{
						partySettingCharacter->InitCharacterByTableId(child.Value->GetCharacterTableId());
					}
					else
					{
						partySettingCharacter->Init(child.Value->GetCharacterUnique());
					}
					int32 characterId = partySettingCharacter->GetCharacterTableId();
					int32 characterLevel = partySettingCharacter->GetCurrentLevel();
					int64 characterUnique = partySettingCharacter->GetCharacterUnique();

					if (false == isShowingModeling)
					{
						partySettingCharacter->SetHidden(true);
						partySettingCharacter->SetHiddenDollMesh(true);
					}

					if (_charSlotInfoWidgetList.Num() < child.Key || 0 == child.Key)
					{
						TESLOG(Error, TEXT("Index is out of array. %d"), child.Key);
						return;
					}

					//. 캐릭터 정보 입력
					_charSlotInfoWidgetList[child.Key - 1]->SetCharacterSlotInfo(partySettingCharacter->GetCharacterData());

					//. 캐릭터 맵에 추가
					_characterMap.Add(child.Key, partySettingCharacter);
				}
				else
				{
					//. 캐릭터 정보 입력
					_charSlotInfoWidgetList[child.Key - 1]->SetCharacterSlotInfo(0, 0, 0);

					//. 빈공간인 상태면 그대로 냅두고, 원래 있었던 캐릭이 위치하고 있었으면 없애준다.
					_characterMap.Add(child.Key, nullptr);
				}
			}
			else
			{
				//. 캐릭터 정보 입력
				_charSlotInfoWidgetList[child.Key - 1]->SetCharacterSlotInfo(0, 0, 0);

				//. 빈공간인 상태면 그대로 냅두고, 원래 있었던 캐릭이 위치하고 있었으면 없애준다.
				_characterMap.Add(child.Key, nullptr);
			}
		}
}

void UTESPartySettingManager::SetCharacterSwapSlotClicked(int32 slotIndex, bool isClicked)
{
	//. 강제 출격이 있는 슬롯들은 제외시켜준다.

	//. 같은 슬롯을 클릭 했는지, 다른 슬롯을 클릭 했는지에 대해 구분
	//. 첫번째 클릭일때는 교체가 가능하게 만들어준다.
	if (_isSwapSlotClicked != isClicked)
	{
		_isSwapSlotClicked = isClicked;
		_clickedSlotIndex = slotIndex;

		//. 교체 가능한 상태이므로 모든 슬롯의 버튼을 열어준다.
		if (true == _isSwapSlotClicked)
		{
			for (int32 index = 0; index < _characterSwapSlotList.Num(); ++index)
			{
				//. 교체 문구는 다른 3개의 슬롯에만 보여준다.
				if (slotIndex == index + 1)
				{
					continue;
				}

				//. 강제 출격이 있는 슬롯들도 제외시킨다.
				if (index < _numofLockedSlot)
				{
					continue;
				}

				TESCHECK_CONTINUE(nullptr != _characterSwapSlotList[index]);
				_characterSwapSlotList[index]->ShowSelectedArrow(true);
				_characterSwapSlotList[index]->SetButtonVisible(true);
			}
		}
		//. 같은 슬롯을 클릭하면 교체 가능한 처음 상태로 바꿔준다.(빈슬롯은 클릭이 안되게 만들어준다.)
		else
		{
			SetCharacterSwapDefaultSetting();
		}
	}
	//. 첫번째 슬릇을 클릭하고 다른 슬롯을 클릭 했을 경우
	else
	{
		//. 2번 엘피나, 3번 에이카
		//. 3번 자리를 엘피나로
		//. 캐릭터의 위치를 서로 바꿔준다.
		//. 모델링도 자리를 바꿔준다.
		ATESCharacterPartySetting* tempCharacter = _characterMap[slotIndex];

		int64 beforeCharacterUnique = 0;
		int64 afterCharacterUnique = 0;

		if (nullptr != tempCharacter)
		{
			afterCharacterUnique = tempCharacter->GetCharacterUnique();
		}

		if (nullptr != _characterMap[_clickedSlotIndex])
		{
			beforeCharacterUnique = _characterMap[_clickedSlotIndex]->GetCharacterUnique();
		}

		SetCharacterSlotBySlotIndex(slotIndex, beforeCharacterUnique, false);
		SetCharacterSlotBySlotIndex(_clickedSlotIndex, afterCharacterUnique, false);

		//. 바꾸는데 성공 했으므로 처음 상태로 다시 돌아간다.
		SetSwapComplete();
	}
}

void UTESPartySettingManager::SetSwapComplete()
{
	SetCharacterSwapDefaultSetting();
	_clickedSlotIndex = 0;
	_isSwapSlotClicked = false;
}

void UTESPartySettingManager::SetSwapEnd()
{
	//. 파티 프리셋에서 뒤로가기 눌러서 취소 했을때.
	if (nullptr != _mainPartySettingWidget)
	{
		_mainPartySettingWidget->CharacterSwapCanceled();
	}

	_clickedSlotIndex = 0;
	_isSwapSlotClicked = false;
}

void UTESPartySettingManager::SetCharacterSwapDefaultSetting()
{
	//. 캐릭터의 슬롯에 있는 상태로 빈 슬롯은 버튼을 없애고, 화살표는 원상복구 시켜준다.
	for (TPair<int32, ATESCharacterPartySetting*> child : _characterMap)
	{
		//. 강제 슬롯이 있으면 스킵한다.
		if (child.Key <= _numofLockedSlot)
		{
			continue;
		}

		//. '교체'문구는 전부 없애준다.
		TESCHECK_CONTINUE(nullptr != _characterSwapSlotList[child.Key - 1]);
		_characterSwapSlotList[child.Key - 1]->SetDefaultSwapSetting();		//. 문구 및 화살표는 원상복구 한다.
		_characterSwapSlotList[child.Key - 1]->SetButtonVisible(true);

		if (nullptr == child.Value)
		{
			//. 빈슬롯은 빈슬롯으로 처리한다.
			_characterSwapSlotList[child.Key - 1]->ShowSelectedArrow(false);
			_characterSwapSlotList[child.Key - 1]->SetButtonVisible(false);
		}
	}
}

void UTESPartySettingManager::SetPartySallyForced(bool isForced)
{
	_isCurrentPartySallyForced = isForced;
}

void UTESPartySettingManager::RefreshParticipationCharacter()
{
	_partyUniqueListByCharacterUnique.Empty();
	_charactersUniqueByPartyUnique.Empty();

	TArray<FTESPartyInfo> partyList = SharedUserDataManager->FindPartyListAll();
	int32 maxCount = partyList.Num();
	for (int32 i = 0; i < maxCount; ++i)
	{
		TArray<int64> charactersUnique = {};
		for (int32 index = 0; index < sizeof(partyList[i]._characterUnique) / sizeof(int64); ++index)
		{
			charactersUnique.Add(partyList[i]._characterUnique[index]);
		}

		SetParticipationCharacter(partyList[i]._unique, charactersUnique);
	}
}

void UTESPartySettingManager::SetParticipationCharacter(uint16 partyId, TArray<int64> characterUnique)
{
	//. 한 파티에 캐릭터들이 누구누구 있는지 담아준다.
	_charactersUniqueByPartyUnique.Add(partyId, characterUnique);

	//. 특정 캐릭터가 어떤 파티에 있는지 넣어준다.
	for (int64 uniqueId : characterUnique)
	{
		//. 캐릭터 아이디를 순회 하면서 캐릭터가 이미 있으면 파티 유니크를 추가한다.
		if (true == _partyUniqueListByCharacterUnique.Contains(uniqueId))
		{
			if (false == _partyUniqueListByCharacterUnique[uniqueId].Contains(partyId))
			{
				_partyUniqueListByCharacterUnique[uniqueId].Add(partyId);
			}
		}
		//. 캐릭터가 없으면 등록해준다.
		else
		{
			TArray<uint16> partyUniqueList = {};
			partyUniqueList.Add(partyId);

			_partyUniqueListByCharacterUnique.Add(uniqueId, partyUniqueList);
		}
	}
}

void UTESPartySettingManager::RefreshCharacterSlotInfo()
{
	for (int32 index = 1; index <= TESConstantValue::MaxPartyCount; ++index)
	{
		if (true == _characterMap.Contains(index))
		{
			if (nullptr != _characterMap[index])
			{
				int32 characterId = _characterMap[index]->GetCharacterTableId();
				int32 characterLevel = _characterMap[index]->GetCurrentLevel();
				int64 characterUnique = _characterMap[index]->GetCharacterUnique();
				
				if (true == _charSlotInfoWidgetList.IsValidIndex(index - 1))
				{
					_charSlotInfoWidgetList[index - 1]->SetCharacterSlotInfo(_characterMap[index]->GetCharacterData());
				}
			}
		}
	}
}

void UTESPartySettingManager::SetParticipationCharacterByCharacterId(uint16 partyUnique, TArray<int32> characterIdList)
{
	//. 한 파티에 캐릭터들이 누구누구 있는지 담아준다.
	if (false == _charactersIdByPartyUnique.Contains(partyUnique))
	{
		_charactersIdByPartyUnique.Add(partyUnique, characterIdList);
	}
	else
	{
		_charactersIdByPartyUnique[partyUnique].Empty();

		for (int32 index = 0; index < characterIdList.Num(); ++index)
		{
			if (true == characterIdList.IsValidIndex(index))
			{
				_charactersIdByPartyUnique[partyUnique].Add(characterIdList[index]);
			}
		}
	}

	//. Key값이 캐릭터 아이디. Value값이 캐릭터가 속해 있는 파티 유니크 리스트
	//. 사실 체험 캐릭을 위한 자료인데, 굳이 어레이로 할 필요가 없을지도.(체험 캐릭은 한 파티에서 쓰면 다른 파티에서 채용 못함)
	for (int32 index = 0; index < characterIdList.Num(); ++index)
	{
		if (false == _partyUniqueListByCharacterId.Contains(characterIdList[index]))
		{
			if (true == characterIdList.IsValidIndex(index))
			{
				if (0 != characterIdList[index])
				{
					TArray<uint16> partyUniqueList = {};
					partyUniqueList.Add(partyUnique);
					_partyUniqueListByCharacterId.Add(characterIdList[index], partyUniqueList);
				}
			}
		}
		else
		{
			int32 maxCount = _partyUniqueListByCharacterId[characterIdList[index]].Num();
			for (int32 i = 0; i < _partyUniqueListByCharacterId[characterIdList[index]].Num(); ++i)
			{
				if (_partyUniqueListByCharacterId[characterIdList[index]][i] == partyUnique)
					continue;

				_partyUniqueListByCharacterId[characterIdList[index]].Add(partyUnique);
			}

			if (0 == maxCount)
			{
				_partyUniqueListByCharacterId[characterIdList[index]].Add(partyUnique);
			}
		}
	}
}

int32 UTESPartySettingManager::RemoveParticipationCharacterByCharacterId(uint16 partyUnique, int32 characterId)
{
	//. 캐릭터의 특정 아이디를 해당 파티 유니크 내에서 지워준다.
	if (true == _partyUniqueListByCharacterId.Contains(characterId))
	{
		int32 maxCount = _partyUniqueListByCharacterId[characterId].Num();
		for (int32 index = 0; index < maxCount; ++index)
		{
			if (partyUnique == _partyUniqueListByCharacterId[characterId][index])
			{
				//. 캐릭터가 속한 파티 유니크 리스트에서 해당 유니크를 지워준다.
				_partyUniqueListByCharacterId[characterId].RemoveAt(index);		//. 없애준다.
				break;
			}
		}
	}

	//. 
	//. 여기서 인덱스 리턴하고 지워야함.
	if (true == _charactersIdByPartyUnique.Contains(partyUnique))
	{
		int32 maxCount = _charactersIdByPartyUnique[partyUnique].Num();
		
		for (int32 index = 0; index < maxCount; ++index)
		{
			if (characterId == _charactersIdByPartyUnique[partyUnique][index])
			{
				_charactersIdByPartyUnique[partyUnique][index] = 0;		//. 빈칸으로 채우고
				return index;
			}
		}
	}

	return 0;
}

void UTESPartySettingManager::SetFocusToCharacter(int32 slotIndex)
{
    if (TESConstantValue::NoneValue == slotIndex)
    {
        //. -1이 들어올때는 카메라를 줌 아웃 시켜준다.(원상 복구)
        _partySettingCameraTargetLocation = _partySettingCameraDefaultLocation;
		_partySettingCameraTargetRotation = _partySettingCameraDefaultRotation;
        _isFocusingToCharacter = true;
        _isJoomingIn = false;
    }
    else
    {
        SetCameraToCharacterHeight(slotIndex);
    }
}

void UTESPartySettingManager::SetBattleUsedPartyUniqueIdList(TArray<uint16> battleUsedPartyUniuqeIdList)
{
	_battleUsedPartyUniqueIdList = battleUsedPartyUniuqeIdList;
}

bool UTESPartySettingManager::CheckIfThereIsSameCharacterIdInParty(uint16 partyUnique)
{
	bool isThereAnyRentalCharacters = false;
	bool result = false;
	TArray<int32> currentPartyCharacterIdList = {};
	TArray<int64> currentPartyCharacterUniqueList = {};

	TOptional<FTESPartyInfo> partyInfo = SharedUserDataManager->FindPartyList(partyUnique);
	if (false == partyInfo.IsSet())
		return false;

	//. 파티에 캐릭터 유니크가 -1이 1명이라도 있으면 체험캐릭으로 간주한다.
	for (int32 index = 0; index < sizeof(partyInfo.GetValue()._characterUnique) / sizeof(int64); ++index)
	{
		currentPartyCharacterUniqueList.Add(partyInfo.GetValue()._characterUnique[index]);

		if (TESConstantValue::NoneValue == partyInfo.GetValue()._characterUnique[index])
		{
			isThereAnyRentalCharacters = true;
		}
	}

	if (true == isThereAnyRentalCharacters)
	{
		currentPartyCharacterIdList = GetCharactersIdByPartyUnique(partyUnique);

		for (int32 i = 0; i < currentPartyCharacterIdList.Num(); i++)
		{
			for (int32 j = i + 1; j < currentPartyCharacterIdList.Num(); j++)
			{
				if (currentPartyCharacterIdList[i] == currentPartyCharacterIdList[j])
				{
					if(0 == currentPartyCharacterIdList[i] || 0 == currentPartyCharacterIdList[j])
						continue;

					result = true;
					break;
				}
			}
		}

		return result;
	}
	else
	{
		for (int32 index = 0; index < currentPartyCharacterUniqueList.Num(); ++index)
		{
			TOptional<UTESGDCharacter*> gdCharacter = SharedUserDataManager->FindHaveCharacter(currentPartyCharacterUniqueList[index], false);

			if(false == gdCharacter.IsSet())
				continue;

			currentPartyCharacterIdList.Add(gdCharacter.GetValue()->GetCharacterTableId());
		}

		for (int32 i = 0; i < currentPartyCharacterIdList.Num(); i++)
		{
			for (int32 j = i + 1; j < currentPartyCharacterIdList.Num(); j++)
			{
				if (currentPartyCharacterIdList[i] == currentPartyCharacterIdList[j])
				{
					result = true;
					break;
				}
			}
		}

		return result;
	}
}

bool UTESPartySettingManager::CheckIfThereIsSameCharacterIdInParty(GamePacket::FTESPartyCharacterInfo partyInfo, int32 fixedIndex, OUT int32& removeIndex)
{
	bool result = false;
	TArray<int32> currentPartyCharacterIdList = {};
	TArray<int64> currentPartyCharacterUniqueList = {};

	//. 파티에 캐릭터 유니크가 -1이 1명이라도 있으면 체험캐릭으로 간주한다.
	for (int32 index = 0; index < sizeof(partyInfo._characterUnique) / sizeof(int64); ++index)
	{
		currentPartyCharacterUniqueList.Add(partyInfo._characterUnique[index]);
	}

	for (int32 index = 0; index < currentPartyCharacterUniqueList.Num(); ++index)
	{
		TOptional<UTESGDCharacter*> gdCharacter = SharedUserDataManager->FindHaveCharacter(currentPartyCharacterUniqueList[index], false);

		if (false == gdCharacter.IsSet())
			continue;

		currentPartyCharacterIdList.Add(gdCharacter.GetValue()->GetCharacterTableId());
	}

	for (int32 i = 0; i < currentPartyCharacterIdList.Num(); i++)
	{
		for (int32 j = i + 1; j < currentPartyCharacterIdList.Num(); j++)
		{
			if (currentPartyCharacterIdList[i] == currentPartyCharacterIdList[j])
			{
				if(0 == currentPartyCharacterIdList[i] || 0 == currentPartyCharacterIdList[j])
					continue;

				if (fixedIndex != i)
				{
					removeIndex = i;
				}
				else if (fixedIndex == i)
				{
					removeIndex = j;
				}

				result = true;
				break;
			}
		}
	}

	return result;
}

void UTESPartySettingManager::SetCharacterEmptySlotHightLight(bool isVisible)
{
	if (true == _charSlotInfoWidgetList.IsValidIndex(3))
	{
		if (true == ::IsValid(_charSlotInfoWidgetList[3]))
		{
			_charSlotInfoWidgetList[3]->SetEmptySlotHightLight(isVisible);
		}
	}
}

void UTESPartySettingManager::SetCharacterSlotHightList(bool isVisible)
{
	if (true == _charSlotInfoWidgetList.IsValidIndex(0))
	{
		if (true == ::IsValid(_charSlotInfoWidgetList[0]))
		{
			_charSlotInfoWidgetList[0]->SetTutorialHightList(isVisible);
		}
	}

	if (true == _charSlotInfoWidgetList.IsValidIndex(3))
	{
		if (true == ::IsValid(_charSlotInfoWidgetList[3]))
		{
			_charSlotInfoWidgetList[3]->SetTutorialHightList(isVisible);
		}
	}
}

ETESPartySettingContents UTESPartySettingManager::GetEntryRoute() const
{
	return _partySettingByContents;
}

uint16 UTESPartySettingManager::GetUsedRecentlyPartyUniqueId() const
{
	//. 현재 어디로 들어와서 어떤 파티를 편성하고 있는지 알고 알려줘야함.
	if (true == _isCurrentPartySallyForced)
	{
		return TESConstantValue::SallyForcedPartyUnique;
	}
	else
	{
		return _recentUsedPartyUniqueId;
	}
}
 
uint16 UTESPartySettingManager::GetUsedRecentlyPartyUniqueIdByContents(ETESPartySettingContents contents) const
{
	return SharedSaveGameManager->GetLastPlayedPartyIdByContents(contents);
}

TArray<uint16> UTESPartySettingManager::GetBattleUsedPartyUniqueIdList() const
{
	return _battleUsedPartyUniqueIdList;
}

int32 UTESPartySettingManager::GetCurrentSelectedStageId() const
{
	return _selectedStageId;
}

TArray<uint16> UTESPartySettingManager::GetPartyUniqueListByCharacterUnique(int64 characterUnique) const
{
	if (true == _partyUniqueListByCharacterUnique.Contains(characterUnique))
	{
		return _partyUniqueListByCharacterUnique[characterUnique];
	}

	return {};
}

TArray<uint16> UTESPartySettingManager::GetPartyUniqueByCharacterId(int32 characterId) const
{
	if (true == _partyUniqueListByCharacterId.Contains(characterId))
	{
		return _partyUniqueListByCharacterId[characterId];
	}

	return {};
}

TArray<int32> UTESPartySettingManager::GetCharactersIdByPartyUnique(uint16 partyUnique) const
{
	if (true == _charactersIdByPartyUnique.Contains(partyUnique))
	{
		return _charactersIdByPartyUnique[partyUnique];
	}

	return {};
}

TArray<int64> UTESPartySettingManager::GetCharactersUniqueByPartyUnique(uint16 partyUnique) const
{
	if (true == _charactersUniqueByPartyUnique.Contains(partyUnique))
	{
		return _charactersUniqueByPartyUnique[partyUnique];
	}

	return {};
}

int32 UTESPartySettingManager::GetNumberOfLockedSlot() const
{
	return _numofLockedSlot;
}

int32 UTESPartySettingManager::GetSelectedSlotIndex() const
{
	return _selectedSlotIndex;
}

TArray<UTESUWPartySettingSwapSlotInfo*> UTESPartySettingManager::GetCharacterSwapSlotList() const
{
	return _characterSwapSlotList;
}

TArray<class UTESUWPartySettingCharSlotInfo*> UTESPartySettingManager::GetCharacterInfoSlot() const
{
	return _charSlotInfoWidgetList;
}

TMap<int32, class ATESCharacterPartySetting*> UTESPartySettingManager::GetCharacterMap() const
{
	return _characterMap;
}

TMap<int32, AActor*> UTESPartySettingManager::GetCharacterSlotActorMap() const
{
	return _characterLocationSlotActorMap;
}

TArray<class UTESGDCharacter*> UTESPartySettingManager::GetCurrentPartyGDCharacterList() const
{
	TArray<class UTESGDCharacter*> gdCharacterList = {};

	for (TPair<int32, ATESCharacterPartySetting*> child : _characterMap)
	{
		if (nullptr != child.Value)
		{
			TOptional<UTESGDCharacter*> characterData = SharedUserDataManager->FindHaveCharacter(child.Value->GetCharacterUnique());

			if (false == characterData.IsSet())
			{
				if (TESConstantValue::NoneValue == child.Value->GetCharacterUnique())
				{
					if (true == ::IsValid(child.Value->GetCharacterData()))
					{
						gdCharacterList.Add(child.Value->GetCharacterData());
					}
				}
				
			}
			else
			{
				gdCharacterList.Add(characterData.GetValue());
			}
		}
		else
		{
			gdCharacterList.Add(nullptr);
		}
	}

	return gdCharacterList;
}

UTESUWPartySettingDetail* UTESPartySettingManager::GetPartySettingDetail() const
{
	return _mainPartySettingDetailWidget;
}

void UTESPartySettingManager::ChangeWidget(ETEPartySettingWidgetId widgetId, bool isInited /*= false*/)
{
	if (_curWidgetID == widgetId)
		return;

	TESCHECK(nullptr != _mainPartySettingDetailWidget);

	//.어느 컨텐츠인가에 따라 달라짐
	if (ETESPartySettingContents::FromTeamTagBattle == _partySettingByContents)
	{
		TESCHECK(nullptr != _teamTagPartySettingWidget);

		if (widgetId == ETEPartySettingWidgetId::Main)
		{
			_teamTagPartySettingWidget->OpenWidget();
			_teamTagPartySettingWidget->RefreshWidget();
			_mainPartySettingDetailWidget->CloseWidget();
		}
		else if (widgetId == ETEPartySettingWidgetId::Detail)
		{
			_teamTagPartySettingWidget->CloseWidget();
			_mainPartySettingDetailWidget->OpenWidget();
		}
		
	}
	else if (ETESPartySettingContents::FromMirageGarden == _partySettingByContents)
	{
		if (true == ::IsValid(_mirageGardenPartySettingWidget))
		{
			if (widgetId == ETEPartySettingWidgetId::Main)
			{
				_mirageGardenPartySettingWidget->OpenWidget();
				_mirageGardenPartySettingWidget->RefreshWidget();
				_mainPartySettingDetailWidget->CloseWidget();
			}
			else if (widgetId == ETEPartySettingWidgetId::Detail)
			{
				_mirageGardenPartySettingWidget->CloseWidget();
				_mainPartySettingDetailWidget->OpenWidget();
			}
		}
	}
	else
	{ 
		TESCHECK(nullptr != _mainPartySettingWidget);

		if (widgetId == ETEPartySettingWidgetId::Main)
		{

			_mainPartySettingWidget->OpenWidget();
			_mainPartySettingWidget->RefreshPartyCombatPower();
			_mainPartySettingWidget->ReFreshStellaStrikeInfo();
			_mainPartySettingWidget->RefreshStartBattleButton();
			_mainPartySettingDetailWidget->CloseWidget();

			if (_curWidgetID == ETEPartySettingWidgetId::Detail)
			{
				if (true == SharedNewbieForcedTutorialManager->IsShowingTutorial())
				{
					SharedNewbieForcedTutorialManager->SetNextTutorialWidgetByTutorialType(ETESNewbieForcedTutorialManagerType::PartySettingTutorial);
				}
			}
		}
		else if (widgetId == ETEPartySettingWidgetId::Detail)
		{
			_mainPartySettingWidget->CloseWidget();
			_mainPartySettingDetailWidget->OpenWidget();
		}
	}

	_curWidgetID = widgetId;
}

void UTESPartySettingManager::TickCameraMoving(const float& deltaTime)
{
    if (false == ::IsValid(_sc_sceneCamera))
        return;

    if (false == _isFocusingToCharacter)
        return;

    FVector curCameraLocation = _sc_sceneCamera->GetActorLocation();
    FVector newCameraLocation = FMath::VInterpTo(curCameraLocation, _partySettingCameraTargetLocation, deltaTime, 12.0f);	
    float distSquared = FVector::DistSquared(_partySettingCameraTargetLocation, newCameraLocation);
	
	FRotator curCameraRotator = _sc_sceneCamera->GetActorRotation();
	FRotator newCameraRotator = FMath::RInterpTo(curCameraRotator, _partySettingCameraTargetRotation, deltaTime, 12.0f);
	float distAngular = _partySettingCameraTargetRotation.GetManhattanDistance(newCameraRotator);
	    
    if ((1.0f > distSquared) && (0.001f >= distAngular))
    {
        _isFocusingToCharacter = false;	
    }

	_sc_sceneCamera->SetActorLocation(newCameraLocation);
	_sc_sceneCamera->SetActorRotation(newCameraRotator);
}

void UTESPartySettingManager::OnEvent_SwitchPartySettings(int32 slotIndex)
{
	_selectedSlotIndex = slotIndex;

	//. 씬에서는 캐릭터를 줌인 혹은 줌 아웃을 해준다.
	SetFocusToCharacter(slotIndex);

	if (TESConstantValue::NoneValue == slotIndex)
	{
		//. 캐릭터 정보창을 다 켜준다.
		for (UTESUWPartySettingCharSlotInfo* characterSlotInfo : _charSlotInfoWidgetList)
		{
			if (true == ::IsValid(characterSlotInfo))
			{
				characterSlotInfo->OpenWidget();
			}
		}
		
		//. 꺼져있는 아이들을 다시 켜줌.
		for (TPair<int32, ATESCharacterPartySetting*> child : _characterMap)
		{
			if (nullptr != child.Value)
			{
				child.Value->SetActorHiddenInGame(false);
				child.Value->SetHiddenDollMesh(false);
			}
		}

		ChangeWidget(ETEPartySettingWidgetId::Main);
	}
	else
	{
		//. 캐릭터 정보창을 다 꺼준다.
		for (UTESUWPartySettingCharSlotInfo* characterSlotInfo : _charSlotInfoWidgetList)
		{
			if (true == ::IsValid(characterSlotInfo))
			{
				characterSlotInfo->CloseWidget();
			}
		}

		//. 한 캐릭터를 제외하고 나머지는 사라지게 해준다.
		for (TPair<int32, ATESCharacterPartySetting*> child : _characterMap)
		{
			//. 같은 슬롯이라면 그대로 둠.
			if (child.Key == slotIndex)
			{
				continue;
			}

			if (nullptr != child.Value)
			{
				child.Value->SetActorHiddenInGame(true);
				child.Value->SetHiddenDollMesh(true);
			}
		}

		//. 캐릭터 상세보기 패널 세팅
		//. 필요한 정보 : 각 슬롯에 있는 캐릭터 아이디, 클릭한 캐릭터 아이디 혹은 인덱스, 
		TMap<int32, int64> joinedPartyMap;
		for (TPair<int32, ATESCharacterPartySetting*> child : _characterMap)
		{
			if (true == ::IsValid(child.Value))
			{
				joinedPartyMap.Add(child.Key, child.Value->GetCharacterUnique());
			}
			else
			{
				joinedPartyMap.Add(child.Key, 0);
			}
		}

		TESCHECK(nullptr != _mainPartySettingDetailWidget);

		if (ETESPartySettingContents::FromTeamTagBattle == _partySettingByContents)
		{
			if (true == joinedPartyMap.Contains(slotIndex))
			{
				int32 selectedCharacterId = 0;

				if (true == ::IsValid(_characterMap[slotIndex]))
				{
					selectedCharacterId = _characterMap[slotIndex]->GetCharacterTableId();
				}

				if (false == _isCurrentPartySallyForced)
				{
					_mainPartySettingDetailWidget->SetWidget(_characterMap, slotIndex, joinedPartyMap[slotIndex], selectedCharacterId, _recentUsedPartyUniqueId, _numofLockedSlot);
				}
				else
				{
					_mainPartySettingDetailWidget->SetWidget(_characterMap, slotIndex, joinedPartyMap[slotIndex], selectedCharacterId, 1, _numofLockedSlot);
				}
			}
			else
			{
				TESLOG(Error, TEXT("Out of Array from joinedPartyMap : %d"), slotIndex);
			}
		}
		else
		{ 
			if (true == joinedPartyMap.Contains(slotIndex))
			{
				int32 selectedCharacterId = 0;

				if (true == ::IsValid(_characterMap[slotIndex]))
				{
					selectedCharacterId = _characterMap[slotIndex]->GetCharacterTableId();
				}

				if (false == _isCurrentPartySallyForced)
				{
					_mainPartySettingDetailWidget->SetWidget(_characterMap, slotIndex, joinedPartyMap[slotIndex], selectedCharacterId, _recentUsedPartyUniqueId, _numofLockedSlot);
				}
				else
				{
					_mainPartySettingDetailWidget->SetWidget(_characterMap, slotIndex, joinedPartyMap[slotIndex], selectedCharacterId, 1, _numofLockedSlot);
				}
			}
			else
			{
				TESLOG(Error, TEXT("Out of Array from joinedPartyMap : %d"), slotIndex);
			}
		}
		ChangeWidget(ETEPartySettingWidgetId::Detail);
	}
}

void UTESPartySettingManager::OnEvent_BackWidget()
{
	//.메인 일때
	if (ETEPartySettingWidgetId::Main == _curWidgetID)
	{
		_mainPartySettingWidget->OnEvent_BackWidget();
	}
	//.디테일 일때
	else if(ETEPartySettingWidgetId::Detail == _curWidgetID)
	{
		_mainPartySettingDetailWidget->OnEvent_BackWidget();
	}
}

void UTESPartySettingManager::OnEvent_ConfirmCharacter()
{
	if (ETEPartySettingWidgetId::Detail == _curWidgetID)
	{
		_mainPartySettingDetailWidget->OnEvent_ChangeCharacter();
	}
}
