/*
 * current_item.ino - Example code using the menu system library.
 *
 * This example shows only the current menu item.
 *
 * Copyright (c) 2015 arduino-menusystem
 * Licensed under the MIT license (see LICENSE)
 */

#include <MenuSystem.h>

// forward declarations

void on_item1_selected(MenuItem* p_menu_item);
void on_item2_selected(MenuItem* p_menu_item);
void on_item3_selected(MenuItem* p_menu_item);
void on_item4_selected(MenuItem* p_menu_item);

// Menu variables
MenuSystem ms;
Menu mm("");
MenuItem mm_mi1("Level 1 - Item 1 (Item)", &on_item1_selected);
MenuItem mm_mi2("Level 1 - Item 2 (Item)", &on_item2_selected);
Menu mu1("Level 1 - Item 3 (Menu)");
MenuItem mu1_mi1("Level 2 - Item 1 (Item)", &on_item3_selected);
MenuItem mu1_mi2("Level 2 - Item 2 (Item)", &on_item4_selected);

// Menu callback function
// In this example all menu items use the same callback.

bool bRanCallback = false;

void on_item1_selected(MenuItem* p_menu_item)
{
  Serial.println("Item1 Selected");
  bRanCallback = true;
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
}

void on_item4_selected(MenuItem* p_menu_item)
{
  Serial.println("Item4 Selected");
  bRanCallback = false;
  ms.reset();
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
  mu1.add_item(&mu1_mi2);
  ms.set_root_menu(&mm);
}

void loop()
{
  Serial.println(ms.get_current_menu()->get_current_component()->get_name());
  ms.select();
  if (bRanCallback)
  {
      ms.next();
      bRanCallback = false;
  }

  // Wait for two seconds so the output is viewable
  delay(2000);
}
