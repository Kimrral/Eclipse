// Fill out your copyright notice in the Description page of Project Settings.


#include "CrunchAnim.h"

#include "Enemy.h"
#include "Kismet/GameplayStatics.h"

void UCrunchAnim::AnimNotify_AttackStart()
{
	Super::AnimNotify_AttackStart();

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), punchSoundNoHit, me->GetActorLocation());
}
