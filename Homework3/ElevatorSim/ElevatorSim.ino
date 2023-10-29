// Here I declare the vector with the pins attached to each button
const int buttonPinArray[3] = {8, 9, 10};
// Here I declare the vector with the pins attached to the LED from each floor
const int ledPinArray[3] = {5, 6, 7};
// Here I declare the buzzerPin and the buzzerState function that dictates when and how the buzzer makes sounds
const int buzzerPin = 13;
int buzzerState = -1;
// The sounds that the buzzer make and the time for the arriving sound
const int arriveSound = 2345;
const int travellingSound = 250;
const int arriveSoundTime = 500;
// The pin of the LED that represents the working condition of the elevator
const int workingLedPin = 12;
// The boolean variable that changes the pin state from always HIGH to being HIGH oance every workingLedPeriod ms (500ms is a good number)
bool workingLedState = 0;
// I use this variable to determine when the workingLedTime has passed so that that I can flash the LED
unsigned long workingLedPeriod = 0;
// Determine how long should the LED be HIGH and LOW
unsigned long workingLedTime = 500;
// The following variables will be used in readButtonValue (the debounce function)
// With reading I detect when the button is being pressed
bool reading = false;
// lastReading is used alongside lastDebounceTime to  determine if the noise has ended and a real press ocured
bool lastReading = false;
unsigned long lastDebounceTime = 0;
// buttonState holds the current value of a button, one at a time
bool buttonState = false;
// this dictates how much time has the button to be pressed so that the reading can be valid
unsigned long debounceDelay = 100;
// I've used only a set of variables for each of the buttons because only the reading of one button at a time can be valide, currentButton represents the button which is in the
// spotline at a time
int currentButton = -1;
// the next floor to which the elevator will go is  kept in nextFloor
int nextFloor = -1;
// the current floor at which the elevator is   
int currentFloor = -1;
// the state of the LED from the current floor is
bool currentLedState = true;
//time that passes from one floor to the other, alongside the period variable for counting that time 
unsigned long closingTime = 4000;
unsigned long closingPeriod = 0;
bool closingState = true;

void setup() {
  // mainly in the setup I set the pinModes for the buttons, the LEDs and the buzzer
  for (int i = 0; i < 3; i++){
    pinMode(buttonPinArray[i], INPUT_PULLUP);
    pinMode(ledPinArray[i], OUTPUT);
  }
  pinMode(workingLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600); // for testing 
}
void readButtonValue(int buttonPin){
  // the following function is used to detect the unwanted noise that can appear while pushing the button
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
  // with the for I check from which button did I receive a reading and then check more readings from that button with the above function
  for (int i = 0; i < 3; i++){
      if (digitalRead(buttonPinArray[i]) == 0 && i != currentFloor){
        currentButton = i;
    }
  }
  readButtonValue(buttonPinArray[currentButton]);
  
  // if the elevator needs to go to another floor I will use this if to start the millis for the closing time to be accurate
  if (nextFloor != -1 && nextFloor != currentFloor && closingState == false){      
      closingPeriod = millis();
      closingState = true;
  }

  //if half of the time passed the LED will close and after another half the next one will open
  if (closingState == true && (millis() - closingPeriod) > closingTime/2){
      currentLedState = false;
      
    if ((millis() - closingPeriod) > closingTime){
      if (currentFloor > nextFloor){
        currentFloor -= 1;
      }
      if (currentFloor < nextFloor){
          currentFloor += 1;
      }
      if (currentFloor == nextFloor){
        buzzerState = 1;
        workingLedState = 1;
        workingLedPeriod = millis();
      }
      closingState = false;
      closingPeriod = 0;
      currentLedState = true;
    }
  }
  // here the buzzer makes the cling sound that tells when the elevator arrived at the desired floor
  if (buzzerState == 1){
    tone(buzzerPin, arriveSound, arriveSoundTime);
    buzzerState = -1;
  }
  // the following sequence of code is used to control the LED that represents the working state of the elevator
  if (currentFloor != -1){
     if (currentFloor != nextFloor){
      tone(buzzerPin, travellingSound);
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
  digitalWrite(ledPinArray[currentFloor],currentLedState);
}
