# 🎮 Jogo da Velha em C — Listas Lineares Encadeadas & Persistência

> **Nota:** Este projeto foi desenvolvido exclusivamente para **fins de estudo e consolidação acadêmica**

---

## 📌 Visão Geral do Projeto

Aplicação de terminal desenvolvida em **Linguagem C** que implementa o clássico **Jogo da Velha (Tic-Tac-Toe)** em partidas ilimitadas de um usuário humano contra o computador. 

O foco central do projeto não reside apenas na mecânica do jogo, mas sim na aplicação prática de **Estruturas de Dados Dinâmicas**: gerenciamento manual de memória via ponteiros, listas lineares dinâmicas encadeadas para histórico de lances e partidas em memória RAM, e persistência em arquivos estruturados.

---

## 🚀 Principais Funcionalidades

- **Mecânica de Partidas Ilimitadas:**
  - Definição do primeiro movimento na partida inicial decidida por **Par ou Ímpar** (quem vence joga com `X`).
  - Alternância estrita da autoria da primeira jogada nas partidas seguintes.
  - Layout interativo do teclado mapeado via **Numpad (1 a 9)**.
- **Relatório e Apuração da Sessão:**
  - Histórico detalhado ao fechar o conjunto de partidas (ID, resultado e coordenadas das jogadas no formato `linha-coluna`).
  - Apuração em tempo real do **Vencedor Geral da Sessão** por saldo de vitórias.
- **Persistência de Dados em Disco:**
  - Gravação cumulativa (*modo append*) no arquivo `partidas_velha.txt`, estruturado com delimitador de ponto e vírgula (`;`).
  - Esvaziamento automático de memória após a gravação para prevenir duplicidade de registros.
- **Ranking Decrescente:**
  - Parser de leitura do arquivo texto para contagem de vitórias por usuário.
  - Inserção e reordenação dinâmica de nós para exibição estritamente em **ordem decrescente de pontuação**.
- **Gestão Eficiente de Memória:**
  - Alocação e desalocação dinâmica estrita (`malloc` e `free`), sem vazamento de memória (*zero memory leaks*).
  - Arquitetur modular.

---

## 🧠 Algoritmo da Máquina

Para a tomada de decisão do computador, foi utilizada a abordagem de **Seleção Aleatória Uniforme sobre Espaços Vazios (*Random Move / Rejection Sampling*)**:
- A máquina analisa o tabuleiro linearizado (`1D`), identifica as células que permanecem com valor nulo e sorteia o lance pseudoaleatório com equiprobabilidade por meio de `rand()` e `time(NULL)`.
- **Referência:** Base conceitual inspirada na implementação didática de [Rafael Stoffalette João](https://github.com/rafaelstojoao/jogo-da-velha-em-C).

---

## 🛠️ Modelagem de Dados

A arquitetura de dados na memória RAM apoia-se em nós encadeados:

```c
// Registro encadeado de cada coordenada jogada na partida
typedef struct Nodo_Jogada {
    int linha;
    int coluna;
    struct Nodo_Jogada *prox;
} Nodo_Jogada;

// Registro encadeado de cada partida concluída na sessão
typedef struct Nodo_Partida {
    int id_partida;
    char nome_usuario[50];
    char nome_computador[50];
    char resultado[50];
    Nodo_Jogada *jogadas_usuario;    
    Nodo_Jogada *jogadas_computador; 
    struct Nodo_Partida *prox;
} Nodo_Partida;

// Estrutura temporária para ordenação do ranking
typedef struct Nodo_Ranking {
    char nome[50];
    int vitorias;
    struct Nodo_Ranking *prox;
} Nodo_Ranking;
