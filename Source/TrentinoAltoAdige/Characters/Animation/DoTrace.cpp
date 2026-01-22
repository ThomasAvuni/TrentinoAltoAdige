// Fill out your copyright notice in the Description page of Project Settings.


#include "DoTrace.h"

#include "TrentinoAltoAdige/Components/CombatSystemComponent.h"
#include "TrentinoAltoAdige/Interfaces/GetComponentInterface.h"

void UDoTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                           const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	OwnerRef = Cast<IGetComponentInterface>(MeshComp->GetOwner());

}

void UDoTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (OwnerRef)
	{
		OwnerRef->GetCombatSystemComponent()->ResetEnemiesHitThisAttack();
	}
}

void UDoTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
                          const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (OwnerRef)
	{
		OwnerRef->GetCombatSystemComponent()->PerformTrace();
	}
}
