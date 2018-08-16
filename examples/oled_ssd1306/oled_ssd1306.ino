/*
 * oled_ssd1306.ino - Example code using the menu system library
 *
 * This example shows using the menu system with an OLED display controlled
 * controled over serial.
 *
 * It is targeted at a 128x64 or 128x32 OLED display that is based on the
 * SSD1306 driver IC (most on eBay and Amazon are) and connected via I2C.
 * It depends on the "SSD1306Ascii" library which can be found in the Arduino
 * IDE Library Manager or at https://github.com/greiman/SSD1306Ascii.
 *
 * If your particular display supports SPI then this code can be easily
 * modified based on the SPI examples in the "SSD1306Ascii" library.
 *
 * Contributed by Matthew Nielsen with all rights transfered to Jon Black
 * Copyright (c) 2015 arduino-menusystem
 * Licensed under the MIT license (see LICENSE)
 */

#include <MenuSystem.h>

/* The address of the OLED display. 0x3C is the default for of most modules */
#define I2C_ADDRESS 0x3C

#include <Arduino.h> // Used by SSD1306Ascii
/* Wire.h is not always required, depending on target platform */
// #include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
SSD1306AsciiWire oled;

class MyRenderer : public MenuComponentRenderer {
public:
    void render(Menu const& menu) const {
        oled.clear();

        if (menu.get_name() == "") {
            oled.println("Main Menu");
        } else {
            oled.println(menu.get_name());
        }

        for (int i = 0; i < menu.get_num_components(); ++i) {
            MenuComponent const* cp_m_comp = menu.get_menu_component(i);
            if (cp_m_comp->is_current()) {
                oled.print(">");
            } else {
                oled.print(" ");
            }
            cp_m_comp->render(*this);

            oled.println("");
        }
    }

    void render_menu_item(MenuItem const& menu_item) const {
        oled.print(menu_item.get_name());
    }

    void render_back_menu_item(BackMenuItem const& menu_item) const {
        oled.print(menu_item.get_name());
    }

    void render_numeric_menu_item(NumericMenuItem const& menu_item) const {
        oled.print(menu_item.get_name());
    }

    void render_menu(Menu const& menu) const {
        oled.print(menu.get_name());
    }
};
MyRenderer my_renderer;

// Forward declarations

void on_item1_selected(MenuComponent* p_menu_component);
void on_item2_selected(MenuComponent* p_menu_component);
void on_item3_selected(MenuComponent* p_menu_component);

// Menu variables

MenuSystem ms(my_renderer);
MenuItem mm_mi1("Level 1 - Item 1 (Item)", &on_item1_selected);
MenuItem mm_mi2("Level 1 - Item 2 (Item)", &on_item2_selected);
Menu mu1("Level 1 - Item 3 (Menu)");
MenuItem mu1_mi1("Level 2 - Item 1 (Item)", on_item3_selected);

// Menu callback function

void on_item1_selected(MenuComponent* p_menu_component) {
    oled.clear();
    oled.print("Item1 Selected");
    delay(1500); // so we can look the result on the display
}

void on_item2_selected(MenuComponent* p_menu_component) {
    oled.clear();
    oled.print("Item2 Selected");
    delay(1500); // so we can look the result on the display
}

void on_item3_selected(MenuComponent* p_menu_component) {
    oled.clear();
    oled.print("Item3 Selected");
    delay(1500); // so we can look the result on the display
}

void serial_print_help() {
    Serial.println("***************");
    Serial.println("w: go to previus item (up)");
    Serial.println("s: go to next item (down)");
    Serial.println("a: go back (right)");
    Serial.println("d: select \"selected\" item");
    Serial.println("?: print this help");
    Serial.println("h: print this help");
    Serial.println("***************");
}

void serial_handler() {
    char inChar;
    if ((inChar = Serial.read()) > 0) {
        switch (inChar) {
            case 'w': // Previus item
                ms.prev();
                ms.display();
                break;
            case 's': // Next item
                ms.next();
                ms.display();
                break;
            case 'a': // Back presed
                ms.back();
                ms.display();
                break;
            case 'd': // Select presed
                ms.select();
                ms.display();
                break;
            case '?':
            case 'h': // Display help
                serial_print_help();
                break;
            default:
                break;
        }
    }
}

// Standard arduino functions

void setup() {
    Serial.begin(9600);

    // Initialize the OLED display
    Wire.begin();
    oled.begin(&Adafruit128x64, I2C_ADDRESS);
    oled.setFont(System5x7);
    oled.print("Display Initialized");

    serial_print_help();

    ms.get_root_menu().add_item(&mm_mi1);
    ms.get_root_menu().add_item(&mm_mi2);
    ms.get_root_menu().add_menu(&mu1);
    mu1.add_item(&mu1_mi1);

    ms.display();
}

void loop() {
    serial_handler();
}
