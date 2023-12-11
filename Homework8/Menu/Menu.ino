// base game reference Tank Wars
#include "LedControl.h"
#include <LiquidCrystal.h>
#include <EEPROM.h>
//lcd pins
const byte rs = 9;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 4;
const byte d7 = 13;

const byte lcdBrightnessPin = 5;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;

const int xPin = A0;
const int yPin = A1;

const byte ledPin = 16;

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);

const byte matrixSize = 8;

byte xPos = matrixSize - 2;
byte yPos = (matrixSize / 2) - 1;
byte xLastPos = 0;
byte yLastPos = 0;

const int minThreshold = 200;
const int maxThreshold = 600;

const byte moveInterval = 100;
unsigned long long lastMoved = 0;

bool matrixChanged = true;
byte matrix[matrixSize][matrixSize] = {
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 1, 1, 1, 1, 1, 1, 1, 1 }
};

byte aimMatrix[matrixSize][matrixSize] = {
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 }
};

byte leftRobot[] = {
  B00000,
  B01011,
  B01110,
  B00101,
  B01100,
  B01110,
  B01011,
  B00010
};

byte rightRobot[] = {
  B00000,
  B11000,
  B01100,
  B10100,
  B00110,
  B01110,
  B11010,
  B01000
};

byte rightArrow[] = {
  B11000,
  B11100,
  B00110,
  B00011,
  B00011,
  B00110,
  B11100,
  B11000
};

byte hearth[] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000
};

unsigned long blinkTime = 0;
const unsigned long blinkPeriod = 500;

const byte unusedPin = 13;

byte oldHight = 1;

const byte aimPin = 3;
const byte shootPin = 2;

volatile int currentButton = -1;
volatile bool possiblePress = false;
volatile bool shouldStartCounter = false;
unsigned long lastDebounceTime = 0;
unsigned int debounceDelay = 75;
bool buttonReading = HIGH;
bool lastReading = HIGH;

bool aimMode = false;
byte aimDirection = 2;
byte oldAdimDirection = -1;
unsigned long aimTime = 0;
const unsigned long aimPeriod = 1500;

byte left = 0;
byte right = 1;
byte up = 2;
byte down = 3;

const byte aimPosibilities = 5;

bool startShooting = false;

bool firstReading = true;
bool oldAimMode = false;

int xBullet = -1;
int yBullet = -1;

unsigned long bulletTime = 0;

byte bulletPeriod = 250;

bool firstShoot = false;

const byte maxHight = 5;

byte startPrint = 0;
byte nextMessage = 0;
byte introMessagesCount = 3;
byte aboutMessagesCount = 6;
byte displayMessagesCount = 5;
bool leftClick = false;
bool rightClick = false;

unsigned long messageTime = 0;
int timer = 0;
const char* introMessages[] = { "RobotFall", "Press any button", "to start" };
const char* optionsMessages[] = { "Start game", "Settings", "About" };
const char* buttonsMessages[] = { "LeftClk -enter", "RightClk -back" };
const char* settingsMessages[] = { "8x8Intensity", "LCDIntensity" };
const char* aboutMessages[] = { "RightClk -back", "RobotFall", "Made by", "George Popescu", "github.com/", "GeorgePopescu318" };
const char* displayMessages[] = { "Operator: ", "Hearts:", "OK", "FINE", "DANGER" };
const char* playerName[] = { "AGP" };
const char* endGameMessages[] = { "END" };
const int messagePeriod = 2000;
char test[] = "test";
byte incOptions = 0;
bool inOption = 0;
bool optionsMode = 0;
bool updateDisplay = true;
bool moveDone = 0;
bool initDisplay = true;
const int movePeriod = 650;
unsigned long moveTime = 0;
bool movePrinted = false;
byte lcdBrithtnessPercentage = 0;
byte lcdBrightness = 0;
byte lcdBrightnessEEPROMbyte = 0;
byte matrixBrightness = 0;
byte matrixBrightnessEEPROMbyte = 1;
bool firstDisplay = false;
bool endDisplay = false;
//EEPROM
//0 - LCD brigthness
//1 - matrix brigthness
bool settingsMode = 0;
byte maxLives = 3;
byte livesRemaining = 0;
const int livesPeriod = 5000;
unsigned long livesTime = 0;
byte incSettings = 0;

bool buttonsMode = true;

const uint64_t ROBOT = {
  0x4242ffc3db427f01
};

