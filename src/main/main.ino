/*
  Arduino UNO + OLED 串口终端显示器
  功能：读取串口数据，实时滚动显示在 OLED 屏幕上
 */
#include <Arduino.h>
#include <U8g2lib.h>
#define BUZZER  5
#define PIN_DOWN  8
#define PIN_UP  7
#define PIN_OK  4

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

#define SCREEN_ROWS     6
#define SCREEN_COLS     21     // 每行最大字符数（根据字体调整）
#define MAX_LINE_LEN    SCREEN_COLS

// 行缓冲区：存储当前屏幕上的所有行文本
char displayBuffer[SCREEN_ROWS][MAX_LINE_LEN + 1];

// 当前正在构建的行（尚未换行）
char currentLine[MAX_LINE_LEN + 1];
uint8_t currentLineLen = 0;

unsigned long previousMillis = 0;
const long interval = 100;
bool refreshPaused = false;
unsigned long lastBtnTime = 0;
const unsigned long debounceDelay = 50;
int lastBtnState = HIGH;

// 标记是否需要刷新屏幕
bool needRefresh = false;

void addCharToDisplay(char c);
void scrollUp(void);
void renderDisplay(void);
void processSerialData(void);
void pause(void);

void setup() {
    Serial.begin(115200);
    
    pinMode(PIN_DOWN, INPUT);
    pinMode(PIN_UP, INPUT);
    pinMode(BUZZER, OUTPUT);
    pinMode(PIN_OK, INPUT);
    lastBtnState = digitalRead(PIN_OK);

    u8g2.begin();
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setFontPosTop();              // 文字从顶部开始绘制
    
    // 清空显示缓冲区
    for (uint8_t row = 0; row < SCREEN_ROWS; row++) {
        displayBuffer[row][0] = '\0';
    }
    currentLine[0] = '\0';
    currentLineLen = 0;

    // 显示启动信息
    strcpy(displayBuffer[0], "--- Serial Terminal ---");
    strcpy(displayBuffer[1], "Ready...");
    renderDisplay();
    
}

void loop() {
    unsigned long currentMillis = millis();
    static unsigned long startPressed3 = 0;   
    bool button3 = false;

    // 1. 处理所有可用的串口数据
    processSerialData();
    rollUp();
    rollDown();
    pause();
    
    // 2. 如果有数据更新，且处于刷新状态时,刷新屏幕
    if (!refreshPaused && needRefresh) {
        renderDisplay();
        needRefresh = false;
    }

    if(digitalRead(PIN_OK) == LOW){
    unsigned long timeReleased3 = millis() - startPressed3;
    if(timeReleased3 < 1000 && timeReleased3 > 10){
      button3 = true;
      if(button3 == true){
      needRefresh = true;
      button3 = false;
      tone(BUZZER, 4000);
      startPressed3 = millis() - 1000;
      }
    }else{
      button3 = false;
    }
  }else{
     startPressed3 = millis();
  }

    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        noTone(BUZZER);
    }
    //微小延时，用以充分利用cpu资源
    delay(1);
}

//处理串口数据
void processSerialData() {
    while (Serial.available() > 0) {
        char c = Serial.read();
        
        // 处理换行符
        if (c == '\n' || c == '\r') {
            // 如果当前行有内容，或者收到的是单独的换行符（空行）
            if (currentLineLen > 0 || c == '\n') {
                // 将当前行加入显示缓冲区（滚动）
                scrollUp();
                strcpy(displayBuffer[SCREEN_ROWS - 1], currentLine);
                // 清空当前行
                currentLine[0] = '\0';
                currentLineLen = 0;
                needRefresh = true;
            }
            // 如果是 \r，忽略（不产生空行）
            // 如果是 \n，且当前行为空，产生一个空行
            if (c == '\n' && currentLineLen == 0) {

            }
        } else {
            // 普通字符：追加到当前行
            addCharToDisplay(c);
        }
    }
}

