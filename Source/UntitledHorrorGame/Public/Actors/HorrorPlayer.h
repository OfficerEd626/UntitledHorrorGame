// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/HorrorBaseCharacter.h"
#include "Interfaces/InteractInterface.h"
#include "HorrorPlayer.generated.h"

/**
 * 
 */
UCLASS()
class UNTITLEDHORRORGAME_API AHorrorPlayer : public AHorrorBaseCharacter
{
	GENERATED_BODY()
	
public:
    AHorrorPlayer();

protected:
    // --- CAMERA (Player Only) ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    class USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    class UCameraComponent* FollowCamera;

    // --- INTERACTION (Player Only) ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
    AActor* CurrentItem;

    void PerformInteract();

    UFUNCTION(Server, Reliable)
    void ServerInteract(AActor* HitActor);

    void PerformDrop();

    UFUNCTION(Server, Reliable)
    void ServerDrop();

    /** Called for forwards/backward input */
    void MoveForward(float Value);

    /** Called for side to side input */
    void MoveRight(float Value);

    /** Called via input to turn at a given rate. */
    void TurnAtRate(float Rate);

    /** Called via input to turn look up/down at a given rate. */
    void LookUpAtRate(float Rate);

public:
    // Override SetupPlayerInput (Enemies don't need this!)
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};