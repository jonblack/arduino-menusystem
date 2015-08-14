/*
 * lcd_advanced_nav.ino - Example code using the menu system library
 *
 * This example shows using the menu system with a 16x2 LCD display
 * (controled over serial).
 *
 * Copyright (c) 2015 arduino-menusystem
 * Licensed under the MIT license (see LICENSE)
 */

#include <MenuSystem.h>
#include <LiquidCrystal.h>

// Menu variables
MenuSystem ms;
Menu mm("Main Menu");
MenuItem mm_mi1("Read Analog 0 A0");
MenuItem mm_mi2("Blink led");
Menu mu1("Date & Time");
MenuItem mu1_mi1("Show current");
MenuItem mu1_mi2("Set");
MenuItem mu1_mi3("Go back");

/*
The LCD circuit:
 * LCD RS pin to digital pin 8
 * LCD Enable pin to digital pin 9
 * LCD D4 pin to digital pin 4
 * LCD D5 pin to digital pin 5
 * LCD D6 pin to digital pin 6
 * LCD D7 pin to digital pin 7
 * LCD R/W pin to ground
 */
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8     ,     9 ,      4 ,      5 ,      6 ,      7 );

//Example related
#define SELECTED_DISPLAY_DELAY 1500
// menuSelected:  State var that indicates that a menu item has been
//                selected. The plan is to update acordingly the LCD
//                if we want to use the display to other thing
//                like set up a variable (show on this example with time)
boolean menuSelected = false; // no menu item has been selected
boolean commandReceived = false;// no command has been received (Seria, button, etc)
enum setMenuSelected_Type { 
  DATETIME, OTHER }; // this enum is in case we want to expand this example
setMenuSelected_Type setMenu;
byte cursorPosition;
String setString;

int led = 41; // connect a led+resistor on pin 41 or change this number to 13 to use default arduino led on board
int ledState = LOW;
String dateTime = "2012-12-02 17:55";

// Menu callback functions
void on_item1_selected(MenuItem* p_menu_item) {
  lcd.setCursor(0,1);
  lcd.print("Analog A0: ");
  lcd.print(analogRead(A0));
  delay(SELECTED_DISPLAY_DELAY);
}

void on_item2_selected(MenuItem* p_menu_item) {
  ledState = !ledState;
  digitalWrite(led,ledState);
  lcd.setCursor(0,1);
  lcd.print("Led state: ");
  if(ledState)
    lcd.print("ON");
  else
    lcd.print("OFF");
  delay(SELECTED_DISPLAY_DELAY);
}
// on_back_selected is usefull if you don't have a button to make the back function
void on_back_selected(MenuItem* p_menu_item) {
  ms.back();
}
void on_time_show_selected(MenuItem* p_menu_item) {
  lcd.setCursor(0,1);
  lcd.print(dateTime);
  delay(SELECTED_DISPLAY_DELAY);
}
void on_time_set_selected(MenuItem* p_menu_item)
{
  setString = dateTime;
  lcd.setCursor(0,1);
  lcd.print(setString);
  cursorPosition = 0;
  lcd.setCursor(cursorPosition,1);
  lcd.blink();
  setMenu = DATETIME;
  menuSelected = true;

}

// Standard arduino functions

void setup() {
  Serial.begin(9600);
  pinMode(led,OUTPUT);
  lcd.begin(16, 2);
  serialPrintHelp();
  Serial.println("Setting up the menu.");

  // init example vars
  menuSelected = false; 

  // Menu setup
  /*
  Menu Structure:
   -Analog read
   -Led blink
   -Date & Time
   --Show current
   --Set
   --Back
   
   */
  mm.add_item(&mm_mi1, &on_item1_selected);
  mm.add_item(&mm_mi2, &on_item2_selected);
  mm.add_menu(&mu1);
  mu1.add_item(&mu1_mi1, &on_time_show_selected);
  mu1.add_item(&mu1_mi2, &on_time_set_selected);
  mu1.add_item(&mu1_mi3, &on_back_selected);
  ms.set_root_menu(&mm);
  Serial.println("Menu setted.");
  displayMenu();
}

void loop() {
  // Handle serial commands
  serialHandler();
  //SELECTED_DISPLAY_DELAY
  updateDisplay();

}

void displayMenu() {
  lcd.clear();
  lcd.setCursor(0,0);

  // Display the menu
  Menu const* cp_menu = ms.get_current_menu();
  lcd.print(cp_menu->get_selected()->get_name());
}

void updateDisplay() {
  if(!menuSelected && commandReceived) {
    displayMenu();
    commandReceived = false;
  } 
  else if (commandReceived) {
    switch(setMenu) {
    case DATETIME:

      break;
      // add new cases as you add set menus (don't forget to add the corresponding enum)
    default:
      break;
    }
  }
}

void serialHandler() {
  char inChar;
  if((inChar = Serial.read())>0) {

    switch (inChar) {
    case 'w': // Previus item
      if(!menuSelected)
        ms.prev();
      else {
        if(setMenu == DATETIME) {
          if(setString[cursorPosition]>47 && setString[cursorPosition]<57)
            setString[cursorPosition]++;
          lcd.write(setString[cursorPosition]);
          lcd.setCursor(cursorPosition,1);
        }
      }
      break;
    case 's': // Next item
      if(!menuSelected)
        ms.next();
      else {
        if(setMenu == DATETIME) {
          if(setString[cursorPosition]>48 && setString[cursorPosition]<58)
            setString[cursorPosition]--;
          lcd.write(setString[cursorPosition]);
          lcd.setCursor(cursorPosition,1);
          }
      }
      break;
    case 'a': // Back presed
      if(!menuSelected)
        ms.back();
      else if(cursorPosition>0)
        lcd.setCursor(--cursorPosition,1);
      break;
    case 'd': // Select presed
      if(!menuSelected)
        ms.select();
      else if(cursorPosition<15)
        lcd.setCursor(++cursorPosition,1);
      break;
    case 'e': // Cancel (clear menuSelected) For debug purpose
      menuSelected = false;
      break;
    case 'q': // Set
      if(menuSelected) {
        if(setMenu == DATETIME) {
          dateTime = setString;
          lcd.setCursor(0,1);
          lcd.print("Time seted      ");
          delay(SELECTED_DISPLAY_DELAY);
          menuSelected = false;
        }
      lcd.noBlink();
      }
    case '?':
    case 'h': // Display help
      serialPrintHelp();
      break;
    default:
      break;
    } 
    commandReceived = true;
  }
}


void serialPrintHelp() {
  Serial.println("***************");
  Serial.println("w: go to previus item (up)");
  Serial.println("s: go to next item (down)");
  Serial.println("a: go back (right)");
  Serial.println("d: select \"selected\" item (left)");
  Serial.println("q: set (no meaning while moving");
  Serial.println("   over the menu, see LCDNav");
  Serial.println("   example if you get confuse");
  Serial.println("e: erase menuSelected flag");
  Serial.println("   can be used as a cancel button");
  Serial.println("?: print this help");
  Serial.println("h: print this help");
  Serial.println("***************");

}







