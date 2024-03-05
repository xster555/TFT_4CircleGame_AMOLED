#include "main.h"

TFT_eSPI tft = TFT_eSPI();

TFT_eSprite bg = TFT_eSprite(&tft);
TFT_eSprite inputRow = TFT_eSprite(&tft);
TFT_eSprite gameBoard = TFT_eSprite(&tft);
TFT_eSprite infoBoard = TFT_eSprite(&tft);

int gameData[6][7] = {
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0}};
int winnerPos[4][2] = {{0, 0}, {0, 1}, {0, 2}, {0, 3}};

OneButton upButton(UP_BTN, true);
OneButton downButton(DOWN_BTN, true);

int gameRound = 0;
int gameState = GS_PRE_GAME; // 0: 预备 1: 游戏中 2: 结束
int inputCol = 3;
bool isPlayer1 = true;
bool isTie = false;

void addLogo()
{
  infoBoard.fillRect(0, 160, infoBoard.width(), 2, TFT_DARKGREY);
  infoBoard.setTextSize(3);
  infoBoard.setTextDatum(TL_DATUM);
  infoBoard.drawString("Developed by", 10, 170);
  infoBoard.setTextSize(4);
  infoBoard.drawString("XSTER", 10, 202);

  infoBoard.pushToSprite(&bg, inputRow.width(), 0);
}

/***** 游戏相关的方法 *****/
// 展示游戏结束界面
void displayGameOver()
{
  inputRow.fillSprite(TFT_DARKGREEN);
  inputRow.pushToSprite(&bg, 0, 0);

  infoBoard.setTextColor(TFT_WHITE, TFT_BLACK);
  infoBoard.fillSprite(TFT_BLACK);
  infoBoard.setTextSize(3);
  infoBoard.setTextDatum(TC_DATUM);
  infoBoard.drawString("Game Over", infoBoard.width() / 2, 10);
  if (isTie)
  {
    infoBoard.drawString("TIE", infoBoard.width() / 2, 70);
  }
  else
  {
    infoBoard.drawString("WINNER", infoBoard.width() / 2, 70);
    if (isPlayer1)
      infoBoard.drawString("Player 1", infoBoard.width() / 2, 100);
    else
      infoBoard.drawString("Player 2", infoBoard.width() / 2, 100);
  }

  infoBoard.pushToSprite(&bg, inputRow.width(), 0);
  addLogo();
  lcd_PushColors(0, 0, WIDTH, HEIGHT, (uint16_t *)bg.getPointer());
}

// 展示游戏开始前界面
void displayPreGameStatus()
{
  infoBoard.fillSprite(TFT_WHITE);
  infoBoard.setTextColor(TFT_BLACK, TFT_WHITE);
  infoBoard.setTextSize(3);
  infoBoard.setTextDatum(TC_DATUM);
  infoBoard.drawString("Double tap", infoBoard.width() / 2, 30);
  infoBoard.drawString("to play", infoBoard.width() / 2, 80);

  infoBoard.pushToSprite(&bg, inputRow.width(), 0);
  addLogo();
  lcd_PushColors(0, 0, WIDTH, HEIGHT, (uint16_t *)bg.getPointer());
}

// 展示游戏信息界面
void displayGameStatus()
{
  infoBoard.fillSprite(TFT_WHITE);
  infoBoard.setTextColor(TFT_BLACK, TFT_WHITE);
  infoBoard.setTextDatum(TL_DATUM);
  infoBoard.setTextSize(3);
  infoBoard.drawString("Turn: ", 5, 5);

  if (isPlayer1)
    infoBoard.drawString("Player1", 5, 30);
  else
    infoBoard.drawString("Player2", 5, 30);

  // infoBoard.pushSprite(inputRow.width(), 0);
  infoBoard.pushToSprite(&bg, inputRow.width(), 0);
  addLogo();
  lcd_PushColors(0, 0, WIDTH, HEIGHT, (uint16_t *)bg.getPointer());
}

// 展示所选列已满提示界面
void displayColFull()
{
  infoBoard.fillSprite(TFT_WHITE);
  infoBoard.setTextSize(3);
  infoBoard.drawString("Column full!!!", 5, 5);

  infoBoard.drawString("Try again", 5, 30);

  // infoBoard.pushSprite(inputRow.width(), 0);
  infoBoard.pushToSprite(&bg, inputRow.width(), 0);
  addLogo();
  lcd_PushColors(0, 0, WIDTH, HEIGHT, (uint16_t *)bg.getPointer());

  delay(800);
  displayGameStatus();
}

