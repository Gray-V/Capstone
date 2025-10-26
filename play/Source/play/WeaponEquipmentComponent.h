// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponEquipmentComponent.generated.h"

// Forward declarations
class AWeaponBase;
class UInputAction;
struct FInputActionValue;

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
	// ========== EASY BLUEPRINT FUNCTIONS ==========
	
	// Toggle equip/unequip - Just connect your Input Action to this!
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void ToggleEquipWeapon();

	// Equip a specific weapon by index (0, 1, 2, etc.) - Perfect for number keys!
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipWeaponByIndex(int32 Index);

	// ========== STANDARD FUNCTIONS ==========

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

	// ========== WEAPON INVENTORY ==========

	// Add a weapon to the inventory
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void AddWeaponToInventory(AWeaponBase* Weapon);

	// Remove a weapon from inventory
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void RemoveWeaponFromInventory(int32 Index);

	// Get weapon from inventory by index
	UFUNCTION(BlueprintPure, Category = "Weapon")
	AWeaponBase* GetWeaponFromInventory(int32 Index) const;

	// Get total number of weapons in inventory
	UFUNCTION(BlueprintPure, Category = "Weapon")
	int32 GetInventorySize() const { return WeaponInventory.Num(); }

public:
	// ========== EDITABLE PROPERTIES ==========

	// Default weapon to equip on start (optional)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	AWeaponBase* DefaultWeapon;

	// Array of weapons available to the character (set in editor or blueprint)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TArray<TSubclassOf<AWeaponBase>> WeaponClasses;

	// Auto-equip default weapon on BeginPlay?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool bAutoEquipDefaultWeapon;

	// Socket name to attach weapon to (e.g., "hand_r_socket")
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FName WeaponSocketName;

protected:
	// Currently equipped weapon
	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	AWeaponBase* CurrentWeapon;

	// Weapon inventory (spawned weapons)
	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	TArray<AWeaponBase*> WeaponInventory;

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

	// Spawn weapons from classes on begin play
	void SpawnWeaponsFromClasses();
};