const uint64_t CLEAR = {
  0x0000000000000000
};

void (*myFunction[])(void){
  game,
  settings,
  about
};
void setup() {
  Serial.begin(9600);

  pinMode(ledPin, OUTPUT);

  lc.shutdown(0, false);
  EEPROM.get(matrixBrightnessEEPROMbyte, matrixBrightness);
  lc.clearDisplay(0);

  matrix[xPos][yPos] = 1;

  randomSeed(analogRead(unusedPin));

  pinMode(aimPin, INPUT_PULLUP);
  pinMode(shootPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(aimPin), handleInterruptAim, FALLING);
  attachInterrupt(digitalPinToInterrupt(shootPin), handleInterruptShoot, FALLING);

  pinMode(lcdBrightnessPin, OUTPUT);

  lcd.begin(16, 2);
  lcd.createChar(0, leftRobot);
  lcd.createChar(1, rightRobot);
  lcd.createChar(2, rightArrow);
  lcd.createChar(3, hearth);
  EEPROM.get(lcdBrightnessEEPROMbyte, lcdBrightness);
  lcdBrithtnessPercentage = map(lcdBrightness, 0, 254, 0, 100);
}
void loop() {
  Menu();
}

void Menu() {

  analogWrite(lcdBrightnessPin, lcdBrightness);
  lc.setIntensity(0, matrixBrightness);
  buttonDebounce();
  if (leftClick == false && rightClick == false && startPrint == 0) {
    introMessage();

  } else {
    if (startPrint != 2) {
      startPrint = 1;
      leftClick = false;
      rightClick = false;
      nextMessage = 0;
      Serial.println("aici");
      lcd.clear();
    }

    if (startPrint == 1) {

      startPrint = 2;
      optionsMode = 1;
    }
    if (startPrint != 0) {
    }
  }
  if (optionsMode == 1) {
    updatePositions(incOptions, optionsMode);  //menu
    incOptions %= 3;
    if (moveDone == 1) {
      if (movePrinted == false) {
        lcd.setCursor(2, 0);
        lcd.write("              ");
        movePrinted = true;
      }
      if (millis() - moveTime >= movePeriod) {
        movePrinted = false;
      }
    }
  }
  if (startPrint == 2) {
    if (buttonsMode == true) {
      buttonsFunction();
    }
    if (leftClick == 1 && inOption == 0) {
      inOption = 1;
      leftClick = 0;
    }
    if (inOption == 1) {
      optionsMode = 0;
      myFunction[incOptions]();
    } else {
      optionsMode = 1;
      choicesMessage(optionsMessages[incOptions]);
    }
  }
}

void introMessage() {

  lcd.setCursor(2, 0);
  lcd.print(introMessages[0]);
  lcd.write(byte(0));
  lcd.write(byte(1));
  lcd.setCursor(0, 1);
  lcd.print(introMessages[nextMessage + 1]);
  if (millis() - messageTime >= messagePeriod) {
    nextMessage++;
    nextMessage %= introMessagesCount - 1;
    messageTime = millis();
    lcd.clear();
  }
}

void choicesMessage(const char printedMessage[]) {
  lcd.setCursor(0, 0);
  lcd.write(byte(2));
  lcd.write(byte(2));
  lcd.setCursor(2, 0);
  lcd.print(printedMessage);
  lcd.setCursor(14, 0);
  lcd.write(byte(2));
  lcd.write(byte(2));
}

void buttonsFunction() {
  lcd.setCursor(0, 1);
  if (millis() - messageTime >= messagePeriod) {
    nextMessage++;
    nextMessage %= 2;
    messageTime = millis();
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
  }
  lcd.print(buttonsMessages[nextMessage]);
}

