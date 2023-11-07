int minIn = 0;
int maxIn = 1023;
int minOut = 0;
int maxOut = 255;

int redInValue = 0;
int GreenInValue = 0;
int blueInValue = 0;

const int redInPin = A0;
const int greenInPin = A1;
const int blueInPin = A2; 


int redOutValue = 0;
int greenOutValue = 0;
int blueOutValue = 0;

const int redOutPin = 9;
const int greenOutPin = 10;
const int blueOutPin = 11;

int maxNoise = 20;

//Check if the RGB LED has common cathode or common anode and modify this value, 0 for common cathode and 1 for common anode
bool CommonPin = false;
void setup() {
  // If commonPin is true the min and max from output swap
  if ( CommonPin == true ){
    int SwapVar = minOut;
    minOut = maxOut;
    maxOut = SwapVar;
  }

  pinMode(redOutPin,OUTPUT);
  pinMode(greenOutPin,OUTPUT);
  pinMode(blueOutPin,OUTPUT);
}

void loop() {

  // The reading of the values from the input
  redInValue = analogRead(redInPin);
  if ( redInValue <= maxNoise )
    redInValue = minOut;
  GreenInValue = analogRead(greenInPin);
   if ( GreenInValue <= maxNoise )
    GreenInValue = minOut;
  blueInValue = analogRead(blueInPin);
   if ( GreenInValue <= maxNoise )
    GreenInValue = minOut;

  // The value from the input is mapped from 0 to 1023 to 0 to 255 via the map function, this is done because
  // analog input accepts values from 0 to 1023 and analog write from 0 to 255, we do this for each colour
  redOutValue = map(redInValue, minIn, maxIn, minOut, maxOut);
  greenOutValue = map(GreenInValue, minIn, maxIn, minOut, maxOut);
  blueOutValue = map(blueInValue, minIn, maxIn, minOut, maxOut);

  // Here the output is being send throught the respective port
  analogWrite(redOutPin, redOutValue);
  analogWrite(greenOutPin, greenOutValue);
  analogWrite(blueOutPin, blueOutValue);

}
