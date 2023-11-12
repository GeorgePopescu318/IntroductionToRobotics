const int latchPin = 11;
const int clockPin = 10;
const int dataPin = 12;

const int segD1 = 4;
const int segD2 = 5;
const int segD3 = 6;
const int segD4 = 7;

//                   start,reset,lap
const int buttonsNo = 3;
const int buttonsPin[buttonsNo] = { 2, 8, 3 };

int buttonsState[buttonsNo] = { HIGH, HIGH, HIGH };

int displayDigits[] = { segD1, segD2, segD3, segD4 };
const int displayCount = 4;
const int encodingsNumber = 16;
byte currentTime[displayCount];

const int possibleLaps = 4;
byte lapTime[possibleLaps][displayCount];
byte currentLap = 0;
bool lapView = false;
byte nextLap = 0;

byte byteEncodings[encodingsNumber] = {
  //A B C D E F G DP
  B11111100,  // 0
  B01100000,  // 1
  B11011010,  // 2
  B11110010,  // 3
  B01100110,  // 4
  B10110110,  // 5
  B10111110,  // 6
  B11100000,  // 7
  B11111110,  // 8
  B11110110,  // 9
  B11101110,  // A
  B00111110,  // b
  B10011100,  // C
  B01111010,  // d
  B10011110,  // E
  B10001110   // F
};

int displayDigit;

unsigned long lastIncrement = 0;
unsigned long incrementDelay = 100;
unsigned long number = 0;

volatile int currentButton = -1;
volatile bool possiblePress = false;
volatile bool shouldStartCounter = false;
unsigned long lastDebounceTime = 0;
unsigned int debounceDelay = 75;
bool buttonReading = HIGH;
bool lastReading = HIGH;

void setup() {

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  for (int i = 0; i < displayCount; i++) {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], LOW);
  }

  for (int i = 0; i < buttonsNo; i++) {
    pinMode(buttonsPin[i], INPUT_PULLUP);
  }

  for (int i = 0; i < displayCount; i++) {
    currentTime[i] = 0;
  }

  attachInterrupt(digitalPinToInterrupt(buttonsPin[0]), handleInterruptStart, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonsPin[2]), handleInterruptSave, FALLING);
  Serial.begin(9600); // for testing
}
void loop() {

  if (digitalRead(buttonsPin[1]) == 0 && possiblePress == false && lastReading == true) {
    possiblePress = true;
    shouldStartCounter = true;
    currentButton = 1;
  } else if (digitalRead(buttonsPin[1]) == 1) {
      lastReading = true;
  }

  if (currentButton != -1) {
    buttonDebounce();
  }

  buttonsOptions();

  if (lapView == true) {
    viewLaps();
  }
  else {
    calculateTime();
    writeNumber(currentTime);
  }
}
void buttonsOptions() {
  if (millis() - lastIncrement >= incrementDelay && buttonsState[0] == 0) {
    number++;
    lastIncrement = millis();
    lapView = false;
  }

  if (buttonsState[0] == 1 && buttonsState[1] == 0) {
    for (int i = 0; i < displayCount; i++) {
      currentTime[i] = 0;
    }
    number = 0;
    if (lapView == true) {
      for (int i = 0; i < possibleLaps; i++) {
        for (int j = 0; j < displayCount; j++) {
          lapTime[i][j] = 0;
        }
      }
      currentLap = 0;
    }
    buttonsState[1] = 1;
  } else {
    buttonsState[1] = 1;
  }

  if (buttonsState[0] == 0 && buttonsState[2] == 0) {
    buttonsState[2] = 1;
    if (number != 0 || currentTime[0] != 0) {
      currentLap %= possibleLaps;
      for (int i = 0; i < displayCount; i++) {
        lapTime[currentLap][i] = currentTime[i];
      }
      currentLap++;
    }
  } else if (buttonsState[0] == 1 && buttonsState[2] == 0 && lapView == false) {
      lapView = true;
      buttonsState[2] = 1;
  }
}
void viewLaps() {
  nextLap %= possibleLaps;
  writeNumber(lapTime[nextLap]);
  if (buttonsState[2] == 0) {
    nextLap++;
    buttonsState[2] = 1;
  }

}

void buttonDebounce() {
  if (possiblePress) {
    if (shouldStartCounter) {
      lastDebounceTime = millis();
      shouldStartCounter = false;
    }
    if ((millis() - lastDebounceTime) > debounceDelay) {
      buttonReading = digitalRead(buttonsPin[currentButton]);
      if (buttonReading == LOW) {
        buttonsState[currentButton] = !buttonsState[currentButton];
        lastReading = buttonReading;
        lastReading = false;
      }
      possiblePress = false;
    }
  }
}
void calculateTime() {
  currentTime[3] = number % 10;
  currentTime[2] = number / 10 % 10;
  currentTime[1] = number / 100 % 10;
  if (currentTime[2] == 0 && currentTime[1] == 6) {
    currentTime[0] += 1;
    number = 0;
  }
  if (currentTime[0] == encodingsNumber) {
    for (int i = 0; i < displayCount; i++) {
      currentTime[i] = 0;
    }
    number = 0;
  }
}

void writeReg(int digit) {

  digitalWrite(latchPin, LOW);
  if (digit != 0 && (displayDigit == 2 || displayDigit == 0)) {
    digit += 1;
  }
  shiftOut(dataPin, clockPin, MSBFIRST, digit);

  digitalWrite(latchPin, HIGH);
}

void activateDisplay(int displayNumber) {

  for (int i = 0; i < displayCount; i++) {
    digitalWrite(displayDigits[i], HIGH);
  }

  digitalWrite(displayDigits[displayNumber], LOW);
}

void writeNumber(byte currentViewedTime[]) {
  displayDigit = displayCount - 1;
  for (int i = displayCount - 1; i >= 0; i--) {
    int currentNumber = currentViewedTime[i];
    activateDisplay(displayDigit);
    writeReg(byteEncodings[currentNumber]);
    delay(0);
    displayDigit--;
    writeReg(B00000000);
  }
}

void handleInterruptStart() {
  possiblePress = true;
  shouldStartCounter = true;
  currentButton = 0;
}

void handleInterruptSave() {
  possiblePress = true;
  shouldStartCounter = true;
  currentButton = 2;
}