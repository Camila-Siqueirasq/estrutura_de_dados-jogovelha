/* ========================================================================== */
/* UNIVERSIDADE FEDERAL DO PARANÁ - UFPR                                      */                                      */
/*                                                                            */
/* REFERÊNCIA DO ALGORITMO DO COMPUTADOR:                  */
/* Estratégia: Seleção Aleatória de Posições (Random Move / Rejection)        */
/* Autor da Referência: Rafael Stoffalette João (rafaelstojoao@gmail.com) 
 https://github.com/rafaelstojoao/jogo-da-velha-em-C/blob/master/velhaComentada.c   */
/* Descrição Lógica: Sorteio pseudoaleatório de coordenadas utilizando        */
/*                  as funções rand() e time(NULL) da biblioteca padrão.      */
/* ========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


// Nó para armazenar as coordenadas de cada jogada individual (linha-coluna)
typedef struct Nodo_Jogada {
    int linha;
    int coluna;
    struct Nodo_Jogada *prox;
} Nodo_Jogada;

// Nó para armazenar o histórico completo de uma partida na memória RAM
typedef struct Nodo_Partida {
    int id_partida;
    char nome_usuario[50];
    char nome_computador[50];
    char resultado[50];
    Nodo_Jogada *jogadas_usuario;    
    Nodo_Jogada *jogadas_computador; 
    struct Nodo_Partida *prox;
} Nodo_Partida;

// Nó para acumular as vitórias e ordenar o ranking de forma decrescente
typedef struct Nodo_Ranking {
    char nome[50];
    int vitorias;
    struct Nodo_Ranking *prox;
} Nodo_Ranking;


/* PROTÓTIPO DAS FUNÇÕES                                                   */

void jogar(Nodo_Partida **historico, int *quem_comeca, int *proximo_id);
void executar_uma_partida(Nodo_Partida **historico, int *quem_comeca, int *proximo_id, char nome_usuario_fixo[50]);
void exibir_jogadas(Nodo_Jogada *lista);
void exibir_historico_sessao(Nodo_Partida *inicio_conjunto);

void salvar_partidas(Nodo_Partida **historico);
void ranquear_usuarios();
void inserir_ranking_ordenado(Nodo_Ranking **lista, char nome[50]);

void inserir_jogada(Nodo_Jogada **lista, int linha, int coluna);
void inserir_partida(Nodo_Partida **historico, Nodo_Partida *nova_partida);
void liberar_memoria(Nodo_Partida **historico);

char gridChar(int i);
void draw(int b[9]);
int win(const int board[9]);
int computerMoveRandom(int board[9], int peca);
int playerMove(int board[9], int peca);


/*  MAIN E MENU DE OPÇÕES                                       */

int main() {
    // Inicialização da semente de aleatoriedade
    srand((unsigned int)time(NULL));

    // Variáveis locais para controle de estado
    Nodo_Partida *historico_sessao = NULL; 
    int quem_comeca_jogo = 0; // 0 = Não sorteado, 1 = Usuário, 2 = Computador
    int proximo_id_partida = 1;
    int opcao = 0;
    char confirmacao;

    while (opcao != 4) {
        printf("\n============================================\n");
        printf("   JOGO DA VELHA - ESTRUTURAS DE DADOS I\n");
        printf("============================================\n\n");
        printf("1) Jogar partidas de Jogo da Velha\n");
        printf("2) Salvar as partidas do Jogo da Velha\n");
        printf("3) Ranquear os usuarios do Jogo da Velha\n");
        printf("4) Sair do Jogo da Velha\n");
        printf("Escolha uma opcao: ");
        
        if (scanf("%d", &opcao) != 1) {
            while (getchar() != '\n'); // Limpa buffer em caso de caracter inválido
            opcao = 0;
            continue;
        }

        switch (opcao) {
            case 1:
                jogar(&historico_sessao, &quem_comeca_jogo, &proximo_id_partida);
                break;
            case 2:
                salvar_partidas(&historico_sessao);
                break;
            case 3:
                ranquear_usuarios();
                break;
            case 4:
                if (historico_sessao != NULL) {
                    printf("\nExistem partidas nao salvas na sessao atual.\n");
                    printf("Deseja salvar as partidas antes de sair? (S/N): ");
                    scanf(" %c", &confirmacao); 
                    if (confirmacao == 'S' || confirmacao == 's') {
                        salvar_partidas(&historico_sessao);
                    }
                }
                printf("Liberando memoria dinamica alocada...\n");
                liberar_memoria(&historico_sessao); 
                printf("Programa encerrado com sucesso.\n");
                exit(0);
            default:
                printf("Opcao invalida! Escolha entre 1 e 4.\n");
        }
    }
    return 0; 
}


