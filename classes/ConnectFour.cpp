#include "ConnectFour.h"
#include "../Logger.h"

ConnectFour::ConnectFour()
{
    _grid = new Grid(7, 6);
}

ConnectFour::~ConnectFour()
{
    delete _grid;
}

//
// make an X or an O
//
Bit* ConnectFour::PieceForPlayer(const int playerNumber)
{
    // depending on playerNumber load the "x.png" or the "o.png" graphic
    Bit *bit = new Bit();
    // should possibly be cached from player class?
    bit->LoadTextureFromFile(playerNumber == AI_PLAYER ? "o.png" : "x.png");
    bit->setOwner(getPlayerAt(playerNumber == AI_PLAYER ? 1 : 0));
    return bit;
}

void ConnectFour::setUpBoard()
{
    setNumberOfPlayers(2);
    _gameOptions.rowX = 7;
    _gameOptions.rowY = 6;
    _grid->initializeSquares(80, "square.png");

    if (gameHasAI() && (_gameOptions.playerVSAI == true || _gameOptions.AIvsAI == true)) {
        setAIPlayer(AI_PLAYER);
    }

    startGame();
}

//
// about the only thing we need to actually fill out for tic-tac-toe
//
bool ConnectFour::actionForEmptyHolder(BitHolder &holder)
{
    
    std::cout << "actionForEmptyHolder called" << std::endl;
    ChessSquare* square = dynamic_cast<ChessSquare*>(&holder);
    Logger::GetInstance().LogGameEvent("Placing piece at position: (" + std::to_string(square->getColumn()) + ", " + std::to_string(  square->getRow()) + ")");
    int x = square->getColumn();
    int y = square->getRow();
    if(ownerAt(x,y) != nullptr)
    {
       return false;
    }
    // if (!clickedSquare) {
    //     return false;
    // }
    Bit *bit = PieceForPlayer(getCurrentPlayer()->playerNumber() == 0 ? HUMAN_PLAYER : AI_PLAYER);
    if (bit) {

        ChessSquare *s = _grid->getSquare(x,0);
        int targetY = findLowestEmpty(y);
         ImVec2 pos = s->getPosition();
         bit->setPosition(pos); 
        ChessSquare *targetSquare = _grid->getSquare(x,targetY);
        targetSquare->setBit(bit);
        pos = targetSquare->getPosition();
        bit->moveTo(pos);
        //bit->setPosition(holder.getPosition());
        endTurn();

        return true;
    }   
    return false;
}
int ConnectFour::findLowestEmpty(int column)
{
    for (int row = 5; row >= 0; --row) {
        if (ownerAt(row, column) == nullptr) {
            Logger::GetInstance().LogGameEvent("Lowest Empty = column " + std::to_string(column)+"in row " + to_string(row));
            return row;
        }
        
    }
    return -1; // Column is full
}
bool ConnectFour::canBitMoveFrom(Bit &bit, BitHolder &src)
{
    // you can't move anything in connect four
    return false;
}

bool ConnectFour::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst)
{
    // you can't move anything in connect four
    return false;
}

//
// free all the memory used by the game on the heap
//
void ConnectFour::stopGame()
{
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
}

//
// helper function for the winner check
//
Player* ConnectFour::ownerAt(int x, int y) const
{
    if (x < 0 || x >= _gameOptions.rowY || y < 0 || y >= _gameOptions.rowX) {
        return nullptr;
    }

    auto square = _grid->getSquare(x, y);
    if (!square || !square->bit()) {
        return nullptr;
    }
    return square->bit()->getOwner();

}

Player* ConnectFour::checkForWinner()
{
    // static const int kWinningTriples[8][3] =  { {0,1,2}, {3,4,5}, {6,7,8},  // rows
    //                                             {0,3,6}, {1,4,7}, {2,5,8},  // cols
    //                                             {0,4,8}, {2,4,6} };         // diagonals
    // for( int i=0; i<8; i++ ) {
    //     const int *triple = kWinningTriples[i];
    //     Player *player = ownerAt(triple[0]);
    //     if( player && player == ownerAt(triple[1]) && player == ownerAt(triple[2]) )
    //         return player;
    // }
    return nullptr;
}

bool ConnectFour::checkForDraw()
{
    bool isDraw = true;
    // check to see if the board is full
    _grid->forEachSquare([&isDraw](ChessSquare* square, int x, int y) {
        if (!square->bit()) {
            isDraw = false;
        }
    });
    return isDraw;
}

//
// state strings
//
std::string ConnectFour::initialStateString()
{
    return "00000000000000000000000000000000000000000";
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string ConnectFour::stateString()
{
    std::string s = "00000000000000000000000000000000000000000";
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        Bit *bit = square->bit();
        if (bit) {
            s[y * 7 + x] = to_string(bit->getOwner()->playerNumber()+1)[0];
        }
    });
    return s;
}

//
// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
//
void ConnectFour::setStateString(const std::string &s)
{
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        int index = y*7 + x;//column + row * 7
        int playerNumber = s[index] - '0';
        if (playerNumber) {
            square->setBit( PieceForPlayer(playerNumber-1) );
        } else {
            square->setBit( nullptr );
        }
    });
}


//
// this is the function that will be called by the AI
//
void ConnectFour::updateAI() 
{
    std::cout << "AI update called" << std::endl;
    int bestVal = -1000;
    BitHolder* bestMove = nullptr;
    std::string state = stateString();
    std::cout << "Current state: " << state << std::endl;

    // Traverse all cells, evaluate minimax function for all empty cells
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        std::cout << "Evaluating square at (" << x << ", " << y << ")" << std::endl;
        int index = y * 3 + x;
        std::cout << "Index: " << index << std::endl;
        std::cout << "len of state string: " << state.length() << std::endl;
        // Check if cell is empty
        if (state[index] == '0') {
            // Make the move
            state[index] = '2';
            int moveVal = -negamax(state, 0, HUMAN_PLAYER);
            // Undo the move
            state[index] = '0';
            // If the value of the current move is more than the best value, update best
            if (moveVal > bestVal) {
                bestMove = square;
                bestVal = moveVal;
            }
        }
    });

    std::cout << "Best move value: " << bestVal << std::endl;


    // Make the best move
    if(bestMove) {
        std::cout << "AI making move at position: (" << bestMove->getPosition().x << ", " << bestMove->getPosition().y << ")" << std::endl;
        if (actionForEmptyHolder(*bestMove)) {
        }
    }
}
// test 
bool isAiBoardFull(const std::string& state) {
    return false;
    //return state.find('0') == std::string::npos;
}

int evaluateAiBoard(const std::string& state) {
    return 0;
}

//
// player is the current player's number (AI or human)
//
int ConnectFour::negamax(std::string& state, int depth, int playerColor) 
{
    int score = evaluateAiBoard(state);

    // Check if AI wins, human wins, or draw
    if(score) { 
        // A winning state is a loss for the player whose turn it is.
        // The previous player made the winning move.
        return -score; 
    }

    if(isAiBoardFull(state)) {
        return 0; // Draw
    }

    int bestVal = -1000; // Min value
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            // Check if cell is empty
            if (state[y * 3 + x] == '0') {
                // Make the move
                state[y * 3 + x] = playerColor == HUMAN_PLAYER ? '1' : '2'; // Set the cell to the current player's color
                bestVal = std::max(bestVal, -negamax(state, depth + 1, -playerColor));
                // Undo the move for backtracking
                state[y * 3 + x] = '0';
            }
        }
    }

    return bestVal;
}