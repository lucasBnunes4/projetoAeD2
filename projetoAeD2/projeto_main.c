#include "projeto_aed2.h"


void menu_escolha_arvore(NoABB *arvore_abb, NoAVL *arvore_avl, Dataset *dataset, Matriz *matriz, int tamanho_matriz) {
    int opcao;
    
    do {
        printf("\nESCOLHA DA ÁRVORE PARA TESTES\n");
        printf("1. Testar Arvore Binária de Busca (ABB)\n");
        printf("2. Testar Arvore AVL\n");
        printf("3. Mostrar informacoes do dataset\n");
        printf("0. Sair do programa\n");
        printf("Escolha uma opcao: ");
        
        if (scanf("%d", &opcao) != 1) {
            printf("Entrada inválida!\n");
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');
        
        switch (opcao) {
            case 1:
                printf("\n=== INICIANDO TESTES NA ARVORE BINARIA DE BUSCA ===\n");
                menu_testes_abb(arvore_abb, dataset, matriz, tamanho_matriz);
                break;
                
            case 2:
                printf("\n=== INICIANDO TESTES NA ARVORE AVL ===\n");
                menu_testes_avl(arvore_avl, dataset, matriz, tamanho_matriz);
                break;
                
            case 3:
                printf("\n=== INFORMACOES DO DATASET ===\n");
                analisar_dataset(dataset);
                printar_matriz(matriz, tamanho_matriz);
                break;
                
            case 0:
                printf("Saindo do programa.\n");
                break;
                
            default:
                printf("Opção invalida! Tente novamente.\n");
        }
    } while (opcao != 0);
}

int main (void) {
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

    printf("\nAnalise DATASET\n");
    analisar_dataset(dataset_limpo);
    
    int tamanho_matriz;
    Matriz *matriz = criar_matriz(dataset_limpo, &tamanho_matriz);
    printar_matriz(matriz, tamanho_matriz);

    printf("Construindo árvore binaria de busca\n");
    NoABB *arvore_abb = construir_abb(matriz, tamanho_matriz);
    if (!arvore_abb) {
        printf("Falha ao construir arvore\n");
        liberar_mem_matriz(matriz);
        liberar_memoria_dataset(dataset);
        return 1;
    }
    
    printf("Construindo Árvore AVL\n");
    NoAVL *arvore_avl = construir_avl(matriz, tamanho_matriz);
    if (!arvore_avl) {
        printf("Falha ao construir Arvore AVL\n");
        liberar_abb(arvore_abb);
        liberar_mem_matriz(matriz);
        liberar_memoria_dataset(dataset);
        liberar_memoria_dataset(dataset_limpo);
        return 1;
    }

    printf("\nSistema carregado com sucesso!\n");
    printf("   • %d registros no banco de dados\n", dataset->num_linhas);
    printf("   • %d códigos CNUC indexados\n", tamanho_matriz);
    printf("     - Arvore Binária de Busca (ABB)\n");
    printf("     - Arvore AVL\n");

    menu_escolha_arvore(arvore_abb, arvore_avl, dataset_limpo, matriz, tamanho_matriz);

    /*Liberação de memória*/
    liberar_abb(arvore_abb);
    liberar_avl(arvore_avl);
    liberar_mem_matriz(matriz);
    liberar_memoria_dataset(dataset);
    liberar_memoria_dataset(dataset_limpo);

    printf("Programa finalizado com sucesso!\n");
    return 0;
}