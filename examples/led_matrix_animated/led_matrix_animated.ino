/*
 * led_matrix_animated.ino - Example code using the menu system library.
 *
 * This example demonstrates an animated menu on a Sure Electronics 32x16
 * LED matrix.
 *
 * Copyright (c) 2015 arduino-menusystem
 * Licensed under the MIT license (see LICENSE)
 */

#include <ht1632c.h>
#include <MenuSystem.h>

// Display constants

#define PIN_LED_DATA  8
#define PIN_LED_WR    10
#define PIN_LED_CLOCK 11
#define PIN_LED_CS    12
#define LED_WIDTH     32
#define LED_HEIGHT    16
#define FONT_WIDTH    5
#define FONT_HEIGHT   7

// Menu variables

MenuSystem ms;
Menu mm("");
MenuItem mi_one("ONE");
MenuItem mi_two("TWO");
MenuItem mi_three("THREE");

// Display variables

ht1632c ledMatrix = ht1632c(&PORTB, PIN_LED_DATA, PIN_LED_WR, PIN_LED_CLOCK,
    PIN_LED_CS, GEOM_32x16, 2);

// Functions

enum VSlideDirection { VSLIDE_UP, VSLIDE_DOWN };

void vslide(const char* menu1, const char* menu2, VSlideDirection d) {
    // Calculate vertical position
    int menu1_start_y = (LED_HEIGHT / 2) - (FONT_HEIGHT / 2);
    int menu2_target_y = menu1_start_y;
    int menu2_start_y;
    if (d == VSLIDE_UP) {
        menu2_start_y = menu1_start_y + LED_HEIGHT;
    } else {
        menu2_start_y = menu1_start_y - LED_HEIGHT;
    }
    int menu1_y = menu1_start_y;
    int menu2_y = menu2_start_y;

    // Calculate horizontal position
    int menu1_text_width = FONT_WIDTH * strlen(menu1);
    int menu1_pixel_spare = LED_WIDTH - menu1_text_width;
    int menu1_x_idnt = (int) floor(menu1_pixel_spare / 2);

    int menu2_text_width = FONT_WIDTH * strlen(menu2);
    int menu2_pixel_spare = LED_WIDTH - menu2_text_width;
    int menu2_x_idnt = (int) floor(menu2_pixel_spare / 2);

    while (1) {
        ledMatrix.clear();
        for (int i = 0; i < strlen(menu1); i++) {
            ledMatrix.putchar(
                (i * FONT_WIDTH) + menu1_x_idnt,
                menu1_y, menu1[i], RED
            );
        }
        for (int i = 0; i < strlen(menu2); i++) {
            ledMatrix.putchar(
                (i * FONT_WIDTH) + menu2_x_idnt,
                menu2_y, menu2[i], RED
            );
        }
        ledMatrix.sendframe();

        if (menu2_y == menu2_target_y) {
            break;
        }

        if (d == VSLIDE_UP) {
            menu1_y -= 1;
            menu2_y -= 1;
        } else {
            menu1_y += 1;
            menu2_y += 1;
        }

        delay(5);
    }
}

enum HSlideDirection { HSLIDE_LEFT, HSLIDE_RIGHT };

void hslide(const char* menu1, const char* menu2, HSlideDirection d) {
    // Calculate vertical position
    int y_idnt = (LED_HEIGHT / 2) - (FONT_HEIGHT / 2);

    // Calculate horizontal position
    int menu1_text_width = FONT_WIDTH * strlen(menu1);
    int menu1_pixel_spare = LED_WIDTH - menu1_text_width;
    int menu1_start_x = (int) floor(menu1_pixel_spare / 2);
    int menu2_text_width = FONT_WIDTH * strlen(menu2);
    int menu2_pixel_spare = LED_WIDTH - menu2_text_width;
    int menu2_target_x = (int) floor(menu2_pixel_spare / 2);
    int menu2_start_x;
    if (d == HSLIDE_LEFT) {
        menu2_start_x = menu1_start_x + LED_WIDTH;
    } else {
        menu2_start_x = menu1_start_x - LED_WIDTH;
    }
    int menu1_x = menu1_start_x;
    int menu2_x = menu2_start_x;

    while (1) {
        ledMatrix.clear();
        for (int i = 0; i < strlen(menu1); i++) {
            ledMatrix.putchar(
                (i * FONT_WIDTH) + menu1_x,
                y_idnt, menu1[i], RED
            );
        }
        for (int i = 0; i < strlen(menu2); i++) {
            ledMatrix.putchar(
                (i * FONT_WIDTH) + menu2_x,
                y_idnt, menu2[i], RED
            );
        }
        ledMatrix.sendframe();

        if (menu2_x == menu2_target_x) {
            break;
        }

        if (d == HSLIDE_LEFT) {
            menu1_x -= 1;
            menu2_x -= 1;
        } else {
            menu1_x += 1;
            menu2_x += 1;
        }

        delay(5);
    }
}

void fade(const char* menu1, const char* menu2) {
    int y_idnt = (LED_HEIGHT / 2) - (FONT_HEIGHT / 2);

    int menu1_text_width = FONT_WIDTH * strlen(menu1);
    int menu1_pixel_spare = LED_WIDTH - menu1_text_width;
    int menu1_x_idnt = (int) floor(menu1_pixel_spare / 2);
    for (int i = 0; i < strlen(menu1); i++) {
        ledMatrix.putchar(
            (i * FONT_WIDTH) + menu1_x_idnt,
            y_idnt, menu1[i], RED
        );
    }
    ledMatrix.sendframe();

    int brightness = 10;
    while (brightness > 0) {
        ledMatrix.pwm(--brightness);
        delay(30);
    }
    ledMatrix.clear();

    int menu2_text_width = FONT_WIDTH * strlen(menu2);
    int menu2_pixel_spare = LED_WIDTH - menu2_text_width;
    int menu2_x_idnt = (int) floor(menu2_pixel_spare / 2);
    for (int i = 0; i < strlen(menu2); i++) {
        ledMatrix.putchar(
            (i * FONT_WIDTH) + menu2_x_idnt,
            y_idnt, menu2[i], RED
        );
    }
    ledMatrix.sendframe();
    delay(30);
    while (brightness < 10) {
        ledMatrix.pwm(++brightness);
        delay(30);
    }
}

// Standard arduino functions

void setup() {
    Serial.begin(9600);

    // LED matrix
    ledMatrix.clear();
    ledMatrix.pwm(10);
    ledMatrix.setfont(FONT_5x7);

    // Menu
    mm.add_item(&mi_one, NULL);
    mm.add_item(&mi_two, NULL);
    mm.add_item(&mi_three, NULL);
    ms.set_root_menu(&mm);
}

void loop() {
    Menu const* menu;

    menu = ms.get_current_menu();
    const char* menu1_name = menu->get_selected()->get_name();
    ms.next(true);
    menu = ms.get_current_menu();
    const char* menu2_name = menu->get_selected()->get_name();

    //vslide(menu1_name, menu2_name, VSLIDE_UP);
    //hslide(menu1_name, menu2_name, HSLIDE_LEFT);
    fade(menu1_name, menu2_name);

    delay(1000);
}
