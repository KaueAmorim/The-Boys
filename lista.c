#include <stdio.h>
#include <stdlib.h>
#include "lista.h"

struct lista_t *lista_cria (){
    
    struct lista_t *lst;
    if(!(lst = malloc(sizeof(struct lista_t)))){
        return NULL;
    }

    lst->prim = NULL;
    lst->ult = NULL;
    lst->tamanho = 0;
    
    return lst;
}

struct lista_t *lista_destroi (struct lista_t *lst){
    
    struct item_t *atual;
    struct item_t *prox;
    
    if(lst == NULL){
        return NULL;
    }
    
    atual = lst->prim;
    while(atual != NULL){
        prox = atual->prox;
        free(atual);
        atual = prox;
    }
     
    free(lst);

    return NULL;
}

int lista_insere (struct lista_t *lst, int item, int pos){

    struct item_t *aux;
    struct item_t *novo;

    if(pos < -1 || lst == NULL){
        return -1;
    }
    
    if(!(novo = malloc(sizeof(struct item_t)))){
        return -1;
    }

    novo->valor = item;
    
    /* Caso de lista vazia */
    if(lst->tamanho == 0){
        lst->prim = novo;
        lst->ult = novo;
        novo->ant = NULL;
        novo->prox = NULL;

        lst->tamanho++;
        return lst->tamanho;
    }
    
    /* Inserção no fim da lista */
    if(pos >= lst->tamanho || pos == -1){
        novo->ant = lst->ult;
        novo->prox = NULL;
        lst->ult->prox = novo;
        lst->ult = novo;

        lst->tamanho++;
        return lst->tamanho;
    }
    
    lst->tamanho++;

    /* Inserção no início da lista */
    if(pos == 0){
        novo->ant = NULL;
        novo->prox = lst->prim;
        lst->prim->ant = novo;
        lst->prim = novo;

        return lst->tamanho;
    }
    
    /* Inserção no meio da lista */
    aux = lst->prim;
    for(int i = 0; i < pos - 1; i++){
        aux = aux->prox;
    }

    novo->prox = aux->prox;
    novo->ant = aux;
    aux->prox->ant = novo;
    aux->prox = novo;
    
    return lst->tamanho;
}

int lista_retira (struct lista_t *lst, int *item, int pos){

    struct item_t *aux;

    if(lst == NULL || item == NULL || pos < -1 || pos >= lst->tamanho){
        return -1;
    }
    
    if(pos == -1){
        pos = lst->tamanho - 1;
    }

    aux = lst->prim;
    for(int i = 0; i < pos; i++){
        aux = aux->prox;
    }
    
    *item = aux->valor;

    /* Caso de lista com um único item */
    if(lst->tamanho == 1){
        lst->prim = NULL;
        lst->ult = NULL;
        free(aux);        

        lst->tamanho--;
        return lst->tamanho;
    }

    /* Retira do fim da lista */
    if(pos == lst->tamanho - 1){
        lst->ult = aux->ant;
        lst->ult->prox = NULL;
        free(aux);

        lst->tamanho--;
        return lst->tamanho;
    }

    lst->tamanho--;
    
    /* Retira do início da lista */
    if(pos == 0){
        lst->prim = aux->prox;
        lst->prim->ant = NULL;
        free(aux);

        return lst->tamanho;
    }
    
    /* Retira do meio da lista */
    aux->ant->prox = aux->prox;
    aux->prox->ant = aux->ant;
    free(aux);

    return lst->tamanho;
}

int lista_consulta (struct lista_t *lst, int *item, int pos){
    
    struct item_t *aux;
    
    if(lst == NULL || item == NULL || pos < -1 || pos >= lst->tamanho){
        return -1;
    }

    if(pos == -1){
        pos = lst->tamanho - 1;
    }

    aux = lst->prim;
    for(int i = 0; i < pos; i++) {
        aux = aux->prox;
    }

    *item = aux->valor;

    return lst->tamanho;
}

int lista_procura (struct lista_t *lst, int valor){
    
    struct item_t *aux;
    int pos = 0;

    if(lst == NULL){
        return -1;
    }

    aux = lst->prim;
    
    while(aux != NULL){
        if(aux->valor == valor){
            return pos;
        }
        aux = aux->prox;
        pos++;
    }

    return -1;
}

int lista_tamanho (struct lista_t *lst){

    if(lst == NULL){
        return -1;
    }

    return lst->tamanho;
}

void lista_imprime (struct lista_t *lst){
    
    struct item_t *aux;

    if(lst == NULL || lst->prim == NULL){
        return;
    }

    aux = lst->prim;

    while(aux->prox != NULL){
        printf("%d ", aux->valor);
        aux = aux->prox;
    }

    printf("%d", aux->valor);
}
