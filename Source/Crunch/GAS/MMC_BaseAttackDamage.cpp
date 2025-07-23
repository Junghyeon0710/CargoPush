﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "MMC_BaseAttackDamage.h"

#include "CAttributeSet.h"

UMMC_BaseAttackDamage::UMMC_BaseAttackDamage()
{
	DamageCaptureDef.AttributeToCapture = UCAttributeSet::GetAttackDamageAttribute();
	DamageCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;

	ArmorCaptureDef.AttributeToCapture = UCAttributeSet::GetArmorAttribute();
	ArmorCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;

	RelevantAttributesToCapture.Add(DamageCaptureDef);
	RelevantAttributesToCapture.Add(ArmorCaptureDef);
	
}

float UMMC_BaseAttackDamage::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{

	FAggregatorEvaluateParameters EvalParms;
	EvalParms.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvalParms.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float AttackDamage = 0.f;
	GetCapturedAttributeMagnitude(DamageCaptureDef, Spec, EvalParms, AttackDamage);

	float Armor = 0.f;
	GetCapturedAttributeMagnitude(ArmorCaptureDef, Spec, EvalParms, Armor);

	float Damage = AttackDamage * (1- Armor / (Armor +100));

	return -Damage;
}
