#include "projeto_aed2.h"

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
        printf("3. Mostrar matriz (primeiras 10 entradas)\n");
        printf("0. Sair\n");
        printf("Escolha uma opção: ");
        
        if (scanf("%d", &opcao) != 1) {
            printf("Entrada inválida!\n");
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n'); /*Limpar buffer*/
        
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
                buscar_cnuc(raiz, dataset);
                break;
            
            case 3:
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