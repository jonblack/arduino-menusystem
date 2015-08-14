/*
 * serial_nav.ino - Example code using the menu system library
 *
 * This example shows the menu system being controlled over the serial port.
 *
 * Copyright (c) 2015 arduino-menusystem
 * Licensed under the MIT license (see LICENSE)
 */

#include <MenuSystem.h>

// Menu variables
MenuSystem ms;
Menu mm("ROOT Menu Title");
MenuItem mm_mi1("Level 1 - Item 1 (Item)");
MenuItem mm_mi2("Level 1 - Item 2 (Item)");
Menu mu1("Level 1 - Item 3 (Menu)");
MenuItem mu1_mi1("Level 2 - Item 1 (Item)");


// Menu callback function
// In this example all menu items use the same callback.

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
}

// Standard arduino functions

void setup()
{
  Serial.begin(9600);
  serialPrintHelp();
  Serial.println("Setting up the menu.");
  // Menu setup
  /*
  Menu Structure:
   -Item1
   -Item2
   -Item3
   --Item1
   
   */
  mm.add_item(&mm_mi1, &on_item1_selected);
  mm.add_item(&mm_mi2, &on_item2_selected);
  mm.add_menu(&mu1);
  mu1.add_item(&mu1_mi1, &on_item3_selected);
  ms.set_root_menu(&mm);
  Serial.println("Menu setted.");
  displayMenu();
}

void loop()
{
  // Handle serial commands
  serialHandler();

  // Wait for two seconds so the output is viewable
  //delay(2000);
}

void displayMenu() {
  Serial.println("");
  // Display the menu
  Menu const* cp_menu = ms.get_current_menu();

  Serial.print("Current menu name: ");
  Serial.println(cp_menu->get_name());

  MenuComponent const* cp_menu_sel = cp_menu->get_selected();
  for (int i = 0; i < cp_menu->get_num_menu_components(); ++i)
  {
    MenuComponent const* cp_m_comp = cp_menu->get_menu_component(i);
    Serial.print(cp_m_comp->get_name());

    if (cp_menu_sel == cp_m_comp)
      Serial.print("<<< ");

    Serial.println("");
  }
}

void serialHandler() {
  char inChar;
  if((inChar = Serial.read())>0) {
    switch (inChar) {
    case 'w': // Previus item
      ms.prev();
      displayMenu();
      break;
    case 's': // Next item
      ms.next();
      displayMenu();
      break;
    case 'a': // Back presed
      ms.back();
      displayMenu();
      break;
    case 'd': // Select presed
      ms.select();
      displayMenu();
      break;
    case '?':
    case 'h': // Display help
      serialPrintHelp();
      break;
    default:
      break;
    }
  }
}

void serialPrintHelp() {
  Serial.println("***************");
  Serial.println("w: go to previus item (up)");
  Serial.println("s: go to next item (down)");
  Serial.println("a: go back (right)");
  Serial.println("d: select \"selected\" item");
  Serial.println("?: print this help");
  Serial.println("h: print this help");
  Serial.println("***************");

}

