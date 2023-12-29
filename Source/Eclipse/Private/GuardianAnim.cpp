// Fill out your copyright notice in the Description page of Project Settings.


#include "GuardianAnim.h"

#include "Enemy.h"

void UGuardianAnim::AnimNotify_Fire()
{
	me->GuardianFireProcess();
}
