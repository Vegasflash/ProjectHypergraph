// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "HAL/Runnable.h"

DECLARE_DELEGATE(OnRunnableCompletion)

class HYPERGRAPH_API FGenericRunnable : public FRunnable
{

public:
	// Constructor, create the thread by calling this
	FGenericRunnable();

	// Destructor
	virtual ~FGenericRunnable() override;

	//FRunnable interface
	void Begin();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

	OnRunnableCompletion OnCompletion;

protected:
	FRunnableThread* Thread = nullptr;

	bool bStopThread = false;
};
