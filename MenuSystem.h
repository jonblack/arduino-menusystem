/*
  MenuSystem.h - Library for creating menu structures.
  Created by Jon Black, August 8th 2011.
  Released into the public domain.
  
  License: GPL 3
*/


#ifndef MENUSYSTEM_H
#define MENUSYSTEM_H

#if defined(ARDUINO) && ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif

// TODO: MenuComponent should have a pointer to prev and next menu item (linked list)

#define MAXIMUM_MENU_ITEMS 5


class MenuComponent
{
public:
    MenuComponent(char* name);
    
    void                    set_name(char* name);
    char*                   get_name();
    
    virtual MenuComponent*  select() = 0;

protected:
    char* _name;
};


class MenuItem : public MenuComponent
{
public:
    MenuItem(char* name);
    
    void set_select_function(void (*on_select)(MenuItem*));

    virtual MenuComponent*  select();

private:
    void (*_on_select)(MenuItem*);
};


class Menu : public MenuComponent
{
public:
    Menu(char* name);
    
    boolean                 next();
    boolean                 prev();
    MenuComponent*          activate();
    virtual MenuComponent*  select();
    
    void                    add_item(MenuItem* pItem, void (*on_select)(MenuItem*));
    Menu*                   add_menu(Menu* pMenu);
    
    void                    set_parent(Menu* pParent);
    Menu*                   get_parent() const;
    
    MenuComponent*          get_selected() const;
    byte                    get_num_menu_items() const;
    byte                    get_cur_menu_item() const;

private:
    MenuComponent* _p_sel_menu_component;
    MenuComponent* _menu_components[MAXIMUM_MENU_ITEMS];
    Menu*          _p_parent;
    byte           _num_menu_components;
    byte           _cur_menu_component;
};


// TODO: get this to create the root menu automatically and provide a function
//       to return a pointer to it.
class MenuSystem
{
public:
    MenuSystem();
    
    boolean         next();
    boolean         prev();
    void            select();
    void            back();
    
    void            set_root_menu(Menu* p_root_menu);
    
    char*           get_current_menu_name();
    byte            get_num_menu_items() const;
    byte            get_cur_menu_item() const;

private:
    Menu* _p_root_menu;
    Menu* _p_curr_menu;
};


#endif
