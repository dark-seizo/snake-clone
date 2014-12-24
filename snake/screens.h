#ifndef _SCREENS_H
#define _SCREENS_H

#include <iostream>
#include <fstream>
#include <list>
#include <Thor/Input/ActionMap.hpp>
#include <Thor/Resources.hpp>
#include "ResourcePath.hpp"
#include "snake.h"
#include "menu.h"
#include "Dyno.h"
class Snake;

///Class used just for subclassing other screens who's main input will be from the Menu class
class MenuScreen : public IState
{
protected:
    MenuScreen();

public:

    virtual ~MenuScreen();

    ///Overloaded from IState
    virtual void Update (sf::Time dt)                               = 0;
    void HandleEvents(sf::RenderWindow &window);
    void Draw(sf::RenderWindow &window)                             = 0;
    virtual void DoInit();
    virtual void DeInit();
    virtual void Pause()                                            = 0;
    virtual void Resume()                                           = 0;

protected:
    std::shared_ptr<sf::Font>   mFont;
    Menu                        mMenu;
};

class PauseScreen : public MenuScreen
{
    public:
        PauseScreen();
        virtual ~PauseScreen();

    ///Overloaded from IState
    void Update (sf::Time dt)                       {};
    void HandleEvents(sf::RenderWindow &window);
    void Draw(sf::RenderWindow &window);
    void DoInit();
    void DeInit();
    void Pause()                                    {};
    void Resume()                                   {};

    private:

        PauseScreen(const PauseScreen& other); /* intentionally left unimplemented */
        PauseScreen& operator=(const PauseScreen& other); /* intentionally left unimplemented */
};

///used for writing the highscore to file after the game is over
struct HighScore
{
    HighScore() : score(0), length(0), multiplier(0), time("00:00") {};
    HighScore(unsigned int newScore, unsigned int newLength, float newMultiplier, std::string &newTime)
    : score(newScore), length(newLength), multiplier(newMultiplier), time(newTime) {};
    
    unsigned int        score;
    unsigned int        length;
    float               multiplier;
    std::string         time;
};

class GameOverScreen : public MenuScreen
{
    /*
    Highscore file format
    1st score length multiplier time
    2nd score length multiplier time
    .
    .
    .
    */
public:

    ///ctor
    GameOverScreen(HighScore score);

    ~GameOverScreen();

    ///Overloaded from IState
    void Update (sf::Time dt)                       {};
    void HandleEvents(sf::RenderWindow &window);
    void Draw(sf::RenderWindow &window);
    void DoInit();
    void DeInit()                                   {};
    void Pause()                                    {};
    void Resume()                                   {};

private:
    void writeHighScores(void);
    void readHighScores(void);
    unsigned int checkScore(HighScore score) const;

private:
    ///file that contains the highscores
    const std::string                       highScoreFile = resourcePath() + "highscore.hs";

    ///Holds the current high score being input (or denied, if it isn't higher than the other scores
    std::vector<HighScore>                  mHighscores;

    //need variables for input of name
};
class MainMenuScreen : public MenuScreen
{
    //ctor/dtor
public:
    MainMenuScreen();

    virtual~MainMenuScreen();

    ///Overloaded from IState
    void Update (sf::Time dt)                       {};
    void HandleEvents(sf::RenderWindow &window);
    void Draw(sf::RenderWindow &window);
    void DoInit();
    void DeInit();
    void Pause()                                    {};
    void Resume()                                   {};
};

#endif // _SCREENS_H
