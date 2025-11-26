#include "projeto_aed2.h"

void remover_acentos(char *str) {
    char *fonte = str;
    char *destino = str; /*src (source / fonte) e dst (destino)*/

    while (*fonte) {
        if ((unsigned char)*fonte == 0xC3) {
            switch ((unsigned char)*(fonte+1)) {
            case 0xA1: case 0xA0: case 0xA2: case 0xA3: case 0xA4: *destino = 'a'; break;
            case 0xA9: case 0xA8: case 0xAA: case 0xAB: *destino = 'e'; break; // é è ê ë
            case 0xAD: case 0xAC: case 0xAE: case 0xAF: *destino = 'i'; break; // í ì î ï
            case 0xB3: case 0xB2: case 0xB4: case 0xB5: case 0xB6: *destino = 'o'; break; // ó ò ô õ ö
            case 0xBA: case 0xB9: case 0xBB: case 0xBC: *destino = 'u'; break; // ú ù û ü
            case 0xA7: *destino = 'c'; break; // ç
            case 0x81: *destino = 'A'; break; // Á
            case 0x89: *destino = 'E'; break; // É
            case 0x8D: *destino = 'I'; break; // Í
            case 0x93: *destino = 'O'; break; // Ó
            case 0x9A: *destino = 'U'; break; // Ú
            case 0x87: *destino = 'C'; break; // Ç
            default: *destino = *fonte; fonte++; destino++; continue;
                break;
            }
            fonte += 2;
        } else {
            *destino = *fonte;
            fonte++;
        }
        destino++;
    }
    *destino = '\0';
}

void limpar_string (char *str) {
    if (str == NULL) return;

    remover_acentos(str);

    char *fonte = str;
    char *destino = str;
    int espaco_ant = 0;

    while (*fonte) {
        if (isalnum((unsigned char)*fonte) || *fonte == ' ' || *fonte == '-' || *fonte == '_' || *fonte == '/') {
            if (*fonte == ' ') {
                if (!espaco_ant) {
                    *destino = *fonte;
                    destino++;
                }
                espaco_ant = 1;
            } else {
                *destino = tolower((unsigned char)*fonte);
                destino++;
                espaco_ant = 0;
            }
        }
        fonte++;
    }
    *destino = '\0';

    char *inicio = str;
    char *fim = str + strlen(str) - 1;

    while (isspace((unsigned char)*inicio)) inicio++;
    while (fim > inicio && isspace((unsigned char)*fim)) fim--;
    
    memmove(str, inicio, fim - inicio + 1);
    str[fim - inicio + 1] = '\0';
}

int contar_colunas(const char *linha, char separador) {
    int count = 0;
    int na_coluna = 0;
    int i;

    for (i = 0; linha[i] != '\0'; i++) {
        if (linha[i] == separador) {
            count++;
            na_coluna = 0;
        } else if (!na_coluna && linha[i] != ' ' && linha[i] != '\t') {
            na_coluna = 1;
        }
    }

    return count + 1;
    
}

char** dividir_linhas(const char *linha, char separador, int *num_colunas) {
    *num_colunas = contar_colunas(linha, separador);
    char **colunas = calloc(*num_colunas, sizeof(char*));

    if (!colunas) return NULL;

    char buffer[MAX_LINHA];
    strcpy(buffer, linha);

    char *token = strtok(buffer, &separador);
    int i = 0;

    while (token != NULL && i < *num_colunas) {
        char *inicio = token;
        char *fim = token + strlen(token) - 1;

        while (isspace((unsigned char)*inicio)) inicio++;
        while (fim > inicio && isspace((unsigned char)*fim)) fim--;

        colunas[i] = calloc((fim - inicio +2), sizeof(char));
        if (colunas[i]) {
            strncpy(colunas[i], inicio, fim - inicio + 1);
            colunas[i][fim - inicio + 1] = '\0';
        }

        token = strtok(NULL, &separador);
        i++;
    }

    return colunas;
}

