// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/HorrorBaseCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

AHorrorBaseCharacter::AHorrorBaseCharacter()
{
    // Set this character to call Tick() every frame.
    PrimaryActorTick.bCanEverTick = true;

    // SHARED MOVEMENT SETTINGS
    // Both enemies and players should generally stay on the ground
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
    GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

    Health = 100.0f;
}

void AHorrorBaseCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void AHorrorBaseCharacter::Die()
{
    // Default death: Fall down (Physics)
    GetMesh()->SetSimulatePhysics(true);
    GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
}

