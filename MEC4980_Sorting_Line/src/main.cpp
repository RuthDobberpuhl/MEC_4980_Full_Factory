#include <Arduino.h>
#include <P1AM.h>

enum MachineStates{
  Waiting,
  ColorSensing,
  CountedMove,
  Eject
};

/*
enum ReadyPucks {
  None,
  White,
  Red, 
  Blue
};*/

MachineStates curState = Waiting;
//ReadyPucks ready = None;

//Moduels
int modInput = 1;
int modOutput = 2;
int modAnalog = 3;
int robInput = 1;

//Inputs
int pulse = 1;
int lbIn = 2;
int lbOut = 3;
int lbWhite = 4;
int lbRed = 5;
int lbBlue = 6;

//Outputs
int conv = 1;
int compressor = 2;
int ejectW = 3;
int ejectR = 4;
int ejectB = 5;
int robWhite = 6;
int robRed = 7;
int robBlue = 8;

//Analog inputs
int color = 1;

//Vars
int colorValue = 10000;
int distToEject = 0;
bool prevKeyState = false;
int distMoved = 0;
bool curKey = 0;
char targetColor = 'b';
// B= 5060 R = 4216 W = 1849


void setup(){
  delay(1000);
  Serial.begin(9600);
  delay(1000);


  // Start up P1am modules
  while(!P1.init()){
    delay(1);
  }

}

bool InputTriggered(){
  return !P1.readDiscrete(modInput,lbIn);
}

bool OutputTriggered(){
  return !P1.readDiscrete(modInput,lbOut);
}

void ToggleConveyor(bool s){
  P1.writeDiscrete(s, modOutput, conv);
}

int GetColor(){
  return P1.readAnalog(modAnalog,color);
}

bool GetPulseKey(){
  return P1.readDiscrete(modInput, pulse);
}

void ToggleCompressor(bool s){
  P1.writeDiscrete(s, modOutput, compressor);
}

void UseEjector(char c){
  int tempPin;

  if (c == 'w'){
    tempPin = ejectW;
  } else if (c == 'r'){
    tempPin = ejectR;
  } else {
    tempPin = ejectB;
  }
  P1.writeDiscrete(true, modOutput,tempPin);
  delay(1500);
  P1.writeDiscrete(false,modOutput,tempPin);
}

/*
void ColorForRob(char targetColor){
  if (targetColor = 'w'){

  } else if (targetColor = 'r'){

  } else{

  }
}*/

/*
bool RobInput(modInput, robInput){
  return !P1.readDiscrete(modInput,robInput);
}*/

void loop(){
  
  /*bool isOn = P1.readDiscrete(1,2);  // 1st moduel 2nd pin
  Serial.println(isOn); // 1 true, 0 false
  delay(100);*/
  bool whitePuck = !P1.readDiscrete(modInput,lbWhite);
  bool redPuck = !P1.readDiscrete(modInput,lbRed);
  bool bluePuck = !P1.readDiscrete(modInput,lbBlue);

  if (whitePuck == true) {
    P1.writeDiscrete(true,modOutput,robWhite);
    Serial.println("White Avaliable");
  } else if(whitePuck == false){
    P1.writeDiscrete(false,modOutput,robWhite);
  }
  if (redPuck == true){
    P1.writeDiscrete(true,modOutput,robRed);
    Serial.println("Red Avaliable");
  } else if(redPuck == false){
    P1.writeDiscrete(false,modOutput,robRed);
  }
  if (bluePuck == true){
    P1.writeDiscrete(true,modOutput,robBlue);
    Serial.println("Blue Avaliable");
  }else if(bluePuck == false){
    P1.writeDiscrete(false,modOutput,robBlue);
  }


  switch (curState){
    case Waiting:
      // Wait for light barrier to be tripped
      //After tripped, switch state and turn on conveyor
      if(InputTriggered()){
        curState = ColorSensing;
        ToggleConveyor(true);
        colorValue = 10000;
      }
      break;
    case ColorSensing:
      //Get color and find min
      
      colorValue = min(GetColor(), colorValue);
      // Keep going until second light barrier 
      //Switch States
      if (OutputTriggered()){
        curState = CountedMove;
        distMoved = 0;
        //Decide how far to move
        if (colorValue <2500){
          distToEject = 3;
          targetColor = 'w';
        }else if (colorValue < 4600){
          distToEject = 9;
          targetColor = 'r';
        } else{
          distToEject = 14;
          targetColor ='b';
        }
        ToggleCompressor(true);
      }
      
      break;

    case CountedMove:
      //Watch pulse key to move that far
      curKey = GetPulseKey();
      if (curKey && !prevKeyState){
        distMoved++;
      }
      prevKeyState = curKey;
      //Switch states and turn off conveyor
      if(distMoved >= distToEject){
        curState = Eject;
        ToggleConveyor(false);
      }
      break;

    case Eject:
      UseEjector(targetColor);
      curState = Waiting;

      break;

    default:
      break;
  }
}