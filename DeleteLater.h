
void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		UE_LOG(LogTemp, Error, TEXT("JoinSession Failed! Result: %d"), static_cast<int32>(Result));

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Red, FString::Printf(TEXT("JoinSession Failed! Result: %d"), static_cast<int32>(Result)));
		}

		return;
	}

	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();

	if (!Subsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("Online Subsystem Invalid!"));
		return;
	}

	IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();

	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Session Interface Invalid!"));
		return;
	}

	FString Address;

	if (!SessionInterface->GetResolvedConnectString(NAME_GameSession, Address) || Address.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed To Resolve Connection Address!"));

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Red, TEXT("Failed To Resolve Connection Address!"));
		}

		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("JoinSession Successful!"));
	UE_LOG(LogTemp, Warning, TEXT("Resolved Address: %s"), *Address);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Green, TEXT("JoinSession Successful!"));
		GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Cyan, FString::Printf(TEXT("Resolved Address: %s"), *Address));
	}

	UGameInstance* GameInstance = GetGameInstance();

	if (!GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstance Invalid!"));
		return;
	}

	APlayerController* PlayerController = GameInstance->GetFirstLocalPlayerController();

	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController Invalid!"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Executing ClientTravel: %s"), *Address);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Yellow, TEXT("Executing ClientTravel!"));
	}

	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}






UClass* SteamDriverClass = LoadClass<UNetDriver>(nullptr, TEXT("/Script/OnlineSubsystemSteam.SteamNetDriver"));

if (SteamDriverClass)
{
	UE_LOG(LogTemp, Warning, TEXT("SteamNetDriver Class Loaded Successfully!"));

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Green, TEXT("SteamNetDriver Class Loaded Successfully!"));
	}
}
else
{
	UE_LOG(LogTemp, Error, TEXT("SteamNetDriver Class Failed To Load!"));

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Red, TEXT("SteamNetDriver Class Failed To Load!"));
	}
}


