#include <LiquidCrystal.h>
LiquidCrystal lcd(37, 36, 35, 34, 33, 32);

#define Motor_forward    0
#define Motor_return     1
#define Motor_L_dir_pin  7
#define Motor_R_dir_pin  8
#define Motor_L_pwm_pin  9
#define Motor_R_pwm_pin  10

#define ENC_R_pin  2
#define ENC_L_pin  3

const float pulsesPerCm = 8.2;
const float targetDistanceCm = 215;

const int   basePwmL = 143;
const int   basePwmR = 150;
const float Kp = 2.0;
const int   maxCorrection = 25;
const float wheelRatio = 1.00;
const int   nudgePwm = 160;
const int   nudgeMs = 30;

volatile unsigned long pulseCountR = 0;
volatile unsigned long pulseCountL = 0;

void countPulseR() {
    pulseCountR++;
}

void countPulseL() {
    pulseCountL++;
}

void showCounts(unsigned long l, unsigned long r) {
    lcd.setCursor(0, 0);
    lcd.print("L:");
    lcd.print(l);
    lcd.print("   ");
    lcd.setCursor(0, 1);
    lcd.print("R:");
    lcd.print(r);
    lcd.print("   ");
}

void setup() {
    Serial.begin(9600);
    lcd.begin(16, 2);
    lcd.clear();

    pinMode(Motor_L_dir_pin, OUTPUT);
    pinMode(Motor_R_dir_pin, OUTPUT);
    pinMode(Motor_L_pwm_pin, OUTPUT);
    pinMode(Motor_R_pwm_pin, OUTPUT);

    pinMode(ENC_R_pin, INPUT_PULLUP);
    pinMode(ENC_L_pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(ENC_R_pin), countPulseR, RISING);
    attachInterrupt(digitalPinToInterrupt(ENC_L_pin), countPulseL, RISING);

    unsigned long targetPulses = targetDistanceCm * pulsesPerCm;

    digitalWrite(Motor_L_dir_pin, Motor_forward);
    digitalWrite(Motor_R_dir_pin, Motor_forward);

    unsigned long lastLcdUpdate = 0;
    unsigned long countL = 0;
    unsigned long countR = 0;

    while (true) {
        noInterrupts();
        countL = pulseCountL;
        countR = pulseCountR;
        interrupts();

        if (((countL + countR) / 2) >= targetPulses) {
            break;
        }

        float error = (float)countL - wheelRatio * (float)countR;
        int correction = constrain((int)(Kp * error), -maxCorrection, maxCorrection);

        int leftPwm = constrain(basePwmL + correction, 0, 255);
        int rightPwm = constrain(basePwmR - correction, 0, 255);

        analogWrite(Motor_L_pwm_pin, leftPwm);
        analogWrite(Motor_R_pwm_pin, rightPwm);

        if (millis() - lastLcdUpdate >= 200) {
            lastLcdUpdate = millis();
            showCounts(countL, countR);
        }
    }

    analogWrite(Motor_L_pwm_pin, 0);
    analogWrite(Motor_R_pwm_pin, 0);

    delay(500);

    for (int i = 0; i < 30; i++) {
        noInterrupts();
        countL = pulseCountL;
        countR = pulseCountR;
        interrupts();

        if (countL == countR) {
            break;
        }

        int nudgePin = (countL < countR) ? Motor_R_pwm_pin : Motor_L_pwm_pin;
        analogWrite(nudgePin, nudgePwm);
        delay(nudgeMs);
        analogWrite(nudgePin, 0);
        delay(150);
    }

    noInterrupts();
    countL = pulseCountL;
    countR = pulseCountR;
    interrupts();

    lcd.clear();
    showCounts(countL, countR);
}

void loop() {
}
