/*
 * Copyright (c) 2015, 2016 arduino-menusystem
 * Licensed under the MIT license (see LICENSE)
 */

#include "MenuSystem.h"
#include <stdlib.h>

char menuSystemTextBuffer[MENUSYSTEM_TEXTBUFFER_SIZE];

// *********************************************************
// MenuComponent
// *********************************************************

MenuComponent::MenuComponent(const char* name)
: _name(name)
{
}

const char* MenuComponent::get_name()
{
    return _name;
}

void MenuComponent::set_name(const char* name)
{
    _name = name;
}


// *********************************************************
// Menu
// *********************************************************

Menu::Menu(const char* name)
: MenuComponent(name),
  _p_sel_menu_component(NULL),
  _menu_components(NULL),
  _p_parent(NULL),
  _num_menu_components(0),
  _cur_menu_component_num(0)
{
}

boolean Menu::next(boolean loop)
{
    if (_cur_menu_component_num != _num_menu_components - 1)
    {
        _cur_menu_component_num++;
        _p_sel_menu_component = _menu_components[_cur_menu_component_num];

        return true;
    } else if (loop)
    {
        _cur_menu_component_num = 0;
        _p_sel_menu_component = _menu_components[_cur_menu_component_num];

        return true;
    }

    return false;
}

boolean Menu::prev(boolean loop)
{
    if (_cur_menu_component_num != 0)
    {
        _cur_menu_component_num--;
        _p_sel_menu_component = _menu_components[_cur_menu_component_num];

        return true;
    } else if (loop)
    {
        _cur_menu_component_num = _num_menu_components - 1;
        _p_sel_menu_component = _menu_components[_cur_menu_component_num];

        return true;
    }

    return false;
}

MenuComponent* Menu::activate()
{
    MenuComponent* pComponent = _menu_components[_cur_menu_component_num];

    if (pComponent == NULL)
        return NULL;

    return pComponent->select();
}

MenuComponent* Menu::select()
{
    return this;
}

void Menu::reset()
{
    for (int i = 0; i < _num_menu_components; ++i)
        _menu_components[i]->reset();
    _cur_menu_component_num = 0;
    _p_sel_menu_component = _menu_components[0];
}

void Menu::add_item(MenuItemBase* pItem)
{
    // Resize menu component list, keeping existing items.
    // If it fails, there the item is not added and the function returns.
    _menu_components = (MenuComponent**) realloc(_menu_components,
                                                 (_num_menu_components + 1)
                                                 * sizeof(MenuComponent*));
    if (_menu_components == NULL)
      return;

    _menu_components[_num_menu_components] = pItem;

    if (_num_menu_components == 0)
        _p_sel_menu_component = pItem;

    _num_menu_components++;
}

void Menu::add_item(MenuItem* pItem, void (*on_select)(MenuItem*))
{
	add_item(pItem);
    pItem->set_select_function(on_select);
}

Menu const* Menu::get_parent() const
{
    return _p_parent;
}

void Menu::set_parent(Menu* pParent)
{
    _p_parent = pParent;
}

Menu const* Menu::add_menu(Menu* pMenu)
{
    // Resize menu component list, keeping existing items.
    // If it fails, there the item is not added and the function returns.
    _menu_components = (MenuComponent**) realloc(_menu_components,
                                                 (_num_menu_components + 1)
                                                 * sizeof(MenuComponent*));
    if (_menu_components == NULL)
      return NULL;

    pMenu->set_parent(this);

    _menu_components[_num_menu_components] = pMenu;

    if (_num_menu_components == 0)
        _p_sel_menu_component = pMenu;

    _num_menu_components++;

    return pMenu;
}

MenuComponent const* Menu::get_menu_component(byte index) const
{
  return _menu_components[index];
}

MenuComponent* Menu::get_selected() const
{
    return _p_sel_menu_component;
}

byte Menu::get_num_menu_components() const
{
    return _num_menu_components;
}

byte Menu::get_cur_menu_component_num() const
{
    return _cur_menu_component_num;
}

// *********************************************************
// MenuItemBase
// *********************************************************
MenuItemBase::MenuItemBase(const char* name): MenuComponent(name) {
}

// *********************************************************
// BackMenuItem
// *********************************************************
BackMenuItem::BackMenuItem(MenuSystem* ms, const char* name): MenuItemBase(name), menu_system(ms) {
}

