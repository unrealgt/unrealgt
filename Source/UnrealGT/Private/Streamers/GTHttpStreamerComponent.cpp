// Fill out your copyright notice in the Description page of Project Settings.

#include "GTHttpStreamerComponent.h"

#include "Json.h"
#include "JsonUtilities.h"
#include "Runtime/Online/HTTP/Public/Http.h"

UGTHttpStreamerComponent::UGTHttpStreamerComponent()
    : Super()
    , TargetUrl(TEXT("localhost:8080/example"))
    , Method(EGTHttpMethod::Post)
    , ContentType(EGTHttpContentType::TextPlain)
{
}

void UGTHttpStreamerComponent::OnDataReady(const TArray<uint8>& Data, const FDateTime& TimeStamp)
{
    TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();

    Request->SetURL(TargetUrl);

    Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));

    FString ContentTypeString;
    switch (ContentType)
    {
        case EGTHttpContentType::Json:
            ContentTypeString = TEXT("application/json");
            break;
        case EGTHttpContentType::PNG:
            ContentTypeString = TEXT("image/png");
            break;
        case EGTHttpContentType::BMP:
            ContentTypeString = TEXT("image/bmp");
            break;
        case EGTHttpContentType::TextPlain:
            ContentTypeString = TEXT("text/plain");
            break;
        case EGTHttpContentType::OctetStream:
            ContentTypeString = TEXT("application/octet-stream");
            break;
        default:
            ContentTypeString = TEXT("text/plain");
            break;
    }
    Request->SetHeader(TEXT("Content-Type"), ContentTypeString);

    FString MethodString;
    switch (Method)
    {
        case EGTHttpMethod::Post:
            MethodString = TEXT("POST");
            break;
        case EGTHttpMethod::Put:
            MethodString = TEXT("PUT");
            break;
        default:
            MethodString = TEXT("POST");
            break;
    }
    Request->SetVerb(MethodString);

    Request->SetContent(Data);

    Request->ProcessRequest();
}
