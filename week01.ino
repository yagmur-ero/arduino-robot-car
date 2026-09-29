const int ledPin = 8;

const float treshold = 2.5;


void setup() {
  
  Serial.begin(9600);
  pinMode(ledPin,OUTPUT);

}

void loop() {
  int adcValue = analogRead(A0);
  float voltage = adcValue * (5.0 / 1023.0);

  Serial.print(voltage);
  Serial.println(" V");

  if (voltage < treshold) {
    digitalWrite(ledPin, LOW);
  }else{
    digitalWrite(ledPin, HIGH);
  }

  delay(1000);

}
