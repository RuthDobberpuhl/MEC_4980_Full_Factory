#include <Arduino.h>
#include <P1AM.h>
#include <Encoder.h>

int modInput = 1;
int modOutput = 2;
int modAnalog = 3;

//Inputs
int lsVert = 1;
int lsHorz = 2;
int lsTurn = 3;

int pulseVert = 5;
int pulseHorz = 7;
int pulseTurn = 9;

int ColorWaitWhite = 11;
int ColorWaitBlue = 13;
int ColorWaitRed = 12;

int warehouseReady = 14;

//Outputs
int vertMotorUp = 1;
int vertMotorDown = 2;

int horzMotorBack = 3;
int horzMotorForw = 4;

int turnMotorCw = 5;
int turnMotorCcw = 6;

int compressor = 7;
int valveVac = 8;
int warehouseSig = 9;

MotorEncoder VertMotor(modInput, modOutput, vertMotorDown, vertMotorUp, pulseVert, lsVert);
MotorEncoder HorzMotor(modInput, modOutput, horzMotorForw, horzMotorBack, pulseHorz, lsHorz);
MotorEncoder TurnMotor(modInput, modOutput,  turnMotorCcw, turnMotorCw,pulseTurn , lsTurn);

void setup(){
  delay(1000);
  Serial.begin(9600);
  delay(1000);

  while(!P1.init()){
    Serial.println("Waiting for connection...");
  }
  Serial.println("Successfully connected");
  VertMotor.Home();
  HorzMotor.Home();
  TurnMotor.Home();
  Serial.println("Homed");
}


enum MachineStates{
  SenseColor,
  WhiteReady,
  BlueReady,
  RedReady,
  MoveToDropOff,
  WaitWarehouse,
  Dropoff,
  MoveHome
};

MachineStates curState = MoveHome;

void ToggleCompressor(bool s){
  P1.writeDiscrete(s, modOutput, compressor);
}

void PickUp(bool s){
  P1.writeDiscrete(s,modOutput,valveVac);
  delay(1000);
}

void StopMoving(bool s){
  VertMotor.Stop();
  HorzMotor.Stop();
  TurnMotor.Stop();
}

void loop(){

switch (curState){

    case SenseColor:

      if (P1.readDiscrete(modInput, ColorWaitWhite) == true){
        curState = WhiteReady;
        Serial.println("white ready");
      }

      else if (P1.readDiscrete(modInput, ColorWaitBlue) == true){
        curState = BlueReady;
        Serial.println("blue ready");
      }

      else if (P1.readDiscrete(modInput, ColorWaitRed) == true){
        curState = RedReady;
        Serial.println("red ready");
      }
      else {
        break;
      }
      break;

    case WhiteReady:
      Serial.println("picking up white");

      ToggleCompressor(true);
      while(!TurnMotor.MoveTo(220)){;}
      while(!HorzMotor.MoveTo(160)) {;}
      while(!VertMotor.MoveTo(400)){;}
      //StopMoving(true);

      Serial.println("Stopped");
      PickUp(true);
      curState = MoveToDropOff;
      while(!VertMotor.MoveTo(300)){;}
      break;

    case BlueReady:
      Serial.println("picking up blue");
      ToggleCompressor(true);
      while(!TurnMotor.MoveTo(150)){;}
      while(!HorzMotor.MoveTo(280)) {;}
      while(!VertMotor.MoveTo(400)){;}
      Serial.println("Stopped");
      PickUp(true);
      curState = MoveToDropOff;
      while(!VertMotor.MoveTo(300)){;}
      break;

    case RedReady:
      Serial.println("picking up red");
      ToggleCompressor(true);
      while(!TurnMotor.MoveTo(185)){;}
      while(!HorzMotor.MoveTo(190)) {;}
      while(!VertMotor.MoveTo(400)){;}
      Serial.println("Stopped");
      PickUp(true);
      curState = MoveToDropOff;
      while(!VertMotor.MoveTo(300)){;}
      break;

    case MoveToDropOff:
      while(!VertMotor.MoveTo(0)){;}
      while(!HorzMotor.MoveTo(0)) {;}
      while(!TurnMotor.MoveTo(670)){;}
      while(!HorzMotor.MoveTo(75)) {;}
      while(!VertMotor.MoveTo(30)){;}
      delay(1000);
      Serial.println("At drop off");
      curState = WaitWarehouse;
      break;

    case WaitWarehouse:
      while(!P1.readDiscrete(modInput,warehouseReady)){
        //delay(500);
      }

      Serial.println("warehouse ready");
      P1.writeDiscrete(true, modOutput, warehouseSig);
      curState = Dropoff;
      break;

    case Dropoff:
      PickUp(false);
      P1.writeDiscrete(false, modOutput, warehouseSig);
      Serial.println("dropped off");
      curState = MoveHome;
      break;

    case MoveHome:
      VertMotor.Home();
      HorzMotor.Home();
      TurnMotor.Home();
      ToggleCompressor(false);
      Serial.println("Homed");
      curState = SenseColor;
      break;

    default:
      break;
  }

}