// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnim.generated.h"


// 탄약 정보를 담는 구조체
USTRUCT(BlueprintType)
struct FAmmoInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 MaxAmmo; // 최대 탄약

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 CurAmmo; // 현재 탄약

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 Capacity; // 탄약 용량

	// 생성자
	FAmmoInfo()
		: MaxAmmo(0), CurAmmo(0), Capacity(0) {} // 기본 생성자

	FAmmoInfo(int32 InMaxAmmo, int32 InCurAmmo, int32 InCapacity)
		: MaxAmmo(InMaxAmmo), CurAmmo(InCurAmmo), Capacity(InCapacity) {}
};
/**
 * 
 */
UCLASS()
class ECLIPSE_API UPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class APlayerCharacter* me;

	UFUNCTION()
	void AnimNotify_ReloadStart() const;

	UFUNCTION()
	void AnimNotify_ReloadEnd();

	UFUNCTION()
	void ReloadAmmo(FAmmoInfo& AmmoInfo);

	UFUNCTION()
	void UpdateWeaponEquipState();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPistol = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bZooming = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRifleZooming = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bM249Zooming = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTiltingLeft = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTiltingRight = true;
};
