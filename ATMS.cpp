#include <Arduino.h>

// Pin Configuration
const int RADAR_SENSOR_PIN = 2;   // Radar sensor input pin
const int HUMAN_SENSOR_PIN = 3;   // Pedestrian detection sensor input pin
const int RF_RECEIVER_PIN = 4;    // RF receiver input pin for emergency vehicles
const int MOTION_SENSOR_PIN = 5;  // Motion sensor input for unexpected vehicle movement
const int RED_LIGHT_PIN = 6;      // Red light relay
const int YELLOW_LIGHT_PIN = 7;   // Yellow light relay
const int GREEN_LIGHT_PIN = 8;    // Green light relay

int vehicleCount = 0;

void setup() {
    pinMode(RADAR_SENSOR_PIN, INPUT);
    pinMode(HUMAN_SENSOR_PIN, INPUT);
    pinMode(RF_RECEIVER_PIN, INPUT);
    pinMode(MOTION_SENSOR_PIN, INPUT);
    
    pinMode(RED_LIGHT_PIN, OUTPUT);
    pinMode(YELLOW_LIGHT_PIN, OUTPUT);
    pinMode(GREEN_LIGHT_PIN, OUTPUT);

    // Start with red light ON
    digitalWrite(RED_LIGHT_PIN, HIGH);
    digitalWrite(YELLOW_LIGHT_PIN, LOW);
    digitalWrite(GREEN_LIGHT_PIN, LOW);

    Serial.begin(9600);
}

void changeTrafficLight(int lightPin, int duration) {
    // Turn off all lights first
    digitalWrite(RED_LIGHT_PIN, LOW);
    digitalWrite(YELLOW_LIGHT_PIN, LOW);
    digitalWrite(GREEN_LIGHT_PIN, LOW);
    
    // Turn on the selected light
    digitalWrite(lightPin, HIGH);
    delay(duration);  // Keep light on for specified duration
    
    // Turn off the light and reset to red
    digitalWrite(lightPin, LOW);
    digitalWrite(RED_LIGHT_PIN, HIGH);
}

int calculateGreenTime(int count) {
    if (count > 15) return 15000; // 15+ vehicles, green for 15 sec
    if (count > 10) return 10000; // 10-15 vehicles, green for 10 sec
    if (count > 5) return 7000;   // 5-10 vehicles, green for 7 sec
    return 5000;                  // Less than 5 vehicles, green for 5 sec
}

void loop() {
    int radarSignal = digitalRead(RADAR_SENSOR_PIN);
    int humanSignal = digitalRead(HUMAN_SENSOR_PIN);
    int rfSignal = digitalRead(RF_RECEIVER_PIN);
    int motionSignal = digitalRead(MOTION_SENSOR_PIN);
    
    // Vehicle detection
    if (radarSignal == HIGH) {
        vehicleCount++;
        Serial.print("Vehicle detected! Count: ");
        Serial.println(vehicleCount);
        delay(1000); // Prevent multiple detections for the same vehicle
    }

    // Pedestrian detection (Emergency Stop)
    if (humanSignal == HIGH) {
        Serial.println("Pedestrian detected! Stopping traffic.");
        changeTrafficLight(RED_LIGHT_PIN, 5000);  // Stop traffic for 5 seconds
    }

    // Emergency vehicle detection
    if (rfSignal == HIGH) {
        Serial.println("Emergency vehicle detected! Giving priority.");
        changeTrafficLight(GREEN_LIGHT_PIN, 5000); // Green light for emergency vehicle
    }

    // Unexpected vehicle movement detection (Security Feature)
    if (motionSignal == HIGH) {
        Serial.println("Unexpected vehicle movement detected! Alert triggered.");
        changeTrafficLight(RED_LIGHT_PIN, 10000); // Stop all traffic for security
    }

    // Control traffic lights based on vehicle count if no emergency or pedestrian detected
    if (humanSignal == LOW && rfSignal == LOW && motionSignal == LOW) {
        int greenLightDuration = calculateGreenTime(vehicleCount);
        Serial.print("Green light duration: ");
        Serial.print(greenLightDuration / 1000);
        Serial.println(" seconds.");
        changeTrafficLight(GREEN_LIGHT_PIN, greenLightDuration);
    }

    delay(1000); // Adjust delay based on system response
}
