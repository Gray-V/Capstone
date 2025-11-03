// AnimNotify_ComboWindowOpen.cpp
// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotify_ComboWindowOpen.h"
#include "CombatComponent.h"

void UAnimNotify_ComboWindowOpen::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		UCombatComponent* CombatComp = MeshComp->GetOwner()->FindComponentByClass<UCombatComponent>();
		if (CombatComp)
		{
			CombatComp->SetComboWindowOpen(true);
		}
	}
}
