#include "IslandsManager.h"
#include "Components/SceneComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/Font.h"  
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

AIslandsManager::AIslandsManager()
{
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    BarsHISM0 = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("BarsHISM0"));
    BarsHISM1 = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("BarsHISM1"));
    BarsHISM2 = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("BarsHISM2"));
    BarsHISM3 = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("BarsHISM3"));
    BarsHISM4 = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("BarsHISM4"));
    BaseHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("BaseHISM"));

    BarsHISM0->SetupAttachment(Root);
    BarsHISM1->SetupAttachment(Root);
    BarsHISM2->SetupAttachment(Root);
    BarsHISM3->SetupAttachment(Root);
    BarsHISM4->SetupAttachment(Root);
    BaseHISM->SetupAttachment(Root);

    BarsHISM0->SetMobility(EComponentMobility::Movable);
    BarsHISM1->SetMobility(EComponentMobility::Movable);
    BarsHISM2->SetMobility(EComponentMobility::Movable);
    BarsHISM3->SetMobility(EComponentMobility::Movable);
    BarsHISM4->SetMobility(EComponentMobility::Movable);
    BaseHISM->SetMobility(EComponentMobility::Movable);

    // Default shifts
    Offsets = {
        FVector(-60.f,   0.f, 0.f),
        FVector(60.f,   0.f, 0.f),
        FVector(0.f, -60.f, 0.f),
        FVector(0.f,  60.f, 0.f),
        FVector(0.f,   0.f, 0.f)
    };
}

void AIslandsManager::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    EnsureOffsets();
    ApplyMeshesAndMaterials();
}

void AIslandsManager::BeginPlay()
{
    Super::BeginPlay();

    ApplyMeshesAndMaterials();
    EnsureOffsets();

    if (bFetchOnBeginPlay && !EndpointURL.IsEmpty())
    {
        FetchAndBuild();
    }
}

void AIslandsManager::EnsureOffsets()
{
    if (Offsets.Num() < 5)
    {
        Offsets.SetNum(5);
        Offsets[0] = FVector(-60.f, 0.f, 0.f);
        Offsets[1] = FVector(60.f, 0.f, 0.f);
        Offsets[2] = FVector(0.f, -60.f, 0.f);
        Offsets[3] = FVector(0.f, 60.f, 0.f);
        Offsets[4] = FVector(0.f, 0.f, 0.f);
    }
}

void AIslandsManager::ApplyMeshesAndMaterials()
{
    // Meshes
    if (BarMesh)
    {
        BarsHISM0->SetStaticMesh(BarMesh);
        BarsHISM1->SetStaticMesh(BarMesh);
        BarsHISM2->SetStaticMesh(BarMesh);
        BarsHISM3->SetStaticMesh(BarMesh);
        BarsHISM4->SetStaticMesh(BarMesh);
    }
    if (BaseMesh)
    {
        BaseHISM->SetStaticMesh(BaseMesh);
    }

    // Materials by column index
    auto ApplyMat = [&](UHierarchicalInstancedStaticMeshComponent* Comp, int32 Index)
        {
            if (Comp && BarMaterials.IsValidIndex(Index) && BarMaterials[Index])
            {
                Comp->SetMaterial(0, BarMaterials[Index]);
            }
        };
    ApplyMat(BarsHISM0, 0);
    ApplyMat(BarsHISM1, 1);
    ApplyMat(BarsHISM2, 2);
    ApplyMat(BarsHISM3, 3);
    ApplyMat(BarsHISM4, 4);
}

void AIslandsManager::ClearAllInstances()
{
    BarsHISM0->ClearInstances();
    BarsHISM1->ClearInstances();
    BarsHISM2->ClearInstances();
    BarsHISM3->ClearInstances();
    BarsHISM4->ClearInstances();
    BaseHISM->ClearInstances();
    for (UTextRenderComponent* C : LabelComponents)
    {
        if (IsValid(C))
        {
            C->DestroyComponent();
        }
    }
    LabelComponents.Reset();
}

void AIslandsManager::ComputeMinMax(const TArray<FIslandDTO>& Islands, float& OutMin, float& OutMax)
{
    OutMin = TNumericLimits<float>::Max();
    OutMax = TNumericLimits<float>::Lowest();

    for (const auto& I : Islands)
    {
        for (int32 j = 0; j < I.Bars.Num(); ++j)
        {
            OutMin = FMath::Min(OutMin, I.Bars[j]);
            OutMax = FMath::Max(OutMax, I.Bars[j]);
        }
    }
    if (!FMath::IsFinite(OutMin) || !FMath::IsFinite(OutMax))
    {
        OutMin = 0.f; OutMax = 1.f;
    }
    if (FMath::IsNearlyEqual(OutMin, OutMax))
    {
        OutMax = OutMin + 1.f; // division by 0 protection
    }
}

float AIslandsManager::MapHeight(float V, float MinV, float MaxV) const
{
    const float T = FMath::Clamp((V - MinV) / (MaxV - MinV), 0.f, 1.f);
    return FMath::Lerp(MinBarHeightCM, MaxBarHeightCM, T);
}

