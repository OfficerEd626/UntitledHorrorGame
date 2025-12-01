// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/HorrorPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

AHorrorPlayer::AHorrorPlayer()
{
    // --- 1. FIRST PERSON CAMERA SETUP ---
    // Look follows mouse
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = false;

    // Strafe Movement (FPS Style)
    GetCharacterMovement()->bOrientRotationToMovement = false;

    // Camera Boom
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetCapsuleComponent());
    CameraBoom->SetRelativeLocation(FVector(20.0f, 0.0f, 65.0f)); // Eye height
    CameraBoom->TargetArmLength = 0.0f;
    CameraBoom->bUsePawnControlRotation = true;

    // Camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;
}


void AHorrorPlayer::MoveForward(float Value)
{
    if ((Controller != nullptr) && (Value != 0.0f))
    {
        // find out which way is forward
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get forward vector
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

void AHorrorPlayer::MoveRight(float Value)
{
    if ((Controller != nullptr) && (Value != 0.0f))
    {
        // find out which way is right
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get right vector 
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        // add movement in that direction
        AddMovementInput(Direction, Value);
    }
}

void AHorrorPlayer::TurnAtRate(float Rate)
{
    // calculate delta for this frame from the rate information
    AddControllerYawInput(Rate * GetWorld()->GetDeltaSeconds() * 45.f); // 45 is Turn Speed
}

void AHorrorPlayer::LookUpAtRate(float Rate)
{
    // calculate delta for this frame from the rate information
    AddControllerPitchInput(Rate * GetWorld()->GetDeltaSeconds() * 45.f);
}

void AHorrorPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Axis Bindings (WASD + Mouse)
    PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AHorrorPlayer::MoveForward);
    PlayerInputComponent->BindAxis("Move Right / Left", this, &AHorrorPlayer::MoveRight);

    // Mouse Look (Turn/LookUp are built-in functions, usually)
    PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);

    // Controller Look (Optional, but good to have)
    PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AHorrorPlayer::TurnAtRate);
    PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AHorrorPlayer::LookUpAtRate);

    // Jump
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

    // Bind Interact/Drop
    PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AHorrorPlayer::PerformInteract);
    PlayerInputComponent->BindAction("Drop", IE_Pressed, this, &AHorrorPlayer::PerformDrop);
}

void AHorrorPlayer::PerformInteract()
{
    // 1. Define the Trace
    FVector Start = FollowCamera->GetComponentLocation();
    FVector End = Start + (FollowCamera->GetForwardVector() * 1500.0f); // 1500 unit reach

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    // 2. Perform Trace
    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

    // Debug Line
    DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f);

    if (bHit && HitResult.GetActor())
    {
        // 3. Check Interface
        if (HitResult.GetActor()->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
        {
            // 4. Ask Server to interact
            ServerInteract(HitResult.GetActor());
        }
    }
}

void AHorrorPlayer::ServerInteract_Implementation(AActor* HitActor)
{
    if (HitActor)
    {
        // If already holding something, ignore
        if (CurrentItem)
        {
            return;
        }

        // Save the item
        CurrentItem = HitActor;

        // Execute the Interface
        IInteractInterface::Execute_Interact(HitActor, this);
    }
}

void AHorrorPlayer::PerformDrop()
{
    ServerDrop();
}

void AHorrorPlayer::ServerDrop_Implementation()
{
    if (CurrentItem)
    {
        // Tell item to drop
        IInteractInterface::Execute_Drop(CurrentItem, this);

        // Forget item
        CurrentItem = nullptr;
    }
}