/* MECÂNICA DO TABULEIRO E ALGORITMO DA MÁQUINA                     */

char gridChar(int i) {
    switch(i) {
        case -1: return 'X';
        case 0:  return ' ';
        case 1:  return 'O';
    }
    return ' ';
}

void draw(int b[9]) {
    printf("\n\n");
    printf(" %c | %c | %c\n", gridChar(b[0]), gridChar(b[1]), gridChar(b[2]));
    printf("---+---+---\n");
    printf(" %c | %c | %c\n", gridChar(b[3]), gridChar(b[4]), gridChar(b[5]));
    printf("---+---+---\n");
    printf(" %c | %c | %c\n\n", gridChar(b[6]), gridChar(b[7]), gridChar(b[8]));
}

int win(const int board[9]) {
    unsigned wins[8][3] = {
        {0,1,2},{3,4,5},{6,7,8}, // Linhas
        {0,3,6},{1,4,7},{2,5,8}, // Colunas
        {0,4,8},{2,4,6}          // Diagonais
    };
    int i;
    for(i = 0; i < 8; ++i) {
        if(board[wins[i][0]] != 0 &&
           board[wins[i][0]] == board[wins[i][1]] &&
           board[wins[i][0]] == board[wins[i][2]])
            return board[wins[i][2]];
    }
    return 0;
}

/* Algoritmo: Baseado na referência*/

int computerMoveRandom(int board[9], int peca) {
    int espacos_vazios[9];
    int count = 0;
    int i;

    // Identifica as casas vazias disponíveis no tabuleiro
    for (i = 0; i < 9; i++) {
        if (board[i] == 0) {
            espacos_vazios[count] = i;
            count++;
        }
    }

    if (count > 0) {
        int indice_sorteado = rand() % count; 
        int jogada = espacos_vazios[indice_sorteado];
        board[jogada] = peca; 
        return jogada;
    }
    return -1;
}

int playerMove(int board[9], int peca) {
    int move = 0, index = -1;
    do {
        printf("Sua vez. Digite a posicao do Numpad (1 a 9): ");
        if (scanf("%d", &move) != 1) {
            while (getchar() != '\n');
            move = 0;
        }

        switch(move) {
            case 7: index = 0; break; case 8: index = 1; break; case 9: index = 2; break;
            case 4: index = 3; break; case 5: index = 4; break; case 6: index = 5; break;
            case 1: index = 6; break; case 2: index = 7; break; case 3: index = 8; break;
            default: index = -1; break;
        }

        if (index == -1 || board[index] != 0) {
            printf("Posicao invalida ou ja ocupada! Tente novamente.\n");
        }
    } while (index == -1 || board[index] != 0);

    board[index] = peca; 
    return index; 
}


/* GESTÃO DE PARTIDAS E HISTÓRICO DA SESSÃO                                */

void exibir_jogadas(Nodo_Jogada *lista) {
    if (lista == NULL) {
        printf("Nenhuma jogada realizada.\n");
        return;
    }
    Nodo_Jogada *aux = lista;
    while (aux != NULL) {
        printf("%d-%d ", aux->linha, aux->coluna);
        aux = aux->prox;
    }
    printf("\n");
}

