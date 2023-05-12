#define START 1
#define GAP_LEFT 2
#define TANK_RIGHT 3
#define CURVY 4
#define DIAMOND 5
#define LOOP 6
#define FINISH 7

const int left_sens = A0;
const int mid_sens = A1;
const int right_sens = A2;

int threshold_l = 500;
int threshold_r = 525;
int threshold_m = 505;
int state;

const int PWML=11; // Pololu drive A
const int LIN2=10;
const int LIN1 =9;
const int STDBY=8;
const int RIN1 =7; // Pololu drive B
const int RIN2 =6;
const int PWMR =5;

unsigned long start_time;
unsigned long time;

int prev = 0;
int cur_turn = 0;
float aggr = 0.95;

// Line Sensor Variables
int l;
int r;
int m;

// Average Line Sensor Variables
double avg_l;
double avg_r;
double avg_m;

bool on_line = true;
bool prev_on_line = true;
int gaps = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(PWMR , OUTPUT);
  pinMode(RIN1 , OUTPUT);
  pinMode(RIN2 , OUTPUT);
  pinMode(LIN1 , OUTPUT);
  pinMode(LIN2 , OUTPUT);
  pinMode(PWML , OUTPUT);
  pinMode(STDBY , OUTPUT);
  digitalWrite(STDBY , HIGH);

  prev = 0;
  cur_turn = 0;
  drive(0, 0);

  state = START;
  start_time = millis();
}


void loop() {
  l = sense_l();
  r = sense_r();
  m = sense_m();

  prev_on_line = on_line;

  if (l == 0 && r == 0 && m == 0){
    on_line = false;
  } else {
    on_line = true;
  }

  switch (state){
    case START:
      simple();

      if (millis() - start_time > 22400){
        state = GAP_LEFT;
        start_time = millis();
      } else if (millis() - start_time > 16000){
        threshold_l = 500;
        threshold_r = 530;
        threshold_m = 505;
      } else if (millis() - start_time > 11500){
        threshold_l = 200;
        threshold_r = 340;
        threshold_m = 200;
      }
      break;


    case GAP_LEFT:
      if (gaps == 3){
        if(on_line == true){
          state = TANK_RIGHT;
          prev = -3;
          start_time = millis();
          //drive(0, 0);
        } else{
          drive(-255, 255);
        }
      }
      else{ 
        if (prev_on_line == true && on_line == false){
        gaps++;
        }
        cur_turn = turn_dir_2();
        make_turn_2(cur_turn);

      }
      break;
    

    case TANK_RIGHT:
      if(on_line == false && millis() - start_time > 1500){
        drive(255, -255);
      } else if (prev_on_line == false && millis() - start_time > 1500){
        state = CURVY;
        prev = 3;
        start_time = millis();
        //drive(0, 0);
      } else {
        simple();
      }
      break;


    case CURVY:
      if(millis() - start_time > 6500){
        state = LOOP;
        start_time = millis();
      } else {
        cur_turn = turn_dir(prev);
        prev = cur_turn;
        make_turn_4(cur_turn);
      }
      break;

    case DIAMOND:
      if(on_line == false && millis() - start_time > 3500){
        drive(200, -200);
      } else if (prev_on_line == false && millis() - start_time > 3500){
        start_time = millis();
        state = LOOP;
      } else {
        cur_turn = turn_dir(prev);
        prev = cur_turn;
        make_turn_5(cur_turn);
      }
      break;

    case LOOP:
      if (on_line == false && millis() - start_time > 10000){
        state = FINISH;
        drive(0, 0);
      } else{
        cur_turn = turn_dir_6(prev);
        prev = cur_turn;
        make_turn_6(cur_turn);
      }
      break;


    case FINISH:
      drive(0, 0);

    default:
      state = START;
      simple();
      break;
  }
  
  delay(5);
}

void simple(){
  cur_turn = turn_dir(prev);
  prev = cur_turn;
  make_turn(cur_turn);
}

void drive(int speedL, int speedR){
  // Here, speed is an integer from -255 to 255
  motorWrite(speedL, LIN1, LIN2, PWML);
  motorWrite(speedR, RIN1, RIN2, PWMR);
}

void motorWrite(int spd, int pin_IN1 , int pin_IN2 , int pin_PWM){
  if (spd < 0){
    digitalWrite(pin_IN1 , HIGH); // go one way
    digitalWrite(pin_IN2 , LOW);
  }
  else{
    digitalWrite(pin_IN1 , LOW); // go the other way
    digitalWrite(pin_IN2 , HIGH);
  }
  analogWrite(pin_PWM , abs(spd));
}

int sense_l() {
  int s = analogRead(left_sens);
  Serial.print("Left: ");
  Serial.println(s);
  if (s > threshold_l) {
    return 1;
  } else {
    return 0;
  }
}
int sense_r() {
  int s = analogRead(right_sens);
  Serial.print("Right: ");
  Serial.println(s);
  if (s > threshold_r) {
    return 1;
  } else {
    return 0;
  }
}
int sense_m() {
  int s = analogRead(mid_sens);
  Serial.print("Mid: ");
  Serial.println(s);
  if (s > threshold_m) {
    return 1;
  } else {
    return 0;
  }
}

