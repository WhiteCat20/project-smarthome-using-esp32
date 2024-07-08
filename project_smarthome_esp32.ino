#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include <WiFi.h>


// Define the servo
Servo myServo1;
Servo myServo2;

#define BLYNK_TEMPLATE_ID "TMPL6orp1mNsh"
#define BLYNK_TEMPLATE_NAME "Project Smarthome"
#define BLYNK_AUTH_TOKEN "_3qz2Dc1aS2OJQgoNof9P-dna4V2a0c6"

#include <BlynkSimpleEsp32.h>

char ssid[] = "Koding Akademi-2.4G";
char pass[] = "kodingq29";
char auth[] = BLYNK_AUTH_TOKEN;

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define the pin where the DHT sensor is connected
#define DHTPIN 4
#define DHTTYPE DHT11   // DHT 11
#define SOUND_SPEED 0.034

int LDR_PIN = 33;
int FAN_PIN = 5;
int LED_PIN = 26;
int SERVO1_PIN = 15;
int SERVO2_PIN = 32;
int TRIG_PIN = 14;
int ECHO_PIN = 27;
int FLAME_PIN = 25;
int BUZZER_PIN = 12;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Initialize serial communication at 115200 baud rate
  Serial.begin(9600);
  // Initialize the DHT sensor
  dht.begin();
  // Initialize the LCD
  lcd.init();
  // Turn on the backlight
  lcd.backlight();
  pinMode(LDR_PIN, INPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(FLAME_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  myServo1.attach(SERVO1_PIN);
  myServo2.attach(SERVO2_PIN);

  // WiFi Setup
  Blynk.begin(auth, ssid, pass,"blynk.cloud",80);
  // put your setup code here, to run once:
  WiFi.begin(ssid, pass); 
  Serial.print("Connecting to Wi-Fi"); 
  while (WiFi.status() != WL_CONNECTED) {
  Serial.print(".");
  delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: "); 
  Serial.println(WiFi.localIP());
  Serial.println();
}

// Gate
int pos = 0;


void dhtFunc(){
  // Read temperature as Celsius
  float t = dht.readTemperature();
  // Read humidity as percentage
  float h = dht.readHumidity();
    // Check if any reads failed and exit early (to try again)
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // Print the results to the serial monitor
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print("%  Temperature: ");
  Serial.print(t);
  Serial.println("°C");

  // Call lcdFunc with the read values
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V3, h);
  lcdFunc(t, h);
}

void lcdFunc(float t, float h){
  // Print a message to the LCD
 // Print temperature and humidity to the LCD
  lcd.setCursor(0, 0); // Set the cursor to column 0, line 1
  lcd.print("Temp: ");
  lcd.print(t);
  lcd.print(" C");
  lcd.setCursor(0, 1); // Set the cursor to column 0, line 2
  lcd.print("Humidity: ");
  lcd.print(h);
  lcd.print("%");
}

void servo1Func(){
  myServo1.write(0);
  delay(1000);
  myServo1.write(145);
  delay(1000);
}

float ultrasonicFunc(){
    // Define the ultrasonic sensor pins
  // Measure distance
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  // Set the TRIG_PIN on HIGH state for 10 micro seconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  // Read the ECHO_PIN, returns the sound wave travel time in microseconds
  long duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate the distance
  float distance = duration * SOUND_SPEED / 2;
  return distance;
}

int flamesensorFunc(){
  int flameState =  digitalRead(FLAME_PIN);
  return flameState;
}

void buzzerFunc(uint8_t val) {
  digitalWrite(BUZZER_PIN, val);
}

void loop() {
  Blynk.run();
  dhtFunc();
  int ldrValue = analogRead(LDR_PIN);
  Serial.print("LDR Value = ");
  Serial.println(ldrValue);
  if (ldrValue > 2500){
    digitalWrite(LED_PIN,HIGH);
  }else{
    digitalWrite(LED_PIN,LOW);
  }


  float distance = ultrasonicFunc();
  if (distance >= 20.0){
    myServo1.write(145);
  }else{
    myServo1.write(0);
  }
  int flame = flamesensorFunc();
  if (flame == 0){
    buzzerFunc(HIGH);
  }else{
    buzzerFunc(LOW);
  }
  Serial.print("Distamce = ");
  Serial.println(distance);

  int fireSensorReading = flamesensorFunc();
  int fireState = 0;
  if (fireSensorReading == 1){
    fireState = 0;
  }else{
    fireState = 1;
  }

  Blynk.virtualWrite(V1, fireState);
  Blynk.virtualWrite(V2, ldrValue);
  delay(1000); 

}

BLYNK_WRITE(V3) 
{
  int buttonState = param.asInt();
  if (buttonState == 1) {
    
    pos = 180;
  } else {
    
    pos = 120;
  }
  
  myServo2.write(pos);
}

BLYNK_WRITE(V4) 
{
  if(param.asInt() == 1)
  {
    // execute this code if the switch widget is now ON
    digitalWrite(FAN_PIN,HIGH);  
  }
  else
  {
    // execute this code if the switch widget is now OFF
    digitalWrite(FAN_PIN,LOW);  
  }
}












