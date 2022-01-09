// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Editor/Blutility/Classes/EditorUtilityWidget.h"
#include "MapToolWidget.generated.h"

/**
 * 
 */
UCLASS()
class NEWMAPTOOL_API UMapToolWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	// �� ũ�� (���簢��)
	int mapSize = 18900;
	// �� �������� �� ũ�⸸ŭ �о ó��.
	int mapCheckGap = 10;
	
	UPROPERTY(meta=(BindWidget))
	class UEditableText* mapSizeInputText;
	UPROPERTY(meta=(BindWidget))
	class UEditableText* mapGapInputText;
	UPROPERTY(meta=(BindWidget))
	class UButton* imageSaveButton;
	UPROPERTY(meta=(BindWidget))
	class UButton* heightMapSaveButton;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void ImageSaveClick();
	UFUNCTION()
	void HeightMapSaveClick();
	UFUNCTION()
	void ChangeMapSize(const FText& txt);
	UFUNCTION()
	void ChangeMapCheckGap(const FText& txt);

	void SetMapToolOnOff(bool&& isOn);

	virtual void NativeDestruct() override;
};