void AIslandsManager::BuildFromData(const TArray<FIslandDTO>& Islands)
{
    ClearAllInstances();
    ApplyMeshesAndMaterials();
    EnsureOffsets();

    if (!BarMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("AIslandsManager: BarMesh is not selected."));
        return;
    }

    float MinV, MaxV;
    ComputeMinMax(Islands, MinV, MaxV);

    UHierarchicalInstancedStaticMeshComponent* BarComps[5] =
    { BarsHISM0, BarsHISM1, BarsHISM2, BarsHISM3, BarsHISM4 };

    for (const FIslandDTO& Island : Islands)
    {
        // base of island (optional)
        if (bUseBaseIslands && BaseMesh)
        {
            const FTransform BaseT(FRotator::ZeroRotator, Island.Pos, BaseScale);
            BaseHISM->AddInstance(BaseT);
        }

        float HTop = 0.f;

        // columns
        for (int32 i = 0; i < 5 && i < Island.Bars.Num(); ++i)
        {
            const float H = MapHeight(Island.Bars[i], MinV, MaxV);
            HTop = FMath::Max(HTop, H);
            const FVector Loc = Island.Pos + Offsets[i] + FVector(0.f, 0.f, H * 0.5f);
            const FVector Scale(BarXYScale, BarXYScale, H / FMath::Max(1.f, BarMeshHeightCM));
            const FTransform T(FRotator::ZeroRotator, Loc, Scale);

            if (BarComps[i])
            {
                BarComps[i]->AddInstance(T);
            }
        }


		// labels
        if (bShowLabels && !Island.Label.IsEmpty())
        {
            UTextRenderComponent* TC = NewObject<UTextRenderComponent>(this);
            if (TC)
            {
                TC->RegisterComponent();
                TC->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);

                TC->SetText(FText::FromString(Island.Label));
                TC->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
                TC->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
                if (LabelFont) TC->SetFont(LabelFont);
                TC->SetWorldSize(LabelWorldSize);
                TC->SetTextRenderColor(FColor::White);

                const FVector LabelLoc = Island.Pos + FVector(0.f, 0.f, HTop + LabelZPadding);
                TC->SetWorldLocation(LabelLoc);

                LabelComponents.Add(TC);
            }
        }
    }

}

void AIslandsManager::FetchAndBuild()
{
    if (EndpointURL.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("AIslandsManager: EndpointURL is empty."));
        return;
    }

    FHttpModule& Http = FHttpModule::Get();
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Req = Http.CreateRequest();
    Req->SetURL(EndpointURL);
    Req->SetVerb(TEXT("GET"));
    Req->OnProcessRequestComplete().Unbind();
    Req->OnProcessRequestComplete().BindUObject(this, &AIslandsManager::OnHttpCompleted);
    Req->ProcessRequest();
}

void AIslandsManager::OnHttpCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
    if (!bConnectedSuccessfully || !Response.IsValid() || !EHttpResponseCodes::IsOk(Response->GetResponseCode()))
    {
        UE_LOG(LogTemp, Error, TEXT("AIslandsManager: request failed. Code=%d"), Response.IsValid() ? Response->GetResponseCode() : -1);
        return;
    }

    TArray<FIslandDTO> Islands;
    const FString Body = Response->GetContentAsString();

    // Array expected
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Body);
    TArray<TSharedPtr<FJsonValue>> JsonArr;
    if (!FJsonSerializer::Deserialize(Reader, JsonArr))
    {
        UE_LOG(LogTemp, Error, TEXT("AIslandsManager: error on JSON parsing."));
        return;
    }

    for (const TSharedPtr<FJsonValue>& V : JsonArr)
    {
        const TSharedPtr<FJsonObject>* ObjPtr;
        if (!V.IsValid() || !V->TryGetObject(ObjPtr)) continue;

        FIslandDTO I;

        // id
        (*ObjPtr)->TryGetStringField(TEXT("id"), I.Id);

        // pos: [x,y,z]
        const TArray<TSharedPtr<FJsonValue>>* PosArr;
        if ((*ObjPtr)->TryGetArrayField(TEXT("pos"), PosArr) && PosArr->Num() >= 3)
        {
            I.Pos.X = (float)(*PosArr)[0]->AsNumber();
            I.Pos.Y = (float)(*PosArr)[1]->AsNumber();
            I.Pos.Z = (float)(*PosArr)[2]->AsNumber();
        }

        // bars: 5 numbers
        const TArray<TSharedPtr<FJsonValue>>* BarsArr;
        if ((*ObjPtr)->TryGetArrayField(TEXT("bars"), BarsArr))
        {
            for (int32 k = 0; k < BarsArr->Num(); ++k)
            {
                I.Bars.Add((float)(*BarsArr)[k]->AsNumber());
            }
        }

        // label (optional)
        (*ObjPtr)->TryGetStringField(TEXT("label"), I.Label);

        Islands.Add(MoveTemp(I));
    }

    BuildFromData(Islands);
}