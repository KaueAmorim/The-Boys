#include <stdio.h>
#include <stdlib.h>
#include "fprio.h"

struct fprio_t *fprio_cria (){

    struct fprio_t *f;
    if(!(f = malloc(sizeof(struct fprio_t)))){
        return NULL;
    }

    f->prim = NULL;
    f->num = 0;

    return f;
}

struct fprio_t *fprio_destroi (struct fprio_t *f){

    struct fpnodo_t *atual;
    struct fpnodo_t *prox;

    if(f == NULL){
        return NULL;
    }
 
    atual = f->prim;
    while(atual != NULL){
        prox = atual->prox;
        free(atual->item);
        free(atual);
        atual = prox;
    }

    free(f);

    return NULL;
}

int fprio_insere (struct fprio_t *f, void *item, int tipo, int prio){

    struct fpnodo_t *aux;
    struct fpnodo_t *novo;

    if(f == NULL || item == NULL){
        return -1;
    }
    
    /* Verifica se o item já está na fila */
    aux = f->prim;
    while(aux != NULL){
        if(aux->item == item){
            return -1;
        }
        aux = aux->prox;
    }

    if(!(novo = malloc(sizeof(struct fpnodo_t)))){
        return -1;
    }

    novo->item = item;
    novo->tipo = tipo;
    novo->prio = prio;
    (f->num)++;

    aux = f->prim;

    /* Insere no início da fila */
    if(aux == NULL || prio < aux->prio){
        novo->prox = aux;
        f->prim = novo;
        return f->num;
    }

    /* Insere no meio da fila */
    while(aux->prox != NULL){
        if(prio < aux->prox->prio){
            novo->prox = aux->prox;
            aux->prox = novo;
            return f->num;
        }
        aux = aux->prox;
    }

    /* Insere no final da fila */
    novo->prox = NULL;
    aux->prox = novo;
    return f->num;
}

void *fprio_retira (struct fprio_t *f, int *tipo, int *prio){

    struct fpnodo_t *aux;
    void *item_retirado;

    if(f == NULL || tipo == NULL || prio == NULL || f->num == 0){
        return NULL;
    }

    aux = f->prim;
    f->prim = aux ->prox;
    (f->num)--;

    *tipo = aux->tipo;
    *prio = aux->prio;
    item_retirado = aux->item;
 
    free(aux);

    return item_retirado;
}

int fprio_tamanho (struct fprio_t *f){

    if(f == NULL){
        return -1;
    }

    return f->num;
}

void fprio_imprime (struct fprio_t *f){

    struct fpnodo_t *aux;

    if(f == NULL || f->prim == NULL){
        return;
    }

    aux = f->prim;

    while(aux->prox != NULL){
        printf("(%d %d) ", aux->tipo, aux->prio);
        aux = aux->prox;
    }

    printf("(%d %d)", aux->tipo, aux->prio);
}
