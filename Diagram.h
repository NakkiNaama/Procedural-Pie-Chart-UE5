// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "DiagramText.h"
#include "Kismet/KismetMathLibrary.h"
#include "Diagram.generated.h"

UCLASS()
class PIECHART_API ADiagram : public AActor
{
	GENERATED_BODY()
	
	struct FPieSection {
		FPieSection(int pIndex, FVector pLocation) {
			index = pIndex;
			location = pLocation;
		}
		int index;
		FVector location;
		TArray<FVector> vertices;
		TArray<FVector> GetVerticies() {
			return vertices;
		}
		TArray<int> triangles;
		TArray<FVector2D> UV0;
		FVector center;
	};

public:
	ADiagram();
	virtual void Tick(float DeltaTime) override;

	void SpawnDiagram();

	UFUNCTION(BlueprintCallable)
		void RefreshWithFloats(TArray<FString> names, TArray<float> values) {
		_valueNames = names;
		_values = values;
		RefreshPie();
	}
	UFUNCTION(BlueprintCallable)
		void RefreshWithIntegers(TArray<FString> names, TArray<int> values) {
		_valueNames = names;
		_values.Empty();
		for (auto& i : values) {
			if (i != 0) {
				_values.Add(i);
			}
		}
		RefreshPie();
	}

	UFUNCTION(BlueprintCallable)
	void AddPieMaterial(UMaterialInstanceDynamic* DynamicMaterial) {
		_pieMaterials.Add(DynamicMaterial);
	}

public:	

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* _root;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UProceduralMeshComponent* _proMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UProceduralMeshComponent* _lines;

	UPROPERTY(EditAnywhere, Category = "Pie Settings")
	float _scale = 1;
	UPROPERTY(EditAnywhere, Category = "Pie Settings")
	float _depth = 0.25;
	UPROPERTY(EditAnywhere, Category = "Pie Settings")
	float _textScale = 1;
	UPROPERTY(EditAnywhere, Category = "Pie Settings")
	float _outlineThickness = 1;
	UPROPERTY(EditAnywhere, Category = "Pie Settings")
	float _minAngleForText = 40;

	UPROPERTY(EditAnywhere, Category = "Values")
	TArray<FString> _valueNames;
	UPROPERTY(EditAnywhere, Category = "Values")
	TArray<float> _values;

	UPROPERTY(EditAnywhere)
	UMaterialInstance* _outlineMaterial;
	UPROPERTY(EditAnywhere)
	TSubclassOf<ADiagramText> _textType;




protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void UpdateText(FVector textPoint);
	void SpawnOutlines(TArray<float> angles);
	void RefreshPie();
	void SpawnSides();


protected:
	float AddPieSection(FPieSection& section, const float angle, const float endAngle);
	void AddPieTriangle(FPieSection& section, FRotator rot, float nextAngle);
	TArray<float> ConvertValuesToAngles(const TArray<float>& values);
	TArray<FPieSection> _pie;
	TArray<ADiagramText*> _texts;
	int _lineCounter = 0;
	int _valueIndex = 0;
	FPieSection _textLines = FPieSection(-1, FVector(100 + _outlineThickness, 0, 1));
	TArray<UMaterialInstanceDynamic*> _pieMaterials;

	float _radius = 1;
	int _sectionCount = 0;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* _back;

	
};
