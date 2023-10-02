// Fill out your copyright notice in the Description page of Project Settings.

#include "TESConversationManager.h"
#include "Contents/Common/Manager/TESSaveGameManager.h"
#include "Contents/Common/Manager/TESGameResourceManager.h"
#include "Contents/Common/Manager/TESUserDataManager.h"
#include "Contents/Common/Manager/TESUIManager.h"
#include "Contents/Common/Manager/TESPopupManager.h"
#include "Contents/Common/Manager/TESSceneManager.h"
#include "Contents/Common/Manager/TESUserTraceManager.h"
#include "Contents/Conversation/Character/TESCharacterConversation.h"
#include "Contents/PartySetting/Manager/TESPartySettingManager.h"
#include "Contents/Lobby/Manager/TESLobbyManager.h"
#include "Contents/Lobby/PlayerController/TESPlayerControllerLobby.h"
#include "Contents/Lobby/Character/TESCharacterLobby.h"
#include "Contents/Lobby/Character/TESCharacterMovableNPC.h"
#include "Contents/Lobby/UI/TESUWLobbyMain.h"
#include "Contents/Intro/PlayerController/TESPlayerControllerIntro.h"
#include "Contents/Battle/PlayerController/TESPlayerControllerBattle.h"
#include "Contents/Battle/Manager/TESBattleCharacterManager_New.h"
#include "Contents/Battle/Manager/TESTriggerEventManager.h"
#include "Contents/Battle/Manager/TESStellaStrikeManager.h"
#include "Contents/Battle/Manager/TESStageManager.h"
#include "Contents/Battle/Character/TESCharacterBattle.h"
#include "Contents/Navigation/Manager/TESNavigationManager.h"
#include "Contents/Tutorial/Manager/TESNewbieForcedTutorialManager.h"
#include "Contents/Conversation/UI/TESUWConversationMain.h"
#include "Contents/MainStory/Manager/TESMainStoryManager.h"
#include "Contents/Intro/UI/TESUWIntroMain.h"
#include "Sound/Manager/TESSoundManager.h"
#include "Network/TESNetworkManager.h"
#include "UniqueAbyss/Manager/TESUniqueAbyssManager.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Camera/CameraActor.h"
#include "TESGameInstance.h"
#include "TESUtilTable.h"
#include "TESUtils.h"
#include "TESUtilPath.h"

void UTESConversationManager::Start()
{
	Super::Start();

	Clear();
}

void UTESConversationManager::Reset()
{
	Super::Reset();

    Clear();
}

void UTESConversationManager::Shutdown()
{
	Super::Shutdown();

	Clear();
}

void UTESConversationManager::Clear()
{
	//. 초기화
	ClearCharacterMap();

	_conversationTableRows.Empty();

	_conversationDataMapById.Empty();

	//. 회화 카메라 변수들 초기화
	_conversationCamera = nullptr;
	_originCameraVector = FVector::ZeroVector;
	_initConversationCameraLocation = FVector::ZeroVector;		
	_destinationConversationCameraLocation = FVector::ZeroVector;
	_conversationCameraInitFov = 0.0f;
	_conversationCameraDestinationFov = 0.0f;
	_conversationCameraMovingRealTime = 0.0f;					
	_conversationCameraMovingRealTimeForFoV = 0.0f;
	_conversationCameraInitTime = 0.0f;							
	_conversationCameraDestinationTime = 0.0f;					
	_initCameraMoving = false;
	_initCameraFovChanging = false;
	_movingFovEnd = false;
	_movingLocationEnd = false;
	_conversationInitData = {};
	_isPerformConversation = false;

	SetEnableTick(false);
}

void UTESConversationManager::Tick(const float& deltaTime)
{
	Super::Tick(deltaTime);

	TickCamera(deltaTime);
}

void UTESConversationManager::ClearCharacterMap()
{
	for (int32 slotIndex = 1; slotIndex <= TESConstantValue::MaxConversationCount; ++slotIndex)
	{
		if (true == _conversationCharacterMap.Contains(slotIndex))
		{
			if (nullptr != _conversationCharacterMap[slotIndex])
			{
				_conversationCharacterMap[slotIndex]->Clear();
				_conversationCharacterMap.Remove(slotIndex);
			}
		}
	}

	_conversationCharacterMap.Empty();
}

void UTESConversationManager::TickCamera(const float& deltaSecond)
{
	TickConversationCamera(deltaSecond);
}

void UTESConversationManager::TickConversationCamera(const float& deltaSecond)
{
	//. 이동이 전부 끝났으면 틱을 꺼준다.
	if (true == _movingFovEnd && true == _movingLocationEnd)
	{
		return;
	}

	TickConversationCameraFov(deltaSecond);

	TickConversationCameraLocation(deltaSecond);
}

void UTESConversationManager::TickConversationCameraFov(const float& deltaSecond)
{
	if (nullptr == _conversationCamera)
		return;

	if (true == _initCameraFovChanging)
	{
		if (0 >= _conversationCameraFovTime)
		{
			_conversationCamera->GetCameraComponent()->FieldOfView = _conversationCameraDestinationFov;
			_initCameraFovChanging = false;
			_movingFovEnd = true;
			return;
		}

		_conversationCameraMovingRealTimeForFoV += deltaSecond;

		float curCameraFov = _conversationCamera->GetCameraComponent()->FieldOfView;
		float newCameraFov = 0.0f;

		//. 시간 x 속력 = 거리
		//. 속력 = 거리 / 시간
		float interpSpeed = FMath::Abs((_conversationCameraInitFov - _conversationCameraDestinationFov) / _conversationCameraFovTime);

		//. 카메라 이동 시간이 최종시간에 도달하면 종료한다.
		if (_conversationCameraFovTime <= _conversationCameraMovingRealTimeForFoV)
		{
			_conversationCameraFovTime = 0.0f;
			_conversationCameraMovingRealTimeForFoV = 0.0f;

			_initCameraFovChanging = false;

			_movingFovEnd = true;
			return;
		}

		//. 카메라 Fov값 변경(줌 인/아웃)
		newCameraFov = FMath::FInterpConstantTo(curCameraFov, _conversationCameraDestinationFov, deltaSecond, interpSpeed);

		_conversationCamera->GetCameraComponent()->FieldOfView = newCameraFov;
	}
}

