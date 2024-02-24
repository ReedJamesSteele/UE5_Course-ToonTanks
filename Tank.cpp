// Fill out your copyright notice in the Description page of Project Settings.


#include "Tank.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Kismet/GameplayStatics.h"


ATank::ATank()
{

    PrimaryActorTick.bCanEverTick = true;

    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComp->SetupAttachment(RootComponent);

    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComp->SetupAttachment(SpringArmComp);
}

// Called to bind functionality to input
void ATank::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent); //Call the parent version

	// Get the player controller
    auto playerController = Cast<APlayerController>(GetController());
 
    // Get the local player enhanced input subsystem
    auto eiSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer());
    //Add the input mapping context
    eiSubsystem->AddMappingContext(inputMapping, 0);
 
    // Get the EnhancedInputComponent
    auto playerEIcomponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	//Bind Move() to the mapping
	//BindAction for enhanced system takes Action, ETriggerEvent, object, and function
	//ETriggerEvent is an enum, where Triggered means "button is held down".
	playerEIcomponent->BindAction(inputMoveForward, ETriggerEvent::Triggered, this, &ATank::Move);
    playerEIcomponent->BindAction(inputTurn, ETriggerEvent::Triggered, this, &ATank::Turn);
    playerEIcomponent->BindAction(inputFire, ETriggerEvent::Triggered, this, &ATank::Fire);

}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();

    TankPlayerController = Cast<APlayerController>(GetController());
	
}

// Called every frame
void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (TankPlayerController)
    {
        FHitResult HitResult;
        TankPlayerController->GetHitResultUnderCursor(
            ECollisionChannel::ECC_Visibility,
            false,
            HitResult);

        RotateTurret(HitResult.ImpactPoint);
    
    
    }

}

void ATank::HandleDestruction()
{
    Super::HandleDestruction();
    SetActorHiddenInGame(true);
    SetActorTickEnabled(false);
    bAlive = false;
}

void ATank::Move(const FInputActionValue & Value)
{
	//To bind to axis mapping: SetupPlayerInputComponent
    FVector TankLocation{FVector::ZeroVector};
	TankLocation.X = Value.Get<float>() * Speed * UGameplayStatics::GetWorldDeltaSeconds(this);
    
	AddActorLocalOffset(TankLocation, true);
    UE_LOG(LogTemp, Display, TEXT("Float value: %f"), Value.Get<float>());
}

void ATank::Turn(const FInputActionValue & Value)
{
    FRotator TankRotation{FRotator::ZeroRotator};
    TankRotation.Yaw = Value.Get<float>() * TurnRate * UGameplayStatics::GetWorldDeltaSeconds(this);

    AddActorLocalRotation(TankRotation, true);
    UE_LOG(LogTemp, Display, TEXT("Float value: %f"), Value.Get<float>());

}