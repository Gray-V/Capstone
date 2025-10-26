// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponEquipmentComponent.h"
#include "WeaponBase.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values for this component's properties
UWeaponEquipmentComponent::UWeaponEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Default socket name - customize this to match your skeleton
	WeaponSocketName = FName("hand_r_socket");
	CurrentWeapon = nullptr;
	DefaultWeapon = nullptr;
	bAutoEquipDefaultWeapon = false;
}

// Called when the game starts
void UWeaponEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	// Get reference to the character's mesh
	if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
	{
		CharacterMesh = OwnerCharacter->GetMesh();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponEquipmentComponent: Owner is not a Character!"));
	}

	// Spawn weapons from classes if any are set
	SpawnWeaponsFromClasses();

	// Auto-equip default weapon if enabled
	if (bAutoEquipDefaultWeapon && DefaultWeapon)
	{
		EquipWeapon(DefaultWeapon);
	}
	// Or try to equip the first weapon in inventory
	else if (bAutoEquipDefaultWeapon && WeaponInventory.Num() > 0)
	{
		EquipWeapon(WeaponInventory[0]);
	}
}

// ========== EASY BLUEPRINT FUNCTIONS ==========

void UWeaponEquipmentComponent::ToggleEquipWeapon()
{
	if (IsWeaponEquipped())
	{
		// Weapon is equipped, so unequip it
		UnequipWeapon();
	}
	else
	{
		// No weapon equipped, try to equip something
		if (DefaultWeapon)
		{
			EquipWeapon(DefaultWeapon);
		}
		else if (WeaponInventory.Num() > 0)
		{
			// Equip first weapon in inventory
			EquipWeapon(WeaponInventory[0]);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ToggleEquipWeapon: No weapon available to equip!"));
		}
	}
}

void UWeaponEquipmentComponent::EquipWeaponByIndex(int32 Index)
{
	// Check if index is valid
	if (Index < 0 || Index >= WeaponInventory.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipWeaponByIndex: Invalid index %d (Inventory size: %d)"), Index, WeaponInventory.Num());
		return;
	}

	// Get weapon at index
	AWeaponBase* WeaponToEquip = WeaponInventory[Index];
	
	if (WeaponToEquip)
	{
		EquipWeapon(WeaponToEquip);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipWeaponByIndex: Weapon at index %d is null!"), Index);
	}
}

// ========== WEAPON INVENTORY ==========

void UWeaponEquipmentComponent::AddWeaponToInventory(AWeaponBase* Weapon)
{
	if (Weapon)
	{
		WeaponInventory.Add(Weapon);
		UE_LOG(LogTemp, Log, TEXT("Added weapon to inventory: %s (Total: %d)"), *Weapon->GetName(), WeaponInventory.Num());
	}
}

void UWeaponEquipmentComponent::RemoveWeaponFromInventory(int32 Index)
{
	if (Index >= 0 && Index < WeaponInventory.Num())
	{
		AWeaponBase* RemovedWeapon = WeaponInventory[Index];
		
		// If this weapon is currently equipped, unequip it first
		if (RemovedWeapon == CurrentWeapon)
		{
			UnequipWeapon();
		}
		
		WeaponInventory.RemoveAt(Index);
		UE_LOG(LogTemp, Log, TEXT("Removed weapon from inventory at index %d"), Index);
	}
}

AWeaponBase* UWeaponEquipmentComponent::GetWeaponFromInventory(int32 Index) const
{
	if (Index >= 0 && Index < WeaponInventory.Num())
	{
		return WeaponInventory[Index];
	}
	return nullptr;
}

// ========== STANDARD FUNCTIONS ==========

