#include<Arduino.h>
#include<Wire.h>
#include<FluxGarage_RoboEyes.h>
#include<Adafruit_GFX.h>
#include<Adafruit_SSD1306.h>
#include<Adafruit_VL53L0X.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_MPU6050 mpu;

const int TC_L = 33;
const int TC_C = 32;
const int TC_R = 35;

unsigned long interactionStartTime = 0;

bool glancing = false;

unsigned long nextGlanceTime = 0;
unsigned long glanceEndTime = 0;

bool curiousGlancing = false;

unsigned long nextCuriousGlanceTime = 0;
unsigned long curiousGlanceEndTime = 0;
unsigned long curiousStartTime = 0;

int interestX = 0;
int interestY = 0;

bool panicShakeLeft = false;

unsigned long nextPanicShakeTime = 0;

bool lookingAtOwner = true;

unsigned long nextFocusSwitchTime = 0;

int ownerX = -8;
int ownerY = -2;

int studyX = 8;
int studyY = 2;

bool yawning = false;

unsigned long nextYawnTime = 0;
unsigned long yawnEndTime = 0;

bool lookingAtOwnerMischief = false;

unsigned long nextMischiefAction = 0;

int awayX = 10;
int awayY = 0;

int confusedStage = 0;

unsigned long nextConfusedLookTime = 0;

int confusedTargetX = 0;
int confusedTargetY = 0;

unsigned long stateEnterTime = 0;

Adafruit_SSD1306 display(
  SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET
);

RoboEyes<Adafruit_SSD1306> eyes(display);

struct Percepts
{
  bool ownerVisible;
  bool objectVisible;
  bool edgeLeft;
  bool edgeCenter;
  bool edgeRight;
  bool batteryLow;
  bool charging;
  bool ownerStudying;
  bool objectClose;
  int objectX;
  int objectY;
  int objectDistance;
  int frontDistance; 

  float accelX;
  float accelY;
  float accelZ;
  float gyroX;
  float gyroY;
  float gyroZ;
};

Percepts world;

Adafruit_VL53L0X lox;

enum MochiState{
  STATE_IDLE,
  STATE_EXPLORE,
  STATE_INTERACT,
  STATE_AVOID,
  STATE_SLEEP,
  STATE_FOCUS
};

enum Emotion{
    NORMAL,
  CURIOUS,
  PANIC,
  MISCHIEF,
  MAD,
  SLEEPY,
  EXCITED,
  DIZZY,
  SHY,
  CONFUSED,
  RELIEVED
};

enum AvoidPhase{
  AVOID_REVERSE,
  AVOID_TURN,
  AVOID_RECOVER
};

enum InteractPhase
{
    INTERACT_LOOK,
    INTERACT_LOOK_LEFT,
    INTERACT_LOOK_RIGHT,
    INTERACT_COMPLETE
};

InteractPhase interactPhase;
unsigned long interactStarttime = 0;

AvoidPhase avoidphase;
unsigned long avoidStartTime = 0;

class MotorDriver {
public:
  MotorDriver(int ain1, int ain2, int pwma, int bin1, int bin2, int pwmb, int stby);

  void begin();
  void stop();
  void forward(uint8_t speed);
  void back(uint8_t speed);
  void setMotorSpeeds(int left,int right);

private:
  int _ain1, _ain2, _pwma, _bin1, _bin2, _pwmb, _stby;
};

MochiState currentState = STATE_IDLE;
Emotion currentEmotion = NORMAL;

MotorDriver motor(26, 27, 25, 18, 19, 23, 12);

void updateIMU(){
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;

  mpu.getEvent(&accel, &gyro, &temp);

  Serial.print("Accel X: ");
  Serial.print(accel.acceleration.x);

  Serial.print("  Y: ");
  Serial.print(accel.acceleration.y);

  Serial.print("  Z: ");
  Serial.println(accel.acceleration.z);

  world.accelX = accel.acceleration.x;
  world.accelY = accel.acceleration.y;
  world.accelZ = accel.acceleration.z;

  world.gyroX = gyro.gyro.x;
  world.gyroY = gyro.gyro.y;
  world.gyroZ = gyro.gyro.z;
}

