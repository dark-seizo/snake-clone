#include "screens.h"

///MenuScreen Methods


MenuScreen::MenuScreen()
{

}

MenuScreen::~MenuScreen() {}

void MenuScreen::DoInit()
{
    thor::Action up(sf::Keyboard::Up, thor::Action::PressOnce);
    thor::Action down(sf::Keyboard::Down, thor::Action::PressOnce);
    thor::Action select(sf::Keyboard::Return, thor::Action::PressOnce);

    m_map["close"] = thor::Action(sf::Event::Closed);
    m_map["up"] = up;
    m_map["down"] = down;
    m_map["select"] = select;

    auto *mngr = StateManager::Instance();
    auto &resourceCache = mngr->getResourceCache();

    mngr->setPreviousStateDraw(true);
    //setup resources
    thor::ResourceKey<sf::Font> fontKey = thor::Resources::fromFile<sf::Font>(resourcePath() + "sansation.ttf");
    try
    {
        mFont = resourceCache.acquire(fontKey);
    }
    catch (thor::ResourceLoadingException &e)
    {
        std::cout << e.what() << std::endl;
    }

    mMenu.setFont(mFont);
}

void MenuScreen::HandleEvents(sf::RenderWindow &window)
{
    m_map.update(window);

    if(m_map.isActive("close")) window.close();

    if(m_map.isActive("up"))
        mMenu.incrementSelection();
    else if(m_map.isActive("down"))
        mMenu.decrementSelection();

    if(m_map.isActive("select"))
        mMenu.selectCurrentMenuItem();
}

void MenuScreen::DeInit()
{
    auto manager = StateManager::Instance();
    manager->setPreviousStateDraw(false);
}

///End of MenuScreen


PauseScreen::PauseScreen()
{
}

PauseScreen::~PauseScreen()
{
    //dtor
}

void PauseScreen::DoInit()
{
    MenuScreen::DoInit();

    mMenu.setPosition(sf::Vector2f(255,195));
    mMenu.setMenuItemSpacing(5);
    mMenu.setCharacterSize(22);
    mMenu.setAlignment(Menu::CENTER_ALIGNMENT);

    mMenu.addMenuItem("Resume",
                      [] () -> void
                      {
                          auto manager = StateManager::Instance();
                          manager->RequestPopState();
                      });
    mMenu.addMenuItem("Quit",
                      [] () -> void
                      {
                          auto manager = StateManager::Instance();
                          manager->Quit();
                      });
}

void PauseScreen::HandleEvents(sf::RenderWindow& window)
{
    MenuScreen::HandleEvents(window);
}

void PauseScreen::Draw(sf::RenderWindow& window)
{

    sf::Vector2u windowSize = window.getSize();
    sf::RectangleShape backdrop;
    backdrop.setSize(sf::Vector2f(200,100));
    backdrop.setFillColor(sf::Color::Green);
    backdrop.setOutlineColor(sf::Color::Yellow);
    backdrop.setOutlineThickness(2.f);
    backdrop.setPosition(sf::Vector2f(windowSize.x/2 - backdrop.getSize().x/2,
                                      windowSize.y/2 - backdrop.getSize().y/2));
    window.draw(backdrop, sf::RenderStates(sf::BlendAlpha));

    sf::Text pauseLabel;
    pauseLabel.setFont(*mFont);
    pauseLabel.setString("Pause");
    pauseLabel.setPosition(sf::Vector2f(backdrop.getPosition().x + backdrop.getSize().x*.3f,
                                        backdrop.getPosition().y));
    pauseLabel.setColor(sf::Color::Blue);
    window.draw(pauseLabel);

    sf::RectangleShape dividerLine;
    dividerLine.setFillColor(sf::Color::Black);
    dividerLine.setSize(sf::Vector2f(backdrop.getSize().x - 20.f, 1.f));
    dividerLine.setPosition(sf::Vector2f(backdrop.getPosition().x + 10,
                                         backdrop.getPosition().y + 40));
    window.draw(dividerLine);

    mMenu.draw(window);
}

void PauseScreen::DeInit()
{
    MenuScreen::DeInit();
}

///GameOverScreen Methods

GameOverScreen::GameOverScreen(HighScore score)
{
    this->readHighScores();

    unsigned int position = this->checkScore(score);
    if(position >= 0)
    {
       //std::vector<HighScore>::iterator it = std::iterator(position);
       std::vector<HighScore>::iterator it = mHighscores.begin();
        mHighscores.insert(it+position, score);
        if(mHighscores.size() > 5)
            mHighscores.pop_back();
    }

    this->writeHighScores();
}

GameOverScreen::~GameOverScreen() {};

