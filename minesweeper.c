#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define CELL_SIZE 40
#define GRID_WIDTH 10
#define GRID_HEIGHT 10
#define NUM_MINES 10

typedef struct {
    bool revealed; //check the cell has been clicked or not
    bool mine;
    int neighborMines; //how many mines surround the cell
    bool flagged;
} Cell;

Cell grid[GRID_WIDTH][GRID_HEIGHT]; //declare a 2D array

void RevealCell(int x, int y) { //Reveal Cell function
    if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) return; //if the coordinates are outside the grid, return
    if (grid[x][y].revealed || grid[x][y].flagged) return; //if (x,y) has been click or flag, return

    grid[x][y].revealed = true;

    if (grid[x][y].neighborMines == 0 && grid[x][y].mine == false) { //when no mines around the grid and the grid itself is not a mine
        for (int dx = -1; dx <= 1; dx++) { //nested loop, access each grid
            for (int dy = -1; dy <= 1; dy++) {
                if (dx != 0 || dy != 0) { //skip dx==0 && dy==0
                    RevealCell(x + dx, y + dy);
                }
            }
        }
    }
}

void InitGrid() {
    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            grid[x][y] = (Cell){ false, false, 0, false }; //reset all cell
        }
    }

    int placed = 0;
    while (placed < NUM_MINES) {
        int x = rand() % GRID_WIDTH; //place the mines at random place, between 0 until grid width-1 (9)
        int y = rand() % GRID_HEIGHT;
        if (grid[x][y].mine == false) {//if the grid dont have a mine yet
            grid[x][y].mine = true;//place a mine at here, and =true
            placed++;
        }
    }

    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            if (grid[x][y].mine) continue;

            int count = 0;
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    int nx = x + dx;
                    int ny = y + dy;
                    if (nx >= 0 && nx < GRID_WIDTH && ny >= 0 && ny < GRID_HEIGHT) { //check if the neighbor coordinates (nx,ny) is within the map
                        if (grid[nx][ny].mine)
                        count++;
                    }
                }
            }
            grid[x][y].neighborMines = count; //
        }
    }
}

bool CheckWin() {
    int correctFlags = 0;
    int totalFlags = 0;
    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            if (grid[x][y].flagged) {
                totalFlags++;
                if (grid[x][y].mine) correctFlags++;
            }
        }
    }
    return (correctFlags == NUM_MINES && totalFlags == NUM_MINES);
}

int main() {
    srand(time(NULL)); //ensure each time run different random sequence
    InitWindow(GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE, "Minesweeper");
    SetTargetFPS(60); //limit frame rate

    InitGrid();
    bool gameOver = false;
    bool gameWon = false;
    bool restart = false;
    int flagsPlaced = 0;

    while (!WindowShouldClose()) {
        if (gameOver || gameWon) {
            int key = GetKeyPressed();
            switch (key){
                case KEY_R:
                    restart = true;
                    break;
                case KEY_Q:
                    CloseWindow();
                    return 0;
                default:
                    break;
            }
        }

        if (restart) {
            InitGrid();
            gameOver = false;
            gameWon = false;
            restart = false;
            flagsPlaced = 0;
        }

        if (!gameOver && !gameWon) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mouse = GetMousePosition();
                int x = mouse.x / CELL_SIZE;
                int y = mouse.y / CELL_SIZE;

                if (!grid[x][y].revealed && !grid[x][y].flagged) {
                    switch (grid[x][y].mine) {
                        case true:
                            grid[x][y].revealed = true;
                            gameOver = true;
                            break;
                        case false:
                            RevealCell(x, y);
                            break;
                    }
                }
            }

            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                Vector2 mouse = GetMousePosition();
                int x = mouse.x / CELL_SIZE;
                int y = mouse.y / CELL_SIZE;

                if (!grid[x][y].revealed) { //only flag unrevealed cell
                    switch (grid[x][y].flagged){
                        case false:
                            if (flagsPlaced < NUM_MINES) {
                                grid[x][y].flagged = true; //flag the cell
                                flagsPlaced++;
                            }
                            break;
                        case true:
                            grid[x][y].flagged = false; //can unflag the cell
                            flagsPlaced--;
                            break;
                    }

                    if (flagsPlaced == NUM_MINES) {
                        if (CheckWin()) {
                            gameWon = true;
                        } else {
                            gameOver = true;
                        }
                    }
                }
            }
        }
        
        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int x = 0; x < GRID_WIDTH; x++) {
            for (int y = 0; y < GRID_HEIGHT; y++) {
                Rectangle cell = { x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE }; //draw a 40x40 square in pixel
                DrawRectangleLinesEx(cell, 1, GRAY);

                if (grid[x][y].revealed) {
                    if (grid[x][y].mine) {
                        DrawCircle(cell.x + CELL_SIZE / 2, cell.y + CELL_SIZE / 2, 10, RED);
                    } else if (grid[x][y].neighborMines > 0) {
                        DrawText(TextFormat("%d", grid[x][y].neighborMines), cell.x + 12, cell.y + 10, 20, DARKBLUE);
                    }
                } else {
                    DrawRectangleRec(cell, LIGHTGRAY); //whole cell fill with lightgrey
                    if (grid[x][y].flagged) {
                        DrawText("F", cell.x + 12, cell.y + 10, 20, MAROON);
                    }
                }
            }
        }

        if (gameOver || gameWon) {
            const char *msg;
            if (gameOver) {
                msg = "Game Over!";
            } else {
                msg = "You Win!";
            }
        
            Color msgColor;
            if (gameOver) {
                msgColor = RED;
            } else {
                msgColor = DARKGREEN;
            }

            int screenWidth = GRID_WIDTH * CELL_SIZE; // 400x400
            int screenHeight = GRID_HEIGHT * CELL_SIZE;

            int textWidth1 = MeasureText(msg, 30);
            int textWidth2 = MeasureText("Press 'R' to Restart or 'Q' to Quit", 20);

            DrawText(msg, (screenWidth - textWidth1) / 2, screenHeight / 2 - 30, 30, msgColor);
            DrawText(msg, (screenWidth - textWidth1) / 2, screenHeight / 2 - 30, 30, msgColor);
            DrawText("Press 'R' to Restart or 'Q' to Quit", (screenWidth - textWidth2) / 2, screenHeight / 2 + 10, 20, BLACK);
            DrawText("Press 'R' to Restart or 'Q' to Quit", (screenWidth - textWidth2) / 2, screenHeight / 2 + 10, 20, BLACK);
            DrawText("Press 'R' to Restart or 'Q' to Quit", (screenWidth - textWidth2) / 2, screenHeight / 2 + 10, 20, BLACK);

        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
