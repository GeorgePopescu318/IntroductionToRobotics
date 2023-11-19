#include <EEPROM.h>
// readProducedValue function from https://forum.arduino.cc/t/serial-input-basics-updated/382007/3
//EEPROM
//byte 0 - Sensor sampling
//byte 1 - 2 - Ultra sonic threshold
//byte 3 - 4 - LDR threshold
//byte 5 - 24 - Ultra logger 5 7 9 11 13 15 17 19 21 23
//byte 25 - 44 - LDR logger
//byte 45 - order of loging
const byte maxMenuOptions = 4;

const byte ultrasonicEEPROMstart = 5;
const byte LDREEPROMstart = 25;
const byte maxLoggerCount = 18;
const byte loggerEEPROMbyte = 45;
const int resetValue = 0;

const byte backButton = 4;
const byte redPin = 2;
const byte greenPin = 3;
const byte bluePin = 4;

byte currentLED = 0;
byte currentMode = 1;

const byte trigPin = 9;
const byte echoPin = 10;
long duration = 0;
int distance = 0;
int LDRvalue = 0;

long samplingDuration = 0;
bool showValues = false;

const int oneSec = 1000;
const byte LDRPin = A0;
const byte samplingEEPROMbyte = 0;
const byte ultrasonicEEPROMbyte = 1;
const byte LDREEPROMbyte = 3;
const byte loggerEEPROMstart = 5;
const byte loggerEEPROMend = 45;
const byte maxSamplingRate = 10;
const int maxSensorThreshold = 2100;
bool printStatement = true;
bool firstSampRead = true;
byte selectedOption = 0;
byte currentMenu = 0;
char readString[5] = "";
byte noChar = 0;
void (*myFunction[])(void){
  sensorMenu,
  resetMenu,
  statusMenu,
  RGBcontrol,
  mainMenu
};

