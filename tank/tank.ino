const int left_sens = A0;
const int mid_sens = A1;
const int right_sens = A2;

int threshold = 600;

const int PWML=11; // Pololu drive A
const int LIN2=10;
const int LIN1 =9;
const int STDBY=8;
const int RIN1 =7; // Pololu drive B
const int RIN2 =6;
const int PWMR =5;

unsigned long start_time;
unsigned long time;

int prev_turn[10] = {0,  0, 0, 0, 0, 0, 0, 0, 0, 0};
int prev = 0;
int cur_turn = 0;
float aggr = 0.95;

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
  start_time = micros();
}

void loop() {
  cur_turn = turn_dir(prev);
  prev = cur_turn;
  update_prev_turn(cur_turn);
  make_turn(cur_turn, aggr);
  delay(25);
  // Serial.println(cur_turn);
  //Serial.print(prev_turn[1]);
  //delay(1000);

  // put your main code here, to run repeatedly:
  // Serial.print("Left Sensor: ");
  // Serial.println(analogRead(left_sens));
  // Serial.print("Middle Sensor: ");
  // Serial.println(analogRead(mid_sens));
  // Serial.print("Right Sensor: ");
  // Serial.println(analogRead(right_sens));
  // Serial.println(" ");
  // motorWrite(155, LIN1, LIN2, PWML);
  //delay(1000);
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
  int left = analogRead(left_sens);
  if (left > 600) {
    return 1;
  } else {
    return 0;
  }
}
int sense_r() {
  int left = analogRead(right_sens);
  if (left > 600) {
    return 1;
  } else {
    return 0;
  }
}
int sense_m() {
  int left = analogRead(mid_sens);
  if (left > 700) {
    return 1;
  } else {
    return 0;
  }
}

int turn_dir(int prev_turn) {
  int l = sense_l();
  int r = sense_r();
  int m = sense_m();
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
    if (prev_turn == 2) {
      return 3;
    }
    else if (prev_turn == -2) {
      return -3;
    }
    return prev_turn; // continue last motion
  }
  else{
    return 0;
  }
}

void make_turn(int turn, float aggr){
  if (turn == 0){
    drive(230, 230);
  }
  if (turn == 1){
    drive(150 + aggr*96, 150 - aggr*96);
  }
  if (turn == 2) {
    drive(128 + aggr*128, 128 - aggr*128);
  }
  if (turn == 3) {
    drive(255, -100);
  }
  if (turn == -1){
    drive(150 - aggr*96, 150 + aggr*96);
  }
  if (turn == -2) {
    drive(128 - aggr*128, 128 + aggr*128);
  }
  if (turn == -3) {
    drive(-100, 255);
  }
}

void update_prev_turn(int cur_turn){
  memcpy(prev_turn, &prev_turn[1], sizeof(prev_turn) - sizeof(int));
  prev_turn[9] =  cur_turn;
}

void special_case(){

}