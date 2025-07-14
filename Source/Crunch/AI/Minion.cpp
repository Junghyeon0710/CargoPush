// Fill out your copyright notice in the Description page of Project Settings.


#include "Minion.h"


void AMinion::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	Super::SetGenericTeamId(NewTeamID);
	PicSkinBasedOnTeamID();
}

void AMinion::PicSkinBasedOnTeamID()
{
	USkeletalMesh** Skin = SkinMap.Find(GetGenericTeamId());
	if (Skin)
	{
		GetMesh()->SetSkeletalMesh(*Skin);
	}
}

void AMinion::OnRep_TeamID()
{
	PicSkinBasedOnTeamID();
}


