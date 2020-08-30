#include "SGEActorSingletonComponent.h"

USGEActorSingletonComponent::USGEActorSingletonComponent()
{
	bWantsInitializeComponent = true;

	//If we are initialized and we already have a component of this type
	//then we return a pointer to the current singleton instance and destroy this component
}