// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Animation/ArtilleryAnim.h"

#include "Eclipse/Enemy/Enemy.h"

void UArtilleryAnim::AnimNotify_Fire()
{
	if (me->HasAuthority())
	{
		me->GuardianFireProcess();
	}
}
