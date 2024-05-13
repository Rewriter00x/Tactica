#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TacticaPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;

UCLASS(Abstract)
class TACTICA_API ATacticaPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void OnPossess(APawn* NewPawn) override;
	virtual void OnUnPossess() override;

private:
	UFUNCTION()
	void OnPossessedPawnDestroyed(AActor* DestroyedPawn);

	void ExitToMenu();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Menu, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UWorld> MenuLevel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* EscapeMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EscapeAction;
	
};
