#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NOME 50
#define MAX_COR 20
#define MAX_TERRITORIOS 42

typedef struct {
    char nome[MAX_NOME];
    char cor[MAX_COR];
    int tropas;
    int jogador; // 0 = neutro, 1+ = jogador específico
} Territorio;

typedef struct {
    Territorio *territorios;
    int quantidade;
    int capacidade;
} Mapa;

// Inicializa o mapa
void inicializarMapa(Mapa *mapa) {
    mapa->capacidade = MAX_TERRITORIOS;
    mapa->quantidade = 0;
    mapa->territorios = (Territorio*)malloc(mapa->capacidade * sizeof(Territorio));
    
    if (mapa->territorios == NULL) {
        printf("Erro ao alocar memória!\n");
        exit(1);
    }
}

// Adiciona um território ao mapa
void adicionarTerritorio(Mapa *mapa, const char *nome, const char *cor, int tropas, int jogador) {
    if (mapa->quantidade >= mapa->capacidade) {
        printf("Mapa cheio!\n");
        return;
    }
    
    Territorio *t = &mapa->territorios[mapa->quantidade];
    strncpy(t->nome, nome, MAX_NOME);
    strncpy(t->cor, cor, MAX_COR);
    t->tropas = tropas;
    t->jogador = jogador;
    
    mapa->quantidade++;
}

// Exibe todos os territórios
void exibirTerritorios(const Mapa *mapa) {
    printf("\n=== TERRITÓRIOS DO WAR ===\n");
    for (int i = 0; i < mapa->quantidade; i++) {
        printf("Território: %-20s | Cor: %-15s | Tropas: %3d | Jogador: %d\n",
               mapa->territorios[i].nome,
               mapa->territorios[i].cor,
               mapa->territorios[i].tropas,
               mapa->territorios[i].jogador);
    }
    printf("Total: %d territórios\n", mapa->quantidade);
}

// Libera a memória do mapa
void liberarMapa(Mapa *mapa) {
    free(mapa->territorios);
    mapa->territorios = NULL;
    mapa->quantidade = 0;
    mapa->capacidade = 0;
}   
typedef struct {
    Territorio *atacante;
    Territorio *defensor;
    int tropas_ataque;
    int tropas_defesa;
} Batalha;

// Função para rolar dados (1-6)
int rolarDado() {
    return rand() % 6 + 1;
}

// Encontra território pelo nome
Territorio* encontrarTerritorio(Mapa *mapa, const char *nome) {
    for (int i = 0; i < mapa->quantidade; i++) {
        if (strcmp(mapa->territorios[i].nome, nome) == 0) {
            return &mapa->territorios[i];
        }
    }
    return NULL;
}

// Simula uma batalha entre dois territórios
Batalha* simularBatalha(Mapa *mapa, const char *nomeAtacante, const char *nomeDefensor, int tropasAtaque) {
    Territorio *atacante = encontrarTerritorio(mapa, nomeAtacante);
    Territorio *defensor = encontrarTerritorio(mapa, nomeDefensor);
    
    if (atacante == NULL || defensor == NULL) {
        printf("Território não encontrado!\n");
        return NULL;
    }
    
    if (atacante->jogador == defensor->jogador) {
        printf("Não pode atacar território do mesmo jogador!\n");
        return NULL;
    }
    
    if (tropasAtaque >= atacante->tropas) {
        printf("Não tem tropas suficientes para atacar!\n");
        return NULL;
    }
    
    // Aloca memória para a batalha
    Batalha *batalha = (Batalha*)malloc(sizeof(Batalha));
    batalha->atacante = atacante;
    batalha->defensor = defensor;
    batalha->tropas_ataque = tropasAtaque;
    batalha->tropas_defesa = defensor->tropas;
    
    printf("\n=== BATALHA ===\n");
    printf("%s (%d tropas) ataca %s (%d tropas)\n",
           atacante->nome, tropasAtaque, defensor->nome, defensor->tropas);
    
    // Simula a batalha
    int dadosAtaque[3], dadosDefesa[2];
    
    // Rola dados do atacante (até 3 dados)
    int numDadosAtaque = (tropasAtaque >= 3) ? 3 : tropasAtaque;
    for (int i = 0; i < numDadosAtaque; i++) {
        dadosAtaque[i] = rolarDado();
    }
    
    // Rola dados do defensor (até 2 dados)
    int numDadosDefesa = (defensor->tropas >= 2) ? 2 : defensor->tropas;
    for (int i = 0; i < numDadosDefesa; i++) {
        dadosDefesa[i] = rolarDado();
    }
    
    // Ordena dados em ordem decrescente
    for (int i = 0; i < numDadosAtaque - 1; i++) {
        for (int j = i + 1; j < numDadosAtaque; j++) {
            if (dadosAtaque[i] < dadosAtaque[j]) {
                int temp = dadosAtaque[i];
                dadosAtaque[i] = dadosAtaque[j];
                dadosAtaque[j] = temp;
            }
        }
    }
    
    for (int i = 0; i < numDadosDefesa - 1; i++) {
        for (int j = i + 1; j < numDadosDefesa; j++) {
            if (dadosDefesa[i] < dadosDefesa[j]) {
                int temp = dadosDefesa[i];
                dadosDefesa[i] = dadosDefesa[j];
                dadosDefesa[j] = temp;
            }
        }
    }
    
    printf("Dados atacante: ");
    for (int i = 0; i < numDadosAtaque; i++) printf("%d ", dadosAtaque[i]);
    printf("\nDados defensor: ");
    for (int i = 0; i < numDadosDefesa; i++) printf("%d ", dadosDefesa[i]);
    printf("\n");
    
    // Compara os dados
    int comparacoes = (numDadosAtaque < numDadosDefesa) ? numDadosAtaque : numDadosDefesa;
    int perdasAtacante = 0, perdasDefensor = 0;
    
    for (int i = 0; i < comparacoes; i++) {
        if (dadosAtaque[i] > dadosDefesa[i]) {
            perdasDefensor++;
        } else {
            perdasAtacante++;
        }
    }
    
    printf("Resultado: Atacante perde %d, Defensor perde %d\n", perdasAtacante, perdasDefensor);
    
    // Atualiza tropas
    atacante->tropas -= perdasAtacante;
    defensor->tropas -= perdasDefensor;
    
    // Verifica se o território foi conquistado
    if (defensor->tropas <= 0) {
        printf("%s foi conquistado!\n", defensor->nome);
        defensor->jogador = atacante->jogador;
        defensor->tropas = tropasAtaque - perdasAtacante;
        atacante->tropas -= (tropasAtaque - perdasAtacante);
    }
    
    return batalha;
}

// Libera memória da batalha
void liberarBatalha(Batalha *batalha) {
    free(batalha);
}
typedef struct {
    char descricao[100];
    int (*verificarVitoria)(Mapa*, int);
    int jogador_alvo;
} Missao;

// Missão: Conquistar um continente específico
int verificarContinente(Mapa *mapa, int jogador) {
    // Exemplo: América do Sul (4 territórios)
    const char *americaSul[] = {"Brasil", "Argentina", "Peru", "Venezuela"};
    int conquistados = 0;
    
    for (int i = 0; i < 4; i++) {
        Territorio *t = encontrarTerritorio(mapa, americaSul[i]);
        if (t != NULL && t->jogador == jogador)