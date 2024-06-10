/* 2023 Ai Racing
 * By Donghyun / https://donghyun.cc
 * with SCPU 1.02b https://github.com/hy2n/SCPU
*/

#include <Pixy2.h>
#include <OLLO.h>

Pixy2 pixy;
OLLO myOLLO;

// SCPU 키 값 (CM530 통신)
const uint8_t moveCommands[][6] = {
  {0xff, 0x55, 0x00, 0xff, 0x00, 0xff}, // Stop
  {0xff, 0x55, 0x01, 0xfe, 0x00, 0xff}, // Forward
  {0xff, 0x55, 0x02, 0xfd, 0x00, 0xff}, // Backward
  {0xff, 0x55, 0x04, 0xfb, 0x00, 0xff}, // TurnLeft
  {0xff, 0x55, 0x08, 0xf7, 0x00, 0xff}, // TurnRight
  {0xff, 0x55, 0x09, 0xf6, 0x00, 0xff}, // Forward & TurnLeft
  {0xff, 0x55, 0x06, 0xf9, 0x00, 0xff}, // Backward & TurnLeft (Kick)
  {0xff, 0x55, 0x05, 0xfa, 0x00, 0xff}  // Forward & TurnRight
};

enum MoveCommand {
  STOP, FORWARD, BACKWARD, TURN_LEFT, TURN_RIGHT, FORWARD_LEFT, KICK, FORWARD_RIGHT
};

// 전역 변수 (초기화)
int objectNotDetected = 1;
int lineMid = 1;
int lineSelect = 0;
int lineCount = 0;
int lineX[10] = {0};
int lineY[10] = {0};
int lineWidth[10] = {0};
int lineHeight[10] = {0};
int xLR = 0;
int xRoot = 0;
int color[10] = {0};
int blockDec = 0;
int btStatus = 0;

void setup() {
  Serial.begin(115200);
  Serial2.begin(57600);
  pixy.init();
}

void loop() {
  while (1) {
    Serial2.write(moveCommands[FORWARD], 6);
    delay(1000);

    blockDec = 0;
    while (objectNotDetected) lineTrace();
    delay(1000);
    Serial2.write(moveCommands[KICK], 6);
    delay(5000);

    while (1) lineTraceSlow();
  }
}

void lineTrace() {
  while (lineMid) {
    scanBlock();
    if (lineCount > 0) {
      lineSelect = (lineCount == 1) ? advancedDefine() : advancedDefine();
    }

    if (blockDec != 1) {
      xRangeDefine(lineSelect);
      if (xRoot > 70) {
        Serial2.write(moveCommands[STOP], 6);
        delay(2000);
        Serial2.write(moveCommands[(xLR == 1) ? TURN_LEFT : TURN_RIGHT], 6);
        delay(150);
        Serial2.write(moveCommands[STOP], 6);
        delay(150);
      } else {
        lineMid = 0;
        Serial2.write(moveCommands[FORWARD], 6);
      }
    } else {
      lineMid = 0;
      blockDec = 0;
      Serial2.write(moveCommands[STOP], 6);
      objectNotDetected = 0;
    }
    delay(50);
  }
  lineMid = 1;
}

void lineTraceSlow() {
  while (lineMid) {
    scanBlock();
    if (lineCount > 0) {
      lineSelect = (lineCount == 1) ? advancedDefine() : advancedDefine();
    }

    if (blockDec != 1) {
      xRangeDefine(lineSelect);
      if (xRoot > 100) {
        Serial2.write(moveCommands[STOP], 6);
        delay(2000);
        Serial2.write(moveCommands[(xLR == 1) ? TURN_LEFT : TURN_RIGHT], 6);
        delay(150);
        Serial2.write(moveCommands[STOP], 6);
        delay(150);
      } else {
        lineMid = 0;
        Serial2.write(moveCommands[FORWARD], 6);
      }
    } else {
      lineMid = 0;
      blockDec = 0;
      Serial2.write(moveCommands[STOP], 6);
      objectNotDetected = 0;
    }
    delay(50);
  }
  lineMid = 1;
}

void scanBlock() {
  lineCount = 0;
  blockDec = 0;

  pixy.ccc.getBlocks();
  for (int i = 0; i < pixy.ccc.numBlocks; i++) {
    if (pixy.ccc.blocks[i].m_height >= 10) {
      if (pixy.ccc.blocks[i].m_signature == 2) {
        blockDec = 1;
      }
      lineX[lineCount] = pixy.ccc.blocks[i].m_x;
      lineY[lineCount] = pixy.ccc.blocks[i].m_y;
      lineWidth[lineCount] = pixy.ccc.blocks[i].m_width;
      lineHeight[lineCount] = pixy.ccc.blocks[i].m_height;
      color[lineCount] = pixy.ccc.blocks[i].m_signature;
      lineCount++;
    }
  }
}

int defineObject(int object1, int object2) {
  if (lineWidth[object1] < 50 || lineWidth[object2] < 50 || lineHeight[object1] < 60 || lineHeight[object2] < 60) {
    return -1; // Too small objects
  } else {
    return (lineY[object1] < lineY[object2]) ? object1 : object2;
  }
}

void debugLine(int object1) {
  Serial.println("Detected Objects: " + String(lineCount) + ", Selected Object: " + String(lineSelect));
  for (int i = 0; i < lineCount; i++) {
    Serial.println("Object " + String(i + 1) + ": X=" + String(lineX[i]) + ", Y=" + String(lineY[i]));
  }
}

int advancedDefine() {
  int maxIndex = 0;
  int maxWidth = 0;
  for (int k = 0; k < lineCount; k++) {
    if ((lineWidth[k] >= 50) && (lineHeight[k] >= 30) && (lineWidth[k] > maxWidth)) {
      maxWidth = lineWidth[k];
      maxIndex = k;
    }
  }
  return maxIndex;
}

void xRangeDefine(int objID) {
  xRoot = abs(160 - lineX[objID]);
  xLR = (lineX[objID] > 160) ? 2 : 1;
}
