// ============================================================================
//  PROJETO WAR ESTRUTURADO - NÍVEL MESTRE (CORRIGIDO E BLINDADO)
// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <ctype.h> // [NOVO] Necessário para toupper()

// --- Constantes e Definições ---
#define TAM_NOME 30
#define TAM_COR 15 // Aumentei um pouco para segurança
#define QTD_MISSOES 5

struct Territorio {
    char nome[TAM_NOME];
    char cor[TAM_COR];
    int tropas;
};

const char* LISTA_MISSOES[QTD_MISSOES] = {
    "Conquistar 3 territorios",
    "Conquistar todo o mapa",
    "Eliminar o exercito VERMELHO", // Tudo em maiúsculo para padronizar
    "Ter mais de 10 tropas totais",
    "Eliminar o exercito AZUL"     // Tudo em maiúsculo para padronizar
};

// --- Protótipos ---
void configurarJogo(struct Territorio** mapa, char** missao, int* qtd, char* cor);
void rodarGameLoop(struct Territorio* mapa, char* missao, int qtd, char* cor);
int obterOpcaoMenu();
int executarAcao(int opcao, struct Territorio* mapa, int qtd, char* missao, char* cor);

// Funções Auxiliares
void cadastrarTerritorios(struct Territorio* mapa, int qtd);
void exibirMapa(const struct Territorio* mapa, int qtd);
void atribuirMissao(char* destino);
void atacar(struct Territorio* atacante, struct Territorio* defensor);
int verificarMissao(char* missao, struct Territorio* mapa, int qtd, char* corJogador);
void liberarMemoria(struct Territorio* mapa, char* missao);
void limparBuffer();
void normalizarString(char* str); // [NOVO] Função mágica que resolve seu problema

// ============================================================================
//                               FUNÇÃO MAIN
// ============================================================================
int main() {
    setlocale(LC_ALL, "Portuguese");
    srand(time(NULL));

    struct Territorio* mapa = NULL;
    char* minhaMissao = NULL;
    char minhaCor[TAM_COR];
    int qtdTerritorios = 0;

    configurarJogo(&mapa, &minhaMissao, &qtdTerritorios, minhaCor);
    rodarGameLoop(mapa, minhaMissao, qtdTerritorios, minhaCor);
    liberarMemoria(mapa, minhaMissao);

    return 0;
}

// ============================================================================
//                           IMPLEMENTAÇÃO
// ============================================================================

/**
 * [NOVO] Converte qualquer string para MAIÚSCULAS.
 * Resolve o problema de "azul" vs "Azul".
 */
void normalizarString(char* str) {
    for(int i = 0; str[i]; i++){
        str[i] = toupper(str[i]);
    }
}

void configurarJogo(struct Territorio** mapa, char** missao, int* qtd, char* cor) {
    printf("=== WAR: CONFIGURAÇÃO INICIAL ===\n");
    
    printf("Escolha sua cor (ex: AZUL): ");
    // %14[^\n] lê no máximo 14 chars para não estourar o vetor de 15
    scanf(" %14[^\n]", cor); 
    normalizarString(cor); // Converte o que o usuário digitou para MAIÚSCULO
    
    printf("Quantos territórios? ");
    scanf("%d", qtd);

    *mapa = (struct Territorio*) calloc(*qtd, sizeof(struct Territorio));
    *missao = (char*) malloc(100 * sizeof(char));

    if (*mapa == NULL || *missao == NULL) {
        printf("Erro Crítico: Falha de Memória.\n");
        exit(1);
    }

    cadastrarTerritorios(*mapa, *qtd);
    atribuirMissao(*missao);
    
    printf("\n>>> Configuração Concluída! Você é o exército: %s <<<\n", cor);
    printf("Sua Missão: %s\n", *missao);
    
    // Pausa para leitura
    printf("\nPressione ENTER para começar...");
    limparBuffer();
    getchar();
}

void cadastrarTerritorios(struct Territorio* mapa, int qtd) {
    for (int i = 0; i < qtd; i++) {
        printf("\n--- Cadastro Território %d ---\n", i + 1);
        
        printf("Nome: "); 
        scanf(" %29[^\n]", mapa[i].nome); // Proteção contra overflow
        
        printf("Cor do Dono: "); 
        scanf(" %14[^\n]", mapa[i].cor);  // Proteção contra overflow
        normalizarString(mapa[i].cor);    // Padroniza para MAIÚSCULO
        
        printf("Tropas: "); 
        scanf("%d", &mapa[i].tropas);
    }
}

void rodarGameLoop(struct Territorio* mapa, char* missao, int qtd, char* cor) {
    int jogando = 1;
    while (jogando) {
        int opcao = obterOpcaoMenu();
        jogando = executarAcao(opcao, mapa, qtd, missao, cor);
    }
}

int obterOpcaoMenu() {
    int op;
    printf("\n--------------------------------------------\n");
    printf("1. Ver Mapa\n");
    printf("2. Atacar Território\n");
    printf("3. Verificar STATUS da Missão\n");
    printf("0. Sair do Jogo\n");
    printf("--------------------------------------------\n");
    printf("Sua escolha: ");
    scanf("%d", &op);
    limparBuffer();
    return op;
}

