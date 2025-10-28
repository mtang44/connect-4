#pragma once
#include "Game.h"

//
// the classic game of connect four
//

//
// the main game class
//
class ConnectFour : public Game
{
public:
    ConnectFour();
    ~ConnectFour();

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
    bool        gameHasAI() override { return true; }
    int         findLowestEmpty(int column);
    Grid* getGrid() override { return _grid; }
private:
    Bit *       PieceForPlayer(const int playerNumber);
    Player*     ownerAt(int x, int y) const;
    int         negamax(std::string& state, int depth, int playerColor);

    Grid*       _grid;
};

