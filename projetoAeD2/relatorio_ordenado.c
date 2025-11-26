#include "projeto_aed2.h"

void escrever_linha_csv(FILE *arquivo, Dataset *dataset, int numero_linha) {
    int idx = numero_linha - 1; 

    for (int col = 0; col < dataset->total_colunas; col++) {
        if (col < dataset->linhas[idx].num_colunas &&
            dataset->linhas[idx].colunas[col] != NULL) 
        {
            fprintf(arquivo, "%s", dataset->linhas[idx].colunas[col]);
        } else {
            fprintf(arquivo, "");
        }

        if (col < dataset->total_colunas - 1)
            fprintf(arquivo, ";");
    }

    fprintf(arquivo, "\n");
}

void percorrer_avl_salvar(NoAVL *no, FILE *arquivo, Dataset *dataset) {
    if (no == NULL) return;

    percorrer_avl_salvar(no->esq, arquivo, dataset);

    escrever_linha_csv(arquivo, dataset, no->linha_para_matriz);

    percorrer_avl_salvar(no->dir, arquivo, dataset);
}

void gerar_relatorio_avl(NoAVL *raiz, Dataset *dataset, const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "w");
    if (!arquivo) {
        printf("Erro ao criar %s\n", nome_arquivo);
        return;
    }

    for (int i = 0; i < dataset->total_colunas; i++) {
        fprintf(arquivo, "%s", dataset->nomes_colunas[i]);
        if (i < dataset->total_colunas - 1)
            fprintf(arquivo, ";");
    }
    fprintf(arquivo, "\n");

    percorrer_avl_salvar(raiz, arquivo, dataset);

    fclose(arquivo);

    printf("Relatorio ordenado gerado em: %s\n", nome_arquivo);
}