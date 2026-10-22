#include <raylib.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define MAX_BALAS 50
#define MAX_AST 3
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
        meteoros[i].pos_x = 0;
        meteoros[i].pos_y = 0;
        meteoros[i].dir_pos_x = -1.0f + 2.0f * ((float)rand() / RAND_MAX);
        meteoros[i].dir_pos_y = -1.0f + 2.0f * ((float)rand() / RAND_MAX);
        meteoros[i].ativo = true;
        meteoros[i].vidas = 3;
        meteoros[i].vel = 2.0f + 1.5f * ((float)rand() / RAND_MAX);
        meteoros[i].vel_angulo = -2.0f + 4.0f * ((float)rand() / RAND_MAX);

        if (meteoros[i].dir_pos_x == 0){
            meteoros[i].dir_pos_x = 0.1f;
        }
        if (meteoros[i].dir_pos_y == 0){
            meteoros[i].dir_pos_y = 0.1f;
        }
        if (meteoros[i].vel_angulo == 0){
            meteoros[i].vel_angulo = 0.1f;
        }
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

        Rectangle destino_projetil = {matriz_balas[i][0], matriz_balas[i][1], spr_bala.width*2.5, spr_bala.height*2.5};
        Vector2 origem_projetil = {destino_projetil.width/2, destino_projetil.height/2};

        DrawTexturePro(spr_bala, recorte_projetil, destino_projetil, origem_projetil, matriz_balas[i][2], WHITE);
    }
}

void desenhar_meteoros(Meteoro meteoros[MAX_AST], Texture2D spr_meteoro, Rectangle recorte_meteoro){
    for (int i = 0; i < MAX_AST; i++){
        if (!meteoros[i].ativo){
            continue;
        }
        
        Rectangle destino_meteoro = {meteoros[i].pos_x, meteoros[i].pos_y, spr_meteoro.width*3.7, spr_meteoro.height*3.7};
        Vector2 origem_meteoro = {destino_meteoro.width/2, destino_meteoro.height/2};
        DrawTexturePro(spr_meteoro, recorte_meteoro, destino_meteoro, origem_meteoro, meteoros[i].angulo, WHITE);
    }
}

void desenhar_estrelas(Texture2D spr_estrela_grande, Texture2D spr_estrela_pequena,Vector2 estrelas_grandes[MAX_ESTRELAS_GRANDES], Vector2 estrelas_pequenas[MAX_ESTRELAS_PEQUENAS], Rectangle recorte_estrela_grande, Rectangle recorte_estrela_pequena){
    for (int i = 0; i < MAX_ESTRELAS_GRANDES; i++){
        
        Rectangle destino_estrela_grande = {estrelas_grandes[i].x, estrelas_grandes[i].y, spr_estrela_grande.width*2, spr_estrela_grande.height*2};
        DrawTexturePro(spr_estrela_grande, recorte_estrela_grande, destino_estrela_grande, (Vector2){0,0}, 0, WHITE);
    }
    for (int i = 0; i < MAX_ESTRELAS_PEQUENAS; i++){
        
        Rectangle destino_estrela_pequena = {estrelas_pequenas[i].x, estrelas_pequenas[i].y, spr_estrela_pequena.width*2, spr_estrela_pequena.height*2};
        DrawTexturePro(spr_estrela_pequena, recorte_estrela_pequena, destino_estrela_pequena, (Vector2){0,0}, 0, WHITE);
    }
}

void Insertion_Short_pontuacao(Jogador *jogadores, int qtd){

    for (int i = 1; i < qtd; ++i) {
        Jogador key = jogadores[i];
        int j = i - 1;

        while (j >= 0 && jogadores[j].pontuacao > key.pontuacao) {
            jogadores[j + 1] = jogadores[j];
            j = j - 1;
        }
        jogadores[j + 1] = key;
    }
}

void Insertion_Short_nome(Jogador *jogadores, int qtd){
    for (int i = 1; i < qtd; i++){
        Jogador atual = jogadores[i];
        int j = i - 1;

        while (j >= 0 && strcmp(jogadores[j].nome, atual.nome) > 0){
            jogadores[j + 1] = jogadores[j];
            j--;
        }
        jogadores[j + 1] = atual;
    }
}

int buscar_jogador(Jogador *jogadores, int qtd, const char *nome){
    int inicio = 0;
    int fim = qtd - 1;

    while (inicio <= fim){
        int meio = inicio + (fim - inicio)/2;
        int comparacao = strcmp(nome, jogadores[meio].nome);

        if (comparacao == 0){
            return meio;
        } else if (comparacao < 0){
            fim = meio - 1;
        } else{
            inicio = meio + 1;
        }
    }
    return -1;
}

Jogador *carregar_jogadores(int *qtd, bool *erro){
    *qtd = 0;
    Jogador *jogadores = malloc(sizeof(Jogador));
    if (jogadores == NULL){
        *erro = true;
        return NULL;
    }

    FILE *arquivo = fopen("save.dat", "rb");
    if (arquivo != NULL){
        fread(qtd, sizeof(int), 1, arquivo);
        jogadores = realloc(jogadores, (*qtd + 1) * sizeof(Jogador));
        if (jogadores == NULL){
            fclose(arquivo);
            *erro = true;
            return NULL;
        }
        fread(jogadores, sizeof(Jogador), *qtd, arquivo);
        fclose(arquivo);
    }

    Insertion_Short_nome(jogadores, *qtd);
    return jogadores;
}

