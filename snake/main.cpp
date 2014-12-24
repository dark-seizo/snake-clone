
#include "StateManager.h"
#include "screens.h"

int main(int argc, char* argv[])
{

    StateManager *manager = StateManager::Instance();
    manager->Init();
    manager->RequestPushState(new MainMenuScreen());

    while(manager->isRunning())
    {
        manager->HandleEvents();
        manager->Update();
        manager->Draw();
    }

    manager->Cleanup();

    delete manager;

    return EXIT_SUCCESS;
}
