// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TES.h"
#include "Structs/Manager/TESManagerBase.h"
#include "TESCharacterInventoryManager.generated.h"

/**
 * 
 */
UCLASS()
class TES_API UTESCharacterInventoryManager : public UTESManagerBase
{
	GENERATED_BODY()

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Override Functions ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────	
public:
	virtual void Start() override;
	virtual void Reset() override;
	virtual void Shutdown() override;

//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. [ Memeber Variables ]
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
//. public:
//. 	void RefreshCharacterInventory();
//. ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
};
