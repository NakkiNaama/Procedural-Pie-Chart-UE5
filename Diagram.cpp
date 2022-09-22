// Fill out your copyright notice in the Description page of Project Settings.


#include "Diagram.h"

// Sets default values
ADiagram::ADiagram()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_root = CreateDefaultSubobject<USceneComponent>("root");
	SetRootComponent(_root);

	_proMesh = CreateDefaultSubobject<UProceduralMeshComponent>("proMesh");
	_proMesh->SetupAttachment(_root);

	_lines = CreateDefaultSubobject<UProceduralMeshComponent>("lines");
	_lines->SetupAttachment(_root);

	_back = CreateDefaultSubobject<UStaticMeshComponent>("back");
	_back->SetupAttachment(_root);

	_proMesh->bUseAsyncCooking = true;
	
}

// Called when the game starts or when spawned
void ADiagram::BeginPlay()
{
	Super::BeginPlay();
	SpawnDiagram();

	_back->SetRelativeScale3D(FVector(_scale, _scale, _depth));
	if (_outlineMaterial) {
		_back->SetMaterial(0, _outlineMaterial);
	}
}

// Called every frame
void ADiagram::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RefreshPie();
}

void ADiagram::RefreshPie() {
	_proMesh->ClearAllMeshSections();
	_pie.Empty();
	for (auto i : _texts) {
		i->Destroy();
	}
	_texts.Empty();
	_textLines = FPieSection(-1, FVector(_radius + _outlineThickness, 0, 1));
	_lineCounter = 0;
	_valueIndex = 0;
	SpawnDiagram();
}

void ADiagram::SpawnDiagram() {
	_radius = _scale * 100;
	TArray<float> sizes;
	if (_values.Num() > 0) {
		sizes = ConvertValuesToAngles(_values);
		float lastAngle = 0;
		for (int i = 0; i < _values.Num(); i++) {
			_pie.Add(FPieSection(i, FVector(_radius, 0, 0)));
			lastAngle = AddPieSection(_pie.Last(), lastAngle, sizes[i]);
		}
	}
	else {
		_pie.Add(FPieSection(0, FVector(_radius, 0, 0)));
		for (int d = 0; d < 36; d++) {
			AddPieTriangle(_pie.Last(), FRotator(0, 10.f * d, 0), 10.f);
		}
	}

	for (auto& p : _pie) {
		_proMesh->CreateMeshSection_LinearColor
		(
			p.index, // section index
			p.vertices, // Vertices
			p.triangles, // Triangles
			TArray<FVector>(),
			p.UV0, // UV
			TArray<FLinearColor>(),
			TArray<FProcMeshTangent>(),
			true
		);
	}
	for (int j = 0; j < _sectionCount; j++) {
		if (_pieMaterials.Num() > j) {
			_proMesh->SetMaterial(j, _pieMaterials[j]);
		}
		else break;
	}
	if (_pie.Num() > 1) {
		SpawnOutlines(sizes);
	}
}

float ADiagram::AddPieSection(FPieSection& section, const float startAngle, const float size) {
	FRotator rot = FRotator(0, startAngle, 0);
	float sizeLeft = size;
	while (true) {
		if (sizeLeft > 5) {
			sizeLeft -= 5;
			AddPieTriangle(section, rot, 5);
			rot.Yaw += 5;
		}
		else {
			AddPieTriangle(section, rot, sizeLeft);
			break;
		}
	}
	section.center = FRotator(0, startAngle + size / 2, 0).RotateVector(FVector(_radius, 0, 0));

	FVector textPoint;
	if (size > _minAngleForText) {
		textPoint = FRotator(0, startAngle + size / 2, 0).RotateVector(FVector(_radius * 0.65, 0, 0));
	}
	else {
		textPoint = FRotator(0, startAngle + size / 2, 0).RotateVector(FVector(_radius * 1.2, 0, 0));
	}
	UpdateText(textPoint);
	_sectionCount++;
	return startAngle + size;
}

void ADiagram::AddPieTriangle(FPieSection& section, FRotator rot, float nextAngle) {
	FVector point1 = rot.RotateVector(section.location);
	rot.Yaw += nextAngle;
	FVector point2 = rot.RotateVector(section.location);

	section.vertices.Add(point1);
	section.vertices.Add(point2);
	section.vertices.Add(FVector(0, 0, section.location.Z));

	int triCount = section.triangles.Num();
	section.triangles.Add(triCount + 2); section.triangles.Add(triCount + 1); section.triangles.Add(triCount);
}

TArray<float> ADiagram::ConvertValuesToAngles(const TArray<float>& values) {
	TArray<float> angles;
	float total = 0;
	for (auto& value : values) {
		total += value;
	}
	for (auto& value : values) {
		float percent = value / total;
		angles.Add(360 * percent);
	}
	return angles;
}

