#include <sstream>
#include <vector>
#include "state.h"

static ostringstream oss;
sqlite3* State::DB=0;

bool Performance::operator< (const Performance &performance) const
{
    return (score<performance.score) || (score==performance.score && step>performance.step);
}

bool Performance::operator> (const Performance &performance) const
{
    return (score>performance.score) || (score==performance.score && step<performance.step);
}

//Read state's performance
StatePerformance::StatePerformance(const string data[3])
{
    player=stoi(data[0]);
    performance.score=stoi(data[1]);
    performance.step=stoi(data[2]);
}

//default constructor
State::State()
{
    id=0;
    player=false;
    place=new char[height*width];
    for (int i=0;i<height*width;++i)
    {
        place[i]='.';
    }
    emptyY=new int[width];
    for (int i=0;i<width;++i)
    {
        emptyY[i]=height-1;
    }
    step=0;
}

//copy constructor
State::State(State &state)
{
    id=state.id;
    player=1-state.GetPlayer();
    place=new char[height*width];
    for (int i=0;i<height*width;++i)
    {
        place[i]=state.GetPlace(i);
    }
    emptyY=new int[width];
    for (int i=0;i<width;++i)
    {
        emptyY[i]=state.emptyY[i];
    }
    step=state.GetStep()+1;
}

//drop the disc to certain column, return the position of the disc(-1 if the column is full)
int State::DropDisc(const int &column)
{
    //if the column is not full
    if (emptyY[column-1]>-1)
    {
        //set the id of the state
        if (player)
        {
            place[emptyY[column-1]*width+column-1]='X';
            id+=2*pow(2,height-1-emptyY[column-1])*pow(idRange,column-1);
        }
        else
        {
            place[emptyY[column-1]*width+column-1]='O';
            id+=pow(2,height-1-emptyY[column-1])*pow(idRange,column-1);
        }
        //update the emptyY
        --emptyY[column-1];
        return emptyY[column-1]+1;
    }
    //if the column is full
    else
    {
        return emptyY[column-1];
    }
}

//return the number of horizontal connect pieces at the position
int State::HorizontalConnect(const int &x, const int &y) const
{
    int length=1;
    char marker=' ';
    if (player)
    {
        marker='X';
    }
    else
    {
        marker='O';
    }

    int position=x+1;
    while (true)
    {
        if (position==width)
        {
            break;
        }
        else if (place[y*width+position]==marker)
        {
            ++length;
            ++position;
        }
        else
        {
            break;
        }
    }

    position=x-1;
    while (true)
    {
        if (position==-1)
        {
            break;
        }
        else if (place[y*width+position]==marker)
        {
            ++length;
            --position;
        }
        else
        {
            break;
        }
    }
    return length;
}

//return the number of vertical connect pieces at the position
int State::VerticalConnect(const int &x, const int &y) const
{
    int length=1;
    char marker=' ';
    if (player)
    {
        marker='X';
    }
    else
    {
        marker='O';
    }

    int position=y+1;
    while (true)
    {
        if (position==height)
        {
            break;
        }
        else if (place[position*width+x]==marker)
        {
            ++length;
            ++position;
        }
        else
        {
            break;
        }
    }

    position=y-1;
    while (true)
    {
        if (position==-1)
        {
            break;
        }
        else if (place[position*width+x]==marker)
        {
            ++length;
            --position;
        }
        else
        {
            break;
        }
    }
    return length;
}

//return the number of top-left-to-bottom-right connect pieces at the position
int State::DiagonalLeftConnect(const int &x, const int &y) const
{
    int length=1;
    char marker=' ';
    if (player)
    {
        marker='X';
    }
    else
    {
        marker='O';
    }

    int xPosition=x+1;
    int yPosition=y+1;
    while (true)
    {
        if (xPosition==width || yPosition==height)
        {
            break;
        }
        else if (place[yPosition*width+xPosition]==marker)
        {
            ++length;
            ++xPosition;
            ++yPosition;
        }
        else
        {
            break;
        }
    }

    xPosition=x-1;
    yPosition=y-1;
    while (true)
    {
        if (xPosition==-1 || yPosition==-1)
        {
            break;
        }
        else if (place[yPosition*width+xPosition]==marker)
        {
            ++length;
            --xPosition;
            --yPosition;
        }
        else
        {
            break;
        }
    }
    return length;
}

