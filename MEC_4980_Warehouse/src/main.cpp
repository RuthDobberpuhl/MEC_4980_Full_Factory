#include <Arduino.h>
#include <P1AM.h>
#include <Encoder.h>

int modInput = 1;
int modOutput = 2;
int modAnalog = 3;

//Inputs
int lbout = 2;
int lbin = 3;

int lsVert = 4;
int lsHorz = 1;
int lsCantFront = 9;
int lsCantBack = 10;

int pulseVert = 7;
int pulseHorz = 5;
int pulseCant = 9;

//Outputs
int convMotorForw = 1;
int convMotorBack = 2;

int vertMotorUp = 6;
int vertMotorDown = 5;

int horzMotorBack = 4;
int horzMotorForw = 3;

int cantMotorForw = 7;
int cantMotorBack = 8;

int robReady = 11;
int readyForRob = 13;

//Warehouse
int Spot1 = 0; // 0 = empty
int Spot2 = 0; // 0 = empty
int Spot3 = 0; // 0 = empty
int Spot4 = 0; // 0 = empty
int Spot5 = 0; // 0 = empty
int Spot6 = 0; // 0 = empty
int Spot7 = 0; // 0 = empty
int Spot8 = 0; // 0 = empty
int Spot9 = 0; // 0 = empty

int vert1 = 25;
int vert2 = 215;
int vert3 = 410;

int horz1 = 965;
int horz2 = 670;
int horz3 = 380;

int vertPos = 0;
int horzPos = 0;

int convPosHorz = 15;
int convPosVert = 300;

int posOutOfConv =convPosVert+50;
int posPickOffConv = convPosVert-100;

MotorEncoder VertMotor(modInput, modOutput, vertMotorDown, vertMotorUp, pulseVert, lsVert);
MotorEncoder HorzMotor(modInput, modOutput, horzMotorForw, horzMotorBack, pulseHorz, lsHorz);
//MotorEncoder CantMotor(modInput, modOutput, cantMotorBack, cantMotorForw, pulseCant , lsCant);

enum MachineStates{
  FindOpenSpot,
  GrabTray,
  PickupPuck,
  PutTrayBack,
  MoveHome
};

MachineStates curState = MoveHome;

void MoveCantForw(){
  while(!P1.readDiscrete(modInput,lsCantFront)){
    P1.writeDiscrete(true,modOutput,cantMotorForw);}
  P1.writeDiscrete(false,modOutput,cantMotorForw);
}

void MoveCantBack(){
  while(!P1.readDiscrete(modInput,lsCantBack)){
    P1.writeDiscrete(true,modOutput,cantMotorBack);}
  P1.writeDiscrete(false,modOutput,cantMotorBack);
}

void MoveConvForw(){
  while(P1.readDiscrete(modInput,lbin)){
    P1.writeDiscrete(true,modOutput,convMotorForw);}
  P1.writeDiscrete(false,modOutput,convMotorForw);
  Serial.println("tray reached front");
}

void MoveConvBack(){
  while(P1.readDiscrete(modInput,lbout)){
    P1.writeDiscrete(true,modOutput,convMotorBack);}
  P1.writeDiscrete(false,modOutput,convMotorBack);
  Serial.println("tray reached back");
}

void setup(){
  delay(1000);
  Serial.begin(9600);
  delay(1000);

  while(!P1.init()){
    Serial.println("Waiting for connection...");
  }
  Serial.println("Successfully connected");
  MoveCantBack();
  HorzMotor.Home();
  VertMotor.Home();
  Serial.println("Homed");
}


void loop(){

  switch (curState){
    case FindOpenSpot:
      if (Spot1 == 0){
        Serial.println("using spot 1");
        Spot1 = 1;
        vertPos = vert1;
        horzPos = horz1;
      }
      else if(Spot2 == 0){
        Spot2 = 1;
        vertPos = vert2;
        horzPos = horz1;
      }
      else if(Spot3 == 0){
        Spot3 = 1;
        vertPos = vert3;
        horzPos = horz1;
      }
      else if(Spot4 == 0){
        Spot4 = 1;
        vertPos = vert1;
        horzPos = horz2;
      }
      else if(Spot5 == 0){
        Spot5 = 1;
        vertPos = vert2;
        horzPos = horz2;
      }
      else if(Spot6 == 0){
        Spot6 = 1;
        vertPos = vert3;
        horzPos = horz2;
      }
      else if(Spot7 == 0){
        Spot7 = 1;
        vertPos = vert1;
        horzPos = horz3;
      }
      else if(Spot8 == 0){
        Spot8 = 1;
        vertPos = vert2;
        horzPos = horz3;
      }
      else if(Spot9 == 0){
        Spot9 = 1;
        vertPos = vert3;
        horzPos = horz3;
      }
      else {
        Spot1 = 0; // 0 = empty
        Spot2 = 0; // 0 = empty
        Spot3 = 0; // 0 = empty
        Spot4 = 0; // 0 = empty
        Spot5 = 0; // 0 = empty
        Spot6 = 0; // 0 = empty
        Spot7 = 0; // 0 = empty
        Spot8 = 0; // 0 = empty
        Spot9 = 0; // 0 = empty
        break;
      }
      curState = GrabTray;
      break;

    case GrabTray:
      Serial.println("grabbing tray");
      while(!HorzMotor.MoveTo(horzPos)){;}
      while(!VertMotor.MoveTo(vertPos)){;}
      MoveCantForw();
      while(!VertMotor.MoveTo(vertPos-20)){;}
      delay(500);
      MoveCantBack();
      Serial.println("have tray, moving to conveyor");
      while(!HorzMotor.MoveTo(convPosHorz)){;}
      while(!VertMotor.MoveTo(convPosVert)){;}
      Serial.println("at conveyor");
      curState = PickupPuck;
      break;

    case PickupPuck:
      Serial.println("placing on converyor");
      MoveCantForw();
      while(!VertMotor.MoveTo(posOutOfConv)){;}
      Serial.println("moving tray forward");
      MoveConvForw();
      
      Serial.println("waiting for pickup station");  
      //
      P1.writeDiscrete(true,modOutput,readyForRob);
      while (!P1.readDiscrete(modInput,robReady)){;}
      delay(3000);
      Serial.println("have puck, moving back");
      P1.writeDiscrete(false,modOutput,readyForRob);
      MoveConvBack();
      curState = PutTrayBack;
      break;

    case PutTrayBack:
      VertMotor.Home();
      HorzMotor.Home();
      MoveCantBack();
      //while(!VertMotor.MoveTo(0)){;}
      while(!HorzMotor.MoveTo(horzPos)){;}
      while(!VertMotor.MoveTo(vertPos-5)){;}
      MoveCantForw();
      while(!VertMotor.MoveTo(vertPos+20)){;}
      MoveCantBack();
      curState = FindOpenSpot;
      break;

    case MoveHome:
      MoveCantBack();
      HorzMotor.Home();
      VertMotor.Home();
      Serial.println("homed");
      curState = FindOpenSpot;

      break;
  }


}