void settings() {
  buttonDebounce();
  if (leftClick == 1) {
    settingsMode = 0;
  } else {
    settingsMode = true;
  }
  Serial.println(leftClick);
  choicesMessage(settingsMessages[incSettings]);
  if (leftClick == true) {
    settingsMode = false;
    buttonsMode = false;
    lcd.setCursor(0, 1);
    lcd.write("              ");
    if (incSettings == 0) {
      if (firstDisplay == false) {
        displayImage(ROBOT);
        lcd.setCursor(2, 1);
        firstDisplay = true;
      }
      lcd.print(matrixBrightness);
      updatePositions(matrixBrightness, true);  //intensity
      if (matrixBrightness > 5) {
        matrixBrightness = 5;
      }
      if (matrixBrightness < 1) {
        matrixBrightness = 1;
      }
      EEPROM.put(matrixBrightnessEEPROMbyte, matrixBrightness);
      if (rightClick == true) {
        inOption = 0;
        rightClick = false;
        buttonsMode = true;
        firstDisplay = false;
        settingsMode = true;
        displayImage(CLEAR);
      }
    }
    if (incSettings == 1) {
      if (firstDisplay == false) {
        lcd.setCursor(2, 1);
        firstDisplay = true;
      }
      lcd.print(lcdBrithtnessPercentage);
      updatePositions(lcdBrithtnessPercentage, true);  //intensity
      if (lcdBrithtnessPercentage > 100) {
        lcdBrithtnessPercentage = 100;
      }
      if (lcdBrithtnessPercentage < 1) {
        lcdBrithtnessPercentage = 1;
      }
      lcdBrightness = map(lcdBrithtnessPercentage, 0, 100, 0, 254);
      if (rightClick == true) {
        EEPROM.put(lcdBrightnessEEPROMbyte, lcdBrightness);
        leftClick = false;
        rightClick = false;
        settingsMode = true;
        buttonsMode = true;
        firstDisplay = false;
        lcd.setCursor(3, 0);
        lcd.write("            ");
      }
    }
  }
  if (buttonsMode == true) {
    buttonsFunction();
  }
  if (settingsMode == true) {
    updatePositions(incSettings, settingsMode);  //settings

    if (moveDone == 1) {
      incSettings %= 2;
      if (movePrinted == false) {
        lcd.setCursor(2, 0);
        lcd.write("            ");
        movePrinted = true;
      }
    }
  }
  if (rightClick == true) {
    rightClick = false;
    leftClick = false;
    inOption = false;
    lcd.clear();
  }
}

void about() {
  buttonDebounce();
  if (rightClick == true) {
    lcd.clear();
    buttonsMode = true;
    rightClick = false;
    leftClick = false;
    inOption = false;
    nextMessage = 0;
  } else {
    buttonsMode = false;
    lcd.setCursor(0, 0);
    lcd.print(aboutMessages[nextMessage]);
    lcd.setCursor(0, 1);
    lcd.print(aboutMessages[nextMessage + 1]);
    if (millis() - messageTime >= messagePeriod) {
      nextMessage++;
      nextMessage %= aboutMessagesCount;
      messageTime = millis();
      lcd.clear();
    }
  }
}
void displayImage(uint64_t image) {  // de pe net
  for (int i = 0; i < 8; i++) {
    byte row = (image >> i * 8) & 0xFF;
    for (int j = 0; j < 8; j++) {
      lc.setLed(0, i, j, bitRead(row, j));
    }
  }
}

void gameDisplay() {
  if (livesRemaining == 1) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
  if (initDisplay == true) {
    livesRemaining = maxLives;
    lcd.clear();
    buttonsMode = 0;
    lcd.setCursor(0, 0);
    lcd.print(displayMessages[0]);
    lcd.print(playerName[0]);
    lcd.write(byte(0));
    lcd.write(byte(1));
    lcd.setCursor(0, 1);
    lcd.print(displayMessages[1]);
    initDisplay = false;
  }
  if (updateDisplay == true) {
    lcd.setCursor(7, 1);
    if (livesRemaining > 0) {
      for (byte i = 0; i < livesRemaining; i++) {
        lcd.write(byte(3));
      }
    }
    lcd.print(displayMessages[displayMessagesCount - livesRemaining]);
    updateDisplay = false;
  }
}

void endGame() {
  buttonDebounce();
  buttonsMode = 1;
  displayImage(CLEAR);
  if (endDisplay == false) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(endGameMessages[0]);
    lcd.setCursor(0, 1);
    endDisplay = true;
  }
  if (rightClick == 1) {
    displayImage(CLEAR);
    livesRemaining = 0;
    endDisplay = false;
    inOption = 0;
    rightClick = 0;
    settingsMode = 1;
    leftClick = 0;
    initDisplay = true;
    updateDisplay = true;
  }
  if (leftClick == 1) {
    displayImage(CLEAR);
    livesRemaining = 0;
    livesRemaining = 0;
    initDisplay = true;
    leftClick = 0;
    endDisplay = false;
    updateDisplay = true;
    game();
  }
}

