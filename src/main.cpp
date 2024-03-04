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
int gameState = 0; // 0: 预备 1: 游戏中 2: 结束
int inputCol = 3;
bool isPlayer1 = true;

/***** 游戏相关的方法 *****/
void displayGameOver()
{
  inputRow.fillSprite(TFT_DARKGREEN);
  // inputRow.pushSprite(0, 0);
  lcd_PushColors(0, 0, WIDTH, HEIGHT, (uint16_t *)inputRow.getPointer());

  infoBoard.fillSprite(TFT_BLACK);
  infoBoard.setTextSize(2);
  infoBoard.setTextDatum(TC_DATUM);
  infoBoard.drawString("Game Over", infoBoard.width() / 2, 10);
  infoBoard.drawString("WINNER", infoBoard.width() / 2, 50);
  if (isPlayer1)
    infoBoard.drawString("Player 1", infoBoard.width() / 2, 70);
  else
    infoBoard.drawString("Player 2", infoBoard.width() / 2, 70);

  // infoBoard.pushSprite(inputRow.width(), 0);
  lcd_PushColors(inputRow.width(), 0, WIDTH, HEIGHT, (uint16_t *)inputRow.getPointer());
}

// 更新信息面板
void displayGameStatus(bool doPush)
{
  infoBoard.setTextSize(3);
  infoBoard.drawString("Turn: ", 5, 5);

  if (isPlayer1)
    infoBoard.drawString("Player1", 5, 30);
  else
    infoBoard.drawString("Player2", 5, 30);

  if (doPush)
  {
    // infoBoard.pushSprite(inputRow.width(), 0);
    infoBoard.pushToSprite(&bg, inputRow.width(), 0);
    lcd_PushColors(0, 0, WIDTH, HEIGHT, (uint16_t *)bg.getPointer());
  }
}

void displayErrorState(bool doPush)
{
  inputRow.fillSprite(TFT_DARKGREEN);
  // inputRow.pushSprite(0, 0);
  lcd_PushColors(0, 0, WIDTH, HEIGHT, (uint16_t *)inputRow.getPointer());

  infoBoard.fillSprite(TFT_BLACK);
  infoBoard.setTextSize(2);
  infoBoard.setTextDatum(TC_DATUM);
  infoBoard.drawString("ERROR", infoBoard.width() / 2, 10);
  infoBoard.drawString("ERROR", infoBoard.width() / 2, 30);
  infoBoard.drawString("ERROR", infoBoard.width() / 2, 50);

  // infoBoard.pushSprite(inputRow.width(), 0);
  lcd_PushColors(inputRow.width(), 0, WIDTH, HEIGHT, (uint16_t *)infoBoard.getPointer());
}

void updateBoard()
{
  Serial.println("Updating board");

  gameBoard.fillSprite(TFT_DARKGREY);

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

void cleanGameBoard()
{
  inputCol = 0;
  isPlayer1 = true;
  gameState = 0;

  for (int row = 0; row < 6; row++)
  {
    for (int col = 0; col < 7; col++)
    {
      gameData[row][col] = 0;
    }
  }
  updateInputRow();
  updateBoard();
}

void createGame()
{
  gameBoard.createSprite(CELL_SIZE * 7, CELL_SIZE * 6);
  inputRow.createSprite(CELL_SIZE * 7, HEIGHT - gameBoard.height());

  // gameBoard.setRotation(3);
  infoBoard.createSprite(WIDTH - gameBoard.width(), HEIGHT);
  // infoBoard.setRotation(3);

  updateInputRow();
  updateBoard();
}

void upClick()
{
  Serial.println("Up click");
  Serial.println(gameState);
  if (gameState == 2)
    return;

  inputCol--;
  if (inputCol < 0)
  {
    inputCol = 6;
  }

  updateInputRow();
}

void downClick()
{
  Serial.println("Down click");
  Serial.println(gameState);
  if (gameState == 2)
    return;

  inputCol++;
  if (inputCol > 6)
  {
    inputCol = 0;
  }

  updateInputRow();
}

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

void confirmPlay()
{
  if (gameState == 0)
    gameState = 1;

  gameRound++;

  if (gameState == 2)
    return;

  for (int row = 5; row >= 0; row--)
  {
    if (gameData[row][inputCol] == 0)
    {
      if (isPlayer1)
        gameData[row][inputCol] = 1;
      else
        gameData[row][inputCol] = 2;

      break;
    }
  }

  // print game board
  for (int row = 0; row < 6; row++)
  {
    for (int col = 0; col < 7; col++)
    {
      Serial.print(gameData[row][col]);
      Serial.print(" ");
    }
    Serial.println();
  }
  Serial.println();

  updateBoard();

  bool winnerFound = checkForWin();

  if (winnerFound) // 发现有赢家，结束
  {
    gameState = 2; // 结束游戏
    displayGameOver();
  }
  else // 未发现赢家，继续游戏
  {
    isPlayer1 = !isPlayer1;

    updateInputRow();
  }
}

void setupGameButtons()
{
  upButton.attachClick(upClick);
  // upButton.attachDoubleClick(upDoubleClick, &upButton);
  // upButton.attachLongPressStart(upLongPressStart, &upButton);
  // upButton.attachDuringLongPress(upDuringLongPress, &upButton);
  // upButton.attachLongPressStop(upLongPressStop, &upButton);
  // upButton.setLongPressIntervalMs(1000);

  downButton.attachClick(downClick);
  downButton.attachDoubleClick(confirmPlay);
  // downButton.attachLongPressStart(downLongPressStart, &downButton);
  // downButton.attachDuringLongPress(downDuringLongPress, &downButton);
  // downButton.attachLongPressStop(downLongPressStop, &downButton);
  // downButton.setLongPressIntervalMs(1000);
}
/***** 游戏相关的方法（end) *****/

void setup()
{
  Serial.begin(115200);

  setupGameButtons();

  rm67162_init();
  lcd_setRotation(3);

  bg.createSprite(WIDTH, HEIGHT);
  bg.fillSprite(TFT_WHITE);
  bg.setSwapBytes(1);

  createGame();
}

void loop()
{
  upButton.tick();
  downButton.tick();

  if (gameState == 2)
  {
    // flash winner pos
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
  else if (gameState == 1)
  {
    displayGameStatus(true);
  }
}