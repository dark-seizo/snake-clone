#ifndef _STATEMANAGER_H_
#define _STATEMANAGER_H_

#include <string>
#include <iostream>
#include <Thor/Time/StopWatch.hpp>
#include <Thor/Resources/MultiResourceCache.hpp>
#include <SFML/Graphics.hpp>
#include "IState.h"

/*
BUG: Need to update Screen_Width and Screen_Height when screen is resized
*/

class IState;

const unsigned int  Screen_Width           = 600;
const unsigned int  Screen_Height          = 400;

const std::string   Screen_Title           = "Another Snake Clone";

const unsigned int FrameRate               = 60;

/// class StateManager - Manages the States in the program,
/// as well as handling the callbacks.
class StateManager {

public:
    ///Dtor
    virtual ~StateManager() {}
    /// Returns the main render window used by the StateManager
    sf::RenderWindow & getWindow ()                 { return m_window; }
    /// Returns the resource cache
    thor::MultiResourceCache& getResourceCache()    { return m_resourceCache; }
    /// Returns true if the StateManager is running, false otherwise
    bool isRunning () const;
    /// Handles calling update on current State
    void Update ();
    /// Handles all the input for the game
    void HandleEvents();
    /// Handles calling the draw method for each State
    void Draw ();
    /// Pushes a new state onto the stack
    /// @param newState		(IState*) pushes a new state onto the stack
    void RequestPushState (IState* newState);
    /// removes the top state from the game state stack
    void RequestPopState ();
    /// Changes the current state to a new state
    /// @param newState  (IState*) pushes the new state to top of stack
    void RequestChangeState (IState *newState);
    ///Initializes the StateManager
    void Init ();
    /// Called when the game has ended
    void Cleanup ();
    /// Sets the running status to false, ends the game
    void Quit ();
    /// Sets the draw previous boolean
    void setPreviousStateDraw(bool drawPrev);
    ///Static instance of StateManager used as Singleton
    static StateManager* Instance();

private:
    /// Holds the render window used throughout the game
    sf::RenderWindow                        m_window;
    /// Resource manager for all states of the game
    thor::MultiResourceCache                m_resourceCache;
    //thor::ResourceCache<sf::Font>           m_resourceCache;
    /// Used to handle update timing, as well as pausing.
    thor::StopWatch                         m_timer;
    /// Holds all current states of the game
    std::vector<IState*>                    m_states;
    /// Holds all of the pending state changes
    ///determines if the StateManager is currently running
    bool                                    m_running;
    ///if true, the game will draw the current IState, as well as the one below it (first)
    bool                                    m_drawPreviousScreen;

    ///singleton pattern
    static StateManager*                     instance;
    ///Declared private so it is impossible to create a StateManager
    StateManager();
    ///Declared private, Singleton pattern
    StateManager(const StateManager & cpy); /* intentionally left unimplemented */
    ///Declared private, Singleton pattern
    void operator=(StateManager const&); /* intentionally left unimplemented */

};

#endif