bool tela_inicial(Jogador *jogador, Jogador *historico, int qtd, bool *mostrar_ranking){
    int tamanho_nome = strlen(jogador->nome);
    int tecla = GetCharPressed();
    while (tecla > 0){
        if (tecla >= 32 && tecla <= 126 &&
            tamanho_nome < (int)sizeof(jogador->nome) - 1){
            jogador->nome[tamanho_nome++] = (char)tecla;
            jogador->nome[tamanho_nome] = '\0';
        }
        tecla = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE) && tamanho_nome > 0){
        jogador->nome[--tamanho_nome] = '\0';
    }
    if (IsKeyPressed(KEY_TAB)){
        *mostrar_ranking = !*mostrar_ranking;
    }

    bool nome_valido = strspn(jogador->nome, " ") < strlen(jogador->nome);
    bool iniciar = IsKeyPressed(KEY_ENTER) && nome_valido;

    BeginDrawing();
        ClearBackground(BLACK);
        int x = TELA_LARGURA/2 - 600;
        DrawText("ASTEROIDS", x, 150, 80, WHITE);
        DrawText("Digite seu nome (ate 39 caracteres):", x, 270, 36, WHITE);
        DrawRectangle(x, 330, 1200, 80, (Color){20, 25, 45, 255});
        DrawText(jogador->nome, x + 20, 350, 36, WHITE);

        DrawText("ENTER: jogar | TAB: ranking | BACKSPACE: apagar", x, 510, 32, WHITE);
        if (*mostrar_ranking){
            Insertion_Short_pontuacao(historico, qtd);
            DrawText("RANKING - 10 maiores pontuacoes", x, 660, 36, SKYBLUE);
            for (int i = 0; i < 10 && i < qtd; i++){
                int posicao = qtd - 1 - i;
                DrawText(TextFormat("%d. %s - %d", i + 1, historico[posicao].nome, historico[posicao].pontuacao), x, 730 + i*50, 30, WHITE);
            }
            Insertion_Short_nome(historico, qtd);
        }
    EndDrawing();
    return iniciar;
}

bool salvar_jogadores(Jogador *jogadores, int qtd){
    FILE *arquivo = fopen("save.dat", "wb");
    if (arquivo == NULL){
        return false;
    }
    bool salvo = fwrite(&qtd, sizeof(int), 1, arquivo) == 1;
    if (salvo){
        salvo = fwrite(jogadores, sizeof(Jogador), qtd, arquivo) == (size_t)qtd;
    }
    if (fclose(arquivo) != 0){
        salvo = false;
    }
    return salvo;
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

    float pos_x_nave = 1250, pos_y_nave = 750;
    float velocidade_nave = 3.7;
    float vel_angulo_nave = 2.3;
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
    float vel_bala = 8;
    int tamanho_matriz = 50;

    // alocando os meteoros

    Meteoro meteoros[MAX_AST];
    alocar_meteoros(meteoros);

    Jogador jogador_n1 = {0};
    int quantidade_jogadores = 0;
    bool erro_save = false;
    Jogador *historico_jogadores = carregar_jogadores(&quantidade_jogadores, &erro_save);
    bool jogo_iniciado = false;
    bool mostrar_ranking = false;
    erro = erro || erro_save;

    SetTargetFPS(60);

    while (!WindowShouldClose()){

        if (!jogo_iniciado && !erro){
            jogo_iniciado = tela_inicial(&jogador_n1, historico_jogadores, quantidade_jogadores, &mostrar_ranking);
            continue;
        }

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

        Rectangle destino_nave_parada = {pos_x_nave, pos_y_nave, 3.7*recorte_nave.width, 3.7*recorte_nave.height};
        Vector2 origem_parada = {destino_nave_parada.width/2, destino_nave_parada.height/2};

        Rectangle destinno_nave_movendo = {pos_x_nave, pos_y_nave, 3.7*recorte_nave_movendo.width, 3.7*recorte_nave_movendo.height};
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

            ClearBackground((Color){2, 2, 12, 255});            
            desenhar_estrelas(spr_estrela_grande, spr_estrela_pequena, estrelas_grandes, estrelas_pequenas, recorte_estrela_grande, recorte_estrela_pequena);
            desenhar_balas(matriz_balas, bala_id, spr_bala, recorte_projetil, vel_bala);
            desenhar_meteoros(meteoros, spr_meteoro, recorte_meteoro);
            
            DrawTexturePro(imagem_atual, recorte_atual, destino_atual, origem_atual, angulo, WHITE);
            DrawText(TextFormat("%s", jogador_n1.nome), 30, 30, 35, WHITE);
            DrawText(TextFormat("pontuação: %d", jogador_n1.pontuacao), 30, 65, 35, WHITE);
            

        EndDrawing();
    }

    if (jogo_iniciado && !erro_save){
        int indice = buscar_jogador(historico_jogadores, quantidade_jogadores, jogador_n1.nome);
        if (indice < 0){
            indice = quantidade_jogadores++;
        } else if (jogador_n1.pontuacao > historico_jogadores[indice].pontuacao){
            historico_jogadores[indice] = jogador_n1;
        }
        
        historico_jogadores[indice] = jogador_n1;
        Insertion_Short_pontuacao(historico_jogadores, quantidade_jogadores);
        if (!salvar_jogadores(historico_jogadores, quantidade_jogadores)){
            fprintf(stderr, "Nao foi possivel salvar os jogadores.\n");
        }
    }
    free(historico_jogadores);

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