#include "raylib.h"
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

const int GRID_SIZE = 10;
const int CELL_SIZE = 60;
const int SCREEN_SIZE = GRID_SIZE * CELL_SIZE;

enum RoomType
{
    EMPTY,
    START,
    REGULAR,
    END,
    BOSS
};

struct Cell
{
    int x;
    int y;
};

RoomType grid[GRID_SIZE][GRID_SIZE];


//IsInside the Grid because it might be negative or beyond the grid
bool IsInside(int x, int y)
{
    return x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE;
}

//Gets the orthogonal neighbors. dx and dy mean directionx and directiony
vector<Cell> GetNeighbors(Cell c)
{
    //Basically, from the point where the cell is, we check the up down left right
    //(from what i understood according to the demo too)
    vector<Cell> neighbors;

    int dx[4] = {1, -1, 0, 0};
    int dy[4] = {0, 0, 1, -1};

    for (int i = 0; i < 4; i++)
    {
        int nx = c.x + dx[i];
        int ny = c.y + dy[i];

        if (IsInside(nx, ny))
            neighbors.push_back({nx, ny});
    }

    return neighbors;
}

//We check if the neighbors are empty or not
int CountOccupiedNeighbors(Cell c)
{
    int count = 0;
    vector<Cell> neighbors = GetNeighbors(c);

    for (auto n : neighbors)
    {
        if (grid[n.x][n.y] != EMPTY)
            count++;
    }

    return count;
}

//Generating the dungeon based on the steps in the slides and the class demo 
void GenerateDungeon()
{
    //Clear the grid and reset the dungeon (for the R thing)
    for (int x = 0; x < GRID_SIZE; x++)
        for (int y = 0; y < GRID_SIZE; y++)
            grid[x][y] = EMPTY;

    //10-20 inclusive rooms
    int targetRooms = 10 + rand() % 11; 

    //get a random cell to make into the starting room
    int startX = rand() % GRID_SIZE;
    int startY = rand() % GRID_SIZE;

    grid[startX][startY] = START;

    queue<Cell> q;
    q.push({startX, startY});

    int roomCount = 1;

    while (roomCount < targetRooms)
    {

        Cell current = q.front();
        q.pop();

        bool createdRoom = false;

        vector<Cell> neighbors = GetNeighbors(current);

        //determines room type, either REGULAR or END
        for (auto n : neighbors)
        {
            if (grid[n.x][n.y] != EMPTY)
                continue;

            if (CountOccupiedNeighbors(n) > 1)
                continue;

            //50% chance of being a room or not
            if (rand() % 2 == 0)
            {
                grid[n.x][n.y] = REGULAR;
                roomCount++;
                createdRoom = true;

                q.push(n);

                //if the current cell WAS an END room, then it would become a regular room
                if (grid[current.x][current.y] == END)
                    grid[current.x][current.y] = REGULAR;

                if (roomCount >= targetRooms)
                    break;
            }
        }

        //if it did not produce a room, then make it an END room
        if (!createdRoom && grid[current.x][current.y] != START)
        {
            grid[current.x][current.y] = END;
            if (CountOccupiedNeighbors(current) == 1)
                grid[current.x][current.y] = END;
        }

        //if the queue is empty BUT we still dont have the target rooms, we readd the start and end
        //rooms back into the queue and do the generation stuff again
        if (q.empty())
        {
            for (int x = 0; x < GRID_SIZE; x++)
            {
                for (int y = 0; y < GRID_SIZE; y++)
                {
                    if (grid[x][y] == START || grid[x][y] == END)
                        q.push({x, y});
                }
            }
        }

    }

    //Convert all the 1 neighbor 1 entrance rooms into end rooms
    Cell startRoom;

    for (int x = 0; x < GRID_SIZE; x++)
    {
        for (int y = 0; y < GRID_SIZE; y++)
        {
            if (grid[x][y] == START)
                startRoom = {x, y};
        }
    }

    for (int x = 0; x < GRID_SIZE; x++)
    {
        for (int y = 0; y < GRID_SIZE; y++)
        {
            if (grid[x][y] == REGULAR || grid[x][y] == END)
            {
                int count = CountOccupiedNeighbors({x, y});

                //if the room only has 1 neighbor AND its not the startroom, then make it 
                //into an end room
                if (count == 1 && !(x == startRoom.x && y == startRoom.y))
                {
                    grid[x][y] = END;
                }
            }
        }
    }

}

Color GetRoomColor(RoomType type)
{
    switch (type)
    {
        case START: return GREEN;
        case REGULAR: return LIGHTGRAY;
        case END: return ORANGE;
        case BOSS: return RED;
        default: return DARKGRAY;
    }
}

int main()
{
    srand(time(NULL));

    InitWindow(SCREEN_SIZE, SCREEN_SIZE, "Dungeon Generator");

    GenerateDungeon();

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_R))
            GenerateDungeon();

        BeginDrawing();
        ClearBackground(BLACK);

        for (int x = 0; x < GRID_SIZE; x++)
        {
            for (int y = 0; y < GRID_SIZE; y++)
            {
                int drawX = x * CELL_SIZE;
                int drawY = y * CELL_SIZE;

                DrawRectangle(drawX, drawY, CELL_SIZE, CELL_SIZE, GetRoomColor(grid[x][y]));
                DrawRectangleLines(drawX, drawY, CELL_SIZE, CELL_SIZE, BLACK);

                //draw the neighbor count (can be removed in future)
                int neighbors = CountOccupiedNeighbors({x, y});
                DrawText(TextFormat("%d", neighbors), drawX + 20, drawY + 20, 20, WHITE);

            }
        }

        EndDrawing();
    }

    CloseWindow();
}

// clang++ Main.cpp libraylib.a -std=c++17 \-framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL -framework Foundation -o level
//  C:\raylib\w64devkit\w64devkit.exe
// HII SIRRR!! - Avielle: cd Documents/"[Y4] Second Semester 2026"/GDEV42/GDEV42 || g++ Main.cpp -o out -I raylib/ -L raylib/ -lraylib -lopengl32 -lgdi32 -lwinmm