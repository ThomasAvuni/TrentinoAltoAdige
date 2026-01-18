// Fill out your copyright notice in the Description page of Project Settings.


#include "PerfectParryWindow.h"

#include "TrentinoAltoAdige/Components/CombatSystemComponent.h"
#include "TrentinoAltoAdige/Interfaces/GetComponentInterface.h"

void UPerfectParryWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                      float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	OwnerRef = Cast<IGetComponentInterface>(MeshComp->GetOwner());
}

void UPerfectParryWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
