// Fill out your copyright notice in the Description page of Project Settings.


#include "MapToolWidget.h"

#include "Components/Button.h"
#include "Components/EditableText.h"
#include <fstream>
#include <string>
#include <vector>

// #include "../../../../../Source/FirstGame/MyPlayerController.h"
// #include "../../../../../Source/FirstGame/ToolMeshActor.h"

void UMapToolWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bAlwaysReregisterWithWindowsMenu = true;
	
	mapSizeInputText->SetText(FText::AsNumber(mapSize));
	mapGapInputText->SetText(FText::AsNumber(mapCheckGap));

	mapSizeInputText->OnTextChanged.AddDynamic(this, &UMapToolWidget::ChangeMapSize);
	mapGapInputText->OnTextChanged.AddDynamic(this, &UMapToolWidget::ChangeMapCheckGap);
	
	imageSaveButton->OnClicked.AddDynamic(this, &UMapToolWidget::ImageSaveClick);
	heightMapSaveButton->OnClicked.AddDynamic(this, &UMapToolWidget::HeightMapSaveClick);
	pathGridSaveButton->OnClicked.AddDynamic(this, &UMapToolWidget::PathGridSaveClick);

	SetMapToolOnOff(true);
}

void UMapToolWidget::ImageSaveClick()
{
	FHitResult hitResult;
	FVector startPoint = FVector(0, 0, 100000);
	FVector downVec = FVector::DownVector * 150000;

	std::ofstream imgFile;
	FString path = FPaths::ProjectDir();
	path.Append(TEXT("map.pgm"));
	imgFile.open(*path);

	// 기록할 최대 높이값.
	float maxHeight = 5500;
	float rate = 255 / maxHeight;
	int count = mapSize / mapCheckGap;
	
	std::string str;
	str.append("P2 ");
	str.append(std::to_string(count));
	str.append(" ");
	str.append(std::to_string(count));
	str.append(" 255\n");
	imgFile.write(str.data(), str.size());
	str.clear();
	
	for (int x = mapSize - 1; x > -1; x -= mapCheckGap) // top 뷰에서 바라봤을때 x 가 위쪽 y 가 오른쪽으로 전개된다.
		{
		startPoint.X = x;
		for (int y = 0; y < mapSize; y += mapCheckGap)
		{
			startPoint.Y = y;	
			if (GEditor->PlayWorld->LineTraceSingleByChannel(hitResult, startPoint, startPoint + downVec, ECollisionChannel::ECC_Visibility,
				FCollisionQueryParams::DefaultQueryParam, FCollisionResponseParams::DefaultResponseParam))
			{
				str.append(std::to_string((int)(hitResult.Location.Z * rate)));
				str.append(" ");
			}
			else
			{
				str.append("0 ");
			}
		}
		str.append("\n");
		imgFile.write(str.data(), str.size());
		str.clear();
		}

	imgFile.close();

	GEditor->OnModalMessageDialog(EAppMsgType::Ok, FText::FromString(TEXT("저장되었습니다.")), FText::FromString(TEXT("알림")));
}

void UMapToolWidget::HeightMapSaveClick()
{
	if ((mapSize % mapCheckGap) > 0)
	{
		// 마지막 인자 알림 텍스트를 실패 텍스트로 바꾸면 빌드 실패한다 -_-;;
		GEditor->OnModalMessageDialog(EAppMsgType::Ok, FText::FromString(TEXT("맵 크기를 간격으로 나눴을때 나머지가 없어야 합니다.")), FText::FromString(TEXT("알림")));
		
		return;
	}
		
	FHitResult hitResult;
	FVector startPoint = FVector(0, 0, 100000);
	FVector downVec = FVector::DownVector * 150000;
	
	FString hpath = FPaths::ProjectDir();
	hpath.Append(TEXT("heightMap.bin"));
	std::ofstream heightFile(*hpath, std::ios::out | std::ios::binary);

	heightFile.write((char*)&mapSize, sizeof(int));
		
	TArray<float> mdata;
	startPoint.X = startPoint.Y = 0;
	for (int x = 0; x < mapSize; x += mapCheckGap) // top뷰 기준 위쪽 방향.
	{
		startPoint.X = x;
		for (int y = 0; y < mapSize; y += mapCheckGap) // top뷰 기준 오른쪽 방향.
		{
			startPoint.Y = y;	
			if (GEditor->PlayWorld->LineTraceSingleByChannel(hitResult, startPoint, startPoint + downVec, ECollisionChannel::ECC_Visibility,
				FCollisionQueryParams::DefaultQueryParam, FCollisionResponseParams::DefaultResponseParam))
			{
				mdata.Emplace(hitResult.Location.Z);
			}
			else
			{
				mdata.Emplace(0);
			}
		}
	}

	heightFile.write((char*)(mdata.GetData()), sizeof(float) *mdata.Num());

	UE_LOG(LogTemp, Warning, TEXT("data count : %d,   write count : %d"), mdata.Num(), (sizeof(float) * mdata.Num())); 
		
	heightFile.close();

	GEditor->OnModalMessageDialog(EAppMsgType::Ok, FText::FromString(TEXT("저장되었습니다.")), FText::FromString(TEXT("알림")));
}

