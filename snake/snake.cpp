#include "snake.h"

Snake::Snake() : IState()
{
    ResetGame();
}

Snake::~Snake()
{
    //dtor
    mSnake.clear();
    mFood.clear();
    mDirBuffer.clear();
}

void Snake::ResetGame()
{
    //clear the data from the previous game
    mSnake.clear();
    mFood.clear();
    mDirBuffer.clear();

    mLength = initial_length;
    mScore = 0;
    mPercentageMultiplier = 100.f;
    mDeltaTime = dt_start;
    mDelta = sf::seconds(mDeltaTime);
    mDir = SNAKE_DIRECTION::NORTH;
    for(unsigned int i = 10; i < 10+mLength; ++i)
    {
        mSnake.push_back(sf::Vector2i(12,i));
    }
    mTimeElapsed.restart();
}

void Snake::DoInit()
{
    //setup the input
    thor::Action up(sf::Keyboard::Up);
    thor::Action down(sf::Keyboard::Down);
    thor::Action left(sf::Keyboard::Left);
    thor::Action right(sf::Keyboard::Right);
    thor::Action pause(sf::Keyboard::Space);

    m_map["close"] = thor::Action(sf::Event::Closed);
    m_map["direction_change_north"] = up;
    m_map["direction_change_south"] = down;
    m_map["direction_change_east"] = right;
    m_map["direction_change_west"] = left;
    m_map["pause"] = pause;

    //setup resources
    thor::ResourceKey<sf::Font> fontKey = thor::Resources::fromFile<sf::Font>(resourcePath() + "sansation.ttf");
    try
    {
        mFont = StateManager::Instance()->getResourceCache().acquire(fontKey);
    }
    catch (thor::ResourceLoadingException &e)
    {
        std::cout << e.what() << std::endl;
    }

    //setup special food timer
    mSpecialFoodTimer.connect([this] (thor::CallbackTimer & timer) -> void
                      {
                          food new_food;
                          sf::Vector2i pos = this->getOpenPosition();
                          new_food.mapPos = pos;
                          int rand = thor::random(0, 100);
                          if(rand <= 40) //41% chance
                          {
                              new_food.type = FOOD_TYPE::GRAPE;
                          }
                          else if(rand > 40) //59% chance
                          {
                              new_food.type = FOOD_TYPE::ORANGE;
                          }
                          mFood.push_back(new_food);
                      });
}

