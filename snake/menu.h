#ifndef MENU_H
#define MENU_H

#include <vector>
#include <iostream>
#include <memory>
#include <SFML/Graphics.hpp>
//#include <Thor/Animation/FadeAnimation.hpp>

class Menu
{
    private:
        struct MenuItem : public sf::Text
        {
            MenuItem(std::function<void ()> func,
                     unsigned int newIndex) : callback(func), index(newIndex) {};
            MenuItem() {};

            std::function<void ()>      callback;
            unsigned int                index;
        };

    /** The different choices for alignment
    */
    public:
        enum Alignment
        {
            CENTER_ALIGNMENT        = 0,
            RIGHT_ALIGNMENT         = 1,
            LEFT_ALIGNMENT          = 2
        };

    public:
        /** constructor */
        Menu();
        Menu(std::shared_ptr<sf::Font> font);
        /** Default destructor */
        virtual ~Menu();
        /** Copy constructor
         *  \param other Object to copy from
         */
        Menu(const Menu& other);
        /** Assignment operator
         *  \param other Object to assign from
         *  \return A reference to this
         */
        Menu& operator=(const Menu& other);

        /** Adds a menu item to the menu
        *   \brief Adds a menu item to the menu
        *   \param menuItemName the text that will display on the menu
        *   \param callbackFunction the function that will be called when the item is selected
        *   \param menuIndex the desired index of the new menuItem (if 0, it will be placed at the end)
        *   **The callbacks will initially be used as lambda's (will look into std::bind)
        */
        void addMenuItem(const std::string menuItemName,
                         std::function<void ()> callbackFunction,
                          unsigned int menuIndex = 0);

        /** Returns a vector containing the size of the menu
        *   \brief  Returns the size of the menu
        *   \return The size of the entire menu
        */
        sf::Vector2f getSize(void);

        /** Returns the current position of the menu relative to the top left corner.
        *   \brief  Returns the position of the menu.
        *   \return The position of the menu
        */
        sf::Vector2f getPosition() { return mPosition; }

        /** Sets the current position of the menu
        *   \param newPosition the new position
        */
        void setPosition(const sf::Vector2f newPosition);

        /** Sets the current position of the menu
        *   \param new_x The new x coordinate of the menu
        *   \param new_y The new y coordinate of the menu
        */
        void setPosition(float new_x, float new_y);

        /** Sets the spacing between each menu Item
        *   \param spacing the new spacing between items (default:0)
        */
        void setMenuItemSpacing(unsigned int spacing);

        /** Draws the menu to the screen using target
        *   \param target reference to the render target
        *   \param states render states, default to sf::RenderStates::Default
        */
        void draw(sf::RenderTarget &target,
                   sf::RenderStates states = sf::RenderStates::Default);

        /** Sets the character size for all menu items
        *   \param charSize new character size (in pixels) to be applied to all menu items
        */
        void setCharacterSize(unsigned int charSize);

        /** Sets the font needed for the menu
        *   \param font font used for all menu items
        */
        void setFont(std::shared_ptr<sf::Font> font) { mFont = font; }

        /** Calls the function tied to the index provided
        *   \param index for which item was selected
        */
        void selectCurrentMenuItem(void);

        /** Increments the currently selected item in the menu
        */
        void incrementSelection(void);

        /** Decrements the currently selected item in the menu
        */
        void decrementSelection(void);

        /** Sets the color for whichever item in the menu is currently focused.  Default is white.
        *   \brief Sets the color of the current index text
        *   \param New color to set selected index
        */
        void setSelectedColor(const sf::Color &color);

        /** Sets the color for all menu items not currently focused.  Default is red.
        *   \brief Sets the color for the menu items that are not currently focused.
        *   \param New color for non-focused menu items
        */
        void setNormalColor(const sf::Color &color);

        /** Sets the looping behavior. When true, if the current index is decremented when at the max
        *   index, it will do loop around to the top.
        *   \brief Sets the looping index behavior
        *   \param loop the new value for looping
        */
        void setLoopAround(bool loop);

        /** Sets the menu items alignment. Each time the alignment changes, all items in the menu are
        *   updated.  Default is right alignment.
        *   \brief Sets the alignment for the menu.
        *   \param alignment the new alignment for the menu
        */
        void setAlignment(Alignment alignment);

    ///Private methods
    private:
        void setMenuItemPositions();

    private:
        bool                            mLoopAround;

        unsigned int                    mCurrentIndex,
                                        mMaxIndex,
                                        mMenuItemSpacing,
                                        mCharacterSize;

        sf::Vector2f                    mPosition,
                                        mSize;

        std::shared_ptr<sf::Font>       mFont;

        std::vector<MenuItem>           mMenuItems;

        sf::Color                       mSelectedColor,
                                        mNormalColor;

        Alignment                       mAlignment;
};

#endif // MENU_H
