#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define LED 2
#define Buzzer 3
#define Sensor A1

float LPG_ppm[] = { 200, 1000, 2000, 5000 };
float LPG_ratio[] = { 3.5, 1.0, 0.7, 0.4 };
float R0 = 10000;
float RL = 10000;

unsigned long previousMillis = 0;
const long interval = 200;

float getGasConcentration(float Rs_R0, float ppm[], float ratio[], int size) {
    for (int i = 0; i < size - 1; i++) {
        if (Rs_R0 <= ratio[i] && Rs_R0 >= ratio[i + 1]) {
            return pow(10, log10(ppm[i]) + (log10(Rs_R0) - log10(ratio[i])) /
                (log10(ratio[i + 1]) - log10(ratio[i])) *
                (log10(ppm[i + 1]) - log10(ppm[i])));
        }
    }
    return -1; // Giá trị ngoài phạm vi
}

void setup() {
    Serial.begin(9600);
    lcd.init();
    lcd.backlight();
    pinMode(LED, OUTPUT);
    pinMode(Buzzer, OUTPUT);
}

void loop() {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        int analogValue = analogRead(Sensor);
        float V_RL = analogValue * 0.004887;
        float sensorResistance = (5.0 - V_RL) / V_RL * RL;
        float Rs_R0 = sensorResistance / R0;
        float LPG_concentration = getGasConcentration(Rs_R0, LPG_ppm, LPG_ratio, 4);

        char buffer[10];
        dtostrf(LPG_concentration, 6, 2, buffer);

        lcd.setCursor(0, 0);
        lcd.print("LPG: ");
        lcd.print(buffer);
        lcd.print(" ppm");

        if (LPG_concentration > 1000) {
            digitalWrite(LED, HIGH);
            digitalWrite(Buzzer, HIGH);
            lcd.setCursor(0, 1);
            lcd.print("GAS Detected! ");
        }
        else {
            digitalWrite(LED, LOW);
            digitalWrite(Buzzer, LOW);
            lcd.setCursor(0, 1);
            lcd.print("Safe          ");
        }
    }
}
