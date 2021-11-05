// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacter.h"
#include "ShootTargetProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "MotionControllerComponent.h"


DEFINE_LOG_CATEGORY_STATIC(FPSLog, Warning, All);

// Sets default values
AFPSCharacter::AFPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	//create camera component
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // relate position
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	//create mesh to hang gun
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	//create gun component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	FP_Gun->SetupAttachment(FirstPersonCameraComponent);

	GunOffset = FVector(100.0f, 0.0f, 10.0f);
}

void AFPSCharacter::print(FString s)
{
	if (debugMode)
		return;
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, s);
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();
}

bool AFPSCharacter::EnableTouchscreenMovement(UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch) {
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AFPSCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AFPSCharacter::EndTouch);
		return true;
	}
	return false;
}

void AFPSCharacter::OnFire()
{
	PlusFireCount();
	if (ProjectileClass != nullptr) 
	{
		UWorld* const World = GetWorld();
		if (World != nullptr) {
			const FRotator rotator = GetControlRotation();
			const FRotator SpawnRotation = UKismetMathLibrary::MakeRotator(rotator.Roll + 130, rotator.Pitch - 5, rotator.Yaw - 3);
			const FVector location = GetActorLocation();
			const FVector SpawnLocation = location + FVector(0, 0, 12) + SpawnRotation.RotateVector(GunOffset);
			//print(FString(SpawnRotation.ToString()));
			//print(FString(SpawnLocation.ToString()));
			//if(FP_MuzzleLocation != nullptr){
			//	//FP_MuzzleLocation->GetComponentLocation()
			//}
			//else
			//{
			//	SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(GunOffset);;
			//}

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			// spawn the projectile at the muzzle
			World->SpawnActor<AShootTargetProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}

		if (FireSound != nullptr) {
			if(EnableFireSound)
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			else
				print(FString::Printf(TEXT("FileSound is disabled!")));
		}
		else {
			print(FString::Printf(TEXT("FileSound is nullptr")));
		}

		if (FireAnimation != nullptr)
		{
			// Get the animation object for the arms mesh
			// TODO
			/*UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
			if (AnimInstance != nullptr)
			{
				AnimInstance->Montage_Play(FireAnimation, 1.f);
			}*/
		}
		else
		{
			//TODO
			//print(FString::Printf(TEXT("FileAnimation is nullptr")));
		}
	}
	else {
		print(FString::Printf(TEXT("Projectile is nullptr")));
	}
}

void AFPSCharacter::MoveForward(float Val)
{
	if (Val != 0.0f)
		AddMovementInput(GetActorForwardVector(), Val);

}

void AFPSCharacter::MoveRight(float Val)
{
	if(Val != 0.0f)
		AddMovementInput(GetActorRightVector(), Val);
}

void AFPSCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AFPSCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AFPSCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AFPSCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
		return;
	TouchItem.bIsPressed = false;
}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Enable touchscreen input for android
	EnableTouchscreenMovement(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacter::OnFire);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);

	// for what? PC, ...
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFPSCharacter::TurnAtRate);
	// for android analog joystick
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFPSCharacter::LookUpAtRate);
}

