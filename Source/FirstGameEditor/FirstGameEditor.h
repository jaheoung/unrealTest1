#pragma once

#include <fstream>
#include <string>

#include "Engine.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "UnrealEd.h"

#include "MapTool.h"
#include "Editor/MainFrame/Public/Interfaces/IMainFrameModule.h"


class FFirstGameEditor : public IModuleInterface
{
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	TSharedPtr<FExtender> ToolbarExtender;
	TSharedPtr<const FExtensionBase> Extension;

	// 맵 크기 (정사각형)
	int mapSize = 18900;
	// 이 간격으로 맵 크기만큼 읽어서 처리.
	int mapCheckGap = 10;

	void MyButtonClick()
	{

		int aa =0;
		// 이 부분이 윈도우를 세팅하는 부분이다.
		TSharedRef<SWindow> MapToolWindow = SNew(SWindow)
			.Title(FText::FromString(TEXT("Map Tool")))
			.ClientSize(FVector2D(800, 400))
			.SupportsMaximize(false)
			.SupportsMinimize(false)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot().HAlign(HAlign_Left).VAlign(VAlign_Top).AutoHeight()
				[
					SNew(STextBlock).Text(FText::FromString(TEXT("[---- 게임 플레이 상태에서 사용해야 합니다 ----]")))
				]
				+ SVerticalBox::Slot().HAlign(HAlign_Left).VAlign(VAlign_Top).AutoHeight().Padding(FMargin(10, 10, 10, 10))
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot().HAlign(HAlign_Left).VAlign(VAlign_Top).AutoWidth().Padding(FMargin(5, 0, 5, 0))
					[
						SNew(STextBlock).Text(FText::FromString(TEXT("맵 크기 (정사각형만) : ")))
					]
					+ SHorizontalBox::Slot().HAlign(HAlign_Left).VAlign(VAlign_Top).AutoWidth().Padding(FMargin(5, 0, 5, 0))
					[
						SNew(SEditableText).Text(FText::AsNumber(mapSize)).OnTextChanged_Raw(this, &FFirstGameEditor::ChangeMapSize)
					]
					+ SHorizontalBox::Slot().HAlign(HAlign_Left).VAlign(VAlign_Top).AutoWidth().Padding(FMargin(10, 0, 0, 5))
					[
						SNew(STextBlock).Text(FText::FromString(TEXT("처리할 간격 (x,y 를 이 간격으로 처리) : ")))
					]
					+ SHorizontalBox::Slot().HAlign(HAlign_Left).VAlign(VAlign_Top).AutoWidth().Padding(FMargin(5, 0, 5, 0))
					[
						SNew(SEditableText).Text(FText::AsNumber(mapCheckGap)).OnTextChanged_Raw(this, &FFirstGameEditor::ChangeMapCheckGap)
					]
				]
				+ SVerticalBox::Slot().HAlign(HAlign_Left).VAlign(VAlign_Top).AutoHeight().Padding(FMargin(5, 0, 5, 0))
				[
					SNew(SButton).Text(FText::FromString(TEXT("맵을 이미지로 저장"))).OnClicked_Raw(this, &FFirstGameEditor::MapImageWrite)
				]
				+ SVerticalBox::Slot().HAlign(HAlign_Left).VAlign(VAlign_Top).AutoHeight().Padding(FMargin(5, 0, 5, 0))
				[
					SNew(SButton).Text(FText::FromString(TEXT("높이맵을 바이너리로 저장"))).OnClicked_Raw(this, &FFirstGameEditor::HeightMapWrite)
				]
			];

		IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));

		if (MainFrameModule.GetParentWindow().IsValid())
		{
			FSlateApplication::Get().AddWindowAsNativeChild(MapToolWindow, MainFrameModule.GetParentWindow().ToSharedRef());
		}
		else
		{
			FSlateApplication::Get().AddWindow(MapToolWindow);
		}
	};

	void AddToolbarExtension(FToolBarBuilder& builder)
	{
		FSlateIcon IconBrush = FSlateIcon(FEditorStyle::GetStyleSetName(),
			"LevelEditor.ViewOptions",
			"LevelEditor.ViewOptions.Small");

		builder.AddToolBarButton(FMapTool::Get().myButton, NAME_None, FText::FromString("Map Tool"),
			FText::FromString("first game map tool"), IconBrush, NAME_None);
	};

	FReply ReadMapSizeButton()
	{
		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsOfClass(GEditor->PlayWorld, AActor::StaticClass(),actors);

		for (auto elem : actors)
		{
			if (elem->GetName().Equals("Floor"))
			{
				break;
			}
			UE_LOG(LogTemp, Warning, TEXT("%s 한글"), *(elem->GetName())); 
		}
		
		return FReply::Handled();
	}

	FReply MapImageWrite()
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
		
		return FReply::Handled();
	}
	
	FReply HeightMapWrite()
	{
		if ((mapSize % mapCheckGap) > 0)
		{
			GEditor->OnModalMessageDialog(EAppMsgType::Ok, FText::FromString(TEXT("맵 크기를 간격으로 나눴을때 나머지가 없어야 합니다.")), FText::FromString(TEXT("저장 실패")));
			return FReply::Handled();
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
		
		return FReply::Handled();
	}

	void ChangeMapSize(const FText& txt)
	{
		int&& getInt = FCString::Atoi(*txt.ToString());
		
		mapSize = getInt;
	}
	
	void ChangeMapCheckGap(const FText& txt)
	{
		int&& getInt = FCString::Atoi(*txt.ToString());
		
		mapCheckGap = getInt;
	}

};