void game() {
  if (endDisplay == false) {
    if (millis() - livesTime >= livesPeriod) {
      updateDisplay = true;
      livesRemaining--;
      livesTime = millis();
      if (livesRemaining == 0) {
        endGame();
      }
    }
    gameDisplay();
    optionsMode = 0;
    if (currentButton != -1) {
      buttonDebounce();
    }

    while (xPos + 2 < matrixSize - 1 && matrix[xPos + 2][yPos] == 0) {
      matrix[xPos][yPos] = 0;
      xPos++;
      matrixChanged = true;
    }

    if (millis() - blinkTime >= blinkPeriod) {

      blinkTime = millis();

      if (matrix[xPos][yPos] == 1) {
        matrix[xPos][yPos] = 0;
      } else {
        matrix[xPos][yPos] = 1;
      }

      matrixChanged = true;
    }

    if (millis() - lastMoved > moveInterval) {

      updatePositions(aimDirection, aimMode);  //game
      aimDirection %= aimPosibilities;
      lastMoved = millis();
    }
    if (matrixChanged == true) {
      updateMatrix();
      matrixChanged = false;
    }

    if (aimMode == 1) {
      aim();
    }

    resetMatrix();

    if (millis() - aimTime >= aimPeriod) {
      for (int row = 0; row < matrixSize; row++) {
        for (int col = 0; col < matrixSize; col++) {
          if (aimMatrix[row][col] == 1) {

            Serial.println("blink");
            if (matrix[row][col] == 1) {
              matrix[row][col] = 0;
            } else {
              matrix[row][col] = 1;
            }
          }
        }
      }
      aimTime = millis();
    }

    if (startShooting) {
      shoot();
    }
  } else {
    endGame();
  }
}

void updateMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {

      lc.setLed(0, row, col, matrix[row][col]);
    }
  }
}

void updatePositions(byte& increment, bool specialNeed) {
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);

  xLastPos = xPos;
  yLastPos = yPos;

  byte lastTerrain = 0;
  if (millis() - moveTime >= movePeriod && moveDone == 1) {
    moveDone = 0;
    moveTime = millis();
    movePrinted = false;
  }
  if (moveDone == 0) {
    if (xValue > maxThreshold) {
      if (specialNeed == 0) {
        if (xPos > 0 && ((matrix[xPos + 1][yPos - 1] == 0 && matrix[xPos + 2][yPos - 1] == 1) || (matrix[xPos + 1][yPos - 1] == 0 && matrix[xPos + 2][yPos - 1] == 1))) {
          xPos++;
          moveDone = 1;
        }
      }
    }

    if (xValue < minThreshold) {
      if (specialNeed == 0) {
        if (matrix[xPos - 1][yPos] != 1) {
          if (xPos > 0 && ((matrix[xPos][yPos - 1] == 1 && matrix[xPos - 1][yPos - 1] == 0) || (matrix[xPos][yPos + 1] == 1 && matrix[xPos - 1][yPos + 1] == 0))) {
            xPos--;
            moveDone = 1;
          }
        }
      }
    }

    if (yValue < minThreshold) {
      if (specialNeed == 0) {
        if (matrix[xPos][yPos + 1] == 0) {
          generateMap(right);
        }
      } else {
        increment++;
        moveDone = 1;
      }
    }

    if (yValue > maxThreshold) {
      if (specialNeed == false) {
        if (matrix[xPos][yPos - 1] == 0) {
          generateMap(left);
        }
      } else {
        increment--;
        moveDone = 1;
      }
    }
  }

  if (xPos != xLastPos || yPos != yLastPos) {
    matrixChanged = true;
    matrix[xLastPos][yLastPos] = lastTerrain;
    matrix[xPos][yPos] = 1;
  }
}