//return the number of top-right-to-bottom-left connect pieces at the position
int State::DiagonalRightConnect(const int &x, const int &y) const
{
    int length=1;
    char marker=' ';
    if (player)
    {
        marker='X';
    }
    else
    {
        marker='O';
    }

    int xPosition=x+1;
    int yPosition=y-1;
    while (true)
    {
        if (xPosition==width || yPosition==-1)
        {
            break;
        }
        else if (place[yPosition*width+xPosition]==marker)
        {
            ++length;
            ++xPosition;
            --yPosition;
        }
        else
        {
            break;
        }
    }

    xPosition=x-1;
    yPosition=y+1;
    while (true)
    {
        if (xPosition==-1 || yPosition==height)
        {
            break;
        }
        else if (place[yPosition*width+xPosition]==marker)
        {
            ++length;
            --xPosition;
            ++yPosition;
        }
        else
        {
            break;
        }
    }
    return length;
}

//check if the player wins
bool State::CheckWin(const int &x, const int &y) const
{
    return HorizontalConnect(x, y)>3 || VerticalConnect(x, y)>3 || DiagonalLeftConnect(x, y)>3 || DiagonalRightConnect(x, y)>3;
}

//check if the game ends in a draw
bool State::CheckDraw() const
{
    for (int i=0;i<width;++i)
    {
        if (place[i]=='.')
        {
            return false;
        }
    }
    return true;
}

//Record state performance
void State::RecordPerformance(const Performance &performance) const
{
    string sql="INSERT INTO Performance VALUES(" + to_string(id) + ", " + to_string(player) + ", " + to_string(performance.score)
                + ", " + to_string(performance.step) + ");";
    sqlite3_exec(DB, sql.c_str(), 0, 0, 0);
}

static int ReadPerformance(void* data, int argc, char** argv, char** azColName)
{
    string* temp=(string*)data;

    for (int i = 1; i < argc; i++)
    {
        temp[i-1] = argv[i];
    }

    return 0;
}

//evaluate the performance by MiniMax
Performance State::MiniMax(const int &x, const int &y, const int &stepLimit)
{
    //if N is a leaf
    //if win
    if (CheckWin(x,y))
    {
        Performance performance(2);
        performance.step=step;

        if (step%2==0)
        {
            return -performance;
        }
        else
        {
            return performance;
        }
    }
    //if draw
    else if (y==0 && CheckDraw())
    {
        Performance performance;
        return performance;
    }
    //if other leave
    else if (step==stepLimit)
    {
        Performance performance(1);

        if (step%2==0)
        {
            return -performance;
        }
        else
        {
            return performance;
        }
    }
    //if N is not a leaf
    else
    {
        //Initiate a new state
        State* newState=0;
        Performance tempMaxMin;
        if (step%2==0)
        {
            tempMaxMin.score=-10;
        }
        else
        {
            tempMaxMin.score=10;
        }
        Performance temp;
        string data[3]={"N", "N", "N"};

        //Check the columns 1 by 1
        int y1=-1;
        for (int i=0;i<width;++i)
        {
            newState=new State(*this);
            y1=(*newState).DropDisc(i+1);

            //If the column is not full
            if (y1>-1)
            {
                data[0]="N";
                sqlite3_exec(DB, ("SELECT * FROM Performance WHERE Id="+to_string((*newState).GetId())+";").c_str(), ReadPerformance, data, 0);

                //If the state is recorded for a predict performance or not recorded
                if (data[0]=="N")
                {
                    temp=(*newState).MiniMax(i, y1, stepLimit);
                }
                //If the recorded state is for a real performance
                else
                {
                    StatePerformance *newStatePerformance=0;
                    newStatePerformance=new StatePerformance(data);
                    if ((*newState).GetPlayer()==(*newStatePerformance).GetPlayer())
                    {
                        temp=-((*newStatePerformance).GetPerformance());
                    }
                    else
                    {
                        temp=(*newStatePerformance).GetPerformance();
                    }
                    delete newStatePerformance;
                    newStatePerformance=0;
                }

                //Update tempMaxMin
                if (step%2==0)
                {
                    if (temp>tempMaxMin)
                    {
                        tempMaxMin=temp;
                    }
                }
                else
                {
                    if (temp<tempMaxMin)
                    {
                        tempMaxMin=temp;
                    }
                }
            }

            //Discard the newState
            delete newState;
            newState=0;
        }

        return tempMaxMin;
    }
}

