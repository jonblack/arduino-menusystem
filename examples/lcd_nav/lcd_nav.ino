

/*
 * lcd_nav.ino - Example code using the menu system library
 *
 * This example shows using the menu system with a 16x2 LCD display
 * (controled over serial).
 *
 * Copyright (c) 2015 arduino-menusystem
 * Licensed under the MIT license (see LICENSE)
 */

#include <MenuSystem.h>
#include <LiquidCrystal.h>
#include <DFR_LCD_Keypad.h>
// renderer

// The LCD circuit:
//    * LCD RS pin to digital pin 8
//    * LCD Enable pin to digital pin 9
//    * LCD D4 pin to digital pin 4
//    * LCD D5 pin to digital pin 5
//    * LCD D6 pin to digital pin 6
//    * LCD D7 pin to digital pin 7
//    * LCD R/W pin to ground
LiquidCrystal lcd = LiquidCrystal(8, 9, 4, 5, 6, 7);
DFR_LCD_Keypad keypad(A0, &lcd);

class MyRenderer : public MenuComponentRenderer {
public:
    void render(Menu const& menu) const {
        lcd.clear();
        
        lcd.setCursor(0,0);
        lcd.print(menu.get_name());
        lcd.setCursor(0,1);
        
        menu.get_current_component()->render(*this);
    }

    void render_menu_item(MenuItem const& menu_item) const {
        lcd.print(menu_item.get_name());
    }

    void render_back_menu_item(BackMenuItem const& menu_item) const {
        lcd.print(menu_item.get_name());
    }

    void render_numeric_menu_item(NumericMenuItem const& menu_item) const {
        lcd.print(menu_item.get_name());
        lcd.print(": ");
        lcd.print((int)menu_item.get_value());
        //lcd.write(byte(1));
    }

    void render_text_menu_item(TextMenuItem const& menu_item) const {
        lcd.print(menu_item.get_name());
        lcd.print(": ");
        lcd.print(menu_item.get_value());
    }    
    void render_menu(Menu const& menu) const {
        lcd.print(menu.get_name());
    }
};

MyRenderer my_renderer;

// Forward declarations

void on_pin_setting_1_selected(MenuComponent* p_menu_component);
void on_pin_setting_2_selected(MenuComponent* p_menu_component);
void on_pin_setting_3_selected(MenuComponent* p_menu_component);
void on_pin_setting_4_selected(MenuComponent* p_menu_component);

void on_work_selected(MenuComponent* p_menu_component);
void on_sleep_selected(MenuComponent* p_menu_component);

void on_setting_selected(MenuComponent* p_menu_component);

// Menu variables

MenuSystem ms(my_renderer);


Menu PinSetting("PIN SETTING");
  NumericMenuItem  pin_setting_1("PIN 1", &on_pin_setting_1_selected,0,0,1);
  NumericMenuItem  pin_setting_2("PIN 2", &on_pin_setting_2_selected,0,0,1);
  NumericMenuItem  pin_setting_3("PIN 3", &on_pin_setting_3_selected,0,0,1);
  NumericMenuItem  pin_setting_4("PIN 4", &on_pin_setting_4_selected,0,0,1);
  
Menu TimeSetting("TIME SETTING");
  NumericMenuItem  work("WORK", &on_work_selected,0,0,10000,10);
  NumericMenuItem  sleep("SLEEP", &on_sleep_selected,0,0,10000);
  TextMenuItem  setting("Setting", &on_setting_selected);

// Menu callback function

void on_pin_setting_1_selected(MenuComponent* p_menu_component) {
    lcd.setCursor(0,1);
    lcd.print("PIN 1 Save ");
    delay(500); // so we can look the result on the LCD
}

void on_pin_setting_2_selected(MenuComponent* p_menu_component) {
    lcd.setCursor(0,1);
    lcd.print("PIN 2 Save ");
    delay(500); // so we can look the result on the LCD
}

void on_pin_setting_3_selected(MenuComponent* p_menu_component) {
    lcd.setCursor(0,1);
    lcd.print("PIN 3 Save ");
    delay(500); // so we can look the result on the LCD
}

void on_pin_setting_4_selected(MenuComponent* p_menu_component) {
    lcd.setCursor(0,1);
    lcd.print("PIN 4 Save ");
    delay(500); // so we can look the result on the LCD
}

void on_work_selected(MenuComponent* p_menu_component) {
    lcd.setCursor(0,1);
    lcd.print("WORK Save ");
    delay(500); // so we can look the result on the LCD
}

void on_sleep_selected(MenuComponent* p_menu_component) {
    lcd.setCursor(0,1);
    lcd.print("SLEEP Save ");
    delay(500); // so we can look the result on the LCD
}

void on_setting_selected(MenuComponent* p_menu_component) {
    lcd.setCursor(0,1);
    lcd.print("SETTING Save ");
    delay(500); // so we can look the result on the LCD
}



void serial_handler() {
  
  int last_key = keypad.get_last_key();
  int key      = keypad.read_key();
    if (key != last_key) {
    // key has changed
    
    // print the key selection to the LCD
    switch (key) {  
        
      case KEY_RIGHT: 
        ms.next();
        ms.display();
        break;
        
      case KEY_LEFT:
        // Previus item
        ms.prev();
        ms.display();
        break;

      case KEY_SELECT:
        ms.select();
        ms.display();
        break;      
      case KEY_UP:
        ms.back();
        ms.display();
        break;  
      case KEY_DOWN:
        ms.reset();
        ms.display();
        break;
      default:
        break;
    }
  }
}

// Standard arduino functions

void setup() {
    Serial.begin(9600);
    lcd.begin(16, 2);
    
    // Create the custom character.
//    lcd.createChar(0, cursor);
//    lcd.createChar(1, watch);
    
    ms.get_root_menu().add_menu(&PinSetting);
      PinSetting.add_item(&pin_setting_1);
      PinSetting.add_item(&pin_setting_2);
      PinSetting.add_item(&pin_setting_3);
      PinSetting.add_item(&pin_setting_4);
    ms.get_root_menu().add_menu(&TimeSetting);
      TimeSetting.add_item(&work);
      TimeSetting.add_item(&sleep);
    ms.get_root_menu().add_item(&setting);
    char* values[] ={"text1","text2","text3"};
    setting.set_values(values,3);

    ms.display();
}

void loop() {
    
    serial_handler();
}
