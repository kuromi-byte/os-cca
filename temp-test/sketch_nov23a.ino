const int EN12 = 13;   // PWM speed
const int IN1  = 14;
const int IN2  = 15;

void setup() {
  Serial.begin(115200);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(EN12, OUTPUT);
  analogWriteFreq(1000);          // 1 kHz PWM
  analogWriteResolution(8);       // 0-255
  digitalWrite(IN1, HIGH);        // one direction: IN1 high, IN2 low
  digitalWrite(IN2, LOW);
}

void loop() {
  Serial.println("Full speed");  analogWrite(EN12, 255); delay(3000);
  Serial.println("Half speed");  analogWrite(EN12, 128); delay(3000);
  Serial.println("Off");         analogWrite(EN12, 0);   delay(2000);
} 