// Osgar Sullivan
// 29/03/2025
// For Esp32 in radon detection

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char* ssid = "ssid";
const char* password = "password";

WebServer server(80);

const int led = 13;
int inputPinAmp = 2;
int inputPinComp = 5;
int ampValues = 0;
int compValues = 0;
int ampCount = 0;
int compCount = 0;

void handleRoot() {
  server.send ( 200, "text/html", getPage() );
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}


void setup() {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println("Network");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  // delay(2);//allow the cpu to switch to other tasks
  // digitalWrite(led, 1);
  // delay(1000);
  // ampValues = analogRead(inputPinAmp);
  compValues = analogRead(inputPinComp);
  // Serial.println(ampValues);
  // delay(1000);
  // digitalWrite(led,0);
  // delay(1000);
  // digitalWrite(LEDPIN, HIGH);
  Serial.println(WiFi.macAddress());
  uint8_t mac[8];
  Serial.println(esp_read_mac(mac,ESP_MAC_WIFI_STA));
  // delay(9000);
  
  
  // Convert the ADC value to voltage (0-3.3V)
  // float voltage = compValues * (3.3 / 4095.0);

  // Print the voltage to the Serial Monitor
  // Serial.print("Voltage: ");
  // Serial.println(voltage, 3); // Print voltage with 3 decimal places

  // 
  // if (voltage > 1.20) {
  //   Serial.println("Pulse detected!");
  // }

  // 1000 approximately equal to 0.85 volts
  if (compValues < 1000) {
    compCount++;
    Serial.println("Comp");
    Serial.println(compCount);
    // digitalWrite(led, 0);
    // delay(100);
  }
  float bq = compCount/37037;
  // if (ampValues > 1200) {
  //   ampCount++;
  //   Serial.println("Amp");
  // }

  // digitalWrite(LEDPIN, LOW);
  // delay(3000);
}

void beginServer()
{
  server.on ( "/", handleRoot );
  server.begin();
  Serial.println ( "HTTP server started" );
}



String getPage(){
  String page = "<!DOCTYPE html>";
page += "<html>";
page += "<head>";
page += "<title>Radon Monitor</title>";
page += "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>";
page += "<style>";
page += "body { max-width: 1200px; margin: 0 auto; padding: 20px; }";
page += ".dashboard { display: flex; flex-direction: column; gap: 20px; }";
page += ".card { padding: 20px; border-radius: 5px; background: #f5f5f5; width: 100%; }";
page += ".chart-container { width: 70%; height: 50%; margin: 0 auto; }";
page += "canvas { width: 100% !important; height: auto !important; }";
page += "</style>";
page += "</head>";
page += "<body>";
page += "<div class='dashboard'>";
page += "<div class='card'>";
page += "<h2>Current Concentration: <span id='current-value'>"bq"</span> Bq/m³</h2>";
page += "</div>";
page += "<div class='chart-container'>";
page += "<canvas id='chart' width='400' height='200'></canvas>";
page += "</div>";
page += "</div>";
page += "<script>";
page += "const ctx = document.getElementById('chart').getContext('2d');";
page += "const chart = new Chart(ctx, {";
page += "type: 'line',";
page += "data: {";
page += "labels: ['Monday','Tuesday','Wednesday','Thursday','Friday','Saturday','Sunday'],";
page += "datasets: [{";
page += "label: 'Average Radon Concentration Past Seven Days (Bq/m³)',";
page += "data: [30,20,40,30,10,15,50,25],";
page += "borderColor: 'rgb(75, 192, 192)'";
page += "}]";
page += "},";
page += "options: {";
page += "responsive: true,";
page += "maintainAspectRatio: true";
page += "}";
page += "});";
page += "</script>";
page += "</body>";
page += "</html>";

  page += "</body></html>";
  return page;
}