void setup() {
  Serial.begin(9600);
  pinMode(LDRPin, INPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  mainMenu();
}

void loop() {
  if (printStatement == true) {
    currentMenu = readProcessedValue();
  }

  if (currentMenu != 0 && currentMenu <= maxMenuOptions) {
    myFunction[currentMenu - 1]();
  }
  sensoring();

  if (currentMode == 1) {
    if (currentLED == 1) {
      digitalWrite(greenPin, HIGH);
      digitalWrite(redPin, LOW);
    } else {
      digitalWrite(redPin, HIGH);
      digitalWrite(greenPin, LOW);
    }
  }
}

int readProcessedValue() {
  char readChar;
  if (Serial.available() > 0) {
    readChar = Serial.read();
    if (readChar != '\n') {
      if (readChar != '-') {
        readString[noChar] = readChar;
        noChar++;
      }
    } else {
      readString[noChar] = '\0';
      noChar = 0;
      int readNumber = 0;
      readNumber = atoi(readString);
      return readNumber;
    }
  }
  return 0;
}
void sensoring() {
  byte samplingRate;
  EEPROM.get(samplingEEPROMbyte, samplingRate);
  if (millis() - samplingDuration >= samplingRate * oneSec) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = 0;
    duration = pulseIn(echoPin, HIGH);
    distance = 0;
    distance = duration * 0.034 / 2;

    LDRvalue = analogRead(LDRPin);

    int maxLDRvalue = 0;
    int maxUltrasonicValue = 0;
    EEPROM.get(ultrasonicEEPROMbyte, maxUltrasonicValue);
    EEPROM.get(LDREEPROMbyte, maxLDRvalue);

    if (currentMode == 1) {
      if (LDRvalue <= maxLDRvalue && distance <= maxUltrasonicValue) {
        currentLED = 1;
      } else {
        currentLED = 0;
      }
    }

    if (showValues == true) {
      Serial.print(F("Ultrasonic Sensor Distance:\n"));
      Serial.print(distance);
      Serial.print("\n");
      Serial.print("LDR value:\n");
      Serial.print(LDRvalue);
      Serial.print("\n");
      Serial.print(F("You can exit with 4\n"));
    }
    samplingDuration = millis();
    byte loggerCount = 0;
    EEPROM.get(loggerEEPROMbyte, loggerCount);
    EEPROM.put(loggerCount + ultrasonicEEPROMstart, distance);
    EEPROM.put(loggerCount + LDREEPROMstart, LDRvalue);
    if (loggerCount == maxLoggerCount)
      loggerCount = 0;
    else {
      loggerCount += 2;
    }
    EEPROM.update(loggerEEPROMbyte, loggerCount);
  }
}
void mainMenu() {
  Serial.print("Main Menu:\n1. Sensor Settings\n2. Reset Logger Data\n3. System Status\n4. RGB LED Control\n");
}
void sensorMenu() {
  if (printStatement == true) {
    Serial.print(F("1. Sensor Settings:\n\t1.1 Sensors Sampling Interval\n\t1.2 Ultrasonic Alert Threshold\n\t1.3 LDR Alert Threshold\n\t1.4 Back\n"));
    printStatement = false;
  }
  if (selectedOption == 0) {
    selectedOption = readProcessedValue();
  }
  switch (selectedOption) {
    case 1:
      {
        if (firstSampRead == true) {
          Serial.print("Enter Sampling Interval between 1 and 10:\n");
          firstSampRead = false;
        }
        byte samplingValue;
        samplingValue = readProcessedValue();
        if (samplingValue != 0) {
          Serial.println(samplingValue);
        }
        if (samplingValue > 0 && samplingValue <= maxSamplingRate) {
          EEPROM.put(samplingEEPROMbyte, samplingValue);
          firstSampRead = true;
          selectedOption = 0;
          Serial.print("Good Value!\n");
          Serial.print(F("1. Sensor Menu:\n\t1.1 Sensors Sampling Interval\n\t1.2 Ultrasonic Alert Threshold\n\t1.3 LDR Alert Threshold\n\t1.4 Back\n"));
        } else {
          if (samplingValue > maxSamplingRate) {
            Serial.print("Incorrect value!\n");
          }
        }
      }
      break;
    case 2:
      {
        if (firstSampRead == true) {
          Serial.print("Enter Ultrasonic Max Threshold under 2100:\n");
          firstSampRead = false;
        }

        int ultrasonicValue;
        ultrasonicValue = readProcessedValue();
        if (ultrasonicValue != 0)
          Serial.println(ultrasonicValue);

        if (ultrasonicValue != 0 && ultrasonicValue <= maxSensorThreshold) {
          EEPROM.put(ultrasonicEEPROMbyte, ultrasonicValue);
          firstSampRead = true;
          selectedOption = 0;
          Serial.print("Good Value!\n");
          Serial.print(F("1. Sensor Menu:\n\t1.1 Sensors Sampling Interval\n\t1.2 Ultrasonic Alert Threshold\n\t1.3 LDR Alert Threshold\n\t1.4 Back\n"));
        } else {
          if (ultrasonicValue < 0 || ultrasonicValue > maxSensorThreshold) {
            Serial.print("Incorrect value!\n");
          }
        }
      }
      break;
    case 3:
      {
        if (firstSampRead == true) {
          Serial.print("Enter LDR Max Threshold under 1023:\n");
          firstSampRead = false;
        }

        int LDRvalue;
        LDRvalue = readProcessedValue();
        if (LDRvalue != 0) {
          Serial.println(LDRvalue);
        }
        if (LDRvalue != 0 && LDRvalue <= 1023) {
          EEPROM.put(LDREEPROMbyte, LDRvalue);
          firstSampRead = true;
          selectedOption = 0;
          Serial.print("Good Value!\n");
          Serial.print(F("1. Sensor Menu:\n\t1.1 Sensors Sampling Interval\n\t1.2 Ultrasonic Alert Threshold\n\t1.3 LDR Alert Threshold\n\t1.4 Back\n"));
        } else {
          if (LDRvalue < 0 || LDRvalue > maxSensorThreshold) {
            Serial.print("Incorrect value!\n");
          }
        }
      }
      break;
    case backButton:
      {
        printStatement = true;
        selectedOption = 0;
        mainMenu();
      }
      break;
    default:
      {
        selectedOption = 0;
      }
      break;
  }
}
void resetMenu() {
  if (printStatement == true) {
    Serial.print(F("2. Reset Logger Data\nAre you sure you want to reset leogged data?\n\t2.1 YES\n\t2.2 NO\n"));
    printStatement = false;
  }
  if (selectedOption == 0) {
    selectedOption = readProcessedValue();
  }
  switch (selectedOption) {
    case 1:
      {
        for (int i = loggerEEPROMstart; i <= loggerEEPROMend; i++) {
          EEPROM.put(i, resetValue);
        }
        EEPROM.put(loggerEEPROMbyte, resetValue);
        Serial.print("Done!\n");
        firstSampRead = true;
        printStatement = true;
        selectedOption = 0;
        mainMenu();
      }
      break;
    case 2:
      {
        printStatement = true;
        selectedOption = 0;
        mainMenu();
      }
      break;
    default:
      {
        selectedOption = 0;
      }
      break;
  }
}
void statusMenu() {
  if (printStatement == true) {
    Serial.print(F("3. System Status:\n\t3.1 Current Sensor Readings\n\t3.2 Current Sensor Settings\n\t3.3 Display Logged Data\n\t1.4 Back\n"));
    printStatement = false;
  }
  if (selectedOption == 0) {
    selectedOption = readProcessedValue();
  }
  switch (selectedOption) {
    case 1:
      {

        showValues = true;
        byte stopOPtion = readProcessedValue();
        if (stopOPtion == backButton) {
          selectedOption = 0;
          showValues = false;
          Serial.print(F("3. System Status:\n\t3.1 Current Sensor Readings\n\t3.2 Current Sensor Settings\n\t3.3 Display Logged Data\n\t1.4 Back\n"));
        }
      }
      break;
    case 2:
      {
        byte samplingRate;
        int ultrasonicThreshold;
        int LDRthreshold;
        Serial.print("Sampling Rate:\n");
        EEPROM.get(samplingEEPROMbyte, samplingRate);
        Serial.print(samplingRate);
        Serial.print("\nUltrasonic Max Threshold:\n");
        EEPROM.get(ultrasonicEEPROMbyte, ultrasonicThreshold);
        Serial.print(ultrasonicThreshold);
        Serial.print("\nLDR Max Threshold:\n");
        EEPROM.get(LDREEPROMbyte, LDRthreshold);
        Serial.print(LDRthreshold);
        Serial.print("\n");
        firstSampRead = true;
        selectedOption = 0;
        Serial.print(F("3. System Status:\n\t3.1 Current Sensor Readings\n\t3.2 Current Sensor Settings\n\t3.3 Display Logged Data\n\t1.4 Back\n"));
      }
      break;
    case 3:
      {
        Serial.print("The last 10 Ultrasonic sensor values:\n");
        for (int i = ultrasonicEEPROMstart; i <= LDREEPROMstart - 1; i += 2) {
          int printedValue = 0;
          EEPROM.get(i, printedValue);
          Serial.println(printedValue);
        }
        Serial.print("The last 10 LDR sensor values:\n");
        for (int i = LDREEPROMstart; i <= loggerEEPROMbyte - 1; i += 2) {
          int printedValue = 0;
          EEPROM.get(i, printedValue);
          Serial.println(printedValue);
        }
        firstSampRead = true;
        selectedOption = 0;
        Serial.print(F("3. System Status:\n\t3.1 Current Sensor Readings\n\t3.2 Current Sensor Settings\n\t3.3 Display Logged Data\n\t1.4 Back\n"));
      }
      break;
    case backButton:
      {
        printStatement = true;
        selectedOption = 0;
        mainMenu();
      }
      break;
    case 5:
      {
        Serial.print(F("3. System Status:\n\t3.1 Current Sensor Readings\n\t3.2 Current Sensor Settings\n\t3.3 Display Logged Data\n\t1.4 Back\n"));
        selectedOption = 0;
      }
      break;
    default:
      {
        selectedOption = 0;
      }
      break;
  }
}

