#include <raylib.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define MAX_BALAS 50
#define MAX_AST 5

typedef struct{
    float pos_x;
    float pos_y;
    float dir_pos_x;
    float dir_pos_y;
    float angulo;
    int vidas;
    bool ativo;
} Meteoro;


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
    srand((unsigned int)time(NULL));
    InitWindow(2500, 1500, "primeira aula");


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

    Texture2D spr_meteoro = LoadTexture("assets/spr_asteroide_grande.png");
    if (!IsTextureValid(spr_meteoro)){
        erro = true;
    }
    Rectangle recorte_meteoro = {0, 0, spr_meteoro.width, spr_meteoro.height};

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

    // alocando os meteoros

    Meteoro meteoros[MAX_AST];
    for (int i = 0; i < MAX_AST; i++){
        meteoros[i].angulo = 0;
        meteoros[i].pos_x = 1000;
        meteoros[i].pos_y = 1000;
        meteoros[i].dir_pos_x = -1 + 2*(rand()/RAND_MAX);
        meteoros[i].dir_pos_y = -1 + 2*(rand()/RAND_MAX);
        meteoros[i].ativo = true;
        meteoros[i].vidas = 3;
    }
    float vel_meteoros = 3;
    float vel_rot_meteoros = 5;
    

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

        if (IsKeyPressed(KEY_P)){
            matriz_balas[bala_id][0] = pos_x_nave;
            matriz_balas[bala_id][1] = pos_y_nave;
            matriz_balas[bala_id][2] = angulo;
            bala_id++;
            if (bala_id >= tamanho_matriz){
                tamanho_matriz += 50;
                float **temp = realloc(matriz_balas, tamanho_matriz * sizeof(float *));
                if (temp != NULL){
                    matriz_balas = temp;        
                    for (int i = tamanho_matriz - 50; i < tamanho_matriz; i++){
                        matriz_balas[i] = calloc(3, sizeof(float));
                    }
                } else{
                    erro = true;
                }

            }
            
        }
        
        for (int i = 0; i < MAX_AST; i++){
            meteoros[i].pos_x += meteoros[i].dir_pos_x*vel_meteoros;
            meteoros[i].pos_y += meteoros[i].dir_pos_y*vel_meteoros;
            meteoros[i].angulo += vel_rot_meteoros;
            if (meteoros[i].angulo >= 360){
                meteoros[i].angulo = 0;
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

            ClearBackground((Color){0, 0, 40, 255});

            for (int i = 0; i < bala_id; i++){

                float angulo_rad = matriz_balas[i][2]*(PI/180);
                matriz_balas[i][0] += cosf(angulo_rad)*vel_bala;
                matriz_balas[i][1] += sinf(angulo_rad)*vel_bala;

                Rectangle destino_projetil = {matriz_balas[i][0], matriz_balas[i][1], spr_bala.width*5, spr_bala.height*5};
                Vector2 origem_projetil = {destino_projetil.width/2, destino_projetil.height/2};

                DrawTexturePro(spr_bala, recorte_projetil, destino_projetil, origem_projetil, matriz_balas[i][2], WHITE);
            }

            for (int i = 0; i < MAX_AST; i++){
                Rectangle destino_meteoro = {meteoros[i].pos_x, meteoros[i].pos_y, spr_meteoro.width*5, spr_meteoro.height*5};
                Vector2 origem_meteoro = {destino_meteoro.width/2, destino_meteoro.height/2};
                DrawTexturePro(spr_meteoro, recorte_meteoro, destino_meteoro, origem_meteoro, meteoros[i].angulo, WHITE);
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