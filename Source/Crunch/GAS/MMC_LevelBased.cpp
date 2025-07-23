// Fill out your copyright notice in the Description page of Project Settings.


#include "MMC_LevelBased.h"

#include "CHeroAttributeSet.h"

UMMC_LevelBased::UMMC_LevelBased()
{
	LevelCaptureDefinition.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	LevelCaptureDefinition.AttributeToCapture = UCHeroAttributeSet::GetLevelAttribute();

	RelevantAttributesToCapture.Add(LevelCaptureDefinition);
}

float UMMC_LevelBased::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	UAbilitySystemComponent* ASC = Spec.GetContext().GetInstigatorAbilitySystemComponent();
	if (!ASC)
	{
		return 0.f;
	}

	float Level = 0;
	FAggregatorEvaluateParameters EvalParms;
	EvalParms.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvalParms.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	GetCapturedAttributeMagnitude(LevelCaptureDefinition, Spec, EvalParms, Level);

	bool bFound;
	float RateAttributeVal = ASC->GetGameplayAttributeValue(RateAttribute, bFound);
	if (!bFound)
	{
		return 0.f;
	}

	return (Level -1 ) * RateAttributeVal;
}