void GameOverScreen::DoInit()
{
    MenuScreen::DoInit();

    //setup the menu
    mMenu.setAlignment(Menu::RIGHT_ALIGNMENT);
    mMenu.setCharacterSize(20);
    mMenu.setLoopAround(false);
    mMenu.setMenuItemSpacing(3);
    /*
    mMenu.addMenuItem("Play Again",
                      [] () -> void
                      {

                      });
    */
    mMenu.addMenuItem("Main Menu",
                      [] () -> void
                      {
                          auto *mng = StateManager::Instance();
                          mng -> RequestPopState();
                          mng -> RequestChangeState(new MainMenuScreen());
                      });
    mMenu.addMenuItem("Quit",
                      [] () -> void
                      {
                          StateManager::Instance()->Quit();
                      });
}

void GameOverScreen::HandleEvents(sf::RenderWindow &window)
{
    MenuScreen::HandleEvents(window);
}

void GameOverScreen::Draw(sf::RenderWindow &window)
{
    sf::Vector2u windowSize = window.getSize();
    sf::RectangleShape backdrop;
    backdrop.setSize(sf::Vector2f(windowSize.x/1.2f, windowSize.y/1.2f));
    backdrop.setFillColor(sf::Color::Green);
    backdrop.setOutlineColor(sf::Color::Yellow);
    backdrop.setOutlineThickness(2.f);
    backdrop.setPosition(sf::Vector2f(windowSize.x/2 - backdrop.getSize().x/2,
                                      windowSize.y/2 - backdrop.getSize().y/2));
    window.draw(backdrop);

    sf::RectangleShape scoreBoard;
    scoreBoard.setSize(sf::Vector2f(backdrop.getSize().x - 15.f - mMenu.getSize().x,
                                    backdrop.getSize().y - 10.f));
    scoreBoard.setFillColor(sf::Color::Blue);
    scoreBoard.setOutlineColor(sf::Color::Yellow);
    scoreBoard.setOutlineThickness(2.f);
    scoreBoard.setPosition(sf::Vector2f(backdrop.getPosition().x + 10.f + mMenu.getSize().x,
                                        backdrop.getPosition().y + 5.f));
    window.draw(scoreBoard);

    //setting this here for testing, move this later
    mMenu.setPosition(sf::Vector2f(backdrop.getPosition().x+5.f,
                                   backdrop.getPosition().y+5.f));

    mMenu.draw(window);

    for(int i = 0; i < 5; ++i)
    {
        float columnBufferHeight = scoreBoard.getSize().y / 10;
        const unsigned int rankAmount = 5;
        float columnBufferWidth = scoreBoard.getSize().x / 5;
        const unsigned int columnCount = 4;

        float column_x_pos = (scoreBoard.getPosition().x + columnBufferWidth) + (i*((scoreBoard.getSize().x-columnBufferWidth)/columnCount));
        float column_y_pos = (scoreBoard.getPosition().y + columnBufferHeight) + (i*((scoreBoard.getSize().y-columnBufferHeight)/rankAmount));

        std::string rank;
        switch(i)
        {
            case 0:
                rank = "1st";
                break;
            case 1:
                rank = "2nd";
                break;
            case 2:
                rank = "3rd";
                break;
            case 3:
                rank = "4th";
                break;
            case 4:
                rank = "5th";
                break;
            default: break;
        }

        sf::Text rankLabel(rank, *mFont, 30);
        rankLabel.setColor(sf::Color::Red);
        rankLabel.setPosition(sf::Vector2f(scoreBoard.getPosition().x+10.f,
                                           column_y_pos));
        window.draw(rankLabel);

        if(i < 4)
        {
            std::string columnTitle;
            switch(i)
            {
            case 0:
                columnTitle = "Score";
                break;
            case 1:
                columnTitle = "Length";
                break;
            case 2:
                columnTitle = "Multiplier";
                break;
            case 3:
                columnTitle = "Time";
                break;
            default: break;
            }

            sf::Text columnLabel(columnTitle, *mFont, 15);
            columnLabel.setColor(sf::Color::Red);

            columnLabel.setPosition(sf::Vector2f(column_x_pos,
                                                 scoreBoard.getPosition().y + 10.f));
            window.draw(columnLabel);
        }
        HighScore score = mHighscores[i];

        //std::string scoreString = dyno::lexical_cast<std::string, int>(score.score);
        std::string scoreString = std::to_string(score.score);
        sf::Text scoreLabel(scoreString, *mFont, 25);
        scoreLabel.setColor(sf::Color::Red);
        scoreLabel.setPosition(sf::Vector2f((scoreBoard.getPosition().x + columnBufferWidth),
                                            column_y_pos + 5.f));
        window.draw(scoreLabel);

        //std::string lengthString = dyno::lexical_cast<std::string, int>(score.length);
        std::string lengthString = std::to_string(score.length);
        sf::Text lengthLabel(lengthString, *mFont, 25);
        lengthLabel.setColor(sf::Color::Red);
        lengthLabel.setPosition(sf::Vector2f((scoreBoard.getPosition().x + columnBufferWidth) + ((scoreBoard.getSize().x-columnBufferWidth)/columnCount),
                                             column_y_pos + 5.f));
        window.draw(lengthLabel);

        //std::string multiplierString = dyno::lexical_cast<std::string, float>(score.multiplier);
        std::string multiplierString = std::to_string(int(score.multiplier));
        multiplierString += "%";
        sf::Text multiplierLabel(multiplierString, *mFont, 25);
        multiplierLabel.setColor(sf::Color::Red);
        multiplierLabel.setPosition(sf::Vector2f((scoreBoard.getPosition().x + columnBufferWidth) + (2*((scoreBoard.getSize().x-columnBufferWidth)/columnCount)),
                                                 column_y_pos + 5.f));
        window.draw(multiplierLabel);

        sf::Text timeLabel(score.time, *mFont, 25);
        timeLabel.setColor(sf::Color::Red);
        timeLabel.setPosition(sf::Vector2f((scoreBoard.getPosition().x + columnBufferWidth) + (3*((scoreBoard.getSize().x-columnBufferWidth)/columnCount)),
                                           column_y_pos + 5.f));
        window.draw(timeLabel);
    }
}


