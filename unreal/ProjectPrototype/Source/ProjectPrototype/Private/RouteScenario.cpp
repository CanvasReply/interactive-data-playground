#include "RouteScenario.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

ARouteScenario::ARouteScenario()
{
    //PrimaryActorTick.bCanEverTick = false; // only markers (without movement)
    PrimaryActorTick.bCanEverTick = true;
}

void ARouteScenario::BeginPlay()
{
    Super::BeginPlay();
    if (bAutoFetchOnBeginPlay)
    {
        FetchAndApply();
    }
}

void ARouteScenario::FetchAndApply()
{
    if (BackendUrl.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("BackendUrl is empty"));
        return;
    }

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Req = FHttpModule::Get().CreateRequest();
    Req->SetURL(BackendUrl);
    Req->SetVerb(TEXT("GET"));
    Req->SetHeader(TEXT("Accept"), TEXT("application/json"));

    Req->OnProcessRequestComplete().BindWeakLambda(this,
        [this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded)
        {
            if (!bSucceeded || !Response.IsValid())
            {
                UE_LOG(LogTemp, Error, TEXT("Scenario request failed"));
                return;
            }
            const int32 Code = Response->GetResponseCode();
            if (Code != 200)
            {
                UE_LOG(LogTemp, Error, TEXT("Scenario request HTTP %d"), Code);
                return;
            }
            ApplyFromJson(Response->GetContentAsString());
        });

    Req->ProcessRequest();
}

void ARouteScenario::ApplyFromJson(const FString& Body)
{
    TSharedPtr<FJsonObject> Root;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Body);
    if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse scenario JSON"));
        return;
    }

    auto ReadVec = [](const TSharedPtr<FJsonObject>& Obj, const FString& Field, FVector& Out) -> bool
        {
            const TSharedPtr<FJsonObject>* SubObj = nullptr;
            if (!Obj->TryGetObjectField(Field, SubObj) || !SubObj || !SubObj->IsValid())
                return false;
            const TSharedPtr<FJsonObject>& S = *SubObj;
            double X = 0, Y = 0, Z = 0;
            if (!S->TryGetNumberField(TEXT("x"), X)) return false;
            if (!S->TryGetNumberField(TEXT("y"), Y)) return false;
            if (!S->TryGetNumberField(TEXT("z"), Z)) return false;
            Out = FVector((float)X, (float)Y, (float)Z);
            return true;
        };

    FVector NewStart, NewEnd;
    if (!ReadVec(Root, TEXT("start"), NewStart) || !ReadVec(Root, TEXT("end"), NewEnd))
    {
        UE_LOG(LogTemp, Error, TEXT("JSON missing start/end vectors"));
        return;
    }
    StartPos = NewStart;
    EndPos = NewEnd;

    if (Root->HasTypedField<EJson::Number>(TEXT("speed")))
    {
        Speed = (float)Root->GetNumberField(TEXT("speed"));
    }
    if (Root->HasTypedField<EJson::String>(TEXT("mode")))
    {
        Mode = Root->GetStringField(TEXT("mode"));
    }

    UpdateMarkers();
    StartRun(); // comment out if you need markers only

    UE_LOG(LogTemp, Log, TEXT("Scenario applied: Start(%.1f,%.1f,%.1f) End(%.1f,%.1f,%.1f) Speed=%.1f Mode=%s"),
        StartPos.X, StartPos.Y, StartPos.Z,
        EndPos.X, EndPos.Y, EndPos.Z,
        Speed, *Mode);
}

AActor* ARouteScenario::EnsureMarker(TSubclassOf<AActor> MarkerClass, AActor*& MarkerRef, const FVector& Pos) const
{
    UWorld* W = GetWorld();
    if (!W) return nullptr;

    const FVector P = Pos + FVector(0, 0, 2.f);

    if (!MarkerRef)
    {
        if (*MarkerClass)
        {
            MarkerRef = W->SpawnActor<AActor>(MarkerClass, P, FRotator::ZeroRotator);
        }
    }
    if (MarkerRef)
    {
        MarkerRef->SetActorLocation(P);
    }
    return MarkerRef;
}

void ARouteScenario::UpdateMarkers() const
{
    const_cast<ARouteScenario*>(this)->EnsureMarker(StartMarkerClass, const_cast<ARouteScenario*>(this)->StartMarker, StartPos);
    const_cast<ARouteScenario*>(this)->EnsureMarker(EndMarkerClass, const_cast<ARouteScenario*>(this)->EndMarker, EndPos);
}

void ARouteScenario::SetStart(const FVector& P)
{
    StartPos = P;
    UpdateMarkers();
}

void ARouteScenario::SetEnd(const FVector& P)
{
    EndPos = P;
    UpdateMarkers();
}

void ARouteScenario::StartRun() {
    if (!Vehicle) {
        UE_LOG(LogTemp, Warning, TEXT("Vehicle is not set"));
        return;
    }
    if (Speed <= 0.f) Speed = 600.f; // default for 0

    FVector StartFlat = StartPos;
    Vehicle->SetActorLocation(StartFlat);
    bMoving = true;
}

void ARouteScenario::Tick(float DeltaSeconds) {
    Super::Tick(DeltaSeconds);
    if (!bMoving || !Vehicle) return;

    const FVector L = Vehicle->GetActorLocation();
    FVector to = EndPos - L;

    to.Z = 0.f; // x.y only

    const float dist = to.Size();
    if (dist <= AcceptanceRadius) {
        bMoving = false;
        return;
    }

    const FVector dir = to / dist;
    const FVector step = dir * Speed * DeltaSeconds;

    // position update
    const FVector newLoc(L.X + step.X, L.Y + step.Y, StartPos.Z);
    Vehicle->SetActorLocation(newLoc);

    // rotation if needed
    const FRotator desired = dir.Rotation();
    const FRotator current = Vehicle->GetActorRotation();
    Vehicle->SetActorRotation(FMath::RInterpTo(current, desired, DeltaSeconds, 6.f));
}
