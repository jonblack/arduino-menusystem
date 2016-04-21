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

class MenuSystem;

class MenuComponent
{
    friend class MenuSystem;
public:
    MenuComponent(const char* name);

    void set_name(const char* name);
    /* returns _name + _value concatenation if applicable.
     * The method returns the same object as sent in, and it relies 
     * on the user to clean up any memory used by the String.
     */
    virtual String& get_composite_name(String& buffer) const;
    
    /* returns the un-altered name assigned by set_name() */
    const char* get_name() const;

    virtual MenuComponent* select() = 0;
    virtual void reset() = 0;
    /* returns true if the menu component is on modal/edit mode. */
    virtual bool is_modal() const {return false;}

protected:
    virtual bool modal_next() {return false;}
    virtual bool modal_prev() {return false;}

    const char* _name;
};

class MenuItem : public MenuComponent
{
public:
    MenuItem(const char* name);

    void set_select_function(void (*on_select)(MenuItem*));

    virtual MenuComponent* select();
    virtual void reset();

protected:
    void (*_on_select)(MenuItem*);
};

/**
 * This is a menu item that will execute MenuSystem::back() when selected.
 */
class BackMenuItem : public MenuItem
{
public:
    BackMenuItem(MenuSystem* ms, const char* name = "back");
    /* Call _on_select() function (if defined) and call MenuSystem::back() */
    virtual MenuComponent* select();
protected:
    MenuSystem* menu_system;
};

class NumericMenuItem;

/* The function signature for the number formatting callback of NumericMenuItem. 
 * When using this function you are supposed to append your custom formatting of
 * menuitem.get_value() to the buffer.
 */ 
typedef const String (*NumberFormat_t)(const float value);

class NumericMenuItem : public MenuItem
{
public:
    /* constructor for NumericMenuItem
     * @param name the name of the menu item
     * @param value default value
     * @param minValue the minimum value
     * @param maxValue the maximum value
     * @param increment how much next() and prev() should increment the value
     * @param numberFormat the custom formatter. If NULL the String float formatter will be used (2 decimals)
     */
    NumericMenuItem(const char* name, float value, float minValue, float maxValue, float increment=1.0, NumberFormat_t numberFormat=NULL );

    /* set the custom number formatter. 
     * @param numberFormat the custom formatter. If NULL the String float formatter will be used (2 decimals)
     */
    void set_number_formatter(NumberFormat_t numberFormat);

    /* Toggles modal mode and invokes the _on_select function (if not NULL) */
    virtual MenuComponent* select();

    /* Returns a concatenated string composed of _name and 
     * the user formatted _value.
     * Do not call this from inside the numberFormat function (infinite loop)
     * Use get_name() if you require the string set by set_name().
     * The returned string is the same object as sent in.
     */
    virtual String& get_composite_name(String& buffer) const;

    /* Returns the value */
    float get_value() const {return _value;}

    /* Sets the value */
    void set_value(float value) {_value = value;}
    
    /* Returns true if this menuitem is in modal mode. i.e. it is using next() 
     * and prev() events to increase and decrease its value.
     */
    virtual bool is_modal() const;

protected:
    virtual bool modal_next();
    virtual bool modal_prev();

    float _value;
    float _minValue;
    float _maxValue;
    float _increment;
    bool _modal;
    NumberFormat_t _numberFormat;
};

class Menu : public MenuComponent
{
    friend class MenuSystem;
public:
    Menu(const char* name);

    boolean next(boolean loop=false);
    boolean prev(boolean loop=false);
    MenuComponent* activate();
    virtual MenuComponent* select();
    virtual void reset();

    void add_item(MenuItem* pItem, void (*on_select)(MenuItem*) = NULL);
    Menu const* add_menu(Menu* pMenu);

    void set_parent(Menu* pParent);
    Menu const* get_parent() const;

    MenuComponent const* get_selected() const;
    MenuComponent const* get_menu_component(byte index) const;

    byte get_num_menu_components() const;
    byte get_cur_menu_component_num() const;
private:
    MenuComponent* _p_sel_menu_component;
    MenuComponent** _menu_components;
    Menu* _p_parent;
    byte _num_menu_components;
    byte _cur_menu_component_num;
};


class MenuSystem
{
public:
    MenuSystem();

    boolean next(boolean loop=false);
    boolean prev(boolean loop=false);
    void reset();
    void select(boolean reset=false);
    boolean back();

    void set_root_menu(Menu* p_root_menu);

    Menu const* get_current_menu() const;

private:
    Menu* _p_root_menu;
    Menu* _p_curr_menu;
};


#endif
