// Here we declare the vector with the pins attached to each button
const int buttonPinArray[3] = {8, 9, 10};
// Here we declare the vector with the pins attached to the LED from each floor
const int ledPinArray[3] = {5, 6, 7};
// H
const int buzzerPin = 13;
int buzzerState = -1;
const int workingLedPin = 12;
bool workingLedState = 0;
unsigned long workingLedPeriod = 0;
unsigned long workingLedTime = 500;
bool reading = false;
bool lastReading = false;
unsigned long lastDebounceTime = 0;
bool buttonState = false;
bool ledStateArray[3] = {true, true, true};
unsigned long debounceDelay = 100;
int currentButton = -1;
int nextFloor = -1;
int currentFloor = -1;
bool currentLedState = true;
unsigned long closingTime = 4000;
unsigned long closingPeriod = 0;
bool closingState = true;

void setup() {
  // put your setup code here, to run once:
  for (int i = 0; i < 3; i++){
    pinMode(buttonPinArray[i], INPUT_PULLUP);
    pinMode(ledPinArray[i], OUTPUT);
  }
  pinMode(workingLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  
  Serial.begin(9600);
}
void readButtonValue(int buttonPin){
  
  reading = digitalRead(buttonPin);
  if (reading != lastReading) {
  lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
  if (reading != buttonState) {
  buttonState = reading;
  if (buttonState == HIGH) {
    nextFloor = currentButton;
    
    lastDebounceTime = 0;
    reading = false;
    lastReading = false;
    buttonState = false;
  }
  }
  }
  lastReading = reading;
}
void loop() {
  for (int i = 0; i < 3; i++){
      if (digitalRead(buttonPinArray[i]) == 0 && i != currentFloor){
        currentButton = i;
    }
  }
  readButtonValue(buttonPinArray[currentButton]);
  
  if (nextFloor != -1 && nextFloor != currentFloor && closingState == false){      
      closingPeriod = millis();
      closingState = true;
  }
  if (closingState == true && (millis() - closingPeriod) > closingTime/2){
      currentLedState = false;
      
    if ((millis() - closingPeriod) > closingTime){
      Serial.println("aici da");
      if (currentFloor > nextFloor){
        currentFloor -= 1;
      }
      if (currentFloor < nextFloor){
          currentFloor += 1;
      }
      if (currentFloor == nextFloor){
          //nextFloor = -1;
        buzzerState = 1;
        Serial.println("a ajuns");
        workingLedState = 1;
        workingLedPeriod = millis();
      }
      closingState = false;
      closingPeriod = 0;
      currentLedState = true;
    }
  }

    if (buzzerState == 1){
      tone(buzzerPin, 2345, 500);
      buzzerState = -1;
      //noTone(buzzerPin);
    }

  if (currentFloor != -1){
     if (currentFloor != nextFloor){
       // tone(buzzerPin, 100);
      tone(buzzerPin, 250);
      if (millis() - workingLedPeriod > workingLedTime){
        workingLedState = !workingLedState;
        workingLedPeriod = millis();
      }
        digitalWrite(workingLedPin, workingLedState);
    }    
    if (currentFloor == nextFloor){
      digitalWrite(workingLedPin, HIGH);
      //noTone(buzzerPin);
    }
    
  }
  else
    digitalWrite(workingLedPin, LOW);

  //Serial.println(currentFloor);
  //Serial.println(nextFloor);
  //digitalWrite(workingLedPin, workingLedState);
  digitalWrite(ledPinArray[currentFloor],currentLedState);
}