void shoot() {

  if (firstShoot == true) {
    aimMode = 0;
    resetMatrix();
    firstShoot = false;
    switch (oldAdimDirection) {
      case 2:
        {
          xBullet = xPos - 1;
          yBullet = yPos;
        }
        break;
      case 3:
        {
          xBullet = xPos - 1;
          yBullet = yPos + 1;
        }
        break;
      case 4:
        {
          xBullet = xPos;
          yBullet = yPos + 1;
        }
        break;
      case 1:
        {
          xBullet = xPos - 1;
          yBullet = yPos - 1;
        }
        break;
      case 0:
        {
          xBullet = xPos;
          yBullet = yPos - 1;
        }
        break;
    }
    if (matrix[xBullet][yBullet] == 1) {
      matrix[xBullet][yBullet] = 0;
      xBullet = -1;
      yBullet = -1;
      startShooting = false;
      firstShoot = false;
    } else {
      matrix[xBullet][yBullet] = 1;
    }
  }


  if (millis() - bulletTime >= bulletPeriod) {
    if (xBullet != -1 && yBullet != -1) {
      switch (oldAdimDirection) {

        case 2:
          {
            matrix[xBullet][yBullet] = 0;
            xBullet--;
            if (matrix[xBullet][yBullet] == 1) {
              matrix[xBullet][yBullet] = 0;
              xBullet = -1;
              yBullet = -1;
              startShooting = false;
              firstShoot = false;
            } else {
              matrix[xBullet][yBullet] = 1;
            }
          }
          break;
        case 3:
          {
            matrix[xBullet][yBullet] = 0;
            xBullet--;
            yBullet++;
            if (matrix[xBullet][yBullet] == 1) {
              matrix[xBullet][yBullet] = 0;
              xBullet = -1;
              yBullet = -1;
              startShooting = false;
              firstShoot = false;
            } else {
              matrix[xBullet][yBullet] = 1;
            }
          }
          break;
        case 4:
          {
            matrix[xBullet][yBullet] = 0;
            yBullet++;
            if (matrix[xBullet][yBullet] == 1) {
              matrix[xBullet][yBullet] = 0;
              xBullet = -1;
              yBullet = -1;
              startShooting = false;
              firstShoot = false;
            } else {
              matrix[xBullet][yBullet] = 1;
            }
          }
          break;
        case 1:
          {
            matrix[xBullet][yBullet] = 0;
            xBullet--;
            yBullet--;
            if (matrix[xBullet][yBullet] == 1) {
              matrix[xBullet][yBullet] = 0;
              xBullet = -1;
              yBullet = -1;
              startShooting = false;
              firstShoot = false;
            } else {
              matrix[xBullet][yBullet] = 1;
            }
          }
          break;
        case 0:
          {
            matrix[xBullet][yBullet] = 0;
            xBullet = xPos;
            yBullet--;
            if (matrix[xBullet][yBullet] == 1) {
              matrix[xBullet][yBullet] = 0;
              xBullet = -1;
              yBullet = -1;
              startShooting = false;
              firstShoot = false;
            } else {
              matrix[xBullet][yBullet] = 1;
            }
          }
          break;
      }
    }
    if (yBullet < 0 || yBullet > matrixSize - 2 || xBullet > matrixSize - 2 || xBullet < 0) {

      if (xBullet != matrixSize - 1) {
        matrix[xBullet][yBullet] = 0;
      }

      xBullet = -10;
      yBullet = -10;
      startShooting = false;
      firstShoot = false;
    }

    matrixChanged = true;
    bulletTime = millis();
  }
}

void resetMatrix() {
  if (aimMode == false && oldAimMode == true) {
    for (int row = 0; row < matrixSize; row++) {
      for (int col = 0; col < matrixSize; col++) {
        if (aimMatrix[row][col] == 1 && matrix[row][col] == 1) {
          aimMatrix[row][col] = 0;
          matrix[row][col] = 0;
        }
      }
    }

    aimDirection = 2;
    firstReading = true;
  }
}