void exibir_historico_sessao(Nodo_Partida *inicio_conjunto) {
    if (inicio_conjunto == NULL) {
        printf("\nNenhuma partida foi realizada neste conjunto.\n");
        return;
    }

    int vitorias_usuario = 0;
    int vitorias_pc = 0;
    int empates = 0;
    char nome_user[50];
    strcpy(nome_user, inicio_conjunto->nome_usuario);

    printf("\n======================================================\n");
    printf("        RELATORIO DO CONJUNTO DE PARTIDAS             \n");
    printf("======================================================\n\n");

    Nodo_Partida *p = inicio_conjunto;
    while (p != NULL) {
        printf("\n------------------------------------------------------\n");
        printf("Partida ID: %d\n", p->id_partida);
        printf("Resultado: %s\n", p->resultado);

        if (strcmp(p->resultado, "Empate") == 0) {
            empates++;
            printf("-> Empate registrado.\n");
            printf("Jogadas de %s: ", p->nome_usuario);
            exibir_jogadas(p->jogadas_usuario);
            printf("Jogadas do %s: ", p->nome_computador);
            exibir_jogadas(p->jogadas_computador);
        } else if (strcmp(p->resultado, p->nome_usuario) == 0) {
            vitorias_usuario++;
            printf("Vencedor: %s\n", p->nome_usuario);
            printf("Jogadas do Vencedor: ");
            exibir_jogadas(p->jogadas_usuario);
        } else {
            vitorias_pc++;
            printf("Vencedor: %s\n", p->nome_computador);
            printf("Jogadas do Vencedor: ");
            exibir_jogadas(p->jogadas_computador);
        }

        p = p->prox;
    }

    printf("\n======================================================\n");
    printf("                 PLACAR GERAL DA RODADA               \n");
    printf("======================================================\n\n");
    printf("%s: %d vitoria(s)\n", nome_user, vitorias_usuario);
    printf("Computador: %d vitoria(s)\n", vitorias_pc);
    printf("Empates: %d\n", empates);
    printf("------------------------------------------------------\n");

    if (vitorias_usuario > vitorias_pc) {
        printf(">> VENCEDOR GERAL DO CONJUNTO: %s <<\n", nome_user);
    } else if (vitorias_pc > vitorias_usuario) {
        printf(">> VENCEDOR GERAL DO CONJUNTO: Computador <<\n");
    } else {
        printf(">> RESULTADO GERAL DO CONJUNTO: Empate Tecnico <<\n");
    }
    printf("======================================================\n\n");
}

