#include <raylib.h>
#include <math.h>
#include <stdbool.h>

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

    Texture2D nave = LoadTexture("assets/spr_nave_parada.png");
    if (!IsTextureValid(nave)){
    }
    Rectangle recorte_nave = {0, 0, nave.width, nave.height};
    Texture2D nave_movendo = LoadTexture("assets/spr_nave_movendo.png");
    if (!IsTextureValid(nave)){
    }
    Rectangle recorte_nave_movendo = {0, 0, nave_movendo.width/5, nave_movendo.height};

    float pos_x_nave = 750, pos_y_nave = 500;
    float velocidade_nave = 5;
    float vel_angulo_nave = 5;
    float angulo = 0;
    Texture2D imagem_atual = nave;
    Rectangle recorte_atual = recorte_nave;
    Rectangle destino_atual;
    Vector2 origem_atual;

    int frame = 0;

    int vel_frame = 6;
    int frame_count = 0;

    SetTargetFPS(60);

    while (!WindowShouldClose()){
        if(frame_count > vel_frame){
            frame++;
            if(frame > 4){
                frame = 0;
            }
            
            frame_count = 0;
        }
        frame_count++;

        recorte_nave_movendo.x = frame*nave_movendo.width/5;

        float pos_x_nave_atual, pos_y_nave_atual;
        pos_x_nave_atual = pos_x_nave; pos_y_nave_atual = pos_y_nave; 
        movimento_nave(&pos_x_nave, &pos_y_nave, velocidade_nave, &angulo, vel_angulo_nave);

        Rectangle destino_nave_parada = {pos_x_nave, pos_y_nave, 5*recorte_nave.width, 5*recorte_nave.height};
        Vector2 origem_parada = {destino_nave_parada.width/2, destino_nave_parada.height/2};

        Rectangle destinno_nave_movendo = {pos_x_nave, pos_y_nave, 5*recorte_nave.width, 5*recorte_nave.height};
        Vector2 origem_movendo = {destinno_nave_movendo.width/2, destinno_nave_movendo.height/2};

        if (pos_x_nave != pos_x_nave_atual || pos_y_nave != pos_y_nave_atual){
            movendo = true;
            imagem_atual = nave_movendo;
            recorte_atual = recorte_nave_movendo;
            destino_atual = destinno_nave_movendo;
            origem_atual = origem_movendo;
        } else {
            movendo = false;
            imagem_atual = nave;
            recorte_atual = recorte_nave;
            destino_atual = destino_nave_parada;
            origem_atual = origem_parada;
        }
        
        BeginDrawing();

            ClearBackground(WHITE);
            
            DrawTexturePro(imagem_atual, recorte_atual, destino_atual, origem_atual, angulo, WHITE);
        EndDrawing();
    }
    UnloadTexture(nave);
    CloseWindow();

    return 0;
}