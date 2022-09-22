// Fill out your copyright notice in the Description page of Project Settings.


#include "DiagramText.h"

// Sets default values
ADiagramText::ADiagramText()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADiagramText::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADiagramText::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADiagramText::SetText(FString name, FString value) {
	_valueName = name;
	_value = value;
	BP_UpdateText();
}




