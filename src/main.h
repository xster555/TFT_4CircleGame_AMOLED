#include <Arduino.h>
#include "rm67162.h"
#include "TFT_eSPI.h"
#include <OneButton.h>
#include <SPI.h>

#define CELL_SIZE 35

#define UP_BTN 0
#define DOWN_BTN 21

#define WIDTH 536
#define HEIGHT 240

// Game states
#define GS_PRE_GAME 0
#define GS_PLAYING 1
#define GS_GAME_OVER 2

/***** 游戏相关的方法 *****/
// 展示游戏结束界面
void displayGameOver();
// 展示游戏开始前界面
void displayPreGameStatus();
// 展示游戏信息界面
void displayGameStatus();
// 展示所选列已满提示界面
void displayColFull();
// 更新游戏面板
void updateBoard();
// 更新输入行
void updateInputRow();
// 清空游戏面板
void cleanGameBoard();
// 上按键点击事件，左移棋子
void upClick();
// 下按键点击事件，右移棋子
void downClick();
// 检测是否胜利
bool checkForWin();
// 棋子落下
void confirmPlay();
// 初始化按键
void setupGameButtons();
// 初始化游戏
void setupGame();
/***** 游戏相关的方法（end) *****/

void checkPair();