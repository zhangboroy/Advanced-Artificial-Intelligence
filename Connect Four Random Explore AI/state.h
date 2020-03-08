#ifndef STATE_H_INCLUDED
#define STATE_H_INCLUDED

#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <cstdlib>
#include <ctime>
#include "sqlite3.h"

using namespace std;

const int height=6;
const int width=7;
const int idRange=pow(2,height+1)-1;
const int levelLimitMiniMax=9;
const int levelLimitAlphaBeta=12;

static ofstream outData;
static ifstream inData;

//performance of the state
struct Performance
{
    //state score: 2~win, -2~lose, 0~draw, 1~unknown
    int score;
    //the number of steps
    int step;

    Performance(const int &score1=0)
    {
        score=score1;
        step=0;
    }

    Performance operator- () const
    {
        Performance performance;
        performance.score=-score;
        performance.step=-step;
        return performance;
    }
    bool operator<(const Performance &performance) const;
    bool operator>(const Performance &performance) const;
};

class State
{
    unsigned __int64 id;
    //indicate which player
    bool player;
    //indicate each position of the board
    char* place;
    //indicate the lowest empty position of each column
    int *emptyY;
    //the number of steps
    int step;
public:
    static sqlite3* DB;
    //default constructor
    State();
    //copy constructor
    State(State &state);
    ~State()
    {
        delete []place;
        place=0;
        delete []emptyY;
        emptyY=0;
    }
    unsigned __int64 GetId() const
    {
        return id;
    }
    bool GetPlayer() const
    {
        return player;
    }
    char GetPlace(const int &n) const
    {
        return place[n];
    }
    int GetStep() const
    {
        return step;
    }
    void SetId();
    void ChangePlayer()
    {
        player=1-player;
    }
    //drop the disc to certain column, return the position of the disc(-1 if the column is full)
    int DropDisc(const int &column);
    //return the number of horizontal connect pieces at the position
    int HorizontalConnect(const int &x, const int &y) const;
    //return the number of vertical connect pieces at the position
    int VerticalConnect(const int &x, const int &y) const;
    //return the number of top-left-to-bottom-right connect pieces at the position
    int DiagonalLeftConnect(const int &x, const int &y) const;
    //return the number of top-right-to-bottom-left connect pieces at the position
    int DiagonalRightConnect(const int &x, const int &y) const;
    //check if the player wins
    bool CheckWin(const int &x, const int &y) const;
    //check if the game ends in a draw
    bool CheckDraw() const;
    //evaluate the performance by MiniMax
    Performance MiniMax(const int &x, const int &y, const int &stepLimit);
    //AI column choice by MiniMax planning
    int MiniMaxPlanning(const int &stepLimit);
    //evaluate the performance by AlphaBeta
    Performance AlphaBeta(const int &x, const int &y, const int &stepLimit, const Performance &alpha, const Performance &beta);
    //AI column choice by AlphaBeta planning
    int AlphaBetaPlanning(const int &stepLimit);
    //Record state performance
    void RecordPerformance(const Performance &temp) const;
};

ostream& operator<<(ostream &out, const State &state);

//performance of the state to store in the disk
class StatePerformance
{
    bool player;
    Performance performance;
public:
    //Read state's performance
    StatePerformance(const string data[3]);

    bool GetPlayer() const
    {
        return player;
    }
    Performance GetPerformance() const
    {
        return performance;
    }

    int ReadPerformance(void* data, int argc, char** argv, char** azColName);
};

#endif // STATE_H_INCLUDED
