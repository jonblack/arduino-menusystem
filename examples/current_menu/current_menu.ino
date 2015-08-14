/*
 * current_menu.ino - Example code using the menu system library.
 *
 * This example shows all items in the current menu.
 *
 * Copyright (c) 2015 arduino-menusystem
 * Licensed under the MIT license (see LICENSE)
 */

#include <MenuSystem.h>

// Menu variables
MenuSystem ms;
Menu mm("");
MenuItem mm_mi1("Level 1 - Item 1 (Item)");
MenuItem mm_mi2("Level 1 - Item 2 (Item)");
Menu mu1("Level 1 - Item 3 (Menu)");
MenuItem mu1_mi1("Level 2 - Item 1 (Item)");

// Example variables
bool bRanCallback = false;
bool bForward = true;

// Menu callback function
// In this example all menu items use the same callback.

void on_item1_selected(MenuItem* p_menu_item)
{
  Serial.println("Item1 Selected");
  bRanCallback = true;
  bForward = true;
}

void on_item2_selected(MenuItem* p_menu_item)
{
  Serial.println("Item2 Selected");
  bRanCallback = true;
}

void on_item3_selected(MenuItem* p_menu_item)
{
  Serial.println("Item3 Selected");
  bRanCallback = true;
  bForward = false;
}

// Standard arduino functions

void setup()
{
  Serial.begin(9600);
  
  // Menu setup
  mm.add_item(&mm_mi1, &on_item1_selected);
  mm.add_item(&mm_mi2, &on_item2_selected);
  mm.add_menu(&mu1);
  mu1.add_item(&mu1_mi1, &on_item3_selected);
  ms.set_root_menu(&mm);
}

void loop()
{
  Serial.println("");
  
  // Display the menu
  Menu const* cp_menu = ms.get_current_menu();
  MenuComponent const* cp_menu_sel = cp_menu->get_selected();
  for (int i = 0; i < cp_menu->get_num_menu_components(); ++i)
  {
    MenuComponent const* cp_m_comp = cp_menu->get_menu_component(i);
    Serial.print(cp_m_comp->get_name());
    
    if (cp_menu_sel == cp_m_comp)
      Serial.print("<<< ");
    
    Serial.println("");
  }
  
  // Simulate using the menu by walking over the entire structure.
  ms.select();
  
  if (bRanCallback)
  {
    if (bForward)
      ms.next();
    else
      ms.prev();
    bRanCallback = false;
  }
  
  // Wait for two seconds so the output is viewable
  delay(2000);
}