void aim() {
  bool changeDone = false;
  if (aimDirection != oldAdimDirection) {
    for (int row = 0; row < matrixSize; row++) {
      for (int col = 0; col < matrixSize; col++) {
        if (aimMatrix[row][col] == 1 && matrix[row][col] == 1) {
          aimMatrix[row][col] = 0;
          matrix[row][col] = 0;
        }
      }
    }
    firstReading = true;
  }

  for (int row = matrixSize - 1; row >= 0; row--) {
    for (int col = 0; col < matrixSize - 1; col++) {
      switch (aimDirection) {
        case 2:
          {
            if (col == yPos && row < xPos) {
              if (firstReading == true) {
                if (matrix[row][col] == 0) {
                  aimMatrix[row][col] = 1;
                }
              }

              if (matrix[row][col] == 0 && aimMatrix[row][col] == 1) {
                matrix[row][col] = 1;
              }
              changeDone = true;
            }
          }
          break;
        case 3:
          {

            if (col + row == xPos + yPos && row < xPos) {
              if (firstReading == true) {
                if (matrix[row][col] == 0) {
                  aimMatrix[row][col] = 1;
                }
              }
              if (aimMatrix[row][col] == 1) {
                matrix[row][col] = 1;
              }
              changeDone = true;
            }
          }
          break;
        case 4:
          {
            if (row == xPos && col > yPos) {
              if (firstReading == true) {
                if (matrix[row][col] == 0) {
                  aimMatrix[row][col] = 1;
                }
              }

              if (matrix[row][col] == 0 && aimMatrix[row][col] == 1) {
                matrix[row][col] = 1;
                Serial.println("aici");
              }
              changeDone = true;
            }
          }
          break;
        case 1:
          {
            if (abs(row - col) == abs(xPos - yPos) && row < xPos && col < yPos) {
              if (firstReading == true) {
                if (matrix[row][col] == 0) {
                  aimMatrix[row][col] = 1;
                }
              }

              if (matrix[row][col] == 0 && aimMatrix[row][col] == 1) {
                matrix[row][col] = 1;
                Serial.println("aici");
              }
              changeDone = true;
            }
          }
          break;
        case 0:
          {
            if (row == xPos && col < yPos) {
              if (firstReading == true) {
                if (matrix[row][col] == 0) {
                  aimMatrix[row][col] = 1;
                }
              }

              if (matrix[row][col] == 0 && aimMatrix[row][col] == 1) {
                matrix[row][col] = 1;
                Serial.println("aici");
              }
              changeDone = true;
            }
          }
          break;
      }
    }
  }
  if (changeDone == true) {
    matrixChanged = true;
  }
  oldAdimDirection = aimDirection;
  firstReading = false;
  oldAimMode = aimMode;
}

void generateMap(byte nextPos) {

  if (nextPos == right) {

    Serial.println("dreapta");
    for (int row = 0; row < matrixSize - 1; row++) {
      for (int col = 0; col < matrixSize - 1; col++) {
        if (!(row == xPos && col == yPos)) {
          matrix[row][col] = matrix[row][col + 1];
        }
      }
    }
    matrix[xPos][yPos - 1] = 0;



    byte nextHight = random(oldHight - 1, oldHight + maxHight - 3);
    if (nextHight > maxHight) {
      nextHight = maxHight - 1;
    }

    if (nextHight == 0) {
      nextHight = 1;
    }

    oldHight = nextHight;
    for (int row = matrixSize - 2; row >= 0; row--) {
      if (nextHight > 0) {
        matrix[row][matrixSize - 1] = 1;
        nextHight--;
      } else {
        matrix[row][matrixSize - 1] = 0;
      }
    }

    matrix[matrixSize - 1][matrixSize - 1] = 1;

    matrixChanged = true;
  }

  if (nextPos == left) {

    Serial.println("stanga");

    for (int row = 0; row < matrixSize - 1; row++) {
      for (int col = matrixSize - 1; col >= 1; col--) {
        if (!(row == xPos && col == yPos)) {
          matrix[row][col] = matrix[row][col - 1];
        }
      }
    }
    matrix[xPos][yPos + 1] = 0;

    byte nextHight = random(oldHight - 1, oldHight + maxHight - 3);
    if (nextHight > maxHight) {
      nextHight = maxHight - 1;
    }

    if (nextHight == 0) {
      nextHight = 1;
    }

    oldHight = nextHight;
    for (int row = matrixSize - 2; row >= 0; row--) {
      if (nextHight > 0) {
        matrix[row][0] = 1;
        nextHight--;
      } else {
        matrix[row][0] = 0;
      }
    }
    matrix[matrixSize - 1][0] = 1;
    matrixChanged = true;
  }
}

void buttonDebounce() {

  if (possiblePress) {

    if (shouldStartCounter) {
      lastDebounceTime = millis();
      shouldStartCounter = false;
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {

      if (currentButton == 0) {
        buttonReading = digitalRead(aimPin);
      }

      if (currentButton == 1) {
        buttonReading = digitalRead(shootPin);
      }

      if (buttonReading == LOW) {
        if (currentButton == 0) {
          aimMode = !aimMode;
          rightClick = true;
        }
        if (currentButton == 1) {
          startShooting = true;
          firstShoot = true;
          leftClick = true;
        }
        lastReading = buttonReading;
        lastReading = false;
        currentButton = -1;
      }
      possiblePress = false;
    }
  }
}

void handleInterruptAim() {
  possiblePress = true;
  shouldStartCounter = true;
  currentButton = 0;
}

void handleInterruptShoot() {
  possiblePress = true;
  shouldStartCounter = true;
  currentButton = 1;
}