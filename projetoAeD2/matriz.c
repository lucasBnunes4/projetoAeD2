#include "projeto_aed2.h"

int encontrar_cnuc (Dataset *dataset) {
    int i;
    for (i = 0; i < dataset->total_colunas; i++) {
        if (dataset->nomes_colunas[i] != NULL && strcmp(dataset->nomes_colunas[i], "codigo_cnuc") == 0) {
            return i;
        }
    }
    return -1;
}

Matriz* criar_matriz(Dataset *dataset, int *tamanho_matriz) {
    int indice_cnuc = encontrar_cnuc(dataset);

    if (indice_cnuc == -1) {
        printf("Codigo cnuc nao encontrado\n");
        *tamanho_matriz = 0;
        return NULL;
    }

    printf("Codigo cnuc encontrado: %d\n", indice_cnuc);

    Matriz *matriz = calloc(dataset->num_linhas, sizeof(Matriz));
    if (!matriz) {
        printf("Falha ao alocar memoria para matriz\n");
        *tamanho_matriz = 0;
        return NULL;
    }

    int i;
    int linhas_validas = 0;

    for (i = 0; i < dataset->num_linhas; i++) {
        if (indice_cnuc < dataset->linhas[i].num_colunas && dataset->linhas[i].colunas[indice_cnuc] !=NULL && strlen(dataset->linhas[i].colunas[indice_cnuc]) > 0) {
            matriz[linhas_validas].linha_para_matriz = i + 1;
            matriz[linhas_validas].chave = atoi(dataset->linhas[i].colunas[indice_cnuc]);

            linhas_validas++;
        }
    }
    
    *tamanho_matriz = linhas_validas;
    printf("Matriz criada com %d entradas\n", linhas_validas);

    return matriz;
}

void printar_matriz(Matriz *matriz, int tamanho) {
    int i;

    printf("\nMatriz Linha da tabela x Chave\n");
    printf("%-12s | %s\n", "LINHA_TABELA", "CHAVE");
    printf("--------------+-----------------\n");
    
    for (i = 0; i < tamanho; i++) {
        printf("%-12d | %d\n", matriz[i].linha_para_matriz, matriz[i].chave);
    }
    
    printf("Total de registros na matriz: %d\n", tamanho);
}

void liberar_mem_matriz(Matriz *matriz) {
    if (matriz) {
        free(matriz);
    }
}

void printar_linha_completa(Dataset *dataset, int numero_linha) {
     int indice_linha = numero_linha - 1; /*Converter para índice 0-based*/
    
    if (indice_linha < 0 || indice_linha >= dataset->num_linhas) {
        printf("Linha inválida: %d\n", numero_linha);
        return;
    }

    printf("Dados completos (linha %d): \n", numero_linha);
    printf("-------------------------------------------------\n");
    
    for (int col = 0; col < dataset->total_colunas; col++) {
        printf("%-20s: ", dataset->nomes_colunas[col]);
        if (col < dataset->linhas[indice_linha].num_colunas && 
            dataset->linhas[indice_linha].colunas[col] != NULL) {
            printf("%s", dataset->linhas[indice_linha].colunas[col]);
        } else {
            printf("(vazio)");
        }
        printf("\n");
    }
    printf("-------------------------------------------------\n");
}