#include <ESP32CAN.h>
#include <CAN_config.h>

CAN_device_t CAN_cfg;

int system_flag = 0;

//GPOI핀번호에 해당하는 변수 설정
const int red1 = 19;
const int green1 = 23;
const int blue1 = 22;

const int red2 = 33;
const int green2 = 25;
const int blue2 = 32;
const int pw2 = 26;

const int red3 = 16;
const int green3 = 36;
const int blue3 = 39;
const int pw3 = 18;

const int red4 = 12;
const int green4 = 14;
const int blue4 = 13;
const int pw4 = 27;

const int sub = 15;
const int ledChannel = 0;
const int freq = 50;
const int resolution = 16;
int deg, duty;
//GPOI핀번호에 해당하는 변수 설정

//FreeRTOS사용을 위한 핸들러 생성
TaskHandle_t emerTaskHandle = NULL;       //비상등 전용 핸들러 생성
TaskHandle_t leftTurnTaskHandle = NULL;   //좌회전 전용 핸들러 생성
TaskHandle_t rightTurnTaskHandle = NULL;  //우회전 전용 핸들러 생성
TaskHandle_t wiperTaskHandle = NULL;      //와이퍼 전용 핸들러 생성
//FreeRTOS사용을 위한 핸들러 생성

//now1과 now2는 can통신으로 받은 실행 명령어를 2중으로 확인하기 위한 변수
int now1, now2;

//열거형(enum)타입 정의 >> 차량의 신호 상태 표현
enum SignalState {
    NONE,
    EMER,
    LEFT_TURN,
    RIGHT_TURN
};
//열거형(enum)타입 정의

//열거형 사용을 위한 변수 선언
SignalState currentSignalState = NONE;

//stopCurrentSignal은 비상등,우회전,좌회전이 서로 동작하지 않도록 하기위한
//후입력 우선순위 인터록 느낌
//비상등이 동작중 일때 좌회전을 동작시키고 싶으면 비상등을 종료한 후 좌회전 실행
void stopCurrentSignal() {
    if (currentSignalState == EMER && emerTaskHandle != NULL) {
        vTaskDelete(emerTaskHandle);
        emerTaskHandle = NULL;
    } else if (currentSignalState == LEFT_TURN && leftTurnTaskHandle != NULL) {
        vTaskDelete(leftTurnTaskHandle);
        leftTurnTaskHandle = NULL;
    } else if (currentSignalState == RIGHT_TURN && rightTurnTaskHandle != NULL) {
        vTaskDelete(rightTurnTaskHandle);
        rightTurnTaskHandle = NULL;
    }
    currentSignalState = NONE;
}

