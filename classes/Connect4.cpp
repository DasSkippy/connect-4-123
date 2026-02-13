#include "Connect4.h"

Connect4::Connect4()
{
    _grid = new Grid(7,6);
}

Connect4::~Connect4()
{
    delete _grid;
}

Bit* Connect4::PieceForPlayer(const int playerNumber) 
{
    Bit *bit = new Bit();

    bit->LoadTextureFromFile(playerNumber == AI_PLAYER ? "red.png" : "yellow.png");
    bit->setOwner(getPlayerAt(playerNumber == AI_PLAYER ? 1 : 0));
    return bit;
}

void Connect4::setUpBoard()
{
    setNumberOfPlayers(2);
    _gameOptions.rowX = 6;
    _gameOptions.rowY = 7;
    _grid->initializeSquares(80, "square.png");

    if(gameHasAI()) {
        setAIPlayer(AI_PLAYER);
    }

    startGame();
}

bool Connect4::actionForEmptyHolder(BitHolder &holder)
{
    ChessSquare* clickedSquare = dynamic_cast<ChessSquare*>(&holder);
    if(!clickedSquare) {
        return false;
    }

    int col = clickedSquare->getColumn();

    if (col == -1) {
        return false;
    }
    
    if (isColumnFull(col)) {
        return false;
    }

    int targetRow = getLowestEmptyRow(col);
    if (targetRow == -1) {
        return false;
    }

    Bit *bit = PieceForPlayer(getCurrentPlayer()->playerNumber() == 0 ? HUMAN_PLAYER : AI_PLAYER);
    if (bit) {
        ChessSquare* topSquare = _grid->getSquare(col, 0);
        ChessSquare* targetSquare = _grid->getSquare(col, targetRow);

        if(targetRow > 0) {
            bit->setPosition(topSquare->getPosition());
            bit->moveTo(targetSquare->getPosition());
            targetSquare->setBit(bit);
            endTurn();
        } else {
            bit->setPosition(targetSquare->getPosition());
            targetSquare->setBit(bit);
            endTurn();
        }
        return true;
    }
    return false;
}

bool Connect4::isColumnFull(int col)
{
    for (int i = 0; i < 6; i++)
    {
        if (!_grid->getSquare(col, i)->bit())
            return false;
    }
    return true;
}

int Connect4::getLowestEmptyRow(int col)
{
    for (int row = 5; row >= 0; row--)
    {
        if (!_grid->getSquare(col, row)->bit())
            return row;
    }
    return -1;
}

bool Connect4::canBitMoveFrom(Bit &bit, BitHolder &src)
{
    return false;
}

bool Connect4::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst)
{
    return false;
}

void Connect4::stopGame()
{
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
}

Player* Connect4::ownerAt(int index) const
{
    auto square= _grid->getSquare(index % 7, index / 7);
    if (!square || !square->bit()) {
        return nullptr;
    }
    return square->bit()->getOwner();
}

Player* Connect4::checkForWinner()
{
    // Horizontal 
    for (int row = 0; row < 6; row++)
    {
        for (int col = 0; col < 4; col++) // stop at 3 before edge
        {
            auto square = _grid->getSquare(col, row);
            if (!square->bit()) continue;

            Player* p = square->bit()->getOwner();

            if (_grid->getSquare(col+1, row)->bit() &&
                _grid->getSquare(col+2, row)->bit() &&
                _grid->getSquare(col+3, row)->bit() &&
                p == _grid->getSquare(col+1, row)->bit()->getOwner() &&
                p == _grid->getSquare(col+2, row)->bit()->getOwner() &&
                p == _grid->getSquare(col+3, row)->bit()->getOwner())
            {
                return p;
            }
        }
    }

    // Vertical 
    for (int col = 0; col < 7; col++)
    {
        for (int row = 0; row < 3; row++) // stop at 2 before bottom
        {
            auto square = _grid->getSquare(col, row);
            if (!square->bit()) continue;

            Player* p = square->bit()->getOwner();

            if (_grid->getSquare(col, row+1)->bit() &&
                _grid->getSquare(col, row+2)->bit() &&
                _grid->getSquare(col, row+3)->bit() &&
                p == _grid->getSquare(col, row+1)->bit()->getOwner() &&
                p == _grid->getSquare(col, row+2)->bit()->getOwner() &&
                p == _grid->getSquare(col, row+3)->bit()->getOwner())
            {
                return p;
            }
        }
    }

    // Diagonal (down-right)
    for (int col = 0; col < 4; col++)
    {
        for (int row = 0; row < 3; row++)
        {
            auto square = _grid->getSquare(col, row);
            if (!square->bit()) continue;

            Player* p = square->bit()->getOwner();

            if (_grid->getSquare(col+1, row+1)->bit() &&
                _grid->getSquare(col+2, row+2)->bit() &&
                _grid->getSquare(col+3, row+3)->bit() &&
                p == _grid->getSquare(col+1, row+1)->bit()->getOwner() &&
                p == _grid->getSquare(col+2, row+2)->bit()->getOwner() &&
                p == _grid->getSquare(col+3, row+3)->bit()->getOwner())
            {
                return p;
            }
        }
    }

    // Diagonal (up-right)
    for (int col = 0; col < 4; col++)
    {
        for (int row = 3; row < 6; row++)
        {
            auto square = _grid->getSquare(col, row);
            if (!square->bit()) continue;

            Player* p = square->bit()->getOwner();

            if (_grid->getSquare(col+1, row-1)->bit() &&
                _grid->getSquare(col+2, row-2)->bit() &&
                _grid->getSquare(col+3, row-3)->bit() &&
                p == _grid->getSquare(col+1, row-1)->bit()->getOwner() &&
                p == _grid->getSquare(col+2, row-2)->bit()->getOwner() &&
                p == _grid->getSquare(col+3, row-3)->bit()->getOwner())
            {
                return p;
            }
        }
    }

    return nullptr;
}

