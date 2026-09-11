#include <raylib.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_BALAS 50


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

    bool erro = false;

    Texture2D spr_nave = LoadTexture("assets/spr_nave_parada.png");
    if (!IsTextureValid(spr_nave)){
        erro = true;
    }
    Rectangle recorte_nave = {0, 0, spr_nave.width,spr_nave.height};

    Texture2D spr_nave_movendo = LoadTexture("assets/spr_nave_movendo.png");
    if (!IsTextureValid(spr_nave_movendo)){
        erro = true;
    }
    Rectangle recorte_nave_movendo = {0, 0, spr_nave_movendo.width/5, spr_nave_movendo.height};

    Texture2D spr_bala = LoadTexture("assets/spr_nave_projetil.png");
    if (!IsTextureValid(spr_bala)){
        erro = true;
    }
    Rectangle recorte_projetil = {0, 0, spr_bala.width, spr_bala.height};

    float pos_x_nave = 750, pos_y_nave = 500;
    float velocidade_nave = 5;
    float vel_angulo_nave = 5;
    float angulo = 0;
    Texture2D imagem_atual = spr_nave;
    Rectangle recorte_atual = recorte_nave;
    Rectangle destino_atual;
    Vector2 origem_atual;

    int frame = 0;

    int vel_frame = 6;
    int frame_count = 0;

    float **matriz_balas = malloc(MAX_BALAS*sizeof(float *));

    if (matriz_balas != NULL){
            
        for (int i = 0; i < MAX_BALAS; i++){
            matriz_balas[i] = calloc(3, sizeof(float));
        }
    } else{
        erro = true;
    }

    int bala_id = 0;
    float vel_bala = 7;
    int tamanho_matriz = 50;

    SetTargetFPS(60);

    while (!WindowShouldClose()){

        if (erro){
            break;
        }
        
        if(frame_count > vel_frame){
            frame++;
            if(frame > 4){
                frame = 0;
            }
            
            frame_count = 0;
        }
        frame_count++;

        recorte_nave_movendo.x = frame*spr_nave_movendo.width/5;

        float pos_x_nave_atual, pos_y_nave_atual;
        pos_x_nave_atual = pos_x_nave; pos_y_nave_atual = pos_y_nave; 
        movimento_nave(&pos_x_nave, &pos_y_nave, velocidade_nave, &angulo, vel_angulo_nave);

        if (IsKeyDown(KEY_P)){
            matriz_balas[bala_id][0] = pos_x_nave;
            matriz_balas[bala_id][1] = pos_y_nave;
            matriz_balas[bala_id][2] = angulo;
            bala_id++;
            if (bala_id >= MAX_BALAS){
                tamanho_matriz += 50;
                realloc(matriz_balas, tamanho_matriz*sizeof(float *));
                if (matriz_balas != NULL){        
                    for (int i = tamanho_matriz - 50; i < tamanho_matriz; i++){
                        matriz_balas[i] = calloc(3, sizeof(float));
                    }
                } else{
                    erro = true;
                }

            }
            
        }
        

        Rectangle destino_nave_parada = {pos_x_nave, pos_y_nave, 5*recorte_nave.width, 5*recorte_nave.height};
        Vector2 origem_parada = {destino_nave_parada.width/2, destino_nave_parada.height/2};

        Rectangle destinno_nave_movendo = {pos_x_nave, pos_y_nave, 5*recorte_nave_movendo.width, 5*recorte_nave_movendo.height};
        Vector2 origem_movendo = {(destinno_nave_movendo.width + (destinno_nave_movendo.width - destino_nave_parada.width))/2, destinno_nave_movendo.height/2};

        if (pos_x_nave != pos_x_nave_atual || pos_y_nave != pos_y_nave_atual){
            imagem_atual = spr_nave_movendo;
            recorte_atual = recorte_nave_movendo;
            destino_atual = destinno_nave_movendo;
            origem_atual = origem_movendo;
        } else {
            imagem_atual = spr_nave;
            recorte_atual = recorte_nave;
            destino_atual = destino_nave_parada;
            origem_atual = origem_parada;
        }
        
        BeginDrawing();

            ClearBackground(WHITE);

            for (int i = 0; i < bala_id; i++){

                float angulo_rad = matriz_balas[i][2]*(PI/180);
                matriz_balas[i][0] += cosf(angulo_rad)*vel_bala;
                matriz_balas[i][1] += sinf(angulo_rad)*vel_bala;

                Rectangle destino_projetil = {matriz_balas[i][0], matriz_balas[i][1], spr_bala.width*5, spr_bala.height*5};
                Vector2 origem_projetil = {destino_projetil.width/2, destino_projetil.height/2};

                DrawTexturePro(spr_bala, recorte_projetil, destino_projetil, origem_projetil, matriz_balas[i][2], WHITE);
            }
            
            DrawTexturePro(imagem_atual, recorte_atual, destino_atual, origem_atual, angulo, WHITE);
            
            

        EndDrawing();
    }
    UnloadTexture(spr_nave);
    UnloadTexture(spr_nave_movendo);
    CloseWindow();
    for (int i = 0; i < tamanho_matriz; i++){
        free(matriz_balas[i]);
    }
    free(matriz_balas);
    

    return 0;
}