void Snake::Update(sf::Time dt)
{
    //update timer BEFORE resetting it
    mSpecialFoodTimer.update();

    //update time string
    std::tm *timeFormat = new std::tm();
    int seconds = mTimeElapsed.getElapsedTime().asSeconds();
    int minutes = (int)(seconds/60);
    seconds -= 60*minutes;
    timeFormat->tm_sec = seconds;
    timeFormat->tm_min = minutes;

    char buffer[6];
    std::strftime(buffer, 6, "%M:%S", timeFormat);
    mTimeString = buffer;
    delete timeFormat;


    mDelta += dt;
    if(mDelta.asSeconds() <= mDeltaTime) return;
    else
    {
        StateManager *manager = StateManager::Instance();

        mDelta = sf::Time::Zero;

        auto head = mSnake.front();

        //collision detection with wall sides
        if(head.x < 0 || head.x > 18 || head.y < 0 || head.y > 18)
        {
            //game over
            std::cout <<"Game over, snake hit edge of map: (" << head.x << "," << head.y << ")\n";
            HighScore score(mScore, mLength, mPercentageMultiplier, mTimeString);
            StateManager::Instance()->RequestPushState(new GameOverScreen(score));

            //manager->Quit();
        }
        //collision detection if the snake has collided with itself
        //start the iteration at the 4th object (do not check if the head == head !)
        for(auto itr = std::next(mSnake.begin(),3); itr != mSnake.end(); ++itr)
        {
            sf::Vector2i vec = *itr;
            if(head.x == vec.x && head.y == vec.y)
            {
                std::cout <<"Game over, snake hit itself at: (" << head.x << "," << head.y << ")\n";

                HighScore score(mScore, mLength, mPercentageMultiplier, mTimeString);
                StateManager::Instance()->RequestPushState(new GameOverScreen(score));

                //manager->Quit();
            }
        }
        //collision detection with foods
        auto itr = mFood.begin(), end = mFood.end();
        for( ; itr < end; ++itr)
        {
            food food = *itr;
            if(head.x == food.mapPos.x && head.y == food.mapPos.y)
            {
                float multiplier = mPercentageMultiplier / 100;
                switch(food.type)
                {
                case FOOD_TYPE::APPLE:
                    //slow the snake down by 1%
                    mDeltaTime += mDeltaTime*0.01f;
                    mLength += 2;
                    mScore += 20 * multiplier;
                    itr = mFood.erase(itr);
                    break;
                case FOOD_TYPE::GRAPE:
                    //speed the snake up by 6%
                    mDeltaTime -= mDeltaTime*0.06f;
                    mScore += 100 * multiplier;
                    itr = mFood.erase(itr);
                    break;
                case FOOD_TYPE::ORANGE:
                    //speed the snake up by 2%
                    mDeltaTime -= mDeltaTime*0.02f;
                    mLength += 6;
                    mScore += 60 * multiplier;
                    itr = mFood.erase(itr);
                    break;
                }
            }
        }

        //take care of the multiplier
        float multiply = (mDeltaTime - dt_start)/dt_start;
        float multi = dyno::decimalPrecision<float>(2,multiply);
        mPercentageMultiplier = 100 - multi * 100;

        if(!mDirBuffer.empty())
        {
            mDir = mDirBuffer.back();
            mDirBuffer.pop_back();
        }

        ///clear the buffer if more then 2 dir changes are queued at once
        if(mDirBuffer.size() > 2)
            mDirBuffer.clear();

        switch(mDir)
        {
        case SNAKE_DIRECTION::NORTH:
            mSnake.push_front(sf::Vector2i(head.x,head.y-1));
            break;
        case SNAKE_DIRECTION::SOUTH:
            mSnake.push_front(sf::Vector2i(head.x, head.y+1));
            break;
        case SNAKE_DIRECTION::EAST:
            mSnake.push_front(sf::Vector2i(head.x+1, head.y));
            break;
        case SNAKE_DIRECTION::WEST:
            mSnake.push_front(sf::Vector2i(head.x-1, head.y));
            break;
        }
        //checks if the snakes length has changed, pops the tail if it hasn't
        if(mSnake.size() > mLength)
            mSnake.pop_back();

        bool appleFood = false;
        for(auto &food : mFood)
        {
            if(food.type == FOOD_TYPE::APPLE)
            {
                appleFood = true;
                break;
            }
        }
        if(!appleFood)
        {
            // make an apple
            food new_food;
            new_food.mapPos = getOpenPosition();
            new_food.type = FOOD_TYPE::APPLE;
            mFood.push_back(new_food);
        }
    }
        //check if timer is expired
    if(mSpecialFoodTimer.isExpired())
        mSpecialFoodTimer.restart(sf::seconds(thor::random(min_spec_food,
                                                           max_spec_food)));
}

void Snake::HandleEvents(sf::RenderWindow &window)
{
    m_map.update(window);

    if(m_map.isActive("close")) window.close();
    if(m_map.isActive("pause"))
    {
        auto *mngr = StateManager::Instance();
        PauseScreen *pause = new PauseScreen();
        mngr->RequestPushState(pause);
    }

    if(m_map.isActive("direction_change_north"))
    {
        if(mDir != SNAKE_DIRECTION::SOUTH)
        {
            mDirBuffer.push_back(SNAKE_DIRECTION::NORTH);
        }
    }
    else if(m_map.isActive("direction_change_south"))
    {
        if(mDir != SNAKE_DIRECTION::NORTH)
        {
            mDirBuffer.push_back(SNAKE_DIRECTION::SOUTH);
        }
    }
    else if(m_map.isActive("direction_change_east"))
    {
        if(mDir != SNAKE_DIRECTION::WEST)
        {
            mDirBuffer.push_back(SNAKE_DIRECTION::EAST);
        }
    }
    else if(m_map.isActive("direction_change_west"))
    {
        if(mDir != SNAKE_DIRECTION::EAST)
        {
            mDirBuffer.push_back(SNAKE_DIRECTION::WEST);
        }
    }
}

