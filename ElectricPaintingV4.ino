#include <LedControl.h>

// Arduino pin numbers
const int BTN_PIN = 2;
const int X_PIN = 0;
const int Y_PIN = 1;
const int DIN_PIN = 12;
const int CS_PIN = 11;
const int CLK_PIN = 10;

// Other constants
const int THRESHHOLD = 256;
const int CURSOR_ON_TIME = 20;
const int CURSOR_OFF_TIME = 180;
const int BUTTON_DEBOUNCE_DELAY = 5;

// Cursor
struct Cursor {
  int x = 3, y = 3;
  bool is_blinking = true;
} cursor;

// Canvas
LedControl canvas = LedControl (DIN_PIN, CLK_PIN, CS_PIN, 1);

byte grid[8] = {
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000
}; 

// Get led value from grid
bool GetLed(int row, int col) {
  return grid[row] & (B10000000 >> col);
}

// Toggle led value of grid
void ToggleLed(int row, int col) {
  grid[row] ^= (B10000000 >> col);
}

// Udpdate canvas to corespond to grid
void UpdateCanvas() {
  for (int r = 0; r < 8; ++r) {
    for (int c = 0; c < 8; ++c) {
      if (c == cursor.x && r == cursor.y && cursor.is_blinking)
        canvas.setLed(0, 7 - c, r, !GetLed(r, c));
      else
        canvas.setLed(0, 7 - c, r, GetLed(r, c));  
    }
  }    
}

// Enums + timestamps
enum CursorState : char {
  BLINKING,
  NORMAL
} cursor_state = CursorState::BLINKING;
unsigned long t_0_cursor = 0;

enum ThumbstickState : char {
  CENTERED,
  LEFT,
  RIGHT,
  UP,
  DOWN
} thumbstick_state =  ThumbstickState::CENTERED;

enum ButtonState : char {
  OPEN,
  CLOSED
} button_state = ButtonState::OPEN;
unsigned long t_0_button = 0;

// Setup and main
void setup() { 
  // Thumbstick button pin setup
  pinMode(BTN_PIN, INPUT);
  digitalWrite(BTN_PIN, HIGH);

  // LED matrix setup
  canvas.shutdown(0, false);
  canvas.setIntensity(0, 1);
  canvas.clearDisplay(0);

  // Monitor setup
  Serial.begin(9600);
}

void loop() {
  ThumbstickStateMachine();
  ButtonStateMachine();
  CursorStateMachine();
  UpdateCanvas();
}

// Thumbstick state machine
void ThumbstickStateMachine() {
  switch (thumbstick_state) {
   case ThumbstickState::CENTERED:
    // Move cursor with change of state
    if (analogRead(X_PIN) < 489 - THRESHHOLD && cursor.y > 0) {
      Serial.println("Up");
      --cursor.y;  
      thumbstick_state = ThumbstickState::UP;
    } else if (analogRead(X_PIN) > 489 + THRESHHOLD && cursor.y < 7) {
      Serial.println("Down");
      ++cursor.y;
      thumbstick_state = ThumbstickState::DOWN;
    } else if (analogRead(Y_PIN) < 491 - THRESHHOLD && cursor.x < 7) {
      Serial.println("Right");
      ++cursor.x;
      thumbstick_state = ThumbstickState::RIGHT;
    } else if (analogRead(Y_PIN) > 491 + THRESHHOLD && cursor.x > 0) {
      Serial.println("Left");
       --cursor.x;
      thumbstick_state = ThumbstickState::LEFT;  
    }
    break;

   case ThumbstickState::UP:
    if (analogRead(X_PIN) > 489 - THRESHHOLD) {
      Serial.println("Centered");
      thumbstick_state = ThumbstickState::CENTERED;
    }  
    break;

   case ThumbstickState::DOWN:
    if (analogRead(X_PIN) < 489 + THRESHHOLD) {
      Serial.println("Centered");
      thumbstick_state = ThumbstickState::CENTERED;
    }  
    break;

   case ThumbstickState::RIGHT:
    if (analogRead(Y_PIN) > 491 - THRESHHOLD) {
      Serial.println("Centered");
      thumbstick_state = ThumbstickState::CENTERED;
    }  
    break;

   case ThumbstickState::LEFT:
    if (analogRead(Y_PIN) < 491 + THRESHHOLD) {
      Serial.println("Centered");
      thumbstick_state = ThumbstickState::CENTERED;
    }  
    break;    
  }
}

// Button state machine
void ButtonStateMachine() {
  switch (button_state) {
   case ButtonState::OPEN:
    if (digitalRead(BTN_PIN) == LOW && millis() - t_0_button > BUTTON_DEBOUNCE_DELAY) {
      t_0_button = millis();
      ToggleLed(cursor.y, cursor.x);
      button_state = ButtonState::CLOSED;
    }
    break;
    
   case ButtonState::CLOSED:
    if (digitalRead(BTN_PIN) == HIGH && millis() - t_0_button > BUTTON_DEBOUNCE_DELAY) {
      t_0_button = millis();
      button_state = ButtonState::OPEN;
    }
    break;
  }
}

// Cursor state machine
void CursorStateMachine() {
  switch (cursor_state) {
   case CursorState::BLINKING:
    if (millis() - t_0_cursor > CURSOR_ON_TIME) {
      t_0_cursor = millis();
      cursor.is_blinking = false;
      cursor_state = CursorState::NORMAL;
    }
    break;
    
   case CursorState::NORMAL:
    if (millis() - t_0_cursor > CURSOR_OFF_TIME) {
      t_0_cursor = millis();
      cursor.is_blinking = true;
      cursor_state = CursorState::BLINKING;
    }
    break;
  }
}
