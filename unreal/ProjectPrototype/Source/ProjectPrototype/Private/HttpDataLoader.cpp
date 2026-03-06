#include "HttpDataLoader.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

AHttpDataLoader::AHttpDataLoader()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AHttpDataLoader::BeginPlay()
{
    Super::BeginPlay();
    LoadData();
}

void AHttpDataLoader::LoadData()
{
    // http request
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &AHttpDataLoader::OnResponseReceived);
    Request->SetURL(TEXT("http://localhost:3000/api/objects"));
    Request->SetVerb(TEXT("GET"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->ProcessRequest();
}

void AHttpDataLoader::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful || !Response.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("HTTP Request failed"));
        return;
    }

    FString ResponseString = Response->GetContentAsString();

    TArray<TSharedPtr<FJsonValue>> JsonArray;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);

    if (FJsonSerializer::Deserialize(Reader, JsonArray))
    {
        for (const TSharedPtr<FJsonValue>& Elem : JsonArray)
        {
            TSharedPtr<FJsonObject> Obj = Elem->AsObject();
            if (!Obj.IsValid())
            {
                continue;
            }

            const FString Name = Obj->GetStringField(TEXT("name"));

            TSharedPtr<FJsonObject> Position = Obj->GetObjectField(TEXT("position"));
            const float X = static_cast<float>(Position->GetNumberField(TEXT("x")));
            const float Y = static_cast<float>(Position->GetNumberField(TEXT("y")));
            const float Z = static_cast<float>(Position->GetNumberField(TEXT("z")));

            const FString Status = Obj->GetStringField(TEXT("status"));

            UE_LOG(LogTemp, Log, TEXT("Object: %s, Position: (%f, %f, %f), Status: %s"), *Name, X, Y, Z, *Status);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON response (expected JSON array)"));
    }
}
