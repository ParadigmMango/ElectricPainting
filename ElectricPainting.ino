#include <LedControl.h>

// Arduino pin numbers
// INPUT
const int SW_PIN = 2;
const int X_PIN = 0;
const int Y_PIN = 1;
// OUTPUT

// Other constants
const int THRESHHOLD = 256;

// Canvas
LedControl canvas = LedControl(12,10,11,1);

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

// Canvas functions
bool getLed(int row, int col) {
  return grid[row] & (B10000000 >> col);
}

void updateCanvas() {
  for (int r = 0; r < 8; ++r)
    for (int c = 0; c < 8; ++c)
      canvas.setLed(0, r, c, getLed(c, 7 - r));
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

  updateCanvas();
    
  delay(1000);
}
