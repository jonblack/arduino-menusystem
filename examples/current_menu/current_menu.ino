/*
 * current_menu.ino - Example code using the menu system library.
 *
 * This example shows all items in the current menu.
 *
 * Copyright (c) 2015 arduino-menusystem
 * Licensed under the MIT license (see LICENSE)
 */

#include <MenuSystem.h>

// forward declarations
void on_item1_selected(MenuItem* p_menu_item);
void on_item2_selected(MenuItem* p_menu_item);
void on_item3_selected(MenuItem* p_menu_item);
void display_menu(Menu* p_menu);

// Menu variables
MenuSystem ms;
Menu mm("", &display_menu);
MenuItem mm_mi1("Level 1 - Item 1 (Item)", &on_item1_selected);
MenuItem mm_mi2("Level 1 - Item 2 (Item)", &on_item2_selected);
Menu mu1("Level 1 - Item 3 (Menu)", &display_menu);
MenuItem mu1_mi1("Level 2 - Item 1 (Item)", &on_item3_selected);

// Menu callback function

void on_item1_selected(MenuItem* p_menu_item)
{
  Serial.println("Item1 Selected");
}

void on_item2_selected(MenuItem* p_menu_item)
{
  Serial.println("Item2 Selected");
}

void on_item3_selected(MenuItem* p_menu_item)
{
  Serial.println("Item3 Selected");
  ms.reset();
  Serial.println("");
  ms.display();
}

void display_menu(Menu* p_menu)
{
  MenuComponent const* cp_menu_sel = p_menu->get_current_component();
  for (int i = 0; i < p_menu->get_num_menu_components(); ++i)
  {
    MenuComponent const* cp_m_comp = p_menu->get_menu_component(i);
    Serial.print(cp_m_comp->get_name());

    if (cp_menu_sel == cp_m_comp)
      Serial.print("<<< ");

    Serial.println("");
  }
}

// Standard arduino functions

void setup()
{
  Serial.begin(9600);

  // Menu setup
  mm.add_item(&mm_mi1);
  mm.add_item(&mm_mi2);
  mm.add_menu(&mu1);
  mu1.add_item(&mu1_mi1);
  ms.set_root_menu(&mm);
}

void loop()
{
  Serial.println("");

  ms.display();

  // Simulate using the menu by walking over the entire structure.
  ms.select();
  ms.next();

  // Wait for two seconds so the output is viewable
  delay(2000);
}
