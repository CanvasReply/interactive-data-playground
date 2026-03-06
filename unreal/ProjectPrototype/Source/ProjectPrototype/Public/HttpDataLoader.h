#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpDataLoader.generated.h"

UCLASS()
class PROJECTPROTOTYPE_API AHttpDataLoader : public AActor
{
	GENERATED_BODY()

	public:
		AHttpDataLoader();

		UFUNCTION(BlueprintCallable, Category = "HTTP")
		void LoadData();

	protected:
		virtual void BeginPlay() override;

	private:
		void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};

/*
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpDataLoader.generated.h"

UCLASS()
class PROJECTPROTOTYPE_API AHttpDataLoader : public AActor
{
    GENERATED_BODY()

	public:
		AHttpDataLoader();

	protected:
		virtual void BeginPlay() override;

	public:
		UFUNCTION(BlueprintCallable, Category="HTTP")
		void LoadData();

	private:
		void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
*/