int turn_dir(int prev_turn) {
  if (l == 1 && r == 1 && m == 1){
    return 0; //straight
  }
  else if (l == 0 && r == 1 && m ==1){
    return 1; // slight right
  }
  else if (l == 0 && r == 1 && m ==0){
    return 2; // big right
  }
  else if (l == 1 && r == 0 && m ==1){
    return -1; // slight left
  }
  else if (l == 1 && r == 0 && m ==0){
    return -2; // big left
  }
  else if (l==0 && r == 0 && m == 0){
    if (prev_turn == 2 || prev_turn == 1) {
      return 3;
    }
    else if (prev_turn == -2 || prev_turn == -1) {
      return -3;
    }
    return prev_turn; // continue last motion
  }
  else{
    return 0;
  }
}

int turn_dir_2() {
  if (l == 1 && r == 1 && m == 1){
    return 0; //straight
  }
  else if (l == 0 && r == 1 && m ==1){
    return 1; // slight right
  }
  else if (l == 0 && r == 1 && m ==0){
    return 2; // big right
  }
  else if (l == 1 && r == 0 && m ==1){
    return -1; // slight left
  }
  else if (l == 1 && r == 0 && m ==0){
    return -2; // big left
  }
  else if (l==0 && r == 0 && m == 0){
    return 0;
  }
  else{
    return 0;
  }
}

int turn_dir_3(int prev_turn) {
  if (l == 1 && r == 1 && m == 1){
    return 1; //slight right
  }
  else if (l == 0 && r == 1 && m ==1){
    return 2; // big right
  }
  else if (l == 0 && r == 1 && m ==0){
    return 3; // huge right
  }
  else if (l == 1 && r == 0 && m ==1){
    return 0; // straight
  }
  else if (l == 1 && r == 0 && m ==0){
    return -2; // big left
  }
  else if (l==0 && r == 0 && m == 0){
    if (prev_turn == -2){
      return -3;
    } else{
      return 3; // huge right
    }
  }
  else{
    return 0;
  }
}

void make_turn(int turn){
  if (turn == 0){
    drive(255, 255);
  }
  if (turn == 1){
    drive(255, 90);
  }
  if (turn == 2) {
    drive(255, 30);
  }
  if (turn == 3) {
    drive(255, -120);
  }
  if (turn == -1){
    drive(90, 255);
  }
  if (turn == -2) {
    drive(30, 255);
  }
  if (turn == -3) {
    drive(-120, 255);
  }
}

void make_turn_2(int turn){
  if (turn == 0){
    drive(170, 170);
  }
  if (turn == 1){
    drive(170, 30);
  }
  if (turn == 2) {
    drive(200, 10);
  }
  if (turn == -1){
    drive(30, 170);
  }
  if (turn == -2) {
    drive(10, 200);
  }
}

void make_turn_4(int turn){
  if (turn == 0){
    drive(170, 170);
  }
  if (turn == 1){
    drive(200, 100);
  }
  if (turn == 2) {
    drive(255, 75);
  }
  if (turn == 3){
    drive(255, -50);
  }
  if (turn == -1){
    drive(50, 255);
  }
  if (turn == -2) {
    drive(-20, 255);
  }
  if (turn == -3){
    drive(-150, 255);
  }
}

void make_turn_5(int turn){
  if (turn == 0){
    drive(200, 200);
  }
  if (turn == 1){
    drive(200, 100);
  }
  if (turn == 2) {
    drive(200, 25);
  }
  if (turn == 3) {
    drive(255, -120);
  }
  if (turn == -1){
    drive(100, 200);
  }
  if (turn == -2) {
    drive(25, 200);
  }
  if (turn == -3) {
    drive(-120, 255);
  }
}
int turn_dir_6(int prev_turn) {
  if (l == 1 && r == 1 && m == 1){
    return 1; // slight right
  }
  else if (l == 0 && r == 1 && m ==1){
    return 2; // hard right
  }
  else if (l == 0 && r == 1 && m ==0){
    return 3; // huge right
  }
  else if (l == 1 && r == 0 && m ==1){
    return 0; // straight
  }
  else if (l == 1 && r == 0 && m ==0){
    return -2; // big left
  }
  else if (l==0 && r == 0 && m == 0){
    if (prev_turn == 2 || prev_turn == 1) {
      return 3;
    }
    else if (prev_turn == -2 || prev_turn == -1) {
      return -3;
    }
    return prev_turn; // continue last motion
  }
  else{
    return prev_turn;
  }
}

void make_turn_6(int turn){
  if (turn == 0){
    drive(200, 200);
  }
  if (turn == 1){
    drive(220, 50);
  }
  if (turn == 2) {
    drive(200, 75);
  }
  if (turn == 3) {
    drive(255, 0);
  }
  if (turn == -1){
    drive(125, 225);
  }
  if (turn == -2) {
    drive(100, 250);
  }
  if (turn == -3) {
    drive(0, 255);
  }
}