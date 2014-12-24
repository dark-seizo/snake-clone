
#ifndef _ISTATE_H_
#define _ISTATE_H_

#include <SFML/System/Time.hpp>
#include <Thor/Input/ActionMap.hpp>
#include "StateManager.h"

///Foward declaration of the state manager
class StateManager;

/// class IState - Base class that all states in the game will inherit from.
class IState {

public:
    /// handles game logic update at set intervals
    /// @param dt (sf::Time) the time elapsed since last call to update
    virtual void Update (sf::Time dt)                   = 0;
    ///Handles input for the state
    virtual void HandleEvents(sf::RenderWindow &window)     = 0;
    /// Handles the draw method called by the StateManager
    virtual void Draw (sf::RenderWindow &window)            = 0;
    /// Called when the IState is pushed onto the stack
    virtual void DoInit ()                                  = 0;
    /// Called when an IState is popped from the stack
    virtual void DeInit ()                                  = 0;
    /// Pauses the current IState
    virtual void Pause ()                                   = 0;
    /// Resumes the current IState
    virtual void Resume ()                                  = 0;

protected:
    thor::ActionMap<std::string>                        m_map;
};

#endif