void UTESConversationManager::TickConversationCameraLocation(const float& deltaSecond)
{
	if (nullptr == _conversationCamera)
		return;

	//. 대화 시작할때 카메라 움직임 
	if (true == _initCameraMoving)
	{
		if (0 >= _conversationCameraInitTime)
		{
			_initCameraMoving = false;
			_prevCameraLocation = _initConversationCameraLocation;
			_conversationCamera->SetActorRelativeLocation(_initConversationCameraLocation);
			return;
		}

		//. 실시간
		_conversationCameraMovingRealTime += deltaSecond;

		//. 카메라 위치 이동
		FVector curCameraLocation = _conversationCamera->GetActorLocation();
		FVector newCameraLocation = FVector::ZeroVector;

		float dist = FMath::Sqrt(FVector::DistSquared(_prevCameraLocation, _initConversationCameraLocation));

		float interpSpeed = FMath::Abs(dist / _conversationCameraInitTime);

		newCameraLocation = FMath::VInterpConstantTo(curCameraLocation, _initConversationCameraLocation, deltaSecond, interpSpeed);
		_conversationCamera->SetActorRelativeLocation(newCameraLocation);

		//. 카메라 이동 시간이 최종시간에 도달하면 종료한다.
		if (_conversationCameraInitTime <= _conversationCameraMovingRealTime)
		{
			_conversationCameraInitTime = 0.0f; 
			_conversationCameraMovingRealTime = 0.0f;

			_prevCameraLocation = newCameraLocation;

			_initCameraMoving = false;
		}

		return;
	}

	//. 첫번째 카메라가 이동한 후 다음 목적지로 이동할때
	if (0 >= _conversationCameraDestinationTime)
	{
		_movingLocationEnd = true;
		_prevCameraLocation = _destinationConversationCameraLocation;
		_conversationCamera->SetActorRelativeLocation(_destinationConversationCameraLocation);
		return;
	}

	//. 실시간
	_conversationCameraMovingRealTime += deltaSecond;

	//. 카메라 이동 시간이 최종시간에 도달하면 종료한다.
	if (_conversationCameraDestinationTime <= _conversationCameraMovingRealTime)
	{
		_conversationCameraDestinationTime = 0.0f;
		_conversationCameraMovingRealTime = 0.0f;
		_movingLocationEnd = true;

		return;
	}

	FVector curCameraLocation = _conversationCamera->GetActorLocation();
	FVector newCameraLocation = FVector::ZeroVector;

	//. 줌인 했을때 카메라 속도
	//. 속도는 거리에 따라 계산을 해야한다. => x,y,z 좌표중 가장 긴걸로 계산
	float dist = FMath::Sqrt(FVector::DistSquared(_prevCameraLocation, _destinationConversationCameraLocation));
	float interpSpeed = FMath::Abs(dist / _conversationCameraDestinationTime);
	
	newCameraLocation = FMath::VInterpConstantTo(curCameraLocation, _destinationConversationCameraLocation, deltaSecond, interpSpeed);
	_conversationCamera->SetActorRelativeLocation(newCameraLocation);
}

void UTESConversationManager::SetConversation(ETypeConversationType category)
{
    //. 해당 카테고리의 정보들만 가져온다.(이벤트 대화, 호감도 대화 등)
	{
        _conversationTableRows.Empty();
        _conversationTableRows = TESUtilTable::GetConversationTableByCategory(category);
	}

	SetEnableTick(true);

    //. 얻어온 카테고리의 정보들로 대화에 필요한 데이터들을 재구축한다.
	{
        RemoveConversationDataAll();

        int32 maxCount = _conversationTableRows.Num();
        for (int32 i = 0; i < maxCount; ++i)
        {
            if (true == _conversationTableRows[i]->IsEmpty())
            {
                continue;
            }

            AddConversationData(_conversationTableRows[i]);
        }
	}
}

void UTESConversationManager::SetConversationLoadingFinishCallback(FOnTESAction onFinishAction)
{
	_onLoadingFinishAction = onFinishAction;
}

void UTESConversationManager::StartConversation(FTESConversationInitData& initData)
{
	_conversationInitData = initData;

	_onLoadingFinishAction.ExecuteIfBound();
	_onLoadingFinishAction.Unbind();
	_onLoadingFinishAction = {};
	
}

void UTESConversationManager::StartConversation(int32 conversationId, FTESConversationInitData conversationData)
{
	//. 타임스케일이 변경된 상태로 회화씬이 진행될수 있어(서포트 커맨드 등) 리셋합니다
	TesGameInstance->ResetGameTimeScale();
	SharedSoundManager->ResetSoundSpeed();

    FConversationRow conversationRowData = SharedTDBManager->GetConversationRow(conversationId);
    ETypeConversationType conversationType = conversationRowData.Category;
    SetConversation(conversationType);

    //. 대화에 필요한 정보들을 비동기 로드한 이후 해야될 행동들을 미리 세팅합니다.
    SetConversationLoadingFinishCallback(FOnTESAction::CreateLambda([=]()
    {
        if (true == CheckIfPossibleToPlay())
        {
			//TESLOG(Error, TEXT("conversation Id : %d"), conversationId);
            const FConversationRow& conversationRow = SharedTDBManager->GetConversationRow(conversationId);
            if (true == conversationRow.IsEmpty())
            {
				TESLOG_S(Error);
                return;
            }

            //. 대화를 실행했으므로 저장해준다.(테이블 값에 따라 저장할지 말지 정한다.)
            if (false == conversationRowData.IsAllowRepeat)
            {
                SharedSaveGameManager->AddGameEventCompleteDataByConversation(conversationId);
            }

			//. 로딩을 끝 마치고 대화 화면으로 넘어갑니다.
            FTESSceneTransitionData sceneTransitionData = {};
            sceneTransitionData._physicalSceneType = SharedSceneManager->GetCurrentPhysicalGameSceneType();
            sceneTransitionData._logicalSceneType = ETESLogicalGameSceneType::Conversation;
            sceneTransitionData._sceneArguments._intValues.Add(TEXT("ConversationId"), conversationId);
			sceneTransitionData._sceneArguments._intValues.Add(TEXT("ComeFrom"), static_cast<int32>(conversationData._startFrom));
            sceneTransitionData._fadeAtrribute._fadeType = ETESSceneFadeType::ConversationSceneChange;
			sceneTransitionData._fadeAtrribute._widgetFadeInAnimName = conversationRow.SceneTransitionInName._fadeIn;
			sceneTransitionData._fadeAtrribute._widgetFadeOutAnimName = conversationRow.SceneTransitionInName._fadeOut;
			sceneTransitionData._fadeAtrribute._fadeOutImmediately = true;
			sceneTransitionData._isShowAllUIWhenEntering = true;
			sceneTransitionData._isHideAllUIWhenExiting = true;

			SharedSceneManager->ChangeGameScene(sceneTransitionData);
        }
		else
		{
			TESLOG(Error, TEXT("Wrong ConversationId : %d"), conversationId);
		}
    }));

    //. 대화를 시작합니다. ( 비동기 로드 -> 콜백호출 -> 대화 시작 )
    StartConversation(conversationData);
}

