// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintPlatformLibrary.h"
#include "FPSGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTTARGET_API UFPSGameInstance : public UPlatformGameInstance
{
	GENERATED_BODY()

public:
	/** Enable fire sound */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int EnableFireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int EnableFireAnimation;
};
