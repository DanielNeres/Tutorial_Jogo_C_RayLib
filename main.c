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
    Vector2 pos;
    Vector2 dir;
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

typedef struct{
    Vector2 pos;
    float angulo;
} Projetil;

typedef struct no_lista_meteoro{
    Meteoro data;
    struct no_lista_meteoro *prox_no;
} No_meteoro;

typedef struct no_lista_projetil{
    Projetil data;
    struct no_lista_projetil *prox_no;
}No_projetil;


No_projetil *adicionar_no_prjetil(Projetil data, No_projetil *primeiro_no){
    No_projetil *novo_no = malloc(sizeof(No_projetil));
    if (novo_no == NULL){
        printf("erro\n\n");
        return primeiro_no;
    }
    novo_no->data = data;
    novo_no->prox_no = primeiro_no;
    return novo_no;    
}

No_meteoro *adicionar_no_meteoro(Meteoro data, No_meteoro *primeiro_no){
    No_meteoro *novo_no = malloc(sizeof(No_meteoro));
    if (novo_no == NULL){
        printf("erro\n\n");
        return primeiro_no;
    }
    novo_no->data = data;
    novo_no->prox_no = primeiro_no;
    return novo_no;    
}

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

No_meteoro *alocar_meteoros(No_meteoro *lista){
    for (int i = 0; i < MAX_AST; i++){
        Meteoro dado_novo_no;
        dado_novo_no.angulo = 0;
        dado_novo_no.pos.x = 0;
        dado_novo_no.pos.y = 0;
        dado_novo_no.dir.x = -1.0f + 2.0f * ((float)rand() / RAND_MAX);
        dado_novo_no.dir.y = -1.0f + 2.0f * ((float)rand() / RAND_MAX);
        dado_novo_no.ativo = true;
        dado_novo_no.vidas = 3;
        dado_novo_no.vel = 2.0f + 1.5f * ((float)rand() / RAND_MAX);
        dado_novo_no.vel_angulo = -2.0f + 4.0f * ((float)rand() / RAND_MAX);

        if (dado_novo_no.dir.x == 0){
            dado_novo_no.dir.x = 0.1f;
        }
        if (dado_novo_no.dir.y == 0){
            dado_novo_no.dir.y = 0.1f;
        }
        if (dado_novo_no.vel_angulo == 0){
            dado_novo_no.vel_angulo = 0.1f;
        }
        lista = adicionar_no_meteoro(dado_novo_no, lista);
    }
    return lista;
}

No_projetil *atirar(No_projetil *lista, float pos_x_nave, float pos_y_nave, float angulo, bool *erro) {
    if (IsKeyPressed(KEY_P)) {
       Projetil data_novo_no;
       data_novo_no.pos.x = pos_x_nave;
       data_novo_no.pos.y = pos_y_nave;
       data_novo_no.angulo = angulo;
       return adicionar_no_prjetil(data_novo_no, lista);
    }
    return lista;
}

void movimento_meteoro(No_meteoro *lista){

    No_meteoro *aux = lista;
    while (aux != NULL){
        
        aux->data.pos.x += aux->data.dir.x*aux->data.vel;
        aux->data.pos.y += aux->data.dir.y*aux->data.vel;
        manter_na_tela(&aux->data.pos.x, &aux->data.pos.y);
        aux->data.angulo += aux->data.vel_angulo;
        
        aux = aux->prox_no;
    }
}

void colisao_meteoro_bala(No_projetil *lista_projetil, No_meteoro *lista_meteoro, int *pontuacao){
    No_meteoro *aux_met = lista_meteoro;
    No_projetil *aux_proj = lista_projetil;
    while (aux_met != NULL){
        while (aux_proj != NULL){

            if (CheckCollisionCircles((Vector2){aux_met->data.pos.x, aux_met->data.pos.y}, RAIO_ASTEROID_GRANDE, (Vector2){aux_proj->data.pos.x, aux_proj->data.pos.y}, RAIO_BALA)){
                aux_met->data.vidas -= 1;
                aux_proj->data.pos.x += -10000;
                aux_proj->data.pos.y += -10000;
            };
            if (aux_met->data.vidas <= 0){
                *pontuacao += 10;
                aux_met->data.ativo = false;
            }

            aux_proj = aux_proj->prox_no;
        }
        
        aux_met = aux_met->prox_no;
    }
}

void desenhar_balas(No_projetil *lista, Texture2D spr_bala, Rectangle recorte_projetil, float vel_bala){

    No_projetil *aux = lista;
    while (aux != NULL){

        float angulo_rad = aux->data.angulo*(PI/180);
        aux->data.pos.x += cosf(angulo_rad)*vel_bala;
        aux->data.pos.y += sinf(angulo_rad)*vel_bala;

        Rectangle destino_projetil = {aux->data.pos.x, aux->data.pos.y, spr_bala.width*2.5, spr_bala.height*2.5};
        Vector2 origem_projetil = {destino_projetil.width/2, destino_projetil.height/2};

        DrawTexturePro(spr_bala, recorte_projetil, destino_projetil, origem_projetil, aux->data.angulo, WHITE);

        aux = aux->prox_no;
    }
}

void desenhar_meteoros(No_meteoro *lista, Texture2D spr_meteoro, Rectangle recorte_meteoro){
    
    No_meteoro *aux = lista;
    while (aux != NULL){
        Rectangle destino_meteoro = {aux->data.pos.x, aux->data.pos.y, spr_meteoro.width*3.7, spr_meteoro.height*3.7};
        Vector2 origem_meteoro = {destino_meteoro.width/2, destino_meteoro.height/2};
        DrawTexturePro(spr_meteoro, recorte_meteoro, destino_meteoro, origem_meteoro, aux->data.angulo, WHITE);
        aux = aux->prox_no;
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

    No_projetil *lista_projetil = NULL;
    float vel_bala = 8;

    // alocando os meteoros
    /*Meteoro meteoros[MAX_AST];
    alocar_meteoros(meteoros);*/
    
    No_meteoro *lista_meteoro = NULL;
    lista_meteoro = alocar_meteoros(lista_meteoro);

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
        lista_projetil = atirar(lista_projetil, pos_x_nave, pos_y_nave, angulo, &erro);
        movimento_meteoro(lista_meteoro);
        colisao_meteoro_bala(lista_projetil, lista_meteoro, &jogador_n1.pontuacao);

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
            desenhar_balas(lista_projetil, spr_bala, recorte_projetil, vel_bala);
            desenhar_meteoros(lista_meteoro, spr_meteoro, recorte_meteoro);
            
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
    No_meteoro *aux = lista_meteoro;
    while (aux != NULL) {
        No_meteoro *removido = aux;
        aux = aux->prox_no;
        free(removido);
    }
    No_projetil *aux_proj = lista_projetil;
    while (aux != NULL) {
        No_projetil *removido = aux_proj;
        aux = aux->prox_no;
        free(removido);
    }


    return 0;
}