//添加字符到当前行
void addCharToDisplay(char c) {
    // 忽略不可打印字符（除了空格）
    if (c < 32 && c != ' ') return;
    
    // 如果当前行已满，直接换行
    if (currentLineLen >= MAX_LINE_LEN) {
        // 将当前行加入显示缓冲区
        scrollUp();
        strcpy(displayBuffer[SCREEN_ROWS - 1], currentLine);
        currentLine[0] = '\0';
        currentLineLen = 0;
        needRefresh = true;
    }
    
    // 追加字符
    currentLine[currentLineLen] = c;
    currentLineLen++;
    currentLine[currentLineLen] = '\0';
    needRefresh = true;
}

//屏幕滚动（所有行上移一行）
void scrollUp() {
    // 将第 1~5 行移动到第 0~4 行
    for (int row = 0; row < SCREEN_ROWS - 1; row++) {
        strcpy(displayBuffer[row], displayBuffer[row + 1]);
    }
    // 最后一行清空
    displayBuffer[SCREEN_ROWS - 1][0] = '\0';
}

//屏幕滚动（所有行下移一行）
void scrollDown() {
  // 将第 0~4 行移动到第 1~5 行
    for (int row = SCREEN_ROWS - 2; row >= 0; row--) {
        strcpy(displayBuffer[row + 1], displayBuffer[row]);
    }
    displayBuffer[0][0] = '\0';
}

//渲染 OLED 屏幕
void renderDisplay() {
    u8g2.clearBuffer();  // 清空
    
    //绘制
    for (uint8_t row = 0; row < SCREEN_ROWS; row++) {
        // 计算 Y 坐标：行号 * 字体高度(10)
        uint8_t y = row * 10;
        u8g2.drawStr(0, y, displayBuffer[row]);
    }
    
    u8g2.sendBuffer();  // 将内容发送到屏幕
}

//手动上移显示
void rollUp() {
   static unsigned long startPressed2 = 0;   
   bool button2 = false;
  if(digitalRead(PIN_UP) == LOW){
    unsigned long timeReleased2 = millis() - startPressed2;
    if(timeReleased2 < 1000 && timeReleased2 > 10){
      button2 = true;
      if(button2 == true){
      scrollUp();
      currentLine[0] = '\0';
      needRefresh = true;
      button2 = false;
      tone(BUZZER, 4000);
      startPressed2 = millis() - 1000;
      }
    }else{
      button2 = false;
    }
  }else{
     startPressed2 = millis();
  } 
}

//手动下移显示
void rollDown() {
   static unsigned long startPressed1 = 0;    
   bool button1 = false;
  if(digitalRead(PIN_DOWN) == LOW){
    unsigned long timeReleased1 = millis() - startPressed1;
    if(timeReleased1 < 1000 && timeReleased1 > 10){
      button1 = true;
      if(button1 == true){
      scrollDown();
      currentLine[0] = '\0';
      needRefresh = true;
      button1 = false;
      tone(BUZZER, 4000);
      startPressed1 = millis() - 1000;
      }
    }else{
      button1 = false;
    }
  }else{
     startPressed1 = millis();
  } 
}

void pause() {
    int currentState = digitalRead(PIN_OK);
    
    // 检测下降沿（从 HIGH 到 LOW，即按钮按下瞬间）
    if (currentState == LOW && lastBtnState == HIGH) {
        // 消抖：确保距离上次触发超过 debounceDelay 毫秒
        if (millis() - lastBtnTime > debounceDelay) {
            refreshPaused = !refreshPaused;   // 切换暂停状态
            lastBtnTime = millis();

            //if (refreshPaused) {
                //Serial.println(">> Screen refresh PAUSED");
            //} else {
                //Serial.println(">> Screen refresh RESUMED");
                // 恢复时立即刷新一次，显示最新数据
                needRefresh = true;
            }
        }
      lastBtnState = currentState;
    }