void UTESConversationManager::EndConversation()
{
	//. 회화가 끝나면 캐릭터들을 전부 지워준다.
	ClearCharacterMap();

	SetEnableTick(false);	

	switch (_conversationInitData._startFrom)
	{
	case ETESConversationStartFrom::Lobby:
		{
			TOptional<FTESUserTraceData> previousUserTraceData = SharedUserTraceManager->PeekUserTrace();
			if (true == previousUserTraceData.IsSet())
			{
				if (ETESUserTraceType::Scene == previousUserTraceData.GetValue()._userTraceType
					&& ETESLogicalGameSceneType::Lobby == previousUserTraceData.GetValue()._logicalSceneType)
				{
					LobbyEndConversation();
				}
			}

			FTESSceneTransitionData sceneTransitionData = {};
			sceneTransitionData._physicalSceneType = SharedSceneManager->GetCurrentPhysicalGameSceneType();
			sceneTransitionData._logicalSceneType = ETESLogicalGameSceneType::Lobby;
			sceneTransitionData._fadeAtrribute._fadeType = ETESSceneFadeType::ConversationSceneChange;
			sceneTransitionData._fadeAtrribute._widgetFadeInAnimName = _conversationDataMapById[_currentConversationId]._sceneTransitionOut._fadeIn;
			sceneTransitionData._fadeAtrribute._widgetFadeOutAnimName = _conversationDataMapById[_currentConversationId]._sceneTransitionOut._fadeOut;


#ifdef TES_BUILD_OPTION_DEV
			if (true == _isEnterDevelopHelper)
			{
				EndDevelopHelper();

				return;
			}
#endif //. TES_BUILD_OPTION_DEV

			//. 회화가 끝나면 npc 메뉴가 있을때는 바로 틀어준다.
			SharedLobbyManager->CallbackAfterConversation(sceneTransitionData);
		}
		break;
	case ETESConversationStartFrom::Battle:
		{		
			ATESPlayerControllerBattle* playerControllerBattle = Cast<ATESPlayerControllerBattle>(GetWorld()->GetFirstPlayerController());
			TESCHECK(nullptr != playerControllerBattle);
			playerControllerBattle->ResetViewTarget();

			for (ATESCharacterBattle* character : SharedBattleCharacterManager->GetAllCharacters(ETeamSide::TeamSideA))
			{
				character->EndUnderEvent();
			}

			for (ATESCharacterBattle* character : SharedBattleCharacterManager->GetAllCharacters(ETeamSide::TeamSideB))
			{
				character->EndUnderEvent();
			}

			if (_conversationInitData._nextTriggerEventId > 0)
			{
				FTESSceneTransitionData sceneTransitionData = {};
				sceneTransitionData._physicalSceneType = SharedSceneManager->GetCurrentPhysicalGameSceneType();
				sceneTransitionData._logicalSceneType = ETESLogicalGameSceneType::Battle;
				sceneTransitionData._sceneArguments._intValues.Add(TEXT("NextTriggerEventId"), _conversationInitData._nextTriggerEventId);
				sceneTransitionData._fadeAtrribute._fadeType = ETESSceneFadeType::ConversationSceneChange;
				sceneTransitionData._fadeAtrribute._widgetFadeInAnimName = _conversationDataMapById[_currentConversationId]._sceneTransitionOut._fadeIn;
				sceneTransitionData._fadeAtrribute._widgetFadeOutAnimName = _conversationDataMapById[_currentConversationId]._sceneTransitionOut._fadeOut;
				sceneTransitionData._fadeAtrribute._fadeOutImmediately = true;

				SharedSoundManager->FadeOutBGM(TESConstantValue::DefaultBGMFadeOutTime);
				SharedSoundManager->StopAmbientSound();

				SharedSceneManager->ChangeGameScene(sceneTransitionData);
				return;
			}

			SharedSoundManager->FadeOutBGM(TESConstantValue::DefaultBGMFadeOutTime);
			SharedSoundManager->StopAmbientSound();

			FTESSceneTransitionData sceneTransitionData = {};
			sceneTransitionData._physicalSceneType = SharedSceneManager->GetCurrentPhysicalGameSceneType();
			sceneTransitionData._logicalSceneType = ETESLogicalGameSceneType::Battle;
			sceneTransitionData._isShowAllUIWhenEntering = true;
			sceneTransitionData._isHideAllUIWhenExiting = true;
			sceneTransitionData._fadeAtrribute._fadeType = ETESSceneFadeType::ConversationSceneChange;
			sceneTransitionData._fadeAtrribute._widgetFadeInAnimName = _conversationDataMapById[_currentConversationId]._sceneTransitionOut._fadeIn;
			sceneTransitionData._fadeAtrribute._widgetFadeOutAnimName = _conversationDataMapById[_currentConversationId]._sceneTransitionOut._fadeOut;
			sceneTransitionData._fadeAtrribute._fadeOutImmediately = true;
			
			SharedSceneManager->ChangeGameScene(sceneTransitionData);

		}
		break;
	case ETESConversationStartFrom::PartySetting:
		{
			if (_conversationInitData._nextTriggerEventId > 0)
			{
				bool isPlayed = false;
				SharedTriggerEventManager->TriggerEventActivation(_conversationInitData._nextTriggerEventId, nullptr, isPlayed, _conversationInitData);

				const FTriggerEventRow& triggerEventRow = SharedTDBManager->GetTriggerEventRow(_conversationInitData._nextTriggerEventId);
				if (false == triggerEventRow.IsEmpty())
				{
					if (ETypeTriggerEvent::ChapterTitleScene == triggerEventRow.T_TriggerEvent)
					{
						return;
					}
				}

				SharedSoundManager->FadeOutBGM(TESConstantValue::DefaultBGMFadeOutTime);
				SharedSoundManager->StopAmbientSound();

				return;
			}
			else
			{
				SharedTriggerEventManager->SetReplayStory(false);
			}
		}
		break;
	case ETESConversationStartFrom::Intro:
		{
			if (_conversationInitData._nextTriggerEventId > 0)
			{
				bool isPlayed = false;
				SharedTriggerEventManager->TriggerEventActivation(_conversationInitData._nextTriggerEventId, nullptr, isPlayed, _conversationInitData);

				const FTriggerEventRow& triggerEventRow = SharedTDBManager->GetTriggerEventRow(_conversationInitData._nextTriggerEventId);
				if (false == triggerEventRow.IsEmpty())
				{
					if (ETypeTriggerEvent::ChapterTitleScene == triggerEventRow.T_TriggerEvent)
					{
						return; 
					}
				}

				SharedSoundManager->FadeOutBGM(TESConstantValue::DefaultBGMFadeOutTime);
				SharedSoundManager->StopAmbientSound();

				return;
			}
			else
			{
				SharedTriggerEventManager->SetReplayStory(false);
			}
			
			//. 이미 한번 캐릭터 선택을 했을 경우,
            if (true == SharedUserDataManager->CheckPlayerSelectedCharacter())
            {
				SharedSoundManager->FadeOutBGM(TESConstantValue::DefaultBGMFadeOutTime);
				SharedSoundManager->StopAmbientSound();

                FTESSceneTransitionData sceneTransitionData = {};
                sceneTransitionData._physicalSceneType = ETESPhysicalGameSceneType::Intro;
                sceneTransitionData._logicalSceneType = ETESLogicalGameSceneType::Intro;
                sceneTransitionData._fadeAtrribute._fadeType = ETESSceneFadeType::ConversationSceneChange;
				sceneTransitionData._fadeAtrribute._widgetFadeInAnimName = _conversationDataMapById[_currentConversationId]._sceneTransitionOut._fadeIn;
				sceneTransitionData._fadeAtrribute._widgetFadeOutAnimName = _conversationDataMapById[_currentConversationId]._sceneTransitionOut._fadeOut;
				sceneTransitionData._sceneArguments._intValues.Add(TEXT("BeginIntroPresetType"), (int32)EIntroPresetType::End);
                SharedSceneManager->ChangeGameScene(sceneTransitionData);
			}
			//. 이미 한번 캐릭터 선택을 하지 않았을 경우,
			else
			{
				SharedSoundManager->FadeOutBGM(TESConstantValue::DefaultBGMFadeOutTime);
				SharedSoundManager->StopAmbientSound();

                FTESSceneTransitionData sceneTransitionData = {};
                sceneTransitionData._physicalSceneType = ETESPhysicalGameSceneType::Intro;
                sceneTransitionData._logicalSceneType = ETESLogicalGameSceneType::Intro;
                sceneTransitionData._fadeAtrribute._fadeType = ETESSceneFadeType::ConversationSceneChange;
				sceneTransitionData._fadeAtrribute._widgetFadeInAnimName = _conversationDataMapById[_currentConversationId]._sceneTransitionOut._fadeIn;
				sceneTransitionData._fadeAtrribute._widgetFadeOutAnimName = _conversationDataMapById[_currentConversationId]._sceneTransitionOut._fadeOut;
				sceneTransitionData._sceneArguments._intValues.Add(TEXT("BeginIntroPresetType"), (int32)EIntroPresetType::LanguageSelection);
                SharedSceneManager->ChangeGameScene(sceneTransitionData);
			}
		}
		break;
	default:
		{
			TESLOG(Error, TEXT("Wrong Type"));
		}
	}	
}

