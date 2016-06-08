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
// TODO: c++11 or not? arduino-makefile doesn't support it by default but can
//       set flags. How many users are on a new enough arduino IDE for c++11?
// TODO: typedef all callbacks
// TODO: Move all implementation to cpp file. Find out why this sometimes
//       increases binary size. Then optimise consciously.

class MenuSystem;
class Menu;
class MenuComponentRenderer;

class MenuComponent
{
    friend class MenuSystem;
    friend class Menu;
public:
    MenuComponent(const char* name);

    void set_name(const char* name);
    const char* get_name() const;

    // TODO: Has to be public. Warn clients not to call this.
    virtual void render(MenuComponentRenderer const& renderer) const = 0;

    bool has_focus() const { return _has_focus; }

protected:
    virtual bool next(bool loop=false) = 0;
    virtual bool prev(bool loop=false) = 0;
    virtual void reset() = 0;
    virtual MenuComponent* select() = 0;

protected:
    const char* _name;
    bool _has_focus;
};


class MenuItem : public MenuComponent
{
public:
    MenuItem(const char* name, void (*on_select)(MenuItem*));

    void set_select_function(void (*on_select)(MenuItem*));

    virtual void render(MenuComponentRenderer const& renderer) const;

protected:
    virtual bool next(bool loop=false) { return false; }
    virtual bool prev(bool loop=false) { return false; }
    virtual void reset();
    virtual MenuComponent* select();

protected:
    void (*_on_select)(MenuItem*);
};

/**
 * A MenuItem executes MenuSystem::back() when selected.
 */
class BackMenuItem : public MenuItem
{
public:
    BackMenuItem(const char* name, void (*on_select)(MenuItem*), MenuSystem* ms);

    virtual void render(MenuComponentRenderer const& renderer) const;

protected:
    virtual MenuComponent* select();

protected:
    MenuSystem* menu_system;
};


class NumericMenuItem : public MenuItem
{
public:
    /**
     * Callback for formatting the numeric value into a String.
     *
     * @param value The value to convert.
     * @returns The String representation of value.
     */
    typedef const String (*ValueFormatter_t)(const float value);

public:
    /// Constructor
    ///
    /// @param name The name of the menu item.
    /// @param on_select The function to call when this MenuItem is selected.
    /// @param value Default value.
    /// @param minValue The minimum value.
    /// @param maxValue The maximum value.
    /// @param increment How much the value should be incremented by.
    /// @param valueFormatter The custom formatter. If NULL the String float
    ///                       formatter will be used.
    NumericMenuItem(const char* name, void (*on_select)(MenuItem*),
                    float value, float minValue, float maxValue,
                    float increment=1.0,
                    ValueFormatter_t value_formatter=NULL);

    /**
     * Sets the custom number formatter.
     *
     * @param numberFormat the custom formatter. If NULL the String float
     *                     formatter will be used (2 decimals)
     */
    void set_number_formatter(ValueFormatter_t value_formatter);


    float get_value() const { return _value; }
    float get_minValue() const { return _minValue; }
    float get_maxValue() const { return _maxValue; }

    String get_value_string() const
    {
        String buffer;
        if (_value_formatter != NULL)
            buffer += _value_formatter(_value);
        else
            buffer += _value;
        return buffer;
    }

    void set_value(float value) { _value = value; }

    virtual void render(MenuComponentRenderer const& renderer) const;


protected:
    virtual bool next(bool loop=false)
    {
        // TODO: Add loop support here! yay!
        _value += _increment;
        if (_value > _maxValue)
            _value = _maxValue;
        return true;
    }

    virtual bool prev(bool loop=false)
    {
        // TODO: Add loop support here! yay!
        _value -= _increment;
        if (_value < _minValue)
            _value = _minValue;
        return true;
    }

    virtual MenuComponent* select();


protected:
    float _value;
    float _minValue;
    float _maxValue;
    float _increment;
    ValueFormatter_t _value_formatter;
};


class Menu : public MenuComponent
{
    friend class MenuSystem;
public:
    Menu(const char* name);

    void add_item(MenuItem* pItem);
    void add_menu(Menu* pMenu);

    void set_parent(Menu* pParent);
    Menu const* get_parent() const;

    MenuComponent const* get_current_component() const;
    MenuComponent const* get_menu_component(byte index) const;

    byte get_num_menu_components() const;
    byte get_cur_menu_component_num() const;
    byte get_prev_menu_component_num() const;

    void render(MenuComponentRenderer const& renderer) const;

protected:
    MenuComponent* activate();
    virtual bool next(bool loop=false);
    virtual bool prev(bool loop=false);
    virtual MenuComponent* select();
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
    MenuSystem(Menu* p_root_menu, MenuComponentRenderer const& renderer);

    bool display() const;
    bool next(bool loop=false);
    bool prev(bool loop=false);
    void reset();
    void select(bool reset=false);
    bool back();

    Menu const* get_current_menu() const;

private:
    Menu* _p_root_menu;
    Menu* _p_curr_menu;
    MenuComponentRenderer const& _renderer;
};


class MenuComponentRenderer
{
public:
    virtual bool render(Menu const& menu) const = 0;

    virtual void render_menu_item(MenuItem const& menu_item) const = 0;
    virtual void render_back_menu_item(BackMenuItem const& menu_item) const = 0;
    virtual void render_numeric_menu_item(NumericMenuItem const& menu_item) const = 0;
    virtual void render_menu(Menu const& menu) const = 0;
};


#endif
