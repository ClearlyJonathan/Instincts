// Copyright Epic Games, Inc. All Rights Reserved.

#include "InstintsCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "GunObject.h"

#include "InputActionValue.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AInstintsCharacter::AInstintsCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AInstintsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AInstintsCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AInstintsCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AInstintsCharacter::Look);

		// Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AInstintsCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AInstintsCharacter::Unsprint);


		// Actions
		EnhancedInputComponent->BindAction(PickupAction, ETriggerEvent::Started, this, &AInstintsCharacter::Pickup);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &AInstintsCharacter::Equip);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AInstintsCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AInstintsCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AInstintsCharacter::Sprint()
{
	// Set walk speed 
	GetCharacterMovement()->MaxWalkSpeed = 500;
}

void AInstintsCharacter::Unsprint()
{
	GetCharacterMovement()->MaxWalkSpeed = 200;
}

void AInstintsCharacter::Equip()
{
	bHasWeaponEquipped = !bHasWeaponEquipped;
	UE_LOG(LogTemp, Warning, TEXT("Weapon equipped: %s"), bHasWeaponEquipped ? TEXT("True") : TEXT("False"));

	if (CurrentGun)
	{
		if (bHasWeaponEquipped)
		{
		
			// TODO: Return to normal animation after equipping
			// GetMesh()->PlayAnimation(EquipAnimation, false);
			CurrentGun->EnableItem();
		}
		else
		{
			CurrentGun->DisableItem();
		}
	}
	else
	{
		// BUG: The gun sometimes causes the camera to clip to the player
		for (AItem* Item : PlayerItems)
		{
			if (Item->ActorHasTag(FName("Shotgun")))
			{
				CurrentGun = Cast<AGunObject>(Item);
				FAttachmentTransformRules TransformRules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
				CurrentGun->AttachToComponent(GetMesh(), TransformRules, FName("WeaponSocket"));
				
				CurrentGun->EnableItem();
				bHasWeaponEquipped = true;
			}
		}
	}
	


}

void AInstintsCharacter::Pickup()
{

	if (OverlappingItem == nullptr) return;
	AddItemToPlayer(OverlappingItem);
}

void AInstintsCharacter::DisplayPickupUi(bool bState)
{
	if (bState)
	{
		PickupWidget->AddToViewport(0);
	}
	else
	{
		PickupWidget->RemoveFromParent();
	}
}

void AInstintsCharacter::AddItemToPlayer(AItem* Item)
{
	
	if (PlayerItems.Find(Item) == INDEX_NONE)
	{
		PlayerItems.Add(Item);
		Item->bHasBeenPickedUp = true;
		Item->DisableItem();
		ResetOverlappingItem();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Item has already been registered"));
	}
}	

void AInstintsCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AInstintsCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AInstintsCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AInstintsCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}