void executar_uma_partida(Nodo_Partida **historico, int *quem_comeca, int *proximo_id, char nome_usuario_fixo[50]) {
    int board[9] = {0,0,0,0,0,0,0,0,0};
    int turn_limit, last_move;
    int peca_usuario, peca_pc;
    
    Nodo_Partida *nova_partida = (Nodo_Partida *) malloc(sizeof(Nodo_Partida));
    nova_partida->jogadas_usuario = NULL;
    nova_partida->jogadas_computador = NULL;
    nova_partida->prox = NULL;
    strcpy(nova_partida->nome_computador, "Computador");
    strcpy(nova_partida->nome_usuario, nome_usuario_fixo);

    nova_partida->id_partida = (*proximo_id)++;

    // Sorteio de Par ou Ímpar apenas na partida inicial
    if (*quem_comeca == 0) {
        int escolha, num_user, num_pc, soma;
        printf("\n--- SORTEIO INICIAL (PAR OU IMPAR) ---\n");
        printf("Quem vencer jogara com o 'X' e comecara a primeira partida!\n");
        printf("Escolha: PAR (0) ou IMPAR (1): ");
        scanf("%d", &escolha);
        printf("Digite um numero de dedos (0 a 5): ");
        scanf("%d", &num_user);
        
        num_pc = rand() % 6;
        soma = num_user + num_pc;
        printf("Computador jogou %d. Soma = %d (%s).\n", num_pc, soma, (soma % 2 == 0) ? "PAR" : "IMPAR");

        if ((soma % 2) == escolha) {
            printf(">> VOCE venceu o sorteio! Inicia com 'X'.\n");
            *quem_comeca = 1;
        } else {
            printf(">> COMPUTADOR venceu o sorteio! Inicia com 'X'.\n");
            *quem_comeca = 2;
        }
    } else {
        printf("\n--- PARTIDA #%d ---\n", nova_partida->id_partida);
        printf("Alternancia de primeiro movimento aplicada com sucesso!\n");
    }

    // Configuração de peças de acordo com quem começa
    if (*quem_comeca == 1) {
        peca_usuario = -1; // X
        peca_pc = 1;       // O
        printf("Em campo: %s (X) vs Computador (O)\n", nova_partida->nome_usuario);
    } else {
        peca_usuario = 1;  // O
        peca_pc = -1;      // X
        printf("Em campo: Computador (X) vs %s (O)\n", nova_partida->nome_usuario);
    }

    printf("\n=== GUIA DO TECLADO NUMPAD ===\n 7 | 8 | 9 \n---+---+---\n 4 | 5 | 6 \n---+---+---\n 1 | 2 | 3 \n==============================\n");

    for (turn_limit = 0; turn_limit < 9 && win(board) == 0; ++turn_limit) {
        if (turn_limit % 2 == 0) {
            if (*quem_comeca == 1) {
                draw(board);
                last_move = playerMove(board, peca_usuario);
                inserir_jogada(&(nova_partida->jogadas_usuario), last_move / 3, last_move % 3);
            } else {
                last_move = computerMoveRandom(board, peca_pc);
                inserir_jogada(&(nova_partida->jogadas_computador), last_move / 3, last_move % 3);
                printf("\nO Computador realizou sua jogada.\n");
            }
        } else {
            if (*quem_comeca == 1) {
                last_move = computerMoveRandom(board, peca_pc);
                inserir_jogada(&(nova_partida->jogadas_computador), last_move / 3, last_move % 3);
                printf("\nO Computador realizou sua jogada.\n");
            } else {
                draw(board);
                last_move = playerMove(board, peca_usuario);
                inserir_jogada(&(nova_partida->jogadas_usuario), last_move / 3, last_move % 3);
            }
        }
    }
    
    draw(board);
    
    int ganhador = win(board);
    if (ganhador == peca_pc) {
        printf(">> Fim de jogo: O Computador Venceu! <<\n");
        strcpy(nova_partida->resultado, nova_partida->nome_computador);
    } else if (ganhador == peca_usuario) {
        printf(">> Fim de jogo: Parabens, voce Venceu! <<\n");
        strcpy(nova_partida->resultado, nova_partida->nome_usuario);
    } else {
        printf(">> Fim de jogo: Deu Velha (Empate)! <<\n");
        strcpy(nova_partida->resultado, "Empate");
    }

    inserir_partida(historico, nova_partida);

    // Alternância estrita para o próximo jogo
    *quem_comeca = (*quem_comeca == 1) ? 2 : 1;
}

void jogar(Nodo_Partida **historico, int *quem_comeca, int *proximo_id) {
    char nome_usuario[50];
    char continuar = 'S';

    printf("\nIdentificacao do Jogador: ");
    scanf("%49s", nome_usuario);

    // Localiza o nó final atual para demarcar o início deste novo lote de jogos
    Nodo_Partida *inicio_conjunto = NULL;
    Nodo_Partida *ultimo_existente = *historico;
    while (ultimo_existente != NULL && ultimo_existente->prox != NULL) {
        ultimo_existente = ultimo_existente->prox;
    }

    while (continuar == 'S' || continuar == 's') {
        executar_uma_partida(historico, quem_comeca, proximo_id, nome_usuario);

        if (inicio_conjunto == NULL) {
            if (ultimo_existente == NULL) {
                inicio_conjunto = *historico;
            } else {
                inicio_conjunto = ultimo_existente->prox;
            }
        }

        printf("\nDeseja jogar outra partida? (S/N): ");
        scanf(" %c", &continuar);
    }

    // Ao sair do fluxo de partidas, emite o relatório obrigatório
    exibir_historico_sessao(inicio_conjunto);
}


