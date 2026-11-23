// ---- Pins ----
const int NTC_PIN = 26;
const int EN12 = 13;   // PWM speed
const int IN1  = 14;
const int IN2  = 15;

// ---- Thermistor ----
const float R_FIXED = 10000.0;
const float R0      = 10000.0;
const float T0_K    = 298.15;
const float BETA    = 3950.0;
const int   ADC_MAX = 4095;
const bool  NTC_ON_LOW_SIDE = true;

// ---- Fan control (demo range 22-28 C) ----
const float T_ON     = 22.0;  // fan turns on at or above this (C)
const float T_OFF    = 21.0;  // fan turns off at or below this (C)
const float T_FULL   = 28.0;  // full speed at or above this (C)
const int   MIN_DUTY = 100;   // lowest PWM (0-255) that reliably spins the fan
const int   MAX_DUTY = 255;

bool fanOn = false;

float readTempC() {
  long sum = 0;
  for (int i = 0; i < 16; i++) { sum += analogRead(NTC_PIN); delay(2); }
  float adc = sum / 16.0;
  if (adc < 1 || adc > ADC_MAX - 1) return NAN;
  float rNtc = NTC_ON_LOW_SIDE ? R_FIXED * adc / (ADC_MAX - adc)
                               : R_FIXED * (ADC_MAX - adc) / adc;
  float tK = 1.0 / (1.0 / T0_K + log(rNtc / R0) / BETA);
  return tK - 273.15;
}

void setFan(int duty) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(EN12, duty);
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(EN12, OUTPUT);
  analogWriteFreq(1000);
  analogWriteResolution(8);
  setFan(0);
}

void loop() {
  float t = readTempC();
  int duty;

  if (isnan(t)) {
    duty = MAX_DUTY;               // sensor fault: fail safe, run full
    fanOn = true;
    Serial.println("Sensor fault -> fan FULL");
  } else {
    if (!fanOn && t >= T_ON)  fanOn = true;
    if (fanOn  && t <= T_OFF) fanOn = false;

    if (fanOn) {
      float frac = (t - T_ON) / (T_FULL - T_ON);
      frac = constrain(frac, 0.0, 1.0);
      duty = MIN_DUTY + (int)(frac * (MAX_DUTY - MIN_DUTY));
    } else {
      duty = 0;
    }
    Serial.print("T="); Serial.print(t, 1);
    Serial.print(" C  fan="); Serial.print(fanOn ? "ON" : "OFF");
    Serial.print("  duty="); Serial.println(duty);
  }

  setFan(duty);
  delay(500);
}