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
        int targetY = findLowestEmpty(x);
         ImVec2 pos = s->getPosition();
         bit->setPosition(pos); 
         s->setBit(bit);
        ChessSquare *targetSquare = _grid->getSquare(x,targetY);
        //Logger::GetInstance().LogGameEvent("Lowest Y position found at " + to_string(targetY));
        targetSquare->setBit(bit);
        pos = targetSquare->getPosition();
        bit->moveTo(pos);
        _lastMove.column = x;
        _lastMove.row = targetY;
        //Logger::GetInstance().LogGameEvent("Last placed piece at column " + to_string(_lastMove.column) + " Row: " + to_string(_lastMove.row));
;       endTurn();

        return true;
    }   
    return false;
}
int ConnectFour::findLowestEmpty(int column)
{
    for (int row = 5; row >= 0; --row) {
        if (ownerAt(column, row) == nullptr) {
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
Player* ConnectFour::ownerAt(int col, int row) const
{
    Logger::GetInstance().LogGameEvent("testing x position " + to_string(row) +" with y" + to_string(col));
    if (row < 0 || row >= _gameOptions.rowY || col < 0 || col >= _gameOptions.rowX) {
        return nullptr;
    }

    auto square = _grid->getSquare(col, row);
    Logger::GetInstance().LogGameEvent("Checking owner at position: (" + std::to_string(row) + ", " + std::to_string(col) + ")");
    Logger::GetInstance().LogGameEvent("Square bit presence: " + std::string(square && square->bit() ? "Yes" : "No"));
    if (!square || !square->bit()) {
        return nullptr;
    }
    return square->bit()->getOwner();

}
// Checks for a winner after the last move was made. 
Player* ConnectFour::checkForWinner()
{
    int horizontalScore = 0; 
    int verticleScore = 0;
    int diagonalRightScore = 0;
    int diagonalLeftScore = 0;
    // check recursive right + recursive left
    

    horizontalScore = checkFourInDirection(_lastMove.column, _lastMove.row, RIGHT) + checkFourInDirection(_lastMove.column, _lastMove.row, LEFT) +1; 

    // check recursive up + recursive down
    verticleScore = checkFourInDirection(_lastMove.column, _lastMove.row, UP) + checkFourInDirection(_lastMove.column, _lastMove.row, DOWN) +1;
    // check recursive diagonal up-right + down left
    diagonalRightScore = checkFourInDirection(_lastMove.column, _lastMove.row, UP_RIGHT) + checkFourInDirection(_lastMove.column, _lastMove.row, DOWN_LEFT) +1;
   // check recursive diagonal up-left + down right
    diagonalLeftScore = checkFourInDirection(_lastMove.column, _lastMove.row, UP_LEFT) + checkFourInDirection(_lastMove.column, _lastMove.row, DOWN_RIGHT) +1;

    if(horizontalScore >= 4 || verticleScore >= 4|| diagonalRightScore >= 4 || diagonalLeftScore >= 4)
    {
        return ownerAt(_lastMove.column, _lastMove.row);
    }
    
    return nullptr;
}
// Recursive function that checks in a given direction for consecutive pieces. 
// 
int ConnectFour::checkFourInDirection(int startColumn, int startRow, Direction direction)
{
    int counter = 0; 
    Player* startOwner = ownerAt(startColumn, startRow);
    switch(direction){
        case RIGHT:
            if(startColumn +1 < _gameOptions.rowX )
            {
                if(ownerAt(startColumn +1, startRow) == startOwner)
                {
                    counter = 1 + checkFourInDirection(startColumn +1, startRow, RIGHT);
                }
            }
            break;
        case LEFT:
             if(startColumn -1 >= 0)
            {
                if(ownerAt(startColumn -1, startRow) == startOwner)
                {
                    counter = 1 + checkFourInDirection(startColumn -1, startRow, LEFT);
                }
            }
            break; 
        case DOWN:
            if(startRow +1 < _gameOptions.rowY )
            {
                if(ownerAt(startColumn, startRow+1) == startOwner)
                {
                    counter = 1 + checkFourInDirection(startColumn, startRow+1, DOWN);
                }
            }
            break;
        case UP:
            if(startRow -1 >= 0 )
            {
                if(ownerAt(startColumn, startRow-1) == startOwner)
                {
                    counter = 1 + checkFourInDirection(startColumn, startRow-1, UP);
                }
            }
            break;
        case UP_RIGHT:
            if(startColumn +1 < _gameOptions.rowX && startRow -1 >= 0 )
            {
                if(ownerAt(startColumn +1, startRow-1) == startOwner)
                {
                    counter = 1 + checkFourInDirection(startColumn +1, startRow-1, UP_RIGHT);
                }
            }
            break;
        case DOWN_LEFT:
             if(startColumn -1 >= 0 && startRow +1 < _gameOptions.rowY )
            {
                if(ownerAt(startColumn -1, startRow +1) == startOwner)
                {
                    counter = 1 + checkFourInDirection(startColumn -1, startRow +1, DOWN_LEFT);
                }
            }
            break;
        case UP_LEFT:
            if(startColumn -1 >= 0 && startRow -1 >= 0 )
            {
                if(ownerAt(startColumn -1, startRow -1) == startOwner)
                {
                    counter = 1 + checkFourInDirection(startColumn -1, startRow -1, UP_LEFT);
                }
            }
            break;

        case DOWN_RIGHT:
            if(startColumn +1 < _gameOptions.rowX && startRow +1 < _gameOptions.rowY)
            {
                    if(ownerAt(startColumn +1, startRow +1) == startOwner)
                    {
                        counter = 1 + checkFourInDirection(startColumn +1, startRow +1, DOWN_RIGHT);
                    }
            }
            break;
        default:
            Logger::GetInstance().LogError("Invalid direction in checkFourInDirection");
            return 0;
    }
    return counter;
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
    return "000000000000000000000000000000000000000000";
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string ConnectFour::stateString()
{
    std::string s = "000000000000000000000000000000000000000000";
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