/* MANIPULAÇÃO DAS LISTAS ENCADEADAS                                       */

void inserir_jogada(Nodo_Jogada **lista, int linha, int coluna) {
    Nodo_Jogada *novo = (Nodo_Jogada *) malloc(sizeof(Nodo_Jogada));
    novo->linha = linha;
    novo->coluna = coluna;
    novo->prox = NULL;

    if (*lista == NULL) {
        *lista = novo;
    } else {
        Nodo_Jogada *aux = *lista;
        while (aux->prox != NULL) aux = aux->prox;
        aux->prox = novo;
    }
}

void inserir_partida(Nodo_Partida **historico, Nodo_Partida *nova_partida) {
    if (*historico == NULL) {
        *historico = nova_partida;
    } else {
        Nodo_Partida *aux = *historico;
        while (aux->prox != NULL) aux = aux->prox;
        aux->prox = nova_partida;
    }
}

void liberar_memoria(Nodo_Partida **historico) {
    if (historico == NULL || *historico == NULL) return;

    Nodo_Partida *part_atual = *historico;
    while (part_atual != NULL) {
        Nodo_Partida *part_prox = part_atual->prox;
        
        Nodo_Jogada *jog_atual = part_atual->jogadas_usuario;
        while (jog_atual != NULL) {
            Nodo_Jogada *jog_prox = jog_atual->prox;
            free(jog_atual);
            jog_atual = jog_prox;
        }
        
        jog_atual = part_atual->jogadas_computador;
        while (jog_atual != NULL) {
            Nodo_Jogada *jog_prox = jog_atual->prox;
            free(jog_atual);
            jog_atual = jog_prox;
        }
        
        free(part_atual);
        part_atual = part_prox;
    }
    *historico = NULL;
}


/* 7. PERSISTÊNCIA EM ARQUIVO E RANKING DECRESCENTE                           */

void salvar_partidas(Nodo_Partida **historico) {
    if (historico == NULL || *historico == NULL) {
        printf("\nNenhuma partida pendente de gravacao na sessao.\n");
        return;
    }

    FILE *arquivo = fopen("partidas_velha.txt", "a");
    if (arquivo == NULL) {
        printf("\nErro critico ao abrir o arquivo 'partidas_velha.txt' para gravacao!\n");
        return;
    }

    Nodo_Partida *aux_part = *historico;
    while (aux_part != NULL) {
        // Formato: ID;nome_usuario;jogadas_usuario;nome_pc;jogadas_pc;resultado
        fprintf(arquivo, "%d;%s;", aux_part->id_partida, aux_part->nome_usuario);

        Nodo_Jogada *aux_jog = aux_part->jogadas_usuario;
        while (aux_jog != NULL) {
            fprintf(arquivo, "%d-%d;", aux_jog->linha, aux_jog->coluna);
            aux_jog = aux_jog->prox;
        }

        fprintf(arquivo, "%s;", aux_part->nome_computador);

        aux_jog = aux_part->jogadas_computador;
        while (aux_jog != NULL) {
            fprintf(arquivo, "%d-%d;", aux_jog->linha, aux_jog->coluna);
            aux_jog = aux_jog->prox;
        }

        fprintf(arquivo, "%s\n", aux_part->resultado);
        aux_part = aux_part->prox;
    }

    fclose(arquivo);
    printf("\nPartidas persistidas com sucesso em 'partidas_velha.txt' (Modo Append).\n");

    // Libera a memória RAM das partidas que acabaram de ser salvas para não duplicar
    liberar_memoria(historico);
}