void updateTCRT(){
  world.edgeCenter = digitalRead(TC_C);
  Serial.print("Center: ");
  Serial.println(world.edgeCenter);

  world.edgeLeft = digitalRead(TC_L);
  Serial.print("Left: ");
  Serial.println(world.edgeLeft);

  world.edgeRight = digitalRead(TC_R);
  Serial.print("Right: ");
  Serial.println(world.edgeRight);
}

void updateVL(){
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure,false);

  if(measure.RangeStatus != 4){
  world.frontDistance = measure.RangeMilliMeter;
  world.objectClose = measure.RangeMilliMeter < 200;

  Serial.print("Dist: ");
  Serial.println(world.frontDistance);
  }else{
    Serial.println("Out of Range");
  }
}

void updateSensors(){
  updateTCRT();
  updateVL();
  updateIMU();
} 

void changeState(MochiState newState)
{
    if (newState == currentState)
    {
        return;
    }

    onExitState(currentState);

    currentState = newState;
    stateEnterTime = millis();

    onEnterState(newState);
}

void evaluateStateTransitions()
{
  if(currentState == STATE_INTERACT && millis() - interactionStartTime < 5000){
    return;
  }
  if(world.edgeLeft || world.edgeCenter || world.edgeRight){
    changeState(STATE_AVOID);
    return;
  }
  if(world.charging){
    changeState(STATE_SLEEP);
    return;
  }
  if(world.ownerVisible && world.ownerStudying){
    changeState(STATE_FOCUS);
    return;
  }
  if(world.objectVisible && world.objectDistance < 50){
    changeState(STATE_INTERACT);
    return;
  }
  if(world.objectVisible){
    changeState(STATE_EXPLORE);
    return;
  }
  if(!world.edgeLeft && !world.edgeCenter && !world.edgeRight && !world.objectVisible && !world.ownerVisible && !world.charging){
    changeState(STATE_IDLE);
    return;
  }
}

void evaluateEmotion(){

}

void updateNormalIdle()
{
    if(!glancing)
    {
        if(millis() >= nextGlanceTime)
        {

            int dx = random(-5, 6);
            int dy = random(-3, 4);

            eyes.eyeLxNext = eyes.eyeLxDefault + dx;
            eyes.eyeLyNext = eyes.eyeLyDefault + dy;

            eyes.eyeRxNext = eyes.eyeRxDefault + dx;
            eyes.eyeRyNext = eyes.eyeRyDefault + dy;

            glancing = true;
            glanceEndTime = millis() + 1000;
        }
    }
    else
    {
        if(millis() >= glanceEndTime)
        {

            eyes.eyeLxNext = eyes.eyeLxDefault;
            eyes.eyeLyNext = eyes.eyeLyDefault;

            eyes.eyeRxNext = eyes.eyeRxDefault;
            eyes.eyeRyNext = eyes.eyeRyDefault;

            glancing = false;

            nextGlanceTime = millis() + random(6000,10001);
        }
    }
}

void updateCurious()
{
    if(millis() - curiousStartTime >= 5000)
    {
        setEmotion(NORMAL);
        return;
    }

    if(!curiousGlancing)
    {
        if(millis() >= nextCuriousGlanceTime)
        {
            int dx = interestX + random(-5,6);
            int dy = interestY + random(-5,6);

            eyes.eyeLxNext = eyes.eyeLxDefault + dx;
            eyes.eyeLyNext = eyes.eyeLyDefault + dy;

            eyes.eyeRxNext = eyes.eyeRxDefault + dx;
            eyes.eyeRyNext = eyes.eyeRyDefault + dy;

            curiousGlancing = true;

            curiousGlanceEndTime = millis() + 700;
        }
    }
    else
    {
        if(millis() >= curiousGlanceEndTime)
        {
            eyes.eyeLxNext = eyes.eyeLxDefault;
            eyes.eyeLyNext = eyes.eyeLyDefault;

            eyes.eyeRxNext = eyes.eyeRxDefault;
            eyes.eyeRyNext = eyes.eyeRyDefault;

            curiousGlancing = false;

            nextCuriousGlanceTime =
                millis() + random(2000,4001);
        }
    }
}



