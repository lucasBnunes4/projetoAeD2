#ifndef PROJETO_AED2_H
#define PROJETO_AED2_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

#define MAX_LINHA 4096
#define MAX_COLUNA 100
#define MAX_NOME_ARQUIVO 300
#define MAX_CHAVE 50
#define TAMANHO_ORDEM_ABM 4

typedef struct LinhaDados {
    char **colunas;
    int num_colunas;
} LinhaDados;

typedef struct Dataset {
    LinhaDados *linhas;
    int num_linhas;
    char **nomes_colunas;
    int total_colunas;
} Dataset;

typedef struct Matriz {
    int linha_para_matriz;
    int chave;
} Matriz;

typedef struct NoABB {
    int chave;
    int linha_para_matriz;
    struct NoABB *esq;
    struct NoABB *dir;
} NoABB;

typedef struct NoAVL {
    int chave;
    int linha_para_matriz;
    int altura;
    struct NoAVL *esq;
    struct NoAVL *dir;
} NoAVL;


/*========================================================*/
/*Funções de manipulação de strings e dataset*/
/*========================================================*/
void remover_acentos(char *str);
void limpar_string (char *str);
int contar_colunas(const char *linha, char separador);
char** dividir_linhas(const char *linha, char separador, int *num_colunas);
Dataset* carregar_dataset(const char *nome_arquivo);
void analisar_dataset(Dataset *dataset);
void salvar_dataset_limpo(Dataset *dataset, const char *nome_arquivo);
void liberar_memoria_dataset(Dataset *dataset);


/*========================================================*/
/*Matriz a partir daqui*/
/*========================================================*/
int encontrar_cnuc (Dataset *dataset);
Matriz* criar_matriz(Dataset *dataset, int *tamanho_matriz);
void printar_matriz(Matriz *matriz, int tamanho);
void liberar_mem_matriz(Matriz *matriz);

/*========================================================*/
/*Arvore binaria de busca a partir daqui*/
/*========================================================*/
NoABB* criar_no_abb(int chave, int linha_para_matriz);
NoABB* inserir_abb(NoABB *raiz, int chave, int linha_para_matriz);
NoABB* construir_abb(Matriz *matriz, int tamanho);
void percorrer_abb(NoABB *raiz);
int buscar_abb(NoABB *raiz, int chave);
void buscar_cnuc(NoABB *raiz, Dataset *dataset);
void liberar_abb(NoABB *raiz);


/*============================================================*/
/*Testes ABB*/
void teste_busca_individual(NoABB *raiz, Dataset *dataset, int chave);
void teste_estrutura_arvore(NoABB *raiz, Dataset *dataset);
void menu_testes_abb(NoABB *raiz, Dataset *dataset, Matriz *matriz, int tamanho_matriz);


/*========================================================*/
/*Arvore binaria de busca(AVL) a partir daqui*/
/*========================================================*/
int altura(NoAVL *no);
int max(int a, int b);
void atualizar_altura(NoAVL *no);
NoAVL* rotacao_direita(NoAVL *y);
NoAVL* rotacao_esquerda(NoAVL *x);
NoAVL* criar_no_avl(int chave, int linha_para_matriz);
NoAVL* inserir_avl(NoAVL *no, int chave, int linha_para_matriz);
NoAVL* construir_avl(Matriz *matriz, int tamanho);
int buscar_avl(NoAVL *raiz, int chave);
void buscar_cnuc_avl(NoAVL *raiz, Dataset *dataset);
void liberar_avl(NoAVL *raiz);
void teste_busca_individual_avl(NoAVL *raiz, Dataset *dataset, int chave);
void teste_estrutura_avl(NoAVL *raiz, Dataset *dataset);
void menu_testes_avl(NoAVL *raiz, Dataset *dataset, Matriz *matriz, int tamanho_matriz);


/*========================================================*/
/*Gerar o relatorio ordenado*/
/*========================================================*/
void escrever_linha_csv(FILE *arquivo, Dataset *dataset, int numero_linha);
void percorrer_avl_salvar(NoAVL *no, FILE *arquivo, Dataset *dataset);
void gerar_relatorio_avl(NoAVL *raiz, Dataset *dataset, const char *nome_arquivo);


/*========================================================*/
/*Exibir linha completa do dataset*/
/*========================================================*/
void printar_linha_completa(Dataset *dataset, int numero_linha);

#endif