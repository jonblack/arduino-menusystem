/*
 * Copyright (c) 2015, 2016 arduino-menusystem
 * Licensed under the MIT license (see LICENSE)
 */

#include "MenuSystem.h"
#include <stdlib.h>


// *********************************************************
// MenuComponent
// *********************************************************

MenuComponent::MenuComponent(const char* name)
: _name(name),
  _has_focus(false)
{
}

const char* MenuComponent::get_name() const
{
    return _name;
}

void MenuComponent::set_name(const char* name)
{
    _name = name;
}

bool MenuComponent::has_focus() const
{
    return _has_focus;
}

// *********************************************************
// Menu
// *********************************************************

Menu::Menu(const char* name)
: MenuComponent(name),
  _p_cur_menu_component(NULL),
  _menu_components(NULL),
  _p_parent(NULL),
  _num_menu_components(0),
  _cur_menu_component_num(0),
  _prev_menu_component_num(0)
{
}

bool Menu::next(bool loop)
{
    _prev_menu_component_num = _cur_menu_component_num;

    if (!_num_menu_components)
    {
        return false;
    }
    else if (_cur_menu_component_num != _num_menu_components - 1)
    {
        _cur_menu_component_num++;
        _p_cur_menu_component = _menu_components[_cur_menu_component_num];

        return true;
    }
    else if (loop)
    {
        _cur_menu_component_num = 0;
        _p_cur_menu_component = _menu_components[_cur_menu_component_num];

        return true;
    }
    return false;
}

bool Menu::prev(bool loop)
{
    _prev_menu_component_num = _cur_menu_component_num;

    if (!_num_menu_components)
    {
        return false;
    }
    else if (_cur_menu_component_num != 0)
    {
        _cur_menu_component_num--;
        _p_cur_menu_component = _menu_components[_cur_menu_component_num];

        return true;
    }
    else if (loop)
    {
        _cur_menu_component_num = _num_menu_components - 1;
        _p_cur_menu_component = _menu_components[_cur_menu_component_num];

        return true;
    }
    return false;
}

Menu* Menu::activate()
{
    if (!_num_menu_components)
        return NULL;

    MenuComponent* pComponent = _menu_components[_cur_menu_component_num];

    if (pComponent == NULL)
        return NULL;

    return pComponent->select();
}

Menu* Menu::select()
{
    return this;
}

void Menu::reset()
{
    for (int i = 0; i < _num_menu_components; ++i)
        _menu_components[i]->reset();

    _prev_menu_component_num = 0;
    _cur_menu_component_num = 0;
    _p_cur_menu_component = _num_menu_components ? _menu_components[0] : NULL;
}

void Menu::add_item(MenuItem* pItem)
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
        _p_cur_menu_component = pItem;

    _num_menu_components++;
}

Menu const* Menu::get_parent() const
{
    return _p_parent;
}

void Menu::set_parent(Menu* pParent)
{
    _p_parent = pParent;
}

void Menu::add_menu(Menu* pMenu)
{
    // Resize menu component list, keeping existing items.
    // If it fails, there the item is not added and the function returns.
    _menu_components = (MenuComponent**) realloc(_menu_components,
                                                 (_num_menu_components + 1)
                                                 * sizeof(MenuComponent*));
    if (_menu_components == NULL)
      return;

    pMenu->set_parent(this);

    _menu_components[_num_menu_components] = pMenu;

    if (_num_menu_components == 0)
        _p_cur_menu_component = pMenu;

    _num_menu_components++;
}

MenuComponent const* Menu::get_menu_component(byte index) const
{
    return _menu_components[index];
}

MenuComponent const* Menu::get_current_component() const
{
    return _p_cur_menu_component;
}

byte Menu::get_num_menu_components() const
{
    return _num_menu_components;
}

byte Menu::get_cur_menu_component_num() const
{
    return _cur_menu_component_num;
}

byte Menu::get_prev_menu_component_num() const
{
    return _prev_menu_component_num;
}

void Menu::render(MenuComponentRenderer const& renderer) const
{
    renderer.render_menu(*this);
}

// *********************************************************
// BackMenuItem
// *********************************************************

BackMenuItem::BackMenuItem(const char* name, void (*on_select)(MenuItem*),
                           MenuSystem* ms)
: MenuItem(name, on_select),
  menu_system(ms)
{
}

Menu* BackMenuItem::select()
{
    if (_on_select!=NULL)
        _on_select(this);

    if (menu_system!=NULL)
        menu_system->back();

    return NULL;
}

