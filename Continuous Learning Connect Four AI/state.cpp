#include <sstream>
#include "state.h"

static ostringstream oss;
static set<unsigned __int64>::iterator iter;

bool Performance::operator< (const Performance &performance) const
{
    //compare four fist
    if (four<performance.four)
    {
        return true;
    }
    else if (four>performance.four)
    {
        return false;
    }
    else if (four==0)
    {
        //compare total then
        if (total<performance.total)
        {
            return true;
        }
        else if (total>performance.total)
        {
            return false;
        }
        else
        {
            //then compare from number[3] to number[0]
            for (int i=3;i>-1;--i)
            {
                if (number[i]<performance.number[i])
                {
                    return true;
                }
                else if (number[i]>performance.number[i])
                {
                    return false;
                }
            }
            //compare step last
            return (step>performance.step);
        }
    }
    //if four are equal and not 0, compare step
    else
    {
        return (step>performance.step);
    }
}

bool Performance::operator> (const Performance &performance) const
{
    //compare four fist
    if (four>performance.four)
    {
        return true;
    }
    else if (four<performance.four)
    {
        return false;
    }
    else if (four==0)
    {
        //compare total then
        if (total>performance.total)
        {
            return true;
        }
        else if (total<performance.total)
        {
            return false;
        }
        else
        {
            //then compare from number[3] to number[0]
            for (int i=3;i>-1;--i)
            {
                if (number[i]>performance.number[i])
                {
                    return true;
                }
                else if (number[i]<performance.number[i])
                {
                    return false;
                }
            }
            //compare step last
            return (step<performance.step);
        }
    }
    //if four are equal and not 0, compare step
    else
    {
        return (step<performance.step);
    }
}

