#include "Grid/Cell.h"
#include "Character/CustomCharacter.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ACell::ACell()
{
	PrimaryActorTick.bCanEverTick = true;


	CellRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Cell Root Component"));
	SetRootComponent(CellRootComponent);

	CellMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cell Mesh Component"));
	CellMeshComponent->SetupAttachment(RootComponent);

	RockMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rock Mesh Component"));
	RockMeshComponent->SetupAttachment(CellMeshComponent);

	SetID(0);
	SetGridPointer(nullptr);
	SetType(CellType::kCellType_Void);

	localGoal = INFINITY;
	globalGoal = INFINITY;
}

// Called when the game starts or when spawned
void ACell::BeginPlay()
{
	Super::BeginPlay();
}

void ACell::Init(int newID, AGrid* newGrid)
{
	SetID(newID);

	if (newGrid != nullptr) {
		SetGridPointer(newGrid);
		SetMeshSize(newGrid->GetCellSize());
	}
}


void ACell::SetID(int newID)
{
	if (Grid == nullptr) {
		id = newID;
		row = 0;
		col = 0;
	}
	else {
		id = newID;
		row = newID / Grid->GridSize.X;
		col = newID % Grid->GridSize.X;
	}
}

void ACell::SetGridPointer(AGrid* newGrid)
{
	Grid = newGrid;
}

void ACell::SetCharacterPointer(ACustomCharacter* newCharacter)
{
	Character = newCharacter;
}


void ACell::SetType(CellType newType)
{
	type = newType;
}

void  ACell::SetMeshSize(float newSize) {
	FVector mesh_size = CellMeshComponent->GetStaticMesh()->GetBounds().GetBox().GetSize();

	//Check Mesh Size Value to avoid dividing by 0.
	if ((int)(mesh_size.X) == 0) mesh_size.X = newSize;
	if ((int)(mesh_size.Y) == 0) mesh_size.Y = newSize;
	if ((int)(mesh_size.Z) == 0) mesh_size.Z = newSize;

	FVector value = FVector(newSize) / mesh_size;
	CellMeshComponent->SetRelativeScale3D(value);
}

void ACell::SetParent(ACell* newParent) {
	parent = newParent;
}

void ACell::SetNeighbours(TArray<ACell*> newNeighbours) {
	neighbours = newNeighbours;
}


bool ACell::IsWalkable()
{
	return (type.GetValue() != kCellType_Void) && (type.GetValue() != kCellType_Wall);
}

int ACell::GetID()
{
	return id;
}

int ACell::GetRow()
{
	return row;
}

int ACell::GetColumn()
{
	return col;
}

CellType ACell::GetType()
{
	return type;
}

AGrid* ACell::GetGridPointer()
{
	return Grid;
}

ACustomCharacter* ACell::GetCharacterPointer()
{
	return Character;
}

ACell* ACell::GetParent()
{
	return parent;
}


TArray<ACell*> ACell::GetNeighbours()
{
	return neighbours;
}

void ACell::SetCellMaterial() {
	if (CellMeshMaterials.Num() > 0) {
		switch (type)
		{
		case kCellType_Void:
			if (CellMeshMaterials[0] != nullptr) CellMeshComponent->SetMaterial(0, CellMeshMaterials[0]);
			break;
		case kCellType_Wall:
			if (CellMeshMaterials[1] != nullptr) CellMeshComponent->SetMaterial(0, CellMeshMaterials[1]);
			RockMeshComponent->SetStaticMesh(RocksMeshes[FMath::RandRange(0, 2)]);
			break;
		case kCellType_Normal:
			if (CellMeshMaterials[2] != nullptr) CellMeshComponent->SetMaterial(0, CellMeshMaterials[2]);
			break;
		case kCellType_Spawn:
			if (CellMeshMaterials[3] != nullptr) CellMeshComponent->SetMaterial(0, CellMeshMaterials[3]);
			break;
		default:
			break;
		}
	}
}

void ACell::SetCellSpawnMaterial(int spawn_team) {
	switch (spawn_team) {
	case kSpawnTeam_Player:
		if (CellSpawnMaterials[0] != nullptr) CellMeshComponent->SetMaterial(0, CellSpawnMaterials[0]);
		break;
	case kSpawnTeam_Team_1:
		if (CellSpawnMaterials[1] != nullptr) CellMeshComponent->SetMaterial(0, CellSpawnMaterials[1]);
		break;
	case kSpawnTeam_Team_2:
		if (CellSpawnMaterials[2] != nullptr) CellMeshComponent->SetMaterial(0, CellSpawnMaterials[2]);
		break;
	case kSpawnTeam_Team_3:
		if (CellSpawnMaterials[3] != nullptr) CellMeshComponent->SetMaterial(0, CellSpawnMaterials[3]);
		break;
	case kSpawnTeam_ERROR:
		if (CellSpawnMaterials[CellSpawnMaterials.Num() - 1] != nullptr) CellMeshComponent->SetMaterial(0, CellSpawnMaterials[CellSpawnMaterials.Num() - 1]);
		break;
	default:
		break;
	}
}

void ACell::SetCellHighlightMaterial(CellMaterial material_type) {
	switch (material_type)
	{
	case kCellMaterial_Default:
		break;
	case kCellMaterial_MoveHighlight:
		if (CellHighLightMaterials[0] != nullptr) CellMeshComponent->SetMaterial(0, CellHighLightMaterials[0]);
		break;
	case kCellMaterial_AttackHighlight:
		if (CellHighLightMaterials[2] != nullptr) CellMeshComponent->SetMaterial(0, CellHighLightMaterials[2]);
		break;
	case kCellMaterial_CurrentCell:
		SetCellCurrentCellMaterial();
		break;
	case kCellMaterial_HealHighlight:
		if (CellHighLightMaterials[3] != nullptr) CellMeshComponent->SetMaterial(0, CellHighLightMaterials[3]);
		break;
	default:
		break;
	};
	
}


void ACell::SetCellCurrentCellMaterial() {
	if (CellHighLightMaterials[1] != nullptr) CellMeshComponent->SetMaterial(0, CellHighLightMaterials[1]);
}


void ACell::HighlightCell(CellMaterial material_type) {

	switch(material_type)
	{
		case CellMaterial::kCellMaterial_Default:
			SetCellMaterial();
			break;
		case CellMaterial::kCellMaterial_MoveHighlight:
			SetCellHighlightMaterial(material_type);
			break;
		case CellMaterial::kCellMaterial_AttackHighlight:
			SetCellHighlightMaterial(material_type);
			break;
		case CellMaterial::kCellMaterial_CurrentCell:
			SetCellCurrentCellMaterial();
			break;
		case CellMaterial::kCellMaterial_HealHighlight:
			SetCellHighlightMaterial(material_type);
			break;
		default:
			break;
	};
}

