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
int threshold_m = 500;
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

  Serial.println(" ");
  Serial.print("L: ");
  Serial.print(l);
  Serial.print("   R: ");
  Serial.print(r);
  Serial.print("   M: ");
  Serial.println(m);

  delay(2000);
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