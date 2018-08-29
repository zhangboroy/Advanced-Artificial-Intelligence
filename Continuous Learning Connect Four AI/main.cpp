#include <cstdlib>
#include "state.h"

//Set AI level
void AILevelSet(const int &algorithm, int &level)
{
    string garbage;

    int levelLimit=0;
    //set max level
    if (algorithm==1)
    {
        levelLimit=levelLimitMiniMax;
    }
    else
    {
        levelLimit=levelLimitAlphaBeta;
    }
    //ask to enter AI level
    do
    {
        cout<<"Please Choose Level (1- Easiest, ... , "<<levelLimit<<"- Hardest): "<<flush;
        cin>>level;
        if (!cin)
        {
            cin.clear();
            getline(cin, garbage);
            level=0;
        }
    }
    while (level<1 || level>levelLimit);
}

//Set player
void PlayerSet(int player[], int algorithm[], int level[], const int &playerNumber)
{
    string garbage;
    cout<<endl;
    //set player as human or AI
    do
    {
        cout<<"Please Choose Player "<<playerNumber+1<<" (1- Human, 2- Computer): "<<flush;
        cin>>player[playerNumber];
        if (!cin)
        {
            cin.clear();
            getline(cin, garbage);
            player[playerNumber]=0;
        }
    }
    while (player[playerNumber]<1 || player[playerNumber]>2);

    //Set AI algorithm
    if (player[playerNumber]==2)
    {
        //Set AI type
        do
        {
            cout<<"Please Choose Algorithm (1- MiniMax, 2- AlphaBeta): "<<flush;
            cin>>algorithm[playerNumber];
            if (!cin)
            {
                cin.clear();
                getline(cin, garbage);
                algorithm[playerNumber]=0;
            }
        }
        while (algorithm[playerNumber]<1 || algorithm[playerNumber]>2);

        //Set AI level
        AILevelSet(algorithm[playerNumber], level[playerNumber]);
    }
}

//the main menu
int Menu(int player[], int algorithm[], int level[], int &round)
{
    string garbage;
    int mode=0;
    cout<<"Welcome to Connect Four Game!\n\n";

    //set the mode
    do
    {
        cout<<"Please Choose Mode (1- Play, 2- AI auto-Training): "<<flush;
        cin>>mode;
        if (!cin)
        {
            cin.clear();
            getline(cin, garbage);
            mode=0;
        }
    }
    while (mode<1 || mode>2);

    //if play mode
    if (mode==1)
    {
        //set players
        PlayerSet(player, algorithm, level, 0);
        PlayerSet(player, algorithm, level, 1);
    }
    //if training mode
    else
    {
        //set parameters
        player[0]=2;
        player[1]=2;
        algorithm[0]=2;
        algorithm[1]=2;

        //Set training round
        while (round<1)
        {
            cout<<"Please Enter Training Round: "<<flush;
            cin>>round;
            if (!cin)
            {
                cin.clear();
                getline(cin, garbage);
                round=0;
            }
        }

        //Set training AI level
        cout<<"\nAI 1:\n";
        AILevelSet(algorithm[0], level[0]);
        cout<<"\nAI 2:\n";
        AILevelSet(algorithm[1], level[1]);
    }

    return mode;
}

//Load the performance list from the file
int ReadPerformanceList(set<unsigned __int64> &performanceList)
{
    unsigned __int64 tempID=0;
    inData.open("performanceList.pfm");
    while (inData>>tempID)
    {
        performanceList.insert(tempID);
    }
    inData.clear();
    inData.close();
    return performanceList.size();
}

//Save the performance list to the file and clear it
int ClearPerformanceList(set<unsigned __int64> &performanceList)
{
    outData.open("performanceList.pfm");
    for (set<unsigned __int64>::iterator i=performanceList.begin();i!=performanceList.end();++i)
    {
        outData<<*i<<endl;
    }
    outData.close();
    int listSize=performanceList.size();
    performanceList.clear();
    return listSize;
}

