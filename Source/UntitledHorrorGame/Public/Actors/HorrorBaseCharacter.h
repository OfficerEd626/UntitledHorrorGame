// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HorrorBaseCharacter.generated.h"

UCLASS()
class UNTITLEDHORRORGAME_API AHorrorBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHorrorBaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Everyone has health (Player and Enemy)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Health;

	// A function to handle death (Both die differently, so we make it virtual)
	virtual void Die();
};