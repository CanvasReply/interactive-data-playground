#include "SensorManager.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "TimerManager.h" 
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/IHttpRequest.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "UObject/ConstructorHelpers.h"

ASensorManager::ASensorManager()
{
    PrimaryActorTick.bCanEverTick = false;

    Spheres = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("Spheres"));
    SetRootComponent(Spheres);

    // Try to use default sphere from Engine
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereObj(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (SphereObj.Succeeded())
    {
        SphereMesh = SphereObj.Object;
    }

    // Component setup
    if (SphereMesh)
    {
        Spheres->SetStaticMesh(SphereMesh);
    }
    Spheres->NumCustomDataFloats = 3; // RGB
};

void ASensorManager::BeginPlay()
{
    Super::BeginPlay();

    if (SensorMaterial)
    {
        Spheres->SetMaterial(0, SensorMaterial);
    }

    RequestSensors();

    if (RefreshSeconds > 0.f)
    {
        FTimerHandle TimerHandle;
        GetWorldTimerManager().SetTimer(
            TimerHandle,
            this,
            &ASensorManager::RequestSensors,
            RefreshSeconds,
            true
        );
    }
}

void ASensorManager::RequestSensors()
{
    FHttpModule& Http = FHttpModule::Get();
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Req = Http.CreateRequest();
    Req->SetURL(ApiUrl);
    Req->SetVerb(TEXT("GET"));
    Req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Req->OnProcessRequestComplete().BindUObject(this, &ASensorManager::OnSensorsResponse);
    Req->ProcessRequest();
}

void ASensorManager::OnSensorsResponse(FHttpRequestPtr /*Request*/, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful || !Response.IsValid() || Response->GetResponseCode() != 200)
    {
        UE_LOG(LogTemp, Warning, TEXT("Sensors API request failed"));
        return;
    }

    const FString Body = Response->GetContentAsString();

    TSharedPtr<FJsonValue> RootJson;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Body);
    if (!FJsonSerializer::Deserialize(Reader, RootJson) || !RootJson.IsValid() || RootJson->Type != EJson::Array)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid JSON from sensors API"));
        return;
    }

    const TArray<TSharedPtr<FJsonValue>>& Arr = RootJson->AsArray();

    // Recreate instance on each update
    Spheres->ClearInstances();

    for (const TSharedPtr<FJsonValue>& Item : Arr)
    {
        if (!Item.IsValid() || Item->Type != EJson::Object)
            continue;

        const TSharedPtr<FJsonObject> Obj = Item->AsObject();

        // position
        FVector Pos = FVector::ZeroVector;
        if (const TSharedPtr<FJsonObject>* PosObjPtr; Obj->TryGetObjectField(TEXT("position"), PosObjPtr))
        {
            const TSharedPtr<FJsonObject>& PosObj = *PosObjPtr;
            double X = 0, Y = 0, Z = 0;
            PosObj->TryGetNumberField(TEXT("x"), X);
            PosObj->TryGetNumberField(TEXT("y"), Y);
            PosObj->TryGetNumberField(TEXT("z"), Z);
            Pos = FVector(X, Y, Z) * CoordScale; // if meters we need * 100
        }

        // temperature -> radius (sm)
        double Temp = 0.0;
        Obj->TryGetNumberField(TEXT("temperature"), Temp);
        const double RadiusCm = Temp * TemperatureToCm;

        // Base radius SM_Sphere = 50 sm (diameter ~100)
        const double BaseRadius = 50.0;
        const float Scale = FMath::Max(0.1f, static_cast<float>(RadiusCm / BaseRadius));

        // Status -> Color
        FString Status;
        Obj->TryGetStringField(TEXT("status"), Status);
        const FLinearColor C = StatusToColor(Status);

        const int32 Index = Spheres->AddInstance(FTransform(FRotator::ZeroRotator, Pos, FVector(Scale)));

        // RGB -> Per-Instance Custom Data (0..2)
        Spheres->SetCustomDataValue(Index, 0, C.R, false);
        Spheres->SetCustomDataValue(Index, 1, C.G, false);
        Spheres->SetCustomDataValue(Index, 2, C.B, true); // true — marker for rerender
    }
}

FLinearColor ASensorManager::StatusToColor(const FString& Status) const
{
    if (Status.Equals(TEXT("active"), ESearchCase::IgnoreCase))
        return FLinearColor(0.f, 1.f, 0.f);        // Green
    if (Status.Equals(TEXT("warning"), ESearchCase::IgnoreCase))
        return FLinearColor(1.f, 0.6f, 0.f);       // Orange
    if (Status.Equals(TEXT("error"), ESearchCase::IgnoreCase))
        return FLinearColor(1.f, 0.f, 0.f);        // Red
    if (Status.Equals(TEXT("inactive"), ESearchCase::IgnoreCase))
        return FLinearColor(0.5f, 0.5f, 0.5f);     // Grey
    return FLinearColor(0.2f, 0.6f, 1.f);          // Default
}
