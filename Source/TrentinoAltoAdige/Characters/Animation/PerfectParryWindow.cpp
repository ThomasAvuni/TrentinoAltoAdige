// Fill out your copyright notice in the Description page of Project Settings.


#include "PerfectParryWindow.h"

#include "TrentinoAltoAdige/Components/CombatSystemComponent.h"
#include "TrentinoAltoAdige/Interfaces/CombatInterface.h"
#include "TrentinoAltoAdige/Interfaces/GetComponentInterface.h"

void UPerfectParryWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                      float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	OwnerRef = Cast<ICombatInterface>(MeshComp->GetOwner());
	if (OwnerRef)
		OwnerRef->GetCombatSystemComponent()->bCanPerfectParry = true;
}

void UPerfectParryWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (OwnerRef)
		OwnerRef->GetCombatSystemComponent()->bCanPerfectParry = false;
}
