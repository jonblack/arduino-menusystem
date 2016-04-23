/*
 * serial_nav.ino - Example code using the menu system library
 *
 * This example shows the menu system being controlled over the serial port.
 *
 * Copyright (c) 2015, 2016 arduino-menusystem
 * Licensed under the MIT license (see LICENSE)
 */

#include <MenuSystem.h>
#include "CustomNumericMenuItem.h"

// forward declarations
const String format_float(const float value);
const String format_int(const float value);
const String format_color(const float value);

// Menu variables
MenuSystem ms;
Menu mm("ROOT Menu Title");
MenuItem mm_mi1("Level 1 - Item 1 (Item)");
MenuItem mm_mi2("Level 1 - Item 2 (Item)");
Menu mu1("Level 1 - Item 3 (Menu)");
BackMenuItem mu1_mi0(&ms, "Level 2 - Back (Item)");
MenuItem mu1_mi1("Level 2 - Item 1 (Item)");
NumericMenuItem mu1_mi2("Level 2 - Txt Item 2 (Item)", 0, 0, 2, 1, format_color);
CustomNumericMenuItem mu1_mi3(12, "Level 2 - Cust Item 3 (Item)", 80, 65, 121, 3, format_int);
NumericMenuItem mm_mi4("Level 1 - Float Item 4 (Item)", 0.5, 0.0, 1.0, 0.1, format_float);
NumericMenuItem mm_mi5("Level 1 - Int Item 5 (Item)", 50, -100, 100, 1, format_int);


// Menu callback function

// writes the (int) value of a float into a char buffer.
const String format_int(const float value)
{
    return String((int) value);
}

// writes the value of a float into a char buffer.
const String format_float(const float value)
{
    return String(value);
}

// writes the value of a float into a char buffer as predefined colors.
const String format_color(const float value)
{
    String buffer;

    switch((int) value){
        case 0:
            buffer += "Red";
            break;
        case 1:
            buffer += "Green";
            break;
        case 2:
            buffer += "Blue";
            break;
        default:
            buffer += "undef";
    }

    return buffer;
}

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

void on_back_item_selected(MenuItem* p_menu_item)
{
    Serial.println("Back item Selected");
}

void display_menu() {
    Menu const* cp_menu = ms.get_current_menu();

    Serial.print("\nCurrent menu name: ");
    Serial.println(cp_menu->get_name());

    String buffer;
    MenuComponent const* cp_menu_sel = cp_menu->get_selected();
    for (int i = 0; i < cp_menu->get_num_menu_components(); ++i)
    {
        MenuComponent const* cp_m_comp = cp_menu->get_menu_component(i);
        Serial.print(cp_m_comp->get_composite_name(buffer));

        if (cp_menu_sel == cp_m_comp)
            Serial.print("<<< ");

        Serial.println("");
    }
}


void display_help() {
    Serial.println("***************");
    Serial.println("w: go to previus item (up)");
    Serial.println("s: go to next item (down)");
    Serial.println("a: go back (right)");
    Serial.println("d: select \"selected\" item");
    Serial.println("?: print this help");
    Serial.println("h: print this help");
    Serial.println("***************");
}

void serial_handler()
{
    char inChar;
    if ((inChar = Serial.read()) > 0)
    {
        switch (inChar)
        {
            case 'w': // Previus item
                ms.prev();
                display_menu();
                break;
            case 's': // Next item
                ms.next();
                display_menu();
                break;
            case 'a': // Back presed
                ms.back();
                display_menu();
                break;
            case 'd': // Select presed
                ms.select();
                display_menu();
                break;
            case '?':
            case 'h': // Display help
                display_help();
                break;
            default:
                break;
        }
    }
}

// Standard arduino functions

void setup()
{
    Serial.begin(9600);

    mm.add_item(&mm_mi1, &on_item1_selected);
    mm.add_item(&mm_mi2, &on_item2_selected);
    mm.add_menu(&mu1);
    mu1.add_item(&mu1_mi0, &on_back_item_selected);
    mu1.add_item(&mu1_mi1, &on_item3_selected);
    mu1.add_item(&mu1_mi2);
    mu1.add_item(&mu1_mi3);
    mm.add_item(&mm_mi4);
    mm.add_item(&mm_mi5);
    ms.set_root_menu(&mm);

    display_help();
    display_menu();
}

void loop()
{
    serial_handler();
}
