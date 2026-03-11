
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RouteScenario.generated.h"

UCLASS()
class PROJECTPROTOTYPE_API ARouteScenario : public AActor
{
    GENERATED_BODY()

public:
    ARouteScenario();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenario|Backend")
    FString BackendUrl = TEXT("http://localhost:3000/api/a-route");

    //-- Markers (set BP_StartMarker & BP_EndMarker)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenario|Markers")
    TSubclassOf<AActor> StartMarkerClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenario|Markers")
    TSubclassOf<AActor> EndMarkerClass;

    // Linked instances (could be set from manually)
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Scenario|Markers")
    AActor* StartMarker = nullptr;

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Scenario|Markers")
    AActor* EndMarker = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenario|Flow")
    bool bAutoFetchOnBeginPlay = true;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scenario|Data")
    FVector StartPos = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scenario|Data")
    FVector EndPos = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scenario|Data")
    float Speed = 600.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scenario|Data")
    FString Mode = TEXT("straight");

    UPROPERTY(EditAnywhere, Category = "Scenario|Data")
    float RefreshSeconds = 2.0f;


    // Manual start or with BeginPlay
    UFUNCTION(BlueprintCallable, Category = "Scenario|Backend")
    void FetchAndApply();

    // To run without BE
    UFUNCTION(BlueprintCallable, Category = "Scenario|Markers")
    void SetStart(const FVector& P);

    UFUNCTION(BlueprintCallable, Category = "Scenario|Markers")
    void SetEnd(const FVector& P);

    // Vehicle
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenario|Vehicle")
    APawn* Vehicle = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenario|Vehicle", meta = (ClampMin = "0.0"))
    float AcceptanceRadius = 20.f;

    UFUNCTION(BlueprintCallable, Category = "Scenario|Flow")
    void StartRun(); // put Vehicle to start and go

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

private:
    void ApplyFromJson(const FString& Body);
    AActor* EnsureMarker(TSubclassOf<AActor> MarkerClass, AActor*& MarkerRef, const FVector& Pos) const;
    void UpdateMarkers() const;
    bool bMoving = false;
};