// 更新游戏面板
void updateBoard()
{
  Serial.println("Updating board");

  gameBoard.fillSprite(TFT_LIGHTGREY);

  // Draw the horizontal and vertical grid
  for (int i = 0; i < 6; i++)
  {
    gameBoard.drawFastHLine(0, CELL_SIZE * i, CELL_SIZE * 7, TFT_BLACK);
  }
  for (int i = 0; i < 7; i++)
  {
    gameBoard.drawFastVLine(CELL_SIZE * i, 0, CELL_SIZE * 6, TFT_BLACK);
  }

  // Draw the pieces
  for (int row = 0; row < 6; row++)
  {
    for (int col = 0; col < 7; col++)
    {
      if (gameData[row][col] == 1)
        gameBoard.fillCircle(CELL_SIZE / 2 * (col + 1) + (CELL_SIZE / 2 + 1) * col,
                             CELL_SIZE / 2 * (row + 1) + (CELL_SIZE / 2 + 1) * row,
                             CELL_SIZE / 2,
                             TFT_RED);
      else if (gameData[row][col] == 2)
        gameBoard.fillCircle(CELL_SIZE / 2 * (col + 1) + (CELL_SIZE / 2 + 1) * col,
                             CELL_SIZE / 2 * (row + 1) + (CELL_SIZE / 2 + 1) * row,
                             CELL_SIZE / 2,
                             TFT_YELLOW);
      else if (gameData[row][col] == 3)
        gameBoard.fillCircle(CELL_SIZE / 2 * (col + 1) + (CELL_SIZE / 2 + 1) * col,
                             CELL_SIZE / 2 * (row + 1) + (CELL_SIZE / 2 + 1) * row,
                             CELL_SIZE / 2,
                             TFT_WHITE);
    }
  }

  // gameBoard.pushSprite(0, inputRow.height());
  gameBoard.pushToSprite(&bg, 0, inputRow.height());
  lcd_PushColors(0, 0, WIDTH, HEIGHT, (uint16_t *)bg.getPointer());
}

// 更新输入行
void updateInputRow()
{
  Serial.println("Updating input row");
  inputRow.fillSprite(TFT_DARKGREEN);

  if (isPlayer1)
    inputRow.fillCircle(CELL_SIZE / 2 * (inputCol + 1) + (CELL_SIZE / 2 + 1) * inputCol, inputRow.height() / 2, inputRow.height() / 2, TFT_RED);
  else
    inputRow.fillCircle(CELL_SIZE / 2 * (inputCol + 1) + (CELL_SIZE / 2 + 1) * inputCol, inputRow.height() / 2, inputRow.height() / 2, TFT_YELLOW);

  // inputRow.pushSprite(0, 0);
  // bg.fillSprite(TFT_BLACK);
  Serial.println("Pushing sprite...");
  inputRow.pushToSprite(&bg, 0, 0);
  Serial.println("Pushing lcd...");
  lcd_PushColors(0, 0, WIDTH, HEIGHT, (uint16_t *)bg.getPointer());
  Serial.println("Pushing lcd...Done");
}

// 清空游戏面板
void cleanGameBoard()
{
  inputCol = 3;
  isPlayer1 = true;
  gameState = GS_PRE_GAME;
  gameRound = 0;
  isTie = false;

  for (int row = 0; row < 6; row++)
  {
    for (int col = 0; col < 7; col++)
    {
      gameData[row][col] = 0;
    }
  }
  updateInputRow();
  updateBoard();
  displayPreGameStatus();
}

// 上按键点击事件，左移棋子
void upClick()
{
  if (gameState == GS_GAME_OVER)
    return;

  inputCol--;
  if (inputCol < 0)
  {
    inputCol = 6;
  }

  updateInputRow();
}

// 下按键点击事件，右移棋子
void downClick()
{
  if (gameState == GS_GAME_OVER)
    return;

  inputCol++;
  if (inputCol > 6)
  {
    inputCol = 0;
  }

  updateInputRow();
}

