// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponEquipmentComponent.generated.h"

// Forward declarations
class AWeaponBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PLAY_API UWeaponEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponEquipmentComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Main equip weapon function - Blueprint callable
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool EquipWeapon(AWeaponBase* NewWeapon);

	// Unequip current weapon
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool UnequipWeapon();

	// Get currently equipped weapon
	UFUNCTION(BlueprintPure, Category = "Weapon")
	AWeaponBase* GetEquippedWeapon() const { return CurrentWeapon; }

	// Check if a weapon is equipped
	UFUNCTION(BlueprintPure, Category = "Weapon")
	bool IsWeaponEquipped() const { return CurrentWeapon != nullptr; }

protected:
	// Currently equipped weapon
	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	AWeaponBase* CurrentWeapon;

	// Socket name to attach weapon to (e.g., "hand_r_socket")
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FName WeaponSocketName;

	// Reference to the character's mesh component
	UPROPERTY()
	USkeletalMeshComponent* CharacterMesh;

	// Blueprint event that fires when a weapon is equipped
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void OnWeaponEquipped(AWeaponBase* Weapon);

	// Blueprint event that fires when a weapon is unequipped
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void OnWeaponUnequipped(AWeaponBase* Weapon);

private:
	// Helper function to attach weapon to socket
	bool AttachWeaponToSocket(AWeaponBase* Weapon);

	// Helper function to detach weapon
	void DetachWeapon(AWeaponBase* Weapon);
};