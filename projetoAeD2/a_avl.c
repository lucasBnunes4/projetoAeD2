#include "projeto_aed2.h"

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
        return no; 

    atualizar_altura(no);

    int balanceamento = altura(no->esq) - altura(no->dir);



    if (balanceamento > 1 && chave < no->esq->chave)
        return rotacao_direita(no);

    if (balanceamento < -1 && chave > no->dir->chave)
        return rotacao_esquerda(no);


    if (balanceamento > 1 && chave > no->esq->chave) {
        no->esq = rotacao_esquerda(no->esq);
        return rotacao_direita(no);
    }

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

                    /*Mostrar todos os dados da linha*/
                    int indice_linha = linha_encontrada - 1; 
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

/*Testes AVL*/

void teste_busca_individual_avl(NoAVL *raiz, Dataset *dataset, int chave) {
    printf("\nBUSCA INDIVIDUAL AVL\n");
    printf("Buscando código CNUC: %d\n", chave);
    
    int resultado = buscar_avl(raiz, chave);
    
    if (resultado != -1) {
        printf("ENCONTRADO na linha: %d\n", resultado);
        
        printar_linha_completa(dataset, resultado);
    } else {
        printf("NÃO ENCONTRADO\n");
        printf("O código CNUC %d não existe no banco de dados.\n", chave);
    }
}

void teste_estrutura_avl(NoAVL *raiz, Dataset *dataset) {
    printf("\nESTRUTURA DA ÁRVORE AVL\n");
    
    printf("Percorrendo em ORDEM (primeiros 5 elementos):\n");
    int contador = 0;
    void percorrer_limitado_avl(NoAVL *raiz, int *cont, Dataset *ds) {
        if (raiz != NULL && *cont < 5) {
            percorrer_limitado_avl(raiz->esq, cont, ds);
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
            percorrer_limitado_avl(raiz->dir, cont, ds);
        }
    }
    percorrer_limitado_avl(raiz, &contador, dataset);
    if (contador == 5) {
        printf("   ... (árvore contém mais elementos)\n");
    }
}


void menu_testes_avl(NoAVL *raiz, Dataset *dataset, Matriz *matriz, int tamanho_matriz) {
    int opcao;
    
    do {
        printf("\nMENU DE TESTES - ÁRVORE AVL\n");
        printf("1. Busca individual por código CNUC\n");
        printf("2. Busca múltipla por códigos CNUC (usando buscar_cnuc_avl)\n");
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
                    teste_busca_individual_avl(raiz, dataset, chave);
                } else {
                    printf("Código inválido!\n");
                }
                while (getchar() != '\n');
                break;
                
            case 2:
                buscar_cnuc_avl(raiz, dataset);
                break;
                
            case 0:
                printf("Saindo do menu de testes AVL...\n");
                break;
                
            default:
                printf("Opção inválida!\n");
        }
    } while (opcao != 0);
}