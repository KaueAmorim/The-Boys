// programa principal do projeto "The Boys - 2024/2"
// Autor: Kauê de Amorim Silva, GRR 20244719

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "theboys.h"

int aleat (int min, int max){

    return rand() % (max - min + 1) + min;
}

float distancia_cartesiana (struct coordenada B, struct coordenada D){

    return sqrt((B.x - D.x)*(B.x - D.x) + (B.y - D.y)*(B.y - D.y));
}

void fila_maxi (struct mundo *M, int B){

    int tam;

    tam = lista_tamanho(M->bases[B].fila);

    if(tam > M->bases[B].fila_max){
        M->bases[B].fila_max = tam;
    }
}

int particiona (struct dist_bases vetor[], struct dist_bases x, int inicio, int fim){

    int m;
    struct dist_bases troca;

    m = inicio - 1;

    for(int i = inicio; i <= fim; i++){
        if(vetor[i].dist <= x.dist){
            m++;
            troca = vetor[m];
            vetor[m] = vetor[i];
            vetor[i] = troca;
        }
    }

    return m;
}

void ordena_vetor (struct dist_bases vetor[], int inicio, int fim){

    int m;

    if(inicio < fim){

        m = particiona(vetor, vetor[fim], inicio, fim);

        ordena_vetor(vetor, inicio, m - 1);
        ordena_vetor(vetor, m + 1, fim);
    }
}

/*--------------------inicialização das estruturas---------------------*/

struct heroi heroi_cria (struct mundo *M, int id){

    struct heroi h;

    h.id = id;
    h.exp = 0;
    h.paci = aleat(0, 100);
    h.vel = aleat(50, 5000);
    h.hab = cjto_aleat(aleat(1, 3), M->num_hab);
    h.base = -1;
    h.morto = 0;

    return h;
}

struct base base_cria (struct mundo *M, int id){

    struct base b;

    b.id = id;
    b.local.x = aleat(0, M->tam_mundo - 1);
    b.local.y = aleat(0, M->tam_mundo - 1);
    b.lot = aleat(3, 10);
    b.pres = cjto_cria(M->num_herois);
    b.fila = lista_cria();
    b.fila_max = 0;
    b.num_missoes = 0;

    return b;
}

struct missao missao_cria (struct mundo *M, int id){

    struct missao ms;

    ms.id = id;
    ms.local.x = aleat(0, M->tam_mundo - 1);
    ms.local.y = aleat(0, M->tam_mundo - 1);
    ms.hab = cjto_aleat(aleat(6, 10), M->num_hab);
    ms.perigo = aleat(0, 100);
    ms.tentativa = 1;

    return ms;
}

struct mundo mundo_cria (){

    int i;
    struct mundo M;

    M.num_herois = N_HEROIS;
    M.num_bases = N_BASES;
    M.num_hab = N_HABILIDADES;
    M.num_missoes = N_MISSOES;
    M.tam_mundo = N_TAMANHO_MUNDO;
    M.relogio = T_INICIO;
    M.ms_cumpridas = 0;
    M.LEF = fprio_cria();

    for(i = 0; i < M.num_herois; i++){
        M.herois[i] = heroi_cria(&M, i);
    }

    for(i = 0; i < M.num_bases; i++){
        M.bases[i] = base_cria(&M, i);
    }

    for(i = 0; i < M.num_missoes; i++){
        M.missoes[i] = missao_cria(&M, i);

        /* Calcula distâncias entre a missão e todas as bases */
        for(int j = 0; j < M.num_bases; j++){
            M.missoes[i].dist[j].id = j;
            M.missoes[i].dist[j].dist = distancia_cartesiana(M.missoes[i].local, M.bases[j].local);
        }

        /* Ordena as distâncias para acesso eficiente */
        ordena_vetor(M.missoes[i].dist, 0, M.num_bases - 1);
    }

    return M;
}

void mundo_destroi (struct mundo *M){

    int i;

    for(i = 0; i < M->num_herois; i++){
        cjto_destroi(M->herois[i].hab);
    }

    for(i = 0; i < M->num_bases; i++){
        cjto_destroi(M->bases[i].pres);
        lista_destroi(M->bases[i].fila);
    }

    for(i = 0; i < M->num_missoes; i++){
        cjto_destroi(M->missoes[i].hab);
    }

    fprio_destroi(M->LEF);
}

/*-----------------------eventos----------------------------*/ 