void BackMenuItem::render(MenuComponentRenderer const& renderer) const
{
    renderer.render_back_menu_item(*this);
}

// *********************************************************
// MenuItem
// *********************************************************

MenuItem::MenuItem(const char* name, void (*on_select)(MenuItem*))
: MenuComponent(name),
  _on_select(on_select)
{
}

void MenuItem::set_select_function(void (*on_select)(MenuItem*))
{
    _on_select = on_select;
}

Menu* MenuItem::select()
{
    if (_on_select != NULL)
        _on_select(this);

    return NULL;
}

void MenuItem::reset()
{
    // Do nothing.
}

void MenuItem::render(MenuComponentRenderer const& renderer) const
{
    renderer.render_menu_item(*this);
}

bool MenuItem::next(bool loop)
{
    return false;
}

bool MenuItem::prev(bool loop)
{
    return false;
}

// *********************************************************
// NumericMenuItem
// *********************************************************

NumericMenuItem::NumericMenuItem(const char* basename,
                                 void (*on_select)(MenuItem*), float value,
                                 float minValue, float maxValue,
                                 float increment,
                                 ValueFormatter_t value_formatter)
: MenuItem(basename, on_select),
  _value(value),
  _minValue(minValue),
  _maxValue(maxValue),
  _increment(increment),
  _value_formatter(value_formatter)
{
    if (_increment < 0.0) _increment = -_increment;
    if (_minValue > _maxValue)
    {
        float tmp = _maxValue;
        _maxValue = _minValue;
        _minValue = tmp;
    }
};

void NumericMenuItem::set_number_formatter(ValueFormatter_t value_formatter)
{
    _value_formatter = value_formatter;
}

Menu* NumericMenuItem::select()
{
    _has_focus = !_has_focus;

    // Only run _on_select when the user is done editing the value
    if (!_has_focus && _on_select != NULL)
        _on_select(this);
    return NULL;
}

void NumericMenuItem::render(MenuComponentRenderer const& renderer) const
{
    renderer.render_numeric_menu_item(*this);
}

float NumericMenuItem::get_value() const
{
    return _value;
}

float NumericMenuItem::get_minValue() const
{
    return _minValue;
}

float NumericMenuItem::get_maxValue() const
{
    return _maxValue;
}

String NumericMenuItem::get_value_string() const
{
    String buffer;
    if (_value_formatter != NULL)
        buffer += _value_formatter(_value);
    else
        buffer += _value;
    return buffer;
}

void NumericMenuItem::set_value(float value)
{
    _value = value;
}


bool NumericMenuItem::next(bool loop)
{
    // TODO: Add loop support here! yay!
    _value += _increment;
    if (_value > _maxValue)
        _value = _maxValue;
    return true;
}

bool NumericMenuItem::prev(bool loop)
{
    // TODO: Add loop support here! yay!
    _value -= _increment;
    if (_value < _minValue)
        _value = _minValue;
    return true;
}

// *********************************************************
// MenuSystem
// *********************************************************

MenuSystem::MenuSystem(MenuComponentRenderer const& renderer)
: _p_root_menu(new Menu("")),
  _p_curr_menu(_p_root_menu),
  _renderer(renderer)
{
}

bool MenuSystem::next(bool loop)
{
    if (_p_curr_menu->_p_cur_menu_component->has_focus())
        return _p_curr_menu->_p_cur_menu_component->next(loop);
    else
        return _p_curr_menu->next(loop);
}

bool MenuSystem::prev(bool loop)
{
    if (_p_curr_menu->_p_cur_menu_component->has_focus())
        return _p_curr_menu->_p_cur_menu_component->prev(loop);
    else
        return _p_curr_menu->prev(loop);
}

void MenuSystem::reset()
{
    _p_curr_menu = _p_root_menu;
    _p_root_menu->reset();
}

void MenuSystem::select(bool reset)
{
    Menu* pMenu = _p_curr_menu->activate();

    if (pMenu != NULL)
        _p_curr_menu = pMenu;
    else
        if (reset)
            this->reset();
}

bool MenuSystem::back()
{
    if (_p_curr_menu != _p_root_menu)
    {
        _p_curr_menu = const_cast<Menu*>(_p_curr_menu->get_parent());
        return true;
    }

    // We are already in the root menu
    return false;
}

Menu& MenuSystem::get_root_menu() const
{
    return *_p_root_menu;
}

Menu const* MenuSystem::get_current_menu() const
{
    return _p_curr_menu;
}

void MenuSystem::display() const
{
    if (_p_curr_menu != NULL)
        _renderer.render(*_p_curr_menu);
}
