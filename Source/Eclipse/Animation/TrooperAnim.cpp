// Fill out your copyright notice in the Description page of Project Settings.


#include "TrooperAnim.h"

#include "Eclipse/Enemy/Enemy.h"

void UTrooperAnim::AnimNotify_Fire() const
{
	me->FireProcess();
}
