#include "secret.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <CAN_config.h>
#include <ESP32CAN.h>

// The MQTT topics that this device should publish/subscribe
#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

/* the variable name CAN_cfg is fixed, do not change */
CAN_device_t CAN_cfg;

int pt_flag = 2;
int hum_flag = 2;
int flag_time = 0;

typedef struct {
    float temperature;
    float humidity;
    int pt_sens;
    bool isSuccess;
} SENS_VAL;
// AWS IoT에 연결하는 작업 함수
void connectAWS()
{
	// WiFi 연결
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print("wifi connect error\n");
    // WiFi 연결 에러 떴을 때 디버깅 코드 (지워도 된다.)
    Serial.print(WIFI_SSID);
    Serial.print(" ");
    Serial.print(WIFI_PASSWORD);
    Serial.print("\n");
    Serial.print(WiFi.status());
    Serial.print(" ");
    Serial.print(WL_CONNECTED);
    Serial.print("\n");

  }

  // WiFi 연결 후 AWS IoT와 연결
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // 이전에 정의된 AWS EndPoint와 MQTT 연결
  client.begin(AWS_IOT_ENDPOINT, 8883, net);

  // 메시지 핸들러 생성
  client.onMessage(messageHandler);

  Serial.print("Connecting to AWS IOT");
	// AWS 연결 에러 관련 코드
  while (!client.connect(THINGNAME)) {
    Serial.print("aws iot connect error \n");
    delay(100);
  }
  // 일정 시간이 지나면 AWS와의 연결이 끊어짐
  if(!client.connected()){
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // MQTT Subscribe TOPIC 설정
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}

// 센서 연결 후 값 AWS로 전송하는 함수
void publishMessage(SENS_VAL sens_value)
{
	// doc 변수가 JSON으로 변환되어 전송됨
  StaticJsonDocument<200> doc;
  doc["temperature"] = sens_value.temperature;
  doc["humidity"] = sens_value.humidity;
  doc["light"] = sens_value.pt_sens;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void messageHandler(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
}

void send_con(int obj, int state1){
  //hum_flag;
  CAN_frame_t act_frame;
  act_frame.FIR.B.FF = CAN_frame_std;
  act_frame.MsgID = 2;
  act_frame.FIR.B.DLC = 8;
  act_frame.data.u8[0] = obj;
  act_frame.data.u8[1] = state1;
  ESP32Can.CANWriteFrame(&act_frame);
}

void send_hum(int hum){
  //hum_flag;
  CAN_frame_t act_frame;
  act_frame.FIR.B.FF = CAN_frame_std;
    act_frame.MsgID = 2;
    act_frame.FIR.B.DLC = 8;
    Serial.println(hum);
    if(hum_flag == 2 && hum >= 51){
      act_frame.data.u8[0] = 2;
      act_frame.data.u8[1] = 0;
      ESP32Can.CANWriteFrame(&act_frame);

}
else if(hum_flag == 2 && hum <= 51){
  act_frame.data.u8[0] = 2;
  act_frame.data.u8[1] = 1;
  ESP32Can.CANWriteFrame(&act_frame);
}
}

void send_pt(int pt_value) {
    //pt_flag
    CAN_frame_t act_frame;
    act_frame.FIR.B.FF = CAN_frame_std;
    act_frame.MsgID = 2;
    act_frame.FIR.B.DLC = 8;
    Serial.println(pt_value);
    if(pt_flag == 2 && pt_value <= 65){
      act_frame.data.u8[0] = 0;
      act_frame.data.u8[1] = 1;
      ESP32Can.CANWriteFrame(&act_frame);
    }
    else if(pt_flag == 2 && pt_value >= 65){
      act_frame.data.u8[0] = 0;
      act_frame.data.u8[1] = 0;
      ESP32Can.CANWriteFrame(&act_frame);
    }

}
// 센서부 eSP32에서 CAN 통신을 통해 데이터 값을 전달받음
// sens_val에 해당 값이 담긴다.
void receive_CAN(SENS_VAL* sens_val, int Print) {
    CAN_frame_t rx_frame;
    // CAN 통신이 진행중인지 확인
    if (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3 * portTICK_PERIOD_MS) == pdTRUE) { // 아무것도 없으면 printf만 진행
        if (Print == 1 && rx_frame.FIR.B.RTR == CAN_RTR) {
            printf("RTR from 0x%08x, DLC %d\r\n", rx_frame.MsgID, rx_frame.FIR.B.DLC);
        }// 아무것도 없고, print 설정도 안해놓으면 그냥 return
        else if (rx_frame.FIR.B.RTR == CAN_RTR) {
            return;
        }
        else {
            if (Print == 1) {
                printf("from 0x%08x, DLC %d\n", rx_frame.MsgID, rx_frame.FIR.B.DLC);
            }
				// MsgID가 1인 데이터를 전달 받았다면, sesns_val에 저장
        if (rx_frame.MsgID == 0x01) {
            if (Print == 1) {
                for (int i = 0; i < 5; i++) {
                    printf("%d\t", (char)rx_frame.data.u8[i]);
                }
            }
            flag_time++;
            float temperature = (rx_frame.data.u8[0] * 100 + rx_frame.data.u8[1]) / 100.0;
            float humidity = (rx_frame.data.u8[2] * 100 + rx_frame.data.u8[3]) / 100.0;
            char temp[5];
            int pt_val = rx_frame.data.u8[4]*1000 + rx_frame.data.u8[5]*100 + rx_frame.data.u8[6]*10 + rx_frame.data.u8[7] ;
            sens_val->temperature = temperature;
            sens_val->humidity = humidity;
            sens_val->pt_sens = pt_val;
            sens_val->isSuccess = true;
            send_pt(pt_val);
            int hum = humidity;
            send_hum(hum); // 습도가 일정값 이상이면 와이퍼(서보모터 동작)
            // 5번에 한 번씩 전달받은 센서 데이터를 AWS iIoT에 전달함
            if(flag_time == 5){
              publishMessage(*sens_val);
              client.loop();
              flag_time = 0;
              Serial.println("AWS IoT Get Data\n");
            }
        }
    }
}
}

