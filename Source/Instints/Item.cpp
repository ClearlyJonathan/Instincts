// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "InstintsCharacter.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SphereComponent->SetupAttachment(RootComponent);

	SphereComponent->SetSphereRadius(100.0f);

	// Configuring collisions
	SphereComponent->SetCollisionObjectType(ECC_WorldDynamic);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AItem::BeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AItem::EndOverlap);
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void AItem::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsHidden()) return;

	Character = Cast<AInstintsCharacter>(OtherActor);
	// Call Pickup method and pass the item to be picked up
	if (Character != nullptr)
	{
		Character->DisplayPickupUi(true);
		Character->SetOverlappingItem(this);
	
	}
}

void AItem::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Character = Cast<AInstintsCharacter>(OtherActor);
	if (Character != nullptr)
	{
		Character->DisplayPickupUi(false);
		Character->ResetOverlappingItem();
	}

}

void AItem::EnableItem()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
}

void AItem::DisableItem()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