void UMapToolWidget::PathGridSaveClick()
{
	// AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GEditor->PlayWorld->GetFirstPlayerController());
	//
	// AToolMeshActor* toolMesh = PlayerController->toolMeshActor;
	//
	// if (toolMesh == nullptr)
	// 	return;;
	//
	// FString path = FPaths::ProjectDir();
	// path.Append(TEXT("pathGrid.bin"));
	// std::ofstream pathGridFile(*path, std::ios::out | std::ios::binary);
	//
	// // 인게임에서 index 를 읽어 x,y 변환 후 x ~ x+spacing, y ~ y+spacing 을 한 블럭으로 보면 될듯.
	// pathGridFile.write((char*)&(toolMesh->spacing), sizeof(int32));
	//
	// pathGridFile.write((char*)&(toolMesh->xysByIndex), sizeof(bool) * toolMesh->xysByIndex.Num());
	//
	// pathGridFile.close();
}


void UMapToolWidget::ChangeMapSize(const FText& txt)
{
	int&& getInt = FCString::Atoi(*txt.ToString());
	mapSize = getInt;
}

void UMapToolWidget::ChangeMapCheckGap(const FText& txt)
{
	int&& getInt = FCString::Atoi(*txt.ToString());
	mapCheckGap = getInt;
}

void UMapToolWidget::SetMapToolOnOff(bool&& isOn)
{
	FString path = FPaths::ProjectDir();
	path.Append(TEXT("toolSetting.bin"));
	std::ofstream heightFile(*path, std::ios::out | std::ios::binary);
	
	heightFile.write((char*)&isOn, sizeof(bool)); // 첫번째는 맵툴용으로 사용.

	heightFile.close();


	// std::string changeProp = "bUseMouseForTouch";
	//
	// FString configPath = FPaths::ProjectConfigDir();
	// configPath.Append(TEXT("DefaultInput.ini"));
	// std::ifstream readInputSettingFile(*configPath, std::ios::in);
	//
	// std::vector<std::string> strs;  
	// if (readInputSettingFile.is_open())
	// {
	// 	std::string str;
	// 	while (!readInputSettingFile.eof())
	// 	{
	// 		std::getline(readInputSettingFile, str);
	//
	// 		std::string::size_type pos = str.find(changeProp); 
	// 		if (pos != std::string::npos)
	// 		    str.replace(pos, str.length(), changeProp.append(isOn ? "=False\n" : "=True\n"));
	// 		else
	// 			str.push_back('\n');
	//
	// 		strs.push_back(str);
	// 	}
	// 	readInputSettingFile.close();
	// }
	//
	// std::ofstream writeInputSettingFile(*configPath, std::ios::out);
	//
	// if (writeInputSettingFile.is_open())
	// {
	// 	for (std::vector<std::string>::iterator iter = strs.begin(); iter != strs.end(); ++iter)
	// 	{
	// 		writeInputSettingFile.write((char*)((*iter).c_str()), (*iter).size());
	// 	}
	// 	
	// 	writeInputSettingFile.close();
	// }
}

void UMapToolWidget::NativeDestruct()
{
	SetMapToolOnOff(false);
}











