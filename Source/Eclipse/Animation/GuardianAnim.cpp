// Fill out your copyright notice in the Description page of Project Settings.


#include "GuardianAnim.h"

#include "Eclipse/Enemy/Enemy.h"

void UGuardianAnim::AnimNotify_Fire()
{
	if (me->HasAuthority())
	{
		me->GuardianFireProcess();
	}
}
