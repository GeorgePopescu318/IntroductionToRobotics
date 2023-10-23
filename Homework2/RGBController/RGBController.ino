//Min and Max values for analog input and output
int minIn = 0;
int maxIn = 1023;
int minOut = 0;
int maxOut = 255;

// The value that the red input reads
int RedInValue = 0;
// The value that the green input reads
int GreenInValue = 0;
// The value that the blue input reads
int BlueInValue = 0;

// The pins from the Arduino that are connected to each input
int RedInPin = A0;
int GreenInPin = A1;
int BlueInPin = A2; 

// The value for the red out 
int RedOutValue = 0;
// The value for the green out 
int GreenOutValue = 0;
// The value for the blue out 
int BlueOutValue = 0;

// The pin assigned to each colour of the led 
int RedOutPin = 9;
int GreenOutPin = 10;
int BlueOutPin = 11;

//Max noise value
int MaxNoise = 20;

//Check if the RGB LED has common cathode or common anode and modify this value, 0 for common cathode and 1 for common anode
bool CommonPin = false;
void setup() {
  // If commonPin is true the min and max from output swap
  if (CommonPin == true){
    int SwapVar = minOut;
    minOut = maxOut;
    maxOut = SwapVar;
  }
  // The initialisation of all the input and output pins
  pinMode(RedInPin,INPUT);
  pinMode(GreenInPin,INPUT);
  pinMode(BlueInPin,INPUT);

  pinMode(RedOutPin,OUTPUT);
  pinMode(GreenOutPin,OUTPUT);
  pinMode(BlueOutPin,OUTPUT);
}

void loop() {

  // The reading of the values from the input
  RedInValue = analogRead(RedInPin);
  if ( RedInValue <= MaxNoise)
    RedInValue = 0;
  GreenInValue = analogRead(GreenInPin);
   if ( GreenInValue <= MaxNoise)
    RedInValue = 0;
  BlueInValue = analogRead(BlueInPin);
   if ( GreenInValue <= MaxNoise)
    RedInValue = 0;

  // The value from the input is mapped from 0 to 1023 to 0 to 255 via the map function, this is done because analog input accepts values from 0 to 1023 and analog write from 0 to 255, we do this for each colour
  RedOutValue = map(RedInValue, minIn, maxIn, minOut, maxOut);
  GreenOutValue = map(GreenInValue, minIn, maxIn, minOut, maxOut);
  BlueOutValue = map(BlueInValue, minIn, maxIn, minOut, maxOut);
  // Here the output is being send throught the respective port
  analogWrite(RedOutPin, RedOutValue);
  analogWrite(GreenOutPin, GreenOutValue);
  analogWrite(BlueOutPin, BlueOutValue);

}
