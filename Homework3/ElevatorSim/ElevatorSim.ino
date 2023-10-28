// Here we declare the vector with the pins attached to each button
int buttonPinArray[3] = {8, 9, 10};
// Here we declare the vector with the pins attached to the LED from each floor
int ledPinArray[3] = {5, 6, 7};
// H
int workingLedPin = 12;
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
  pinMode(workingLedPin,OUTPUT);
  
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
      if (currentFloor > nextFloor){
        currentFloor -= 1;
        workingLedState = 0;
      }
      else
        if (currentFloor < nextFloor){
          currentFloor += 1;
          workingLedState = 0;
        }
      else
        if (currentFloor == nextFloor){
          //nextFloor = -1;
          workingLedState = 1;
          workingLedPeriod = millis();
        }

      closingState = false;
      closingPeriod = 0;
      currentLedState = true;
    }
  }

  if (currentFloor != -1){
     if (currentFloor != nextFloor){
      if (millis() - workingLedPeriod > workingLedTime){
        workingLedState = !workingLedState;
        workingLedPeriod = millis();
      }
        digitalWrite(workingLedPin, workingLedState);
    }    
    if (currentFloor == nextFloor){
      digitalWrite(workingLedPin, HIGH);
    }
    
  }
  else
    digitalWrite(workingLedPin, LOW);
  Serial.println(currentFloor);
  digitalWrite(workingLedPin, workingLedState);
  digitalWrite(ledPinArray[currentFloor],currentLedState);
}
