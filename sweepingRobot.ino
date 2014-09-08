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
#define LSWING 9
#define RSWING 10
#define LBSWING 11
#define RBSWING 12
#define STOP 13   //停止不動


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


boolean isDelayFinish = false;

void setup() 
{ 
  	// attaches the servo on pin 9 to the servo object 
  	myServoLeft.attach(2);
  	myServoRight.attach(3);  
  
        //set initial value, otherwise will always back first time  
  	obstacleDistance(1);
  	obstacleDistance(2);
  	obstacleDistance(3);
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
int ultraSonicThreshold = 4;
void loop() 
{  
    RobotMove();
} 
long previousTime = 0;
boolean isLastSecondCollide = false;
void DetectAndHandleRobotCollideObstacle(){
	int t=0;
    if((obstacleDistance(1) < ultraSonicThreshold)
    || (obstacleDistance(2) < ultraSonicThreshold)  
    || (obstacleDistance(3) < ultraSonicThreshold)){   //　少加了(contCollision % 2 == 0 ) 為什麼會左左
        Serial.println("detected!!!");
        isLastSecondCollide = true;
        isDelayFinish = false;
//    	while(!isDelayFinish){
//    		isDelayFinish = DelayHanlder(2000,BB); 
//	}     	
		t=0;
		while(t<2000){
			DecompositionAction(BB);
			delay(1);			
			t++;
//			Serial.println(t);	
		}
        Serial.println("###move Back");                  
        isDelayFinish = false;    
//    	while(!isDelayFinish){
//    		isDelayFinish = DelayHanlder(3400,LL); 
//	} 	
		t=0;
		while(t<3400){
			DecompositionAction(LL);
			delay(1);			
			t++;
//			Serial.println(t);	
		}
        Serial.println("###move LEFT");                  
        currDirection = FF;
        contCollision++;            
        delay(10);
    }else if((obstacleDistance(1) < ultraSonicThreshold) 
    	|| (obstacleDistance(2) < ultraSonicThreshold)  
    	|| (obstacleDistance(3) < ultraSonicThreshold)
        && (contCollision % 2 == 1 ) && (contCollision != 0)){
		Serial.println("detected!!!");
		isLastSecondCollide = true;
        isDelayFinish = false;          
//    	while(!isDelayFinish){
//    		isDelayFinish = DelayHanlder(2000,BB); 
		t=0;
		while(t<2000){
			DecompositionAction(BB);
			delay(1);		
			t++;		
//			Serial.println(t);
		}
                Serial.println("###move Back");                  
			
//	}
        isDelayFinish = false;    	
//    	while(!isDelayFinish){
//    		isDelayFinish = DelayHanlder(3400,RR); 
//	} 	
		t=0;
		while(t<3400){
			DecompositionAction(RR);
			delay(1);			
			t++;
//			Serial.println(t);	
		}
        Serial.println("###move Right");                  
        currDirection = FF;  
        contCollision++;       
        delay(10);        
	}
	
}
boolean DelayHanlder(long interval,int DecompositionActionDirection){   
  	unsigned long currentTime = millis();
 
  	if(currentTime - previousTime > interval) {     // delay time is timeout
		DecompositionAction(DecompositionActionDirection);
	
		previousTime = currentTime;
		return true;		 
	}	
	
	//can do another event: 
	//we need to detect if the robot collide 
	//the obstacle during swing left and right
	DetectAndHandleRobotCollideObstacle();
	return false;		 
}
int myServoLeftPos = 91;
int myServoRightPos = 90;
int bonusSpeedCw = 10;
void DecompositionAction(int DecompositionActionDirection){
  switch(DecompositionActionDirection){
      case FF:
        if(motorSpeed == 1){
          myServoLeft.write(180);      
          myServoRight.write(5); 
          
        }else if(motorSpeed == 2){
          
        }else if(motorSpeed == 3){
          myServoLeft.write(100+bonusSpeedCw);      
          myServoRight.write(82-bonusSpeedCw);        
      
        }                 
      break;
      case BB:
        if(motorSpeed == 1){
          myServoLeft.write(1);      
          myServoRight.write(180); 
        }else if(motorSpeed == 2){

        }else if(motorSpeed == 3){
          myServoLeft.write(80-bonusSpeedCw);      
          myServoRight.write(100+bonusSpeedCw);       
        }                    
        break;
      case RR:              
        if(motorSpeed == 1){
          myServoLeft.write(180);      
          myServoRight.write(90);         
//          delay(1900);   
        }else if(motorSpeed == 2){

        }else if(motorSpeed == 3){
          myServoLeft.write(100);      
          myServoRight.write(90);        
//          delay(3400);          
        } 
//        currDirection = FF;
        
      break;
      case LL:
        if(motorSpeed == 1){
          myServoLeft.write(91);      
          myServoRight.write(1);         
//          delay(1900);   
        }else if(motorSpeed == 2){

        }else if(motorSpeed == 3){
          myServoLeft.write(91);      
          myServoRight.write(80);        
//          delay(3400);          
        } 
//        currDirection = FF;      
      break;
      case BR:
      break;
      case FR:
      break;  
      case FL:
      break;  
      case LSWING:
        if(motorSpeed == 1){
               
        }else if(motorSpeed == 2){
          
        }else if(motorSpeed == 3){

          myServoLeft.write(91);   //left swing   
          myServoRight.write(80);        
          Serial.println("###left swing");              	  
		}    	        
	  break;
      case RSWING:
        if(motorSpeed == 1){
               
        }else if(motorSpeed == 2){
          
        }else if(motorSpeed == 3){

          myServoLeft.write(103);    //right swing  
          myServoRight.write(90);  
          Serial.println("###right swing");                         
          
        } 
      break;      
      case LBSWING:
        if(motorSpeed == 1){
               
        }else if(motorSpeed == 2){
          
        }else if(motorSpeed == 3){

          myServoLeft.write(91);      //left back swing   
          myServoRight.write(100);              
          Serial.println("###left back swing");                    
        } 
      break;      
      case RBSWING:
        if(motorSpeed == 1){
               
        }else if(motorSpeed == 2){
          
        }else if(motorSpeed == 3){
          myServoLeft.write(80);      //right back swing  
          myServoRight.write(90);        
          Serial.println("###right back swing");                                                      
        }              
      break;
      case STOP:
        myServoLeft.write(91);      
        myServoRight.write(90);                    
      break; 
  }      

}

void RobotMove(){
  isDelayFinish = false;
  switch(currDirection){
      case FF:
        if(motorSpeed == 1){
          myServoLeft.write(180);      
          myServoRight.write(5);  
          Serial.println("move front");                 
        }else if(motorSpeed == 2){
          
        }else if(motorSpeed == 3){
          myServoLeft.write(100+bonusSpeedCw);      
          myServoRight.write(82-bonusSpeedCw);        
          Serial.println("move front");         
        }            
    	while(!isDelayFinish){
    		isDelayFinish = DelayHanlder(3000,FF); 
    	}
    	if(!isLastSecondCollide){
        	currDirection = LSWING;         
        
    	}else{
    		isLastSecondCollide = false;
    	}

      break;
      case BB:
        if(motorSpeed == 1){
          myServoLeft.write(1);      
          myServoRight.write(180);      
          Serial.println("move back"); 
        }else if(motorSpeed == 2){

        }else if(motorSpeed == 3){
          myServoLeft.write(80-bonusSpeedCw);      
          myServoRight.write(100+bonusSpeedCw);  
          Serial.println("move back");                  
        }                    
        break;
      case RR:              

        if(motorSpeed == 1){
          myServoLeft.write(180);      
          myServoRight.write(90);         
          delay(1900);   
		  Serial.println("move right");           
        }else if(motorSpeed == 2){


        }else if(motorSpeed == 3){
          myServoLeft.write(100);      
          myServoRight.write(90);        
          delay(3400);           
		  Serial.println("move right");                       
        } 
        currDirection = FF;
        
      break;
      case LL:
        if(motorSpeed == 1){
          myServoLeft.write(91);      
          myServoRight.write(1);         
          delay(1900);   
		  Serial.println("move left");                     
        }else if(motorSpeed == 2){


        }else if(motorSpeed == 3){
          myServoLeft.write(91);      
          myServoRight.write(80);        
          delay(3400);            
		  Serial.println("move left");                                
        } 
        currDirection = FF;      
      break;
      case BR:
      break;
      case FR:
      break;  
      case FL:
      break;  
      case LSWING:
        if(motorSpeed == 1){
               
        }else if(motorSpeed == 2){
          
        }else if(motorSpeed == 3){

          myServoLeft.write(91);   //left swing   
          myServoRight.write(80);        
          Serial.println("left swing");              	  
		}
    	while(!isDelayFinish){
    		isDelayFinish = DelayHanlder(600,LSWING); 
    	} 
    	if(!isLastSecondCollide){
        	currDirection = LBSWING;                       
        
    	}else{
    		isLastSecondCollide = false;
    	}		        
        
      break;
      case RSWING:
        if(motorSpeed == 1){
               
        }else if(motorSpeed == 2){
          
        }else if(motorSpeed == 3){

          myServoLeft.write(103);    //right swing  
          myServoRight.write(90);  
          Serial.println("right swing");                         
          
        } 
    	while(!isDelayFinish){
    		isDelayFinish = DelayHanlder(600,RSWING); 
    	} 
    	if(!isLastSecondCollide){
        	currDirection = RBSWING;                       
        
    	}else{
    		isLastSecondCollide = false;
    	}            
      break;      
      case LBSWING:
        if(motorSpeed == 1){
               
        }else if(motorSpeed == 2){
          
        }else if(motorSpeed == 3){

          myServoLeft.write(91);      //left back swing   
          myServoRight.write(100);              
          Serial.println("left back swing");                    
        } 
    	while(!isDelayFinish){
    		isDelayFinish = DelayHanlder(600,LBSWING); 
    	}   
    	if(!isLastSecondCollide){
        	currDirection = RSWING;                       
        
    	}else{
    		isLastSecondCollide = false;
    	}                	                      	
      break;      
      case RBSWING:
        if(motorSpeed == 1){
               
        }else if(motorSpeed == 2){
          
        }else if(motorSpeed == 3){
          myServoLeft.write(80);      //right back swing  
          myServoRight.write(90);        
          Serial.println("right back swing");                                                      
        }              
    	while(!isDelayFinish){
    		isDelayFinish = DelayHanlder(700,RBSWING); 
    	} 
    	if(!isLastSecondCollide){
        	currDirection = FF;                       
        
    	}else{
    		isLastSecondCollide = false;
    	}                	      
      break;
      case STOP:
        myServoLeft.write(91);      
        myServoRight.write(90);                    
      break; 
  }      
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
//  Serial.println(cmMsec);
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
