
#include <iostream>
#include <Windows.h>
#include <list>
#include <thread>
using namespace std;


int nScreenWidth = 120;
int nScreenHeight = 30;

struct sSnakeSegment
{
    int x_pos;
    int y_pos;
};

int main()
{
    // Create Screen Buffer
    wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

   
    // Game Loop
    while(1)
    {
        list<sSnakeSegment> snake = {{60, 15}, {61,15}, {62, 15}, {63, 15}, {64, 15}, {65, 15}, {66, 15}, {67, 15}, {68, 15}, {69, 15}};
        int nFoodX = 30;
        int nFoodY = 15;
        int nScore = 0;
        int nSnakeDirection = 3; // 0->N 1->E 2->S 3->W
        bool bDead = false;
        bool bKeyLeft = false, bKeyRight = false, bKeyLeftOld = false, bKeyRightOld = false;

        while(!bDead)
        {
            // Timing & Input
            //this_thread::sleep_for(200ms);  This method is too primitive, essentially stops the game
            


            // Get Input
            auto t1 = chrono::system_clock::now();
            while((chrono::system_clock::now() - t1) < ((nSnakeDirection % 2 == 1) ? 120ms : 200ms))
            {
                bKeyLeft = (0x8000 & GetAsyncKeyState((unsigned char)('\x25'))) != 0;
                bKeyRight = (0x8000 & GetAsyncKeyState((unsigned char)('\x27'))) != 0;

                if(bKeyRight && !bKeyRightOld)
                {
                    nSnakeDirection++;
                    if(nSnakeDirection == 4)
                        nSnakeDirection = 0;
                }

                if(bKeyLeft && !bKeyLeftOld)
                {
                    nSnakeDirection--;
                    if(nSnakeDirection == -1)
                        nSnakeDirection = 3;
                }

                bKeyLeftOld = bKeyLeft;
                bKeyRightOld = bKeyRight;
            }
           
            // Game Logic
            switch(nSnakeDirection)
            {
            case 0: // UP
                snake.push_front({snake.front().x_pos, snake.front().y_pos - 1});
                break;

            case 1: // RIGHT
                snake.push_front({snake.front().x_pos + 1, snake.front().y_pos});
                break;

            case 2: // DOWN
                snake.push_front({snake.front().x_pos, snake.front().y_pos + 1});
                break;

            case 3: // LEFT
                snake.push_front({snake.front().x_pos - 1, snake.front().y_pos});
                break;
            }

            // Chop off Snake Tail
            snake.pop_back();

            //Collision Detection

            // COllision Detect Snake vs World
            if(snake.front().x_pos < 0 || snake.front().x_pos >= nScreenWidth)
                bDead = true;
            if(snake.front().y_pos < 3 || snake.front().y_pos >= nScreenHeight)
                bDead = true;

            // Collision Detect Snake vs Food
            if(snake.front().x_pos == nFoodX && snake.front().y_pos == nFoodY)
            {
                nScore++;
                while(screen[nFoodY * nScreenWidth + nFoodX] != L' ')
                {
                    nFoodX = rand() % nScreenWidth;
                    nFoodY = (rand() % (nScreenHeight - 3)) + 3;
                }

                for(int i = 0; i < 5; i++)
                {
                    snake.push_back({snake.back().x_pos, snake.back().y_pos});
                }
            }

            //Collision Detect Snake vs Snake
            for(list<sSnakeSegment>::iterator i = snake.begin(); i != snake.end(); i++)
                if(i != snake.begin() && i->x_pos == snake.front().x_pos && i->y_pos == snake.front().y_pos)
                    bDead = true;



            // Display to Player 
            // Clear Screen
            for(int i = 0; i < nScreenWidth * nScreenHeight; i++)
                screen[i] = L' ';

            // Setup Border
            for(int i = 0; i < nScreenWidth; i++)
            {
                screen[i] = L'=';
                screen[2 * nScreenWidth + i] = L'=';
            }
            wsprintf(&screen[nScreenWidth + 5], L"S N A K E | Score: %d", nScore);

            // Draw Snake Body
            for(auto s : snake)
                screen[s.y_pos * nScreenWidth + s.x_pos] = bDead ? L'+' : L'O';
            // Draw Snake Head
            screen[snake.front().y_pos * nScreenWidth + snake.front().x_pos] = bDead ? L'X' : L'@';

            // Draw Food
            screen[nFoodY * nScreenWidth + nFoodX] = L'%';

            //Snake Death
            if(bDead)
                wsprintf(&screen[15 * nScreenWidth + 40], L"    PRESS 'SPACE' TO RESTART");
            // Display Frame
            screen[nScreenWidth * nScreenHeight - 1] = '\0';
            WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, {0,0}, &dwBytesWritten);

            
        }
        //Wait for Space
        while((0x8000 & GetAsyncKeyState((unsigned char)('\x20'))) == 0);
    }
    
}


// TODO: Implement a local highscore system 
// TODO: Implement powerups: Speed Up, Slow Down, Multi-Fruit, etc.
// TODO: Put it online