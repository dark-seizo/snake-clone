#ifndef _SNAKE_H
#define _SNAKE_H

#include <iostream>
#include <list>
#include <stack>
#include <time.h>
#include <Thor/Input/ActionMap.hpp>
#include <Thor/Math.hpp>
#include <Thor/Resources.hpp>
#include <Thor/Time/CallbackTimer.hpp>
#include "IState.h"
#include "Dyno.h"
#include "screens.h"

///initial length for the snake
const unsigned int initial_length   = 8;

///the initial delay for updates
const float dt_start                = .17f;

///min and max time delay for special food spawn
const float min_spec_food           = 6.5f;
const float max_spec_food           = 9.0f;

///Position of the gameboard
const sf::Vector2f gameBoardPos = sf::Vector2f(205,10);
const sf::Vector2f gameBoardSize = sf::Vector2f(381,381);

class Snake : public IState
{
private:

    ///holds the possible directions of the snake
    enum class SNAKE_DIRECTION
    {
        NORTH,
        SOUTH,
        EAST,
        WEST
    };

    enum class FOOD_TYPE
    {
        APPLE,
        ORANGE,
        GRAPE
    };

    ///Holds the data needed for putting foods on the board
    struct food
    {
        food(sf::Vector2i pos, FOOD_TYPE foodType)
        : mapPos(pos), type(foodType) {};
        food() : mapPos(sf::Vector2i(0,0)), type(FOOD_TYPE::GRAPE) {};

        sf::Vector2i  mapPos;
        FOOD_TYPE     type;
    };

public:
    /** Default constructor */
    Snake();
    /** Default destructor */
    virtual ~Snake();

    ///Overloaded from IState
    void Update (sf::Time dt);
    void HandleEvents(sf::RenderWindow &window);
    void Draw(sf::RenderWindow &window);
    void DoInit();
    void DeInit();
    void Pause();
    void Resume();

    ///resets the game
    void ResetGame();

//private methods
private:
    ///returns a position on the board that is not taken
    ///by the snake
    sf::Vector2i getOpenPosition(void);

private:
    ///Copy and = are private
    Snake(const Snake& other);
    Snake& operator=(const Snake& other);

private:
    ///holds the cumulative time between updates (+=dt)
    sf::Time                                        mDelta;
    ///holds the amount of time between updates
    float                                           mDeltaTime;
    ///holds font for score string
    std::shared_ptr<sf::Font>                       mFont;
    ///hold the players current score
    unsigned int                                    mScore;
    ///ensures that only one direction change can happen per update
    std::vector<SNAKE_DIRECTION>                    mDirBuffer;
    ///snakes length
    unsigned int                                    mLength;
    ///holds all current positions of the snake
    std::list<sf::Vector2i>                         mSnake;
    ///holds the current direction the snakes head is facing
    SNAKE_DIRECTION                                 mDir;
    ///holds all of the foods on the field
    std::vector<food>                               mFood;
    ///tracks the time until the next special food is put on the map
    thor::CallbackTimer                             mSpecialFoodTimer;
    ///holds the elapsed time of the game
    thor::StopWatch                                 mTimeElapsed;
    ///holds the string displaying the time
    std::string                                     mTimeString;
    ///the current multiplier percentage(affects score)
    float                                           mPercentageMultiplier;
};

#endif // _SNAKE_H