//Main Game
int Game(int player[], int algorithm[], int level[], set<unsigned __int64> &performanceList)
{
    string garbage;
    //initiate the state
    State state;
    int input=-1;
    int y=0;

    //output the state
    cout<<endl<<state<<endl;
    //rotate the 2 players
    do
    {
        //the X player
        y=-1;
        //if the player is human
        if (player[0]==1)
        {
            state.ChangePlayer();
            //let human choose column
            do
            {
                cout<<"Player "<<2-state.GetPlayer()<<" turn:\n";
                cin>>input;
                if (!cin)
                {
                    cin.clear();
                    getline(cin, garbage);
                    input=-1;
                }
                else if (input==0)
                {
                    break;
                }
                else
                {
                    y=state.DropDisc(input);
                    if (y==-1)
                    {
                        cout<<"Column "<<input<<" is full!\n";
                    }
                }
            }
            while (y==-1);

            //0 means exit
            if (input==0)
            {
                break;
            }
            //output the new state
            else
            {
                cout<<state<<endl;

                //if win
                if (state.CheckWin(input-1, y))
                {
                    cout<<"Player "<<2-state.GetPlayer()<<" wins!\n";
                    return 0;
                }
            }
        }
        //if the player is AI
        else
        {
            //plan the column
            if (algorithm[0]==1)
            {
                input=state.MiniMaxPlanning(level[0], performanceList)+1;
            }
            else
            {
                input=state.AlphaBetaPlanning(level[0], performanceList)+1;
            }
            state.ChangePlayer();
            y=state.DropDisc(input);
            cout<<state<<endl;

            //if win
            if (state.CheckWin(input-1, y))
            {
                cout<<"Player "<<2-state.GetPlayer()<<" wins!\n";
                return 0;
            }
        }

        //the O player
        //if the player is human
        if (player[1]==1)
        {
            state.ChangePlayer();
            //let human choose column
            do
            {
                cout<<"Player "<<2-state.GetPlayer()<<" turn:\n";
                cin>>input;
                if (!cin)
                {
                    cin.clear();
                    getline(cin, garbage);
                    input=-1;
                }
                else if (input==0)
                {
                    break;
                }
                else
                {
                    y=state.DropDisc(input);
                    if (y==-1)
                    {
                        cout<<"Column "<<input<<" is full!\n";
                    }
                }
            }
            while (y==-1);

            //0 means exit
            if (input==0)
            {
                break;
            }
            //output the new state
            else
            {
                cout<<state<<endl;
                //if win
                if (state.CheckWin(input-1, y))
                {
                    cout<<"Player "<<2-state.GetPlayer()<<" wins!\n";
                    return 1;
                }
                //if draw
                else if (y==0 && state.CheckDraw())
                {
                    cout<<"Draw game!\n";
                    break;
                }
            }
        }
        //if the player is AI
        else
        {
            //plan the column
            if (algorithm[1]==1)
            {
                input=state.MiniMaxPlanning(level[1], performanceList)+1;
            }
            else
            {
                input=state.AlphaBetaPlanning(level[1], performanceList)+1;
            }
            state.ChangePlayer();
            y=state.DropDisc(input);
            cout<<state<<endl;

            //if win
            if (state.CheckWin(input-1, y))
            {
                cout<<"Player "<<2-state.GetPlayer()<<" wins!\n";
                return 1;
            }
            //if draw
            else if (y==0 && state.CheckDraw())
            {
                cout<<"Draw game!\n";
                break;
            }
        }
    }
    while (input!=0);

    return -1;
}

int main()
{
    //initiate the parameters
    set<unsigned __int64> performanceList;
    int player[2]={0,0};
    int algorithm[2]={0,0};
    int level[2]={0,0};
    int round=0;

    //if play mode
    if (Menu(player, algorithm, level, round)==1)
    {
        //if any player is AI
        if (player[0]==2 || player[1]==2)
        {
            //Load the performance list from the file
            ReadPerformanceList(performanceList);
            Game(player, algorithm, level, performanceList);
            //Save the performance list to the file and clear it
            ClearPerformanceList(performanceList);
        }
        //if no player is AI
        else
        {
            Game(player, algorithm, level, performanceList);
        }
    }
    //if training mode
    else
    {
        //initiate the training result
        int result=-1;
        int win[2]={0,0};
        int initialListSize=0;
        int finalListSize=0;
        int newRecord=0;
        for (int i=0; i<round; ++i)
        {
            cout<<"\nRound "<<i+1<<":\n";
            //Load the performance list from the file and get the list size
            initialListSize=ReadPerformanceList(performanceList);

            //Train one round and record the winner
            result=Game(player, algorithm, level, performanceList);
            //if not draw
            if (result!=-1)
            {
                ++win[result];
            }
            //Save the performance list to the file, get the list size and clear it
            finalListSize=ClearPerformanceList(performanceList);
            newRecord+= finalListSize-initialListSize;
        }
        cout<<"\nTraining Summary:\n"
            <<"Total Games: "<<round<<endl
            <<"AI 1: "<<win[0]<<" wins\n"
            <<"AI 2: "<<win[1]<<" wins\n"
            <<"Draw: "<<round-win[0]-win[1]<<endl
            <<newRecord<<" new records saved.\n\n";
    }

    //wait to end the game
    system("pause");

    return 0;
}