//AI column choice by MiniMax planning
int State::MiniMaxPlanning(const int &stepLimit)
{
    int column=-1;
    Performance Max(-10);
    //Performance temp;

    //Initiate a new state
    State* newState=0;

    //unrecorded columns
    vector<int> unknownColumns;
    string data[3]={"N", "N", "N"};

    //Check the columns 1 by 1
    int y=-1;

    for (int i=0;i<width;++i)
    {
        Performance temp;

        newState=new State(*this);
        y=(*newState).DropDisc(i+1);

        //If the column is not full
        if (y>-1)
        {
            data[0]="N";
            sqlite3_exec(DB, ("SELECT * FROM Performance WHERE Id="+to_string((*newState).GetId())+";").c_str(), ReadPerformance, data, 0);

            //If the state is not recorded
            if (data[0]=="N")
            {
                temp=(*newState).MiniMax(i, y, stepLimit);
                if (temp.score==1 || temp.score==-1)
                {
                    unknownColumns.push_back(i);
                }
            }
            //If the state is recorded
            else
            {
                StatePerformance *newStatePerformance=0;
                newStatePerformance=new StatePerformance(data);
                if ((*newState).GetPlayer()==(*newStatePerformance).GetPlayer())
                {
                    temp=-(*newStatePerformance).GetPerformance();
                }
                else
                {
                    temp=(*newStatePerformance).GetPerformance();
                }
                delete newStatePerformance;
                newStatePerformance=0;
            }

            //Update the decision(Max and column)
            if (temp>Max)
            {
                Max=temp;
                column=i;
            }
        }

        //Discard the newState
        delete newState;
        newState=0;
    }

    //Update or create the corresponding file
    if (Max.score!=1 && Max.score!=-1)
    {
        data[0]="N";
        sqlite3_exec(DB, ("SELECT * FROM Performance WHERE Id="+to_string(id)+";").c_str(), ReadPerformance, data, 0);
        if (data[0]=="N")
        {
            if (Max.step%2==0)
            {
                --(Max.step);
                RecordPerformance(-Max);
            }
            else
            {
                ++(Max.step);
                RecordPerformance(Max);
            }
        }
    }
    else
    {
        column=unknownColumns[rand()%unknownColumns.size()];
    }

    return column;
}

//evaluate the performance by AlphaBeta
Performance State::AlphaBeta(const int &x, const int &y, const int &stepLimit, const Performance &alpha, const Performance &beta)
{
    //if N is a leaf
    //if win
    if (CheckWin(x,y))
    {
        Performance performance(2);
        performance.step=step;

        if (step%2==0)
        {
            return -performance;
        }
        else
        {
            return performance;
        }
    }
    //if draw
    else if (y==0 && CheckDraw())
    {
        Performance performance;
        return performance;
    }
    //if other leave
    else if (step==stepLimit)
    {
        Performance performance(1);

        if (step%2==0)
        {
            return -performance;
        }
        else
        {
            return performance;
        }
    }
    //if N is not a leaf
    else
    {
        //Initiate a new state
        State* newState=0;

        //Initiate the arguments
        Performance alphaMax=alpha;
        Performance betaMin=beta;
        Performance temp;
        string data[3]={"N", "N", "N"};

        //Check the columns 1 by 1
        int y1=-1;
        for (int i=0;i<width;++i)
        {
            newState=new State(*this);
            y1=(*newState).DropDisc(i+1);

            //If the column is not full
            if (y1>-1)
            {
                data[0]="N";
                sqlite3_exec(DB, ("SELECT * FROM Performance WHERE Id="+to_string((*newState).GetId())+";").c_str(), ReadPerformance, data, 0);

                //If the state is not recorded
                if (data[0]=="N")
                {
                    temp=(*newState).AlphaBeta(i, y1, stepLimit, alphaMax, betaMin);
                }
                //If the state is recorded
                else
                {
                    StatePerformance *newStatePerformance=0;
                    newStatePerformance=new StatePerformance(data);
                    if ((*newState).GetPlayer()==(*newStatePerformance).GetPlayer())
                    {
                        temp=-((*newStatePerformance).GetPerformance());
                    }
                    else
                    {
                        temp=(*newStatePerformance).GetPerformance();
                    }
                    delete newStatePerformance;
                    newStatePerformance=0;
                }

                //Update alphaMax or betaMin
                if (step%2==0)
                {
                    if (temp>alphaMax)
                    {
                        alphaMax=temp;
                    }
                    if (!(alphaMax<betaMin))
                    {
                        //Discard the newState
                        delete newState;
                        newState=0;
                        break;
                    }
                }
                else
                {
                    if (temp<betaMin)
                    {
                        betaMin=temp;
                    }
                    if (!(alphaMax<betaMin))
                    {
                        //Discard the newState
                        delete newState;
                        newState=0;
                        break;
                    }
                }
            }

            //Discard the newState
            delete newState;
            newState=0;
        }

        if (step%2==0)
        {
            return alphaMax;
        }
        else
        {
            return betaMin;
        }
    }
}

