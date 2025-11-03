// CombatComponent.cpp
#include "CombatComponent.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

UCombatComponent::UCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    
    bIsAttacking = false;
    CurrentComboIndex = 0;
    bComboWindowOpen = false;
    bShouldContinueCombo = false;
    CurrentAttackType = EAttackType::Light;
}

void UCombatComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

UAnimInstance* UCombatComponent::GetOwnerAnimInstance() const
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (Character && Character->GetMesh())
    {
        return Character->GetMesh()->GetAnimInstance();
    }
    return nullptr;
}

void UCombatComponent::LightAttack()
{
    // If we're in a combo window, queue the next attack
    if (bComboWindowOpen && CurrentAttackType == EAttackType::Light)
    {
        bShouldContinueCombo = true;
    }
    // If we're not attacking, start a new combo
    else if (!bIsAttacking)
    {
        CurrentComboIndex = 0;
        CurrentAttackType = EAttackType::Light;
        PerformLightAttack();
    }
}

void UCombatComponent::PerformLightAttack()
{
    if (LightAttackMontages.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("CombatComponent: No light attack montages assigned!"));
        return;
    }

    if (!LightAttackMontages.IsValidIndex(CurrentComboIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("CombatComponent: Invalid combo index %d"), CurrentComboIndex);
        CurrentComboIndex = 0;
    }

    UAnimMontage* MontageToPlay = LightAttackMontages[CurrentComboIndex];
    if (MontageToPlay)
    {
        PlayAttackMontage(MontageToPlay);
    }
}

void UCombatComponent::HeavyAttack()
{
    // Heavy attacks can interrupt or start fresh
    if (!bIsAttacking || bComboWindowOpen)
    {
        CurrentComboIndex = 0;
        CurrentAttackType = EAttackType::Heavy;
        PerformHeavyAttack();
    }
}

void UCombatComponent::PerformHeavyAttack()
{
    if (HeavyAttackMontages.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("CombatComponent: No heavy attack montages assigned!"));
        return;
    }

    // For now, just use the first heavy attack. You can expand this for heavy combos too
    int32 HeavyIndex = FMath::Min(CurrentComboIndex, HeavyAttackMontages.Num() - 1);
    UAnimMontage* MontageToPlay = HeavyAttackMontages[HeavyIndex];
    
    if (MontageToPlay)
    {
        PlayAttackMontage(MontageToPlay);
    }
}

void UCombatComponent::PlayAttackMontage(UAnimMontage* Montage)
{
    UAnimInstance* AnimInstance = GetOwnerAnimInstance();
    if (!AnimInstance || !Montage)
    {
        UE_LOG(LogTemp, Warning, TEXT("CombatComponent: Cannot play montage - AnimInstance or Montage is null"));
        return;
    }

    // Stop any currently playing montage
    AnimInstance->Montage_Stop(MontageBlendOutTime);
    
    // Play the attack montage
    AnimInstance->Montage_Play(Montage, MontagePlayRate);
    
    bIsAttacking = true;
    bShouldContinueCombo = false;
    
    // Bind to montage end
    FOnMontageEnded MontageEndedDelegate;
    MontageEndedDelegate.BindUObject(this, &UCombatComponent::OnMontageEnded);
    AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, Montage);
    
    UE_LOG(LogTemp, Log, TEXT("CombatComponent: Playing montage - Combo Index: %d"), CurrentComboIndex);
}

void UCombatComponent::SetComboWindowOpen(bool bOpen)
{
    bComboWindowOpen = bOpen;
    
    UE_LOG(LogTemp, Log, TEXT("CombatComponent: Combo window %s - Should continue: %s"), 
           bOpen ? TEXT("OPENED") : TEXT("CLOSED"),
           bShouldContinueCombo ? TEXT("YES") : TEXT("NO"));
    
    // When combo window opens, check if player wants to continue
    if (bOpen && bShouldContinueCombo)
    {
        CurrentComboIndex++;
        
        // Handle combo wrapping based on attack type
        if (CurrentAttackType == EAttackType::Light)
        {
            if (CurrentComboIndex >= MaxLightComboCount || CurrentComboIndex >= LightAttackMontages.Num())
            {
                CurrentComboIndex = 0;
            }
            PerformLightAttack();
        }
        else // Heavy
        {
            if (CurrentComboIndex >= HeavyAttackMontages.Num())
            {
                CurrentComboIndex = 0;
            }
            PerformHeavyAttack();
        }
    }
}

void UCombatComponent::OnAttackEnd()
{
    // Optional: Called from an anim notify at the very end of an attack
    // if you want explicit control over when attacks end
    if (!bShouldContinueCombo && !bComboWindowOpen)
    {
        ResetCombo();
    }
}

void UCombatComponent::ResetCombo()
{
    UE_LOG(LogTemp, Log, TEXT("CombatComponent: Resetting combo"));
    
    bIsAttacking = false;
    CurrentComboIndex = 0;
    bComboWindowOpen = false;
    bShouldContinueCombo = false;
}

void UCombatComponent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    UE_LOG(LogTemp, Log, TEXT("CombatComponent: Montage ended - Interrupted: %s"), 
           bInterrupted ? TEXT("YES") : TEXT("NO"));
    
    // If the combo didn't continue and we're done, reset everything
    if (!bShouldContinueCombo)
    {
        ResetCombo();
    }
}