int executarAcao(int opcao, struct Territorio* mapa, int qtd, char* missao, char* cor) {
    switch (opcao) {
        case 1:
            exibirMapa(mapa, qtd);
            return 1;
        
        case 2: {
            int atk, def;
            exibirMapa(mapa, qtd);
            printf("\nDigitar ID do Atacante: "); scanf("%d", &atk);
            printf("Digitar ID do Defensor: "); scanf("%d", &def);
            
            if (atk > 0 && atk <= qtd && def > 0 && def <= qtd) {
                atacar(&mapa[atk-1], &mapa[def-1]);
            } else {
                printf("\n[!] IDs inválidos.\n");
            }
            return 1;
        }

        case 3:
            // Se retornar 1, venceu.
            if (verificarMissao(missao, mapa, qtd, cor)) {
                printf("\n\n***************************************\n");
                printf("     PARABÉNS! MISSÃO CUMPRIDA!      \n");
                printf("        VOCÊ VENCEU O JOGO!          \n");
                printf("***************************************\n\n");
                return 0; // Encerra o loop
            } else {
                printf("\n[i] Missão ainda pendente. Continue jogando.\n");
                printf("    Objetivo: %s\n", missao);
            }
            return 1;

        case 0:
            printf("Encerrando...\n");
            return 0;

        default:
            printf("Opção inválida.\n");
            return 1;
    }
}

void atacar(struct Territorio* atacante, struct Territorio* defensor) {
    // Como normalizamos tudo, o strcmp vai funcionar perfeitamente agora
    if (strcmp(atacante->cor, defensor->cor) == 0) {
        printf("\n[!] Erro: Você não pode atacar seu próprio território (%s)!\n", atacante->cor);
        return;
    }
    if (atacante->tropas <= 1) {
        printf("\n[!] Erro: Território atacante precisa de mais de 1 tropa.\n");
        return;
    }

    printf("\n>>> BATALHA: %s (%s) vs %s (%s) <<<\n", 
           atacante->nome, atacante->cor, defensor->nome, defensor->cor);

    int d1 = (rand() % 6) + 1;
    int d2 = (rand() % 6) + 1;
    printf("Dados: Atacante [%d] x Defensor [%d]\n", d1, d2);

    if (d1 > d2) {
        printf("RESULTADO: Vitoria do Ataque!\n");
        printf("O território %s agora pertence ao exército %s!\n", defensor->nome, atacante->cor);
        
        // Troca o dono
        strcpy(defensor->cor, atacante->cor);
        
        // Move metade das tropas (lógica ajustada para garantir movimento justo)
        int tropasParaMover = atacante->tropas / 2;
        defensor->tropas = tropasParaMover;
        atacante->tropas -= tropasParaMover;
    } else {
        printf("RESULTADO: A Defesa segurou o ataque!\n");
        printf("O atacante perdeu 1 tropa.\n");
        atacante->tropas--;
    }
}

void exibirMapa(const struct Territorio* mapa, int qtd) {
    printf("\n%-3s | %-20s | %-12s | %s\n", "ID", "NOME", "COR (DONO)", "TROPAS");
    printf("----------------------------------------------------------\n");
    for(int i=0; i<qtd; i++) {
        printf("%02d  | %-20s | %-12s | %d\n", 
            i+1, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
}

void atribuirMissao(char* destino) {
    int r = rand() % QTD_MISSOES;
    strcpy(destino, LISTA_MISSOES[r]);
}

int verificarMissao(char* missao, struct Territorio* mapa, int qtd, char* corJogador) {
    int meusTerritorios = 0;
    int minhasTropas = 0;
    int qtdVermelhos = 0; 
    int qtdAzuis = 0;     

    // Varredura Estatística
    for(int i = 0; i < qtd; i++) {
        // Agora o strcmp compara "AZUL" com "AZUL". Sucesso!
        if (strcmp(mapa[i].cor, corJogador) == 0) {
            meusTerritorios++;
            minhasTropas += mapa[i].tropas;
        }
        
        if (strcmp(mapa[i].cor, "VERMELHO") == 0) qtdVermelhos++;
        if (strcmp(mapa[i].cor, "AZUL") == 0) qtdAzuis++;
    }

    // Debug: Descomente a linha abaixo se quiser ver a contagem em tempo real
    // printf("\n[DEBUG] Meus Terr: %d/%d | Minhas Tropas: %d\n", meusTerritorios, qtd, minhasTropas);

    // Lógica das Missões (Strings de busca ajustadas para os textos da constante)
    
    if (strstr(missao, "3 territorios") && meusTerritorios >= 3) return 1;
    
    if (strstr(missao, "todo o mapa") && meusTerritorios == qtd) return 1;
    
    if (strstr(missao, "10 tropas") && minhasTropas > 10) return 1;
    
    if (strstr(missao, "VERMELHO")) {
        // Se eu não sou vermelho e vermelhos acabaram
        if (strcmp(corJogador, "VERMELHO") != 0 && qtdVermelhos == 0) return 1;
    }
    
    if (strstr(missao, "AZUL")) {
        if (strcmp(corJogador, "AZUL") != 0 && qtdAzuis == 0) return 1;
    }

    return 0;
}

void liberarMemoria(struct Territorio* mapa, char* missao) {
    if (mapa) free(mapa);
    if (missao) free(missao);
    printf("\nSistema encerrado e memória liberada.\n");
}

void limparBuffer() {
    int c; while ((c = getchar()) != '\n' && c != EOF);
}
