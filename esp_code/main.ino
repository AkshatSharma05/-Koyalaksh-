#include <GyverOLED.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
 
GyverOLED<SSH1106_128x64> oled;
 
const char* ssid = "Vansh";
const char* password = "12345678";
const char* serverName = "http://192.168.58.178:5000/data";
 
// Sensor value variables
int methane = 0;
int carbon_monoxide = 0;
float temperature = 0.0;
float humidity = 0.0;
 
// Define DHT sensor
#define DHTPIN 4         // D4 pin
#define DHTTYPE DHT22    // or DHT22, depending on your sensor
DHT dht(DHTPIN, DHTTYPE);
 
// Initialize the OLED display
const uint8_t bitmap_32x32[] PROGMEM = {
  // (Bitmap data here)
};
 
void setup() {
  Serial.begin(115200);
 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
 
  oled.init();  
  oled.clear();  
  oled.update();  
 
  // Initialize DHT sensor
  dht.begin();
 
  oled.setCursor(1, 4);   
  oled.setScale(2);
  oled.print("xSENTINALx");
  oled.update();
  delay(2000);
  oled.clear();
  oled.update();
}
 
void loop() {
  // Read values from sensors
  methane = analogRead(34);  // Read methane from VP pin
  carbon_monoxide = analogRead(35); // Read carbon monoxide from VN pin
  temperature = dht.readTemperature(); // Read temperature
  humidity = dht.readHumidity(); // Read humidity
 
  // Check if any reads failed and exit early (to try again).
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
 
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
 
    // Prepare JSON data
    String jsonData = String("{\"miner_id\":\"Miner 4\", \"methane\":") + carbon_monoxide +
                      String(", \"carbon_monoxide\":") + methane +
                      String(", \"temperature\":") + temperature +
                      String(", \"humidity\":") + humidity + "}";
 
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");
 
    int httpResponseCode = http.POST(jsonData);
 
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.println("Error on sending POST");
    }
    http.end();
  }
 
  // Update OLED display
  oled.setCursor(50, 2);
  oled.setScale(1);
  oled.print("STATS");
  oled.setCursor(1, 4);
  oled.setScale(1);
  oled.print("Temp: ");
  oled.print(temperature);
  oled.print("C");
  oled.setCursor(1, 6);
  oled.print("Hum: ");
  oled.print(humidity);
  oled.print("%");
  oled.setCursor(70, 4);
  oled.print("CH4: ");
  oled.print(carbon_monoxide);  
  oled.setCursor(70, 6);
  oled.print("CO: ");
  oled.print(methane);  
  oled.update();
 
  delay(1000); // Send data every second
}

