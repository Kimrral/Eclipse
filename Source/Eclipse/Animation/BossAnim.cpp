// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Animation/BossAnim.h"

void UBossAnim::AnimNotify_MontageEnd()
{
	MontageSectionFinishedDelegate.ExecuteIfBound();
}
