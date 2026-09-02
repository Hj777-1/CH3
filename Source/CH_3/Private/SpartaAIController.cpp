// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaAIController.h"

ASpartaAIController::ASpartaAIController()
{
	// 기본 생성자 초기화 로직 (필요 시 작성)
}

void ASpartaAIController::OnPossess(APawn* Inpawn)
{
	Super::OnPossess(Inpawn); //부모클래스가 원래 실행하던 기본 로직을 먼저 실행
	if (Inpawn) // 조건이 맞으면 
	{
	}

}
