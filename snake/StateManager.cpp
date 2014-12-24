#include "StateManager.h"

/**
*Implements the singleton patter on the StateManager
*/
StateManager* StateManager::instance = NULL;
StateManager* StateManager::Instance()
{
    if(instance == NULL)
    {
        instance = new StateManager();
    }
    return instance;
}

StateManager::StateManager()
{
    m_drawPreviousScreen = false;
}

void StateManager::Init()
{
    m_window.create(sf::VideoMode(Screen_Width,Screen_Height), Screen_Title);

    //set window fps
    m_window.setFramerateLimit(FrameRate);

    //set running to true
    m_running = true;
}

void StateManager::Update()
{
    sf::Time dt = m_timer.getElapsedTime();
    m_states.back()->Update(dt);
    m_timer.restart();
}

void StateManager::HandleEvents()
{
    m_states.back()->HandleEvents(m_window);
}

void StateManager::Draw()
{
    m_window.clear(sf::Color(0,0,0));

    //call draw to IState's between these
    if(m_drawPreviousScreen && (m_states.size() >= 2))
    {
        size_t state_size = m_states.size();
        m_states[state_size-2]->Draw(m_window);
        m_states[state_size-1]->Draw(m_window);
    }
    else
        m_states.back()->Draw(m_window);

    m_window.display();
}

void StateManager::RequestChangeState(IState *newState)
{
    if(!m_states.empty())
    {
        m_states.back()->DeInit();
        m_states.pop_back();
    }

    m_states.push_back(newState);
    m_states.back()->DoInit();
}

void StateManager::RequestPushState(IState *newState)
{
    if(!m_states.empty())
    {
        m_states.back()->Pause();
    }

    m_states.push_back(newState);

    m_states.back()->DoInit();
}

void StateManager::RequestPopState()
{
    if(!m_states.empty())
    {
        m_states.back()->DeInit();
        m_states.pop_back();
    }

    if(!m_states.empty())
        m_states.back()->Resume();
}

void StateManager::Cleanup()
{
    while(!m_states.empty())
    {
        m_states.back()->DeInit();
        m_states.pop_back();
    }
}

bool StateManager::isRunning() const
{
    return m_running;
}

void StateManager::Quit()
{
    m_running = false;
}

void StateManager::setPreviousStateDraw(bool drawPrev)
{
    m_drawPreviousScreen = drawPrev;
}
