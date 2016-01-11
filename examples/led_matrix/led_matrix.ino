/*
 * current_item.ino - Example code using the menu system library.
 *
 * This example demonstrates using the library with a Sure Electronics 32x16
 * LED matrix.
 *
 * Copyright (c) 2015 arduino-menusystem
 * Licensed under the MIT license (see LICENSE)
 */

#include <ht1632c.h>
#include <MenuSystem.h>

// Display constants

#define PIN_LED_DATA  0
#define PIN_LED_WR    3
#define PIN_LED_CLOCK 5
#define PIN_LED_CS    4
#define LED_WIDTH     32
#define LED_HEIGHT    16
#define FONT_WIDTH    5
#define FONT_HEIGHT   7

// Menu variables

MenuSystem ms;
Menu mm("");
MenuItem mi_time("TIME");
MenuItem mi_date("DATE");
Menu mu_disp("DISP*");
MenuItem mi_brightness("BRTNS");
MenuItem mi_color("COLOR");

// Display variables

ht1632c ledMatrix = ht1632c(&PORTB, PIN_LED_DATA, PIN_LED_WR, PIN_LED_CLOCK, PIN_LED_CS, GEOM_32x16, 2);
byte color = RED;

// Functions

void update_menu() {
    ledMatrix.clear();

    int y_idnt = (LED_HEIGHT / 2) - (FONT_HEIGHT / 2);
    ledMatrix.setfont(FONT_5x7);

    Menu const* menu = ms.get_current_menu();
    const char* name = menu->get_selected()->get_name();

    // Calculate indentation for center alignment
    int text_width = FONT_WIDTH * strlen(name);
    int pixel_spare = LED_WIDTH - text_width;
    int x_idnt = (int) floor(pixel_spare / 2);

    // Display text
    for (int i = 0; i < strlen(name); i++) {
        ledMatrix.putchar((i * FONT_WIDTH) + x_idnt, y_idnt, name[i], color);
    }
    ledMatrix.sendframe();
}

bool process_serial() {
    char inChar;
    if ((inChar = Serial.read()) > 0) {
        switch (inChar) {
            case 'w':
                if (ms.prev()) {
                    Serial.println("Going to previous menu item");
                    return true;
                } else {
                    return false;
                }
            case 's':
                if (ms.next()) {
                    Serial.println("Going to mext menu item");
                    return true;
                } else {
                    return false;
                }
            case 'a':
                if (ms.back()) {
                    Serial.println("Going up a level");
                    return true;
                } else {
                    return false;
                }
            case 'd':
                Serial.println("Selecting menu item");
                ms.select();
                return true;
            default:
                return false;
        }
    }

    return false;
}

// Menu callback functions

void on_time_selected(MenuItem* p_menu_item) {
    Serial.println("Time selected");
}

void on_date_selected(MenuItem* p_menu_item) {
    Serial.println("Date selected");
}

void on_brightness_selected(MenuItem* p_menu_item) {
    Serial.println("Brightness selected");
}

void on_color_selected(MenuItem* p_menu_item) {
    Serial.println("Color selected");
    if (color == RED) {
        color = GREEN;
    } else {
        color = RED;
    }
}

// Standard arduino functions

void setup() {
    Serial.begin(9600);

    // LED matrix
    ledMatrix.clear();

    // Menu
    mm.add_item(&mi_time, &on_time_selected);
    mm.add_item(&mi_date, &on_date_selected);
    mm.add_menu(&mu_disp);
    mu_disp.add_item(&mi_brightness, &on_brightness_selected);
    mu_disp.add_item(&mi_color, &on_color_selected);
    ms.set_root_menu(&mm);

    update_menu();
}

void loop() {
    delay(100);

    bool changed = process_serial();
    if (changed) {
        update_menu();
    }
}
