#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableActor.generated.h"

class USphereComponent;

UCLASS()
class TACTICA_API AInteractableActor : public AActor
{
	GENERATED_BODY()

public:
	AInteractableActor();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintImplementableEvent, DisplayName="Begin Interact")
	void K2_BeginInteract(AActor* InteractingActor);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="End Interact")
	void K2_EndInteract(AActor* InteractingActor);
	
	virtual void BeginInteract(AActor* InteractingActor);
	virtual void EndInteract(AActor* InteractingActor);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> Sphere;

private:
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};
