#include <LiquidCrystal.h>
LiquidCrystal lcd(37, 36, 35, 34, 33, 32);

#define Motor_forward    0
#define Motor_return     1
#define Motor_L_dir_pin  7
#define Motor_R_dir_pin  8
#define Motor_L_pwm_pin  9
#define Motor_R_pwm_pin  10

const int swPin = 19;
const int vrxPin = A8;
const int vryPin = A9;

volatile int pressCount = 0;
volatile bool state = true;
volatile bool runMotor = false;
volatile unsigned long lastPressTime = 0;

bool lastState = true;

void runMotorTest() {
  digitalWrite(Motor_R_dir_pin, Motor_return);
  digitalWrite(Motor_L_dir_pin, Motor_return);

  for (int pwm = 150; pwm > 50; pwm--) {
    analogWrite(Motor_L_pwm_pin, pwm);
    analogWrite(Motor_R_pwm_pin, pwm);
    delay(100);
  }

  digitalWrite(Motor_R_dir_pin, Motor_forward);
  digitalWrite(Motor_L_dir_pin, Motor_forward);

  for (int pwm = 50; pwm < 150; pwm++) {
    analogWrite(Motor_L_pwm_pin, pwm);
    analogWrite(Motor_R_pwm_pin, pwm);
    delay(100);
  }

  analogWrite(Motor_L_pwm_pin, 0);
  analogWrite(Motor_R_pwm_pin, 0);
  delay(500);
}

void setup() {
  Serial.begin(9600);

  pinMode(swPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(swPin), interrupt, FALLING);

  lcd.begin(16, 2);
  lcd.clear();

  pinMode(Motor_L_dir_pin, OUTPUT);
  pinMode(Motor_R_dir_pin, OUTPUT);
  pinMode(Motor_L_pwm_pin, OUTPUT);
  pinMode(Motor_R_pwm_pin, OUTPUT);

}

void interrupt() {
  unsigned long now = millis();
  if (now - lastPressTime > 500) {
    pressCount++;
    state = !state;
    lastPressTime = now;

    runMotor = true;

  }
}


void loop() {
  Serial.print("pressCount: ");
  Serial.println(pressCount);
  if (state != lastState) {
    lcd.clear();
    lastState = state;
  }

  if (runMotor == false) {
    int xValue = analogRead(vrxPin);
    float xPercent = xValue * (100.0 / 1023.0);

    int yValue = analogRead(vryPin);
    float yPercent = yValue * (100.0 / 1023.0);

    lcd.setCursor(0, 0);
    lcd.print("X:");
    lcd.print(xValue);
    lcd.print(" ");
    lcd.print(xPercent);
    lcd.print("%");

    lcd.setCursor(0, 1);
    lcd.print("Y:");
    lcd.print(yValue);
    lcd.print(" ");
    lcd.print(yPercent);
    lcd.print("%");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Push counter: ");
    lcd.setCursor(0, 1);
    lcd.print(pressCount);
  }

  if (runMotor == true) {
    runMotorTest();
    runMotor = false;
  }
}
