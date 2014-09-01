#include <Servo.h> 
#include <Wire.h>
#include <Ultrasonic.h>

#define ULTRASONIC_1_TRIGGER_PIN  12
#define ULTRASONIC_1_ECHO_PIN     13

#define ULTRASONIC_2_TRIGGER_PIN  10
#define ULTRASONIC_2_ECHO_PIN     11

#define ULTRASONIC_3_TRIGGER_PIN  8
#define ULTRASONIC_3_ECHO_PIN     9

#define SLAVE_ADDRESS 0x04

#define FF 1
#define BB 2
#define FR 3
#define FL 4
#define BR 5
#define BL 6
#define LL 7
#define RR 8
#define STOP 9   //停止不動


Ultrasonic ultrasonic_1(ULTRASONIC_1_TRIGGER_PIN, ULTRASONIC_1_ECHO_PIN);
Ultrasonic ultrasonic_2(ULTRASONIC_2_TRIGGER_PIN, ULTRASONIC_2_ECHO_PIN);
Ultrasonic ultrasonic_3(ULTRASONIC_3_TRIGGER_PIN, ULTRASONIC_3_ECHO_PIN);

int number = 0;
char motorCmd[2];
int state = 0;
int lightSensorPin = 2;
int lightValue = 0;
Servo myServoLeft;  // create servo object to control a servo 
Servo myServoRight;
                // a maximum of eight servo objects can be created 

 
int pos = 0;    // variable to store the servo position 



void setup() 
{ 
  // attaches the servo on pin 9 to the servo object 
  myServoLeft.attach(2);
  myServoRight.attach(3);  
  
    Serial.begin(9600);         // start serial for output
    // initialize i2c as slave
    Wire.begin(SLAVE_ADDRESS);

    // define callbacks for i2c communication
    Wire.onReceive(receiveData);
    Wire.onRequest(sendData);
} 
 
int currDirection = FF;
int motorSpeed = 3; //1 fast  2 mid  3 slow  
int contCollision = 0;
void loop() 
{  
    RobotMove(0);
    if((obstacleDistance(1) < 10) || obstacleDistance(2) < 10  || obstacleDistance(3) < 10){   //　少加了(contCollision % 2 == 0 ) 為什麼會左左
        currDirection = BB;
        RobotMove(2000);
        currDirection = LL;
        contCollision++;            
        delay(10);
    }else if((obstacleDistance(1) < 10) || obstacleDistance(2) < 10  || obstacleDistance(3) < 10
                                      && (contCollision % 2 == 1 ) && (contCollision != 0)){
        currDirection = BB;
        RobotMove(2000);
        currDirection = RR;
        delay(10);        
    }
//   myServoLeft.write(180);   // cw
//   myServoRight.write(1);   // ccw
// 
//   if(obstacleDistance(1) < 10){
//   
//   }
// 
//  for(pos = 0; pos < 180; pos += 1)  // goes from 0 degrees to 180 degrees 
//  {                                  // in steps of 1 degree 
//    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
//    delay(15);                       // waits 15ms for the servo to reach the position 
//  } 
//  for(pos = 180; pos>=1; pos-=1)     // goes from 180 degrees to 0 degrees 
//  {                                
//    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
//    delay(15);                       // waits 15ms for the servo to reach the position 
//  } 
} 

void RobotMove(int delayTime){
  int myServoLeftPos = 180;
  int myServoRightPos = 180;

  switch(currDirection){
      case FF:
        if(motorSpeed == 1){
          myServoLeft.write(180);      
          myServoRight.write(5);      
        }else if(motorSpeed == 2){
          
        }else if(motorSpeed == 3){
          myServoLeft.write(100);      
          myServoRight.write(82);        
        }              
      break;
      case BB:
        if(motorSpeed == 1){
          myServoLeft.write(1);      
          myServoRight.write(180);      
        }else if(motorSpeed == 2){

        }else if(motorSpeed == 3){
          myServoLeft.write(80);      
          myServoRight.write(100);        
        }                    
        break;
      case RR:              

        if(motorSpeed == 1){
          myServoLeft.write(180);      
          myServoRight.write(90);         
          delay(1900);   
        }else if(motorSpeed == 2){


        }else if(motorSpeed == 3){
          myServoLeft.write(100);      
          myServoRight.write(90);        
          delay(3400);             
        } 
        currDirection = FF;
        
      break;
      case LL:
        if(motorSpeed == 1){
          myServoLeft.write(91);      
          myServoRight.write(1);         
          delay(1900);   
        }else if(motorSpeed == 2){


        }else if(motorSpeed == 3){
          myServoLeft.write(91);      
          myServoRight.write(80);        
          delay(3400);             
        } 
        currDirection = FF;      
      break;
      case BR:
      break;
      case FR:
      break;  
      case FL:
      break;  
      case STOP:
        myServoLeft.write(91);      
        myServoRight.write(90);                    
      break; 
  }      

  delay(delayTime);
}

void CustomizeDiffSpeedChangeDirectionDelaySecond(){

}
float obstacleDistance(int ultrasonicNum){

  float cmMsec, inMsec;
  long microsec;
  switch(ultrasonicNum){
      case 1:
        microsec = ultrasonic_1.timing();
        cmMsec = ultrasonic_1.convert(microsec, Ultrasonic::CM);      
      break;
      case 2:
        microsec = ultrasonic_2.timing();
        cmMsec = ultrasonic_2.convert(microsec, Ultrasonic::CM);      
      break;
      case 3:
        microsec = ultrasonic_3.timing();
        cmMsec = ultrasonic_3.convert(microsec, Ultrasonic::CM);      
      break;
      
  }
  Serial.println(cmMsec);
  return cmMsec;
}
// callback for received data
void receiveData(int byteCount){

    while(Wire.available()) {
        number = Wire.read();    // recv from Pi  (i2c
        Serial.print("data received: ");
        Serial.println(number);
     }
}

// callback for sending data
void sendData(){
  lightValue = analogRead(lightSensorPin);  
  Serial.println(lightValue,DEC);
  Wire.write(lightValue);   //send to Pi  (i2c
}