//Read state's performance from corresponding file
StatePerformance::StatePerformance(const unsigned __int64 &newId)
{
    oss<<newId;
    inData.open(("pfm\\"+oss.str()+".pfm").c_str());
    if (inData.is_open())
    {
        inData>>stepLimit>>player>>performance.four>>performance.step>>performance.total;
        for (int i=0;i<4;++i)
        {
            inData>>performance.number[i];
        }
    }

    inData.close();
    oss.str("");
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

//calculate the horizontal winning patterns
Performance State::HorizontalChoice(const bool &player1) const
{
    Performance performance1;

    char marker=' ';
    if (player1)
    {
        marker='O';
    }
    else
    {
        marker='X';
    }

    bool available=true;
    int emptySpace=0;

    for (int y=0;y<height;++y)
    {
        for (int x=0;x<width-3;++x)
        {
            available=true;
            emptySpace=0;
            for (int i=0;i<4;++i)
            {
                if (place[y*width+x+i]==marker)
                {
                    available=false;
                    x+=i;
                    break;
                }
                else if (place[y*width+x+i]=='.')
                {
                    ++emptySpace;
                }
            }
            if (available)
            {
                ++performance1.total;
                ++performance1.number[4-emptySpace];
            }
        }
    }

    return performance1;
}

//calculate the vertical winning patterns
Performance State::VerticalChoice(const bool &player1) const
{
    Performance performance1;

    char marker=' ';
    if (player1)
    {
        marker='O';
    }
    else
    {
        marker='X';
    }

    bool available=true;
    int emptySpace=0;

    for (int x=0;x<width;++x)
    {
        for (int y=0;y<height-3;++y)
        {
            available=true;
            emptySpace=0;
            for (int i=0;i<4;++i)
            {
                if (place[(y+i)*width+x]==marker)
                {
                    available=false;
                    y+=i;
                    break;
                }
                else if (place[(y+i)*width+x]=='.')
                {
                    ++emptySpace;
                }
            }
            if (available)
            {
                ++performance1.total;
                ++performance1.number[4-emptySpace];
            }
            else
            {
                break;
            }
        }
    }

    return performance1;
}

//calculate the top-left-to-bottom-right winning patterns
Performance State::DiagonalLeftChoice(const bool &player1) const
{
    Performance performance1;

    char marker=' ';
    if (player1)
    {
        marker='O';
    }
    else
    {
        marker='X';
    }

    bool available=true;
    int emptySpace=0;

    for (int i=4-height;i<width-3;++i)
    {
        for (int x=i,y=0;x<width-3 && y<height-3;++x,++y)
        {
            available=true;
            emptySpace=0;
            for (int j=0;j<4;++j)
            {
                if (x<0 || place[(y+j)*width+x+j]==marker)
                {
                    available=false;
                    x+=j;
                    y+=j;
                    break;
                }
                else if (place[(y+j)*width+x+j]=='.')
                {
                    ++emptySpace;
                }
            }
            if (available)
            {
                ++performance1.total;
                ++performance1.number[4-emptySpace];
            }
        }
    }

    return performance1;
}

//calculate the top-right-to-bottom-left winning patterns
Performance State::DiagonalRightChoice(const bool &player1) const
{
    Performance performance1;

    char marker=' ';
    if (player1)
    {
        marker='O';
    }
    else
    {
        marker='X';
    }

    bool available=true;
    int emptySpace=0;

    for (int i=width+height-5;i>2;--i)
    {
        for (int x=i,y=0;x>2 && y<height-3;--x,++y)
        {
            available=true;
            emptySpace=0;
            for (int j=0;j<4;++j)
            {
                if (x>width-1 || place[(y+j)*width+x-j]==marker)
                {
                    available=false;
                    x-=j;
                    y+=j;
                    break;
                }
                else if (place[(y+j)*width+x-j]=='.')
                {
                    ++emptySpace;
                }
            }
            if (available)
            {
                ++performance1.total;
                ++performance1.number[4-emptySpace];
            }
        }
    }

    return performance1;
}

//return the number of horizontal connect pieces at the position
int State::HorizontalConnect(const int &x, const int &y, int &toEnd) const
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
    toEnd=position-x;

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
int State::VerticalConnect(const int &x, const int &y, int &toEnd) const
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
    toEnd=position-y;

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
int State::DiagonalLeftConnect(const int &x, const int &y, int &toEnd) const
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
    toEnd=xPosition-x;

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
int State::DiagonalRightConnect(const int &x, const int &y, int &toEnd) const
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
    toEnd=xPosition-x;

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
    int position=0;
    return HorizontalConnect(x, y, position)>3 || VerticalConnect(x, y, position)>3 || DiagonalLeftConnect(x, y, position)>3 || DiagonalRightConnect(x, y, position)>3;
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

//evaluate the performance
void State::Evaluate(const int &x, const int &y, Performance &performance)
{
    Performance p1=HorizontalChoice(player);
    Performance p2=VerticalChoice(player);
    Performance p3=DiagonalLeftChoice(player);
    Performance p4=DiagonalRightChoice(player);
    Performance p5=HorizontalChoice(1-player);
    Performance p6=VerticalChoice(1-player);
    Performance p7=DiagonalLeftChoice(1-player);
    Performance p8=DiagonalRightChoice(1-player);
    performance.total=p1.total+p2.total+p3.total+p4.total-p5.total-p6.total-p7.total-p8.total;
    for (int i=0;i<4;++i)
    {
        performance.number[i]=p1.number[i]+p2.number[i]+p3.number[i]+p4.number[i]-p5.number[i]-p6.number[i]-p7.number[i]-p8.number[i];
    }
    performance.step=step;
}

//Record state performance
void State::RecordPerformance(const Performance &performance, const int &stepLimit) const
{
    oss<<id;
    outData.open(("pfm\\"+oss.str()+".pfm").c_str());

    outData<<stepLimit<<'\t'<<player<<'\t'<<performance.four<<'\t'<<performance.step<<'\t'<<performance.total;
    for (int j=0;j<4;++j)
    {
        outData<<'\t'<<performance.number[j];
    }

    outData.close();
    oss.str("");
}

//evaluate the performance by MiniMax
Performance State::MiniMax(const int &x, const int &y, const int &stepLimit, const set<unsigned __int64> &performanceList)
{
    //if N is a leaf
    //if win
    if (CheckWin(x,y))
    {
        Performance performance(1);
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
    //if other leave
    else if (step==stepLimit || (y==0 && CheckDraw()))
    {
        Performance performance;
        Evaluate(x, y, performance);

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
            tempMaxMin.four=-2;
        }
        else
        {
            tempMaxMin.four=2;
        }
        Performance temp;
        //Check the columns 1 by 1
        int y1=-1;
        for (int i=0;i<width;++i)
        {
            newState=new State(*this);
            y1=(*newState).DropDisc(i+1);

            //If the column is not full
            if (y1>-1)
            {
                iter=performanceList.find((*newState).GetId());

                //If the state is recorded for a predict performance or not recorded
                if (iter==performanceList.end())
                {
                    temp=(*newState).MiniMax(i, y1, stepLimit, performanceList);
                }
                //If the recorded state is for a real performance
                else
                {
                    StatePerformance *newStatePerformance=0;
                    newStatePerformance=new StatePerformance((*newState).GetId());
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
int State::MiniMaxPlanning(const int &stepLimit, set<unsigned __int64> &performanceList)
{
    int column=-1;
    Performance Max(-2);
    Performance temp;
    //Initiate a new state
    State* newState=0;
    //Check the columns 1 by 1
    int y=-1;
    for (int i=0;i<width;++i)
    {
        newState=new State(*this);
        y=(*newState).DropDisc(i+1);

        //If the column is not full
        if (y>-1)
        {
            iter=performanceList.find((*newState).GetId());

            //If the state is recorded for a predict performance or not recorded
            if (iter==performanceList.end())
            {
                temp=(*newState).MiniMax(i, y, stepLimit, performanceList);
            }
            //If the recorded state is for a real performance
            else
            {
                StatePerformance *newStatePerformance=0;
                newStatePerformance=new StatePerformance((*newState).GetId());
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
    if (Max.four!=0)
    {
        if (performanceList.find(id)==performanceList.end())
        {
            performanceList.insert(id);

            if (Max.step%2==0)
            {
                --(Max.step);
                RecordPerformance(-Max, stepLimit+1);
            }
            else
            {
                ++(Max.step);
                RecordPerformance(Max, stepLimit+1);
            }
        }
        else
        {
            StatePerformance *newStatePerformance=0;
            newStatePerformance=new StatePerformance(id);

            if ((*newStatePerformance).GetStepLimit()<=stepLimit)
            {
                if (Max.step%2==0)
                {
                    --(Max.step);
                    RecordPerformance(-Max, stepLimit+1);
                }
                else
                {
                    ++(Max.step);
                    RecordPerformance(Max, stepLimit+1);
                }
            }

            delete newStatePerformance;
            newStatePerformance=0;
        }
    }

    return column;
}

//evaluate the performance by AlphaBeta
Performance State::AlphaBeta(const int &x, const int &y, const int &stepLimit, const Performance &alpha, const Performance &beta, const set<unsigned __int64> &performanceList)
{
    //if N is a leaf
    //if win
    if (CheckWin(x,y))
    {
        Performance performance(1);
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
    //if other leave
    if (step==stepLimit || (y==0 && CheckDraw()))
    {
        Performance performance;
        Evaluate(x, y, performance);

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

        //Check the columns 1 by 1
        int y1=-1;
        for (int i=0;i<width;++i)
        {
            newState=new State(*this);
            y1=(*newState).DropDisc(i+1);

            //If the column is not full
            if (y1>-1)
            {
                iter=performanceList.find((*newState).GetId());

                //If the state is recorded for a predict performance or not recorded
                if (iter==performanceList.end())
                {
                    temp=(*newState).AlphaBeta(i, y1, stepLimit, alphaMax, betaMin, performanceList);
                }
                //If the recorded state is for a real performance
                else
                {
                    StatePerformance *newStatePerformance=0;
                    newStatePerformance=new StatePerformance((*newState).GetId());
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
int State::AlphaBetaPlanning(const int &stepLimit, set<unsigned __int64> &performanceList)
{
    //Initiate the arguments
    int column=-1;
    Performance alphaMax(-2);
    Performance betaMin(2);
    Performance temp;

    //Initiate a new state
    State* newState=0;

    //Check the columns 1 by 1
    int y=-1;
    for (int i=0;i<width;++i)
    {
        newState=new State(*this);
        y=(*newState).DropDisc(i+1);

        //If the column is not full
        if (y>-1)
        {
            iter=performanceList.find((*newState).GetId());

            //If the state is recorded for a predict performance or not recorded
            if (iter==performanceList.end())
            {
                temp=(*newState).AlphaBeta(i, y, stepLimit, alphaMax, betaMin, performanceList);
            }
            //If the recorded state is for a real performance
            else
            {
                StatePerformance *newStatePerformance=0;
                newStatePerformance=new StatePerformance((*newState).GetId());

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
            if (temp>alphaMax)
            {
                alphaMax=temp;
                column=i;
            }
        }

        //Discard the newState
        delete newState;
        newState=0;
    }

    //Update or create the corresponding file
    if (alphaMax.four!=0)
    {
        if (performanceList.find(id)==performanceList.end())
        {
            performanceList.insert(id);

            if (alphaMax.step%2==0)
            {
                --(alphaMax.step);
                RecordPerformance(-alphaMax, stepLimit+1);
            }
            else
            {
                ++(alphaMax.step);
                RecordPerformance(alphaMax, stepLimit+1);
            }
        }
        else
        {
            StatePerformance *newStatePerformance=0;
            newStatePerformance=new StatePerformance(id);

            if ((*newStatePerformance).GetStepLimit()<=stepLimit)
            {
                if (alphaMax.step%2==0)
                {
                    --(alphaMax.step);
                    RecordPerformance(-alphaMax, stepLimit+1);
                }
                else
                {
                    ++(alphaMax.step);
                    RecordPerformance(alphaMax, stepLimit+1);
                }
            }

            delete newStatePerformance;
            newStatePerformance=0;
        }
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
