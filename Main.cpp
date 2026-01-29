#include <raylib.h>
#include <raymath.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

static ios_base::Init iostream_initializer;

const float WINDOW_WIDTH(800);
const float WINDOW_HEIGHT(600);

Vector2 minEdge = {-500, -500};
Vector2 maxEdge = {1548, 1032};

bool isStatic = false;
bool isZoomed = false;

struct Object {
    string name;
    int x;
    int y;
    bool found;
};

int main() {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "AlvarezCorpuzGregorio_Homework01");
    
    Vector2 position = {400, 300};

    Rectangle view = {0, 0, 0, 0};

    SetTargetFPS(60.0f);

    int cam_type = 0;
    Camera2D camera = {0};
    camera.target = position;
    camera.offset = {WINDOW_WIDTH/2, WINDOW_HEIGHT/2};

    bool selecting = false;     
    float selectSize = 100.0f;

    //for pointer
    float center_x = WINDOW_WIDTH / 2.0f;
    float center_y = WINDOW_HEIGHT / 2.0f;
    float pointerSize = 20.0f;

    //Setting up using the settings.txt
    ifstream file("settings.txt");
    string line, imageName, objectName;
    int x, y, minEdge_x, minEdge_y, maxEdge_x, maxEdge_y;
    int cam_w, cam_h;
    float drift, zoom;
    vector<Object> objects;

    while(getline(file, line)){
        istringstream stream(line); //https://www.geeksforgeeks.org/cpp/processing-strings-using-stdistringstream/
        string key;
        stream >> key; //this reads the first tokens, puts them in key so we can identify the type

        if(key == "IMAGE_NAME"){
            stream >> imageName; 
        }else if(key.rfind("OBJECT", 0) == 0){//https://cplusplus.com/reference/string/string/rfind/
            stream >> objectName >> x >> y;
            objects.push_back({objectName, x, y, false});
        }else if(key == "CAM_EDGES"){
            stream >> minEdge_x >> minEdge_y >> maxEdge_x >> maxEdge_y;
            minEdge.x = minEdge_x;
            minEdge.y = minEdge_y;
            maxEdge.x = maxEdge_x;
            maxEdge.y = maxEdge_y;
        } else if (key == "CAM_WINDOW") { //NOT USED YET
            stream >> cam_w >> cam_h;
            view.width = cam_w;
            view.height = cam_h;
        } else if (key == "CAM_DRIFT") { //NOT USED YET
            stream >> drift;
        } else if (key == "CAM_ZOOM") {
            stream >> zoom;
            camera.zoom = zoom;
        }
    }

    file.close();
    Texture2D background = LoadTexture(imageName.c_str());

    while (!WindowShouldClose()) {
        float delta_time = GetFrameTime();
        if (IsKeyPressed(KEY_ENTER)) {
            isZoomed = true;
            if (!isStatic) {
                // First enter: zoom in
                camera.zoom = 1.0f;
                isStatic = true;
                cout << "Zoomed in" << endl;
            } else if (!selecting) {
                // Second enter: selection time
                selecting = true;
                cout << "secod enter for select" << endl;
                for (auto& obj : objects) {
                    cout << obj.x << " " << obj.y << endl;
                    if(
                        (position.x > obj.x - selectSize/2) &&
                        (position.x < (obj.x + selectSize/2)) &&
                        (position.y > obj.y - selectSize/2) &&
                        (position.y < (obj.y + selectSize/2))
                    ){
                        obj.found = true;
                        cout << "Successfully selected " << obj.name << endl;
                    }
                }
            }
        }
        
        if(IsKeyPressed(KEY_TAB) && camera.zoom == 1.0f) {
            camera.zoom = 0.5f;
            isStatic = false;
            isZoomed = false;
            selecting = false;
        }

        if (!isStatic) {
            if(IsKeyDown(KEY_W)) {
                position.y -= 200 * delta_time;
            }
            if(IsKeyDown(KEY_A)) {
                position.x -= 200 * delta_time;
            }
            if(IsKeyDown(KEY_S)) {
                position.y += 200 * delta_time;
            }
            if(IsKeyDown(KEY_D)) {
                position.x += 200 * delta_time;
            }
        }

        //comment or uncomment for debugging to find the position of the stuff since this just prints the positions in the console
        //cout << position.x << " " << position.y << endl;

        // Camera drift; disabled if zoomed
        if (!isZoomed) {
            Vector2 diff = Vector2Subtract(position, camera.target);
            camera.target = Vector2Add(camera.target, Vector2Scale(diff, drift*delta_time));
            
        } else {
            camera.target = position;
        }

        // Clamps cursor to the world boundaries
        position = Vector2Clamp(position, {minEdge.x + 10, minEdge.y + 10}, {maxEdge.x - 10, maxEdge.y - 10});
        
        // Centers rectangle view
        view.x = camera.target.x - view.width / 2;
        view.y = camera.target.y - view.height / 2;

        // Keeps pointer inside rectangle view
        if (position.x < view.x) camera.target.x -= view.x - position.x;
        else if (position.x > view.x + view.width) camera.target.x += position.x - (view.x + view.width);

        if (position.y < view.y) camera.target.y -= view.y - position.y;
        else if (position.y > view.y + view.height) camera.target.y += position.y - (view.y + view.height);

        // Clamps camera to the world boundaries
        camera.target = Vector2Clamp(camera.target,
            {minEdge.x + (WINDOW_WIDTH/2)/camera.zoom, minEdge.y + (WINDOW_HEIGHT/2)/camera.zoom},
            {maxEdge.x - (WINDOW_WIDTH/2)/camera.zoom, maxEdge.y - (WINDOW_HEIGHT/2)/camera.zoom});

    
        BeginDrawing();
        BeginMode2D(camera);
        ClearBackground(MAROON);
        DrawTexture(background, minEdge.x, minEdge.y, WHITE);
        if (!isZoomed) DrawRectangleLinesEx(view, 10.0f, SKYBLUE);
        DrawCircle(position.x, position.y, 10.0f, BLUE);


        //comment or uncomment for debugging, this will add a green box so you can see the selection area
        for (auto& obj : objects) {
            DrawRectangle(obj.x - selectSize/2, obj.y - selectSize/2, selectSize, selectSize, GREEN);
            for (int i = 0; i < 5; i++)
            {
                DrawCircleLines(obj.x, obj.y, 100-i, ColorAlpha(RED, obj.found ? 1.0f : 0.0f));
            }
            
            DrawText(obj.name.c_str(), obj.x - 50, obj.y - 12, 25, ColorAlpha(RED, obj.found ? 1.0f : 0.0f));
        }

        EndMode2D();

        DrawRectangle(0, 0, WINDOW_WIDTH, 50, ColorAlpha(BLACK, 0.5));
        int count = 0;
        int found_objects = 0;
        for (Object obj : objects) {
            DrawText(obj.name.c_str(), 25+(count * ((WINDOW_WIDTH-20)/objects.size())), 10, 20, obj.found ? GRAY : WHITE);
            if (obj.found) {found_objects++;}
            count++;
        }
        if (found_objects == count) {
            ClearBackground(WHITE);
            DrawText("You Win!", WINDOW_WIDTH/2-200, WINDOW_HEIGHT/2-50, 100, BLACK);
        }

        EndDrawing();
    }

    UnloadTexture(background);
    CloseWindow();
    return 0;

}


// DELETE BEFORE SUBMISSION: clang++ Main.cpp libraylib.a -std=c++17 \-framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL -framework Foundation -o ispy
//  C:\raylib\w64devkit\w64devkit.exe
// HII SIRRR!! - Avielle: cd Documents/"[Y4] Second Semester 2026"/GDEV42/GDEV42 || g++ Main.cpp -o out -I raylib/ -L raylib/ -lraylib -lopengl32 -lgdi32 -lwinmm