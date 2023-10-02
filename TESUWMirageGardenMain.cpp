// Fill out your copyright notice in the Description page of Project Settings.

#include "Contents/MirageGarden/UI/TESUWMirageGardenMain.h"
#include "Contents/MirageGarden/UI/TESUWMirageGardenDifficultyInfo.h"
#include "Contents/Common/Manager/TESSceneManager.h"
#include "Contents/Common/Manager/TESUIManager.h"
#include "Contents/Common/UI/TESUWCommonHeader.h"
#include "TESUtilString.h"

void UTESUWMirageGardenMain::InitWidget()
{
	Super::InitWidget();

	InitCommonHeader();

	InitMirageGardenDifficultyInfoWidget();
}

void UTESUWMirageGardenMain::ClearWidget()
{
	Super::ClearWidget();

	ClearCommonHeader();

	if (true == ::IsValid(_wbp_mirageGardenDifficultyInfo))
	{
		_wbp_mirageGardenDifficultyInfo->ClearWidget();
		SharedUIManager->DestroyGenericWidget(_wbp_mirageGardenDifficultyInfo);
		_wbp_mirageGardenDifficultyInfo = nullptr;
	}
}

void UTESUWMirageGardenMain::OpenWidget()
{
	Super::OpenWidget();

	if (true == ::IsValid(_wbp_commonHeader))
	{
		_wbp_commonHeader->InitWidget();
		_wbp_commonHeader->OpenWidget();
	}
}

void UTESUWMirageGardenMain::CloseWidget()
{
	Super::CloseWidget();

	if (true == ::IsValid(_wbp_commonHeader))
	{
		_wbp_commonHeader->CloseWidget();
	}
}

void UTESUWMirageGardenMain::OpenDelegate()
{
	Super::OpenDelegate();
}

void UTESUWMirageGardenMain::CloseDelegate()
{
	Super::CloseDelegate();
}

void UTESUWMirageGardenMain::InitCommonHeader()
{
	_wbp_commonHeader = Cast<UTESUWCommonHeader>(GetWidgetFromName(TEXT("WBP_CommonHeader")));
	if (true == ::IsValid(_wbp_commonHeader))
	{
		_wbp_commonHeader->InitWidget();
		_wbp_commonHeader->CloseWidget();
	}
}

void UTESUWMirageGardenMain::InitMirageGardenDifficultyInfoWidget()
{
	_wbp_mirageGardenDifficultyInfo = SharedUIManager->CreateGenericWidget<UTESUWMirageGardenDifficultyInfo>(TEXT("WBP_MirageGardenDifficulty"), TEXT("MirageGarden/WBP"));

	if (false == ::IsValid(_wbp_mirageGardenDifficultyInfo))
	{
		TESLOG(Error, TEXT("Invalid Mirage Garden Info Widget."));

		return;
	}

	_wbp_mirageGardenDifficultyInfo->InitWidget();
	_wbp_mirageGardenDifficultyInfo->AddToViewport();
}

void UTESUWMirageGardenMain::ClearCommonHeader()
{
	if (true == ::IsValid(_wbp_commonHeader))
	{
		_wbp_commonHeader->CloseWidget();
		_wbp_commonHeader->ClearWidget();
		_wbp_commonHeader = nullptr;
	}
}

void UTESUWMirageGardenMain::SetWidget()
{
	SetCommonHeader();

	SetMirageGardenDifficultyInfoWidget();
}

void UTESUWMirageGardenMain::SetCommonHeader()
{
	if (true == ::IsValid(_wbp_commonHeader))
	{
		_wbp_commonHeader->OpenWidget();
		_wbp_commonHeader->SetWidget();
	}
}

void UTESUWMirageGardenMain::SetMirageGardenDifficultyInfoWidget()
{
	if (true == ::IsValid(_wbp_mirageGardenDifficultyInfo))
	{
		_wbp_mirageGardenDifficultyInfo->OpenWidget();
		_wbp_mirageGardenDifficultyInfo->SetWidget();
	}
}

void UTESUWMirageGardenMain::OnEvent_Back()
{
	SharedSceneManager->OnEvent_ContentsBack();
}

void UTESUWMirageGardenMain::OnEvent_Home()
{
	SharedSceneManager->OnEvent_ContentsHome();
}