#include <Arduino.h>
#include <P1AM.h>
#include <Encoder.h>

int modInput = 1;
int modOutput = 2;
int modAnalog = 3;

//Multi Processing Pins
int turntableSuc = 1;
int turntableConv = 2;
int endConv = 3;
int turntableSaw = 4;
int turntablePos = 5;
int ovenFeedIN = 6;
int ovenFeedOUT = 7;
int kilnPos = 8;
int lbKiln = 9;
int mttCw = 1;
int mttCcw = 2;
int convForward = 3;
int mSaw = 4;
int ovenFeedRectract = 5;
int ovenFeedExtend = 6;
int vaccOven = 7;
int vaccMotor = 8;
int lampKiln = 9;
int compressor = 10;
int vacc = 11;
int lower = 12;
int kilnDoor = 13;
int convFeed = 14;


//MotorEncoder myFirstMotor(modInput, modOutput, 4,3,7,2);
//MotorEncoder tiltMotor(modInput, modOutput, 1,2,5,1);

void setup(){
  delay(1000);
  Serial.begin(9600);
  delay(1000);

  while(!P1.init()){
    Serial.println("Waiting for connection...");
  }
  Serial.println("Successfully connected");
  //myFirstMotor.Home();
  //tiltMotor.Home();
}

void ToggleCompressor(bool s){
  P1.writeDiscrete(s, modOutput, compressor);
}

//Multi Processing Station

void loop(){
  //place workpice through kiln lb
  
  while (P1.readDiscrete(modInput, lbKiln)){
    //wait for it to have piece
  }

  ToggleCompressor(true);
  Serial.println("object dectected, ready for kiln");

  //open kiln door
  P1.writeDiscrete(true, modOutput,kilnDoor);
  Serial.println("opening door");
  delay(1000); // wait for door to open

  // move into kiln
  while (!P1.readDiscrete(modInput,ovenFeedIN)){
    P1.writeDiscrete(true, modOutput,ovenFeedRectract);

  }

  P1.writeDiscrete(false, modOutput,ovenFeedRectract);// turn off retract

  // close kiln door
  P1.writeDiscrete(false,modOutput,kilnDoor);
  Serial.println("closing door");

  // turn on heating lamp
  P1.writeDiscrete(true, modOutput, lampKiln);

  //keep under heating lamp 3 sec
  delay(3000);

  //turn off lamp
  P1.writeDiscrete(false, modOutput, lampKiln);

  //open kiln door
  P1.writeDiscrete(true,modOutput,kilnDoor);
  Serial.println("opening door");

  //move out
  while (!P1.readDiscrete(modInput,ovenFeedOUT)){
    P1.writeDiscrete(true, modOutput,ovenFeedExtend);
  }

  P1.writeDiscrete(false, modOutput,ovenFeedExtend);// turn off extend

  //close kiln door
  P1.writeDiscrete(false,modOutput,kilnDoor);
  Serial.println("closing door");

  //move gripper over
  while (!P1.readDiscrete(modInput,kilnPos)){
    P1.writeDiscrete(true, modOutput,vaccOven);
  }
  P1.writeDiscrete(false, modOutput,vaccOven);// turn off extend

  //suction down
  P1.writeDiscrete(true, modOutput,lower);
  delay(1000);
  P1.writeDiscrete(true, modOutput,vacc);
  delay(1000);
  P1.writeDiscrete(false, modOutput,lower);

  // move gripper back
  while (!P1.readDiscrete(modInput,turntablePos)){
    P1.writeDiscrete(true, modOutput,vaccMotor);
  }
  P1.writeDiscrete(false, modOutput,vaccMotor);// turn off extend

  //turn table to gripper
   while (!P1.readDiscrete(modInput,turntableSuc)){
    P1.writeDiscrete(true, modOutput,mttCcw);
  }
  P1.writeDiscrete(false, modOutput,mttCcw);

  //move suction down, turn off
  P1.writeDiscrete(true, modOutput,lower);
  delay(1000);
  P1.writeDiscrete(false, modOutput,vacc);
  delay(1000);
  P1.writeDiscrete(false, modOutput,lower);

  // move to saw
   while (!P1.readDiscrete(modInput,turntableSaw)){
    P1.writeDiscrete(true, modOutput,mttCw);
  }
  P1.writeDiscrete(false, modOutput,mttCw);
  //saw for 5 seconds
  P1.writeDiscrete(true, modOutput,mSaw);
  delay(5000);
  P1.writeDiscrete(false, modOutput,mSaw);

  // turn to conveyor
  while (!P1.readDiscrete(modInput,turntableConv)){
    P1.writeDiscrete(true, modOutput,mttCw);
  }
  P1.writeDiscrete(false, modOutput,mttCw);
  delay(500);
// eject to conveyor
  P1.writeDiscrete(true, modOutput,convFeed);
  delay(500);
  P1.writeDiscrete(false, modOutput,convFeed);

  //turn on conveyor
  while (P1.readDiscrete(modInput,endConv)){
  P1.writeDiscrete(true, modOutput,convForward);
  }
  delay(2000);
  P1.writeDiscrete(false, modOutput,convForward);

  ToggleCompressor(false);
}

