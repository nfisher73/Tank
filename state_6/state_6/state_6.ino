#define START 1
#define GAP_LEFT 2
#define TANK_RIGHT 3
#define CURVY 4
#define DIAMOND 5
#define LOOP 6

const int left_sens = A0;
const int mid_sens = A1;
const int right_sens = A2;

int threshold_l = 750;
int threshold_r = 800;
int threshold_m = 800;
int state;

const int PWML=11; // Pololu drive A
const int LIN2=10;
const int LIN1 =9;
const int STDBY=8;
const int RIN1 =7; // Pololu drive B
const int RIN2 =6;
const int PWMR =5;


int prev = 0;
int cur_turn = 0;
float aggr = 0.95;

// Line Sensor Variables
int l;
int r;
int m;

bool on_line = true;
bool prev_on_line = true;
int gaps = 0;


#define GAP 1;
#define RIGHT 2;
#define LEFT 3;

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

  prev = 3;
  cur_turn = 0;
  drive(0, 0);

}


void loop() {
  l = sense_l();
  r = sense_r();
  m = sense_m();

  cur_turn = turn_dir_6(prev);
  prev = cur_turn;
  make_turn_6(cur_turn);

  delay(5);
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
    return 0;
  }
}

void make_turn_6(int turn){
  if (turn == 0){
    drive(200, 200);
  }
  if (turn == 1){
    drive(220, 70);
  }
  if (turn == 2) {
    drive(255, 0);
  }
  if (turn == 3) {
    drive(255, -200);
  }
  if (turn == -1){
    drive(70, 255);
  }
  if (turn == -2) {
    drive(100, 255);
  }
  if (turn == -3) {
    drive(-120, 255);
  }
}