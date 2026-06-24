/*****************************************************
 * Course: CS111  | Term: 2026 Spring
 * Instructor: zyliang@must.edu.mo
 * Homework Assignment: 3
 *****************************************************/


/*****************************************************
 * If you modified this file, provide the following information: 
 * GROUP INFORMATION (FILL IN ACCURATELY)
王子昂 D2
 * Notes on Modifications (optional): 
 * [Briefly describe the changes you made to this file]
 *****************************************************/



// !!!!!!!!!!!!!! < Hmk3 Task 9 > !!!!!!!!!!!!!!
// Provide the missing code of this file. You can design 
// the content of this file. #ifndef CS111_BOARD_GAME_CONNECT_HPP

#include "../Include/connect.hpp"
#include <queue>
#include <unordered_set>

namespace CS111::BOARD_GAME
{

Connect::Connect(int boardSize, const std::string& gameName, const std::vector<std::string>& playerNames)
    : GoLike(boardSize, gameName, playerNames)
{
    Board newBd = make_connect_board(boardSize, boardSize + 1);
    currentGameState_.board_ = newBd;
}

Board Connect::make_connect_board(int rows, int cols)
{
    Board bd(cols, rows);
    bd.set_mark_style(MarkStyle::Professional);
    bd.set_stone_position_style(StonePositionStyle::AtBlank);
    return bd;
}



bool Connect::check_and_handle_no_more_step()
{
    bool black_win = all_stone_linked(1);
    bool white_win = all_stone_linked(2);
    if(!black_win && !white_win)
        return false;
    auto& snap = currentGameState_;
    snap.gameStage_ = GameStage::FINISHED;
    if(black_win && white_win)
    {
        snap.winnerPlayerIndex_ = -1;
        snap.loserPlayerIndex_ = -1;
    }
    else if(black_win)
    {
        snap.winnerPlayerIndex_ = 1;
        snap.loserPlayerIndex_ = 2;
    }
    else
    {
        snap.winnerPlayerIndex_ = 2;
        snap.loserPlayerIndex_ = 1;
    }
    return true;
}

void Connect::end()
{
    auto& snap = currentGameState_;
    std::cout << "\n==== Connect Game End ====\n";
    if(snap.winnerPlayerIndex_ == -1)
        std::cout << "Draw! Both players connected all stones.\n";
    else
        std::cout << "Winner: " << snap.playerNames_[snap.winnerPlayerIndex_-1] << "\n";
    std::cout << "Total Steps: " << snap.stepHistory_.size() << "\n";
}
}
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