void UTESConversationManager::LobbyEndConversation()
{
	FTESLobbyInteractionInfo currentInteractionInfo = SharedLobbyManager->GetCurrentInteractionInfo();
	if (FTESLobbyInteractionInfo() == currentInteractionInfo)
	{
		if (nullptr != currentInteractionInfo._lobbyCharacter)
			currentInteractionInfo._lobbyCharacter->EndConversation();
	}
	else if (currentInteractionInfo._menuIdList.Num() <= 0)
	{
		if (nullptr != currentInteractionInfo._lobbyCharacter)
			currentInteractionInfo._lobbyCharacter->EndConversation();

		ATESCharacterMovableNPC* targetNPC = Cast<ATESCharacterMovableNPC>(currentInteractionInfo._targetNPC);
		if (nullptr != targetNPC)
		{
			targetNPC->EndConversation();
		}
	}
}

bool UTESConversationManager::IsPerformConversation()
{
	return _isPerformConversation;
}

void UTESConversationManager::SetPerformConversation(bool isPerformConversation)
{
	TESLOG(Display, TEXT("Perform Conversation %s"), isPerformConversation ? TEXT("Start") : TEXT("End"));
	_isPerformConversation = isPerformConversation;
}

void UTESConversationManager::UpdateConversation(int32 converationId)
{
	//. 카메라 먼저 업데이트
	SetConversationCamera();

	//. 현재 데이터를 받아와서 등장인물들이 바뀌는지, 없어지는지 체크를 한 후에 변경한다.
	if (0 < GetConversationDataCount())
	{
		if (true == _conversationDataMapById.Contains(converationId))
		{
			//. 현재 대화에서 등장하는 캐릭터들이 전 대사로부터 그대로 있으면 유지, 없으면 추가, 없어지면 삭제 해줍니다.
			for (int slotIndex = 1; slotIndex <= _conversationDataMapById[converationId]._speakingCharacters.Num(); ++slotIndex)
			{
				int32 characterId = _conversationDataMapById[converationId]._speakingCharacters[slotIndex]._characterId;

				if (TESConstantValue::MaleHeroCharacterId == characterId)
				{
					GENDER_TYPE gender = SharedUserDataManager->GetUserGender();
					switch (gender)
					{
					case GENDER_TYPE::GENDER_TYPE_MALE:
						{
							//. 남자일 경우에는 그대로
						}
						break;
					case GENDER_TYPE::GENDER_TYPE_FEMALE:
						{
							//. 여자일 경우에는 +1만큼 더해준다.
							characterId += 1;
						}
						break;
					}
				}

				//. 현재 대화에 등장하지않지만, 바로 전 대화에서 있었으면 삭제해줍니다.
				if (0 == characterId)
				{
					if (true == _conversationCharacterMap.Contains(slotIndex))
					{
						//. 이전에 있던 모델링을 제거합니다.
						if (nullptr != _conversationCharacterMap[slotIndex])
						{
							_conversationCharacterMap[slotIndex]->Clear();
						}

						_conversationCharacterMap.Remove(slotIndex);
					}
				}
				else
				{
					//. 변경할 데이터가 있으면, 그에 맞게 변경해줍니다.
					if (false == _conversationCharacterMap.Contains(slotIndex))
					{
						InitCharacter(characterId, slotIndex);
					}
					else
					{
						//. 캐릭터가 이미 있고 전 대화랑 똑같은 위치에 있으면 그대로 유지, 캐릭터가 바뀌면 삭제 후 새로 생성
						if (true == ::IsValid(_conversationCharacterMap[slotIndex]))
						{
							if (characterId != _conversationCharacterMap[slotIndex]->GetCharacterId())
							{
								//. 현재 이 슬롯에는 캐릭터가 존재하지만, 다른 캐릭으로 바뀌므로 삭제 한 후 새로운 캐릭을 넣는다.
								_conversationCharacterMap[slotIndex]->Clear();
								_conversationCharacterMap.Remove(slotIndex);
								InitCharacter(characterId, slotIndex);
							}
							else if (characterId == _conversationCharacterMap[slotIndex]->GetCharacterId())
							{
								SetDetachToActor(slotIndex);
								_conversationCharacterMap[slotIndex]->SetCharacterInfo(_conversationDataMapById[converationId]._speakingCharacters[slotIndex]);
								SetAttachToActor(slotIndex);
							}
						}
					}
				}
			}
		}
		else
		{
			TESLOG(Error, TEXT("conversation id is invalid"));
		}
	}
}

