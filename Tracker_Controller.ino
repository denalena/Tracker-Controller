/*Tracker Controller  for ProMicro.  Charles Hobbs 2023, dena 2025
Emmulates either joystick or keyboard.  Multiple alternate keymapping modes available, 
configured by holding a key during start up.
*/
#include <Keyboard.h>
#include <Joystick.h> // https://github.com/MHeironimus/ArduinoJoystickLibrary
#include "Timer.h"

Timer timers[8];
Joystick_ Joystick;

const int DEBOUNCE = 50;

//pin mapping
byte a     = 2;  // m8:edit
byte b     = 9;  // m8:option
byte up    = 8;
byte left  = 4;
byte right = 3;
byte down  = 6;   
byte star  = 5;  // m8:play
byte select = 7; // m8:shift

int button[8] = {a, b, up, left, right, down, star, select};

//3 rows, 8 columns   row=mode;  column = command for button.    
int command[3][8] = {{1, 0, 12, 14, 15, 13, 9, 8},              //mode 0   generic NES-styled pad game controller
                     {'s', 'a', 218, 216, 215, 217, 'x', 'z'},  //mode 1   M8C keyboard mapping
                     {'x', 'z', 218, 216, 215, 217, 32, 129}    //mode 2   M8.run keyboard mapping
                   //{0,0,0,0,0,0,0,0},                         //mode 3
                   //{0,0,0,0,0,0,0,0},                         //mode 4
};

//joystick or keyboard?   joystick = 1, keyboard = 0    {mode 0, mode 1, mode 2...}
bool joyMode[3] = {1, 0, 0};    
int mode = 1;  // default mode m8c

int buttonState[8] = {};
int previousButtonState[8] = {HIGH};


void setup() {
  Joystick.begin();
  Keyboard.begin();

  for (int i = 0; i < 8; i++) {
    pinMode(button[i], INPUT_PULLUP);
    timers[i].start();
  }

  // set mode
  if (digitalRead(up) == LOW) {
    mode = 0;
  }
  if (digitalRead(down) == LOW) {
    mode = 2;
  }
}

void loop() {
  for (int i = 0; i < 8; i++) {
    buttonState[i] = digitalRead(button[i]);

    if (buttonState[i] == LOW && previousButtonState[i] == HIGH) {
      if (timers[i].read() > DEBOUNCE) {
        timers[i].stop();
        timers[i].start();

        if (joyMode[mode] == 0) {
          Keyboard.press(command[mode][i]);
        } else {
          Joystick.setButton(command[mode][i], HIGH);
        }
      }
    }

    if (buttonState[i] == HIGH && previousButtonState[i] == LOW) {
      if (joyMode[mode] == 0) {
        Keyboard.release(command[mode][i]);
      } else {
        Joystick.setButton(command[mode][i], LOW);
      }
    }

    previousButtonState[i] = buttonState[i];
  }
}