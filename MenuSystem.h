/*
 * Copyright (c) 2015, 2016 arduino-menusystem
 * Licensed under the MIT license (see LICENSE)
 */

#ifndef MENUSYSTEM_H
#define MENUSYSTEM_H

#if defined(ARDUINO) && ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif

// TODO: coding standards e.g. p_menu vs pMenu
// TODO: docstrings ala doxygen
// TODO: update all examples
// TODO: Don't use String.
// TODO: Rule of 3 (or 5?) and freeing memory (will be needed for dynamic
//       menus)
// TODO: unint8_t instead of byte?

class Menu;
class MenuComponentRenderer;
class MenuSystem;

//! \brief Abstract base class that represents a component in the menu
//!
//! This is the abstract base class for the main components used to build a
//! menu structure: Menu and MenuItem.
//!
//! MenuComponent should not be used as a base class in clients. Instead use
//! either Menu or MenuItem.
//!
//! MenuComponent is the `Component` part of the composite design pattern (see:
//! https://en.wikipedia.org/wiki/Composite_pattern).
//!
//! \see Menu
//! \see MenuItem
class MenuComponent
{
    friend class MenuSystem;
    friend class Menu;
public:
    //! \brief Construct a MenuComponent
    //! \param[in] name The name of the menu component that is displayed in
    //!                 clients.
    MenuComponent(const char* name);

    //! \brief Set the component's name
    //! \param[in] name The name of the menu component that is displayed in
    //!                 clients.
    void set_name(const char* name);

    //! \brief Gets the component's name
    //! \returns The component's name.
    const char* get_name() const;

    //! \brief Renders the component using the given MenuComponentRenderer
    //!
    //! This is the `accept` method in the visitor design pattern. It should
    //! call the appropriate method on the given MenuComponentRenderer for
    //! rendering this type of component. This method should be called from the
    //! MenuComponent::render method.
    //!
    //! \param[in] renderer The MenuComponentRenderer to use to render this
    //!                     component
    //!
    //! \see MenuComponentRenderer
    virtual void render(MenuComponentRenderer const& renderer) const = 0;

    //! \brief Returns true if this component has focus; false otherwise
    //!
    //! A component has focus when the next and prev functions are able to
    //! change its state rather than navigate the menu structure. It's
    //! primarily used in NumericMenuItem so next and prev change the value
    //! associated with the component.
    //!
    //! Subclasses should set _has_focus to true when this behaviour is desired
    //! and reset it to false when it's no longer required. The usual place to
    //! do this is in the MenuComponent::select method.
    //!
    //! \returns true if this component has focus, false otherwise.
    //!
    //! \see MenuComponent::select
    //! \see NumericMenuComponent
    bool has_focus() const;

protected:
    //! \brief Processes the next action
    //!
    //! The behaviour of this function can differ depending on whether
    //! MenuComponent::has_focus returns true or false.
    //!
    //! When MenuComponent::has_focus returns true, this method should change
    //! some state in the component; when it returns false, this method should
    //! navigate the menu structure.
    //!
    //! \param[in] loop if true the action should loop around to the beginning
    //!                 for finite ranges; otherwise overflow should be
    //!                 prevented.
    //! \returns true if the component processed the action, false otherwise.
    //!
    //! \see MenuComponent::prev
    //! \see MenuComponent::has_focus
    virtual bool next(bool loop=false) = 0;

    //! \brief Processes the prev action
    //!
    //! The behaviour of this function can differ depending on whether
    //! MenuComponent::has_focus returns true or false.
    //!
    //! When MenuComponent::has_focus returns true, this method should change
    //! some state in the component; when it returns false, this method should
    //! navigate the menu structure.
    //!
    //! \param[in] loop if true the action should loop around to the end
    //!                 for finite ranges; otherwise overflow should be
    //!                 prevented.
    //! \returns true if the component processed the action, false otherwise.
    //!
    //! \see MenuComponent::next
    //! \see MenuComponent::has_focus
    virtual bool prev(bool loop=false) = 0;

    //! \brief Resets the component to its initial state
    virtual void reset() = 0;

    //! \brief Processes the select action
    //!
    //! When a menu component is selected by the client an action may need to
    //! performed.
    //!
    //! If the component supports focus, this method is the recommended place
    //! set _has_focus to true so the MenuComponent::next and
    //! MenuComponent::prev methods can be used to change some state in the
    //! component.
    //!
    //! \returns The Menu instance selected or nullptr. The returned Menu
    //!          instance is used in MenuSystem::activate to set the current
    //!          menu in the MenuSystem.
    //!
    //! \see MenuComponent::has_focus
    //! \see NumericMenuComponent
    virtual Menu* select() = 0;

protected:
    const char* _name;
    bool _has_focus;
};


//! \brief A MenuComponent that calls a callback function when selected.
//!
//! MenuItem represents the `Leaf` in the composite design pattern (see:
//! https://en.wikipedia.org/wiki/Composite_pattern). When a MenuItem is
//! selected, the user-defined MenuItem::_select_fn callback is called.
//!
//! \see MenuComponent
//! \see Menu
class MenuItem : public MenuComponent
{
public:
    //! \brief Callback for when the MenuItem is selected
    //!
    //! \param menu_item The menu item being selected.
    using SelectFnPtr = void (*)(MenuItem* menu_item);

public:
    //! \brief Construct a MenuItem
    //! \param[in] name The name of the menu component that is displayed in
    //!                 clients.
    //! \param[in] select_fn The function to call when the MenuItem is
    //!                      selected.
    MenuItem(const char* name, SelectFnPtr select_fn);

