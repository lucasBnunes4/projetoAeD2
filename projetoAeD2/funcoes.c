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


/*========================================================*/
/*Matriz a partir daqui*/
/*========================================================*/
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

/*========================================================*/
/*Arvore binaria de busca a partir daqui*/
/*========================================================*/
NoABB* criar_no_abb(int chave, int linha_para_matriz) {
    NoABB *novo_no = (NoABB*)malloc(sizeof(NoABB));
    if (novo_no) {
        novo_no->chave = chave;
        novo_no->linha_para_matriz = linha_para_matriz;
        novo_no->esq = NULL;
        novo_no->dir = NULL;
    }
    
    return novo_no;
}


NoABB* inserir_abb(NoABB *raiz, int chave, int linha_para_matriz) {
    if (raiz == NULL) {
        return criar_no_abb(chave, linha_para_matriz);
    }

    int comparacao = chave - raiz->chave;

    if (comparacao < 0) {
        raiz->esq = inserir_abb(raiz->esq, chave, linha_para_matriz);
    } else if (comparacao > 0) {
        raiz->dir = inserir_abb(raiz->dir, chave, linha_para_matriz);
    }
    
    return raiz;
}

NoABB* construir_abb(Matriz *matriz, int tamanho) {
    NoABB *raiz = NULL;
    int i;

    for (i = 0; i < tamanho; i++)
    {
        raiz = inserir_abb(raiz, matriz[i].chave, matriz[i].linha_para_matriz);
    }

    printf("Arvore binaria de busca contruida com %d elementos", tamanho);
    return raiz;    
}

void percorrer_abb(NoABB *raiz) {
    if (raiz != NULL) {
        percorrer_abb(raiz->esq);
        printf("Chave: %d. Linha: %d\n", raiz->chave, raiz->linha_para_matriz);
        percorrer_abb(raiz->dir);
    } else {
        printf("\nErro ao percorrer arvore binaria de busca.\n");
    }
}

int buscar_abb(NoABB *raiz, int chave) {
    if (raiz == NULL) return -1;

    if (chave == raiz->chave) return raiz->linha_para_matriz;
    if (chave < raiz->chave)  return buscar_abb(raiz->esq, chave);
    else                      return buscar_abb(raiz->dir, chave);
}

void buscar_cnuc(NoABB *raiz, Dataset *dataset) {
     if (raiz == NULL) {
        printf("Arvore vazia!\n");
        return;
    }

    printf("\nBusca pelo cogigo CNUC\n");
    printf("Digite os numeros dos codigos CNUC que deseja buscar (separados por vurgula)\n");
    printf("Exemplo: 123, 456, 789\n");
    printf("Numeros CNUC: ");

    char entrada[1000];
    if (fgets(entrada, sizeof(entrada), stdin) == NULL) {
        printf("Erro na leitura da entrada.\n");
        return;
    }

    /*Remover quebra de linha*/
    entrada[strcspn(entrada, "\n")] = 0;

    if (strlen(entrada) == 0) {
        printf("Nenhum codigo informado!\n");
        return;
    }

     /*Processar os códigos*/
    char *token = strtok(entrada, ",");
    int encontrados = 0;
    int total_buscados = 0;

    printf("\nResultados Busca por Codigo CNUC\n");

    while (token != NULL) {
        char *inicio = token;
        char *fim = token + strlen(token) - 1;
        
        while (isspace((unsigned char)*inicio)) inicio++;
        while (fim > inicio && isspace((unsigned char)*fim)) fim--;
        
        char numero_limpo[MAX_CHAVE];
        strncpy(numero_limpo, inicio, fim - inicio + 1);
        numero_limpo[fim - inicio + 1] = '\0';

        if (strlen(numero_limpo) > 0) {
            total_buscados++;
            
            // Construir o código CNUC completo
            int codigo_busca = atoi(numero_limpo);
            printf("\nBuscando: %d\n", codigo_busca);
            
            int linha_encontrada = buscar_abb(raiz, codigo_busca);
            
            if (linha_encontrada != -1) {
                printf("Encontrado na Linha %d da tabela\n", linha_encontrada);
                
                // Mostrar todos os dados da linha
                int indice_linha = linha_encontrada - 1; // Converter para índice 0-based
                if (indice_linha >= 0 && indice_linha < dataset->num_linhas) {
                    printar_linha_completa(dataset, linha_encontrada);
                    encontrados++;
            } else {
                printf("NAO ENCONTRADO\n");
            }
        }
    }

        token = strtok(NULL, ",");
    }

    printf("\n=== RESUMO ===\n");
    printf("Codigos buscados: %d\n", total_buscados);
    printf("Codigos encontrados: %d\n", encontrados);
    printf("Codigos nao encontrados: %d\n", total_buscados - encontrados);
}


