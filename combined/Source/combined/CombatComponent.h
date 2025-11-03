// CombatComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

UENUM(BlueprintType)
enum class EAttackType : uint8
{
    Light,
    Heavy
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COMBINED_API UCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCombatComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Attack Functions
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void LightAttack();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void HeavyAttack();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void ResetCombo();

    // Called from Animation Notifies
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void SetComboWindowOpen(bool bOpen);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void OnAttackEnd();

protected:
    // Attack Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Light Attack")
    TArray<UAnimMontage*> LightAttackMontages;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Heavy Attack")
    TArray<UAnimMontage*> HeavyAttackMontages;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Settings")
    int32 MaxLightComboCount = 4;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Settings")
    float MontagePlayRate = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Settings")
    float MontageBlendOutTime = 0.2f;

    // Combat State
    UPROPERTY(BlueprintReadOnly, Category = "Combat|State")
    bool bIsAttacking;

    UPROPERTY(BlueprintReadOnly, Category = "Combat|State")
    int32 CurrentComboIndex;

    UPROPERTY(BlueprintReadOnly, Category = "Combat|State")
    bool bComboWindowOpen;

    UPROPERTY(BlueprintReadOnly, Category = "Combat|State")
    bool bShouldContinueCombo;

    UPROPERTY(BlueprintReadOnly, Category = "Combat|State")
    EAttackType CurrentAttackType;

private:
    void PerformLightAttack();
    void PerformHeavyAttack();
    void PlayAttackMontage(UAnimMontage* Montage);
    UAnimInstance* GetOwnerAnimInstance() const;
    
    UFUNCTION()
    void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};