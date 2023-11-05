/*
  The following code allows you to move through a 7-segment Display using a joystick, the current LED segment will blink so you know where you are,
  additionally by short pressing the joystick/an external button the current segment will turn HIGH after you move your current segment from it, and you can turn them LOW if you
  short press again. By long pressing the joystick/button all the HIGH segments will turn LOW and the current segment will return to dp (decimal point).
  
*/

const int SWPin = 2; // button pin
const int xPin = A1; // X axis pin
const int yPin = A0; // Y axis pin

const int noSegs = 8; // number of segments on the 7-Seg
const int possibleDirections = 4; // number of possible moves

//                       a,b,c, d, e,f,g,dp
//                       0,1,2, 3, 4,5,6,7
const int segsPins[8] = {5,4,9,10,11,6,7,8}; // the vector with the segments pins, the order is the alphabetical order of the segments' name

int xVal = 0; // the value from the x axis
int yVal = 0; // the value from the y axis

int currentSegIndex = noSegs - 1; // the index in the vector of the current segment (dp)
int currentSeg = segsPins[7]; // the pin of the current seg, by default dp

bool moveDone = false; // this checks if a move was already done
int nextPosition = -1; // can take a value from 0 to 3 indicating the direction of the next move (0 - up, 1 - down, 2 - left, 3 - right), -1 if there is not a valid move

unsigned long moveTimer = 0; // the following variables check if there is a window of 30 ms between moves
const unsigned long movePeriod = 30;

/*
The following matrix indicates what moves are valid and where the blinking light can go from one segment

I - Index
U - Up
D - Down
L - Left
R - Right

I    U    D    L    R
a   -1    g    f    b  
b    a    g    f   -1
c    g    d    e    dp
d    g   -1    e    c
e    g    d   -1    c
f    a    g   -1    b
g    a    d   -1   -1
dp  -1   -1    c   -1

*/
int segMatrix[noSegs][possibleDirections] = { // the matrix from above using values from segPrins
    {-1, segsPins[6], segsPins[5], segsPins[1]},
    {segsPins[0], segsPins[6], segsPins[5], -1},
    {segsPins[6], segsPins[3], segsPins[4], segsPins[7]},
    {segsPins[6], -1, segsPins[4], segsPins[2]},
    {segsPins[6], segsPins[3], -1, segsPins[2]},
    {segsPins[0], segsPins[6], -1, segsPins[1]},
    {segsPins[0], segsPins[3], -1, -1},
    {-1, -1, segsPins[2], -1}
  };


int highSegs[noSegs] = {0,0,0,0,0,0,0,0}; //vector that will contain only 1 and 0, I iterate throught the vector and if the value is 1 the letter that is on that position turns on
// by using the segsPins

const unsigned long blinkPeriod = 350; // the current seggment will be HIGH for 350 ms and LOW for another 350 ms one after the other, creating the blink
unsigned long blinkTime = 0; // this counts how much time passed between blinks
int blinkState = LOW; // the current state of the blink

//Debounce

// the volatile variables get changed in the ISR function that is triggered by the interrupt from pin 2 and the following variables are used reduce noise in the button
volatile bool possiblePress = false;
volatile bool shouldStartCounter = false;
unsigned long lastDebounceTime = 0;
const unsigned int debounceDelay = 50;
bool buttonReading = HIGH;

// the button has to be pressed 2500 ms (2.5s) so that a long press will be recorded
unsigned long longPressTime = 0;
const unsigned long longPressPeriod = 2500;
bool  longPressState = false;

// the max and min values for the joystick in X and Y directions
const int minValueUpRight = 0;
const int maxValueUpRight = 100;
const int minValueDownLeft = 950;
const int maxValueDownLeft = 1023;
const int minValThreshold = 475;
const int maxValThreshold = 575;

void setup() {
  for (int i = 0; i <= noSegs - 1; i++){
    pinMode(segsPins[i], OUTPUT);
  }
  pinMode(SWPin, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(SWPin), handleInterrupt, CHANGE); // declaration of the ISR function
}
void readAxis(){
  xVal = analogRead(xPin);
  yVal = analogRead(yPin);

  if (moveDone == true && (xVal >= minValThreshold && xVal <= maxValThreshold) && (yVal >= minValThreshold && yVal <= maxValThreshold)){
    moveDone = false;
    nextPosition = -1;
  }

  if (moveDone == false && millis() - moveTimer > movePeriod){
    if (xVal >= minValueUpRight && xVal <= maxValueUpRight){
      nextPosition = 0;
    }
    else
    if (xVal <= maxValueDownLeft && xVal >= minValueDownLeft){
      nextPosition = 1;
    }
    else
    if (yVal <= maxValueDownLeft && yVal >= minValueDownLeft){
      nextPosition = 2;
    }
    else
    if (yVal >= minValueUpRight && yVal <= maxValueUpRight){
      nextPosition = 3;
    }

   if(segMatrix[currentSegIndex][nextPosition] != -1 && nextPosition != -1){ // if the value of nextPosition is different from -1 in the SegMatrix at line currentSefIndex and at 
   // nextPosition the blinking led moves
    moveDone = true;

    moveTimer = millis();
    
    currentSeg = segMatrix[currentSegIndex][nextPosition]; // currentSeg takes the new segs' pin

    for (int i = 0; i <= noSegs - 1; i++){
      if (currentSeg == segsPins[i]){ //a search for the index of the current segment
          currentSegIndex = i;
      }
    
    nextPosition = -1;
    }
   }
  }
}
void loop() {
  readAxis();

  for (int i = 0; i <= noSegs - 1; i++) {
    if (highSegs[i] == 1 && segsPins[i] != currentSeg){ 
      digitalWrite(segsPins[i], HIGH);
    }else{
      if (segsPins[i] != currentSeg){
        digitalWrite(segsPins[i], LOW);
      }
    }
   }

  if (millis() - blinkTime >= blinkPeriod){
    blinkTime = millis();
    blinkState = !blinkState;
  }
  if (possiblePress) {
// Start the debounce counter if needed
  if (shouldStartCounter) {
      lastDebounceTime = millis();
      shouldStartCounter = false;
  }
// If the debounce period has elapsed
  if ((millis() - lastDebounceTime) > debounceDelay) {
// Read the current state of the button
    buttonReading = digitalRead(SWPin);
    if (longPressState == false){
      longPressTime = millis();
      longPressState = true;
    }
    
  if (buttonReading == 1){
    if ( millis() - longPressTime >= longPressPeriod && longPressState == true){      
      
        for (int i = 0; i <= noSegs - 1; i++) { // if a long press happened all the led that were on are gonna be turned off and the current position will be dp
          highSegs[i] = 0;
        }

        currentSeg = segsPins[7];
        currentSegIndex = noSegs - 1;

        longPressState = false;
    }
  else{
        highSegs[currentSegIndex] = !highSegs[currentSegIndex];

        longPressState = false;
        }
      }
  
  possiblePress = false;
    }
  }
  
  digitalWrite(currentSeg, blinkState);
}
void handleInterrupt(){
  possiblePress = true;
  shouldStartCounter = true;
}