void liberar_abb(NoABB *raiz) {
    if (raiz != NULL) {
        liberar_abb(raiz->esq);
        liberar_abb(raiz->dir);
        free(raiz);
    }
}

/*============================================================*/
/*Testes ABB*/
void teste_busca_individual(NoABB *raiz, Dataset *dataset, int chave) {
    printf("\n🔍 --- BUSCA INDIVIDUAL ---\n");
    printf("Buscando código CNUC: %d\n", chave);
    
    int resultado = buscar_abb(raiz, chave);
    
    if (resultado != -1) {
        printf("ENCONTRADO na linha: %d\n", resultado);
        
        // USAR A FUNÇÃO printar_linha_completa EXISTENTE
        printar_linha_completa(dataset, resultado);
    } else {
        printf("NÃO ENCONTRADO\n");
        printf("O código CNUC %d não existe no banco de dados.\n", chave);
    }
}

void teste_estrutura_arvore(NoABB *raiz, Dataset *dataset) {
    printf("\nESTRUTURA DA ÁRVORE BINÁRIA DE BUSCA\n");
    
    printf("Percorrendo em ORDEM (primeiros 5 elementos):\n");
    int contador = 0;
    void percorrer_limitado(NoABB *raiz, int *cont, Dataset *ds) {
        if (raiz != NULL && *cont < 5) {
            percorrer_limitado(raiz->esq, cont, ds);
            if (*cont < 5) {
                printf("   Código: %d -> Linha: %d", raiz->chave, raiz->linha_para_matriz);
                
                // Mostrar nome da unidade de conservação
                int indice_linha = raiz->linha_para_matriz - 1;
                if (indice_linha >= 0 && indice_linha < ds->num_linhas && 
                    1 < ds->linhas[indice_linha].num_colunas && 
                    ds->linhas[indice_linha].colunas[1] != NULL) {
                    printf(" | %s", ds->linhas[indice_linha].colunas[1]);
                }
                printf("\n");
                (*cont)++;
            }
            percorrer_limitado(raiz->dir, cont, ds);
        }
    }
    percorrer_limitado(raiz, &contador, dataset);
    if (contador == 5) {
        printf("   ... (árvore contém mais elementos)\n");
    }
}

