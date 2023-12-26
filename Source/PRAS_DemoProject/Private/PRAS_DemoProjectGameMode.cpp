// Designed by Hitman's Store 2022

#include "PRAS_DemoProjectGameMode.h"
#include "PRAS_DemoProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

APRAS_DemoProjectGameMode::APRAS_DemoProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/DemoCharacter_BP"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
