#include <esp_now.h>
#include <WiFi.h>
#include <Ticker.h>

#define INTERVAL_HELLO 3000
#define INTERVAL_BEAT  10000

bool registered = false;
Ticker helloTicker;
Ticker beatTicker;

void sendHello();
void sendHeartbeat();

void onDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  String msg = String((char*)incomingData);
  if (msg == "ACK") {
    registered = true;
    helloTicker.detach(); // stop HELLO
    beatTicker.attach_ms(INTERVAL_BEAT, sendHeartbeat); // start heartbeat
    Serial.println("Registered with coordinator");
  }
}

void sendHello() {
  if (!registered) {
    esp_now_send(NULL, (uint8_t*)"HELLO", 5); // broadcast
    Serial.println("Sending HELLO");
  }
}

void sendHeartbeat() {
  esp_now_send(NULL, (uint8_t*)"BEAT", 4);
  Serial.println("Sending BEAT");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(onDataRecv);
  helloTicker.attach_ms(INTERVAL_HELLO, sendHello);
}