void send_cmd_string(String cmd) {
  uint8_t readData;
  if (cmd == "go") {
    readData = SPI.transfer('w');
    Serial.println("go");
  } 
  else if (cmd == "back") {
    readData = SPI.transfer('x');
    Serial.println("back");
  }
  else if( cmd == "left"){
    readData = SPI.transfer('a');
    Serial.println("left");
  }
  else if( cmd == "right"){
    readData = SPI.transfer('d');
    Serial.println("right");
  }
  else if( cmd == "mid"){
    readData = SPI.transfer('s');
    Serial.println("mid");
  }
  else if( cmd == "stop"){
    readData = SPI.transfer('f');
    Serial.println("stop");
  }
  else if( cmd == "turn_on_left"){
    send_con(2,1);
    Serial.println("con_left_on");
  }
  else if( cmd == "turn_off_left"){
    send_con(2,0);
    Serial.println("con_left_off");
  }

  else if( cmd == "turn_on_right"){
    send_con(3,1);
    Serial.println("con_right_on");
  }
  else if( cmd == "turn_off_right"){
    send_con(3,0);
    Serial.println("con_right_off");
  }

  else if( cmd == "turn_on_emer"){
    send_con(1,1);
    Serial.println("con_emer_on");
  }
  else if( cmd == "turn_off_emer"){
    send_con(1,0);
    Serial.println("con_emer_off");
  }

  else if( cmd == "turn_on_high"){
    send_con(0,1);
    pt_flag = 1;
    Serial.println("con_high_on");
  }
  else if( cmd == "turn_off_high"){
    send_con(0,0);
    pt_flag = 2;
    Serial.println("con_high_off");
  }

  else if( cmd == "turn_on_wiper"){
    send_con(4,1);
    hum_flag = 1;
    Serial.println("con_wiper_on");
  }
  else if( cmd == "turn_off_wiper"){
    send_con(4,0);
    hum_flag = 2;
    Serial.println("con_wiper_off");
  }
}
// RCCAR 명령어 수행 후 MySQL DB에 완료했다는 신호를 보내는 함수
void send_finish_signal(int id) {
  HTTPClient http;
  if (id) {
    String url = "http://13.124.218.110:8080/rccar/finished/" + String(id);
    http.begin(url); // Change this URL to your local server's address
  int httpCode = http.GET();
  }
}
// MySQL DB로부터 RCCAR가 수행할 명령어를 전달받는 함수
void get_rccar_cmd() {
  HTTPClient http;
  http.begin("http://13.124.218.110:8080/rccar"); // Change this URL to your local server's address
  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();
    //Serial.println(payload); // Print response to serial monitor

const size_t capacity = JSON_OBJECT_SIZE(5) + JSON_ARRAY_SIZE(1) + 150;
DynamicJsonDocument doc(capacity);

DeserializationError error = deserializeJson(doc, payload);

if (error) {
  Serial.print(F("deserializeJson() failed: "));
  Serial.println(error.f_str());
  return;
}

JsonArray arr = doc.as<JsonArray>();
JsonObject obj = arr[0];
int id = obj["id"];
String cmd_string = obj["cmd_string"];
// Serial.print(F("Parsing CMD : "));
// Serial.println(cmd_string);

send_cmd_string(cmd_string);
send_finish_signal(id);
  } else {
    // Serial.println("Error on HTTP request");
  }
  http.end();
}

void setup() {
    Serial.begin(115200);
    // SPI 통신 설정
    SPI.begin(SCK, MISO, MOSI, SS);
    SPI.setFrequency(16000000);
    digitalWrite(SS, LOW);
		// CAN 통신 설정
    CAN_cfg.speed = CAN_SPEED_1000KBPS;
    CAN_cfg.tx_pin_id = GPIO_NUM_15;
    CAN_cfg.rx_pin_id = GPIO_NUM_4;
    CAN_cfg.rx_queue = xQueueCreate(10, sizeof(CAN_frame_t));
    ESP32Can.CANInit();

connectAWS();
}

void loop() {
		// CAN 통신으로 데이터 전달 받고, AWS IoT로 전달하기
    SENS_VAL sens_val = { -1, -1, -1, false };
    receive_CAN(&sens_val, 1);
    if (sens_val.isSuccess) {
        printf("temperature : %f, humidity : %f, photo : %d \n", sens_val.temperature, sens_val.humidity, sens_val.pt_sens);
    }
  // RCCAR 명령어 받아서 전달하기
  get_rccar_cmd();
}    