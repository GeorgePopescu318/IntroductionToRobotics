// base game reference Tank Wars
#include "LedControl.h"

const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;

const int xPin = A0;
const int yPin = A1;

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);

byte matrixBrightness = 2;

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

void setup() {
  Serial.begin(9600);
  
  lc.shutdown(0, false);                 
  lc.setIntensity(0, matrixBrightness);  
  lc.clearDisplay(0);                    

  matrix[xPos][yPos] = 1;  

  randomSeed(analogRead(unusedPin));

  pinMode(aimPin, INPUT_PULLUP);
  pinMode(shootPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(aimPin), handleInterruptAim, FALLING);
  attachInterrupt(digitalPinToInterrupt(shootPin), handleInterruptShoot, FALLING);
}
void loop() {

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
                                             
    updatePositions();                        
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
}

void updateMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {

      lc.setLed(0, row, col, matrix[row][col]);
    }
  }
}

void updatePositions() {
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);

  xLastPos = xPos;
  yLastPos = yPos;

  byte lastTerrain = 0;

  if (xValue > maxThreshold) {
    if (xPos > 0 && ((matrix[xPos + 1][yPos - 1] == 0 && matrix[xPos + 2][yPos - 1] == 1) || (matrix[xPos + 1][yPos - 1] == 0 && matrix[xPos + 2][yPos - 1] == 1))) {
      xPos++;
    }
  }

  if (xValue < minThreshold) {
    if (matrix[xPos - 1][yPos] != 1) {
      if (xPos > 0 && ((matrix[xPos][yPos - 1] == 1 && matrix[xPos - 1][yPos - 1] == 0) || (matrix[xPos][yPos + 1] == 1 && matrix[xPos - 1][yPos + 1] == 0))) {
        xPos--;
      }
    }
  }

  if (yValue < minThreshold) {
    if (matrix[xPos][yPos + 1] == 0 && aimMode == 0) {
      generateMap(right);
    }

    if (aimMode == 1) {
      aimDirection++;
      aimDirection %= aimPosibilities;
    }
  }

  if (yValue > maxThreshold) {
    if (matrix[xPos][yPos - 1] == 0 && aimMode == 0) {
      generateMap(left);
    }

    if (aimMode == 1) {
      aimDirection--;
      aimDirection %= aimPosibilities;
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
    Serial.println(xBullet);
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
        }
        if (currentButton == 1) {
          startShooting = true;
          firstShoot = true;
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