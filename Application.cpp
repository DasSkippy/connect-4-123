#include "Application.h"
#include "imgui/imgui.h"
#include "classes/TicTacToe.h"
#include "classes/Checkers.h"
#include "classes/Othello.h"
#include "classes/Connect4.h"

namespace ClassGame {
        //
        // our global variables
        //
        Game *game = nullptr;
        bool gameOver = false;
        int gameWinner = -1;

        //
        // game starting point
        // this is called by the main render loop in main.cpp
        //
        void GameStartUp() 
        {
            game = nullptr;
        }

        //
        // game render loop
        // this is called by the main render loop in main.cpp
        //
        void RenderGame() 
        {
                ImGui::DockSpaceOverViewport();

                //ImGui::ShowDemoWindow();

                ImGui::Begin("Settings");

                static bool playVsAI = false;
                static int aiPlaysAs = 1; // 0 = Player 1, 1 = Player 2
                static int connect4Depth = 7;

                if (gameOver) {
                    ImGui::Text("Game Over!");
                    ImGui::Text("Winner: %d", gameWinner);
                    if (ImGui::Button("Reset Game")) {
                        game->stopGame();
                        game->setUpBoard();
                        gameOver = false;
                        gameWinner = -1;
                    }
                }
                if (!game) {
                    ImGui::SeparatorText("Opponent");
                    ImGui::Checkbox("Play vs AI", &playVsAI);
                    if (playVsAI) {
                        ImGui::RadioButton("AI is Player 1", &aiPlaysAs, 0);
                        ImGui::RadioButton("AI is Player 2", &aiPlaysAs, 1);
                    }
                    ImGui::SeparatorText("Connect 4 AI");
                    ImGui::SliderInt("Negamax depth", &connect4Depth, 1, 9);

                    if (ImGui::Button("Start Tic-Tac-Toe")) {
                        game = new TicTacToe();
                        game->_gameOptions.AIPlaying = playVsAI;
                        game->_gameOptions.AIPlayer = aiPlaysAs;
                        game->setUpBoard();
                    }
                    if (ImGui::Button("Start Checkers")) {
                        game = new Checkers();
                        game->setUpBoard();
                    }
                    if (ImGui::Button("Start Othello")) {
                        game = new Othello();
                        game->_gameOptions.AIPlaying = playVsAI;
                        game->_gameOptions.AIPlayer = aiPlaysAs;
                        game->setUpBoard();
                    }
                    if (ImGui::Button("Start Connect4")) {
                        game = new Connect4();
                        game->_gameOptions.AIPlaying = playVsAI;
                        game->_gameOptions.AIPlayer = aiPlaysAs;
                        game->_gameOptions.AIMAXDepth = connect4Depth;
                        game->setUpBoard();
                    }
                } else {
                    ImGui::Text("Current Player Number: %d", game->getCurrentPlayer()->playerNumber());
                    ImGui::Text("Current Board State: %s", game->stateString().c_str());
                    if (game->gameHasAI() && game->_gameOptions.AIPlaying) {
                        ImGui::Text("AI Player Number: %d", game->getAIPlayer());
                        ImGui::Text("AI Nodes Searched: %d", game->getAIDepathSearches());
                        ImGui::Text("AI Depth: %d", game->getAIMAXDepth());
                    }
                }
                ImGui::End();

                ImGui::Begin("GameWindow");
                if (game) {
                    if (!gameOver && game->gameHasAI() && (game->getCurrentPlayer()->isAIPlayer() || game->_gameOptions.AIvsAI))
                    {
                        game->updateAI();
                    }
                    if (!gameOver) {
                        game->drawFrame();
                    } else {
                        // Still render the final board state, but don't allow interaction after game over.
                        Grid* grid = game->getGrid();
                        grid->forEachEnabledSquare([](ChessSquare* square, int x, int y) {
                            square->paintSprite();
                        });
                        grid->forEachEnabledSquare([](ChessSquare* square, int x, int y) {
                            if (square->bit()) {
                                square->bit()->update();
                                square->bit()->paintSprite();
                            }
                        });
                    }
                }
                ImGui::End();
        }

        //
        // end turn is called by the game code at the end of each turn
        // this is where we check for a winner
        //
        void EndOfTurn() 
        {
            Player *winner = game->checkForWinner();
            if (winner)
            {
                gameOver = true;
                gameWinner = winner->playerNumber();
            }
            if (game->checkForDraw()) {
                gameOver = true;
                gameWinner = -1;
            }
        }
}
