#include "ItemBase.h"
#include "SpartaCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AItemBase::AItemBase()
{
    PrimaryActorTick.bCanEverTick = false;

    Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
    SetRootComponent(Scene);
    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    Collision->SetupAttachment(Scene);

    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    StaticMesh->SetupAttachment(Collision);

    Collision->OnComponentBeginOverlap.AddDynamic(this, &AItemBase::OnItemOverlap);
    Collision->OnComponentEndOverlap.AddDynamic(this, &AItemBase::OnItemEndOverlap);
}

void AItemBase::OnItemOverlap(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{

	if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(OtherActor))
	{


		ActivateItem(PlayerCharacter);
	}

}

void AItemBase::OnItemEndOverlap(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
}

void AItemBase::ActivateItem(AActor* Activator)
{
   UParticleSystemComponent* Particle = nullptr;
	
		if (PickupParticle)
		{
				Particle = UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					PickupParticle,
					GetActorLocation(),
					GetActorRotation(),
					true
				);
		}

		if (PickupSound)
		{
			 UGameplayStatics::PlaySoundAtLocation(
				GetWorld(),
				PickupSound,
				GetActorLocation()
			);
		}

	
		if (Particle)
		{
				FTimerHandle DestroyParticleTimerHandle;
						
				GetWorld()->GetTimerManager().SetTimer(
					DestroyParticleTimerHandle,
					[Particle]()
					{
						Particle->DestroyComponent();		
					},
					2.0f,
					false
				);
		}
}

FName AItemBase::GetItemType() const
{
    return ItemType;
}

void AItemBase::DestroyItem()
{
    Destroy();
}