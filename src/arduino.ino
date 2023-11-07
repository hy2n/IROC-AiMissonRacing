/* 2023 Ai Racing
 * By Donghyun / https://donghyun.cc
 * with SCPU 1.02b https://github.com/hy2n/SCPU
*/

//Ai Camera   **다이나믹셀 라이브러리하고 출돌해서 먼저 선언 필수**

#include <Pixy2.h>
Pixy2 pixy;
//=========================================================//
//SCPU

//키값 - CM530 통신

//Stop
const uint8_t moveStop[6] = { 0xff, 0x55, 0x00, 0xff, 0x00, 0xff };
const uint8_t* moveStop_ptr = moveStop;
//U: Forward
const uint8_t moveForward[6] = { 0xff, 0x55, 0x01, 0xfe, 0x00, 0xff };
const uint8_t* moveForward_ptr = moveForward;
//D: Backward
const uint8_t moveBackward[6] = { 0xff, 0x55, 0x02, 0xfd, 0x00, 0xff };
const uint8_t* moveBackward_ptr = moveBackward;
//L: TurnLeft
const uint8_t moveTurnLeft[6] = { 0xff, 0x55, 0x04, 0xfb, 0x00, 0xff };
const uint8_t* moveTurnLeft_ptr = moveTurnLeft;
//R: TurnRight
const uint8_t moveTrunRight[6] = { 0xff, 0x55, 0x08, 0xf7, 0x00, 0xff };
const uint8_t* moveTrunRight_ptr = moveTrunRight;
//U+L: Forward & TurnLeft
const uint8_t moveForwardLeft[6] = { 0xff, 0x55, 0x09, 0xf6, 0x00, 0xff };
const uint8_t* moveForwardLeft_ptr = moveForwardLeft;
//D+L: Backward & TurnLeft
const uint8_t moveBackwardLeft[6] = { 0xff, 0x55, 0x06, 0xf9, 0x00, 0xff };
const uint8_t* kick = moveBackwardLeft;
//U+R: Forward & TurnRight
const uint8_t moveForwardRithg[6] = { 0xff, 0x55, 0x05, 0xfa, 0x00, 0xff };
const uint8_t* moveForwardRithg_ptr = moveForwardRithg;


//Block 1
int X_root_D = 1;
int ObjectnotDected = 1;
int Line_not_Dected = 1;
int Line_mid = 1;
int Line_big_no = 1;



