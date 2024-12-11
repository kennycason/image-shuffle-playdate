#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "pd_api.h"

static PlaydateAPI* pd = NULL;
static LCDFont* font = NULL;
static char textBuffer[32];

// Game constants
#define PUZZLE_SIZE 10
static int tiles[PUZZLE_SIZE * PUZZLE_SIZE];
static const int TILE_SIZE = 24;
static int emptyTileIndex = 0;
static LCDBitmap* spriteSheet = NULL;
static int lastMovedMs = 0;

static void initTiles(void) {
    for (int i = 0; i < PUZZLE_SIZE * PUZZLE_SIZE; i++) {
        tiles[i] = i; // 0 is empty tile
    }
}

static void loadSprites(void) {
    const char* err;
    spriteSheet = pd->graphics->loadBitmap("images/navi", &err);
    if (spriteSheet == NULL) {
        pd->system->error("Failed to load sprite sheet: %s", err);
    }
}

static void swapTiles(int from, int to) {
    int temp = tiles[from];
    tiles[from] = tiles[to];
    tiles[to] = temp;
    emptyTileIndex = to;
}

static void getPossibleMoves(int* moves, int* moveCount) {
    *moveCount = 0;
    int emptyRow = emptyTileIndex / PUZZLE_SIZE;
    int emptyCol = emptyTileIndex % PUZZLE_SIZE;

    if (emptyRow > 0) moves[(*moveCount)++] = emptyTileIndex - PUZZLE_SIZE;
    if (emptyRow < PUZZLE_SIZE - 1) moves[(*moveCount)++] = emptyTileIndex + PUZZLE_SIZE;
    if (emptyCol > 0) moves[(*moveCount)++] = emptyTileIndex - 1;
    if (emptyCol < PUZZLE_SIZE - 1) moves[(*moveCount)++] = emptyTileIndex + 1;
}

static void shufflePuzzle(void) {
    int moves[4];
    int moveCount;

    for (int i = 0; i < 100; i++) {
        getPossibleMoves(moves, &moveCount);
        int randomIndex = rand() % moveCount;
        swapTiles(emptyTileIndex, moves[randomIndex]);
    }
}

static void drawTiles(void) {
    pd->graphics->clear(kColorWhite);

    for (int i = 0; i < PUZZLE_SIZE * PUZZLE_SIZE; i++) {
        if (tiles[i] == 0) continue; // Skip empty tile

        int tileNum = tiles[i] - 1;
        int srcX = ((tileNum + PUZZLE_SIZE + 1) % PUZZLE_SIZE) * TILE_SIZE;
        int srcY = (tileNum / PUZZLE_SIZE) * TILE_SIZE;

        int destX = (i % PUZZLE_SIZE) * TILE_SIZE;
        int destY = (i / PUZZLE_SIZE) * TILE_SIZE;

        LCDBitmap* tileBitmap = pd->graphics->newBitmap(TILE_SIZE, TILE_SIZE, kColorClear);
        pd->graphics->pushContext(tileBitmap);
        pd->graphics->drawBitmap(spriteSheet, -srcX, -srcY, kBitmapUnflipped);
        pd->graphics->popContext();

        pd->graphics->drawBitmap(tileBitmap, destX, destY, kBitmapUnflipped);
        pd->graphics->freeBitmap(tileBitmap);
    }
}

static int update(void* userdata) {
    PDButtons current, pushed, released;
    pd->system->getButtonState(&current, &pushed, &released);

    int currentTime = pd->system->getCurrentTimeMilliseconds();
    if (currentTime - lastMovedMs > 150) {
        int emptyRow = emptyTileIndex / PUZZLE_SIZE;
        int emptyCol = emptyTileIndex % PUZZLE_SIZE;

        if (current & kButtonUp && emptyRow < PUZZLE_SIZE - 1) {
            swapTiles(emptyTileIndex, emptyTileIndex + PUZZLE_SIZE);
            lastMovedMs = currentTime;
        }
        if (current & kButtonDown && emptyRow > 0) {
            swapTiles(emptyTileIndex, emptyTileIndex - PUZZLE_SIZE);
            lastMovedMs = currentTime;
        }
        if (current & kButtonLeft && emptyCol < PUZZLE_SIZE - 1) {
            swapTiles(emptyTileIndex, emptyTileIndex + 1);
            lastMovedMs = currentTime;
        }
        if (current & kButtonRight && emptyCol > 0) {
            swapTiles(emptyTileIndex, emptyTileIndex - 1);
            lastMovedMs = currentTime;
        }
    }

    if (pushed & kButtonA) {
        shufflePuzzle();
    }

    drawTiles();
    return 1;
}

int eventHandler(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg) {
    if (event == kEventInit) {
        pd = playdate;
        pd->system->setUpdateCallback(update, NULL);

        initTiles();
        loadSprites();

        pd->display->setRefreshRate(30);
    }
    
    return 0;
}