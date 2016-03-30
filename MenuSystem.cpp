/*
 * Copyright (c) 2015 arduino-menusystem
 * Licensed under the MIT license (see LICENSE)
 */

#include "MenuSystem.h"

// *********************************************************
// MenuComponent
// *********************************************************

MenuComponent::MenuComponent(const char* name)
: _name(name)
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


// *********************************************************
// Menu
// *********************************************************

Menu::Menu(const char* name, void (*callback)(Menu*))
: MenuComponent(name),
  _disp_callback(callback),
  _p_sel_menu_component(NULL),
  _menu_components(NULL),
  _p_parent(NULL),
  _num_menu_components(0),
  _cur_menu_component_num(0),
  _prev_menu_component_num(0)
{
}


boolean Menu::display()
{
    if (_disp_callback) {
        (*_disp_callback)(this);
        return true;
    }
    return false;
}

void Menu::set_display_callback(void (*callback)(Menu*))
{
    _disp_callback = callback;
}


boolean Menu::next(boolean loop)
{
    _prev_menu_component_num = _cur_menu_component_num;
    if (!_num_menu_components) {
            return false;
    } else if (_cur_menu_component_num != _num_menu_components - 1)
    {
        _cur_menu_component_num++;
        _p_sel_menu_component = _menu_components[_cur_menu_component_num];

        return true;
    } else if (loop) {
        _cur_menu_component_num = 0;
        _p_sel_menu_component = _menu_components[_cur_menu_component_num];

        return true;
    }
    return false;
}

boolean Menu::prev(boolean loop)
{
    _prev_menu_component_num = _cur_menu_component_num;

    if (!_num_menu_components) {
            return false;
    } else if (_cur_menu_component_num != 0)
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
    if (!_num_menu_components) return NULL;

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

    _prev_menu_component_num = 0;
    _cur_menu_component_num = 0;
    _p_sel_menu_component = _num_menu_components ? _menu_components[0] : NULL;
}

void Menu::add_item(MenuItem* pItem, void (*on_select)(MenuItem*))
{
    // Resize menu component list, keeping existing items.
    // If it fails, there the item is not added and the function returns.
    _menu_components = (MenuComponent**) realloc(_menu_components,
                                                 (_num_menu_components + 1)
                                                 * sizeof(MenuComponent*));
    if (_menu_components == NULL)
      return;

    _menu_components[_num_menu_components] = pItem;

    pItem->set_select_function(on_select);

    if (_num_menu_components == 0)
        _p_sel_menu_component = pItem;

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

MenuComponent const* Menu::get_selected() const
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

byte Menu::get_prev_menu_component_num() const
{
    return _prev_menu_component_num;
}

// *********************************************************
// MenuItem
// *********************************************************

MenuItem::MenuItem(const char* name)
: MenuComponent(name),
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

void MenuItem::reset()
{
    // Do nothing.
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
    return _p_curr_menu->next(loop);
}

boolean MenuSystem::prev(boolean loop)
{
    return _p_curr_menu->prev(loop);
}

void MenuSystem::reset()
{
    _p_curr_menu = _p_root_menu;
    _p_root_menu->reset();
}

void MenuSystem::select(bool reset)
{
    MenuComponent* pComponent = _p_curr_menu->activate();

    if (pComponent != NULL) {
        _p_curr_menu = (Menu*) pComponent;
    } else if (reset){
        this->reset();
    }

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

Menu const* MenuSystem::get_current_menu() const
{
  return _p_curr_menu;
}

boolean MenuSystem::display()
{
    if (_p_curr_menu != NULL) return _p_curr_menu->display();
    return false;
}