void GameOverScreen::readHighScores(void)
{
    std::ifstream hsData(highScoreFile);
    if(hsData.is_open())
    {
        while(!hsData.eof())
        {
            std::string data;
            HighScore score;
            getline(hsData, data);
            //std::cout << "data line recieved: " << data << std::endl;
            std::vector<std::string> splitData = dyno::splitString(data);

            /*
            for(auto &str : splitData)
            {
                std::cout << str << " \n";
            }
            */

            if(splitData.size() == 4)
            {
                score.score = atoi(splitData[0].c_str());
                score.length = atoi(splitData[1].c_str());
                score.multiplier = atof(splitData[2].c_str());
                score.time = splitData[3];
                mHighscores.push_back(score);
            }

        }
    }
    
    //if the high score file is not full, fill it in with zeros
    if(mHighscores.size() < 5)
        for(unsigned int i = mHighscores.size(); i <= 5; ++i)
        {
            HighScore zeroScore;
            mHighscores.push_back(zeroScore);
        }
}

void GameOverScreen::writeHighScores(void)
{
    std::ofstream hsData(highScoreFile, std::ios::trunc);

    for(auto &score : mHighscores)
    {
        std::string stringData;

        //stringData += dyno::lexical_cast<std::string, unsigned int>(score.score);
        //stringData += " " + dyno::lexical_cast<std::string, unsigned int>(score.length);
        //stringData += " " + dyno::lexical_cast<std::string, float>(score.multiplier);
        //stringData += " " + score.time + "\n";
        stringData += std::to_string(score.score);
        stringData += " " + std::to_string(score.length);
        stringData += " " + std::to_string(score.multiplier);
        stringData += " " + score.time + "\n";
        
        hsData << stringData;
    }
}

unsigned int GameOverScreen::checkScore(HighScore newScore) const
{
    if(mHighscores.empty())
        return 0;

    size_t highscoreSize = mHighscores.size();
    for(size_t i = 0; i < highscoreSize; ++i)
    {
        if(newScore.score > mHighscores[i].score)
        {
            return i;
        }
    }
    //the score is not a high score
    return mHighscores.size();
}

///MainMenuScreen Methods


MainMenuScreen::MainMenuScreen() {}

MainMenuScreen::~MainMenuScreen() {}


void MainMenuScreen::DoInit()
{
    MenuScreen::DoInit();

    auto *manager = StateManager::Instance();
    sf::RenderWindow &window = manager->getWindow();
    sf::Vector2u windowSize = window.getSize();


    mMenu.setCharacterSize(25);
    mMenu.setMenuItemSpacing(7);
    mMenu.setAlignment(Menu::CENTER_ALIGNMENT);
    mMenu.setLoopAround(false);

    mMenu.addMenuItem("Play Game",
                      [] () -> void
                      {
                          StateManager::Instance()->RequestChangeState(new Snake());
                      });
    mMenu.addMenuItem("Quit",
                      [] () -> void
                      {
                          StateManager::Instance()->Quit();
                      });

    mMenu.setPosition(sf::Vector2f(windowSize.x/2-mMenu.getSize().x/2,
                                   windowSize.y/2-mMenu.getSize().y/2));

}

void MainMenuScreen::Draw(sf::RenderWindow& window)
{
    mMenu.draw(window);
}

void MainMenuScreen::HandleEvents(sf::RenderWindow &window)
{
    MenuScreen::HandleEvents(window);
}

void MainMenuScreen::DeInit()
{
    MenuScreen::DeInit();
}