void eventos_iniciais_cria (struct mundo *M){

    int i;
    struct evento *ev;

    for(i = 0; i < M->num_herois; i++){

        if(!(ev = malloc(sizeof(struct evento)))){
            return;
        }

        ev->T = aleat(0, 4320);
        ev->H = i;
        ev->B = aleat(0, M->num_bases - 1);
        ev->MS = -1;
        fprio_insere(M->LEF, ev, EV_CHEGA, ev->T);
    }

    for(i = 0; i < M->num_missoes; i++){

        if(!(ev = malloc(sizeof(struct evento)))){
            return;
        }

        ev->T = aleat(0, T_FIM_DO_MUNDO);
        ev->H = -1;
        ev->B = -1;
        ev->MS = i;
        fprio_insere(M->LEF, ev, EV_MISSAO, ev->T);
    }

    if(!(ev = malloc(sizeof(struct evento)))){
        return;
    }

    ev->T = T_FIM_DO_MUNDO;
    ev->H = -1;
    ev->B = -1;
    ev->MS = -1;
    fprio_insere(M->LEF, ev, EV_FIM, ev->T);
}

void chega(struct mundo *M, int T, int H, int B){

    int espera, pres, lot, tam_fila;
    struct evento *ev;

    if(M->herois[H].morto){
        return;
    }

    if(!(ev = malloc(sizeof(struct evento)))){
        return;
    }

    M->herois[H].base = B;
    pres = cjto_card(M->bases[B].pres);
    lot = M->bases[B].lot;
    tam_fila = lista_tamanho(M->bases[B].fila);

    if(pres < lot && tam_fila == 0){
        espera = 1;
    }
    else{
        espera = (M->herois[H].paci) > (10 * tam_fila);
    }

    ev->T = T;
    ev->H = H;
    ev->B = B;
    ev->MS = -1;

    if(espera){
        fprio_insere(M->LEF, ev, EV_ESPERA, T);

        printf("%6d: CHEGA  HEROI %2d BASE %d (%2d/%2d) ESPERA\n", T, H, B, pres, lot);
    }
    else{
        fprio_insere(M->LEF, ev, EV_DESISTE, T);

        printf("%6d: CHEGA  HEROI %2d BASE %d (%2d/%2d) DESISTE\n", T, H, B, pres, lot);
    }
}

void espera(struct mundo *M, int T, int H, int B){

    int tam_fila;
    struct evento *ev;

    if(M->herois[H].morto){
        return;
    }
    
    if(!(ev = malloc(sizeof(struct evento)))){
        return;
    }

    tam_fila = lista_tamanho(M->bases[B].fila);

    lista_insere(M->bases[B].fila, M->herois[H].id, -1);
    fila_maxi(M, B);

    ev->T = T;
    ev->H = -1;
    ev->B = B;
    ev->MS = -1;
    fprio_insere(M->LEF, ev, EV_AVISA, T);

    printf("%6d: ESPERA HEROI %2d BASE %d (%2d)\n", T, H, B, tam_fila);
}

void desiste(struct mundo *M, int T, int H, int B){

    int D;
    struct evento *ev;

    if(M->herois[H].morto){
        return;
    }

    if(!(ev = malloc(sizeof(struct evento)))){
        return;
    }

    D = aleat(0, M->num_bases - 1);

    ev->T = T;
    ev->H = H;
    ev->B = D;
    ev->MS = -1;
    fprio_insere(M->LEF, ev, EV_VIAJA, T);

    printf("%6d: DESIST HEROI %2d BASE %d\n", T, H, B);
}

void avisa(struct mundo *M, int T, int B){

    int pres, lot, H;
    struct evento *ev;

    pres = cjto_card(M->bases[B].pres);
    lot = M->bases[B].lot;

    printf("%6d: AVISA  PORTEIRO BASE %d (%2d/%2d) FILA [ ", T, B, pres, lot);
    lista_imprime(M->bases[B].fila);
    printf(" ]\n");

    while((cjto_card(M->bases[B].pres) < lot) && (lista_tamanho(M->bases[B].fila) > 0)){
        lista_retira(M->bases[B].fila, &H, 0);
        cjto_insere(M->bases[B].pres, H);

        if(!(ev = malloc(sizeof(struct evento)))){
            return;
        }

        ev->T = T;
        ev->H = H;
        ev->B = B;
        ev->MS = -1;
        fprio_insere(M->LEF, ev, EV_ENTRA, T);

        printf("%6d: AVISA  PORTEIRO BASE %d ADMITE %2d\n", T, B, H);
    }
}

void entra(struct mundo *M, int T, int H, int B){

    int pres, lot, TPB;
    struct evento *ev;

    if(M->herois[H].morto){
        return;
    }

    if(!(ev = malloc(sizeof(struct evento)))){
        return;
    }

    pres = cjto_card(M->bases[B].pres);
    lot = M->bases[B].lot;
    TPB = 15 + M->herois[H].paci * aleat(1, 20);

    ev->T = T + TPB;
    ev->H = H;
    ev->B = B;
    ev->MS = -1;
    fprio_insere(M->LEF, ev, EV_SAI, ev->T);

    printf("%6d: ENTRA  HEROI %2d BASE %d (%2d/%2d) SAI %d\n", T, H, B, pres, lot, ev->T);
}

