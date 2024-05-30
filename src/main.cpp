#include "ESP8266WiFi.h"
#include <PubSubClient.h>

// Ganti dengan kredensial Wi-Fi Anda
const char *ssid = "WifiBawaan";
const char *password = "mifi1234";

const char *mqtt_broker = "broker.emqx.io"; // client id : mqttx_9468fc85
const char *topic = "coba/soundbox";
const char *mqtt_username = "soundbox";
const char *mqtt_password = "11111111";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
  Serial.begin(115200);

  setup_wifi();

  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  reconnect();
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  // serial monitor
  if (Serial.available() > 0)
  {
    //  input dari Serial Monitor
    String input = Serial.readStringUntil('\n');

    input.trim();

    Serial.print("Mengirim nilai transaksi: ");
    Serial.println(input);

    // Mengirim trx ke MQTT
    client.publish(topic, input.c_str());
  }
}

void setup_wifi()
{
  delay(10);
  // Memulai koneksi Wi-Fi
  Serial.println();
  Serial.print("Menghubungkan ke ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi terhubung");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Menghubungkan ke MQTT...");

    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password))
    {
      Serial.println("Terhubung");
      // Berlangganan ke topik
      client.subscribe(topic);
    }
    else
    {
      Serial.print("Gagal, rc=");
      Serial.print(client.state());
      Serial.println(" mencoba lagi dalam 5 detik");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Pesan diterima di topik [");
  Serial.print(topic);
  Serial.print("]: ");
  
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
