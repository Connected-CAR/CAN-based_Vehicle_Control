//센서부 >> 메인

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ESP32CAN.h>
#include <CAN_config.h>

#define DHTPIN 13
#define DHTTYPE DHT11
typedef struct {
	float temperature;
	float humidity;
} DHT_VALUE;

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;
CAN_device_t CAN_cfg;

const int pt_sensor = 36;
// DHT11 센서로부터 온습도 값 읽는 함수
void read_sensor(DHT_VALUE* dht_value, int Print) {
	// Delay between measurements.
	delay(delayMS);
	// Get temperature event and print its value.
	sensors_event_t event;
	dht.temperature().getEvent(&event);
	if (isnan(event.temperature)) {
		Serial.println(F("Error reading temperature!"));
		dht_value->temperature = -1;
	}
	else {
		dht_value->temperature = event.temperature;
	}
	// Get humidity event and print its value.
	dht.humidity().getEvent(&event);
	if (isnan(event.relative_humidity)) {
		Serial.println(F("Error reading humidity!"));
		dht_value->humidity = -1;
	}
	else {
		dht_value->humidity = event.relative_humidity;
	}
}
// CAN 통신으로 센서 값을 전달하는 함수
void send_sensor(DHT_VALUE dht_value, int pt_val, int Print) {
	CAN_frame_t frame;
	frame.FIR.B.FF = CAN_frame_std;
	frame.MsgID = 1;
	frame.FIR.B.DLC = 8;

	frame.data.u8[0] = (dht_value.temperature + 0.5);
	frame.data.u8[1] = ((int)(dht_value.temperature * 100 + 0.5)) % 100;
	frame.data.u8[2] = (dht_value.humidity + 0.5);
	frame.data.u8[3] = ((int)(dht_value.humidity * 100 + 0.5)) % 100;
	frame.data.u8[4] = pt_val / 1000;
  frame.data.u8[5] = pt_val / 100 - frame.data.u8[4] * 10;
  frame.data.u8[6] = pt_val / 10 - frame.data.u8[4] * 100 - frame.data.u8[5] * 10;
  frame.data.u8[7] = pt_val - frame.data.u8[4] * 1000 - frame.data.u8[5] * 100 - frame.data.u8[6]*10;
	
  if (Print == 1) {
		Serial.print(F("Transmit Data : \n"));
		Serial.print(F("---Tempuerature: "));
		Serial.print(frame.data.u8[0]);
		Serial.print(F("."));
		Serial.print(frame.data.u8[1]);
		Serial.println(F("C"));
		Serial.print(F("---Humidity: "));
		Serial.print(frame.data.u8[2]);
		Serial.print(F("."));
		Serial.print(frame.data.u8[3]);
		Serial.println(F("%"));
		Serial.print(F("---Photo Resistor: "));
		Serial.println(pt_val);
	}

	ESP32Can.CANWriteFrame(&frame);
}

void setup() {
	Serial.begin(115200);
	dht.begin();
	sensor_t sensor;
	dht.temperature().getSensor(&sensor);
	dht.humidity().getSensor(&sensor);
	delayMS = sensor.min_delay / 1000;

	CAN_cfg.speed = CAN_SPEED_1000KBPS;
	CAN_cfg.tx_pin_id = GPIO_NUM_5;
	CAN_cfg.rx_pin_id = GPIO_NUM_4;
	CAN_cfg.rx_queue = xQueueCreate(10, sizeof(CAN_frame_t));
	ESP32Can.CANInit();
}

void loop() {
	DHT_VALUE dht_value;
	float pt_val = analogRead(pt_sensor) / 4095.0 * 100;
	read_sensor(&dht_value, 1);
	send_sensor(dht_value, pt_val, 1);
}
