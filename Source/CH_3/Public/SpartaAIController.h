// Fill out your copyright notice in the Description page of Project Settings.

#pragma once// 헤더 파일이 한 번만 포함되도록 막아주는 전처리

#include "CoreMinimal.h" // 언리얼에서 자주 사용하는 기본 타입과 매크로를 사용하기 위한 헤더
#include "AIController.h" // AI 전용 컨트롤러 클래스인 AAIController를 사용하기 위한 헤더
#include "SpartaAIController.generated.h" // UHT 생성한 코드를 포함

/**
 * 
 */
UCLASS() // C++ 클래스를 언리얼의 리플렉션 시스템이 이식하도록 등록 
class CH_3_API ASpartaAIController : public AAIController
{
	GENERATED_BODY()

public:
	ASpartaAIController(); //생성자

protected:

	virtual void OnPossess(APawn* Inpawn) override;
	// AIController가 Pawn을 조종하기 시작할 때 호출되는 함수를 재정의 하겠다고 선언
	
};
