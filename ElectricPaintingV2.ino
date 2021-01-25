#include <LedControl.h>

// Arduino pin numbers
const int SW_PIN = 2;
const int X_PIN = 0;
const int Y_PIN = 1;
const int DIN_PIN = 12;
const int CS_PIN = 11;
const int CLK_PIN = 10;

// Other constants
const int THRESHHOLD = 256;

// Tracks loop number, allowing led to blink
int loop_num = 0;

// Cursor
struct Cursor {
  int x = 3, y = 3;
} cursor;

// Canvas
LedControl canvas = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 1);

byte grid[8] = {
  B00000000,
  B00000000,
  B00111110,
  B01000010,
  B00000010,
  B01000100,
  B00010000,
  B00000000
};

// Get led value from grid
bool GetLed(int row, int col) {
  return grid[row] & (B10000000 >> col);
}

// Udpdate canvas to corespond to grid
void UpdateCanvas() {
  for (int r = 0; r < 8; ++r) {
    for (int c = 0; c < 8; ++c) {
      if (c == cursor.x && r == cursor.y && loop_num == 0)
        canvas.setLed(0, 7 - c, r, !GetLed(r, c));
      else
        canvas.setLed(0, 7 - c, r, GetLed(r, c));  
    }
  }    
}

void setup() {
  // Thumbstick switch pin setup
  pinMode(SW_PIN, INPUT);
  digitalWrite(SW_PIN, HIGH);

  // LED matrix setup
  canvas.shutdown(0, false);
  canvas.setIntensity(0, 3);
  canvas.clearDisplay(0);

  // Monitor setup
  Serial.begin(9600);
}

void loop() {
  // Thumbstick value interpretation
  if (analogRead(X_PIN) < 489 - THRESHHOLD)
    Serial.print("Up");
  else if (analogRead(X_PIN) > 489 + THRESHHOLD)
    Serial.print("Down");
    
  if (analogRead(Y_PIN) < 491 - THRESHHOLD)
    Serial.println("Right");
  else if (analogRead(Y_PIN) > 491 + THRESHHOLD)
    Serial.println("Left");

  UpdateCanvas();
  
  ++loop_num;
  if (loop_num == 4) loop_num = 0;
    
  delay(250);
}
