# Connect 4 (CMPM 123)

This project adds **Connect 4** as a 4th game option alongside Tic-Tac-Toe, Checkers, and Othello.

## How to play

- Open the app and click **Start Connect4**.
- To place a piece, click a square in the **top row** of the column you want to drop into.
- The game ends when a player connects 4 horizontally, vertically, or diagonally (or the board fills).

## AI opponent (Negamax + Alpha-Beta)

Connect 4 supports:

- **Two-player** mode (human vs human)
- **Vs AI** mode (human vs AI), with the AI able to play as **Player 1** or **Player 2**

The AI uses a **Negamax** search with **Alpha-Beta pruning**:

- The board is evaluated by scoring every possible 4-cell “window” on the board.
- Alpha-beta pruning cuts off branches that cannot affect the final decision.
- Moves are searched in a center-first order (`3,2,4,1,5,0,6`) to improve pruning.

You can change the search depth in the **Settings** window before starting Connect 4.

## Where the code lives

- `classes/Connect4.h` / `classes/Connect4.cpp`: Connect 4 game implementation + Negamax AI
- `Application.cpp`: menu button + AI settings UI (choose “Play vs AI”, AI side, and Connect 4 depth)

## Build / Run

This project uses CMake.

### Windows (Visual Studio generator)

```powershell
cmake -S . -B build
cmake --build build --config Release
.\build\Release\demo.exe
```

### macOS / Linux

```bash
cmake -S . -B build
cmake --build build -j
./build/demo
```

## Assets

Connect 4 uses:

- `resources/yellow.png` (Player 1)
- `resources/red.png` (Player 2)
