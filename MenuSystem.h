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
class MenuComponent {
    friend class MenuSystem;
    friend class Menu;
public:
    //! \brief Callback for when the MenuComponent is selected
    //!
    //! \param menu_component The menu component being selected.
    using SelectFnPtr = void (*)(MenuComponent* menu_component);

public:
    //! \brief Construct a MenuComponent
    //! \param[in] name The name of the menu component that is displayed in
    //!                 clients.
    MenuComponent(const char* name, SelectFnPtr select_fn);

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

    //! \brief Returns true if this is the current component; false otherwise
    //!
    //! This bool registers if the component is the current selected component.
    //!
    //! Subclasses should use set_current() when the component becomes
    //! activated and use set_previous() once the component is no longer the
    //! current component.
    //!
    //! \returns true if this component is the current component, false
    //!          otherwise.
    //! \see MenuComponent::set_current
    bool is_current() const;

    //! \brief Sets the function to call when the MenuItem is selected
    //! \param[in] select_fn The function to call when the MenuItem is
    //!                      selected.
    void set_select_function(SelectFnPtr select_fn);

    enum Type { TypeMenu, TypeMenuItem, TypeNumericMenuItem, TypeBackMenuItem };
    //! \brief Returns the type of this item.
    //!
    //! Subclasses should reimplement this to return the correct type.
    virtual Type type() const = 0;

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
    //! The default implementation calls select_fn if it's not null. Components
    //! that derive from this class must call their parent's implementation.
    //!
    //! \returns The Menu instance selected or nullptr. The returned Menu
    //!          instance is used in MenuSystem::activate to set the current
    //!          menu in the MenuSystem.
    //!
    //! \see MenuComponent::has_focus
    //! \see NumericMenuComponent
    virtual Menu* select();

    //! \brief Set the current state of the component
    //!
    //! \paran is_current true if this component is the current one; false
    //!                   otherwise.
    //! \see is_current
    void set_current(bool is_current=true);

protected:
    const char* _name;
    bool _has_focus;
    bool _is_current;
    SelectFnPtr _select_fn;
};


//! \brief A MenuComponent that calls a callback function when selected.
//!
//! MenuItem represents the `Leaf` in the composite design pattern (see:
//! https://en.wikipedia.org/wiki/Composite_pattern). When a MenuItem is
//! selected, the user-defined MenuItem::_select_fn callback is called.
//!
//! \see MenuComponent
//! \see Menu
class MenuItem : public MenuComponent {
public:
    //! \brief Construct a MenuItem
    //! \param[in] name The name of the menu component that is displayed in
    //!                 clients.
    //! \param[in] select_fn The function to call when the MenuItem is
    //!                      selected.
    MenuItem(const char* name, SelectFnPtr select_fn);

    //! \copydoc MenuComponent::render
    virtual void render(MenuComponentRenderer const& renderer) const;
    virtual Type type() const;

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
};


//! \brief A MenuItem that calls MenuSystem::back() when selected.
//! \see MenuItem
class BackMenuItem : public MenuItem {
public:
    BackMenuItem(const char* name, SelectFnPtr select_fn, MenuSystem* ms);

    virtual void render(MenuComponentRenderer const& renderer) const;

    virtual Type type() const;
protected:
    virtual Menu* select();

protected:
    MenuSystem* _menu_system;
};


class NumericMenuItem : public MenuItem {
public:
    //! \brief Callback for formatting the numeric value into a String.
    //!
    //! \param value The value to convert.
    //! \returns The String representation of value.
    using FormatValueFnPtr = const String (*)(const float value);

public:
    //! Constructor
    //!
    //! @param name The name of the menu item.
    //! @param select_fn The function to call when this MenuItem is selected.
    //! @param value Default value.
    //! @param min_value The minimum value.
    //! @param max_value The maximum value.
    //! @param increment How much the value should be incremented by.
    //! @param format_value_fn The custom formatter. If nullptr the String
    //!                        float formatter will be used.
    NumericMenuItem(const char* name, SelectFnPtr select_fn,
                    float value, float min_value, float max_value,
                    float increment=1.0,
                    FormatValueFnPtr format_value_fn=nullptr);

    //!
    //! \brief Sets the custom number formatter.
    //!
    //! \param numberFormat the custom formatter. If nullptr the String float
    //!                     formatter will be used (2 decimals)
    //!
    void set_number_formatter(FormatValueFnPtr format_value_fn);

    float get_value() const;
    float get_min_value() const;
    float get_max_value() const;
    float get_increment() const;

    void set_value(float value);
    void set_min_value(float value);
    void set_max_value(float value);

    void set_increment(float increment);

    String get_formatted_value() const;

    virtual void render(MenuComponentRenderer const& renderer) const;

    virtual Type type() const;
protected:
    virtual bool next(bool loop=false);
    virtual bool prev(bool loop=false);

    virtual Menu* select();

protected:
    float _value;
    float _min_value;
    float _max_value;
    float _increment;
    FormatValueFnPtr _format_value_fn;
};


//! \brief A MenuComponent that can contain other MenuComponents.
//!
//! Menu represents the branch in the composite design pattern (see:
//! https://en.wikipedia.org/wiki/Composite_pattern). When a Menu is
//! selected, the user-defined Menu::_select_fn callback is called.
//!
//! \see MenuComponent
//! \see MenuItem
class Menu : public MenuComponent {
    friend class MenuSystem;
public:
    Menu(const char* name, SelectFnPtr select_fn=nullptr);

    //! \brief Adds a MenuItem to the Menu
    void add_item(MenuItem* p_item);

    //! \brief Adds a Menu to the Menu
    void add_menu(Menu* p_menu);

    MenuComponent const* get_current_component() const;
    MenuComponent const* get_menu_component(uint8_t index) const;

    uint8_t get_num_components() const;
    uint8_t get_current_component_num() const;
    uint8_t get_previous_component_num() const;

    //! \copydoc MenuComponent::render
    void render(MenuComponentRenderer const& renderer) const;

    virtual Type type() const;
protected:
    void set_parent(Menu* p_parent);
    Menu const* get_parent() const;

    //! \brief Activates the current selection
    //!
    //! When a client makes a selection, activate is called on the current menu
    //! which in turn calls the menu's current item's callback.
    Menu* activate();

    //! \copydoc MenuComponent::next
    virtual bool next(bool loop=false);

    //! \copydoc MenuComponent::prev
    virtual bool prev(bool loop=false);

    //! \copydoc MenuComponent::select
    virtual Menu* select();

    //! \copydoc MenuComponent::reset
    virtual void reset();

    void add_component(MenuComponent* p_component);

private:
    MenuComponent* _p_current_component;
    MenuComponent** _menu_components;
    Menu* _p_parent;
    uint8_t _num_components;
    uint8_t _current_component_num;
    uint8_t _previous_component_num;
};


class MenuSystem {
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


class MenuComponentRenderer {
public:
    virtual void render(Menu const& menu) const = 0;

    virtual void render_menu_item(MenuItem const& menu_item) const = 0;
    virtual void render_back_menu_item(BackMenuItem const& menu_item) const = 0;
    virtual void render_numeric_menu_item(NumericMenuItem const& menu_item) const = 0;
    virtual void render_menu(Menu const& menu) const = 0;
};


#endif
