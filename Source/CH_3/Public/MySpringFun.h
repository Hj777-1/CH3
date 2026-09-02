#pragma once

#include "CoreMinimal.h"

template<typename T>
class MySpringFun
{
public:
	MySpringFun()
		: Stiffness(200.0f), Damping(10.0f), Mass(1.0f)
		, CurrentValue(T()), TargetValue(T()), Velocity(T()) 
	{
	}

	MySpringFun(float InStiffness, float InDamping, float InMass)
		: Stiffness(InStiffness)
		, Damping(InDamping)
		, Mass(FMath::Max(0.01f, InMass))
		, CurrentValue(T())
		, TargetValue(T())
		, Velocity(T())
	{
	}

public:
	float Stiffness; // 강성 (높을수록 빠르게 목표로 이동, 진동 증가)
	float Damping;   // 감쇠 (높을수록 진동이 줄고 끈적해짐)
	float Mass;      // 질량 (보통 1.0f로 고정)

	T CurrentValue;
	T Velocity;
	T TargetValue;

public:
	void Initialize(T StartValue) {
		CurrentValue = StartValue;
		TargetValue = StartValue;
		Velocity = T();
	}

	void SetPhysicsSettings(float NewStiffness, float NewDamping, float NewMass)
	{
		Stiffness = NewStiffness;
		Damping = NewDamping;
		Mass = FMath::Max(0.01f, NewMass);
	}

	void SetTarget(T NewTarget) {
		TargetValue = NewTarget;
	}


	T Update(float DeltaTime) {
		if (DeltaTime <= 0.0f) return CurrentValue;

		T Displacement = TargetValue - CurrentValue;
		T SpringForce = (Displacement * Stiffness) - (Velocity * Damping);
		T Acceleration = SpringForce / Mass;

		Velocity += Acceleration * DeltaTime;
		CurrentValue += Velocity * DeltaTime;

		return CurrentValue;
	}
};