void ADiagram::SpawnOutlines(TArray<float> angles) {
	FPieSection outline(_pie.Num(), FVector(_radius + _outlineThickness, 0, 1));
	int currentVertex = 0;
	float currentAngle = 0;
	FRotator rot = FRotator(0, 0, 0);
	for (int i = 0; i < _pie.Num(); i++) {		
		rot = FRotator(0, currentAngle, 0);
		currentVertex = i * 4;

		outline.vertices.Add(rot.RotateVector(FVector(0, -_outlineThickness, 1)));
		outline.vertices.Add(rot.RotateVector(FVector(0, _outlineThickness, 1)));
		outline.vertices.Add(rot.RotateVector(FVector(_radius, -_outlineThickness, 1)));
		outline.vertices.Add(rot.RotateVector(FVector(_radius, _outlineThickness, 1)));


		outline.triangles.Add(0 + currentVertex);
		outline.triangles.Add(1 + currentVertex);
		outline.triangles.Add(3 + currentVertex);

		outline.triangles.Add(3 + currentVertex);
		outline.triangles.Add(2 + currentVertex);
		outline.triangles.Add(0 + currentVertex);

		currentAngle += angles[i];
	}

	TArray<FPieSection*> sections;
	sections.Add(&outline);
	_textLines.index = outline.index + 1;
	_pie.Add(_textLines);
	sections.Add(&_textLines);

	int i = 0;
	for (auto &section : sections) {
		_lines->CreateMeshSection_LinearColor
		(
			i, // section index
			section->vertices, // Vertices
			section->triangles, // Triangles
			TArray<FVector>(),
			TArray<FVector2D>(), // UV
			TArray<FLinearColor>(),
			TArray<FProcMeshTangent>(),
			true
		);
		_lines->SetMaterial(i, _outlineMaterial);
		i++;
	}
}

void ADiagram::SpawnSides() {
	/*
	FRotator rot = FRotator(0, 0, 0);
	FPieSection side(_pie.Num() + 1, FVector(100 + _outlineThickness * 2, 0, -1));
	int triCount = 0;
	for (int d = 0; d < 36; d++) {
		_sides.vertices.Add(rot.RotateVector(FVector(-_outlineThickness, 100, 0)));
		_sides.vertices.Add(rot.RotateVector(FVector(_outlineThickness, 100, 0)));
		_sides.vertices.Add(rot.RotateVector(FVector(-_outlineThickness, 100, -100)));
		_sides.vertices.Add(rot.RotateVector(FVector(_outlineThickness, 100, -100)));

		_sides.triangles.Add(0 + triCount);
		_sides.triangles.Add(1 + triCount);
		_sides.triangles.Add(3 + triCount);

		_sides.triangles.Add(3 + triCount);
		_sides.triangles.Add(2 + triCount);
		_sides.triangles.Add(0 + triCount);
		triCount += 4;
		rot.Pitch += 10;
	}
	*/
}

void ADiagram::UpdateText(FVector textPoint) {
	if (_textType) {
		FTransform transform;
		transform.SetLocation(this->GetActorRotation().RotateVector(
			textPoint) + GetActorLocation());
		
		FVector start = this->GetActorLocation();
		start.Z = 0;
		textPoint.Z = 0;

		FRotator rot = UKismetMathLibrary::FindLookAtRotation(start, textPoint + start);

		transform.SetRotation(this->GetActorRotation().Quaternion()); // rot.Quaternion()

		ADiagramText* spawned = GetWorld()->SpawnActor<ADiagramText>(_textType, transform);
		spawned->BP_SetScale(_scale * _textScale);
		if (_valueNames.Num() > _valueIndex && _values.Num() > _valueIndex) {
			spawned->SetText(_valueNames[_valueIndex], FString::SanitizeFloat(_values[_valueIndex]));			
		}
		else {
			spawned->SetText("Nameless", "0");
		}
		_texts.Add(spawned);
		_valueIndex++;

		if (textPoint.Size() > _radius) {
			_textLines.vertices.Add(rot.RotateVector(FVector(_radius, -_outlineThickness, 1)));
			_textLines.vertices.Add(rot.RotateVector(FVector(_radius, _outlineThickness, 1)));
			_textLines.vertices.Add(textPoint + rot.RotateVector(FVector(0, -_outlineThickness, 1)));
			_textLines.vertices.Add(textPoint + rot.RotateVector(FVector(0, _outlineThickness, 1)));

			_textLines.triangles.Add(0 + _lineCounter);
			_textLines.triangles.Add(1 + _lineCounter);
			_textLines.triangles.Add(3 + _lineCounter);

			_textLines.triangles.Add(3 + _lineCounter);
			_textLines.triangles.Add(2 + _lineCounter);
			_textLines.triangles.Add(0 + _lineCounter);
			_lineCounter += 4;
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Diagram type not defined!"));
	}
}