void sai(struct mundo *M, int T, int H, int B){

    int pres, lot, D;
    struct evento *ev;

    if(M->herois[H].morto){
        return;
    }

    cjto_retira(M->bases[B].pres, H);

    pres = cjto_card(M->bases[B].pres);
    lot = M->bases[B].lot;
    D = aleat(0, M->num_bases - 1);

    if(!(ev = malloc(sizeof(struct evento)))){
        return;
    }

    ev->T = T;
    ev->H = H;
    ev->B = D;
    ev->MS = -1;
    fprio_insere(M->LEF, ev, EV_VIAJA, T);

    if(!(ev = malloc(sizeof(struct evento)))){
        return;
    }

    ev->T = T;
    ev->H = -1;
    ev->B = B;
    ev->MS = -1;
    fprio_insere(M->LEF, ev, EV_AVISA, T);

    printf("%6d: SAI    HEROI %2d BASE %d (%2d/%2d)\n", T, H, B, pres, lot);
}

void viaja(struct mundo *M, int T, int H, int B){

    int B_atual, vel, duracao;
    float dist;
    struct evento *ev;

    if(M->herois[H].morto){
        return;
    }

    if(!(ev = malloc(sizeof(struct evento)))){
        return;
    }

    B_atual = M->herois[H].base;
    dist = distancia_cartesiana(M->bases[B_atual].local, M->bases[B].local);
    vel = M->herois[H].vel;
    duracao = dist / vel;
    M->herois[H].base = -1;

    ev->T = T + duracao;
    ev->H = H;
    ev->B = B;
    ev->MS = -1;
    fprio_insere(M->LEF, ev, EV_CHEGA, ev->T);

    printf("%6d: VIAJA  HEROI %2d BASE %d BASE %d DIST %f VEL %d CHEGA %d\n", T, H, B_atual, B, dist, vel, ev->T);
}

void morre(struct mundo *M, int T, int H, int B, int MS){

    struct evento *ev;

    if(!(ev = malloc(sizeof(struct evento)))){
        return;
    }

    M->herois[H].morto = 1;
    M->herois[H].base = -1;
    cjto_retira(M->bases[B].pres, H);

    ev->T = T;
    ev->H = -1;
    ev->B = B;
    ev->MS = -1;
    fprio_insere(M->LEF, ev, EV_AVISA, T);

    printf("%6d: MORRE  HEROI %2d MISSAO %d\n", T, H, MS);
}

int base_mais_proxima(struct mundo *M, int T, int MS){

    int id_BMP;
    float dist_BMP;
    struct cjto_t *uniao, *aux;

    for(int i = 0; i < M->num_bases; i++){

        uniao = cjto_cria(M->num_hab);

        id_BMP = M->missoes[MS].dist[i].id;
        dist_BMP = M->missoes[MS].dist[i].dist;

        printf("%6d: MISSAO %d BASE %d DIST %f HEROIS [ ", T, MS, id_BMP, dist_BMP);
        cjto_imprime(M->bases[id_BMP].pres);
        printf(" ]\n");

        for(int j = 0; j < M->num_herois; j++){
            if(cjto_pertence(M->bases[id_BMP].pres, j)){

                aux = cjto_uniao(uniao, M->herois[j].hab);
                cjto_destroi(uniao);
                uniao = cjto_copia(aux);
                cjto_destroi(aux);

                printf("%6d: MISSAO %d HAB HEROI %2d: [ ", T, MS, j);
                cjto_imprime(M->herois[j].hab);
                printf(" ]\n");
            }
        }

        printf("%6d: MISSAO %d UNIAO HAB BASE %d: [ ", T, MS, id_BMP);
        cjto_imprime(uniao);
        printf(" ]\n");

        if(cjto_contem(uniao, M->missoes[MS].hab)){

            printf("%6d: MISSAO %d CUMPRIDA BASE %d HABS: [ ", T, MS, id_BMP);
            cjto_imprime(uniao);
            printf(" ]\n");

            cjto_destroi(uniao);
            return id_BMP;
        }

        cjto_destroi(uniao);
    }

    return -1;
}