// 检测是否胜利
bool checkForWin()
{
  // Check for win
  for (int row = 0; row < 6; row++)
  {
    for (int col = 0; col < 7; col++)
    {
      if (gameData[row][col] != 0)
      {
        // Check horizontal
        if (col < 4)
        {
          if (gameData[row][col] == gameData[row][col + 1] && gameData[row][col] == gameData[row][col + 2] && gameData[row][col] == gameData[row][col + 3])
          {
            // save winner pos
            winnerPos[0][0] = row;
            winnerPos[0][1] = col;
            winnerPos[1][0] = row;
            winnerPos[1][1] = col + 1;
            winnerPos[2][0] = row;
            winnerPos[2][1] = col + 2;
            winnerPos[3][0] = row;
            winnerPos[3][1] = col + 3;

            return true;
          }
        }

        // Check vertical
        if (row < 3)
        {
          if (gameData[row][col] == gameData[row + 1][col] && gameData[row][col] == gameData[row + 2][col] && gameData[row][col] == gameData[row + 3][col])
          {
            // save winner pos
            winnerPos[0][0] = row;
            winnerPos[0][1] = col;
            winnerPos[1][0] = row + 1;
            winnerPos[1][1] = col;
            winnerPos[2][0] = row + 2;
            winnerPos[2][1] = col;
            winnerPos[3][0] = row + 3;
            winnerPos[3][1] = col;

            return true;
          }
        }

        // Check diagonal
        if (row < 3 && col < 4)
        {
          if (gameData[row][col] == gameData[row + 1][col + 1] && gameData[row][col] == gameData[row + 2][col + 2] && gameData[row][col] == gameData[row + 3][col + 3])
          {
            // save winner pos
            winnerPos[0][0] = row;
            winnerPos[0][1] = col;
            winnerPos[1][0] = row + 1;
            winnerPos[1][1] = col + 1;
            winnerPos[2][0] = row + 2;
            winnerPos[2][1] = col + 2;
            winnerPos[3][0] = row + 3;
            winnerPos[3][1] = col + 3;

            return true;
          }
        }

        if (row < 3 && col > 2)
        {
          if (gameData[row][col] == gameData[row + 1][col - 1] && gameData[row][col] == gameData[row + 2][col - 2] && gameData[row][col] == gameData[row + 3][col - 3])
          {
            // save winner pos
            winnerPos[0][0] = row;
            winnerPos[0][1] = col;
            winnerPos[1][0] = row + 1;
            winnerPos[1][1] = col - 1;
            winnerPos[2][0] = row + 2;
            winnerPos[2][1] = col - 2;
            winnerPos[3][0] = row + 3;
            winnerPos[3][1] = col - 3;

            return true;
          }
        }
      }
    }
  }

  return false;
}

// 棋子落下
void confirmPlay()
{
  if (gameState == GS_PRE_GAME)
    gameState = GS_PLAYING;
  else if (gameState == GS_GAME_OVER)
    return;

  bool colFull = true;
  for (int row = 5; row >= 0; row--)
  {
    if (gameData[row][inputCol] == 0)
    {
      if (isPlayer1)
        gameData[row][inputCol] = 1;
      else
        gameData[row][inputCol] = 2;

      colFull = false;
      break;
    }
  }

  if (colFull)
  {
    displayColFull();
    return;
  }

  updateBoard();

  bool winnerFound = checkForWin();

  if (winnerFound) // 发现有赢家，结束
  {
    gameState = GS_GAME_OVER; // 结束游戏
    displayGameOver();
  }
  else // 未发现赢家，继续游戏
  {
    isPlayer1 = !isPlayer1;
    displayGameStatus();
    updateInputRow();
    gameRound++;
    if (gameRound >= 42)
    {
      gameState = GS_GAME_OVER;
      isTie = true;
    }
  }
}

// 初始化按键
void setupGameButtons()
{
  upButton.attachClick(upClick);                 // 左移棋子
  upButton.attachLongPressStart(cleanGameBoard); // 重置游戏
  upButton.setLongPressIntervalMs(800);          // 设置长按时间

  downButton.attachClick(downClick);         // 右移棋子
  downButton.attachDoubleClick(confirmPlay); // 释放棋子
}

// 初始化游戏
void setupGame()
{
  gameBoard.createSprite(CELL_SIZE * 7, CELL_SIZE * 6);
  inputRow.createSprite(CELL_SIZE * 7, HEIGHT - gameBoard.height());
  infoBoard.createSprite(WIDTH - gameBoard.width(), HEIGHT);

  updateInputRow();
  updateBoard();
}

/***** 游戏相关的方法（end) *****/

void setup()
{
  Serial.begin(115200);

  setupGameButtons();

  rm67162_init();
  lcd_setRotation(3);

  bg.createSprite(WIDTH, HEIGHT);
  bg.fillSprite(TFT_BLACK);
  bg.setSwapBytes(1);

  setupGame();

  displayPreGameStatus();
}

void loop()
{
  upButton.tick();
  downButton.tick();

  if (gameState == GS_GAME_OVER)
  {

    displayGameOver();
    if (!isTie)
    {
      // 提示赢家棋子位置
      for (int i = 0; i < 4; i++)
      {
        gameData[winnerPos[i][0]][winnerPos[i][1]] = 3;
        updateBoard();
        delay(100);
        if (isPlayer1)
        {
          gameData[winnerPos[i][0]][winnerPos[i][1]] = 1;
        }
        else
        {
          gameData[winnerPos[i][0]][winnerPos[i][1]] = 2;
        }
        updateBoard();
        delay(100);
      }
    }
  }
}