//Line
int Line_select = 0;                                //선택된라인
int Line_count = 0;                                 //감지라인
int Line_X[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };  //init
int Line_Y[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int Line_Height[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int Line_Width[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int X_LR = 0;    //왼쪽오른쪽구분
int X_root = 0;  //양모서리거리값
int Color[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//=========================================================//

#include <OLLO.h>
OLLO myOLLO;
int BT_status = 0;  //버튼

int block_dec = 0;

void setup() {
  Serial.begin(115200);  //PC
  Serial2.begin(57600);  //로봇 통신

  Serial.println("Ai-Car Sofware Begin..");
  delay(100);
  pixy.init();
}


void loop() {
  //ButtonWait();  //버튼대기
  while (1) {
    Serial2.write(moveForward_ptr, 6);
    delay(1000);

    block_dec = 0;
    while (ObjectnotDected) linetrace(); //물건 감지될때까지 직진
    delay(1000);
    Serial2.write(kick, 6);
    delay(5000);

    while (1) linetrace_k(); //물건 감지후 저속 보행
  }
}

void linetrace() {
  while (Line_mid) {
    ScanBlock();                                              //Line ID번의 객체 스캔
    if (Line_count > 0) {                                     // 1개 이상의 객체가 발견돼면
      if (Line_count == 1) Line_select = AdvancedDefine();  //2개 발견하면 일반 Define해서 선택
      else Line_select = AdvancedDefine();                    //3개이상 발견하면 고급 Define해서 선택
    } else {
      Line_select = 0;  //1개 발견시 그거로 선택
    }
    if (!(block_dec == 1)) {
      X_range_define(Line_select);
      Serial.println(X_root);
      //DebugLine(Line_select);
      if (X_root > 70) {
        if (X_LR == 1) {  //왼쪽에 있으므로 오른쪽으로 회전
          Serial2.write(moveStop_ptr, 6);
          delay(2000);
          Serial2.write(moveTurnLeft_ptr, 6);
          delay(150);
          Serial2.write(moveStop_ptr, 6);
          delay(150);
        } else {  //오른쪽에 있으므로 왼쪽으로 회전
          Serial2.write(moveStop_ptr, 6);
          delay(2000);
          Serial2.write(moveTrunRight_ptr, 6);
          delay(150);
          Serial2.write(moveStop_ptr, 6);
          delay(150);
        }

      } else {
        Line_mid = 0;
        Serial2.write(moveForward_ptr, 6);
      }
      delay(50);
    } else {
      Line_mid = 0;
      block_dec = 0;//초기화
      Serial2.write(moveStop_ptr, 6);
      delay(50);
      ObjectnotDected = 0;
    }
  }
  Line_mid = 1;
}


void linetrace_k() {
  while (Line_mid) {
    ScanBlock();                                              //Line ID번의 객체 스캔
    if (Line_count > 0) {                                     // 1개 이상의 객체가 발견돼면
      if (Line_count == 1) Line_select = AdvancedDefine();  //2개 발견하면 일반 Define해서 선택
      else Line_select = AdvancedDefine();                    //3개이상 발견하면 고급 Define해서 선택
    } else {
      Line_select = 0;  //1개 발견시 그거로 선택
    }
    if (!(block_dec == 1)) {
      X_range_define(Line_select);
      Serial.println(X_root);
      //DebugLine(Line_select);
      if (X_root > 100) {
        if (X_LR == 1) {  //왼쪽에 있으므로 오른쪽으로 회전
          Serial2.write(moveStop_ptr, 6);
          delay(2000);
          Serial2.write(moveTurnLeft_ptr, 6);
          delay(150);
          Serial2.write(moveStop_ptr, 6);
          delay(150);
        } else {  //오른쪽에 있으므로 왼쪽으로 회전
          Serial2.write(moveStop_ptr, 6);
          delay(2000);
          Serial2.write(moveTrunRight_ptr, 6);
          delay(150);
          Serial2.write(moveStop_ptr, 6);
          delay(150);
        }

      } else {
        Line_mid = 0;
        Serial2.write(moveForward_ptr, 6);
      }
      delay(50);
    } else {
      Line_mid = 0;
      block_dec = 0;//초기화
      Serial2.write(moveStop_ptr, 6);
      delay(50);
      ObjectnotDected = 0;
    }
  }
  Line_mid = 1;
}


void ScanBlock() {
  int i;
  pixy.ccc.getBlocks();  //Scan Blocks
  if (pixy.ccc.numBlocks) {
    Serial.print("Detected ");
    Serial.println(pixy.ccc.numBlocks);

    for (i = 0; i < pixy.ccc.numBlocks; i++) {
    if (!(pixy.ccc.blocks[i].m_height < 10)) {
    if (pixy.ccc.blocks[i].m_signature == 2) {
      block_dec = 1;
    }
    pixy.ccc.blocks[i].print();
    Line_X[i] = pixy.ccc.blocks[i].m_x;  //변수에 값 저장
    Line_Y[i] = pixy.ccc.blocks[i].m_y;
    Line_Width[i] = pixy.ccc.blocks[i].m_width;
    Line_Height[i] = pixy.ccc.blocks[i].m_height;
    Color[i] = pixy.ccc.blocks[i].m_signature;
    }
    
    }
  } else {
    Line_count = 0;
  }
}

int DefineObject(int object1, int object2) {
  if (((Line_Width[object1] < 50) || (Line_Width[object2] < 50)) && ((Line_Height[object1] < 60) || (Line_Height[object2] < 60))) {
  } else if (Line_Y[object1] < Line_Y[object2]) return object1;
  else if (Line_Y[object1] > Line_Y[object2]) return object2;
}

int DebugLine(int object1) {
  Serial.println(String() + F("Dected Objects : ") + Line_count + F(", Selected Object : ") + Line_select);
  Serial.println(String() + F("Object info: X=") + Line_X[1] + F(", Y=") + Line_Y[1]);
  Serial.println(String() + F("Object info: X=") + Line_X[2] + F(", Y=") + Line_Y[2]);
}

int AdvancedDefine() {  //가장 큰 오브젝트로 선택한다
  int k = 0;
  int max = 1;
  int maxID = 1;
  while (k < (Line_count + 1)) {
    if (Line_X[k] > 160) {
      X_root_D = 320 - Line_X[k];
    }  // 오른쪽 축 계산
    else {
      X_root_D = Line_X[k];
    }  //왼쪽 축 계산
    if ((Line_Width[k] < 50) && (Line_Height[k] < 30)) { //너무 작은거 인식 안해요

    } else if (Line_Width[k] > max) {
      max = Line_Width[k];
      maxID = k;
    }
    k++;
  }
  return maxID;
}

int X_range_define(int objID) {
  if (Line_X[objID] > 160) {
    X_root = Line_X[objID] - 160;
    X_LR = 2;
  }  // 오른쪽 축 계산
  else {
    X_root = 160 - Line_X[objID];
    X_LR = 1;
  }  //왼쪽 축 계산
}