void missao(struct mundo *M, int T, int MS){

    int id_BMP, risco;
    struct evento *ev;

    printf("%6d: MISSAO %d TENT %d HAB REQ: [ ", T, MS, M->missoes[MS].tentativa);
    cjto_imprime(M->missoes[MS].hab);
    printf(" ]\n");

    id_BMP = base_mais_proxima(M, T, MS);

    if(id_BMP == -1){

        (M->missoes[MS].tentativa)++;

        if(!(ev = malloc(sizeof(struct evento)))){
            return;
        }

        ev->T = T + 24*60;
        ev->H = -1;
        ev->B = -1;
        ev->MS = MS;
        fprio_insere(M->LEF, ev, EV_MISSAO, ev->T);

        printf("%6d: MISSAO %d IMPOSSIVEL\n", T, MS);
        return;
    }

    (M->ms_cumpridas)++;
    (M->bases[id_BMP].num_missoes)++;

    for(int i = 0; i < M->num_herois; i++){
        if(cjto_pertence(M->bases[id_BMP].pres, i)){

            risco = M->missoes[MS].perigo / (M->herois[i].paci + M->herois[i].exp + 1);

            if(risco > aleat(0, 30)){

                if(!(ev = malloc(sizeof(struct evento)))){
                    return;
                }

                ev->T = T;
                ev->H = i;
                ev->B = id_BMP;
                ev->MS = MS;
                fprio_insere(M->LEF, ev, EV_MORRE, ev->T);
            }
            else{
                (M->herois[i].exp)++;
            }
        }
    }
}

void fim(struct mundo *M, int T, int num_ev){

    int i, h_mortos, tent, maior_tent, menor_tent;
    float prct, media;

    h_mortos = 0;

    printf("%6d: FIM\n", T);

    for(i = 0; i < M->num_herois; i++){
        if(M->herois[i].morto){
            printf("HEROI %2d MORTO  PAC %3d VEL %4d EXP %4d HABS [ ", i, M->herois[i].paci, M->herois[i].vel, M->herois[i].exp);
            h_mortos++;
        }
        else{
            printf("HEROI %2d VIVO  PAC %3d VEL %4d EXP %4d HABS [ ", i, M->herois[i].paci, M->herois[i].vel, M->herois[i].exp);
        }

        cjto_imprime(M->herois[i].hab);
        printf(" ]\n");
    }

    for(i = 0; i < M->num_bases; i++){
        printf("BASE %2d LOT %2d FILA MAX %2d MISSOES %d\n", i, M->bases[i].lot, M->bases[i].fila_max, M->bases[i].num_missoes);
    }

    printf("EVENTOS TRATADOS: %d\n", num_ev);

    maior_tent = 1;
    menor_tent = 1;

    for(i = 0; i < M->num_missoes; i++){
        tent = M->missoes[i].tentativa;

        if(tent > maior_tent){
            maior_tent = tent;
            continue;
        }

        if(tent < menor_tent){
            menor_tent = tent;
        }
    }

    prct = M->ms_cumpridas * 100.0 / M->num_missoes;
    media = (menor_tent + maior_tent) / 2.0;

    printf("MISSOES CUMPRIDAS: %d/%d (%.1f%%)\n", M->ms_cumpridas, M->num_missoes, prct);
    printf("TENTATIVAS/MISSAO: MIN %d, MAX %d, MEDIA %.1f\n", menor_tent, maior_tent, media);

    prct = h_mortos * 100.0 / M->num_herois;

    printf("TAXA MORTALIDADE: %.1f%%\n", prct);
}

int main (){

    int num_ev, FIM, tipo, prioridade;
    struct evento *ev;
    struct mundo M;

    srand(0);

    M = mundo_cria();
    eventos_iniciais_cria(&M);
    M.relogio = 0;
    num_ev = 0;
    FIM = 0;

    while(!FIM){
        ev = fprio_retira(M.LEF, &tipo, &prioridade);
        num_ev++;

        switch(tipo){
            case EV_CHEGA:
                M.relogio = prioridade;
                chega(&M, M.relogio, ev->H, ev->B);
                break;
            case EV_ESPERA:
                M.relogio = prioridade;
                espera(&M, M.relogio, ev->H, ev->B);
                break;
            case EV_DESISTE:
                M.relogio = prioridade;
                desiste(&M, M.relogio, ev->H, ev->B);
                break;
            case EV_AVISA:
                M.relogio = prioridade;
                avisa(&M, M.relogio, ev->B);
                break;
            case EV_ENTRA:  
                M.relogio = prioridade;
                entra(&M, M.relogio, ev->H, ev->B);
                break;
            case EV_SAI:
                M.relogio = prioridade;
                sai(&M, M.relogio, ev->H, ev->B);
                break;
            case EV_VIAJA:
                M.relogio = prioridade;
                viaja(&M, M.relogio, ev->H, ev->B);
                break;  
            case EV_MORRE:
                M.relogio = prioridade;
                morre(&M, M.relogio, ev->H, ev->B, ev->MS);
                break;
            case EV_MISSAO: 
                M.relogio = prioridade;
                missao(&M, M.relogio, ev->MS);
                break;
            case EV_FIM:
                M.relogio = prioridade;
                fim(&M, M.relogio, num_ev);
                FIM = 1;
                break;
        }

        free(ev);
        ev = NULL;
    }

    mundo_destroi(&M);

    return (0);
}