void UTESConversationManager::InitCharacter(int32 characterId, int32 slotIndex)
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

	if (true == _conversationDataMapById.Contains(_currentConversationId))
	{
		ATESCharacterConversation* conversationCharacter = worldContext->SpawnActor<ATESCharacterConversation>(location, FRotator::ZeroRotator, spawnInfo);
		TESCHECK(nullptr != conversationCharacter);

		conversationCharacter->Init(characterId, _conversationDataMapById[_currentConversationId]._speakingCharacters[slotIndex]);

		_conversationCharacterMap.Add(slotIndex, conversationCharacter); 

		SetAttachToActor(slotIndex);
	}
	else
	{
		TESLOG(Error, TEXT("Wrong conversation Id"));
	}
}

void UTESConversationManager::InitCamera()
{
	//. 초기 위치는 카메라 기본위치
	_originCameraVector = TESConstantValue::ConversationCameraOriginLocation;

	if (nullptr == _conversationCamera)
	{
		_conversationCamera = GetCameraActor(TESConstantValue::SceneCameraName_Conversation);
		TESCHECK(nullptr != _conversationCamera);

		//. 카메라 위치를 초기화 해준다.
		_conversationCamera->SetActorLocation(_originCameraVector);
	}
}

void UTESConversationManager::SetConversationCamera()
{
	InitCamera();

	if (true == _conversationDataMapById.Contains(_currentConversationId))
	{
		_initConversationCameraLocation.X = _originCameraVector.X + _conversationDataMapById[_currentConversationId]._initCameraLocation._x;
		_initConversationCameraLocation.Y = _originCameraVector.Y + _conversationDataMapById[_currentConversationId]._initCameraLocation._y;
		_initConversationCameraLocation.Z = _originCameraVector.Z + _conversationDataMapById[_currentConversationId]._initCameraLocation._z;
		_conversationCameraInitTime = _conversationDataMapById[_currentConversationId]._initCameraLocation._time;

		_destinationConversationCameraLocation.X = _originCameraVector.X + _conversationDataMapById[_currentConversationId]._endCameraLocation._x;
		_destinationConversationCameraLocation.Y = _originCameraVector.Y + _conversationDataMapById[_currentConversationId]._endCameraLocation._y;
		_destinationConversationCameraLocation.Z = _originCameraVector.Z + _conversationDataMapById[_currentConversationId]._endCameraLocation._z;
		_conversationCameraDestinationTime = _conversationDataMapById[_currentConversationId]._endCameraLocation._time;

		_conversationCameraDestinationFov = _conversationDataMapById[_currentConversationId]._cameraFov;

		if (0 == _conversationCameraDestinationFov)
		{
			_conversationCameraDestinationFov = 45.0f;
		}
		else if (90 <= _conversationCameraDestinationFov)
		{
			_conversationCameraDestinationFov = 90.0f;
		}

		_conversationCameraFovTime = _conversationDataMapById[_currentConversationId]._cameraFovTime;

		_initCameraMoving = true;
		_initCameraFovChanging = true;

		_movingFovEnd = false;
		_movingLocationEnd = false;

		TESCHECK(nullptr != _conversationCamera);
		_conversationCameraInitFov = _conversationCamera->GetCameraComponent()->FieldOfView;

		_prevCameraLocation = _conversationCamera->GetActorLocation();
	}
	else
	{
		TESLOG(Error, TEXT("conversation id is invalid"));
	}
	
}

void UTESConversationManager::SetConversationEnd()
{
	//. 카메라 이동 완료
	SetConversationCameraToEnd();

	//. 캐릭터 이동 및 애니메이션, 모퍼 완료
	SetCharacterToEnd();
}

void UTESConversationManager::SetCharacterToEnd()
{
	//. 모퍼를 아이들 상태로 바꿔준다.
	for (TPair<int32, class ATESCharacterConversation*> child : _conversationCharacterMap)
	{
		if (nullptr != child.Value)
		{
			child.Value->SetcharacterMouthMorpherToIdle();
		}
	}
}