void RGBcontrol() {
  if (printStatement == true) {
    Serial.print(F("4. RGB LED Control\n\t4.1 Manual Color Control\n\t4.2 LED Toggle Automatic ON/OFF\n\t4.3 Back\n"));
    printStatement = false;
  }
  if (selectedOption == 0) {
    selectedOption = readProcessedValue();
  }
  switch (selectedOption) {
    case 1:
      {
        if (currentMode == 0) {
          if (firstSampRead == true) {
            Serial.print(F("Each color represents a digit, red for"));
            Serial.print(redPin);
            Serial.print(", green for ");
            Serial.print(greenPin);
            Serial.print(" and blue for");
            Serial.print(bluePin);
            Serial.print(F(", enter a number that has these digits and see what happens!\n"));
            firstSampRead = false;
          }

          int manualColor = 0;
          bool colorDone = false;
          manualColor = readProcessedValue();
          digitalWrite(redPin, LOW);
          digitalWrite(greenPin, LOW);
          digitalWrite(bluePin, LOW);
          bool red = false;
          bool green = false;
          bool blue = false;
          while (manualColor > 0) {
            if (manualColor % 10 == redPin)
              red = true;
            if (manualColor % 10 == greenPin)
              green = true;
            if (manualColor % 10 == bluePin)
              blue = true;
            colorDone = true;
            manualColor /= 10;
          }
          digitalWrite(redPin, red);
          digitalWrite(greenPin, green);
          digitalWrite(bluePin, blue);
          if (manualColor == 0 && colorDone == true) {
            Serial.print(F("Color Set\n"));
            firstSampRead = true;
            selectedOption = 0;
            Serial.print(F("4. RGB LED Control\n\t4.1 Manual Color Control\n\t4.2 LED Toggle Automatic ON/OFF\n\t4.3 Back\n"));
          }

        } else {
          firstSampRead = true;
          selectedOption = 0;
          Serial.print(F("4. RGB LED Control\n\t4.1 Manual Color Control\n\t4.2 LED Toggle Automatic ON/OFF\n\t4.3 Back\n"));
        }
      }
      break;
    case 2:
      {
        digitalWrite(redPin, LOW);
        digitalWrite(greenPin, LOW);
        digitalWrite(bluePin, LOW);
        currentMode = !currentMode;
        firstSampRead = true;
        selectedOption = 0;
        Serial.print(F("Modification Done\n"));
        Serial.print(F("4. RGB LED Control\n\t4.1 Manual Color Control\n\t4.2 LED Toggle Automatic ON/OFF\n\t4.3 Back\n"));
      }
      break;
    case backButton -1:
      {
        printStatement = true;
        selectedOption = 0;
        mainMenu();
      }
      break;
    default:
      {
        selectedOption = 0;
      }
      break;
  }
}
