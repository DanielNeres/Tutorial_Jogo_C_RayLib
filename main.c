#include <raylib.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define MAX_BALAS 50
#define MAX_AST 5
#define TELA_LARGURA 2500
#define TELA_ALTURA 1500
#define RAIO_NAVE 33
#define RAIO_BALA 8
#define RAIO_ASTEROID_GRANDE 55
#define MAX_ESTRELAS_GRANDES 200
#define MAX_ESTRELAS_PEQUENAS 950

typedef struct{
    float pos_x;
    float pos_y;
    float dir_pos_x;
    float dir_pos_y;
    float angulo;
    float vel_angulo;
    float vel;
    int vidas;
    bool ativo;
} Meteoro;

typedef struct {
    int pontuacao;
    char nome[40];
}Jogador;



void manter_na_tela(float *pos_x, float *pos_y){
    if (*pos_x >= TELA_LARGURA){
        *pos_x = 0;
    } else if (*pos_x <= 0){
        *pos_x = TELA_LARGURA;
    }
    if (*pos_y >= TELA_ALTURA){
        *pos_y = 0;
    } else if (*pos_y <= 0){
        *pos_y = TELA_ALTURA;
    }
}

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
    manter_na_tela(pos_x, pos_y);
}

void alocar_meteoros(Meteoro meteoros[MAX_AST]){
    for (int i = 0; i < MAX_AST; i++){
        meteoros[i].angulo = 0;
        meteoros[i].pos_x = 1000;
        meteoros[i].pos_y = 1000;
        meteoros[i].dir_pos_x = -1.0f + 2.0f * ((float)rand() / RAND_MAX);
        meteoros[i].dir_pos_y = -1.0f + 2.0f * ((float)rand() / RAND_MAX);
        meteoros[i].ativo = true;
        meteoros[i].vidas = 3;
        meteoros[i].vel = 4.0f + 3.0f * ((float)rand() / RAND_MAX);
        meteoros[i].vel_angulo = 6.0f + 6.0f * ((float)rand() / RAND_MAX) * (-2.0f + 4.0f * ((float)rand() / RAND_MAX));
    }
}

void atirar(float ***matriz_balas, int *bala_id, float pos_x_nave, float pos_y_nave, float angulo, int *tamanho_matriz, bool *erro) {
    if (IsKeyPressed(KEY_P)) {
        (*matriz_balas)[*bala_id][0] = pos_x_nave;
        (*matriz_balas)[*bala_id][1] = pos_y_nave;
        (*matriz_balas)[*bala_id][2] = angulo;
        
        (*bala_id)++;
        
        if (*bala_id >= *tamanho_matriz) {
            *tamanho_matriz += 50;
            float **temp = realloc(*matriz_balas, (*tamanho_matriz) * sizeof(float *));
            if (temp != NULL) {
                *matriz_balas = temp;
                for (int i = *tamanho_matriz - 50; i < *tamanho_matriz; i++) {
                    (*matriz_balas)[i] = calloc(3, sizeof(float));
                }
            } else {
                *erro = true;
            }
        }
    }
}

void movimento_meteoro(Meteoro meteoros[MAX_AST]){
    for (int i = 0; i < MAX_AST; i++){
        if (!meteoros[i].ativo){
            continue;
        }
        meteoros[i].pos_x += meteoros[i].dir_pos_x*meteoros[i].vel;
        meteoros[i].pos_y += meteoros[i].dir_pos_y*meteoros[i].vel;
        manter_na_tela(&meteoros[i].pos_x, &meteoros[i].pos_y);
        meteoros[i].angulo += meteoros[i].vel_angulo;
        if (meteoros[i].angulo >= 360){
            meteoros[i].angulo = 0;
        }
    }  
}

void colisao_meteoro_bala(float **matriz_balas, Meteoro meteoros[MAX_AST], int *pontuacao, int bala_id){
    for (int i = 0; i < MAX_AST; i++){
        if (!meteoros[i].ativo){
            continue;
        }
        
        for (int j = 0; j < bala_id; j++){
            if (CheckCollisionCircles((Vector2){meteoros[i].pos_x, meteoros[i].pos_y}, RAIO_ASTEROID_GRANDE, (Vector2){matriz_balas[j][0], matriz_balas[j][1]}, RAIO_BALA)){
                meteoros[i].vidas -= 1;
                matriz_balas[j][0] += -10000;
                matriz_balas[j][1] += -10000;
            };
            if (meteoros[i].vidas <= 0){
                *pontuacao += 10;
                meteoros[i].ativo = false;
            }
        }
    }
}

void desenhar_balas(float **matriz_balas, float bala_id, Texture2D spr_bala, Rectangle recorte_projetil, float vel_bala){
for (int i = 0; i < bala_id; i++){
        float angulo_rad = matriz_balas[i][2]*(PI/180);
        matriz_balas[i][0] += cosf(angulo_rad)*vel_bala;
        matriz_balas[i][1] += sinf(angulo_rad)*vel_bala;

        Rectangle destino_projetil = {matriz_balas[i][0], matriz_balas[i][1], spr_bala.width*5, spr_bala.height*5};
        Vector2 origem_projetil = {destino_projetil.width/2, destino_projetil.height/2};

        DrawTexturePro(spr_bala, recorte_projetil, destino_projetil, origem_projetil, matriz_balas[i][2], WHITE);
    }
}

