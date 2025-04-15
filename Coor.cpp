#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <esp_now.h>
#include <WiFi.h>
#include <Encoder.h>

// Pin untuk rotary encoder dan tombol
#define ENCODER_PIN_A 32
#define ENCODER_PIN_B 33
#define BUTTON_PIN     34

Adafruit_SSD1306 display(128, 64, &Wire, -1);

struct NodeInfo {
  uint8_t mac[6];
  unsigned long lastSeen;
  String status;
};

NodeInfo nodes[MAX_NODES];
int nodeCount = 0;
int currentNodeIndex = 0;

Encoder enc(ENCODER_PIN_A, ENCODER_PIN_B);

void onDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  String msg = String((char*)incomingData);
  int idx = findNode(mac);
  
  if (msg == "HELLO") {
    if (idx == -1) addNode(mac);
    esp_now_send(mac, (uint8_t*)"ACK", 3);
  } else if (msg == "BEAT" && idx != -1) {
    nodes[idx].lastSeen = millis();
    nodes[idx].status = "Active";
  }
}

int findNode(const uint8_t* mac) {
  for (int i = 0; i < nodeCount; i++) {
    if (macEquals(nodes[i].mac, mac)) return i;
  }
  return -1;
}

bool macEquals(const uint8_t* a, const uint8_t* b) {
  for (int i = 0; i < 6; i++) if (a[i] != b[i]) return false;
  return true;
}

void addNode(const uint8_t* mac) {
  if (nodeCount >= MAX_NODES) return;
  memcpy(nodes[nodeCount].mac, mac, 6);
  nodes[nodeCount].status = "Pending";
  nodes[nodeCount].lastSeen = millis();
  nodeCount++;
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }
  esp_now_register_recv_cb(onDataRecv);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
}

void loop() {
  display.clearDisplay();

  // Tampilkan daftar node yang terdaftar
  display.setCursor(0, 0);
  display.print("Node List:");

  for (int i = 0; i < nodeCount; i++) {
    if (i == currentNodeIndex) {
      display.setTextColor(BLACK, WHITE);  // Highlight node yang dipilih
    } else {
      display.setTextColor(WHITE);
    }
    
    display.setCursor(0, 10 + i * 10);
    display.print("Node ");
    display.print(i + 1);
    display.print(": ");
    display.print(nodes[i].status);
  }

  // Periksa perubahan encoder untuk navigasi
  long position = enc.read();
  if (position != currentNodeIndex) {
    currentNodeIndex = position % nodeCount;
    if (currentNodeIndex < 0) currentNodeIndex = nodeCount - 1;
  }

  // Tampilkan status saat ini
  display.setTextColor(WHITE);
  display.setCursor(0, 10 + nodeCount * 10);
  display.print("Selected Node: ");
  display.print(currentNodeIndex + 1);
  
  // Tombol tekan untuk mengaktifkan perintah atau melihat detail
  if (digitalRead(BUTTON_PIN) == LOW) {
    sendCommandToNode(currentNodeIndex);
    delay(500); // debounce
  }

  display.display();
  delay(100);
}

void sendCommandToNode(int nodeIndex) {
  // Kirim perintah ke node yang dipilih
  String command = "Command to Node " + String(nodeIndex + 1);
  esp_now_send(nodes[nodeIndex].mac, (uint8_t*)command.c_str(), command.length());
  Serial.println("Sent command to node");
}
