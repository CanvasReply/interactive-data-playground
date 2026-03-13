#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Http.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"

class UTextRenderComponent;
class UFont;

#include "IslandsManager.generated.h"

USTRUCT(BlueprintType)
struct FIslandDTO
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Id;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Pos = FVector::ZeroVector; // X,Y,Z in sm

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<float> Bars; // count of bars 5

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Label;
};

UCLASS(Blueprintable)
class PROJECTPROTOTYPE_API AIslandsManager : public AActor
{
    GENERATED_BODY()

public:
    AIslandsManager();

    virtual void OnConstruction(const FTransform& Transform) override;
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "Islands")
    void BuildFromData(const TArray<FIslandDTO>& Islands);

    UFUNCTION(BlueprintCallable, Category = "Islands|Data")
    void FetchAndBuild(); // HTTP GET to EndpointURL

protected:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    USceneComponent* Root;

    // 5 columns - id 0..4
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UHierarchicalInstancedStaticMeshComponent* BarsHISM0;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UHierarchicalInstancedStaticMeshComponent* BarsHISM1;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UHierarchicalInstancedStaticMeshComponent* BarsHISM2;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UHierarchicalInstancedStaticMeshComponent* BarsHISM3;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UHierarchicalInstancedStaticMeshComponent* BarsHISM4;

    // Optional: base of islands
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UHierarchicalInstancedStaticMeshComponent* BaseHISM;

    // Materials and meshes
    UPROPERTY(EditAnywhere, Category = "Islands|Meshes")
    UStaticMesh* BarMesh;

    UPROPERTY(EditAnywhere, Category = "Islands|Meshes")
    UStaticMesh* BaseMesh;

    UPROPERTY(EditAnywhere, Category = "Islands|Materials")
    TArray<UMaterialInterface*> BarMaterials;

    // Height control
    UPROPERTY(EditAnywhere, Category = "Islands|Bars", meta = (ClampMin = "1"))
    float BarMeshHeightCM = 100.f; // Real heigh of base mesh

    UPROPERTY(EditAnywhere, Category = "Islands|Bars", meta = (ClampMin = "1"))
    float MinBarHeightCM = 80.f;

    UPROPERTY(EditAnywhere, Category = "Islands|Bars", meta = (ClampMin = "1"))
    float MaxBarHeightCM = 400.f;

    UPROPERTY(EditAnywhere, Category = "Islands|Bars", meta = (ClampMin = "0.01"))
    float BarXYScale = 1.f; // Column width X/Y

    // Shift
    UPROPERTY(EditAnywhere, Category = "Islands|Layout")
    TArray<FVector> Offsets;

    // Bases
    UPROPERTY(EditAnywhere, Category = "Islands|Base")
    bool bUseBaseIslands = false;

    UPROPERTY(EditAnywhere, Category = "Islands|Base")
    FVector BaseScale = FVector(2.f, 2.f, 0.5f);

    // Data loading
    UPROPERTY(EditAnywhere, Category = "Islands|Data")
    bool bFetchOnBeginPlay = true;

    UPROPERTY(EditAnywhere, Category = "Islands|Data")
    FString EndpointURL = TEXT("http://localhost:3000/api/islands");

    // text displaying
    UPROPERTY(EditAnywhere, Category = "Islands|Labels") 
    bool bShowLabels = true;

    UPROPERTY(EditAnywhere, Category = "Islands|Labels") 
    float LabelWorldSize = 40.f; // font size

    UPROPERTY(EditAnywhere, Category = "Islands|Labels") 
    float LabelZPadding = 40.f; // padding

    UPROPERTY(EditAnywhere, Category = "Islands|Labels") 
    UFont* LabelFont = nullptr;



private:
    UPROPERTY(Transient)
    TArray<class UTextRenderComponent*> LabelComponents;

    void ClearAllInstances();
    void EnsureOffsets();
    void ApplyMeshesAndMaterials();
    static void ComputeMinMax(const TArray<FIslandDTO>& Islands, float& OutMin, float& OutMax);
    float MapHeight(float V, float MinV, float MaxV) const;
    void OnHttpCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
};
