#define BLYNK_TEMPLATE_ID "TMPL63zcSLyYx"    // Correct Template ID from Blynk
#define BLYNK_TEMPLATE_NAME "Water Tank Monitoring"    // Template Name from Blynk
#define BLYNK_AUTH_TOKEN "_G7ey4lXV3PpBSb_aMnNK0DfK5bIG04A"    // Auth Token from Blynk

#define BLYNK_PRINT Serial

#include <WiFi.h>              // Use the library corresponding to your board (ESP8266, ESP32, etc.)
#include <BlynkSimpleEsp32.h>   // Blynk library for ESP32
//#include <BlynkTimer.h>         // Blynk's built-in timer (replacement for SimpleTimer)

#define trig 25
#define echo 26

char ssid[] = "Redmi10hesh";    // Replace with your Wi-Fi SSID
char pass[] = "9852691hesh";    // Replace with your Wi-Fi password

BlynkTimer timer;  // Initialize Blynk timer

bool manualMode = false;  // Flag to track manual mode
bool pumpState = false;   // Track pump state for manual control

void sendSensorData();  // Forward declaration for sensor data function

// Blynk callback for manual mode switch (V2)
BLYNK_WRITE(V2) {
  manualMode = param.asInt();  // Read the value of the manual mode switch
  Serial.print("Manual Mode: ");
  Serial.println(manualMode ? "ON" : "OFF");
}

// Blynk callback for manual pump control (when manual mode is enabled) (V3)
BLYNK_WRITE(V3) {
  if (manualMode) {  // Only allow manual pump control if manual mode is ON
    pumpState = param.asInt();  // Read the value from the Blynk app
    digitalWrite(5, pumpState);  // Control pump manually
    Serial.println(pumpState ? "Pump turned ON manually" : "Pump turned OFF manually");
  }
}

void setup() {
  // Initialize pin modes
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(5, OUTPUT);

  Serial.begin(9600);           // Start Serial communication for debugging

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);  // Initialize Blynk with your Wi-Fi credentials

  timer.setInterval(1000L, sendSensorData);   // Set a timer to send sensor data to Blynk every second
}

void loop() {
  Blynk.run();   // Run Blynk
  timer.run();   // Run the BlynkTimer for scheduled tasks
}

// Function to send sensor data to Blynk
void sendSensorData() {
  if (!manualMode) {  // Only run automatic control if manual mode is off
    // Send a pulse
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);

    // Read the echo
    long duration = pulseIn(echo, HIGH);
    long cm = (duration / 29) / 2;  // Convert echo time to distance in cm

    Serial.print(cm);
    Serial.println(" cm");

    // Send distance to Blynk on virtual pin V0
    Blynk.virtualWrite(V0, cm);

    // Send pump status to Blynk on virtual pin V1
    Blynk.virtualWrite(V1, digitalRead(5));

    // Control LEDs based on distance
    if (cm >= 35) {
      digitalWrite(13, HIGH);  // Turn on LED for > 35 cm
      digitalWrite(12, LOW);
      digitalWrite(14, LOW);
      digitalWrite(5, HIGH);   // Motor ON
      Serial.println("Motor must turn on");
    } else if (cm < 35 && cm >= 25) {
      digitalWrite(13, LOW);
      digitalWrite(12, HIGH);  // Turn on LED for 25-35 cm
      digitalWrite(14, LOW);
    } else if (cm < 25 && cm >= 10) {
      digitalWrite(13, LOW);
      digitalWrite(12, LOW);
      digitalWrite(14, HIGH);  // Turn on LED for 10-25 cm
    } else {
      digitalWrite(13, HIGH);  // All LEDs ON for < 10 cm
      digitalWrite(12, HIGH);
      digitalWrite(14, HIGH);
      digitalWrite(5, LOW);    // Motor OFF
      Serial.println("Motor must turn off");
    }
    
    delay(500);  // Small delay to prevent sensor overflow
  }
}
