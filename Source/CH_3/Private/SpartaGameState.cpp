#include "SpartaGameState.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "ItemBase.h"
#include "Components/ProgressBar.h"
#include "SpartaCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"

ASpartaGameState::ASpartaGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	LevelDuration = 30.0f;
	CurrentLevelIndex = 0;
	MaxLevels = 3;
	CurrentWaveIndex = 0;
	MaxWaves = 3;

	LevelMapNames.Empty();
	LevelMapNames.Add(TEXT("BasicLevel")); 
	LevelMapNames.Add(TEXT("AdvancedLevel"));   
	LevelMapNames.Add(TEXT("IntermediateLevel"));   
}

void ASpartaGameState::BeginPlay()
{
	Super::BeginPlay();

	StartLevel();

	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&ASpartaGameState::UpdateHUD,
		0.1f,
		true
	);
}

int32 ASpartaGameState::GetScore() const
{
	return Score;
}

void ASpartaGameState::AddScore(int32 Amount)
{
	Score += Amount;

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			SpartaGameInstance->AddToScore(Amount);
		}
	}
}

void ASpartaGameState::ClearField()
{
	TArray<AActor*> FoundItems;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItemBase::StaticClass(), FoundItems);

	for (AActor* Item : FoundItems)
	{
		if (Item)
		{
			Item->Destroy();
		}
	}
}


void ASpartaGameState::StartLevel()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowGameHUD();
		}
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
		}
	}

	CurrentWaveIndex = 0;
	StartWave();
}

void ASpartaGameState::StartWave()
{
	ClearField();

	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	float WaveDuration = 60.0f - (CurrentWaveIndex * 15.0f); 
	TargetSpawnCount = 3 - CurrentWaveIndex;              
	CurrentSpawnCount = 0;


	SpawnItemCycle();

	
	if (TargetSpawnCount > 1)
	{
		float SpawnInterval = WaveDuration / TargetSpawnCount; 

		GetWorldTimerManager().SetTimer(
			ItemSpawnTimerHandle,
			this,
			&ASpartaGameState::SpawnItemCycle,
			SpawnInterval,
			true 
		);
	}
	
	FString WaveStartMessage = FString::Printf(TEXT("Wave %d"), CurrentWaveIndex + 1);

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPC = Cast<ASpartaPlayerController>(PC))
		{
			if (UUserWidget* HUDWidget = SpartaPC->GetHUDWidget())
			{
				UFunction* AnnouncementFunc = HUDWidget->FindFunction(FName("PlayAnnouncementUI"));
				if (AnnouncementFunc)
				{
					struct FAnnouncementParams
					{
						FText InText;
					};

					FAnnouncementParams Params;
					Params.InText = FText::FromString(WaveStartMessage);

					HUDWidget->ProcessEvent(AnnouncementFunc, &Params);
				}
			}
		}
	}




	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, WaveStartMessage);
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), *WaveStartMessage);

	GetWorldTimerManager().SetTimer(
		WaveTimerHandle,
		this,
		&ASpartaGameState::OnWaveTimeUp,
		WaveDuration,
		false
	);

	UpdateHUD();
}

void ASpartaGameState::SpawnItemCycle()
{
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	if (FoundVolumes.Num() > 0)
	{
		ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
		if (SpawnVolume)
		{
			for (int32 i = 0; i < ItemsPerSpawnCycle; i++)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
				}
			}
		}
	}

	CurrentSpawnCount++;

	UE_LOG(LogTemp, Warning, TEXT("Wave %d"),
		CurrentWaveIndex + 1, CurrentSpawnCount, TargetSpawnCount, SpawnedCoinCount);

	if (CurrentSpawnCount >= TargetSpawnCount)
	{
		GetWorldTimerManager().ClearTimer(ItemSpawnTimerHandle);
	}
}

float ASpartaGameState::GetWaveTimeRatio() const
{
	float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
	RemainingTime = FMath::Max(0.0f, RemainingTime);

	float WaveDuration = 60.0f - (CurrentWaveIndex * 15.0f);

	return (WaveDuration > 0.0f) ? (RemainingTime / WaveDuration) : 0.0f;
}

void ASpartaGameState::OnWaveTimeUp()
{
	EndWave();
}

void ASpartaGameState::OnCoinCollected()
{
	CollectedCoinCount++;

		CollectedCoinCount++;

		if (CurrentSpawnCount >= TargetSpawnCount && SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
		{

			GetWorldTimerManager().ClearTimer(WaveTimerHandle);
			EndWave();
		}
}

void ASpartaGameState::EndWave()
{
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	GetWorldTimerManager().ClearTimer(ItemSpawnTimerHandle); 

	CurrentWaveIndex++;

	if (CurrentWaveIndex < MaxWaves)
	{
		GetWorldTimerManager().SetTimer(
			WaveIntermissionTimerHandle,
			this,
			&ASpartaGameState::StartWave,
			2.0f,
			false
		);
	}
	else
	{
		EndLevel();
	}
}


void ASpartaGameState::EndLevel()
{
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	GetWorldTimerManager().ClearTimer(WaveIntermissionTimerHandle);

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			AddScore(Score);
			CurrentLevelIndex++;
			SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex;

			if (CurrentLevelIndex >= MaxLevels)
			{
				OnGameOver();
				return;
			}

			if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
			{
				UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
			}
			else
			{
				OnGameOver();
			}
		}
	}
}



void ASpartaGameState::OnGameOver()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->SetPause(true);
			SpartaPlayerController->ShowMainMenu(true);
		}
	}
}

void ASpartaGameState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController);
		{
			if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
					RemainingTime = FMath::Max(0.0f, RemainingTime);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), RemainingTime)));
				}

				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
						if (SpartaGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
						}
					}
				}

				if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				{
					LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level %d"), CurrentLevelIndex + 1)));
				}

				if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
				{
					LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Wave %d/%d"), CurrentWaveIndex + 1, MaxWaves)));
				}

				if (UProgressBar* HPBar = Cast<UProgressBar>(HUDWidget->GetWidgetFromName(TEXT("HPBar"))))
				{
					if (ASpartaCharacter* SpartaChar = Cast<ASpartaCharacter>(SpartaPlayerController->GetPawn()))
					{
						HPBar->SetPercent(SpartaChar->GetHealthPercent());
					}
				}

				if (UTextBlock* HPText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("HPText"))))
				{
					if (ASpartaCharacter* SpartaChar = Cast<ASpartaCharacter>(SpartaPlayerController->GetPawn()))
					{
						int32 CurrentHP = FMath::RoundToInt(SpartaChar->GetHealth());
						int32 MaxHP = FMath::RoundToInt(SpartaChar->GetMaxHealth());

						HPText->SetText(FText::FromString(
							FString::Printf(TEXT("%d / %d"), CurrentHP, MaxHP)
						));
					}
				}
			}
		}
	}
}