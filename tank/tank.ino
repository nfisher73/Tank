#define START 1;
#define GAP_LEFT 2;
#define TANK_RIGHT 3;
#define CURVY 4;
#define LOOP 5;

const int left_sens = A0;
const int mid_sens = A1;
const int right_sens = A2;

int threshold_l = 600;
int threshold_r = 600;
int threshold_m = 700;
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

int prev_turn[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int prev_mid[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int prev_left[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int prev_right[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

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

  state = START;
  start_time = millis();
}

  cur_turn = turn_dir(prev);
  prev = cur_turn;
  update_prev_turn(cur_turn);
  make_turn(cur_turn, aggr);
  delay(25);

void loop() {
  l = sense_l();
  r = sense_r();
  m = sense_m();
  update_sens(int l, prev_left);
  update_sens(int r, prev_right);
  update_sens(int m, prev_mid);

  switch (state){
    case START:
      simple();
      if (millis() - start_time > 20000) state = GAP_LEFT;
      break;

    case GAP_LEFT:
      cur_turn = turn_dir(prev);
      prev = cur_turn;
      avg_l = get_avg(prev_left, 20);
      avg_r = get_avg(prev_right, 20);
      avg_m = get_avg(prev_mid, 20);

      update_prev_turn(cur_turn);
      make_turn(cur_turn, aggr);
      break;
    
    case TANK_RIGHT:
      // BLuh
      break;

    case CURVY:
      // Gobba
      break;

    case LOOP:
      // GOO
      break;

    default:
      state = START;
      simple();
      break;
  }
  
  delay(25);
}

void simple(){
  cur_turn = turn_dir(prev);
  prev = cur_turn;
  update_prev_turn(cur_turn);
  make_turn(cur_turn, aggr);
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
  if (s > 600) {
    return 1;
  } else {
    return 0;
  }
}
int sense_r() {
  int s = analogRead(right_sens);
  if (s > 600) {
    return 1;
  } else {
    return 0;
  }
}
int sense_m() {
  int s = analogRead(mid_sens);
  if (s > 700) {
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

void update_sens(int val, int sensor[]){
  memcpy(sensor, &sensor[1], sizeof(sensor) - sizeof(int));
  sensor[19] = val;
}

double get_avg(int arr[], int size){
  double avg;
  int sum = 0;

  for (int i = 0; i < size; i++){
    sum += arr[i];
  }
  avg = double(sum)/size;
  
  return avg;
}