// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USphereComponent;
class AInstintsCharacter;

UCLASS()
class INSTINTS_API AItem : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USphereComponent* SphereComponent;
public:	
	AItem();
	void EnableItem();
	void DisableItem();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintCallable)
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);



public:
	UFUNCTION(BlueprintCallable)
	void DisableCollision();
	bool bHasBeenPickedUp = false;

protected:
	AInstintsCharacter* Character;

public:	

	virtual void Tick(float DeltaTime) override;

};
