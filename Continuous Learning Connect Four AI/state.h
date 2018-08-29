#ifndef STATE_H_INCLUDED
#define STATE_H_INCLUDED

#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include <set>

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
    //the number of four
    int four;
    //the number of steps
    int step;
    //the number of possible winning patterns
    int total;
    //the number of different types of potential winning patterns
    int number[4];
    //the draw flag
    bool draw;

    Performance(const int &four1=0)
    {
        four=four1;
        step=0;
        total=0;
        //the number of zero
        number[0]=0;
        //the number of one
        number[1]=0;
        //the number of two
        number[2]=0;
        //the number of three
        number[3]=0;
        draw=false;
    }

    Performance operator- () const
    {
        Performance performance;
        performance.four=-four;
        performance.step=-step;
        performance.number[0]=-number[0];
        performance.number[1]=-number[1];
        performance.number[2]=-number[2];
        performance.number[3]=-number[3];
        performance.total=-total;
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
    //calculate the horizontal winning patterns
    Performance HorizontalChoice(const bool &player1) const;
    //calculate the vertical winning patterns
    Performance VerticalChoice(const bool &player1) const;
    //calculate the top-left-to-bottom-right winning patterns
    Performance DiagonalLeftChoice(const bool &player1) const;
    //calculate the top-right-to-bottom-left winning patterns
    Performance DiagonalRightChoice(const bool &player1) const;
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
    Performance MiniMax(const int &x, const int &y, const int &stepLimit, const set<unsigned __int64> &performanceList);
    //AI column choice by MiniMax planning
    int MiniMaxPlanning(const int &stepLimit, set<unsigned __int64> &performanceList);
    //evaluate the performance by AlphaBeta
    Performance AlphaBeta(const int &x, const int &y, const int &stepLimit, const Performance &alpha, const Performance &beta, const set<unsigned __int64> &performanceList);
    //AI column choice by AlphaBeta planning
    int AlphaBetaPlanning(const int &stepLimit, set<unsigned __int64> &performanceList);
    //evaluate the performance
    void Evaluate(const int &x, const int &y, Performance &performance);
    //Record state performance
    void RecordPerformance(const Performance &temp, const int &stepLimit) const;
};

ostream& operator<<(ostream &out, const State &state);

//performance of the state to store in the disk
class StatePerformance
{
    //the steps foreseen
    int stepLimit;
    bool player;
    Performance performance;
public:
    //Read state's performance from corresponding file
    StatePerformance(const unsigned __int64 &newId);

    int GetStepLimit() const
    {
        return stepLimit;
    }
    bool GetPlayer() const
    {
        return player;
    }
    Performance GetPerformance() const
    {
        return performance;
    }
};

#endif // STATE_H_INCLUDED
