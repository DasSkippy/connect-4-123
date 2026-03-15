#include "Connect4.h"
#include <array>
#include <limits>

Connect4::Connect4()
{
    _grid = new Grid(7,6);
}

Connect4::~Connect4()
{
    delete _grid;
}

Bit* Connect4::PieceForPlayer(int playerNumber) 
{
    Bit *bit = new Bit();

    bit->LoadTextureFromFile(playerNumber == 1 ? "red.png" : "yellow.png");
    bit->setOwner(getPlayerAt(playerNumber));
    return bit;
}

void Connect4::setUpBoard()
{
    setNumberOfPlayers(2);
    _gameOptions.rowX = 7;
    _gameOptions.rowY = 6;
    _grid->initializeSquares(80, "square.png");

    if (_gameOptions.AIPlaying) {
        setAIPlayer(_gameOptions.AIPlayer);
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

    const int playerNumber = getCurrentPlayer()->playerNumber();
    Bit *bit = PieceForPlayer(playerNumber);
    if (bit) {
        // Animate the piece dropping from the top square of the chosen column.
        ChessSquare* topSquare = _grid->getSquare(col, 0);
        ChessSquare* targetSquare = _grid->getSquare(col, targetRow);

        bit->setPosition(topSquare->getPosition());
        bit->moveTo(targetSquare->getPosition());
        targetSquare->setBit(bit);
        endTurn();
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
            Bit* bit = PieceForPlayer(playerNumber - 1);
            bit->setPosition(square->getPosition());
            square->setBit(bit);
        } else {
            square->setBit( nullptr );
        }
    });
}

void Connect4::updateAI()
{
    if (!_gameOptions.AIPlaying) {
        return;
    }

    const int playerToMove = getCurrentPlayer()->playerNumber(); // 0 or 1
    std::string state = stateString();

    // Negamax with alpha-beta pruning. Score is always from the perspective of the side-to-move.
    _gameOptions.AIDepthSearches = 0;

    const int depth = std::max(1, _gameOptions.AIMAXDepth);
    const int INF = 1'000'000'000;

    static constexpr std::array<int, 7> kMoveOrder = {3, 2, 4, 1, 5, 0, 6};

    int bestScore = std::numeric_limits<int>::min();
    int bestCol = -1;
    int alpha = -INF;
    int beta = INF;

    for (int col : kMoveOrder)
    {
        std::string child = state;
        if (!applyMove(child, col, playerToMove)) {
            continue;
        }

        const int score = -negamaxAlphaBeta(child, depth - 1, -beta, -alpha, 1 - playerToMove);
        if (score > bestScore) {
            bestScore = score;
            bestCol = col;
        }
        alpha = std::max(alpha, score);
    }

    if (bestCol >= 0) {
        actionForEmptyHolder(*_grid->getSquare(bestCol, 0));
    }
}

bool Connect4::applyMove(std::string& state, int col, int playerNumber) const
{
    const char piece = static_cast<char>('1' + playerNumber);
    for (int row = 5; row >= 0; row--)
    {
        const int index = row * 7 + col;
        if (state[index] == '0')
        {
            state[index] = piece;
            return true;
        }
    }
    return false;
}

bool Connect4::isDrawState(const std::string& state) const
{
    return state.find('0') == std::string::npos;
}

int Connect4::winnerFromState(const std::string& state) const
{
    auto wins = [&](char player) -> bool
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

    if (wins('1')) return 0;
    if (wins('2')) return 1;
    return -1;
}

int Connect4::evaluatePosition(const std::string& state, int playerPerspective) const
{
    const char self = static_cast<char>('1' + playerPerspective);
    const char opp = static_cast<char>('1' + (1 - playerPerspective));

    int score = 0;

    // Prefer center column
    for (int row = 0; row < 6; row++)
    {
        const char center = state[row * 7 + 3];
        if (center == self) score += 3;
        else if (center == opp) score -= 3;
    }

    auto scoreWindow = [&](char a, char b, char c, char d) -> int
    {
        int selfCount = 0, oppCount = 0, emptyCount = 0;
        const char window[4] = {a, b, c, d};
        for (char cell : window)
        {
            if (cell == self) selfCount++;
            else if (cell == opp) oppCount++;
            else emptyCount++;
        }

        if (selfCount == 4) return 100000;
        if (oppCount == 4) return -100000;

        if (selfCount == 3 && emptyCount == 1) return 120;
        if (selfCount == 2 && emptyCount == 2) return 12;
        if (selfCount == 1 && emptyCount == 3) return 1;

        if (oppCount == 3 && emptyCount == 1) return -140;
        if (oppCount == 2 && emptyCount == 2) return -14;

        return 0;
    };

    // Horizontal windows
    for (int r = 0; r < 6; r++)
        for (int c = 0; c < 4; c++)
            score += scoreWindow(state[r*7+c], state[r*7+c+1], state[r*7+c+2], state[r*7+c+3]);

    // Vertical windows
    for (int c = 0; c < 7; c++)
        for (int r = 0; r < 3; r++)
            score += scoreWindow(state[r*7+c], state[(r+1)*7+c], state[(r+2)*7+c], state[(r+3)*7+c]);

    // Diagonal down-right windows
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 3; r++)
            score += scoreWindow(state[r*7+c], state[(r+1)*7+c+1], state[(r+2)*7+c+2], state[(r+3)*7+c+3]);

    // Diagonal up-right windows
    for (int c = 0; c < 4; c++)
        for (int r = 3; r < 6; r++)
            score += scoreWindow(state[r*7+c], state[(r-1)*7+c+1], state[(r-2)*7+c+2], state[(r-3)*7+c+3]);

    return score;
}

int Connect4::negamaxAlphaBeta(const std::string& state, int depth, int alpha, int beta, int playerToMove)
{
    _gameOptions.AIDepthSearches++;

    const int winner = winnerFromState(state);
    if (winner != -1)
    {
        const int WIN_SCORE = 1'000'000;
        return (winner == playerToMove) ? (WIN_SCORE + depth) : (-WIN_SCORE - depth);
    }

    if (depth == 0 || isDrawState(state))
    {
        return evaluatePosition(state, playerToMove);
    }

    static constexpr std::array<int, 7> kMoveOrder = {3, 2, 4, 1, 5, 0, 6};

    int best = std::numeric_limits<int>::min();

    for (int col : kMoveOrder)
    {
        std::string child = state;
        if (!applyMove(child, col, playerToMove)) {
            continue;
        }

        const int score = -negamaxAlphaBeta(child, depth - 1, -beta, -alpha, 1 - playerToMove);
        best = std::max(best, score);
        alpha = std::max(alpha, score);
        if (alpha >= beta) {
            break; // alpha-beta prune
        }
    }

    return best;
}

