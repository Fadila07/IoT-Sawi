#include <DHT.h>
#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <PubSubClient.h>

#define DHTTYPE DHT11
uint8_t pinDHT = 4;
DHT dht(pinDHT, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2);

const uint8_t moistPin = A0;

//WiFi
const char* ssid = "Redmi 9";
const char* pass = "01012020";

//MQTT
const char* mqtt_server = "ec2-54-236-230-11.compute-1.amazonaws.com"; //IP of the MQTT broker
const char* humTopic = "sawi/humidity";
const char* temTopic = "sawi/temperature";
const char* moisTopic = "sawi/moisture";
const char* mqtt_username = "frtris";
const char* mqtt_password = "123456";
const char* clientID = "client_sawi";

//Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;

//1883 is the listener port of the Broker
PubSubClient client(wifiClient);
long lastMsg = 0;
int value = 0;

byte derajat[] = {
  B01110,
  B01010,
  B01010,
  B01110,
  B00000,
  B00000,
  B00000,
  B00000
};

//

void setup_wifi(){
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void reconnect(){
  while(!client.connected()){
    Serial.print("Attempting MQTT Connection...");
    //Attempt to connect
    if(client.connect(clientID)){
      Serial.println("connected");
      //Once connected, publish an announcement
    } else{
      Serial.print("Failed, rc= ");
      Serial.println(client.state());
      Serial.println("Try Again in 2 seconds");
      delay(2000);
    }
  }
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(pinDHT, INPUT);
  dht.begin();

  setup_wifi();
  client.setServer(mqtt_server, 1883);

  lcd.init(); //Inisialisasi LCD
  lcd.backlight();
  lcd.clear();
  lcd.createChar(0, derajat);
  lcd.setCursor(0, 0);
  lcd.print("    WELCOME     ");
  lcd.setCursor(0, 1);
  lcd.print("  IoT Polinema  ");
  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(!client.connected()){
    reconnect();
  }
  client.loop();

  int h = dht.readHumidity();
  int t = dht.readTemperature();
  float mois = 100.00 - ((analogRead(moistPin) / 1023.00) * 100.00);
  
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println("C");
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println("%");
  Serial.print("Moisture: ");
  Serial.print(mois);
  Serial.println("%");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T: " +String((int)t));
  lcd.write(0);
  lcd.print("C H:" +String((int)h) +"%");
  lcd.setCursor(0, 1);
  lcd.print("Moist: " +String((float)mois) +"%");
  delay(2000);

  long now = millis();
  if(now - lastMsg > 1000*60){
    lastMsg = now;
    ++value;

    if (client.publish(temTopic, String(t).c_str())) {
    Serial.println("Temperature sent!");
    } 
    // Again, client.publish will return a boolean value depending on whether it succeded or not.
      // If the message failed to send, we will try again, as the connection may have broken.
    else {
      Serial.println("Temperature failed to send. Reconnecting to MQTT Broker and trying again");
      client.connect(clientID);
      delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
      client.publish(temTopic, String(t).c_str());
    }

    // PUBLISH to the MQTT Broker (topic = Humidity, defined at the beginning)
    if (client.publish(humTopic, String(h).c_str())) {
      Serial.println("Humidity sent!");
    }
    // Again, client.publish will return a boolean value depending on whether it succeded or not.
    // If the message failed to send, we will try again, as the connection may have broken.
    else {
      Serial.println("Humidity failed to send. Reconnecting to MQTT Broker and trying again");
      client.connect(clientID);
      delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
      client.publish(humTopic, String(h).c_str());
    }

    // PUBLISH to the MQTT Broker (topic = Kelembaban Tanah, defined at the beginning)
    if (client.publish(moisTopic, String(mois).c_str())) {
      Serial.println("Moisture sent!");
    }
    // Again, client.publish will return a boolean value depending on whether it succeded or not.
    // If the message failed to send, we will try again, as the connection may have broken.
    else {
      Serial.println("Moisture failed to send. Reconnecting to MQTT Broker and trying again");
      client.connect(clientID);
      delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
      client.publish(moisTopic, String(t).c_str());
    }
  }
}