//can통신 수신하는 부분
void receive_CAN(void *pvParameters) {
    (void) pvParameters;
    CAN_frame_t rx_frame;
    //can관련 FreeRTOS를 사용하기 때문에 계속해서 CAN메시지 수신
    while (true) {
      //CAN통신이 수신된다면 나에게 온건지 확인 후 명령 수행
        if (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, portMAX_DELAY) == pdTRUE) {
            if (rx_frame.FIR.B.RTR != CAN_RTR) {
                if (rx_frame.MsgID == 0x02) {
                  //추가된 부분은 now1과 now2로 쓰레드 동작시 다른 함수가 동작 되지 않도록 함.
                  now1 = rx_frame.data.u8[0];
                  now2 = rx_frame.data.u8[1];
                    // 0은 앞 LED, 1은 뒤 LED, 2는 서보모터
                    //rx_frame.data.u8[0]이 0이면 상향등, 1이면 비상등, 2이면 좌회전, 3이면 우회전, 4면 와이퍼
                  if (rx_frame.data.u8[0] == 0) { //상향등
                    //상향등 on
                    if (rx_frame.data.u8[1] == 1) {
                      digitalWrite(red1, HIGH);
                      digitalWrite(green1, HIGH);
                      digitalWrite(blue1, HIGH);
                      digitalWrite(pw2, HIGH);
                      } 
                    //상향등 off
                    else if (rx_frame.data.u8[1] == 0) {
                      digitalWrite(red1, LOW);
                      digitalWrite(green1, LOW);
                      digitalWrite(blue1, LOW);
                      digitalWrite(pw2, LOW);
                    }
                  }

                  else if (rx_frame.data.u8[0] == 1) { // 비상등
                    if (rx_frame.data.u8[1] == 0 && emerTaskHandle != NULL) { //비상등 off
                      vTaskDelete(emerTaskHandle); // off시 생성된 쓰레드 종료 
                      emerTaskHandle = NULL;       // 생성되었던 쓰레드를 비워줌
                      digitalWrite(pw3, LOW);
                      digitalWrite(pw4, LOW);
                    }
                    else if (rx_frame.data.u8[1] == 1 && emerTaskHandle == NULL) { // 비상등 on
                      stopCurrentSignal();        //이전에 동작하고 있는 쓰레드가 있으면 종료
                      currentSignalState = EMER;  //현재 동작중인 쓰레드는 EMER이라는걸 작성
                      //쓰레드 생성
                      xTaskCreate(emerLights, "EMER", 2048, NULL, 1, &emerTaskHandle);
                    }
                  } 
                  //쓰레드 관련 이하 동문
                  else if (rx_frame.data.u8[0] == 2) { // 좌회전
                    if(rx_frame.data.u8[1] == 1 && leftTurnTaskHandle == NULL){ //좌회전 on
                      stopCurrentSignal();
                      currentSignalState = LEFT_TURN;
                      xTaskCreate(leftTurnSignal, "Left Turn", 2048, NULL, 1, &leftTurnTaskHandle);
                    }
                      //좌회전 off
                    else if(rx_frame.data.u8[1] == 0 && leftTurnTaskHandle != NULL){
                        vTaskDelete(leftTurnTaskHandle);
                        leftTurnTaskHandle = NULL;
                        digitalWrite(pw4, LOW);
                      }
                  } 

                  else if (rx_frame.data.u8[0] == 3) { // 우회전
                      if(rx_frame.data.u8[1] == 1){ //우회전 on
                      stopCurrentSignal();
                        if (rightTurnTaskHandle == NULL) {
                          currentSignalState = RIGHT_TURN;
                          xTaskCreate(rightTurnSignal, "Right Turn", 2048, NULL, 1, &rightTurnTaskHandle);
                        }
                      }
                      //우회전 off
                      else if(rx_frame.data.u8[1] == 0 && rightTurnTaskHandle != NULL){
                         vTaskDelete(rightTurnTaskHandle);
                         rightTurnTaskHandle = NULL;
                         digitalWrite(pw3, LOW);
                      }
                    }       
                  else if (rx_frame.data.u8[0] == 4) { // 와이퍼
                    if (rx_frame.data.u8[1] == 1) { //와이퍼 on
                      if (wiperTaskHandle == NULL) {
                        xTaskCreate(wiperFunction, "Wiper", 2048, NULL, 1, &wiperTaskHandle);
                      }
                    }
                    else if (rx_frame.data.u8[1] == 0) {//와이퍼 off
                      if (wiperTaskHandle != NULL) {
                        vTaskDelete(wiperTaskHandle);
                        wiperTaskHandle = NULL;
                      }
                    }
                  }
                }
            }
        }
    }
}
//비상등 실행 함수
void emerLights(void *pvParameters) {
    while (true) {
      //can통신으로 수신한 명령어가 1,0이 아니면 실행 취소
      if(now1 != 1 && now2 == 0){
        break;
      }
        digitalWrite(pw3, HIGH);
        digitalWrite(pw4, HIGH);
        delay(1000);
        digitalWrite(pw3, LOW);
        digitalWrite(pw4, LOW);
        delay(1000);
    }
}
//좌회전 실행 함수
void leftTurnSignal(void *pvParameters) {
    while (true) {
      //can통신으로 수신한 명령어가 2,1이 아니면 실행 취소
      if(now1 != 2 && now2 == 1){
        break;
      }
      digitalWrite(pw4, HIGH);
      delay(1000);
      digitalWrite(pw4, LOW);
      delay(1000);
    }
}
//우회전 실행 함수
void rightTurnSignal(void *pvParameters) {
    while (true) {
      //can통신으로 수신한 명령어가 3,1이 아니면 실행 취소
      if(now1 != 3 && now2 == 1){
      break;
      }
      digitalWrite(pw3, HIGH);
      delay(1000);
      digitalWrite(pw3, LOW);
      delay(1000);
    }
}
//서보모터 동작 함수
void servoWrite(int ch, int deg) {
    duty = map(deg, 0, 180, 1638, 8192);
    ledcWrite(ch, duty);
    delay(15); // delay를 줄이면 180도가 완전히 돌지 않음
}
//서보모터 동작 함수
void wiperFunction(void *pvParameters) {
    while (true) {
        for (deg = 0; deg <= 180; deg++) {
            servoWrite(ledChannel, deg);
        }
        for (deg = 180; deg >= 0; deg--) {
            servoWrite(ledChannel, deg);
        }
    }
}



void setup() {
    //시리얼 통신을 시작
    Serial.begin(115200);

    //can통신의 기본 설정 셋팅
    CAN_cfg.speed = CAN_SPEED_1000KBPS;
    CAN_cfg.tx_pin_id = GPIO_NUM_5;
    CAN_cfg.rx_pin_id = GPIO_NUM_4;
    CAN_cfg.rx_queue = xQueueCreate(10, sizeof(CAN_frame_t));
    ESP32Can.CANInit();

    //서보모터 PWM제어를 위한 설정
    ledcSetup(ledChannel, freq, resolution); 
    ledcAttachPin(sub, ledChannel);          

    // 핀 모드 설정
    pinMode(red1, OUTPUT);
    pinMode(green1, OUTPUT);
    pinMode(blue1, OUTPUT);

    pinMode(red2, OUTPUT);
    pinMode(green2, OUTPUT);
    pinMode(blue2, OUTPUT);
    pinMode(pw2, OUTPUT);

    pinMode(red3, OUTPUT);
    pinMode(green3, OUTPUT);
    pinMode(blue3, OUTPUT);
    pinMode(pw3, OUTPUT);

    pinMode(red4, OUTPUT);
    pinMode(green4, OUTPUT);
    pinMode(blue4, OUTPUT);
    pinMode(pw4, OUTPUT);

    digitalWrite(red1, LOW);
    digitalWrite(green1, LOW);
    digitalWrite(blue1, LOW);

    analogWrite(red2, 0);
    analogWrite(green2, 0);
    analogWrite(blue2, 0);
    digitalWrite(pw2, LOW);

    analogWrite(red3, 0);
    analogWrite(green3, 0);
    analogWrite(blue3, 0);
    digitalWrite(pw3, LOW);

    analogWrite(red4, 0);
    analogWrite(green4, 128);
    analogWrite(blue4, 255);
    digitalWrite(pw4, LOW);

    //can통신용 FreeRTOS 설정
    xTaskCreate(receive_CAN, "CAN Receiver", 4096, NULL, 1, NULL);
}

void loop() {
    // 메인 루프는 비워둠. 모든 작업은 태스크에서 처리됨.
}