    //! \brief Sets the function to call when the MenuItem is selected
    //! \param[in] select_fn The function to call when the MenuItem is
    //!                      selected.
    void set_select_function(SelectFnPtr select_fn);

    //! \copydoc MenuComponent::render
    virtual void render(MenuComponentRenderer const& renderer) const;

protected:
    //! \copydoc MenuComponent::next
    //!
    //! This method does nothing in MenyItem.
    virtual bool next(bool loop=false);

    //! \copydoc MenuComponent::prev
    //!
    //! This method does nothing in MenuItem.
    virtual bool prev(bool loop=false);

    //! \copydoc MenuComponent::reset
    //!
    //! This method does nothing in MenuItem.
    virtual void reset();

    //! \copydoc MenuComponent:select
    virtual Menu* select();

protected:
    SelectFnPtr _select_fn;
};

//! \brief A MenuItem that calls MenuSystem::back() when selected.
//! \see MenuItem
class BackMenuItem : public MenuItem
{
public:
    BackMenuItem(const char* name, SelectFnPtr select_fn, MenuSystem* ms);

    virtual void render(MenuComponentRenderer const& renderer) const;

protected:
    virtual Menu* select();

protected:
    MenuSystem* menu_system;
};


class NumericMenuItem : public MenuItem
{
public:
    //! \brief Callback for formatting the numeric value into a String.
    //!
    //! \param value The value to convert.
    //! \returns The String representation of value.
    using FormatValueFnPtr = const String (*)(const float value);

public:
    /// Constructor
    ///
    /// @param name The name of the menu item.
    /// @param select_fn The function to call when this MenuItem is selected.
    /// @param value Default value.
    /// @param minValue The minimum value.
    /// @param maxValue The maximum value.
    /// @param increment How much the value should be incremented by.
    /// @param valueFormatter The custom formatter. If nullptr the String float
    ///                       formatter will be used.
    NumericMenuItem(const char* name, SelectFnPtr select_fn,
                    float value, float minValue, float maxValue,
                    float increment=1.0,
                    FormatValueFnPtr format_value_fn=nullptr);

    /**
     * Sets the custom number formatter.
     *
     * @param numberFormat the custom formatter. If nullptr the String float
     *                     formatter will be used (2 decimals)
     */
    void set_number_formatter(FormatValueFnPtr format_value_fn);

    float get_value() const;
    float get_minValue() const;
    float get_maxValue() const;

    String get_value_string() const;
    void set_value(float value);

    virtual void render(MenuComponentRenderer const& renderer) const;

protected:
    virtual bool next(bool loop=false);
    virtual bool prev(bool loop=false);

    virtual Menu* select();

protected:
    float _value;
    float _minValue;
    float _maxValue;
    float _increment;
    FormatValueFnPtr _format_value_fn;
};


class Menu : public MenuComponent
{
    friend class MenuSystem;
public:
    Menu(const char* name);

    void add_item(MenuItem* pItem);
    void add_menu(Menu* pMenu);

    MenuComponent const* get_current_component() const;
    MenuComponent const* get_menu_component(byte index) const;

    // TODO: get_current_component vs get_menu_component,
    // get_cur_menu_component_num.
    //
    // So, shoud it be get_num_components? I think so.
    byte get_num_menu_components() const;
    byte get_cur_menu_component_num() const;
    byte get_prev_menu_component_num() const;

    void render(MenuComponentRenderer const& renderer) const;

protected:
    void set_parent(Menu* pParent);
    Menu const* get_parent() const;

    Menu* activate();
    virtual bool next(bool loop=false);
    virtual bool prev(bool loop=false);
    virtual Menu* select();
    virtual void reset();

private:
    MenuComponent* _p_cur_menu_component;
    MenuComponent** _menu_components;
    Menu* _p_parent;
    byte _num_menu_components;
    byte _cur_menu_component_num;
    byte _prev_menu_component_num;
};


class MenuSystem
{
public:
    MenuSystem(MenuComponentRenderer const& renderer);

    void display() const;
    bool next(bool loop=false);
    bool prev(bool loop=false);
    void reset();
    void select(bool reset=false);
    bool back();

    Menu& get_root_menu() const;
    Menu const* get_current_menu() const;

private:
    Menu* _p_root_menu;
    Menu* _p_curr_menu;
    MenuComponentRenderer const& _renderer;
};


class MenuComponentRenderer
{
public:
    virtual void render(Menu const& menu) const = 0;

    virtual void render_menu_item(MenuItem const& menu_item) const = 0;
    virtual void render_back_menu_item(BackMenuItem const& menu_item) const = 0;
    virtual void render_numeric_menu_item(NumericMenuItem const& menu_item) const = 0;
    virtual void render_menu(Menu const& menu) const = 0;
};


#endif
