
#include "PAL/PAL.h"

void check_collision_x(PAL_Rect& player, PAL_Rect& wall, f32 velocity);
void check_collision_y(PAL_Rect& player, PAL_Rect& wall, f32 velocity);

int main(int argc, char** argv)
{
    PAL_Init();

    u32 flags = PAL_WindowFlags_Center | PAL_WindowFlags_Show;
    PAL_Window* window = PAL_CreateWindow("window", 640, 480, flags);

    PAL_ContextDesc desc;
    desc.type = PAL_ContextTypes_OpenGL;
    desc.major = 3;
    desc.minor = 3;
    PAL_Context* context = PAL_CreateContext(window, desc);
    PAL_MakeActive(context);
    PAL_SetVsync(true);

    PAL_Renderer* renderer = PAL_CreateRenderer();

    PAL_Camera camera;
    camera.view = { 0.0f, 0.0f, 640.0f, 480.0f };
    camera.rotation = 0.0f;

    PAL_Rect player, wall;
    player.x = 200.0f;
    player.y = 200.0f;
    player.w = 50.0f;
    player.h = 50.0f;

    wall.x = 400.0f;
    wall.y = 300.0f;
    wall.w = 50.0f;
    wall.h = 50.0f;

    f32 speed = 4.0f;
   
    while (!PAL_WindowShouldClose(window)) {
        PAL_PullEvents();

        f32 velocity_x = 0.0f;
        f32 velocity_y = 0.0f;

        PAL_Clear({ .2f, .2f, .2f, 1.0f });
        PAL_SetRendererCamera(renderer, camera);

        if (PAL_GetKeyState(window, PAL_Keys_Right)) {
            velocity_x = speed;
        }

        else if (PAL_GetKeyState(window, PAL_Keys_Left)) {
            velocity_x = -speed;
        }

        else if (PAL_GetKeyState(window, PAL_Keys_Down)) {
            velocity_y = speed;
        }

        else if (PAL_GetKeyState(window, PAL_Keys_Up)) {
            velocity_y = -speed;
        }

        player.x += velocity_x;
        check_collision_x(player, wall, velocity_x);
        player.y += velocity_y;
        check_collision_y(player, wall, velocity_y);

        PAL_RendererDrawRect(renderer, player, PAL_RED);
        PAL_RendererDrawRect(renderer, wall, PAL_GREEN);


        PAL_RendererFlush(renderer);
        PAL_SwapBuffers(); 
    }

    PAL_DestroyRenderer(renderer);
    PAL_DestroyContext(context);
    PAL_DestroyWindow(window);
    PAL_Shutdown();
    return 0;
}

void check_collision_x(PAL_Rect& player, PAL_Rect& wall, f32 velocity)
{
    if (PAL_RectCollide(player, wall)) {
        if (velocity > 0.0f) { // moving right
            player.x = wall.x - player.w;
        }
        else if (velocity < 0.0f) { // moving left
            player.x = wall.x + player.w;
        }
    }
}

void check_collision_y(PAL_Rect& player, PAL_Rect& wall, f32 velocity)
{
    if (PAL_RectCollide(player, wall)) {
        if (velocity > 0.0f) { // moving down
            player.y = wall.y - player.h;
        }
        else if (velocity < 0.0f) { // moving up
            player.y = wall.y + player.h;
        }
    }
}