void menu_testes_abb(NoABB *raiz, Dataset *dataset, Matriz *matriz, int tamanho_matriz) {
    int opcao;
    
    do {
        printf("\nMENU DE TESTES - ÁRVORE BINÁRIA DE BUSCA\n");
        printf("1. Busca individual por código CNUC\n");
        printf("2. Busca múltipla por códigos CNUC (usando buscar_cnuc)\n");
        printf("3. Teste de performance\n");
        printf("4. Ver estrutura da árvore\n");
        printf("5. Mostrar matriz (primeiras 10 entradas)\n");
        printf("0. Sair\n");
        printf("Escolha uma opção: ");
        
        if (scanf("%d", &opcao) != 1) {
            printf("Entrada inválida!\n");
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n'); // Limpar buffer
        
        switch (opcao) {
            case 1:
                printf("Digite o código CNUC para busca individual: ");
                int chave;
                if (scanf("%d", &chave) == 1) {
                    teste_busca_individual(raiz, dataset, chave);
                } else {
                    printf("Código inválido!\n");
                }
                while (getchar() != '\n');
                break;
                
            case 2:
                // USAR A FUNÇÃO buscar_cnuc EXISTENTE
                buscar_cnuc(raiz, dataset);
                break;
                
            case 4:
                teste_estrutura_arvore(raiz, dataset);
                break;
                
            case 5:
                printar_matriz(matriz, tamanho_matriz);
                break;
                
            case 0:
                printf("Saindo do menu de testes...\n");
                break;
                
            default:
                printf("Opção inválida!\n");
        }
    } while (opcao != 0);
}



/*========================================================*/
/*Arvore binaria de busca(AVL) a partir daqui*/
/*========================================================*/
int altura(NoAVL *no) {
    if (no == NULL) {
        return 0;
    }
    return no->altura;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

void atualizar_altura(NoAVL *no) {
    if (no != NULL) {
        no->altura = 1 + max(altura(no->esq), altura(no->dir));
    }
}

NoAVL* rotacao_direita(NoAVL *y) {
    NoAVL *x = y->esq;
    NoAVL *T2 = x->dir;

    x->dir = y;
    y->esq = T2;

    atualizar_altura(y);
    atualizar_altura(x);

    return x;
}

NoAVL* rotacao_esquerda(NoAVL *x) {
    NoAVL *y = x->dir;
    NoAVL *T2 = y->esq;

    y->esq = x;
    x->dir = T2;

    atualizar_altura(x);
    atualizar_altura(y);

    return y;
}

NoAVL* criar_no_avl(int chave, int linha_para_matriz) {
    NoAVL *novo_no = (NoAVL*)malloc(sizeof(NoAVL));
    if (novo_no) {
        novo_no->chave = chave;
        novo_no->linha_para_matriz = linha_para_matriz;
        novo_no->altura = 1;
        novo_no->esq = NULL;
        novo_no->dir = NULL;
    }
    
    return novo_no;
}

NoAVL* inserir_avl(NoAVL *no, int chave, int linha_para_matriz) {
    if (no == NULL)
        return criar_no_avl(chave, linha_para_matriz);

    if (chave < no->chave)
        no->esq = inserir_avl(no->esq, chave, linha_para_matriz);
    else if (chave > no->chave)
        no->dir = inserir_avl(no->dir, chave, linha_para_matriz);
    else
        return no; // Chave duplicada

    atualizar_altura(no);

    int balanceamento = altura(no->esq) - altura(no->dir);

    // =====================
    // CASOS CLÁSSICOS AVL
    // =====================

    // LL (pesado à esquerda)
    if (balanceamento > 1 && chave < no->esq->chave)
        return rotacao_direita(no);

    // RR (pesado à direita)
    if (balanceamento < -1 && chave > no->dir->chave)
        return rotacao_esquerda(no);

    // LR
    if (balanceamento > 1 && chave > no->esq->chave) {
        no->esq = rotacao_esquerda(no->esq);
        return rotacao_direita(no);
    }

    // RL
    if (balanceamento < -1 && chave < no->dir->chave) {
        no->dir = rotacao_direita(no->dir);
        return rotacao_esquerda(no);
    }

    return no;
}

NoAVL* construir_avl(Matriz *matriz, int tamanho) {
    NoAVL *raiz = NULL;
    int i;

    for (i = 0; i < tamanho; i++)
    {
        raiz = inserir_avl(raiz, matriz[i].chave, matriz[i].linha_para_matriz);
    }

    printf("Arvore AVL contruida com %d elementos", tamanho);
    return raiz;    
}

int buscar_avl(NoAVL *raiz, int chave) {
    if (raiz == NULL)
        return -1;

    if (chave == raiz->chave)
        return raiz->linha_para_matriz;

    if (chave < raiz->chave)
        return buscar_avl(raiz->esq, chave);
    else
        return buscar_avl(raiz->dir, chave);
}

void buscar_cnuc_avl(NoAVL *raiz, Dataset *dataset) {
        if (raiz == NULL) {
            printf("Arvore vazia!\n");
            return;
        }
    
        printf("\nBusca pelo cogigo CNUC\n");
        printf("Digite os numeros dos codigos CNUC que deseja buscar (separados por vurgula)\n");
        printf("Exemplo: 123, 456, 789\n");
        printf("Numeros CNUC: ");
    
        char entrada[1000];
        if (fgets(entrada, sizeof(entrada), stdin) == NULL) {
            printf("Erro na leitura da entrada.\n");
            return;
        }
    
        /*Remover quebra de linha*/
        entrada[strcspn(entrada, "\n")] = 0;
    
        if (strlen(entrada) == 0) {
            printf("Nenhum codigo informado!\n");
            return;
        }
    
        /*Processar os códigos*/
        char *token = strtok(entrada, ",");
        int encontrados = 0;
        int total_buscados = 0;
    
        printf("\nResultados Busca por Codigo CNUC\n");
    
        while (token != NULL) {
            char *inicio = token;
            char *fim = token + strlen(token) - 1;
            
            while (isspace((unsigned char)*inicio)) inicio++;
            while (fim > inicio && isspace((unsigned char)*fim)) fim--;
            
            char numero_limpo[MAX_CHAVE];
            strncpy(numero_limpo, inicio, fim - inicio + 1);
            numero_limpo[fim - inicio + 1] = '\0';
    
            if (strlen(numero_limpo) > 0) {
                total_buscados++;
                
                // Construir o código CNUC completo
                char codigo_busca[MAX_CHAVE];
                snprintf(codigo_busca, sizeof(codigo_busca), "%s", numero_limpo);
                
                // Converter para minúsculo
                for (int i = 0; codigo_busca[i]; i++) {
                    codigo_busca[i] = tolower((unsigned char)codigo_busca[i]);
                }
    
                int codigo = atoi(numero_limpo);
                printf("Buscando: %d\n", codigo);
                int linha_encontrada = buscar_avl(raiz, codigo);


                if (linha_encontrada != -1) {
                    printf("Encontrado na Linha %d da tabela\n", linha_encontrada);

                    // Mostrar todos os dados da linha
                    int indice_linha = linha_encontrada - 1; // Converter para índice 0-based
                    if (indice_linha >= 0 && indice_linha < dataset->num_linhas) {
                        printar_linha_completa(dataset, linha_encontrada);
                    }
                    encontrados++;
                } else {
                    printf("NAO ENCONTRADO\n");
                }
            }
            token = strtok(NULL, ",");
        }

        printf("\n=== RESUMO ===\n");
        printf("Codigos buscados: %d\n", total_buscados);
        printf("Codigos encontrados: %d\n", encontrados);       
        printf("Codigos nao encontrados: %d\n", total_buscados - encontrados);
}
    
void liberar_avl(NoAVL *raiz) {
    if (raiz != NULL) {
        liberar_avl(raiz->esq);
        liberar_avl(raiz->dir);
        free(raiz);
    }
}

/*========================================================*/
/*Gerar o relatorio ordenado*/
/*========================================================*/
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

/*========================================================*/
/*Exibir linha completa do dataset*/
/*========================================================*/
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

int main() {
    setlocale(LC_ALL, "Portuguese");

    const char *arquivo_entrada = "G2_utf8.csv";
    const char *arquivo_saida = "dG2_limpo.csv";

    Dataset *dataset = carregar_dataset(arquivo_entrada);
    if (!dataset) {
        printf("Falha ao carregar dataset\n");
        return 1;
    }
    salvar_dataset_limpo(dataset, arquivo_saida);
    Dataset *dataset_limpo = carregar_dataset(arquivo_saida);
    if (!dataset_limpo) {
        printf("Falha ao carregar dataset limpo\n");
        liberar_memoria_dataset(dataset);
        return 1;
    }

    printf("\n=====Testes dataset======\n");
    analisar_dataset(dataset_limpo);
    int tamanho_matriz;
    Matriz *matriz = criar_matriz(dataset_limpo, &tamanho_matriz);
    printar_matriz(matriz, tamanho_matriz);

    printf("Construindo árvore binária de busca...\n");
    NoABB *arvore_abb = construir_abb(matriz, tamanho_matriz);
    if (!arvore_abb) {
        printf("Falha ao construir árvore\n");
        liberar_mem_matriz(matriz);
        liberar_memoria_dataset(dataset);
        return 1;
    }

    printf("\n✅ Sistema carregado com sucesso!\n");
    printf("   • %d registros no banco de dados\n", dataset->num_linhas);
    printf("   • %d códigos CNUC indexados\n", tamanho_matriz);

    // Executar menu de testes
    menu_testes_abb(arvore_abb, dataset, matriz, tamanho_matriz);

    /*Liberação de memória*/
    liberar_abb(arvore_abb);
    liberar_mem_matriz(matriz);
    liberar_memoria_dataset(dataset);
    liberar_memoria_dataset(dataset_limpo);

    printf("Programa finalizado com sucesso!\n");
    return 0;
}