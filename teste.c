#include <raylib.h>
#include <math.h>

// kleber
void movimento_nave(float *pos_x, float *pos_y, float vel, float *angulo, float vel_angulo){

    if (IsKeyDown(KEY_RIGHT)){
        *angulo += vel_angulo;
        if (*angulo >= 360){
            *angulo = 0;
        }
        
    } else if (IsKeyDown(KEY_LEFT)){
        *angulo -= vel_angulo;
        if (*angulo <= 0){
            *angulo = 360;
        }
    }

    if (IsKeyDown(KEY_UP)){

        float angulo_rad = *angulo*(PI/180);

        *pos_x += cosf(angulo_rad)*vel;
        *pos_y += sinf(angulo_rad)*vel;
    }
    
    
    

}

int main(){

    InitWindow(1500, 1000, "primeira aula");

    Texture2D nave = LoadTexture("spr_nave_parada.png");
    if (!IsTextureValid(nave)){
    }
    Rectangle recorte_nave = {0, 0, nave.width, nave.height};

    float pos_x_nave = 750, pos_y_nave = 500;
    float velocidade_nave = 5;
    float vel_angulo_nave = 5;
    float angulo = 0;

    SetTargetFPS(60);

    while (!WindowShouldClose()){

        movimento_nave(&pos_x_nave, &pos_y_nave, velocidade_nave, &angulo, vel_angulo_nave);
        
        
        BeginDrawing();

            ClearBackground(WHITE);
            Rectangle destino_nave = {pos_x_nave, pos_y_nave, 5*recorte_nave.width, 5*recorte_nave.height};
            Vector2 origem = {destino_nave.width/2, destino_nave.height/2};
            DrawTexturePro(nave, recorte_nave, destino_nave, origem, angulo, WHITE);

        EndDrawing();
    }
    
    UnloadTexture(nave);
    CloseWindow();


    return 0;
}