Dataset* carregar_dataset(const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "r");

    if (!arquivo) {
        printf("Erro ao abrir o arquivo %s\n", nome_arquivo);
        return NULL;
    }

    Dataset *dataset = malloc(sizeof(Dataset));
    if (!dataset) {
        fclose(arquivo);
        return NULL;
    }

    dataset->linhas = NULL;
    dataset->num_linhas = 0;
    dataset->nomes_colunas = NULL;
    dataset->total_colunas = 0;
    int i;
    
    char linha[MAX_LINHA];
    int nlinha = 0;

    if (fgets(linha, MAX_LINHA, arquivo))
    {
        linha[strcspn(linha, "\n")] = 0;

        dataset->nomes_colunas = dividir_linhas(linha, ';', &dataset->total_colunas);
        printf("Cabecalho carregado com %d colunas\n", dataset->total_colunas);

        for (i = 0; i < dataset->total_colunas; i++) {
            if (dataset->nomes_colunas[i]) {
                limpar_string(dataset->nomes_colunas[i]);
            }
            
        }
        
    }

    int total_linhas = 0;
    while (fgets(linha,MAX_LINHA,arquivo)) {
        total_linhas++;
    }

    rewind(arquivo);
    fgets(linha, MAX_LINHA, arquivo);

    dataset->linhas = calloc(total_linhas, sizeof(LinhaDados));
    dataset->num_linhas = total_linhas;

    nlinha = 0;
    while (fgets(linha, MAX_LINHA, arquivo) && nlinha < total_linhas) {
        linha[strcspn(linha, "\n")] = 0;

        dataset->linhas[nlinha].colunas = dividir_linhas(linha, ';', &dataset->linhas[nlinha].num_colunas);

        for (i = 0; i < dataset->linhas[nlinha].num_colunas; i++) {
            if (dataset->linhas[nlinha].colunas[i]) {
                limpar_string(dataset->linhas[nlinha].colunas[i]);
            }
        }
        
        nlinha++;
    }

    fclose(arquivo);
    printf("Dataset carregado com %d linhas de dados\n", dataset->num_linhas);
    return dataset;
}

void analisar_dataset(Dataset *dataset) {
    int i;

    printf("\nInformacoes do dataset\n");
    printf("Numero de linhas: %d\n", dataset->num_linhas);
    printf("Numero de colunas: %d\n", dataset->total_colunas);

    printf("\nNomes das colunas:\n");
    for (i = 0; i < dataset->total_colunas; i++) {
        printf("%2d. %s\n", i + 1, dataset->nomes_colunas[i]);
    }
}

void salvar_dataset_limpo(Dataset *dataset, const char *nome_arquivo) {
    int i;
    int lin;
    int col;
    
    FILE *arquivo = fopen(nome_arquivo, "w");
    if (!arquivo) {
        printf("Erro ao salar o dataset limpo");
        return;
    }

    for (i = 0; i < dataset->total_colunas; i++) {
        fprintf(arquivo, "%s", dataset->nomes_colunas[i]);
        if (i < dataset->total_colunas - 1) {
            fprintf(arquivo, ";");
        }
    }
    fprintf(arquivo, "\n");

    /*salvamento dos dados*/
    for (lin = 0; lin < dataset->num_linhas; lin++) {
        for (col = 0; col < dataset->total_colunas; col++) {
            if (col < dataset->linhas[lin].num_colunas && dataset->linhas[lin].colunas[col] != NULL) {
                fprintf(arquivo, "%s", dataset->linhas[lin].colunas[col]);
            }
            if (col <dataset->total_colunas - 1) {
                fprintf(arquivo, ";");
            }
        }
        fprintf(arquivo, "\n");
    }
    
    fclose(arquivo);
    printf("Dataset limpo e salvo %s\n", nome_arquivo);
}

void liberar_memoria_dataset(Dataset *dataset) {
    int i;
    int j;

    if(!dataset) return;

    if (dataset->nomes_colunas) {
        for (i = 0; i < dataset->total_colunas; i++) {
            free(dataset->nomes_colunas[i]);
        }
        free(dataset->nomes_colunas);
    }

    if (dataset->linhas) {
        for (i = 0; i < dataset->num_linhas; i++) {
            if (dataset->linhas[i].colunas) {
                for (j = 0; j < dataset->linhas[i].num_colunas; j++) {
                    free(dataset->linhas[i].colunas[j]);
                }
                free(dataset->linhas[i].colunas);
            }
        }
        free(dataset->linhas);
    }
    
    free(dataset);
}