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

/***** 游戏相关的方法 *****/
void displayGameOver();

// 更新信息面板
void displayGameStatus(bool doPush);
void displayErrorState(bool doPush);

void updateBoard();
void updateInputRow();
void cleanGameBoard();
void createGame();
void upClick();
void downClick();
bool checkForWin();
void confirmPlay();
void processPlay();
void setupGameButtons();
/***** 游戏相关的方法（end) *****/

void checkPair();