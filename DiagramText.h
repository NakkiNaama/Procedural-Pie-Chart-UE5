// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DiagramText.generated.h"

UCLASS()
class PIECHART_API ADiagramText : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADiagramText();

	void SetText(FString name, FString value);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_SetScale(float scale);

	UFUNCTION(BlueprintPure)
	FString GetValueName() {
		return _valueName;
	}
	UFUNCTION(BlueprintPure)
	FString GetValue() {
		return _value;
	}
	UFUNCTION(BlueprintImplementableEvent)
	void BP_UpdateText();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	FString _valueName = "";
	FString _value = "";

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