void Snake::Draw(sf::RenderWindow &window)
{
    sf::RectangleShape gameBoard;
    //set up the gameboard
    gameBoard.setPosition(sf::Vector2f(205,10));
    gameBoard.setSize(sf::Vector2f(381, 381));
    gameBoard.setFillColor(sf::Color::Green);
    gameBoard.setOutlineThickness(2.f);
    gameBoard.setOutlineColor(sf::Color::Yellow);
    window.draw(gameBoard);

    sf::Vector2f gameBoardPos = gameBoard.getPosition();
    sf::Vector2f gameBoardSize = gameBoard.getSize();

    //draw score board
    sf::Vector2f boardSize = sf::Vector2f(185,75);

    sf::RectangleShape scoreBoard;
    scoreBoard.setPosition(sf::Vector2f(10,10));
    scoreBoard.setSize(boardSize);
    scoreBoard.setFillColor(sf::Color::Green);
    scoreBoard.setOutlineColor(sf::Color::Yellow);
    scoreBoard.setOutlineThickness(2.f);
    window.draw(scoreBoard);

    sf::Text scoreString;
    scoreString.setFont(*mFont);
    scoreString.setColor(sf::Color::Blue);
    scoreString.setString("Score");
    scoreString.setPosition(sf::Vector2f(scoreBoard.getPosition().x+.3*boardSize.x,
                                         scoreBoard.getPosition().y));
    window.draw(scoreString);

    //std::string score_str = dyno::lexical_cast<std::string, unsigned int>(mScore);
    std::string score_str = std::to_string(mScore);

    sf::Text score;
    score.setFont(*mFont);
    score.setColor(sf::Color::Blue);
    score.setString(score_str);
    score.setPosition(sf::Vector2f(scoreString.getPosition().x,
                                   scoreString.getPosition().y + boardSize.y/2));
    window.draw(score);

    //draw speed frame
    sf::RectangleShape speedBoard;
    speedBoard.setPosition(sf::Vector2f(scoreBoard.getPosition().x,
                                        scoreBoard.getPosition().y*2+boardSize.y));
    speedBoard.setSize(boardSize);
    speedBoard.setFillColor(sf::Color::Green);
    speedBoard.setOutlineColor(sf::Color::Yellow);
    speedBoard.setOutlineThickness(2.f);
    window.draw(speedBoard);

    //draw speed label
    sf::Text speedLabel;
    speedLabel.setFont(*mFont);
    speedLabel.setString("Multiplier");
    //speedLabel.setCharacterSize(20);
    speedLabel.setColor(sf::Color::Blue);
    speedLabel.setPosition(sf::Vector2f(speedBoard.getPosition().x + .2f*boardSize.x,
                                        speedBoard.getPosition().y));
    window.draw(speedLabel);

    //std::string str = dyno::lexical_cast<std::string,float>(mPercentageMultiplier);
    std::string str = std::to_string(int(mPercentageMultiplier));
    str += "%";

    sf::Text multiplier;
    multiplier.setFont(*mFont);
    multiplier.setString(str);
    multiplier.setColor(sf::Color::Blue);
    multiplier.setPosition(sf::Vector2f(speedLabel.getPosition().x,
                                        speedLabel.getPosition().y + boardSize.y/2));
    window.draw(multiplier);

    sf::RectangleShape lengthBoard;
    lengthBoard.setSize(boardSize);
    lengthBoard.setFillColor(sf::Color::Green);
    lengthBoard.setOutlineThickness(2.f);
    lengthBoard.setOutlineColor(sf::Color::Yellow);
    lengthBoard.setPosition(sf::Vector2f(10,
                                         scoreBoard.getPosition().y*3 + boardSize.y*2));
    window.draw(lengthBoard);

    sf::Text lengthLabel;
    lengthLabel.setColor(sf::Color::Blue);
    lengthLabel.setFont(*mFont);
    lengthLabel.setString("Length");
    lengthLabel.setPosition(sf::Vector2f(lengthBoard.getPosition().x + .2f*boardSize.x,
                                         lengthBoard.getPosition().y));
    window.draw(lengthLabel);

    //std::string lengthString = dyno::lexical_cast<std::string, unsigned int>(mLength);
    std::string lengthString = std::to_string(mLength);

    sf::Text length;
    length.setColor(sf::Color::Blue);
    length.setFont(*mFont);
    length.setString(lengthString);
    length.setPosition(sf::Vector2f(lengthLabel.getPosition().x,
                                    lengthLabel.getPosition().y + boardSize.y/2));
    window.draw(length);

    sf::RectangleShape timeBoard;
    timeBoard.setSize(boardSize);
    timeBoard.setFillColor(sf::Color::Green);
    timeBoard.setOutlineThickness(2.f);
    timeBoard.setOutlineColor(sf::Color::Yellow);
    timeBoard.setPosition(10,
                          10 * 4 + boardSize.y*3);
    window.draw(timeBoard);

    sf::Text timeLabel;
    timeLabel.setFont(*mFont);
    timeLabel.setString("Time");
    timeLabel.setColor(sf::Color::Blue);
    timeLabel.setPosition(sf::Vector2f(timeBoard.getPosition().x + .2f * boardSize.x,
                                       timeBoard.getPosition().y));
    window.draw(timeLabel);

    sf::Text time;
    time.setFont(*mFont);
    time.setString(mTimeString);
    time.setColor(sf::Color::Blue);
    time.setPosition(sf::Vector2f(timeLabel.getPosition().x,
                                  timeLabel.getPosition().y + boardSize.y / 2));
    window.draw(time);

    //draw vertical lines
    for(unsigned int i = 0; i < 20; ++i)
    {
        sf::RectangleShape  line;

        line.setPosition(sf::Vector2f(gameBoardPos.x, gameBoardPos.y+(i*20)));
        line.setSize(sf::Vector2f(gameBoardSize.x, 1.f));
        line.setFillColor(sf::Color::Black);
        window.draw(line);
    }
    //draw horizontal lines
    for(unsigned int i = 0; i < 20; ++i)
    {
        sf::RectangleShape  line;

        line.setPosition(sf::Vector2f(gameBoardPos.x+(i*20), gameBoardPos.y));
        line.setSize(sf::Vector2f(1.f, gameBoardSize.y));
        line.setFillColor(sf::Color::Black);
        window.draw(line);
    }

    for(auto &vec : mSnake)
    {
        sf::RectangleShape shape;
        shape.setPosition(sf::Vector2f(gameBoardPos.x+(vec.x*20)+1.f,
                                       gameBoardPos.y+(vec.y*20)+1.f));
        shape.setSize(sf::Vector2f(19,19));
        shape.setFillColor(sf::Color::Blue);
        window.draw(shape);
    }

    //drawing eyes on the snake
    sf::Vector2i head = mSnake.front();
    sf::CircleShape rightEye;
    sf::CircleShape leftEye;

    rightEye.setRadius(3.f);
    leftEye.setRadius(3.f);
    rightEye.setFillColor(sf::Color::Black);
    leftEye.setFillColor(sf::Color::Black);

    rightEye.setPosition(sf::Vector2f(gameBoardPos.x+(head.x*20)+1.f,
                                      gameBoardPos.y+(head.y*20)+1.f));
    leftEye.setPosition(sf::Vector2f(gameBoardPos.x+(head.x*20)+1.f,
                                     gameBoardPos.y+(head.y*20)+1.f));
    sf::Vector2f rightEyePos = rightEye.getPosition();
    sf::Vector2f leftEyePos = leftEye.getPosition();
    //make minor adjustments to eye placement for each direction
    switch(mDir)
    {
    case SNAKE_DIRECTION::NORTH:
        rightEye.setPosition(sf::Vector2f(rightEyePos.x+1.f,
                                          rightEyePos.y+2.f));
        leftEye.setPosition(sf::Vector2f(leftEyePos.x+12.f,
                                         leftEyePos.y+2.f));
        break;

    case SNAKE_DIRECTION::SOUTH:
        rightEye.setPosition(sf::Vector2f(rightEyePos.x+1.f,
                                          rightEyePos.y+12.f));
        leftEye.setPosition(sf::Vector2f(leftEyePos.x+12.f,
                                         leftEyePos.y+12.f));
        break;

    case SNAKE_DIRECTION::EAST:
        rightEye.setPosition(sf::Vector2f(rightEyePos.x+12.f,
                                          rightEyePos.y+12.f));
        leftEye.setPosition((sf::Vector2f(leftEyePos.x+12.f,
                                          leftEyePos.y+2.f)));
        break;

    case SNAKE_DIRECTION::WEST:
        rightEye.setPosition(sf::Vector2f(rightEyePos.x+1.f,
                                          rightEyePos.y+12.f));
        leftEye.setPosition(sf::Vector2f(leftEyePos.x+1.f,
                                         leftEyePos.y+2.f));
        break;
    }
    window.draw(rightEye);
    window.draw(leftEye);

    //draw the foods on the board
    for(auto &food : mFood)
    {
        sf::CircleShape circle(9.f);
        circle.setPosition(sf::Vector2f(gameBoardPos.x+(food.mapPos.x*20)+1.f,
                                        gameBoardPos.y+(food.mapPos.y*20)+1.f));
        switch(food.type)
        {
        case FOOD_TYPE::APPLE:
            circle.setFillColor(sf::Color::Red);
            break;
        case FOOD_TYPE::GRAPE:
            //this needs to be purple
            circle.setFillColor(sf::Color(216,36,164));
            break;
        case FOOD_TYPE::ORANGE:
            //this needs to be orange
            circle.setFillColor(sf::Color(237,135,31));
            break;
        }
        window.draw(circle);
    }
}

void Snake::DeInit()
{

}

void Snake::Pause()
{
    mSpecialFoodTimer.stop();
    mTimeElapsed.stop();
    mDelta = sf::seconds(0.f);
}

void Snake::Resume()
{
    mSpecialFoodTimer.start();
    mTimeElapsed.start();
}

sf::Vector2i Snake::getOpenPosition()
{
    int rand_x = thor::random(0,18);
    int rand_y = thor::random(0,18);

    //check position of the snake nodes
    for(auto &vec : mSnake)
    {
        if(vec.x == rand_x && vec.y == rand_y)
            return getOpenPosition();
    }
    //check food currently on the board
    for(auto &food : mFood)
    {
        if(food.mapPos.x == rand_x && food.mapPos.y == rand_y)
            return getOpenPosition();
    }
    return sf::Vector2i(rand_x,rand_y);
}
