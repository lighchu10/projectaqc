// Fill out your copyright notice in the Description page of Project Settings.
#include "Contents/CharacterInventory/Manager/TESCharacterInventoryManager.h"
#include "Contents/CharacterInventory/UI/TESUWCharacterInventoryMain.h"
#include "Contents/Conversation/Character/TESCharacterConversation.h"
#include "Contents/Common/Manager/TESPopupManager.h"
#include "TESGameInstance.h"
#include "TESUtils.h"

void UTESCharacterInventoryManager::Start()
{
	Super::Start();
}

void UTESCharacterInventoryManager::Reset()
{
	Super::Reset();

}

void UTESCharacterInventoryManager::Shutdown()
{
	Super::Shutdown();
}

//void UTESCharacterInventoryManager::RefreshCharacterInventory()
//{
//	UTESUWCharacterInventoryMain* characterInventoryWidget = SharedPopupManager->FindPopupWidget<UTESUWCharacterInventoryMain>();
//
//	if (nullptr != characterInventoryWidget)
//	{
//		characterInventoryWidget->RereshCharacterData();
//	}
//}