void updatePanic()
{
    if(millis() >= nextPanicShakeTime)
    {
        int shakeOffset;

        if(panicShakeLeft)
        {
            shakeOffset = -2;
        }
        else
        {
            shakeOffset = 2;
        }

        eyes.eyeLxNext = eyes.eyeLxDefault + shakeOffset;
        eyes.eyeRxNext = eyes.eyeRxDefault + shakeOffset;

        eyes.eyeLyNext = eyes.eyeLyDefault;
        eyes.eyeRyNext = eyes.eyeRyDefault;

        panicShakeLeft = !panicShakeLeft;

        nextPanicShakeTime = millis() + 100;
    }
}

void updateAvoid(){
  switch(avoidphase){
    case AVOID_REVERSE:
    
    motor.back(150);
     if(millis() - avoidStartTime > 1000){
      motor.stop();

      avoidphase = AVOID_TURN;
      avoidStartTime = millis();
     }
    break;

    case AVOID_TURN:
    if(world.edgeLeft){
      motor.setMotorSpeeds(150,-150);
    }else if(world.edgeRight){
      motor.setMotorSpeeds(-150,150);
    }else if(world.edgeCenter){
      if(random(0,2) == 0){
        motor.setMotorSpeeds(150,-150);
      }else{
        motor.setMotorSpeeds(-150,150);
      }
    }
    if(millis() - avoidStartTime > 500){
    motor.stop();
    avoidphase = AVOID_RECOVER;
    avoidStartTime = millis();
    }
    break;

    case AVOID_RECOVER:
    if(millis() - avoidStartTime > 500){
      setEmotion(RELIEVED);
      changeState(STATE_IDLE);
    }
    break;
  }
}

void enterInteract(){
  interactStarttime = millis();
  interactPhase = INTERACT_LOOK;
  setEmotion(CURIOUS);
}

void enterFocus()
{
  Serial.println("ENTER FOCUS");
    eyes.setWidth(20,20);
    eyes.setHeight(30,30);

    eyes.setBorderradius(10,10);

    eyes.setSpacebetween(10);

    eyes.setMood(DEFAULT);

    eyes.setAutoblinker(ON,8,2);

    eyes.setIdleMode(OFF);

    lookingAtOwner = true;

    nextFocusSwitchTime =
        millis() + random(3000,5001);
}

void updateFocus()
{
    Serial.println("UPDATE FOCUS");

    int targetX;
    int targetY;

    if(lookingAtOwner)
    {
        targetX = ownerX;
        targetY = ownerY;
    }
    else
    {
        targetX = studyX;
        targetY = studyY;
    }

    // Steady gaze (no jitter for now)
    eyes.eyeLxNext = eyes.eyeLxDefault + targetX;
    eyes.eyeLyNext = eyes.eyeLyDefault + targetY;

    eyes.eyeRxNext = eyes.eyeRxDefault + targetX;
    eyes.eyeRyNext = eyes.eyeRyDefault + targetY;

    if(millis() >= nextFocusSwitchTime)
    {
        lookingAtOwner = !lookingAtOwner;

        if(lookingAtOwner)
        {
            nextFocusSwitchTime =
                millis() + random(3000,5001);
        }
        else
        {
            nextFocusSwitchTime =
                millis() + random(5000,10001);
        }
    }
}

void enterSleepy(){
  eyes.setWidth(30,30);
  eyes.setHeight(10,10);
  eyes.setBorderradius(5,5);
  eyes.setSpacebetween(15);
  
  nextYawnTime = millis() + random(2000,3001);

  
  eyes.setAutoblinker(ON,10,3);
  eyes.setIdleMode(OFF);
}

void updateSleepy()
{
    if(!yawning)
    {
        if(millis() >= nextYawnTime)
        {
            yawning = true;

            eyes.setWidth(35,35);
            eyes.setHeight(40,40);
            eyes.setBorderradius(15,15);

            yawnEndTime = millis() + 2000;
        }
    }
    else
    {
        if(millis() >= yawnEndTime)
        {
            yawning = false;

            eyes.setWidth(30,30);
            eyes.setHeight(10,10);

            nextYawnTime =
                millis() + random(20000,40001);
        }
    }
}