// Insere ou atualiza mantendo a lista encadeada ordenada de forma decrescente
void inserir_ranking_ordenado(Nodo_Ranking **lista, char nome[50]) {
    Nodo_Ranking *ant = NULL;
    Nodo_Ranking *atual = *lista;

    //  Procura se o jogador já existe na lista
    while (atual != NULL && strcmp(atual->nome, nome) != 0) {
        ant = atual;
        atual = atual->prox;
    }

    if (atual != NULL) {
        // Incrementa a vitória
        atual->vitorias++;

        // Remove temporariamente o nó para reposicioná-lo na ordem correta
        if (ant == NULL) {
            *lista = atual->prox;
        } else {
            ant->prox = atual->prox;
        }

        // Reinsere ordenado decrescente
        Nodo_Ranking *p_ant = NULL;
        Nodo_Ranking *p_atual = *lista;
        while (p_atual != NULL && p_atual->vitorias >= atual->vitorias) {
            p_ant = p_atual;
            p_atual = p_atual->prox;
        }
        if (p_ant == NULL) {
            atual->prox = *lista;
            *lista = atual;
        } else {
            atual->prox = p_atual;
            p_ant->prox = atual;
        }
    } else {
        // Novo jogador com 1 vitória
        Nodo_Ranking *novo = (Nodo_Ranking *) malloc(sizeof(Nodo_Ranking));
        strcpy(novo->nome, nome);
        novo->vitorias = 1;
        novo->prox = NULL;

        // Localiza posição de inserção (decrescente)
        Nodo_Ranking *p_ant = NULL;
        Nodo_Ranking *p_atual = *lista;
        while (p_atual != NULL && p_atual->vitorias >= novo->vitorias) {
            p_ant = p_atual;
            p_atual = p_atual->prox;
        }
        if (p_ant == NULL) {
            novo->prox = *lista;
            *lista = novo;
        } else {
            novo->prox = p_atual;
            p_ant->prox = novo;
        }
    }
}

void ranquear_usuarios() {
    FILE *arquivo = fopen("partidas_velha.txt", "r");
    if (arquivo == NULL) {
        printf("\nNenhum arquivo de historico encontrado ('partidas_velha.txt').\n");
        printf("Jogue algumas partidas e salve-as primeiro!\n");
        return;
    }

    Nodo_Ranking *lista_ranking = NULL;
    char linha[2048];

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        linha[strcspn(linha, "\r\n")] = '\0'; // Remove quebras de linha

        if (strlen(linha) == 0) continue;

        // Extrai tokens delimitados por ';'
        char *token = strtok(linha, ";");
        if (token == NULL) continue; // ID_partida

        token = strtok(NULL, ";");
        if (token == NULL) continue;
        char nome_usuario[50];
        strcpy(nome_usuario, token);

        // Percorre os tokens restantes até o último, que corresponde ao resultado
        char ultimo_campo[50] = "";
        while (token != NULL) {
            strcpy(ultimo_campo, token);
            token = strtok(NULL, ";");
        }

        // Se o vencedor for o usuário (e não Computador nem Empate), pontua no ranking
        if (strcmp(ultimo_campo, nome_usuario) == 0) {
            inserir_ranking_ordenado(&lista_ranking, nome_usuario);
        }
    }
    fclose(arquivo);

    printf("\n======================================================\n");
    printf("         RANKING DE VITORIAS \n");
    printf("======================================================\n");
    if (lista_ranking == NULL) {
        printf("Nenhuma vitoria de usuario registrada no arquivo.\n");
    } else {
        int posicao = 1;
        Nodo_Ranking *aux = lista_ranking;
        while (aux != NULL) {
            printf("%d Lugar -> Jogador: %-20s | Vitorias: %d\n", posicao++, aux->nome, aux->vitorias);
            Nodo_Ranking *temp = aux;
            aux = aux->prox;
            free(temp); // Limpeza da memória alocada para o ranking temporário
        }
    }
    printf("======================================================\n");
}