void UTESConversationManager::SetToNextScene(ETESPhysicalGameSceneType physicalSceneType)
{
#ifdef TES_BUILD_OPTION_DEV
	if (true == _isEnterDevelopHelper)
	{
		EndDevelopHelper();

		return;
	}
#endif //. TES_BUILD_OPTION_DEV

	//. 씬 아이디에 따라 씬 이동하기 전에 대한 내용을 세팅하고 씬 이동 시킨다.
	switch (physicalSceneType)
	{
	case ETESPhysicalGameSceneType::Lobby:
		{
			int32 curStageId = _conversationInitData._stageId;
			const FStageRow& stageTable = SharedTDBManager->GetStageRow(curStageId);
			if (true == stageTable.IsEmpty())
			{
				SharedUIManager->ShowAllUI();

				Clear();

				SharedMainStoryManager->MoveToNextStageByCurStageId(curStageId);

				return;
			}

			//. 전투에서 승리하였으므로, 다음 스테이지로 세팅해줍니다.
			SharedUIManager->ShowAllUI();

			if ((TESConstantValue::MainStoryAct1Chapter1Stage1 == stageTable.Id) && (false == SharedTriggerEventManager->GetReplayStory()))
			{
				FTESSceneTransitionData sceneTransitionData = {};
				sceneTransitionData._physicalSceneType = ETESPhysicalGameSceneType::LoadingLobby;
				sceneTransitionData._logicalSceneType = ETESLogicalGameSceneType::LoadingLobby;
				sceneTransitionData._fadeAtrribute._fadeType = ETESSceneFadeType::ConversationSceneChange;
				sceneTransitionData._fadeAtrribute._fadeColor = FColor::Black;

				if (true == _conversationDataMapById.Contains(_currentConversationId))
				{
					sceneTransitionData._fadeAtrribute._widgetFadeInAnimName = _conversationDataMapById[_currentConversationId]._sceneTransitionOut._fadeIn;
					sceneTransitionData._fadeAtrribute._widgetFadeOutAnimName = _conversationDataMapById[_currentConversationId]._sceneTransitionOut._fadeOut;
				}

				Clear();

				SharedSceneManager->ChangeGameScene(sceneTransitionData);
				return;
			}
			else
			{
				Clear();

				SharedMainStoryManager->MoveToNextStageByCurStageId(curStageId);
			}
		}
		break;

	case ETESPhysicalGameSceneType::Battle:
		{
			//. 전투 진입을 진행합니다.
			SharedUniqueAbyssManager->ClearData();
			SharedPartySettingManager->ClearScene();
			SharedStellaStrikeManager->SetStellaStrikeDatas();

			SharedStageManager->Reset();
			SharedStageManager->SetUseGameFlow(true);
			SharedStageManager->SetStage(_conversationInitData._stageId);

			FTESSceneTransitionData sceneTransitionData = {};
			sceneTransitionData._physicalSceneType = ETESPhysicalGameSceneType::LoadingBattle;
			sceneTransitionData._logicalSceneType = ETESLogicalGameSceneType::LoadingBattle;
            sceneTransitionData._fadeAtrribute._fadeType = ETESSceneFadeType::ConversationSceneChange;
            sceneTransitionData._fadeAtrribute._fadeColor = FColor::Black;

			if (true == _conversationDataMapById.Contains(_currentConversationId))
			{
				sceneTransitionData._fadeAtrribute._widgetFadeInAnimName = _conversationDataMapById[_currentConversationId]._sceneTransitionOut._fadeIn;
				sceneTransitionData._fadeAtrribute._widgetFadeOutAnimName = _conversationDataMapById[_currentConversationId]._sceneTransitionOut._fadeOut;

				//. 1-1 스테이지 전투 직전에 들어가는 회화라면 전투포기가 안되게 바꿔준다.
				if (ETypeConversationType::MainStoryDialogueAct1Chapter1_1_front == _conversationDataMapById[_currentConversationId]._currentCategory)
				{
					sceneTransitionData._sceneArguments._booleanValues.Add(TESConstantValue::Battle_GSKey_WhetherHideToGiveUp, true);
				}
				else if (ETypeConversationType::MainStoryDialogueAct1Chapter1_2_front == _conversationDataMapById[_currentConversationId]._currentCategory)
				{
					if (ETESNewbieForcedTutorialManagerType::None != SharedNewbieForcedTutorialManager->CheckCurrentPlayingTutorialType())
					{
						sceneTransitionData._sceneArguments._booleanValues.Add(TESConstantValue::Battle_GSKey_WhetherHideToGiveUp, true);
					}
				}
			}

			SharedSceneManager->ChangeGameScene(sceneTransitionData);

			Clear();
		}
		break;

	default:
		{
			Clear();
		}
		break;
	}
}

void UTESConversationManager::SetCurrentConversationId(int32 conversationId)
{
	_currentConversationId = conversationId;
}

void UTESConversationManager::SetConversationCameraToEnd()
{
	//. 회화 카메라를 즉시 목적지로 이동 시키고 틱을 꺼준다.
	TESCHECK(nullptr != _conversationCamera);
	_conversationCamera->SetActorLocation(_destinationConversationCameraLocation);

	_initCameraMoving = false;
	_initCameraFovChanging = false;

	_conversationCameraInitTime = 0.0f;
	_conversationCameraDestinationTime = 0.0f;
	_conversationCameraFovTime = 0.0f;
	_conversationCameraMovingRealTime = 0.0f;
	_conversationCameraMovingRealTimeForFoV = 0.0f;
}

void UTESConversationManager::SetConversationById(int32 conversationId)
{
	UTESUWConversationMain* conversationWidget = SharedUIManager->FindGenericWidget<UTESUWConversationMain>();
	TESCHECK(nullptr != conversationWidget);
	
	conversationWidget->PlayConversationByConversationId(conversationId);
}

