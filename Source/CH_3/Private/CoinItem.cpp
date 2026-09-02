// Fill out your copyright notice in the Description page of Project Settings.


#include "CoinItem.h"
#include "SpartaCharacter.h"
#include "SpartaGameState.h"

ACoinItem::ACoinItem()
{
    PointValue = 0;
    ItemType = "DefaultCoin";
}

void ACoinItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(Activator))
	{
		if (UWorld* World = GetWorld())
		{
			if (ASpartaGameState* GameState = World->GetGameState<ASpartaGameState>())
			{
				GameState->AddScore(PointValue);
				GameState->OnCoinCollected();
			}
		}
		DestroyItem(); 
	}
}