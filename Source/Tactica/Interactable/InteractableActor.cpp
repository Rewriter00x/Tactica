#include "InteractableActor.h"

#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

AInteractableActor::AInteractableActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetSphereRadius(200.f);
	SetRootComponent(Sphere);

	bReplicates = true;
}

void AInteractableActor::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AInteractableActor::OnSphereBeginOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AInteractableActor::OnSphereEndOverlap);
}

void AInteractableActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Sphere->OnComponentBeginOverlap.RemoveDynamic(this, &AInteractableActor::OnSphereBeginOverlap);
	Sphere->OnComponentEndOverlap.RemoveDynamic(this, &AInteractableActor::OnSphereEndOverlap);
	
	Super::EndPlay(EndPlayReason);
}

void AInteractableActor::BeginInteract(AActor* InteractingActor)
{
	K2_BeginInteract(InteractingActor);
}

void AInteractableActor::EndInteract(AActor* InteractingActor)
{
	K2_EndInteract(InteractingActor);
}

void AInteractableActor::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (const APawn* Pawn = Cast<APawn>(OtherActor))
	{
		if (OtherComp->IsA<UCapsuleComponent>() && Pawn->GetController() && Pawn->GetController()->IsPlayerController())
		{
			BeginInteract(OtherActor);
		}
	}
}

void AInteractableActor::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (const APawn* Pawn = Cast<APawn>(OtherActor))
	{
		if (OtherComp->IsA<UCapsuleComponent>() && Pawn->GetController() && Pawn->GetController()->IsPlayerController())
		{
			EndInteract(OtherActor);
		}
	}
}