void UTESConversationManager::AddConversationData(const FConversationRow* conversationTable)
{
	TESCHECK(nullptr != conversationTable);

    //. 실제 위젯에서 사용할 데이터들을 넣어준다.
    FTESConversationData conversationData = {};

	conversationData._currentCategory = conversationTable->Category;
	conversationData._conversationTableId = conversationTable->Id;
    conversationData._infoStringId = conversationTable->InfoStringId;
    conversationData._backgroundResourceName = conversationTable->BackgroundResourceName;
	conversationData._backgroundLocation = conversationTable->BackgroundLocation;
	conversationData._backgroundRotation = conversationTable->BackgroundRotation;
	conversationData._2dBackgroundEffectColor = conversationTable->Image2DEffect;
	conversationData._nextConversationId = conversationTable->NextConversationId;
    int32 charSlotIndex = 1;	//. 대화씬에 등장하는 캐릭터 슬롯 위치 인덱스

    conversationData._character1 = conversationTable->Character1;
	conversationData._characterEffect1 = conversationTable->Character1Effect;
	conversationData._speakingCharacterEffects.Add(charSlotIndex, conversationData._characterEffect1);
    conversationData._speakingCharacters.Add(charSlotIndex, conversationData._character1);

	for (int32 index = 0; index < conversationTable->Character1Morpher.Num(); ++index)
	{
		const FConversationFacialRow& facialRow = SharedTDBManager->GetConversationFacialRow(conversationTable->Character1Morpher[index]);
		FTESCharacterMorpher characterMorpher;
		characterMorpher._morphName = facialRow.Morpher;
		characterMorpher._curveName = facialRow.Morpher_Curve;
		characterMorpher._facialType = static_cast<int32>(facialRow.FaceType);

		conversationData._characterMorpher1.Add(characterMorpher);
	}

	conversationData._speakingCharactersMorphers.Add(charSlotIndex, conversationData._characterMorpher1);
    ++charSlotIndex;

    conversationData._character2 = conversationTable->Character2;
	conversationData._characterEffect2 = conversationTable->Character2Effect;
	conversationData._speakingCharacterEffects.Add(charSlotIndex, conversationData._characterEffect2);
    conversationData._speakingCharacters.Add(charSlotIndex, conversationData._character2);

	for (int32 index = 0; index < conversationTable->Character2Morpher.Num(); ++index)
	{
		const FConversationFacialRow& facialRow = SharedTDBManager->GetConversationFacialRow(conversationTable->Character2Morpher[index]);
		FTESCharacterMorpher characterMorpher;
		characterMorpher._morphName = facialRow.Morpher;
		characterMorpher._curveName = facialRow.Morpher_Curve;
		characterMorpher._facialType = static_cast<int32>(facialRow.FaceType);

		conversationData._characterMorpher2.Add(characterMorpher);
	}

	conversationData._speakingCharactersMorphers.Add(charSlotIndex, conversationData._characterMorpher2);
    ++charSlotIndex;

    conversationData._character3 = conversationTable->Character3;
	conversationData._characterEffect3 = conversationTable->Character3Effect;
	conversationData._speakingCharacterEffects.Add(charSlotIndex, conversationData._characterEffect3);
    conversationData._speakingCharacters.Add(charSlotIndex, conversationData._character3);

	for (int32 index = 0; index < conversationTable->Character3Morpher.Num(); ++index)
	{
		const FConversationFacialRow& facialRow = SharedTDBManager->GetConversationFacialRow(conversationTable->Character3Morpher[index]);
		FTESCharacterMorpher characterMorpher;
		characterMorpher._morphName = facialRow.Morpher;
		characterMorpher._curveName = facialRow.Morpher_Curve;
		characterMorpher._facialType = static_cast<int32>(facialRow.FaceType);

		conversationData._characterMorpher3.Add(characterMorpher);
	}

	conversationData._speakingCharactersMorphers.Add(charSlotIndex, conversationData._characterMorpher3);
    ++charSlotIndex;

    conversationData._character4 = conversationTable->Character4;
	conversationData._characterEffect4 = conversationTable->Character4Effect;
	conversationData._speakingCharacterEffects.Add(charSlotIndex, conversationData._characterEffect4);
    conversationData._speakingCharacters.Add(charSlotIndex, conversationData._character4);

	for (int32 index = 0; index < conversationTable->Character4Morpher.Num(); ++index)
	{
		const FConversationFacialRow& facialRow = SharedTDBManager->GetConversationFacialRow(conversationTable->Character4Morpher[index]);
		FTESCharacterMorpher characterMorpher;
		characterMorpher._morphName = facialRow.Morpher;
		characterMorpher._curveName = facialRow.Morpher_Curve;
		characterMorpher._facialType = static_cast<int32>(facialRow.FaceType);

		conversationData._characterMorpher4.Add(characterMorpher);
	}

	conversationData._speakingCharactersMorphers.Add(charSlotIndex, conversationData._characterMorpher4);
    ++charSlotIndex;

    conversationData._character5 = conversationTable->Character5;
	conversationData._characterEffect5 = conversationTable->Character5Effect;
	conversationData._speakingCharacterEffects.Add(charSlotIndex, conversationData._characterEffect5);
    conversationData._speakingCharacters.Add(charSlotIndex, conversationData._character5);

	for (int32 index = 0; index < conversationTable->Character5Morpher.Num(); ++index)
	{
		const FConversationFacialRow& facialRow = SharedTDBManager->GetConversationFacialRow(conversationTable->Character5Morpher[index]);
		FTESCharacterMorpher characterMorpher;
		characterMorpher._morphName = facialRow.Morpher;
		characterMorpher._curveName = facialRow.Morpher_Curve;
		characterMorpher._facialType = static_cast<int32>(facialRow.FaceType);

		conversationData._characterMorpher5.Add(characterMorpher);
	}

	conversationData._speakingCharactersMorphers.Add(charSlotIndex, conversationData._characterMorpher5);
    ++charSlotIndex;

    conversationData._character6 = conversationTable->Character6;
	conversationData._characterEffect6 = conversationTable->Character6Effect;
	conversationData._speakingCharacterEffects.Add(charSlotIndex, conversationData._characterEffect6);
    conversationData._speakingCharacters.Add(charSlotIndex, conversationData._character6);

	for (int32 index = 0; index < conversationTable->Character6Morpher.Num(); ++index)
	{
		const FConversationFacialRow& facialRow = SharedTDBManager->GetConversationFacialRow(conversationTable->Character6Morpher[index]);
		FTESCharacterMorpher characterMorpher;
		characterMorpher._morphName = facialRow.Morpher;	
		characterMorpher._curveName = facialRow.Morpher_Curve;
		characterMorpher._facialType = static_cast<int32>(facialRow.FaceType);

		conversationData._characterMorpher6.Add(characterMorpher);
	}

	conversationData._speakingCharactersMorphers.Add(charSlotIndex, conversationData._characterMorpher6);
    ++charSlotIndex;

    conversationData._character7 = conversationTable->Character7;
	conversationData._characterEffect7 = conversationTable->Character7Effect;
	conversationData._speakingCharacterEffects.Add(charSlotIndex, conversationData._characterEffect7);
    conversationData._speakingCharacters.Add(charSlotIndex, conversationData._character7);

	for (int32 index = 0; index < conversationTable->Character7Morpher.Num(); ++index)
	{
		const FConversationFacialRow& facialRow = SharedTDBManager->GetConversationFacialRow(conversationTable->Character7Morpher[index]);
		FTESCharacterMorpher characterMorpher;
		characterMorpher._morphName = facialRow.Morpher;
		characterMorpher._curveName = facialRow.Morpher_Curve;
		characterMorpher._facialType = static_cast<int32>(facialRow.FaceType);

		conversationData._characterMorpher7.Add(characterMorpher);
	}

	conversationData._dialougeChoiceList.Add(conversationTable->ConversationChoice1);
	conversationData._dialougeChoiceList.Add(conversationTable->ConversationChoice2);
	conversationData._dialougeChoiceList.Add(conversationTable->ConversationChoice3);

	conversationData._speakingCharactersMorphers.Add(charSlotIndex, conversationData._characterMorpher7);

	conversationData._2dCutsceneForMale = conversationTable->Cutscene2D;
	conversationData._2dCutsceneForFeMale = conversationTable->Cutscene2DFemale;
	conversationData._popupImageList.Add(conversationTable->PopupImage1);
	conversationData._popupImageList.Add(conversationTable->PopupImage2);
	conversationData._popupImageList.Add(conversationTable->PopupImage3);
    conversationData._cameraShake = conversationTable->CameraShake;
    conversationData._characterNameId = conversationTable->CharacterNameId;
    conversationData._talkId = conversationTable->TalkId;
    conversationData._talkPoint = conversationTable->TalkPoint;
    conversationData._talkSpeed = conversationTable->TalkSpeed;
    conversationData._talkStyleId = conversationTable->TalkStyleId;
    conversationData._voiceId = conversationTable->VoiceId;
    conversationData._bgmId = conversationTable->BGMId;
	conversationData._ambientSoundId = conversationTable->AmbientSound;
    conversationData._soundEffectId = conversationTable->SoundEffectId;
	conversationData._vfxId = conversationTable->VFXId;
	conversationData._psScreenUIFXPath = conversationTable->PSScreenEffect;
	conversationData._skipSummaryId = conversationTable->ConversationSummaryId;
	conversationData._vfxLocation = conversationTable->VFXLocation;
	conversationData._initCameraLocation = conversationTable->CameraStartLocation;
	conversationData._endCameraLocation = conversationTable->CameraEndLocation;
	conversationData._cameraFov = conversationTable->CameraFOV._fov;
	conversationData._cameraFovTime = conversationTable->CameraFOV._time;
	conversationData._sceneId = conversationTable->SceneConnect;
	conversationData._sceneTransitionIn = conversationTable->SceneTransitionInName;
	conversationData._sceneTransitionOut = conversationTable->SceneTransitionOutName;
	conversationData._bgmFadeOutTime = conversationTable->BGMFadeOutTime;
	conversationData._conversationIntervalFadeIn = conversationTable->ConversationIntervalFadeIn;
	conversationData._conversationIntervalFadeOut = conversationTable->ConversationIntervalFadeOut;
	conversationData._backgroundBlurValue = conversationTable->BackgroundBlur;

	_conversationDataMapById.Add(conversationData._conversationTableId, conversationData);
}

