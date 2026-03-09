// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h" // Spheres
#include "Engine/StaticMesh.h"                                   // SphereMesh
#include "Materials/MaterialInterface.h"                         // SensorMaterial
#include "Http.h"
#include "SensorManager.generated.h"

UCLASS()
class PROJECTPROTOTYPE_API ASensorManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASensorManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere)
    UHierarchicalInstancedStaticMeshComponent* Spheres;

    UPROPERTY(EditDefaultsOnly, Category = "Sensors|Assets")
    UStaticMesh* SphereMesh;

    UPROPERTY(EditDefaultsOnly, Category = "Sensors|Assets")
    UMaterialInterface* SensorMaterial;

    UPROPERTY(EditAnywhere, Category = "Sensors|Data")
    FString ApiUrl = TEXT("http://127.0.0.1:3000/sensors");

    UPROPERTY(EditAnywhere, Category = "Sensors|Data")
    float CoordScale = 1.f;

    UPROPERTY(EditAnywhere, Category = "Sensors|Visual")
    float TemperatureToCm = 2.f;

    UPROPERTY(EditAnywhere, Category = "Sensors|Data")
    float RefreshSeconds = 2.0f;

public:	
	// Called every frame
    void RequestSensors();
    void OnSensorsResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    FLinearColor StatusToColor(const FString& Status) const;
};

