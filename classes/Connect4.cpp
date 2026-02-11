#include "Connect4.h"

Connect4::Connect4()
{
    _grid = new Grid(6,7);
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
    for (int i = 5; i >= 0; i--)
    {
        for (int j = 0; j < 7; j++)
        {
            int noBit = 0;
            if(!_grid->getSquare(j, i)->bit())
                noBit++;

            if(noBit == 7)
                return i;
        }
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
    auto square= _grid->getSquare(index % 7, index / 6);
    if (!square || !square->bit()) {
        return nullptr;
    }
    return square->bit()->getOwner();
}

Player* Connect4::checkForWinner()
{
    static const int kWinningTriples[8][4] =  { {0,1,2}, {3,4,5}, {6,7,8},  // rows
                                                {0,3,6}, {1,4,7}, {2,5,8},  // cols
                                                {0,4,8}, {2,4,6} };         // diagonals
    for( int i=0; i<41; i++ ) {
        const int *triple = kWinningTriples[i];
        Player *player = ownerAt(triple[0]);
        if( player && player == ownerAt(triple[1]) && player == ownerAt(triple[2]) )
            return player;
    }
    return nullptr;
}