void UTESConversationManager::RemoveConversationDataAll()
{
	_conversationDataMapById.Empty();
}

const int32 UTESConversationManager::GetConversationDataCount()
{
    return _conversationDataMapById.Num();
}

void UTESConversationManager::SetAttachToActor(const int32 slotIndex)
{
	//. 슬롯 엑터에 붙인다.
	FString charSlotName = FString::Printf(TEXT("CharSlot_%d"), slotIndex);
	AActor* charSlotActor = TESUtils::FindActorByName<AActor>(GetWorld(), charSlotName);
	TESCHECK(nullptr != charSlotActor);

	if (true == _conversationCharacterMap.Contains(slotIndex))
	{
		if (nullptr != _conversationCharacterMap[slotIndex])
		{
			int32 characterId = _conversationCharacterMap[slotIndex]->GetCharacterId();
			_conversationCharacterMap[slotIndex]->AttachToActor(charSlotActor, FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
}

void UTESConversationManager::SetDetachToActor(const int32 slotIndex)
{
	//. 슬롯 엑터에 붙인다.
	FString charSlotName = FString::Printf(TEXT("CharSlot_%d"), slotIndex);
	AActor* charSlotActor = TESUtils::FindActorByName<AActor>(GetWorld(), charSlotName);
	TESCHECK(nullptr != charSlotActor);

	if (true == _conversationCharacterMap.Contains(slotIndex))
	{
		if (nullptr != _conversationCharacterMap[slotIndex])
		{
			int32 characterId = _conversationCharacterMap[slotIndex]->GetCharacterId();
			_conversationCharacterMap[slotIndex]->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		}
	}
}

TArray_Conversation UTESConversationManager::GetConversationTableRowDataById(const int32 conversationId)
{
	TArray_Conversation resultConversationTableRows;

	for (int32 index = 0; index < _conversationTableRows.Num(); ++index)
	{
		if (conversationId == _conversationTableRows[index]->Id)
		{
			resultConversationTableRows.Add(_conversationTableRows[index]);
		}
	}

	return resultConversationTableRows;
}

TArray_Conversation UTESConversationManager::GetConversationTableRowDataByCategory(const ETypeConversationType category)
{
	TArray_Conversation resultConversationTableRows = {};

	for (int32 index = 0; index < _conversationTableRows.Num(); ++index)
	{
		if (category == _conversationTableRows[index]->Category)
		{
			resultConversationTableRows.Add(_conversationTableRows[index]);
		}
	}

	return resultConversationTableRows;
}

TMap<int32, ATESCharacterConversation*> UTESConversationManager::GetCurrentSpeakingCharacters()
{
	return _conversationCharacterMap;
}

ACameraActor* UTESConversationManager::GetCameraActor(FString cameraActorName)
{
	ACameraActor* cameraActor = TESUtils::FindActorByName<ACameraActor>(GetWorld(), *cameraActorName);
	TESCHECK(nullptr != cameraActor, nullptr);

	return cameraActor;
}

ACameraActor* UTESConversationManager::GetCameraActor()
{
	return _conversationCamera;
}

const FTESConversationData UTESConversationManager::GetFirstConversationData()
{
	FTESConversationData conversationData = {};

	//. 첫번째 대사를 반환시킨다.
	if (0 < GetConversationDataCount())
	{
		return _conversationDataMapById.begin()->Value;
	}

	return conversationData;
}

const FTESConversationData UTESConversationManager::GetConversationDataById(int32 conversationId)
{
	for (TPair<int32, FTESConversationData> conversationData : _conversationDataMapById)
	{
		if (conversationData.Key == conversationId)
		{
			return conversationData.Value;
		}
	}

	return {};
}

const TMap<int32, FTESConversationData>& UTESConversationManager::GetConversationDataMap() const
{
	return _conversationDataMapById;
}

const FTESConversationInitData UTESConversationManager::GetConversationInitData() const
{
	return _conversationInitData;
}

const bool UTESConversationManager::CheckIfPossibleToPlay()
{
	return 0 < _conversationDataMapById.Num();
}

void UTESConversationManager::SetIsEnterDevelopHelper(bool isOn)
{
	_isEnterDevelopHelper = isOn;
}

void UTESConversationManager::EndDevelopHelper()
{
	if (true == _isEnterDevelopHelper)
	{
		SetIsEnterDevelopHelper(false);

		FTESGoToPreviousAttribute gotoPreviousAttribute;
		gotoPreviousAttribute._fadeAtrribute._fadeType = ETESSceneFadeType::WidgetAnimation;
		gotoPreviousAttribute._fadeAtrribute._fadeColor = FColor::Black;
		gotoPreviousAttribute._fadeAtrribute._fadeOutImmediately = true;
		gotoPreviousAttribute._isShowAllUIWhenEntering = true;
		SharedSceneManager->GoToPrevious(gotoPreviousAttribute);
	}
}
