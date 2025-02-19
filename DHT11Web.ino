#include <WiFi.h>      // ใช้สำหรับ ESP8266
#include <WebServer.h>  // ใช้สำหรับ ESP8266
#include "DHT.h"

// กำหนดค่าคงที่สำหรับเซ็นเซอร์และ LED
#define DHTPIN 5       // ขา GPIO สำหรับเซ็นเซอร์ DHT11
#define DHTTYPE DHT11  // ประเภทของเซ็นเซอร์ DHT11
#define LED_PIN 4      // ขา GPIO สำหรับควบคุม LED

// กำหนดข้อมูล WiFi
const char* ssid = "NNNNN";   // เปลี่ยนเป็นชื่อ WiFi ของคุณ
const char* password = "12345678"; // เปลี่ยนเป็นรหัสผ่าน WiFi ของคุณ
s
// สร้างออบเจ็กต์สำหรับเซ็นเซอร์ DHT11
DHT dht(DHTPIN, DHTTYPE);

// สร้างเว็บเซิร์ฟเวอร์ที่ใช้พอร์ต 80
WebServer server(80);

void setup() {
    // เริ่มต้นการสื่อสารผ่าน Serial Monitor
    Serial.begin(115200);
    
    // เริ่มต้นการเชื่อมต่อ WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi!");

    // เริ่มต้นการทำงานของเซ็นเซอร์ DHT11
    dht.begin();

    // กำหนดขา LED เป็นขา OUTPUT
    pinMode(LED_PIN, OUTPUT);

    // ตั้งค่าเส้นทางสำหรับการให้บริการข้อมูลบนเว็บ
    server.on("/", HTTP_GET, [](){
        // อ่านค่าความชื้นและอุณหภูมิจากเซ็นเซอร์
        float humidity = dht.readHumidity();
        float temperature = dht.readTemperature();

        // ตรวจสอบว่าการอ่านค่าล้มเหลวหรือไม่
        if (isnan(humidity) || isnan(temperature)) {
            server.send(200, "text/plain", "Failed to read from DHT sensor!");
            return;
        }

        // สร้าง HTML สำหรับแสดงผล
        String html = "<html><body>";
        html += "<h1>ESP8266 DHT11 Sensor Data</h1>";
        html += "<p>Humidity: " + String(humidity) + "%</p>";
        html += "<p>Temperature: " + String(temperature) + "°C</p>";
        
        // เพิ่มปุ่มควบคุม LED
        html += "<form method='get' action='/toggleLED'>";
        html += "<input type='submit' value='Toggle LED'>";
        html += "</form>";
        
        html += "</body></html>";
        
        // ส่งข้อมูล HTML กลับไปยังเบราว์เซอร์
        server.send(200, "text/html", html);
    });

    // ตั้งค่าการทำงานสำหรับการควบคุม LED
    server.on("/toggleLED", HTTP_GET, [](){
        // สลับสถานะ LED
        static bool ledState = LOW;
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState ? HIGH : LOW);

        // ส่งผู้ใช้กลับไปที่หน้าแรก
        server.sendHeader("Location", "/");
        server.send(303, "text/plain", "");
    });

    // เริ่มต้นเว็บเซิร์ฟเวอร์
    server.begin();
}

void loop() {
    // ทำงานเว็บเซิร์ฟเวอร์
    server.handleClient();
}
