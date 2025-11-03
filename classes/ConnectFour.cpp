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
    bit->LoadTextureFromFile(playerNumber == AI_PLAYER ? "yellow.png" : "red.png");
    bit->setOwner(getPlayerAt(playerNumber == AI_PLAYER ? 1 : 0));
    return bit;
}

void ConnectFour::setUpBoard()
{
    setNumberOfPlayers(2);
    _gameOptions.rowX = 7;
    _gameOptions.rowY = 6;
    if(_gameOptions.playerTypeSelected)
    {
        _grid->initializeSquares(80, "square.png");

        if (gameHasAI() && (_gameOptions.playerVSAI == true || _gameOptions.AIvsAI == true)) {
            setAIPlayer(AI_PLAYER);
        }
        startGame();
    }

}

//
// about the only thing we need to actually fill out for tic-tac-toe
//
bool ConnectFour::actionForEmptyHolder(BitHolder &holder)
{
    ChessSquare* square = dynamic_cast<ChessSquare*>(&holder);
    Logger::GetInstance().LogGameEvent("Placing piece at position: (" + std::to_string(square->getColumn()) + ", " + std::to_string(  square->getRow()) + ")");
    int x = square->getColumn();
    int y = square->getRow();
    if(ownerAt(x,y) != nullptr)
    {
       return false;
    }
    Bit *bit = PieceForPlayer(getCurrentPlayer()->playerNumber() == 0 ? HUMAN_PLAYER : AI_PLAYER);
    if (bit) {

        ChessSquare *s = _grid->getSquare(x,0);
        int targetY = findLowestEmpty(x);
         ImVec2 pos = s->getPosition();
         bit->setPosition(pos); 
        // s->setBit(bit);
        ChessSquare *targetSquare = _grid->getSquare(x,targetY);
        Logger::GetInstance().LogGameEvent("Lowest Y position found at " + to_string(targetY));
        targetSquare->setBit(bit);
        pos = targetSquare->getPosition();
        bit->moveTo(pos);
        _lastMove.column = x;
        _lastMove.row = targetY; 
        Logger::GetInstance().LogGameEvent(" " + to_string(getCurrentPlayer()->playerNumber()) + "is placing piece in col: " +  to_string(_lastMove.column) + " in row: " +   to_string(_lastMove.row));
        Logger::GetInstance().LogGameEvent("Last placed piece at column " + to_string(_lastMove.column) + " Row: " + to_string(_lastMove.row));
;       endTurn();

        return true;
    }   
    return false;
}
int ConnectFour::findLowestEmpty(int column)
{
    for (int row = 5; row >= 0; --row) {
        if (ownerAt(column, row) == nullptr) {
            //Logger::GetInstance().LogGameEvent("Lowest Empty = column " + std::to_string(column)+"in row " + to_string(row));
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
    //Logger::GetInstance().LogGameEvent("testing x position " + to_string(row) +" with y" + to_string(col));
    if (row < 0 || row >= _gameOptions.rowY || col < 0 || col >= _gameOptions.rowX) {
        return nullptr;
    }

    auto square = _grid->getSquare(col, row);
    //Logger::GetInstance().LogGameEvent("Checking owner at position: (" + std::to_string(row) + ", " + std::to_string(col) + ")");
    //Logger::GetInstance().LogGameEvent("Square bit presence: " + std::string(square && square->bit() ? "Yes" : "No"));
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


int findAiLowestEmpty(std::string& state, int x) {
    // Start from the bottom row and go up
    for (int y = 5; y >= 0; y--) {
        if(state[y * 7 + x] == '0')
        {
            return y;
        }
    }
    return -1; // column full
}




//
// this is the function that will be called by the AI
//
void ConnectFour::updateAI() 
{
  //  testing to make sure statestring and grid match delete later
  //=========================================
    std::string teststate = stateString();
    std::cout << "AI board state:\n";
    for(int row = 0; row < 6; row++) {
        for(int col = 0; col < 7; col++) {
            std::cout << teststate[row * 7 + col] << " ";
        }
        std::cout << std::endl;
    }
//============================================
    std::cout << "AI update called" << std::endl;
    
    int bestVal = -1000;
    BitHolder* bestMove = nullptr;
    std::string state = stateString();
    //std::cout << "Current state: " << state << std::endl;

    // Traverse all cells, evaluate minimax function for all empty cells
    //_grid->forEachSquare([&](ChessSquare* square, int x, int y) { // only need to look at top row since drop will always fall to lowest piece. 
          // check for loswest position: 
            // Check if cell is empty
            int array[] = {3,2,4,1,5,0,6};
            int column;
    for (int x = 0; x < 7; x++) { // only need to look at top row since drop will always fall to lowest piece. 
          
        
        int column = array[x];
        // Start from the bottom row and go up
        int y = findAiLowestEmpty(state,column); 
        //std::cout << "Column " << column << " lowest empty row: " << y << std::endl;
        if(y != -1)
        {
            int index = y * 7 +column;
            // Check if cell is empty
            
            if (state[index] == '0') {
                // Make the move
                state[index] = '2';
                
                int moveVal = -negamax(state, 0, -1000, +1000,HUMAN_PLAYER); 
                // Undo the move
            
                // If the value of the current move is more than the best value, update best
                if (moveVal > bestVal) {
                    bestMove = _grid->getSquareByIndex(y * 7 +column);

                    bestVal = moveVal;
                    std::cout << "New best move: column " << column << " row " << y << " value: " << moveVal << std::endl; // testing new
                }
                state[index] = '0';
            }
        }
        
    std::cout << "Best move value: " << bestVal << std::endl;
    }
      // testing fall back and pick next open 
 
    // Make the best move
    if(bestMove) {
        std::cout << "AI making move at position: (" << bestMove->getPosition().x << ", " << bestMove->getPosition().y << ")" << std::endl;
        if (actionForEmptyHolder(*bestMove)) {
        }
    }
  // created a fall back incase bestmove does not exist;
   if (!bestMove) {
    for (int col = 0; col < 7; col++) {
        int y = findAiLowestEmpty(state, col);
        if (y != -1) {
            bestMove = _grid->getSquare(col, y);
            std::cout << "Fallback move at column " << col << " row " << y << std::endl;
            break;
        }
    }
}

}
// test 
bool isAiBoardFull(const std::string& state) {
    return state.find('0') == std::string::npos;
}

bool HorizontalBlocked(const std::string& state, int index, int size, int player)
{
    constexpr int WIDTH = 7;
    char enemyChar = (player == 1 ? '2' : '1');

    int row = index / WIDTH;
    int col = index % WIDTH;

    // Left end
    bool leftBlocked = false;
    int leftCol = col - 1;
    if (leftCol < 0) // off the board
        leftBlocked = true;
    else if (state[row * WIDTH + leftCol] == enemyChar)
        leftBlocked = true;

    // Right end
    bool rightBlocked = false;
    int rightCol = col + (size);
    if (rightCol >= WIDTH)
        rightBlocked = true;
    else if (state[row * WIDTH + rightCol] == enemyChar)
        rightBlocked = true;

    return leftBlocked && rightBlocked;
}

bool VerticalBlockedIn(const std::string& state, int index, int size, int player)
{
    constexpr int WIDTH = 7;
    constexpr int HEIGHT = 6;
    char enemyChar = (player == 1 ? '2' : '1');

    int row = index / WIDTH;
    int col = index % WIDTH;

    // Top end
    bool topBlocked = false;
    int topRow = row - 1;
    if (topRow < 0)
        topBlocked = true;
    else if (state[topRow * WIDTH + col] == enemyChar)
        topBlocked = true;

    // Bottom end
    bool bottomBlocked = false;
    int bottomRow = row + size;
    if (bottomRow >= HEIGHT)
        bottomBlocked = true;
    else if (state[bottomRow * WIDTH + col] == enemyChar)
        bottomBlocked = true;

    return topBlocked && bottomBlocked;
}

bool DiagonalUpRightBlockedIn(const std::string& state, int index, int size, int player)
{
    constexpr int WIDTH = 7;
    constexpr int HEIGHT = 6;
    char enemyChar = (player == 1 ? '2' : '1');

    int row = index / WIDTH;
    int col = index % WIDTH;

    // Top-right end
    int trRow = row - (size - 1);
    int trCol = col + (size - 1);
    bool topRightBlocked = (trRow < 0 || trCol >= WIDTH) || state[trRow * WIDTH + trCol] == enemyChar;

    // Bottom-left end
    int blRow = row + (size - 1);
    int blCol = col - (size - 1);
    bool bottomLeftBlocked = (blRow >= HEIGHT || blCol < 0) || state[blRow * WIDTH + blCol] == enemyChar;

    return topRightBlocked && bottomLeftBlocked;
}

bool DiagonalDownLeftBlockedIn(const std::string& state, int index, int size, int player)
{
    constexpr int WIDTH = 7;
    constexpr int HEIGHT = 6;
    char enemyChar = (player == 1 ? '2' : '1');

    int row = index / WIDTH;
    int col = index % WIDTH;

    // Bottom-left end
    int blRow = row + (size - 1);
    int blCol = col - (size - 1);
    bool bottomLeftBlocked = (blRow >= HEIGHT || blCol < 0) || state[blRow * WIDTH + blCol] == enemyChar;

    // Top-right end
    int trRow = row - (size - 1);
    int trCol = col + (size - 1);
    bool topRightBlocked = (trRow < 0 || trCol >= WIDTH) || state[trRow * WIDTH + trCol] == enemyChar;

    return bottomLeftBlocked && topRightBlocked;
}
int ConnectFour:: findNumberPieces(const std::string& state, int index, int parentPlayer, AiDirectionCheck direction)
{
    int pieceCount = 0;
    char player = parentPlayer == HUMAN_PLAYER ? '1' : '2'; // converts -1,1 player to 1,2 for state string

    switch(direction)
    {
        case HORIZONTAL:
            //checking for four in a row
            if(state[index] == player && 
                state[index+1] == player && 
                state[index+2] == player && 
                state[index+3] == player) 
            {
            return 4;
            }
            // check for 3 in a row:
            // check if XXXO or OXXX no other possible 3 grouping options
            else if((state[index] == player && state[index+1] == player && state[index+2] == player )||(state[index+1] == player && state[index+2] == player && state[index+3] == player ))
            {
                if(!HorizontalBlocked(state, index, player,3))
                {
                    return 3;
                }
                return 0;
                
            }
            // check for pairs XX00, or 00XX or 0XX0
            else if( (state[index] == player && state[index+1] == player) || (state[index+2] == player && state[index+3] == player) || (state[index+1 ] == player && state[index+2] == player)) 
            {
                 if(!HorizontalBlocked(state, index, player, 2))
                {
                    return 2;
                }
                return 0;
            }
             // if XOXO  or OXOX or blocked
            return 0;
        case VERTICAL:
            //checking for four in a row
            if((state[index] == player) && (state[index -7] == player) && (state[index -14] == player) && (state[index -21] == player))
            {
                
                return 4;
            }
            // check for verticle 3 
            else if(((state[index] == player) && (state[index -7] == player) && (state[index -14] == player)) || (state[index -7] == player && state[index -14] == player && state[index -21] == player ))
            {
                if(!VerticalBlockedIn(state, index, 3, player))
                {
                    return 3;
                }
                return 0;
            }
            else if((state[index] == player && state[index-7] == player) || (state[index-7] == player && state[index -14] == player) || (state[index -14] == player && state[index-21] == player))
            {
                 if(!VerticalBlockedIn(state, index, 2, player))
                {
                    return 2;
                }
                return 0;
            }
            return 0;
        case DIAGONAL_RIGHT:
            // -6 
            if(state[index] == player && state[index -6] == player && state[index -12] == player && state[index -18] == player)
            {
                return 4;
            }
            // check for verticle 3 
            else if((state[index] == player && state[index -6] == player && state[index -12] == player) ||state[index -6] == player && state[index -12] == player && state[index -18] == player )
            {
                if(!DiagonalUpRightBlockedIn(state, index, player, 3))
                {
                    return 3;
                }
                return 0;
            }
            else if((state[index] == player && state[index-6] == player) || (state[index-6] == player && state[index -12] == player) || (state[index -12] == player && state[index-18] == player))
            {
                if(!DiagonalUpRightBlockedIn(state, index, player, 2))
                {
                    return 2;
                }
                return 0;
            }
            else{
                return 0;
            }
        case DIAGONAL_LEFT:
            // +8 need to have a check to make sure not out of bounds. 
            if(state[index] == player && state[index +8] == player && state[index +16] == player && state[index +24] == player)
            {
                return 4;
            }
            // check for diagonal 3 
            else if((state[index] == player && state[index +8] == player && state[index +16] == player) ||state[index +8] == player && state[index +16] == player && state[index +24] == player )
            {
                if(!DiagonalDownLeftBlockedIn(state, index, player, 3))
                {
                    return 3;
                }
                return 0;
            }
            // check for diagonal pairs
            else if((state[index] == player && state[index+8] == player) || (state[index+8] == player && state[index +16] == player) || (state[index +16] == player && state[index+24] == player))
            {
                 if(!DiagonalDownLeftBlockedIn(state, index, player, 2))
                {
                    return 2;
                }
                return 0;
            }
            else{
                return 0;
            }
        default:
            Logger::GetInstance().LogError("Invalid ai direction in findNumberPieces");
            return 0;
    }
return 0;
}

//=========================================
int calculatePieceScore(int friendlyPieces, int enemyPieces)
{
  int score = 0;

    // Friendly sequences
    if (friendlyPieces == 4) {
        score += 1000; // AI wins
    } 
    else if (friendlyPieces == 3) {
        score += 100 ; // strong sequence, penalized by enemy
    } 
    else if (friendlyPieces == 2) {
        score += 10 ;   // medium sequence
    } 
    else if (friendlyPieces == 1) {
        score += 1;        // single piece slightly valuable
    }

    // Enemy sequences
    if (enemyPieces == 4) {
        score -= 1000; // Human wins
    } 
    else if (enemyPieces == 3) {
        score -= 100; // strong threat, penalized by AI
    } 
    else if (enemyPieces == 2) {
        score -= 10;   // medium threat
    } 
    else if (enemyPieces == 1) {
        score -= 1;        // single threat slightly dangerous
    }

    return score;
}

int ConnectFour::evaluateAiBoard(const std::string& state, int playerColor) {
     
    // count for  horiozntal sequences of pieces in a row and assign score
    // count for vertical sequences of pieces in a column and assign a score
    // diagonal sequences 
    // only count sequences with empty spaces to complete them 
    // 2 friendly in a row +10 points | 2 enemy in a row -10 points
    // 3 in a row + 100 points | 3 enemy in a row - 100 points
    // 4 in a row + 1000 points | 4 enemy in a row - 1000 points
    // total score = sum of all sequences on the board

    int slidingWindowSize = 4;

    // start sliding window in bottom left corner of board
    int score = 0;
    for(int row = 5; row > 2; row--)
    {
         for(int col = 0; col < _gameOptions.rowX-3; col++)
         {
            //cout << "Current 4x4 pivot created at Col: " + to_string(col) + " Row: " + to_string(row) << endl;
            int index = row * 7 + col;
            int tempIndex = index;
            int friendlyCount = 0;
            int enemyCount = 0; 
            int enemyPlayer = (playerColor == AI_PLAYER) ? HUMAN_PLAYER : AI_PLAYER;
                // check horizontal rows
                for(int k = 0; k < slidingWindowSize; k++)
                {
                    friendlyCount = findNumberPieces(state,tempIndex, playerColor, HORIZONTAL); // might need to look into if switching player color with negamax
                    enemyCount = findNumberPieces(state,tempIndex, enemyPlayer, HORIZONTAL);// test instead of enemy player it was -playerColor
                    score += calculatePieceScore(friendlyCount,enemyCount);
                    tempIndex -=7; // moves one row up and searches horizontal again 
                }

                tempIndex = index;
                for(int k = 0; k < slidingWindowSize; k++)
                {
                // check Vertical columns
                  friendlyCount = findNumberPieces(state,tempIndex, playerColor, VERTICAL); // look into alternating player piece 
                  enemyCount = findNumberPieces(state,tempIndex, enemyPlayer, VERTICAL);// test instead of enemy player it was -playerColor
                  score += calculatePieceScore(friendlyCount,enemyCount);
                  tempIndex += 1; // sets index to begin in next column 
                }
                 
                // check Diagonal right // "/ direction"
                tempIndex = index;
                friendlyCount = findNumberPieces(state,tempIndex,playerColor,DIAGONAL_RIGHT);
                enemyCount = findNumberPieces(state,tempIndex,enemyPlayer,DIAGONAL_RIGHT);// test instead of enemy player it was -playerColor
                score += calculatePieceScore(friendlyCount,enemyCount);

                // check diagonal left; "\ direction"
                tempIndex -= 21; // sets index to = top left of 4x4 window
                friendlyCount = findNumberPieces(state,tempIndex, playerColor,DIAGONAL_LEFT);
                enemyCount = findNumberPieces(state,tempIndex, enemyPlayer,DIAGONAL_LEFT);// test instead of enemy player it was -playerColor
                score += calculatePieceScore(friendlyCount,enemyCount);
               }
         }
    return score;
}


//
// player is the current player's number (AI or human)
//

 int ConnectFour::negamax(std::string& state, int depth, int alpha, int beta, int playerColor) 
{
    
    int score = evaluateAiBoard(state, playerColor); 
    // Check if AI wins, human wins, or draw
    if(abs(score) >= 1000) { 
        // A winning state is a loss for the player whose turn it is.
        // The previous player made the winning move.
        return score * playerColor;
    }

    if(depth == 4||isAiBoardFull(state)) {
        return score * playerColor; 
    }

    // thought that instead of having nested forloop for each open position instead make move based on only looking at top row then seeing where the piece falls to
    int bestVal = -1000; // Min value
    int array[] = {3,2,4,1,5,0,6};
    int column;
    for (int x = 0; x < 7; x++) { // only need to look at top row since drop will always fall to lowest piece. 
        int column = array[x];
        // check for lowest position: 
        int y = findAiLowestEmpty(state,column); 
        //std::cout << "Column " << column << " lowest empty row: " << y << std::endl;
          
        if(y != -1) // skip column if already filled.
        {
            if (state[y * 7 + column] == '0') { 
                // Make the move
                state[y * 7 + column] = (playerColor == HUMAN_PLAYER) ? '1' : '2'; 
                int result = -negamax(state, depth + 1, -beta, -alpha, -playerColor); 
                //Logger::GetInstance().LogGameEvent("A move at location : column: " + to_string(column) + " row: " + to_string(y) + "had a score of " + to_string(result));                if(result > bestVal)
                if(result > bestVal)
                {
                    bestVal = result;
                }
                if(result > alpha)
                {
                    alpha = result;
                }
                state[y * 7 + column] = '0';
                // Undo the move for backtracking
                if(alpha >= beta)
                {
                    break;
                }
                
            }
        }  
    }


    return bestVal;
}