bool Connect4::checkForDraw()
{
    // If someone already won, it's not a draw
    if (checkForWinner() != nullptr)
        return false;

    for (int col = 0; col < 7; col++)
    {
        if (!isColumnFull(col))
            return false;
    }

    return true; // All columns full, no winner
}

std::string Connect4::initialStateString()
{
    return std::string(42, '0');
}

std::string Connect4::stateString()
{
    std::string s(42, '0');
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        Bit *bit = square->bit();
        if (bit) {
            s[y * 7 + x] = std::to_string(bit->getOwner()->playerNumber()+1)[0];
        }
    });
    return s;
}

void Connect4::setStateString(const std::string &s)
{
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        int index = y*7 + x;
        int playerNumber = s[index] - '0';
        if (playerNumber) {
            square->setBit( PieceForPlayer(playerNumber-1) );
        } else {
            square->setBit( nullptr );
        }
    });
}

void Connect4::updateAI()
{
    std::string state = stateString();

    int bestScore = -100000;
    BitHolder* bestMove = nullptr;

    for (int col = 0; col < 7; col++)
    {
        // Check if column has space
        if (state[col] == '0' ||
            state[7+col] == '0' ||
            state[14+col] == '0' ||
            state[21+col] == '0' ||
            state[28+col] == '0' ||
            state[35+col] == '0')
        {
            std::string newState = state;

            // simulate drop for AI (player 2)
            for (int row = 5; row >= 0; row--)
            {
                int index = row * 7 + col;
                if (newState[index] == '0')
                {
                    newState[index] = '2';
                    break;
                }
            }

            int score = minimax(newState, 5, false);

            if (score > bestScore)
            {
                bestScore = score;
                int row = getLowestEmptyRow(col);
                bestMove = _grid->getSquare(col, row);
            }
        }
    }

    if (bestMove)
        actionForEmptyHolder(*bestMove);
}


int Connect4::minimax(std::string state, int depth, bool maximizingPlayer)
{
    // --- Local win checker lambda ---
    auto wins = [&](char player)
    {
        // Horizontal
        for (int r = 0; r < 6; r++)
            for (int c = 0; c < 4; c++)
                if (state[r*7+c] == player &&
                    state[r*7+c+1] == player &&
                    state[r*7+c+2] == player &&
                    state[r*7+c+3] == player)
                    return true;

        // Vertical
        for (int c = 0; c < 7; c++)
            for (int r = 0; r < 3; r++)
                if (state[r*7+c] == player &&
                    state[(r+1)*7+c] == player &&
                    state[(r+2)*7+c] == player &&
                    state[(r+3)*7+c] == player)
                    return true;

        // Diagonal down-right
        for (int c = 0; c < 4; c++)
            for (int r = 0; r < 3; r++)
                if (state[r*7+c] == player &&
                    state[(r+1)*7+c+1] == player &&
                    state[(r+2)*7+c+2] == player &&
                    state[(r+3)*7+c+3] == player)
                    return true;

        // Diagonal up-right
        for (int c = 0; c < 4; c++)
            for (int r = 3; r < 6; r++)
                if (state[r*7+c] == player &&
                    state[(r-1)*7+c+1] == player &&
                    state[(r-2)*7+c+2] == player &&
                    state[(r-3)*7+c+3] == player)
                    return true;

        return false;
    };

    // --- Terminal checks ---
    if (wins('2')) return 1000 + depth;   // AI win
    if (wins('1')) return -1000 - depth;  // Human win

    bool draw = true;
    for (char c : state)
        if (c == '0')
            draw = false;
    if (draw || depth == 0)
        return evaluateBoard(state);

    if (maximizingPlayer)
    {
        int maxEval = -100000;

        for (int col = 0; col < 7; col++)
        {
            std::string newState = state;

            bool valid = false;
            for (int row = 5; row >= 0; row--)
            {
                int index = row * 7 + col;
                if (newState[index] == '0')
                {
                    newState[index] = '2';
                    valid = true;
                    break;
                }
            }

            if (!valid) continue;

            int eval = minimax(newState, depth - 1, false);
            maxEval = std::max(maxEval, eval);
        }

        return maxEval;
    }
    else
    {
        int minEval = 100000;

        for (int col = 0; col < 7; col++)
        {
            std::string newState = state;

            bool valid = false;
            for (int row = 5; row >= 0; row--)
            {
                int index = row * 7 + col;
                if (newState[index] == '0')
                {
                    newState[index] = '1';
                    valid = true;
                    break;
                }
            }

            if (!valid) continue;

            int eval = minimax(newState, depth - 1, true);
            minEval = std::min(minEval, eval);
        }

        return minEval;
    }
}


int Connect4::evaluateBoard(std::string state)
{
    int score = 0;

    // Prefer center column
    for (int row = 0; row < 6; row++)
    {
        if (state[row*7 + 3] == '2')
            score += 3;
        if (state[row*7 + 3] == '1')
            score -= 3;
    }

    return score;
}


