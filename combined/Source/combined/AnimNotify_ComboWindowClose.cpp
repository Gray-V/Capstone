// AnimNotify_ComboWindowClose.cpp
// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotify_ComboWindowClose.h"
#include "CombatComponent.h"

void UAnimNotify_ComboWindowClose::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		UCombatComponent* CombatComp = MeshComp->GetOwner()->FindComponentByClass<UCombatComponent>();
		if (CombatComp)
		{
			CombatComp->SetComboWindowOpen(false);
		}
	}
}