//AI column choice by AlphaBeta planning
int State::AlphaBetaPlanning(const int &stepLimit)
{
    //Initiate the arguments
    int column=-1;
    Performance maxPerformance(-10);
    Performance alphaMax(-10);
    Performance betaMin(10);
    //Performance temp;

    //Initiate a new state
    State* newState=0;

    //unrecorded columns
    vector<int> unknownColumns;
    string data[3]={"N", "N", "N"};

    //Check the columns 1 by 1
    int y=-1;

    for (int i=0;i<width;++i)
    {
        Performance temp;

        newState=new State(*this);
        y=(*newState).DropDisc(i+1);

        //If the column is not full
        if (y>-1)
        {
            data[0]="N";
            sqlite3_exec(DB, ("SELECT * FROM Performance WHERE Id="+to_string((*newState).GetId())+";").c_str(), ReadPerformance, data, 0);

            //If the state is not recorded
            if (data[0]=="N")
            {
                temp=(*newState).AlphaBeta(i, y, stepLimit, alphaMax, betaMin);
                if (temp.score==1 || temp.score==-1)
                {
                    unknownColumns.push_back(i);
                }
            }
            //If the state is recorded
            else
            {
                StatePerformance *newStatePerformance=0;
                newStatePerformance=new StatePerformance(data);

                if ((*newState).GetPlayer()==(*newStatePerformance).GetPlayer())
                {
                    temp=-(*newStatePerformance).GetPerformance();
                }
                else
                {
                    temp=(*newStatePerformance).GetPerformance();
                }
                delete newStatePerformance;
                newStatePerformance=0;
            }

            //Update the decision(alphaMax and column)
            if (temp>maxPerformance)
            {
                maxPerformance=temp;
                column=i;
            }
        }

        //Discard the newState
        delete newState;
        newState=0;
    }

    //Update or create the corresponding file
    if (maxPerformance.score!=1 && maxPerformance.score!=-1)
    {
        data[0]="N";
        sqlite3_exec(DB, ("SELECT * FROM Performance WHERE Id="+to_string(id)+";").c_str(), ReadPerformance, data, 0);
        if (data[0]=="N")
        {
            if (maxPerformance.step%2==0)
            {
                --(maxPerformance.step);
                RecordPerformance(-maxPerformance);
            }
            else
            {
                ++(maxPerformance.step);
                RecordPerformance(maxPerformance);
            }
        }
    }
    else
    {
        column=unknownColumns[rand()%unknownColumns.size()];
    }

    return column;
}

ostream& operator<<(ostream &out, const State &state)
{
    out<<' ';
    for (int i=0;i<width;++i)
    {
        out<<i+1;
    }
    out<<endl;
    for (int i=0;i<height;++i)
    {
        out<<i+1;
        for (int j=0;j<width;++j)
        {
            out<<state.GetPlace(i*width+j);
        }
        out<<endl;
    }
    return out;
}