MenuComponent* BackMenuItem::select()
{
	if (menu_system!=NULL){
  	  menu_system->back();
	}
    return 0;
}

// *********************************************************
// MenuItem
// *********************************************************

MenuItem::MenuItem(const char* name)
: MenuItemBase(name),
  _on_select(0)
{
}

void MenuItem::set_select_function(void (*on_select)(MenuItem*))
{
    _on_select = on_select;
}

MenuComponent* MenuItem::select()
{
    if (_on_select != NULL)
        _on_select(this);

    return 0;
}

// *********************************************************
// NumericMenuItem
// *********************************************************

NumericMenuItem::NumericMenuItem(const char* basename, float value, float minValue, float maxValue, float increment, void (*numberFormat)(float value, char* buffer)):
    MenuItem(basename), _value(value), _minValue(minValue), _maxValue(maxValue), _increment(increment), _modal(false), _numberFormat(numberFormat)
{
  menuSystemTextBuffer[0] = 0;
  if (increment < 0.0) increment = -increment;
  if (minValue > maxValue){
	  float tmp = maxValue;
	  maxValue = minValue;
	  minValue = tmp;
  }
};

void NumericMenuItem::set_number_formatter(void (*numberFormat)(float value, char* buffer)){
	_numberFormat = numberFormat;
}

bool NumericMenuItem::is_modal() const {
  return _modal;
}

MenuComponent* NumericMenuItem::select() {
  _modal = !_modal;
  // only run _on_select when the user is done editing the value
  if (!_modal && _on_select != NULL)
      _on_select(this);
  return NULL;
}

bool NumericMenuItem::modal_next(){
  _value += _increment;
  if (_value > _maxValue) _value = _maxValue;
  return true;
}

bool NumericMenuItem::modal_prev(){
  _value -= _increment;
  if (_value < _minValue) _value = _minValue;
  return true;
}

const char* NumericMenuItem::get_name() {
  int i = strlen(_name);
  memcpy( menuSystemTextBuffer, _name, i );
  if (is_modal()) {
	menuSystemTextBuffer[i++] = '<';
  } else {
	menuSystemTextBuffer[i++] = '=';
  }
  if (_numberFormat!=NULL) {
    _numberFormat(_value, menuSystemTextBuffer+i);
    i+=strlen(menuSystemTextBuffer+i);
  } else {
    i+=strlen(dtostrf(_value, 5, 2, menuSystemTextBuffer+i));
  }

  if (is_modal()) {
	  menuSystemTextBuffer[i++] = '>';
  }
  menuSystemTextBuffer[i] = 0;
  return menuSystemTextBuffer;
}

// *********************************************************
// MenuSystem
// *********************************************************

MenuSystem::MenuSystem()
: _p_root_menu(NULL),
  _p_curr_menu(NULL)
{
}

boolean MenuSystem::next(boolean loop)
{
    if (_p_curr_menu->get_selected()->is_modal()) {
      _p_curr_menu->get_selected()->modal_next();
      return true;
    } else {
      return _p_curr_menu->next(loop);
    }
}

boolean MenuSystem::prev(boolean loop)
{
  if (_p_curr_menu->get_selected()->is_modal()) {
    _p_curr_menu->get_selected()->modal_prev();
    return true;
  } else {
    return _p_curr_menu->prev(loop);
  }
}

void MenuSystem::reset()
{
    _p_curr_menu = _p_root_menu;
    _p_root_menu->reset();
}

void MenuSystem::select(bool reset)
{
    MenuComponent* pComponent = _p_curr_menu->activate();

    if (pComponent != NULL)
        _p_curr_menu = (Menu*) pComponent;
    else
        if (reset) this->reset();
}

boolean MenuSystem::back()
{
    if (_p_curr_menu != _p_root_menu)
    {
        _p_curr_menu = const_cast<Menu*>(_p_curr_menu->get_parent());
        return true;
    }

    // We are already in the root menu
    return false;
}

void MenuSystem::set_root_menu(Menu* p_root_menu)
{
    _p_root_menu = p_root_menu;
    _p_curr_menu = p_root_menu;
}

Menu * MenuSystem::get_current_menu() const
{
  return _p_curr_menu;
}
