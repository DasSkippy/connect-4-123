#pragma once
#include "Game.h"

class Connect4 : public Game
{
public:
    Connect4();
    ~Connect4();

    // set up the board
    void        setUpBoard() override;

    Player*     checkForWinner() override;
    bool        checkForDraw() override;
    std::string initialStateString() override;
    std::string stateString() override;
    void        setStateString(const std::string &s) override;
    bool        actionForEmptyHolder(BitHolder &holder) override;
    bool        canBitMoveFrom(Bit &bit, BitHolder &src) override;
    bool        canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;
    void        stopGame() override;

    void        updateAI() override;
    bool        gameHasAI() override {return true;}
    Grid*       getGrid() override {return _grid;}

    bool isColumnFull(int col);
    int getLowestEmptyRow(int col);
private:
    // Player numbers are 0/1, matching Game's Player objects (not HUMAN_PLAYER/AI_PLAYER constants).
    Bit *       PieceForPlayer(int playerNumber);
    Player *    ownerAt(int index) const;

    // Negamax with alpha-beta pruning.
    int         negamaxAlphaBeta(const std::string& state, int depth, int alpha, int beta, int playerToMove);
    // Heuristic evaluation of a position (windows-of-4 scoring + center preference).
    int         evaluatePosition(const std::string& state, int playerPerspective) const;
    int         winnerFromState(const std::string& state) const;
    bool        isDrawState(const std::string& state) const;
    // Apply a Connect-4 drop in a column (mutates state). Returns false if column is full.
    bool        applyMove(std::string& state, int col, int playerNumber) const;

    Grid*       _grid;
};
