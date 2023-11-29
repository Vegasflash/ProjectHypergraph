// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericRunnable.h"
#include "ThreadActor.h"

/**
 * 
 */
class HYPERGRAPH_API FActorRunnable : public FGenericRunnable
{
public:
	FActorRunnable(int32 _Calculations, AThreadActor* Actor);

	virtual ~FActorRunnable() override;


	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

private:

	bool bStopThread;
	int32 Calculations;
	int32 CalcCount;

	AThreadActor* CurrentThreadActor;
	int32 CurrentCalculation;
};
