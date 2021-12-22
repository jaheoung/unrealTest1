// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

AWeapon::AWeapon()
{
	myMesh = CreateDefaultSubobject<UStaticMeshComponent>("myMesh");

	RootComponent = myMesh;

	/*auto meshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/MyResource/Weapon/testWeapon.testWeapon'"));

	if (meshAsset.Object != nullptr)
	{
		myMesh->SetStaticMesh(meshAsset.Object);
	}*/
}

void AWeapon::SetWeapon(WEAPON_TYPE _type)
{
	this->type = _type;

	FString path = "";

	switch (type)
	{
	case WEAPON_TYPE::SINGLE_SWORD:
		path = TEXT("StaticMesh'/Game/MyResource/Weapon/testWeapon.testWeapon'");
		break;
	case WEAPON_TYPE::SINGLE_GUN:
		path = TEXT("StaticMesh'/Game/MyResource/Weapon/singleGun.singleGun'");
		break;
	default:
		return;
	}

	UStaticMesh* loadMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *path));

	if (loadMesh != nullptr && myMesh != nullptr)
	{
		myMesh->SetStaticMesh(loadMesh);
	}
}

WEAPON_TYPE AWeapon::GetWeaponType()
{
	return type;
}