bool UWeaponEquipmentComponent::EquipWeapon(AWeaponBase* NewWeapon)
{
	// Validation checks
	if (!NewWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipWeapon: NewWeapon is null!"));
		return false;
	}

	if (!CharacterMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("EquipWeapon: CharacterMesh is null!"));
		return false;
	}

	// Check if socket exists
	if (!CharacterMesh->DoesSocketExist(WeaponSocketName))
	{
		UE_LOG(LogTemp, Error, TEXT("EquipWeapon: Socket '%s' does not exist on mesh!"), *WeaponSocketName.ToString());
		return false;
	}

	// Don't re-equip the same weapon
	if (CurrentWeapon == NewWeapon)
	{
		UE_LOG(LogTemp, Log, TEXT("EquipWeapon: Weapon already equipped!"));
		return true;
	}

	// Unequip current weapon if one is equipped
	if (CurrentWeapon)
	{
		UnequipWeapon();
	}

	// Attach the new weapon
	if (AttachWeaponToSocket(NewWeapon))
	{
		CurrentWeapon = NewWeapon;
		
		// Fire Blueprint event
		OnWeaponEquipped(CurrentWeapon);
		
		UE_LOG(LogTemp, Log, TEXT("Successfully equipped weapon: %s"), *NewWeapon->GetName());
		return true;
	}

	return false;
}

bool UWeaponEquipmentComponent::UnequipWeapon()
{
	if (!CurrentWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("UnequipWeapon: No weapon currently equipped!"));
		return false;
	}

	AWeaponBase* WeaponToUnequip = CurrentWeapon;
	
	// Detach the weapon
	DetachWeapon(CurrentWeapon);
	
	// Clear the current weapon reference
	CurrentWeapon = nullptr;
	
	// Fire Blueprint event
	OnWeaponUnequipped(WeaponToUnequip);
	
	UE_LOG(LogTemp, Log, TEXT("Successfully unequipped weapon: %s"), *WeaponToUnequip->GetName());
	return true;
}

// ========== PRIVATE HELPER FUNCTIONS ==========

bool UWeaponEquipmentComponent::AttachWeaponToSocket(AWeaponBase* Weapon)
{
	if (!Weapon || !CharacterMesh)
	{
		return false;
	}

	// Set up attachment rules
	FAttachmentTransformRules AttachmentRules(
		EAttachmentRule::SnapToTarget,  // Location
		EAttachmentRule::SnapToTarget,  // Rotation
		EAttachmentRule::KeepWorld,     // Scale
		true                             // Weld simulated bodies
	);

	// Attach to socket
	bool bAttached = Weapon->AttachToComponent(
		CharacterMesh,
		AttachmentRules,
		WeaponSocketName
	);

	if (bAttached)
	{
		// Disable physics if enabled
		if (UPrimitiveComponent* WeaponRoot = Cast<UPrimitiveComponent>(Weapon->GetRootComponent()))
		{
			WeaponRoot->SetSimulatePhysics(false);
			WeaponRoot->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	return bAttached;
}

void UWeaponEquipmentComponent::DetachWeapon(AWeaponBase* Weapon)
{
	if (!Weapon)
	{
		return;
	}

	// Set up detachment rules
	FDetachmentTransformRules DetachmentRules(
		EDetachmentRule::KeepWorld,
		true
	);

	// Detach from parent
	Weapon->DetachFromActor(DetachmentRules);
	
	// Hide the weapon instead of destroying it (keeps it in inventory)
	Weapon->SetActorHiddenInGame(true);
	Weapon->SetActorEnableCollision(false);
}

void UWeaponEquipmentComponent::SpawnWeaponsFromClasses()
{
	if (WeaponClasses.Num() == 0)
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	// Spawn each weapon class and add to inventory
	for (TSubclassOf<AWeaponBase> WeaponClass : WeaponClasses)
	{
		if (WeaponClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = Owner;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AWeaponBase* SpawnedWeapon = GetWorld()->SpawnActor<AWeaponBase>(
				WeaponClass,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				SpawnParams
			);

			if (SpawnedWeapon)
			{
				// Hide weapon initially
				SpawnedWeapon->SetActorHiddenInGame(true);
				SpawnedWeapon->SetActorEnableCollision(false);
				
				// Add to inventory
				AddWeaponToInventory(SpawnedWeapon);
				
				UE_LOG(LogTemp, Log, TEXT("Spawned weapon: %s"), *SpawnedWeapon->GetName());
			}
		}
	}
}