void enterMischief()
{
    eyes.setWidth(30,30);
    eyes.setHeight(25,25);

    eyes.setBorderradius(8,8);

    eyes.setSpacebetween(10);

    eyes.setAutoblinker(ON,4,2);

    eyes.setIdleMode(OFF);

    lookingAtOwnerMischief = true;

    nextMischiefAction =
        millis() + random(1000,2001);
}

void updateMischief()
{
    int targetX;
    int targetY;

    if(lookingAtOwnerMischief)
    {
        targetX = ownerX;
        targetY = ownerY;
    }
    else
    {
        targetX = awayX;
        targetY = awayY;
    }

    eyes.eyeLxNext = eyes.eyeLxDefault + targetX;
    eyes.eyeLyNext = eyes.eyeLyDefault + targetY;

    eyes.eyeRxNext = eyes.eyeRxDefault + targetX;
    eyes.eyeRyNext = eyes.eyeRyDefault + targetY;

    if(millis() >= nextMischiefAction)
    {
        lookingAtOwnerMischief =
            !lookingAtOwnerMischief;

        nextMischiefAction =
            millis() + random(1000,3001);
    }
}

void updateConfused()
{
    if(millis() >= nextConfusedLookTime)
    {
        confusedStage++;

        if(confusedStage > 2)
        {
            confusedStage = 0;
        }

        switch(confusedStage)
        {
            case 0: // look left
                confusedTargetX = -10;
                confusedTargetY = 0;
                break;

            case 1: // look right
                confusedTargetX = 10;
                confusedTargetY = 0;
                break;

            case 2: // look at owner
                confusedTargetX = 0;
                confusedTargetY = 0;
                break;
        }

        nextConfusedLookTime =
            millis() + 1500;
    }

    if(confusedTargetX < 0)
    {
      eyes.setWidth(35,30);
      eyes.setHeight(25,20);

      eyes.eyeLyNext = eyes.eyeLyDefault;
      eyes.eyeRyNext = eyes.eyeRyDefault - 2;
    }
    else if(confusedTargetX > 0)
    {
        eyes.setWidth(30,35);
        eyes.setHeight(20,25);

        eyes.eyeLyNext = eyes.eyeLyDefault-2;
        eyes.eyeRyNext = eyes.eyeRyDefault;
    }
    else
    {
      eyes.setWidth(30,30);
      eyes.setHeight(25,25);

      eyes.eyeLyNext = eyes.eyeLyDefault;
      eyes.eyeRyNext = eyes.eyeRyDefault;
    }

    eyes.eyeLxNext =
        eyes.eyeLxDefault + confusedTargetX;

    eyes.eyeRxNext =
        eyes.eyeRxDefault + confusedTargetX;

    eyes.eyeLyNext =
        eyes.eyeLyDefault + confusedTargetY;

    eyes.eyeRyNext =
        eyes.eyeRyDefault + confusedTargetY;
}

void updateInteract(){
  switch(interactPhase){
    case INTERACT_LOOK:
    if(millis() - interactStarttime > 1000){
      interactPhase = INTERACT_LOOK_LEFT;
      interactStarttime = millis();
    }
    break;

    case INTERACT_LOOK_LEFT:
    motor.setMotorSpeeds(-50,50);
    if(millis() - interactStarttime > 1000){
      interactPhase = INTERACT_LOOK_RIGHT;
      interactStarttime = millis();
    }
    break;

    case INTERACT_LOOK_RIGHT:
    motor.setMotorSpeeds(50,-50);
    if(millis() - interactStarttime > 1000){
      changeState(STATE_IDLE);
    }
    break;
  }
}

void enterConfused()
{
    eyes.setWidth(30,30);
    eyes.setHeight(25,25);

    eyes.setBorderradius(8,8);

    eyes.setSpacebetween(10);

    eyes.setAutoblinker(ON,4,2);

    eyes.setIdleMode(OFF);

    confusedStage = 0;

    nextConfusedLookTime =
        millis() + 1500;
}

void enterAvoid(){
  avoidphase = AVOID_REVERSE;
  avoidStartTime = millis();
  setEmotion(PANIC);
}

