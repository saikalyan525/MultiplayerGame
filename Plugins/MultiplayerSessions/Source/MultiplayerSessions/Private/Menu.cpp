// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"

#include "Engine/Engine.h"
#include "Engine/NetDriver.h"
#include "Engine/World.h"

#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"

void UMenu::MenuSetup(int32 InNumPublicConnections, FString InMatchType)
{
	
	NumPublicConnections = InNumPublicConnections;
	MatchType = InMatchType;

	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	SetIsEnabled(true);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Menu Widget Added to Screen !"));
	}

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);	
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);

	}

}

bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}

	return true;


}

void UMenu::NativeDestruct()
{
	MenuTearDown();
	Super::NativeDestruct();
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Session Created Successfully!"));
		}

		UWorld* World = GetWorld();
		if (World)
		{

			if (GEngine)
			{

				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("Current Map: %s"), *World->GetMapName()));

			}

			World->ServerTravel("/Game/DarkForest/Maps/Demo?listen");

		}

	}
	else
	{

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed To Create Session!"));
		}

	}

}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{

	if (MultiplayerSessionsSubsystem == nullptr)
	{
		return;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString::Printf(TEXT("Total Sessions Found: %d"), SessionResults.Num()));
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString::Printf(TEXT("Search Successful: %s"), bWasSuccessful ? TEXT("TRUE") : TEXT("FALSE")));
	}

	for (auto Result : SessionResults )
	{
		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);

		if (SettingsValue == MatchType)
		{
			MultiplayerSessionsSubsystem->JoinSession(Result);
			return;
		}

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Cyan, TEXT("Inside Session Array!"));
			GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Cyan, FString::Printf(TEXT("Host Name: %s"), *Result.Session.OwningUserName));
			GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Yellow, FString::Printf(TEXT("Found MatchType: %s"), *SettingsValue));
			GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Yellow, FString::Printf(TEXT("Required MatchType: %s"), *MatchType));
	
		}


	}

}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{

	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();

		if (SessionInterface.IsValid())
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController)
			{
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}

			
		}
	}



}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
}

void UMenu::HostButtonClicked()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Host Button Clicked!"));
	}

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
	}

}

void UMenu::JoinButtonClicked()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Join Button Clicked!"));

		for (const FNetDriverDefinition& Definition : GEngine->NetDriverDefinitions)
		{
			GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Cyan, FString::Printf(TEXT("Driver Name: %s"), *Definition.DefName.ToString()));
			GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Yellow, FString::Printf(TEXT("Driver Class: %s"), *Definition.DriverClassName.ToString()));
			GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Yellow, FString::Printf(TEXT("Fallback Class: %s"), *Definition.DriverClassNameFallback.ToString()));

			UE_LOG(LogTemp, Warning, TEXT("NetDriver: %s | Class: %s | Fallback: %s"), *Definition.DefName.ToString(), *Definition.DriverClassName.ToString(), *Definition.DriverClassNameFallback.ToString());
		}
	}



	if (MultiplayerSessionsSubsystem)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("Searching MatchType: %s"), *MatchType));
		}

		MultiplayerSessionsSubsystem->FindSessions(10000);
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("MultiplayerSessionsSubsystem Invalid!"));
		}
	}

}

void UMenu::MenuTearDown()
{
	
	RemoveFromParent();
	UWorld* World = GetWorld();
	if (World)
	{

		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}

	}

}
