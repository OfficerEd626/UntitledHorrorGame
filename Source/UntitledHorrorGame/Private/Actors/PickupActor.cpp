// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PickupActor.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APickupActor::APickupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;
    SetReplicateMovement(true);

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;

    // Physics settings defaults
    MeshComp->SetSimulatePhysics(true);
    MeshComp->SetCollisionProfileName(TEXT("PhysicsActor"));
}

// Called when the game starts or when spawned
void APickupActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void APickupActor::Interact_Implementation(APawn* InstigatorPawn)
{
    // Only the Server can handle attachment repliction
    if (HasAuthority())
    {
        if (InstigatorPawn)
        {
            // Disable Physics (or it will fight the hand)
            MeshComp->SetSimulatePhysics(false);

            // Attach to the Character's Mesh, specifically the Right Hand
            USceneComponent* PawnMesh = InstigatorPawn->FindComponentByClass<USkeletalMeshComponent>();

            if (PawnMesh)
            {
                // Snap to target keeps the object tight in hand
                FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
                this->AttachToComponent(PawnMesh, AttachRules, FName("hand_r"));
            }
        }
    }
}
