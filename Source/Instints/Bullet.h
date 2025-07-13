// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

class AInstintsCharacter;


UCLASS()
class INSTINTS_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	ABullet();
	virtual void Tick(float DeltaTime) override;


	void Move(float DeltaTime);
protected:
	virtual void BeginPlay() override;


private:
	float Speed = 500;
	AInstintsCharacter* Character;

	FVector ForwardVector;
};