void setEmotion(Emotion e){
  currentEmotion = e;

  switch(e){

    case NORMAL:

    eyes.setWidth(30,30);
    eyes.setHeight(25,25);
    eyes.setBorderradius(8,8);
    eyes.setSpacebetween(10);

    eyes.setMood(DEFAULT);

    eyes.setCuriosity(false);

    eyes.setAutoblinker(true, 3, 2);

    eyes.setIdleMode(false);
    break;

    case CURIOUS:
    eyes.setWidth(35,35);
    eyes.setHeight(30,30);
    eyes.setBorderradius(8,8);
    eyes.setSpacebetween(10);

    eyes.setMood(DEFAULT);

    eyes.setAutoblinker(ON,6,2);

    eyes.setIdleMode(OFF);

    interestX = random(-15,16);
    interestY = random(-10,11);

    curiousStartTime = millis();

    nextCuriousGlanceTime = millis();

    curiousGlancing = false;
    break;

    case PANIC:
    eyes.setWidth(18,18);
    eyes.setHeight(18,18);
    eyes.setBorderradius(9,9);
    eyes.setSpacebetween(10);

    eyes.setMood(DEFAULT);

    eyes.setAutoblinker(ON,1,1);

    eyes.setIdleMode(OFF);
    break;

    case MISCHIEF:
    break;

    case MAD:
    break;

    case SLEEPY:
    enterSleepy();
    break;

    case EXCITED:
    break;

    case DIZZY:
    break;
    
    case SHY:
    break;

    case CONFUSED:
    enterConfused();
    break;

    case RELIEVED:
    break;
  }
}

void onEnterState(MochiState s)
{
    Serial.print("ENTER ");
    Serial.println(s);

    switch(s)
    {
        case STATE_IDLE:
            Serial.println("IDLE");
            setEmotion(NORMAL);
            break;

        case STATE_EXPLORE:
            setEmotion(CURIOUS);
            Serial.println("EXPLORE");
            break;

        case STATE_AVOID:
            enterAvoid();
            Serial.println("AVOID");
            break;

        case STATE_FOCUS:
            enterFocus();
            Serial.println("FOCUS");
            break;
        
        case STATE_INTERACT:
            enterInteract();
            Serial.println("INTERACT");
            break;

        default:
            break;
    }
}

void onExitState(MochiState s) {
  Serial.print("EXIT ");
  Serial.println(s);
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21,22);

  if (!mpu.begin()) {
    Serial.println("MPU6050 FAILED");
    while (1);
  }

  Serial.println("MPU6050 OK");

  if (!lox.begin()) {
  Serial.println("VL53L0X FAILED");
  while (1);
  }

  Serial.println("VL53L0X OK");

  display.begin(SSD1306_SWITCHCAPVCC,0x3C);
  motor.begin();
  eyes.begin(128,64,100);
  randomSeed(micros());
  pinMode(TC_L, INPUT);
  pinMode(TC_C, INPUT);
  pinMode(TC_R, INPUT);

  setEmotion(CONFUSED);

  nextGlanceTime = millis() + random(6000,10001);
}

// void loop() {
//   digitalWrite(12, HIGH);  // STBY

//   digitalWrite(26, HIGH);
//   digitalWrite(27, LOW);

//   digitalWrite(25, HIGH);  // PWMA HIGH

//   while (1);
// }

// void loop() {

//   Serial.println("Forward");
//   motor.forward(150);
//   delay(2000);

//   Serial.println("Stop");
//   motor.stop();
//   delay(1000);

//   Serial.println("Backward");
//   motor.back(150);
//   delay(2000);

//   Serial.println("Stop");
//   motor.stop();
//   delay(1000);
// }

void loop() {
  updateSensors();

  evaluateStateTransitions();

  evaluateEmotion();

  switch(currentState)
{
    case STATE_IDLE:
      updateNormalIdle();
      break;

    case STATE_EXPLORE:
      updateCurious();
      break;

    case STATE_AVOID:
      updateAvoid();
      break;

    case STATE_FOCUS:
      updateFocus();
      break;

    case STATE_INTERACT:
      updateInteract();
      break;

    default:
        break;
}

if(currentEmotion == SLEEPY){
  updateSleepy();
}else if(currentEmotion == MISCHIEF){
  updateMischief();
}else if(currentEmotion == CONFUSED){
  updateConfused();
}

eyes.update();
}
