/*
 * pcd8544_nav.ino - Example code using the menu system library
 *
 * This example shows using the menu system with a pcd8544 LCD display
 * (controlled over serial).
 *
 * Copyright (c) 2016 arduino-menusystem
 * Licensed under the MIT license (see LICENSE)
 */

#include <MenuSystem.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

// Menu variables
MenuSystem ms;
Menu mm("ROOT Title");
MenuItem mm_mi1("Lvl1-Item1(I)");
MenuItem mm_mi2("Lvl1-Item2(I)");
Menu mu1("Lvl1-Item3(M)");
MenuItem mu1_mi1("Lvl2-Item1(I)");

// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
//Adafruit_PCD8544 lcd = Adafruit_PCD8544(7, 6, 5, 4, 3);

// Hardware SPI (faster, but must use certain hardware pins):
// SCK is LCD serial clock (SCLK) - this is pin 13 on Arduino Uno
// MOSI is LCD DIN - this is pin 11 on an Arduino Uno
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 lcd = Adafruit_PCD8544(5, 4, 3);
// Note with hardware SPI MISO and SS pins aren't used but will still be read
// and written to during SPI transfer.  Be careful sharing these pins!

#define PCD8544_CHAR_HEIGHT 8

// Menu callback function
// In this example all menu items use the same callback.

void on_item1_selected(MenuItem* p_menu_item)
{
  lcd.setCursor(0,2*PCD8544_CHAR_HEIGHT);
  lcd.print("Item1 Selectd");
  lcd.display();
  delay(1500); // so we can look the result on the LCD
}

void on_item2_selected(MenuItem* p_menu_item)
{
  lcd.setCursor(0,2*PCD8544_CHAR_HEIGHT);
  lcd.print("Item2 Selectd");
  lcd.display();
  delay(1500); // so we can look the result on the LCD
}

void on_item3_selected(MenuItem* p_menu_item)
{
  lcd.setCursor(0,2*PCD8544_CHAR_HEIGHT);
  lcd.print("Item3 Selectd");
  lcd.display();
  delay(1500); // so we can look the result on the LCD
}

// Standard arduino functions

void setup()
{
  Serial.begin(9600);
  lcd.begin();
  // you can change the contrast around to adapt the display
  // for the best viewing!
  lcd.setContrast(60);
  
  #ifdef HIDE_SPLASH
  lcd.clearDisplay();
  #else
  lcd.display(); // show splashscreen
  delay(2000);
  #endif // HIDE_SPLASH

  lcd.setTextSize(1);
  
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
  lcd.clearDisplay();
  lcd.setCursor(0,0*PCD8544_CHAR_HEIGHT);
  // Display the menu
  Menu const* cp_menu = ms.get_current_menu();

  //lcd.print("Current menu name: ");
  lcd.print(cp_menu->get_name());
  lcd.setCursor(0,1*PCD8544_CHAR_HEIGHT);
  lcd.print(cp_menu->get_selected()->get_name());
  lcd.display();
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
    case 'a': // Back pressed
      ms.back();
      displayMenu();
      break;
    case 'd': // Select pressed
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