void desenhar_meteoros(Meteoro meteoros[MAX_AST], Texture2D spr_meteoro, Rectangle recorte_meteoro){
    for (int i = 0; i < MAX_AST; i++){
        if (!meteoros[i].ativo){
            continue;
        }
        
        Rectangle destino_meteoro = {meteoros[i].pos_x, meteoros[i].pos_y, spr_meteoro.width*5, spr_meteoro.height*5};
        Vector2 origem_meteoro = {destino_meteoro.width/2, destino_meteoro.height/2};
        DrawTexturePro(spr_meteoro, recorte_meteoro, destino_meteoro, origem_meteoro, meteoros[i].angulo, WHITE);
    }
}

void desenhar_estrelas(Texture2D spr_estrela_grande, Texture2D spr_estrela_pequena,Vector2 estrelas_grandes[MAX_ESTRELAS_GRANDES], Vector2 estrelas_pequenas[MAX_ESTRELAS_PEQUENAS], Rectangle recorte_estrela_grande, Rectangle recorte_estrela_pequena){
    for (int i = 0; i < MAX_ESTRELAS_GRANDES; i++){
        
        Rectangle destino_estrela_grande = {estrelas_grandes[i].x, estrelas_grandes[i].y, spr_estrela_grande.width*3.5, spr_estrela_grande.height*3.5};
        DrawTexturePro(spr_estrela_grande, recorte_estrela_grande, destino_estrela_grande, (Vector2){0,0}, 0, WHITE);
    }
    for (int i = 0; i < MAX_ESTRELAS_PEQUENAS; i++){
        
        Rectangle destino_estrela_pequena = {estrelas_pequenas[i].x, estrelas_pequenas[i].y, spr_estrela_pequena.width*3.5, spr_estrela_pequena.height*3.5};
        DrawTexturePro(spr_estrela_pequena, recorte_estrela_pequena, destino_estrela_pequena, (Vector2){0,0}, 0, WHITE);
    }
}

int main(){
    srand((unsigned int)time(NULL));
    InitWindow(TELA_LARGURA, TELA_ALTURA, "primeira aula");

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

    Texture2D spr_estrela_grande = LoadTexture("assets/spr_estrela_grande.png");
    if (!IsTextureValid(spr_estrela_grande)){
        erro = true;
    }
    Rectangle recorte_estrela_grande = {0, 0, spr_estrela_grande.width, spr_estrela_grande.height};

    Texture2D spr_estrela_pequena = LoadTexture("assets/spr_estrela_pequena.png");
    if (!IsTextureValid(spr_estrela_pequena)){
        erro = true;
    }
    Rectangle recorte_estrela_pequena = {0, 0, spr_estrela_pequena.width, spr_estrela_pequena.height};

    Vector2 estrelas_grandes[MAX_ESTRELAS_GRANDES];
    Vector2 estrelas_pequenas[MAX_ESTRELAS_PEQUENAS];
    for (int i = 0; i < MAX_ESTRELAS_GRANDES; i++) {
        estrelas_grandes[i].x = ((float)rand() / (float)RAND_MAX) * TELA_LARGURA;
        estrelas_grandes[i].y = ((float)rand() / (float)RAND_MAX) * TELA_ALTURA;
    }
    for (int i = 0; i < MAX_ESTRELAS_PEQUENAS; i++) {
        estrelas_pequenas[i].x = ((float)rand() / (float)RAND_MAX) * TELA_LARGURA;
        estrelas_pequenas[i].y = ((float)rand() / (float)RAND_MAX) * TELA_ALTURA;
    }

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
    alocar_meteoros(meteoros);

    Jogador jogador_n1;
    strcpy(jogador_n1.nome, "nome");

    FILE *Arquivo_Leitura = fopen("save.dat", "rb");
    if (Arquivo_Leitura != NULL){
        fread(&jogador_n1.pontuacao, sizeof(int), 1, Arquivo_Leitura);
    }
    
    SetTargetFPS(60);

    while (!WindowShouldClose()){

        if (erro || IsKeyPressed(KEY_H)){
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
        atirar(&matriz_balas, &bala_id, pos_x_nave, pos_y_nave, angulo, &tamanho_matriz, &erro);
        movimento_meteoro(meteoros);
        colisao_meteoro_bala(matriz_balas, meteoros, &jogador_n1.pontuacao, bala_id);

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
            desenhar_estrelas(spr_estrela_grande, spr_estrela_pequena, estrelas_grandes, estrelas_pequenas, recorte_estrela_grande, recorte_estrela_pequena);
            desenhar_balas(matriz_balas, bala_id, spr_bala, recorte_projetil, vel_bala);
            desenhar_meteoros(meteoros, spr_meteoro, recorte_meteoro);
            
            DrawTexturePro(imagem_atual, recorte_atual, destino_atual, origem_atual, angulo, WHITE);
            DrawText(TextFormat("pontuação: %d", jogador_n1.pontuacao), 30, 30, 35, WHITE);

        EndDrawing();
    }

    FILE *Arquivo_Escrita = fopen("save.dat", "w");
    fwrite(&jogador_n1.pontuacao, sizeof(int), 1, Arquivo_Escrita);

    UnloadTexture(spr_nave);
    UnloadTexture(spr_nave_movendo);
    UnloadTexture(spr_meteoro);
    UnloadTexture(spr_bala);
    UnloadTexture(spr_estrela_grande);
    UnloadTexture(spr_estrela_pequena);
    CloseWindow();
    for (int i = 0; i < tamanho_matriz; i++){
        free(matriz_balas[i]);
    }
    free(matriz_balas);
    

    return 0;
}