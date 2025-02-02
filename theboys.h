#ifndef THEBOYS
#define THEBOYS

#include "fprio.h"
#include "conjunto.h"
#include "lista.h"

#define T_INICIO 0
#define T_FIM_DO_MUNDO 525600
#define N_TAMANHO_MUNDO 20000
#define N_HABILIDADES 10
#define N_HEROIS N_HABILIDADES * 5
#define N_BASES N_HEROIS / 5
#define N_MISSOES T_FIM_DO_MUNDO / 100
#define EV_CHEGA 0
#define EV_ESPERA 1
#define EV_DESISTE 2
#define EV_AVISA 3
#define EV_ENTRA 4
#define EV_SAI 5
#define EV_VIAJA 6
#define EV_MORRE 7
#define EV_MISSAO 8
#define EV_FIM 9

struct coordenada{
    int x;
    int y;
};

struct dist_bases{
    int id;       /*identificação da base*/
    float dist;   /*distância da missão*/
};

struct evento{
    int T;    /*tempo do evento*/
    int H;    /*herói*/
    int B;    /*base*/
    int MS;   /*missão*/
};

struct heroi{
    int id;               /*identificação*/
    int paci;             /*paciência*/
    int vel;              /*velocidade*/
    int exp;              /*experiência*/
    int base;             /*ID da base*/
    int morto;            /*flag de morte*/
    struct cjto_t *hab;   /*habilidades*/
};

struct base{
    int id;                   /*identificação*/
    int lot;                  /*lotação*/
    int fila_max;             /*tamanho máximo da fila*/
    int num_missoes;          /*número de missões que a base participou*/
    struct cjto_t *pres;      /*conjunto dos id dos heróis presentes na base*/
    struct lista_t *fila;     /*fila de espera para entrar na base*/
    struct coordenada local;  /*local da base*/
};

struct missao{
    int id;                           /*identificação*/
    int perigo;                       /*nível de perigo*/
    int tentativa;                    /*número de tentativas*/
    struct cjto_t *hab;               /*habilidades necessárias para missão*/
    struct dist_bases dist[N_BASES];  /*vetor de distâncias das bases*/
    struct coordenada local;          /*local da missão*/
};

struct mundo{
    int num_herois;                    /*número de heróis no mundo*/
    int num_bases;                     /*número de bases no mundo*/
    int num_missoes;                   /*número total de missões*/
    int num_hab;                       /*número total de habilidades*/
    int tam_mundo;                     /*tamanho do mundo*/
    int relogio;                       /*tempo do mundo*/
    int ms_cumpridas;                  /*missões cumpridas*/
    struct heroi herois[N_HEROIS];     /*vetor com todos os heróis*/
    struct base bases[N_BASES];        /*vetor com todos as bases*/
    struct missao missoes[N_MISSOES];  /*vetor com todas as missões*/
    struct fprio_t *LEF;               /*lista de eventos futuros*/
};

/* Retorna um número aleatório entre min e max, inclusive */
int aleat (int min, int max);

/* Calcula a distância cartesiana entre dois pontos em um plano bidimensional */
float distancia_cartesiana (struct coordenada B, struct coordenada D);

/* Atualiza o maior valor de tamanho da fila para uma base específica */
void fila_maxi (struct mundo *M, int B);

/* Rearranja um vetor baseando-se na comparação de cada elemento com o pivô 'x' */
int particiona (struct dist_bases vetor[], struct dist_bases x, int inicio, int fim);

/* Ordena um vetor de structs dist_bases usando o algoritmo Quick Sort */
void ordena_vetor (struct dist_bases vetor[], int inicio, int fim);

/* Inicializa um herói através de seu id */
struct heroi heroi_cria (struct mundo *M, int id);

/* Inicializa uma base através de seu id */
struct base base_cria (struct mundo *M, int id);

/* Inicializa uma missão através de seu id */
struct missao missao_cria (struct mundo *M, int id);

/* Cria e inicializa o mundo da simulação configurando suas estruturas principais. */
struct mundo mundo_cria ();

/* Libera a memória alocada para o mundo da simulação */
void mundo_destroi (struct mundo *M);

/* Eventos iniciais que farão a simulação avançar */
void eventos_iniciais_cria (struct mundo *M);

/* Representa um herói H chegando em uma base B no instante T. Ao chegar, o
 * herói analisa o tamanho da fila e decide se espera para entrar ou desiste. */
void chega (struct mundo *M, int T, int H, int B);

/* O herói H entra na fila de espera da base B. Assim que H entra na 
 * fila, o porteiro da base B é avisado para verificar a fila. */
void espera (struct mundo *M, int T, int H, int B);

/* O herói H desiste de entrar na base B, escolhendo 
 * uma base aleatória D e viajando para lá */
void desiste (struct mundo *M, int T, int H, int B);

/* O porteiro da base B trata a fila de espera */
void avisa (struct mundo *M, int T, int B);

/* O herói H entra na base B. Ao entrar, o herói decide 
 * quanto tempo vai ficar e agenda sua saída da base */
void entra (struct mundo *M, int T, int H, int B);

/* O herói H sai da base B. Ao sair, escolhe uma base de destino para 
 * viajar; o porteiro de B é avisado, pois uma vaga foi liberada */
void sai (struct mundo *M, int T, int H, int B);

/* O herói H se desloca para uma base D (que pode ser a mesma onde já está) */
void viaja (struct mundo *M, int T, int H, int B);

/* O herói H morre no instante T */
void morre (struct mundo *M, int T, int H, int B, int MS);

/* Retorna o índice da base mais próxima ao local da missão MS cujos heróis possam cumpri-lá */
int base_mais_proxima (struct mundo *M, int T, int MS);

/* Uma missão M é disparada no instante T. São características de uma missão:
   - Cada missão ocorre em um local aleatório e requer um conjunto aleatório 
     de habilidades, ambos são definidos durante a inicialização;
   - Cada equipe é formada pelo conjunto de heróis presentes em uma base;
   - Uma equipe está apta para a missão se a união das habilidades
     de seus heróis contém as habilidades requeridas pela missão; 
   - Deve ser escolhida para a missão a equipe da base mais 
     próxima ao local da missão e que esteja apta para ela;
   - Ao completar uma missão, os heróis da equipe escolhida 
     ganham pontos de experiência;
   - Um herói pode morrer ao participar de uma missão.
   - Se uma missão não puder ser completada, ela é marcada 
     como “impossível” e adiada de 24 horas. */
void missao (struct mundo *M, int T, int MS);

/* Encerra a simulação no instante T */
void fim (struct mundo *M, int T, int num_ev);

#endif
