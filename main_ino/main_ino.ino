/*******Interrupt-based Rotary Encoder Sketch*******

adapted from a sketch by Simon Merrett, based on insight from Oleg Mazurov, Nick Gammon, rt, Steve Spence

*/

static int pinA = 2; // Our first hardware interrupt pin is digital pin 2
static int pinB = 3; // Our second hardware interrupt pin is digital pin 3

volatile byte aFlag = 0; // let's us know when we're expecting a rising edge on pinA to signal that the encoder has arrived at a detent
volatile byte bFlag = 0; // let's us know when we're expecting a rising edge on pinB to signal that the encoder has arrived at a detent (opposite direction to when aFlag is set)
volatile byte encoderPos = 0; //this variable stores our current value of encoder position. Change to int or uin16_t instead of byte if you want to record a larger range than 0-255
volatile byte oldEncPos = 0; //stores the last encoder position value so we can compare to the current reading and see if it has changejjjjjjjjd (so we know when to print to the serial monitor)
volatile byte reading = 0; //somewhere to store the direct values we read from our interrupt pins before checking to see if we have moved a whole detent
volatile byte headPos = 8;

int state = 0;
int random_hide = random(1000, 10000);
int random_show = random(1000, 10000);
int choice = random(0, 2);

static int motorPinLeft = 5;
static int motorPinRight = 6;

void setup() {
  pinMode(pinA, INPUT_PULLUP); // set pinA as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  pinMode(pinB, INPUT_PULLUP); // set pinB as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  pinMode(motorPinLeft, OUTPUT);
  pinMode(motorPinRight, OUTPUT);
  attachInterrupt(0,PinA,RISING); // set an interrupt on PinA, looking for a rising edge signal and executing the "PinA" Interrupt Service Routine (below)
  attachInterrupt(1,PinB,RISING); // set an interrupt on PinB, looking for a rising edge signal and executing the "PinB" Interrupt Service Routine (below)
  Serial.begin(9600); // start the serial monitor link
}

void PinA(){
  cli(); //stop interrupts happening before we read pin values
  reading = PIND & 0xC; // read all eight pin values then strip away all but pinA and pinB's values
  if(reading == B00001100 && aFlag) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    encoderPos--; //decrement the encoder's position count
    headPos=headPos-1;
    bFlag = 0; //reset flags for the next turn
    aFlag = 0; //reset flags for the next turn
  }else if (reading == B00000100) bFlag = 1; //signal that we're expecting pinB to signal the transition to detent from free rotation
  sei(); //restart interrupts
}


void PinB(){
  cli(); //stop interrupts happening before we read pin values
  reading = PIND & 0xC; //read all eight pin values then strip away all but pinA and pinB's values
  if (reading == B00001100 && bFlag) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    encoderPos++; //increment the encoder's position count
    headPos=headPos+1;
    bFlag = 0; //reset flags for the next turn
    aFlag = 0; //reset flags for the next turn
  }else if (reading == B00001000) aFlag = 1; //signal that we're expecting pinA to signal the transition to detent from free rotation
  sei(); //restart interrupts
}


void loop(){
  //if(oldEncPos != encoderPos) {
  Serial.print("Encoder at: ");
  Serial.print(headPos); 
  Serial.print(", State: "); 
  Serial.println(state);
    //oldEncPos = encoderPos;
  //}

  // State 0 - Hide behind Tombstone
  if(state == 0){
    analogWrite(motorPinLeft, 0);
    analogWrite(motorPinRight, 0);
    //if(millis() - previousTime){
    Serial.println(choice);
    delay(random_hide);
    random_hide = random(2000, 10000);
    if(choice == 0){
      state = 1;
    }else{
      state = 4;
    }
    choice = random(0, 2);
  } 
  
  // State 1 - Pop out
  else if(state == 1){
    if(headPos < 13){
      analogWrite(motorPinLeft, 255);
      analogWrite(motorPinRight, 0);
    }else{
      analogWrite(motorPinLeft, 0);
      analogWrite(motorPinRight, 0);
      state++;
    }
  }
  // State 2 - Show Goblin Head for X seconds
  else if(state == 2){
    analogWrite(motorPinLeft, 0);
    analogWrite(motorPinRight, 0);
    delay(random_show);
    random_show = random(1000, 4000);
    state++;
  }
  // State 3 - Pop back in
  else if(state == 3){
    analogWrite(motorPinLeft, 0);
    analogWrite(motorPinRight, 255); 
    if(headPos == 8){
      analogWrite(motorPinLeft, 0);
      analogWrite(motorPinRight, 0);
      //headPos = 0;
      state=0;
    }
  }
  
  if(state == 4){
    analogWrite(motorPinLeft, 0);
    analogWrite(motorPinRight, 0);
    //if(millis() - previousTime){
    delay(random_hide);
    random_hide = random(2000, 10000);
    state++;
  }
  
  // State 1 - Pop out
  if(state == 5){
    if(headPos > 3){
      analogWrite(motorPinLeft, 0);
      analogWrite(motorPinRight, 255);
    }else{
      analogWrite(motorPinLeft, 0);
      analogWrite(motorPinRight, 0);
      state++;
    }
  }
  // State 2 - Show Goblin Head for X seconds
  else if(state == 6){
    analogWrite(motorPinLeft, 0);
    analogWrite(motorPinRight, 0);
    delay(random_show);
    random_show = random(200, 4000);
    state++;
  }
  // State 3 - Pop back in
  else if(state == 7){
    analogWrite(motorPinLeft, 255);
    analogWrite(motorPinRight, 0);
    if(headPos == 8){
      analogWrite(motorPinLeft, 0);
      analogWrite(motorPinRight, 0);
      state = 0;
    }
  }
}