// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Enemy/Drone.h"

#include "Eclipse/AI/EnemyFSM.h"

ADrone::ADrone()
{
	// Enemy FSM
	EnemyFSM = CreateDefaultSubobject<UEnemyFSM>(TEXT("EnemyFSM"));
}
