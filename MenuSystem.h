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

#ifndef MENUSYSTEM_TEXTBUFFER_SIZE
#define MENUSYSTEM_TEXTBUFFER_SIZE 20
#endif

class MenuSystem;

class MenuComponent
{
	friend class MenuSystem;
public:
    MenuComponent(const char* name);

    void set_name(const char* name);
    virtual const char* get_name();

    virtual MenuComponent* select() = 0;
    virtual void reset() = 0;
    // modal methods
    virtual bool is_modal() const {return false;}

protected:
    virtual bool modal_next() {return false;}
    virtual bool modal_prev() {return false;}

    const char* _name;
};

/**
 * virtual base class for BackMenuItem and MenuItem
 */
class MenuItemBase : public MenuComponent
{
public:
	MenuItemBase(const char* name);
	virtual MenuComponent* select() = 0;
	virtual void reset() {}
};

/**
 * This is a menu item that will execute MenuSystem::back() when selected.
 */
class BackMenuItem : public MenuItemBase
{
public:
	BackMenuItem(MenuSystem* ms, const char* name = "back");

    virtual MenuComponent* select();
protected:
    MenuSystem* menu_system;
};

class MenuItem : public MenuItemBase
{
public:
    MenuItem(const char* name);

    void set_select_function(void (*on_select)(MenuItem*));

    virtual MenuComponent* select();

protected:
    void (*_on_select)(MenuItem*);
};


class NumericMenuItem : public MenuItem
{
public:
  NumericMenuItem(const char* basename, float value, float minValue, float maxValue, float increment=1.0, void (*numberFormat)(float value, char* buffer)=NULL );
  void set_number_formatter(void (*numberFormat)(float value, char* buffer));
  virtual MenuComponent* select();
  virtual const char* get_name();
  float get_value() {return _value;}
  virtual bool is_modal() const;

protected:
  virtual bool modal_next();
  virtual bool modal_prev();

  float _value;
  //char _valueAsText[NUMERIC_MENU_ITEM_BUFFER];
  float _minValue;
  float _maxValue;
  float _increment;
  bool _modal;
  void (*_numberFormat)(float value, char* buffer);
};

class Menu : public MenuComponent
{
public:
    Menu(const char* name);

    boolean next(boolean loop=false);
    boolean prev(boolean loop=false);
    MenuComponent* activate();
    virtual MenuComponent* select();
    virtual void reset();

    void add_item(MenuItemBase* pItem);
    void add_item(MenuItem* pItem, void (*on_select)(MenuItem*));
    Menu const* add_menu(Menu* pMenu);

    void set_parent(Menu* pParent);
    Menu const* get_parent() const;

    MenuComponent* get_selected() const;
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

    Menu * get_current_menu() const;

private:
    Menu* _p_root_menu;
    Menu* _p_curr_menu;
};


#endif
