// Fill out your copyright notice in the Description page of Project Settings.


#include "SetCanMove.h"

#include "TrentinoAltoAdige/Interfaces/GetComponentInterface.h"

void USetCanMove::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                      float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	OwnerRef = Cast<IGetComponentInterface>(MeshComp->GetOwner());
	if (OwnerRef)
		OwnerRef->SetCanMove(false);
}

void USetCanMove::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (OwnerRef)
		OwnerRef->SetCanMove(true);
}
