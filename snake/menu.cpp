#include "menu.h"

Menu::Menu()
{
    mMaxIndex = mCurrentIndex = mMenuItemSpacing = 0;
    mCharacterSize = 30;
    mPosition = sf::Vector2f(0,0);
    this->setSelectedColor(sf::Color::White);
    this->setNormalColor(sf::Color::Red);
    mLoopAround = true;
    mAlignment = Menu::RIGHT_ALIGNMENT;
}

Menu::Menu(std::shared_ptr<sf::Font> font) : Menu()
{
    //ctor
    this->setFont(font);
}

Menu::~Menu()
{
    //dtor
}

Menu::Menu(const Menu& other)
{
    //copy ctor
}

Menu& Menu::operator=(const Menu& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}

void Menu::addMenuItem(const std::string text,
                       std::function<void ()> callback,
                       unsigned int menuIndex)
{
    if(!(mMaxIndex == 0 && mMenuItems.empty()))
    {
        ++mMaxIndex;
    }

    unsigned int indexAssigned = 0;

    if((menuIndex == 0 && !mMenuItems.empty()) || menuIndex > mMaxIndex)
    {
        indexAssigned = mMaxIndex;
    }
    else
        indexAssigned = menuIndex;

    MenuItem item(callback, indexAssigned);
    item.setString(text);
    item.setFont(*mFont);

    item.setCharacterSize(mCharacterSize);

    auto boundingRect = item.getLocalBounds();
    if(boundingRect.width > mSize.x)
        mSize.x = boundingRect.width;

    mSize.y += mCharacterSize + mMenuItemSpacing;

    mMenuItems.push_back(item);
    this->setMenuItemPositions();
}

void Menu::setMenuItemPositions()
{
    for( auto &item : mMenuItems)
    {
        float x_alignment_adjustment = 0;

        switch(mAlignment)
        {
        case RIGHT_ALIGNMENT:
            break;
        case LEFT_ALIGNMENT:
            x_alignment_adjustment = mSize.x - item.getLocalBounds().width;
            break;
        case CENTER_ALIGNMENT:
            x_alignment_adjustment = mSize.x/2 - item.getLocalBounds().width/2;
            break;
        }

        item.setPosition(sf::Vector2f(mPosition.x + x_alignment_adjustment,
                                      mPosition.y + (item.index * mCharacterSize) + (mMenuItemSpacing*item.index)));
    }
}

void Menu::setMenuItemSpacing(unsigned int space)
{
    mMenuItemSpacing = space;
    this->setMenuItemPositions();
}

void Menu::setCharacterSize(unsigned int charSize)
{
    mCharacterSize = charSize;
    for(auto &text : mMenuItems)
    {
        text.setCharacterSize(charSize);
    }
    this->setMenuItemPositions();
}

void Menu::selectCurrentMenuItem()
{
    mMenuItems[mCurrentIndex].callback();
}

sf::Vector2f Menu::getSize()
{
    return mSize;
}

void Menu::incrementSelection(void)
{
    if(mCurrentIndex == 0)
    {
        if(mLoopAround)
            mCurrentIndex = mMaxIndex;
    }
    else
        --mCurrentIndex;
}

void Menu::decrementSelection(void)
{
    if(mCurrentIndex == mMaxIndex)
    {
        if(mLoopAround)
            mCurrentIndex = 0;
    }
    else
        ++mCurrentIndex;
}

void Menu::draw(sf::RenderTarget &target, sf::RenderStates states)
{
    for(auto &item : mMenuItems)
    {
        if(item.index == mCurrentIndex)
            item.setColor(mSelectedColor);
        else
            item.setColor(mNormalColor);

        target.draw(item);
    }
    /*
    sf::RectangleShape boundingBox;
    sf::Vector2f size = this->getSize();
    boundingBox.setPosition(mPosition.x, mPosition.y);
    boundingBox.setSize(size);
    boundingBox.setOutlineColor(sf::Color::Red);
    boundingBox.setOutlineThickness(2.f);
    boundingBox.setFillColor(sf::Color::Transparent);
    target.draw(boundingBox);
    */
}

void Menu::setSelectedColor(const sf::Color &color)
{
    mSelectedColor = color;
}

void Menu::setNormalColor(const sf::Color &color)
{
    mNormalColor = color;
}

void Menu::setLoopAround(bool loop)
{
    mLoopAround = loop;
}

void Menu::setAlignment(Alignment alignment)
{
    mAlignment = alignment;
    this->setMenuItemPositions();
}

void Menu::setPosition(const sf::Vector2f newPosition)
{
    mPosition = newPosition;
    this->setMenuItemPositions();
}

void Menu::setPosition(float new_x, float new_y)
{
    this->setPosition(sf::Vector2f(new_x, new_y));
}

