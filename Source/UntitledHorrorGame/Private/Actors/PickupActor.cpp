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

    MeshComp->SetMobility(EComponentMobility::Movable);
}

// Called when the game starts or when spawned
void APickupActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void APickupActor::Interact_Implementation(APawn* InstigatorPawn)
{
    if (HasAuthority())
    {
        // 1. Set the Holder (Triggers OnRep on Client)
        CurrentHolder = InstigatorPawn;
        OnRep_CurrentHolder(); // Run manually on Server

        // Note: We don't even need the Attach code here anymore 
        // because OnRep_CurrentHolder does it for us!
    }
}

void APickupActor::OnRep_CurrentHolder()
{
    if (CurrentHolder)
    {
        // --- PICKED UP ---
        // 1. Stop Physics immediately
        MeshComp->SetSimulatePhysics(false);
        MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        // 2. FORCE ATTACHMENT ON CLIENT (The Fix)
        // We don't wait for the server. We snap it ourselves right now.
        USceneComponent* PawnMesh = CurrentHolder->FindComponentByClass<USkeletalMeshComponent>();
        if (PawnMesh)
        {
            FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
            this->AttachToComponent(PawnMesh, AttachRules, FName("hand_r"));
        }
    }
    else
    {
        // --- DROPPED ---
        // 1. Detach
        DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

        // 2. Restart Physics
        MeshComp->SetSimulatePhysics(true);
        MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }
}

void APickupActor::Drop_Implementation(APawn* InstigatorPawn)
{
    if (HasAuthority())
    {
        // 1. Clear the Holder
        CurrentHolder = nullptr;
        OnRep_CurrentHolder(); // Run manually on Server to detach

        // 2. The Toss
        if (InstigatorPawn)
        {
            FVector Forward = InstigatorPawn->GetActorForwardVector();
            FVector ThrowImpulse = (Forward * 500.0f) + FVector(0.0f, 0.0f, 200.0f);
            MeshComp->AddImpulse(ThrowImpulse, NAME_None, true);
        }
    }
}

void APickupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // Replicate the Pawn reference instead of the bool
    DOREPLIFETIME(APickupActor, CurrentHolder);
}