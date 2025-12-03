#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

// Define os estados do jogo 
typedef enum
{
    MENU,
    INSTRUCOES,
    FASE_1,
    TRANSICAO_1,
    FASE_2,
    TRANSICAO_2,
    FASE_3,
    TRANSICAO_3,
    FASE_4,
    TRANSICAO_4,
    ESCOLHA,
    NARRADOR1_F1,
    FINAL_1,
    NARRADOR2_F1,
    FINAL_2,
    TELA_OBRIGADO,
    TELA_GAMEOVER
} EstadoJogo;

// Variaveis das magias
typedef struct
{
    float x, y;
    float velocidade_x;
    bool ativa;
    EstadoJogo tipo_magia;
    float largura;
    float altura;
} Magia;

#define MAXIMO_DE_MAGIAS_TOTAL 15

// Variaveis dos itens
typedef struct
{
    float x, y;
    bool ativo;
    ALLEGRO_BITMAP *sprite;
    float largura;
    float altura;
    int tipo_item; 
} Item;

#define MAX_ITENS 2

Item itens[MAX_ITENS];

int minions_para_derrotar = 0;
int minions_derrotados_atual = 0;
bool chefe_derrotado_atual = false;
bool tem_item_minion_atual = false;
bool tem_item_chefe_atual = false;

// Variaveis dos inimigos
typedef struct
{
    float x, y;
    ALLEGRO_BITMAP *sprite;

    float y_velocidade;
    float velocidade_x;
    bool no_chao;
    float forca_pulo;
    int timer_pulo;
    int frequencia_pulo;

    bool virado_para_direita; // Direção individual de cada inimigo
    float patrol_x_inicio;
    float patrol_x_fim;

    // Hitbox e Desenho
    float hitbox_offset_x;
    float hitbox_offset_y;
    float hitbox_largura;
    float hitbox_altura;
    float largura_desenho;
    float altura_desenho;

    int vida;

    bool ativo; // Diz de o inimigo está na tela, "vivo"
} Inimigo;

#define MAX_INIMIGOS 10

#define TEMPO_INVENCIBILIDADE (60 * 2)

// Inimigos da fase da floresta
#define FREQUENCIA_SPAWN_SLIME (60 * 1) 
#define TEMPO_SPAWN_CHEFE (60 * 20)     
#define TOTAL_SLIMES_FASE_2 25

int timer_spawn_slime = 0; 
int slimes_spawnados = 0;  
int timer_fase_2 = 0;

// Inimigos da fase da montanha
#define FREQUENCIA_SPAWN_GOLEM (60 * 0.8) 
#define TEMPO_SPAWN_CYCLOPE (60 * 13)     
#define TOTAL_GOLEMS_FASE_3 19

int timer_spawn_golem = 0;
int golems_spawnados = 0;
int timer_fase_3 = 0;

// Inimigos da fase do vulcão
#define FREQUENCIA_SPAWN_GOLEM_LAVA (60) 
#define TEMPO_SPAWN_MAXIMO_FASE_4 (60 * 15)  
#define TOTAL_GOLEMS_FASE_4 15

int timer_spawn_golem_lava = 0;
int golems_lava_spawnados = 0;
int timer_fase_4 = 0;
bool spawn_ativo_fase_4 = true;
bool boss_final_spawnado = false;

Inimigo inimigos[MAX_INIMIGOS];
ALLEGRO_BITMAP *sprite_slime_normal = NULL;
ALLEGRO_BITMAP *sprite_slime_bravo = NULL;
ALLEGRO_BITMAP *sprite_golem_gelo = NULL;
ALLEGRO_BITMAP *sprite_ice_cyclop = NULL;
ALLEGRO_BITMAP *sprite_golem_lava = NULL;
ALLEGRO_BITMAP *sprite_titan_lava = NULL;
ALLEGRO_BITMAP *sprite_item_minion = NULL;
ALLEGRO_BITMAP *sprite_item_chefe = NULL;

// Magia de Fogo (Fase 2)
#define MAX_MAGIAS_FOGO 5
#define VELOCIDADE_FOGO 12.0f

// Magia de Gelo (Fase 3)
#define MAX_MAGIAS_GELO 5
#define VELOCIDADE_GELO 8.0f

// Magia de raio (Fase 4)
#define MAX_MAGIAS_raio 3
#define VELOCIDADE_raio 15.0f

// Cooldown das magias
#define COOLDOWN_MAGIA 11.8f

typedef struct {
    float x, y;
    float velocidade_y;
    bool ativo;
} BolaDeFogo;

#define MAX_BOLAS_DE_FOGO 10 
BolaDeFogo bolas_de_fogo[MAX_BOLAS_DE_FOGO];

#define FREQUENCIA_BOLA_FOGO 50 
int timer_spawn_fogo = 0;

// Variaveis de dialogo
bool em_dialogo = false;
int indice_dialogo_atual = 0;
int total_dialogos = 0;
bool dialogo_final1_iniciado = false;

#define NUM_DIALOGOS_FASE1 6
#define NUM_DIALOGOS_FINAL1 12

// Define a altura e a largura da tela
#define LARGURA_JOGO 1536
#define ALTURA_JOGO 1024

// Variável Global para o fundo atual 
ALLEGRO_BITMAP *background_atual = NULL;

// Declaração das Funções 
void carregar_menu();
void descarregar_menu();
void carregar_instrucoes();
void descarregar_instrucoes();

void carregar_fase_1(float *jogador_x_ptr, float *jogador_y_ptr, bool *virado_dir_ptr, float *chao_y_ptr);
void descarregar_fase_1();
void carregar_fase_2(float *jogador_x_ptr, float *jogador_y_ptr, bool *virado_dir_ptr, float *chao_y_ptr, Inimigo array_inimigos[]);
void descarregar_fase_2();
void carregar_fase_3(float *jogador_x_ptr, float *jogador_y_ptr, bool *virado_dir_ptr, float *chao_y_ptr, Inimigo array_inimigos[]);
void descarregar_fase_3();
void carregar_fase_4(float *jogador_x_ptr, float *jogador_y_ptr, bool *virado_dir_ptr, float *chao_y_ptr, Inimigo array_inimigos[]);
void descarregar_fase_4();

void carregar_transicao_1();
void descarregar_transicao_1();
void carregar_transicao_2();
void descarregar_transicao_2();
void carregar_transicao_3();
void descarregar_transicao_3();
void carregar_transicao_4();
void descarregar_transicao_4();

void carregar_escolha();
void descarregar_escolha();

void carregar_narrador1_f1();
void descarregar_narrador1_f1();

void carregar_final_1(float *jogador_x_ptr, float *jogador_y_ptr, bool *virado_dir_ptr, float *chao_y_ptr);
void descarregar_final_1();

void carregar_narrador2_f1();
void descarregar_narrador2_f1();

void carregar_final_2();
void descarregar_final_2();
void carregar_tela_obrigado();
void descarregar_tela_obrigado();

void carregar_gameover();
void descarregar_gameover();

void limpar_inimigos(Inimigo array_inimigos[]);
void spawn_item(float x, float chao_y_atual, ALLEGRO_BITMAP *sprite, int tipo_item);
void limpar_itens(Item array_itens[]);

void limpar_bolas_de_fogo();

void spawn_inimigo(Inimigo array_inimigos[], ALLEGRO_BITMAP *sprite,
                   float x, float y, float y_velocidade, bool no_chao,
                   float vel_x, float forca_pulo, int freq_pulo,
                   float p_inicio, float p_fim,
                   float escala,
                   float h_offset_x, float h_offset_y, float h_ajuste_w, float h_ajuste_h,
                   int vida_inicial);

bool checa_colisao(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);

// Função para checar clique no botão 
bool mouse_dentro_da_area(float mouse_x, float mouse_y, float x1, float y1, float x2, float y2)
{
    return (mouse_x >= x1 && mouse_x <= x2 && mouse_y >= y1 && mouse_y <= y2);
}

int main()
{
    // Variáveis de representação 
    ALLEGRO_DISPLAY *janela = NULL;
    ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
    ALLEGRO_TIMER *timer = NULL;
    ALLEGRO_BITMAP *mestre_imagem = NULL;
    ALLEGRO_BITMAP *mestre_doente = NULL;
    ALLEGRO_BITMAP *magia_imagem = NULL;

    ALLEGRO_BITMAP *jogador_fase1 = NULL;
    ALLEGRO_BITMAP *jogador_fase2 = NULL;
    ALLEGRO_BITMAP *jogador_fase3 = NULL;
    ALLEGRO_BITMAP *jogador_fase4 = NULL;
    ALLEGRO_BITMAP *jogador_final1 = NULL;
    ALLEGRO_BITMAP *jogador_imagem_atual = NULL;
    ALLEGRO_BITMAP *coracao_img = NULL;

    ALLEGRO_BITMAP *magia_fogo_img = NULL;
    ALLEGRO_BITMAP *magia_gelo_img = NULL;
    ALLEGRO_BITMAP *magia_raio_img = NULL;
    ALLEGRO_BITMAP* bola_de_fogo = NULL;

    ALLEGRO_BITMAP *sprite_item_minion_f2 = NULL;
    ALLEGRO_BITMAP *sprite_item_chefe_f2 = NULL;
    ALLEGRO_BITMAP *sprite_item_minion_f3 = NULL;
    ALLEGRO_BITMAP *sprite_item_chefe_f3 = NULL;

    ALLEGRO_BITMAP *dialogo_fase1_imgs[NUM_DIALOGOS_FASE1];
    ALLEGRO_BITMAP *dialogo_final1_imgs[NUM_DIALOGOS_FINAL1];
    ALLEGRO_BITMAP *texto_p = NULL;
    ALLEGRO_BITMAP* avance = NULL;

    Magia magias[MAXIMO_DE_MAGIAS_TOTAL];

    // O jogo começa no estado menu
    EstadoJogo estado_atual = MENU;

    // Inicialização do Allegro
    al_init();
    al_init_primitives_addon();
    al_init_image_addon();
    al_install_keyboard();
    al_install_mouse();

    // Criação da Janela, Timer e Fila de Eventos 
    timer = al_create_timer(1.0 / 60.0);
    al_set_new_display_flags(ALLEGRO_RESIZABLE);
    janela = al_create_display(LARGURA_JOGO, ALTURA_JOGO);
    fila_eventos = al_create_event_queue();

    // Carrega imagens permanentes 
    jogador_fase1 = al_load_bitmap("imagens/p1.png");
    jogador_fase2 = al_load_bitmap("imagens/p2.png");
    jogador_fase3 = al_load_bitmap("imagens/p3.png");
    jogador_final1 = al_load_bitmap("imagens/p4.png");
    mestre_imagem = al_load_bitmap("imagens/Mestre1.png");
    mestre_doente = al_load_bitmap("imagens/mestre_doente.png");
    coracao_img = al_load_bitmap("imagens/Coracao-normal.png");

    sprite_slime_normal = al_load_bitmap("imagens/slime-normal.png");
    sprite_slime_bravo = al_load_bitmap("imagens/slime-bravo.png");
    sprite_golem_gelo = al_load_bitmap("imagens/golem-gelo.png");
    sprite_ice_cyclop = al_load_bitmap("imagens/ice-cyclope.png");
    sprite_golem_lava = al_load_bitmap("imagens/golem-lava.png");
    sprite_titan_lava = al_load_bitmap("imagens/titan-lava.png");

    sprite_item_minion_f3 = al_load_bitmap("imagens/enxofre.png");
    sprite_item_chefe_f3 = al_load_bitmap("imagens/ferro.png");
    sprite_item_minion_f2 = al_load_bitmap("imagens/nitrato-amonia.png");
    sprite_item_chefe_f2 = al_load_bitmap("imagens/cubo-gelo.png");

    magia_fogo_img = al_load_bitmap("imagens/Fogo.png");
    magia_gelo_img = al_load_bitmap("imagens/Gelo.png");
    magia_raio_img = al_load_bitmap("imagens/Raio.png");
    bola_de_fogo = al_load_bitmap("imagens/Meteoro.png");

    dialogo_fase1_imgs[0] = al_load_bitmap("imagens/f1_m1.png");
    dialogo_fase1_imgs[1] = al_load_bitmap("imagens/f1_m2.png");
    dialogo_fase1_imgs[2] = al_load_bitmap("imagens/f1_m3.png");
    dialogo_fase1_imgs[3] = al_load_bitmap("imagens/f1_g1.png");
    dialogo_fase1_imgs[4] = al_load_bitmap("imagens/f1_g2.png");
    dialogo_fase1_imgs[5] = al_load_bitmap("imagens/f1_g3.png");

    dialogo_final1_imgs[0] = al_load_bitmap("imagens/Dialogos_final1/f2_m1.png");
    dialogo_final1_imgs[1] = al_load_bitmap("imagens/Dialogos_final1/f2_m2.png");
    dialogo_final1_imgs[2] = al_load_bitmap("imagens/Dialogos_final1/f2_m3.png");
    dialogo_final1_imgs[3] = al_load_bitmap("imagens/Dialogos_final1/f2_g1.png");
    dialogo_final1_imgs[4] = al_load_bitmap("imagens/Dialogos_final1/f2_g2.png");
    dialogo_final1_imgs[5] = al_load_bitmap("imagens/Dialogos_final1/f2_g3.png");
    dialogo_final1_imgs[6] = al_load_bitmap("imagens/Dialogos_final1/f2_m4.png");
    dialogo_final1_imgs[7] = al_load_bitmap("imagens/Dialogos_final1/f2_m5.png");
    dialogo_final1_imgs[8] = al_load_bitmap("imagens/Dialogos_final1/f2_g4.png");
    dialogo_final1_imgs[9] = al_load_bitmap("imagens/Dialogos_final1/f2_g5.png");
    dialogo_final1_imgs[10] = al_load_bitmap("imagens/Dialogos_final1/f2_m6.png");
    dialogo_final1_imgs[11] = al_load_bitmap("imagens/Dialogos_final1/f2_m7.png");

    texto_p = al_load_bitmap("imagens/texto_p.png");
    avance = al_load_bitmap("imagens/avance.png");

    // Verificação de erro para todas as imagens
    if (!jogador_fase1 || !jogador_fase2 || !jogador_fase3 || !mestre_imagem || !magia_fogo_img || !magia_gelo_img || !magia_raio_img || !coracao_img || !bola_de_fogo ||
        !dialogo_fase1_imgs[0] || !dialogo_fase1_imgs[1] || !dialogo_fase1_imgs[2] || !dialogo_fase1_imgs[3] || !dialogo_fase1_imgs[4] || !dialogo_fase1_imgs[5] || !mestre_doente || !texto_p || !avance ||
        !sprite_slime_normal || !sprite_slime_bravo || !sprite_golem_gelo || !sprite_ice_cyclop || !sprite_golem_lava || !sprite_titan_lava)
    {
        printf("Erro ao carregar uma ou mais imagens permanentes!\n");
        return -1;
    }

    // Define a imagem inicial a ser usada
    jogador_imagem_atual = jogador_fase1;

    // Inicializa magias como inativas
    for (int i = 0; i < MAXIMO_DE_MAGIAS_TOTAL; i++)
    {
        magias[i].ativa = false;
    }

    // Inicializa inimigos como inativos
    for (int i = 0; i < MAX_INIMIGOS; i++)
    {
        inimigos[i].ativo = false;
    }

    for (int i = 0; i < MAX_BOLAS_DE_FOGO; i++) {
        bolas_de_fogo[i].ativo = false;
    }

    // Registra fontes de eventos 
    al_register_event_source(fila_eventos, al_get_display_event_source(janela));
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
    al_register_event_source(fila_eventos, al_get_mouse_event_source());

    // botões do menu
    float botao_iniciar_x1 = 580, botao_iniciar_y1 = 790, botao_iniciar_x2 = 925, botao_iniciar_y2 = 860;
    float botao_instrucoes_x1 = 520, botao_instrucoes_y1 = 820, botao_instrucoes_x2 = 1014, botao_instrucoes_y2 = 950;
    float botao_voltar_x1 = 618, botao_voltar_y1 = 800, botao_voltar_x2 = 918, botao_voltar_y2 = 880;

    // Botão de avançar nas transições
    float botao_avancar_x1 = 570, botao_avancar_y1 = 800, botao_avancar_x2 = 967, botao_avancar_y2 = 940;

    // Botões da tela de escolha
    float botao_final1_x1 = 350, botao_final1_y1 = 390, botao_final1_x2 = 1180, botao_final1_y2 = 564;
    float botao_final2_x1 = 350, botao_final2_y1 = 590, botao_final2_x2 = 1180, botao_final2_y2 = 750;

    // Botão da tela de obrigado
    float botao_para_obrigado_x1 = 592, botao_para_obrigado_y1 = 810, botao_para_obrigado_x2 = 950, botao_para_obrigado_y2 = 933;

    // Botão da tela de gameover
    float botao_reiniciar_x1 = 520, botao_reiniciar_y1 = 630, botao_reiniciar_x2 = 995, botao_reiniciar_y2 = 755;

    // --- Variáveis do Jogo ---
    float chao_y = 990;

    float jogador_x = 550, jogador_y = 800;
    float jogador_velocidade_y = 0;
    float velocidade_horizontal = 5.0;
    float gravidade = 0.5;
    float forca_pulo = 15.0;
    bool no_chao = false;
    bool tecla_a = false, tecla_d = false;
    bool virado_para_direita = false;

    int vida_jogador = 3;

    int timer_magia = 0;

    bool jogador_invencivel = false;
    int timer_invencibilidade = 0;

    // Carrega o menu
    carregar_menu();

    float jogador_largura = al_get_bitmap_width(jogador_imagem_atual);
    float jogador_altura = al_get_bitmap_height(jogador_imagem_atual);
    float jogador_hitbox_offset_x = 10;
    float jogador_hitbox_offset_y = 5;
    float jogador_hitbox_largura = jogador_largura - 40;
    float jogador_hitbox_altura = jogador_altura - 10;

    al_start_timer(timer);
    bool sair = false;
    bool redesenhar = true;

    while (!sair)
    {
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_TIMER)
        {

            if (timer_magia > 0)
            {
                timer_magia--;
            }

            if (timer_invencibilidade > 0)
            {
                timer_invencibilidade--;
                if (timer_invencibilidade == 0)
                {
                    jogador_invencivel = false;
                }
            }

            // A lógica de física e movimento só roda durante as fases
            if (((estado_atual >= FASE_1 && estado_atual <= FASE_4) || estado_atual == FINAL_1) && !em_dialogo)
            {
                // Movimento Horizontal
                if (tecla_a)
                {
                    jogador_x -= velocidade_horizontal;
                }
                if (tecla_d)
                {
                    jogador_x += velocidade_horizontal;
                }

                if (jogador_x < 0)
                {
                    jogador_x = 0;
                }

                // Gravidade e Pulo
                jogador_velocidade_y += gravidade;
                jogador_y += jogador_velocidade_y;
                no_chao = false;
                if (jogador_y + jogador_altura >= chao_y)
                {
                    jogador_y = chao_y - jogador_altura;
                    jogador_velocidade_y = 0;
                    no_chao = true;
                }

                // Movimento das magias (só na fase 2, mas a verificação já está no clique)
                for (int i = 0; i < MAXIMO_DE_MAGIAS_TOTAL; i++)
                {
                    if (magias[i].ativa)
                    {
                        magias[i].x += magias[i].velocidade_x;
                        if (magias[i].x < 0 || magias[i].x > 1536)
                        {
                            magias[i].ativa = false;
                        }
                    }
                }

                // Movimentação dos inimigos
                switch (estado_atual)
                {
                case FASE_2:
                    timer_fase_2++; // Incrementa o timer principal da fase

                    // Spawna o Slime Bravo uma única vez
                    if (timer_fase_2 == TEMPO_SPAWN_CHEFE)
                    {
                        printf("CHEFE SLIME BRAVO SPAWNOU!\n");

                        spawn_inimigo(inimigos, sprite_slime_bravo,
                                      1000, 500, 0, false,
                                      0, 9.0f, 150,
                                      0, 0,
                                      1.5f,
                                      15, 140, 30, 180, 15); // 4 Primeiros são a hitbox e o ultimo é a vida
                    }

                    // Spawna os Slimes Normais
                    if (slimes_spawnados < TOTAL_SLIMES_FASE_2)
                    {
                        timer_spawn_slime--;
                        if (timer_spawn_slime <= 0)
                        {
                            printf("Slime normal #%d spawnou!\n", slimes_spawnados + 1);

                            // Spawna o slime em um X aleatório (entre 500 e 1299)
                            float spawn_x = 500 + (rand() % 800);

                            spawn_inimigo(inimigos, sprite_slime_normal,
                                          spawn_x, 600, 10.0f, false,
                                          0, 15.0f, 65,
                                          0, 0,
                                          1.0f,
                                          5, 80, 10, 85, 3); // 4 Primeiros são a hitbox e o ultimo é a vida

                            slimes_spawnados++;                         // Incrementa a contagem
                            timer_spawn_slime = FREQUENCIA_SPAWN_SLIME; // Reseta o timer para o próximo
                        }
                    }

                    for (int i = 0; i < MAX_INIMIGOS; i++)
                    {
                        if (inimigos[i].ativo)
                        {
                            // Aplica gravidade
                            inimigos[i].y_velocidade += gravidade;
                            inimigos[i].y += inimigos[i].y_velocidade;
                            inimigos[i].x += inimigos[i].velocidade_x;

                            // Checa colisão com o chão
                            if (inimigos[i].y + inimigos[i].altura_desenho >= chao_y)
                            {
                                inimigos[i].y = chao_y - inimigos[i].altura_desenho;
                                inimigos[i].y_velocidade = 0;
                                inimigos[i].velocidade_x = 0; // Para de andar quando está no chão
                                inimigos[i].no_chao = true;
                            }

                            // Lógica de Pulo
                            if (inimigos[i].no_chao)
                            {
                                inimigos[i].timer_pulo--; // Diminui o contador
                                if (inimigos[i].timer_pulo <= 0)
                                {
                                    // Deu o tempo de pular!
                                    inimigos[i].y_velocidade = -inimigos[i].forca_pulo;
                                    if (jogador_x > inimigos[i].x)
                                    {
                                        inimigos[i].velocidade_x = (inimigos[i].frequencia_pulo == 65) ? 3.0f : 3.5f;
                                    }
                                    else
                                    {
                                        inimigos[i].velocidade_x = (inimigos[i].frequencia_pulo == 85) ? -3.0f : -4.5f;
                                    }
                                    inimigos[i].no_chao = false;
                                    inimigos[i].timer_pulo = inimigos[i].frequencia_pulo;
                                }
                            }
                        }
                    }
                    break;

                case FASE_3:

                    timer_fase_3++; // Incrementa o timer principal da fase 3

                    // Spawna o Cyclope
                    if (timer_fase_3 == TEMPO_SPAWN_CYCLOPE)
                    {
                        printf("CHEFE CYCLOPE SPAWNOU!\n");
                        // Spawna o Cyclope de Gelo
                        spawn_inimigo(inimigos, sprite_ice_cyclop,
                                      1200, 450.0f, 0, true,
                                      -1.5f, 0, 0,
                                      0, 1536,
                                      1.0f,
                                      95, 5, 150, 30, 16); // 4 Primeiros são a hitbox e o ultimo é a vida
                    }

                    // Spawna os Golems
                    if (golems_spawnados < TOTAL_GOLEMS_FASE_3)
                    {
                        timer_spawn_golem--;
                        if (timer_spawn_golem <= 0)
                        {
                            printf("Golem de Gelo #%d spawnou!\n", golems_spawnados + 1);

                            float spawn_x = 400 + (rand() % 800); // Spawna em área aleatória

                            spawn_inimigo(inimigos, sprite_golem_gelo,
                                          spawn_x, 100.0f, 0, true,
                                          -1.0f, 0, 0,
                                          0, 1536,
                                          1.0f,
                                          5, 100, 10, 105, 3); // 4 Primeiros são a hitbox e o ultimo é a vida

                            golems_spawnados++;
                            timer_spawn_golem = FREQUENCIA_SPAWN_GOLEM; // Reseta o timer
                        }
                    }

                    for (int i = 0; i < MAX_INIMIGOS; i++)
                    {
                        if (inimigos[i].ativo)
                        {
                            // Aplica gravidade
                            inimigos[i].y_velocidade += gravidade;
                            inimigos[i].y += inimigos[i].y_velocidade;

                            // Checa colisão com o chão
                            if (inimigos[i].y + inimigos[i].altura_desenho >= chao_y)
                            {
                                inimigos[i].y = chao_y - inimigos[i].altura_desenho;
                                inimigos[i].y_velocidade = 0;
                                inimigos[i].no_chao = true;
                            }

                            // Lógica de Patrulha
                            if (inimigos[i].no_chao)
                            {
                                // Aplica o movimento horizontal
                                inimigos[i].x += inimigos[i].velocidade_x;

                                // Verifica se atingiu os limites da patrulha e inverte
                                if (inimigos[i].x <= inimigos[i].patrol_x_inicio)
                                {
                                    inimigos[i].x = inimigos[i].patrol_x_inicio;
                                    inimigos[i].velocidade_x *= -1;
                                    inimigos[i].virado_para_direita = true;
                                }
                                else if (inimigos[i].x + inimigos[i].largura_desenho >= inimigos[i].patrol_x_fim)
                                {
                                    inimigos[i].x = inimigos[i].patrol_x_fim - inimigos[i].largura_desenho;
                                    inimigos[i].velocidade_x *= -1;
                                    inimigos[i].virado_para_direita = false;
                                }
                            }
                        }
                    }
                    break;

                case FASE_4:
                    timer_fase_4++; 

                    // Verifica se ainda pode spawnar 
                    if (timer_fase_4 >= TEMPO_SPAWN_MAXIMO_FASE_4)
                    {
                        spawn_ativo_fase_4 = false; // Para de spawnar após 15 segundos
                    }

                    // Spawna os Golems de Lava 
                    if (spawn_ativo_fase_4 && golems_lava_spawnados < TOTAL_GOLEMS_FASE_4)
                    {
                        timer_spawn_golem_lava--;
                        if (timer_spawn_golem_lava <= 0)
                        {
                            printf("Golem de Lava #%d spawnou!\n", golems_lava_spawnados + 1);

                            float spawn_x = 400 + (rand() % 800); 

                            spawn_inimigo(inimigos, sprite_golem_lava,
                                          spawn_x, 100.0f, 0, true,
                                          -1.0f, 0, 0,
                                          0, 1536,
                                          1.0f,
                                          75, 60, 140, 155, 3); // 4 Primeiros são a hitbox e o ultimo é a vida

                            golems_lava_spawnados++;
                            timer_spawn_golem_lava = FREQUENCIA_SPAWN_GOLEM_LAVA; // Reseta o timer
                        }
                    }

                    // Verifica se deve spawnar o boss final
                    // Condições: spawn desativado (passou 15s) + todos os inimigos mortos + boss ainda não spawnado
                    if (!spawn_ativo_fase_4 && !boss_final_spawnado)
                    {
                        // Conta quantos inimigos ainda estão vivos
                        int inimigos_vivos = 0;
                        for (int i = 0; i < MAX_INIMIGOS; i++)
                        {
                            if (inimigos[i].ativo)
                            {
                                inimigos_vivos++;
                            }
                        }

                        // Se não há mais inimigos vivos, spawna o boss
                        if (inimigos_vivos == 0)
                        {
                            printf("BOSS FINAL TITAN DE LAVA SPAWNOU!\n");

                            spawn_inimigo(inimigos, sprite_titan_lava,
                                          700, (chao_y - al_get_bitmap_height(sprite_titan_lava)) + 200, 0, true,
                                          -1.5f, 0, 0,
                                          0, 1536,
                                          1.0f,
                                          10, 5, 20, 170, 28); // 4 Primeiros são a hitbox e o ultimo é a vida

                            boss_final_spawnado = true;
                        }
                    }

                    // Timer da bola de fogo
                    timer_spawn_fogo--;
                    if (timer_spawn_fogo <= 0) {
                        for (int i = 0; i < MAX_BOLAS_DE_FOGO; i++) {
                            if (!bolas_de_fogo[i].ativo) {
                                bolas_de_fogo[i].ativo = true;
                                bolas_de_fogo[i].x = rand() % LARGURA_JOGO; // Posição X aleatória
                                bolas_de_fogo[i].y = 0; 
                                bolas_de_fogo[i].velocidade_y = 8.5f; // Velocidade da queda 
                                timer_spawn_fogo = FREQUENCIA_BOLA_FOGO;
                                break;
                            }
                        }
                    }

                    // Movimento das Bolas de Fogo
                    for (int i = 0; i < MAX_BOLAS_DE_FOGO; i++) {
                        if (bolas_de_fogo[i].ativo) {
                            bolas_de_fogo[i].y += bolas_de_fogo[i].velocidade_y;
                            // Desativa se sair da tela 
                            if (bolas_de_fogo[i].y > ALTURA_JOGO) {
                                bolas_de_fogo[i].ativo = false;
                            }
                        }
                    }

                    // Movimento dos inimigos 
                    for (int i = 0; i < MAX_INIMIGOS; i++)
                    {
                        if (inimigos[i].ativo)
                        {
                            // Aplica gravidade
                            inimigos[i].y_velocidade += gravidade;
                            inimigos[i].y += inimigos[i].y_velocidade;

                            // Checa colisão com o chão
                            if (inimigos[i].y + inimigos[i].altura_desenho >= chao_y)
                            {
                                inimigos[i].y = chao_y - inimigos[i].altura_desenho;
                                inimigos[i].y_velocidade = 0;
                                inimigos[i].no_chao = true;
                            }

                            // Lógica de Patrulha
                            if (inimigos[i].no_chao)
                            {
                                // Aplica o movimento horizontal
                                inimigos[i].x += inimigos[i].velocidade_x;

                                // Verifica se atingiu os limites da patrulha e inverte
                                if (inimigos[i].x <= inimigos[i].patrol_x_inicio)
                                {
                                    inimigos[i].x = inimigos[i].patrol_x_inicio;
                                    inimigos[i].velocidade_x *= -1;
                                    inimigos[i].virado_para_direita = true;
                                }
                                else if (inimigos[i].x + inimigos[i].largura_desenho >= inimigos[i].patrol_x_fim)
                                {
                                    inimigos[i].x = inimigos[i].patrol_x_fim - inimigos[i].largura_desenho;
                                    inimigos[i].velocidade_x *= -1;
                                    inimigos[i].virado_para_direita = false;
                                }
                            }
                        }
                    }
                    break;

                case FINAL_1:
                    // Verifica se o diálogo já começou. Se não, checa a posição.
                    if (!dialogo_final1_iniciado)
                    {
                        // Posição X que ativa o diálogo
                        float ponto_x_gatilho = 860;

                        if (jogador_x <= ponto_x_gatilho)
                        {
                            em_dialogo = true; // Ativa o modo diálogo
                            dialogo_final1_iniciado = true;
                        }
                    }
                    break;
                }

                // Colisão entre as magias e os inimigos
                for (int i = 0; i < MAXIMO_DE_MAGIAS_TOTAL; i++)
                {
                    // Se a magia atual estiver ativa percorre todos os inimigos
                    if (magias[i].ativa)
                    {

                        // ...percorre todos os inimigos
                        for (int j = 0; j < MAX_INIMIGOS; j++)
                        {
                            // Se o inimigo atual estiver ativo calcula as coordenadas reais das hitboxes na tela
                            if (inimigos[j].ativo)
                            {

                                // Calcula as coordenadas reais das hitboxes na tela
                                float magia_hitbox_x = magias[i].x;
                                float magia_hitbox_y = magias[i].y;

                                float inimigo_hitbox_x = inimigos[j].x + inimigos[j].hitbox_offset_x;
                                float inimigo_hitbox_y = inimigos[j].y + inimigos[j].hitbox_offset_y;

                                // Usa a função para checar se as hitboxes se tocam
                                if (checa_colisao(magia_hitbox_x, magia_hitbox_y, magias[i].largura, magias[i].altura,
                                                  inimigo_hitbox_x, inimigo_hitbox_y, inimigos[j].hitbox_largura, inimigos[j].hitbox_altura))
                                {
                                    // --- Colisão ---
                                    magias[i].ativa = false;
                                    inimigos[j].vida--;
                                    inimigos[j].y_velocidade = -2.0f;
                                    inimigos[j].no_chao = false;

                                    // Checa se o inimigo morreu
                                    if (inimigos[j].vida <= 0)
                                    {
                                        inimigos[j].ativo = false;
                                        printf("INIMIGO DERROTADO!\n");

                                        // --- Lógica de drop ---
                                        switch (estado_atual)
                                        {
                                        case FASE_2:
                                            if (inimigos[j].sprite == sprite_slime_bravo)
                                            {
                                                if (!chefe_derrotado_atual)
                                                {
                                                    chefe_derrotado_atual = true;
                                                    spawn_item(inimigos[j].x, chao_y, sprite_item_chefe_f2, 2);
                                                    printf("Dropou item do Chefe da Fase 2!\n");
                                                }
                                            }
                                            else if (inimigos[j].sprite == sprite_slime_normal)
                                            {
                                                minions_derrotados_atual++;
                                                if (minions_derrotados_atual == TOTAL_SLIMES_FASE_2)
                                                {
                                                    spawn_item(inimigos[j].x, chao_y, sprite_item_minion_f2, 1);
                                                    printf("Dropou item dos Minions da Fase 2!\n");
                                                }
                                            }
                                            break;

                                        case FASE_3:
                                            if (inimigos[j].sprite == sprite_ice_cyclop)
                                            {
                                                if (!chefe_derrotado_atual)
                                                {
                                                    chefe_derrotado_atual = true;
                                                    spawn_item(inimigos[j].x, chao_y, sprite_item_chefe_f3, 2);
                                                    printf("Dropou item do Chefe da Fase 3!\n");
                                                }
                                            }
                                            else if (inimigos[j].sprite == sprite_golem_gelo)
                                            {
                                                minions_derrotados_atual++;
                                                if (minions_derrotados_atual == TOTAL_GOLEMS_FASE_3)
                                                {
                                                    spawn_item(inimigos[j].x, chao_y, sprite_item_minion_f3, 1);
                                                    printf("Dropou item dos Minions da Fase 3!\n");
                                                }
                                            }
                                            break;

                                        case FASE_4:

                                            if (inimigos[j].sprite == sprite_titan_lava)
                                            {
                                                printf("BOSS FINAL DERROTADO! Fase 4 completa!\n");
                                            }
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        printf("Inimigo atingido! Vida restante: %d\n", inimigos[j].vida);
                                    }
                                }
                            }
                        }
                    }
                }

                // Colisão Jogador vs Inimigos 
                float jogador_hitbox_x_real = jogador_x + jogador_hitbox_offset_x;
                float jogador_hitbox_y_real = jogador_y + jogador_hitbox_offset_y;

                for (int i = 0; i < MAX_INIMIGOS; i++)
                {
                    if (inimigos[i].ativo)
                    {
                        float inimigo_hitbox_x_real = inimigos[i].x + inimigos[i].hitbox_offset_x;
                        float inimigo_hitbox_y_real = inimigos[i].y + inimigos[i].hitbox_offset_y;

                        // Verifica a colisão e se o jogador NÃO está invencível
                        if (checa_colisao(jogador_hitbox_x_real, jogador_hitbox_y_real, jogador_hitbox_largura, jogador_hitbox_altura,
                                          inimigo_hitbox_x_real, inimigo_hitbox_y_real, inimigos[i].hitbox_largura, inimigos[i].hitbox_altura) &&
                            !jogador_invencivel)
                        {

                            // Aplica o dano e ativa a invencibilidade
                            vida_jogador--;
                            jogador_invencivel = true;
                            timer_invencibilidade = TEMPO_INVENCIBILIDADE; // Ativa o timer
                            printf("JOGADOR ATINGIDO! Vidas restantes: %d\n", vida_jogador);

                            // Knockback - O inimigo não desaparece, é jogado para trás
                            inimigos[i].y_velocidade = -5.0f; // Joga o inimigo para cima
                            inimigos[i].no_chao = false;

                            // Empurra o inimigo para longe do jogador
                            if (inimigos[i].x < jogador_x)
                            {
                                inimigos[i].velocidade_x = -5.0f; // Empurra para esquerda
                            }
                            else
                            {
                                inimigos[i].velocidade_x = 5.0f; // Empurra para direita
                            }

                            // Checa o Game Over
                            if (vida_jogador <= 0)
                            {
                                printf("GAME OVER!\n");
                                estado_atual = TELA_GAMEOVER;
                                carregar_gameover();
                                limpar_inimigos(inimigos);
                            }
                        }
                    }
                }

                // --- Colisão Jogador vs Bolas de Fogo ---
                if (estado_atual == FASE_4 && !jogador_invencivel) {
                    for (int i = 0; i < MAX_BOLAS_DE_FOGO; i++) {
                        if (bolas_de_fogo[i].ativo) {
                            // Define a hitbox da bola de fogo 
                            float bola_largura = al_get_bitmap_width(bola_de_fogo);
                            float bola_altura = al_get_bitmap_height(bola_de_fogo);
                                if (checa_colisao(jogador_hitbox_x_real, jogador_hitbox_y_real, jogador_hitbox_largura, jogador_hitbox_altura,
                                    bolas_de_fogo[i].x, bolas_de_fogo[i].y, bola_largura, bola_altura)){
                                    // Ação de dano 
                                    vida_jogador--;
                                    jogador_invencivel = true;
                                    timer_invencibilidade = TEMPO_INVENCIBILIDADE;
                                    printf("JOGADOR ATINGIDO! Vidas restantes: %d\n", vida_jogador);

                                    bolas_de_fogo[i].ativo = false; // Bola desaparece ao atingir

                                    // Checa o Game Over
                                    if (vida_jogador <= 0) {
                                        printf("GAME OVER!\n");
                                        estado_atual = TELA_GAMEOVER;
                                        carregar_gameover();
                                        limpar_inimigos(inimigos);
                                    }
                                }
                        }
                    }
                }

                // --- Colisão Jogador vs Itens ---
                for (int i = 0; i < MAX_ITENS; i++)
                {
                    // Se o item está ativo no chão verifica a colisão entre a hitbox do jogador e a hitbox do item
                    if (itens[i].ativo)
                    {

                        // Verifica a colisão entre a hitbox do jogador e a hitbox do item
                        if (checa_colisao(jogador_hitbox_x_real, jogador_hitbox_y_real, jogador_hitbox_largura, jogador_hitbox_altura,
                                          itens[i].x, itens[i].y, itens[i].largura, itens[i].altura))
                        {
                            // --- Lógica de coletar os itens ---
                            itens[i].ativo = false; // Desativa o item/pega ele

                            if (itens[i].tipo_item == 1)
                            {
                                tem_item_minion_atual = true;
                                printf("Item de MINION coletado!\n");
                            }
                            else if (itens[i].tipo_item == 2)
                            {
                                tem_item_chefe_atual = true;
                                printf("Item de CHEFE coletado!\n");
                            }
                        }
                    }
                }

                // Gatilho de mudança de fase
                if (jogador_x > 1536)
                {
                    switch (estado_atual)
                    {
                    case FASE_1:
                        descarregar_fase_1();
                        estado_atual = TRANSICAO_1;
                        limpar_inimigos(inimigos);
                        carregar_transicao_1();
                        break;

                    case FASE_2:
                        if (tem_item_minion_atual && tem_item_chefe_atual)
                        {
                            descarregar_fase_2();
                            estado_atual = TRANSICAO_2;
                            limpar_inimigos(inimigos);
                            limpar_itens(itens);
                            carregar_transicao_2();
                        }
                        else
                        {
                            jogador_x = 1536 - 1;
                        }
                        break;

                    case FASE_3:
                        if (tem_item_minion_atual && tem_item_chefe_atual)
                        {
                            descarregar_fase_3();
                            estado_atual = TRANSICAO_3;
                            limpar_inimigos(inimigos);
                            limpar_itens(itens);
                            carregar_transicao_3();
                        }
                        else
                        {
                            jogador_x = 1536 - 1;
                        }
                        break;
                    case FASE_4:
                        // Na fase 4, só pode avançar se o boss final foi derrotado
                        if (boss_final_spawnado)
                        {
                            // Verifica se o boss ainda está vivo
                            bool boss_vivo = false;
                            for (int i = 0; i < MAX_INIMIGOS; i++)
                            {
                                if (inimigos[i].ativo && inimigos[i].sprite == sprite_titan_lava)
                                {
                                    boss_vivo = true;
                                    break;
                                }
                            }

                            // Se o boss não está mais vivo, permite passar
                            if (!boss_vivo)
                            {
                                descarregar_fase_4();
                                estado_atual = TRANSICAO_4;
                                limpar_inimigos(inimigos);
                                carregar_transicao_4();
                            }
                            else
                            {
                                jogador_x = 1536 - 1; // Não deixa sair da tela
                            }
                        }
                        else
                        {
                            jogador_x = 1536 - 1; // Não deixa sair enquanto o boss não spawnou
                        }
                        break;
                    }
                }
            }
            redesenhar = true;
        }
        else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            sair = true;
        }

        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            // Teclado só funciona durante as fases
            if (((estado_atual >= FASE_1 && estado_atual <= FASE_4) || estado_atual == FINAL_1) && !em_dialogo)
            {

                switch (evento.keyboard.keycode)
                {
                case ALLEGRO_KEY_A:
                    tecla_a = true;
                    virado_para_direita = false;
                    break;
                case ALLEGRO_KEY_D:
                    tecla_d = true;
                    virado_para_direita = true;
                    break;
                case ALLEGRO_KEY_W:
                    if (no_chao)
                    {
                        jogador_velocidade_y = -forca_pulo;
                    }
                    break;
                }
            }

            // Usa o p para avançar os dialogos
            if (evento.keyboard.keycode == ALLEGRO_KEY_P && em_dialogo)
            {
                // Adiciona a checagem para FINAL_1
                if (estado_atual == FASE_1 || estado_atual == FINAL_1)
                { 
                    indice_dialogo_atual++;
                    if (indice_dialogo_atual >= total_dialogos)
                    {
                        em_dialogo = false;
                        indice_dialogo_atual = 0;
                        if (estado_atual == FINAL_1)
                        {
                            descarregar_final_1();
                            estado_atual = NARRADOR2_F1;
                            carregar_narrador2_f1();
                        }
                    }
                }
            }

            // Usa o p para avancar nas telas de narrador
            else if (evento.keyboard.keycode == ALLEGRO_KEY_P && !em_dialogo)
            {

                if (estado_atual == NARRADOR1_F1)
                {
                    descarregar_narrador1_f1();
                    estado_atual = FINAL_1;
                    jogador_imagem_atual = jogador_final1;

                    carregar_final_1(&jogador_x, &jogador_y, &virado_para_direita, &chao_y);

                    jogador_largura = al_get_bitmap_width(jogador_imagem_atual);
                    jogador_altura = al_get_bitmap_height(jogador_imagem_atual);
                    jogador_y = chao_y - jogador_altura;
                }
 
                else if (estado_atual == NARRADOR2_F1)
                {
                    descarregar_narrador2_f1();
                    estado_atual = TELA_OBRIGADO;
                    carregar_tela_obrigado();
                }
            }

            // ESC para sair de qualquer tela
            if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
            {
                sair = true;
            }
        }
        else if (evento.type == ALLEGRO_EVENT_KEY_UP)
        {
            switch (evento.keyboard.keycode)
            {
            case ALLEGRO_KEY_A:
                tecla_a = false;
                break;
            case ALLEGRO_KEY_D:
                tecla_d = false;
                break;
            }
        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
        {

            ALLEGRO_TRANSFORM transform_atual;
            al_copy_transform(&transform_atual, al_get_current_transform());
            al_invert_transform(&transform_atual);

            float mouseX_transformado = evento.mouse.x;
            float mouseY_transformado = evento.mouse.y;
            al_transform_coordinates(&transform_atual, &mouseX_transformado, &mouseY_transformado);

            switch (estado_atual)
            {
            case MENU:

                vida_jogador = 3; // Reseta a vida 
                jogador_invencivel = false; 
                timer_invencibilidade = 0;

                if (mouse_dentro_da_area(mouseX_transformado, mouseY_transformado, botao_iniciar_x1, botao_iniciar_y1, botao_iniciar_x2, botao_iniciar_y2))
                {
                    descarregar_menu();
                    estado_atual = FASE_1;
                    jogador_imagem_atual = jogador_fase1; // Garante que o sprite inicial seja usado
                    carregar_fase_1(&jogador_x, &jogador_y, &virado_para_direita, &chao_y);

                    jogador_largura = al_get_bitmap_width(jogador_imagem_atual);
                    jogador_altura = al_get_bitmap_height(jogador_imagem_atual);
                    // Coloca o jogador exatamente no chão
                    jogador_y = chao_y - jogador_altura;

                    // Atualiza a hitbox do jogador para o novo tamanho
                    jogador_hitbox_largura = jogador_largura - 40;
                    jogador_hitbox_altura = jogador_altura - 10;
                }
                else if (mouse_dentro_da_area(mouseX_transformado, mouseY_transformado, botao_instrucoes_x1, botao_instrucoes_y1, botao_instrucoes_x2, botao_instrucoes_y2))
                {
                    descarregar_menu();
                    estado_atual = INSTRUCOES;
                    carregar_instrucoes();
                }
                break;

            case TRANSICAO_1:
                if (mouse_dentro_da_area(mouseX_transformado, mouseY_transformado, botao_avancar_x1, botao_avancar_y1, botao_avancar_x2, botao_avancar_y2))
                {
                    descarregar_transicao_1();
                    estado_atual = FASE_2;
                    jogador_imagem_atual = jogador_fase1;
                    limpar_inimigos(inimigos);
                    carregar_fase_2(&jogador_x, &jogador_y, &virado_para_direita, &chao_y, inimigos);
                }
                break;
            case TRANSICAO_2:
                if (mouse_dentro_da_area(mouseX_transformado, mouseY_transformado, botao_avancar_x1, botao_avancar_y1, botao_avancar_x2, botao_avancar_y2))
                {
                    descarregar_transicao_2();
                    estado_atual = FASE_3;
                    jogador_imagem_atual = jogador_fase2;
                    limpar_inimigos(inimigos);
                    carregar_fase_3(&jogador_x, &jogador_y, &virado_para_direita, &chao_y, inimigos);
                }
                break;
            case TRANSICAO_3:
                if (mouse_dentro_da_area(mouseX_transformado, mouseY_transformado, botao_avancar_x1, botao_avancar_y1, botao_avancar_x2, botao_avancar_y2))
                {
                    descarregar_transicao_3();
                    estado_atual = FASE_4;
                    jogador_imagem_atual = jogador_fase3;
                    limpar_inimigos(inimigos);
                    carregar_fase_4(&jogador_x, &jogador_y, &virado_para_direita, &chao_y, inimigos);
                }
                break;
            case TRANSICAO_4:
                if (mouse_dentro_da_area(mouseX_transformado, mouseY_transformado, botao_avancar_x1, botao_avancar_y1, botao_avancar_x2, botao_avancar_y2))
                {
                    descarregar_transicao_4();
                    estado_atual = ESCOLHA;
                    limpar_inimigos(inimigos);
                    carregar_escolha();
                }
                break;

            case ESCOLHA:
                if (mouse_dentro_da_area(mouseX_transformado, mouseY_transformado, botao_final1_x1, botao_final1_y1, botao_final1_x2, botao_final1_y2))
                {
                    printf("Botao para o FINAL 1 foi clicado!\n");
                    descarregar_escolha();
                    estado_atual = NARRADOR1_F1;
                    carregar_narrador1_f1();
                }
                else if (mouse_dentro_da_area(mouseX_transformado, mouseY_transformado, botao_final2_x1, botao_final2_y1, botao_final2_x2, botao_final2_y2))
                {
                    printf("Botao para o FINAL 2 foi clicado!\n");
                    descarregar_escolha();
                    estado_atual = FINAL_2;
                    carregar_final_2();
                }
                break;

            case FINAL_2:
                if (mouse_dentro_da_area(mouseX_transformado, mouseY_transformado, botao_reiniciar_x1, botao_reiniciar_y1, botao_reiniciar_x2, botao_reiniciar_y2))
                {
                    descarregar_final_2();
                    estado_atual = TELA_OBRIGADO;
                    carregar_tela_obrigado();
                }
                break;

            case TELA_OBRIGADO:
                descarregar_tela_obrigado();
                estado_atual = MENU;
                carregar_menu();
                break;

            case TELA_GAMEOVER:
                if (mouse_dentro_da_area(mouseX_transformado, mouseY_transformado, botao_reiniciar_x1, botao_reiniciar_y1, botao_reiniciar_x2, botao_reiniciar_y2))
                {
                    descarregar_gameover();
                    estado_atual = MENU;
                    carregar_menu();

                    // Reseta a vida do jogador
                    vida_jogador = 3;
                }
                break;

                // --- LÓGICA DAS INSTRUÇÕES ---
            case INSTRUCOES:
                if (mouse_dentro_da_area(mouseX_transformado, mouseY_transformado, botao_voltar_x1, botao_voltar_y1, botao_voltar_x2, botao_voltar_y2))
                {
                    descarregar_instrucoes();
                    estado_atual = MENU;
                    carregar_menu();
                }
                break;

                // --- Lógica de magia por fase ---
            case FASE_2: { // Magia de Fogo
                if (timer_magia <= 0) { // Verifica o cooldown
                    int contador_magias_ativas = 0;

                    for (int i = 0; i < MAXIMO_DE_MAGIAS_TOTAL; i++) {
                        if (magias[i].ativa && magias[i].tipo_magia == FASE_2) {
                            contador_magias_ativas++;
                        }
                    }

                    if (contador_magias_ativas < MAX_MAGIAS_FOGO) {
                        for (int i = 0; i < MAXIMO_DE_MAGIAS_TOTAL; i++) {
                            if (!magias[i].ativa) {
                                magias[i].ativa = true;
                                magias[i].tipo_magia = FASE_2;
                                timer_magia = COOLDOWN_MAGIA; // Reseta o timer
                                magias[i].largura = al_get_bitmap_width(magia_fogo_img);
                                magias[i].altura = al_get_bitmap_height(magia_fogo_img);
                                magias[i].y = jogador_y + jogador_altura / 2;
                                if (virado_para_direita) {
                                    magias[i].x = jogador_x + jogador_largura;
                                    magias[i].velocidade_x = VELOCIDADE_FOGO;
                                }
                                else {
                                    magias[i].x = jogador_x;
                                    magias[i].velocidade_x = -VELOCIDADE_FOGO;
                                }
                                break;
                            }
                        }
                    }
                }
                break;
            }

            case FASE_3: { // Magia de Gelo
                if (timer_magia <= 0) { // Verifica o cooldown
                    int contador_magias_ativas = 0;
                    for (int i = 0; i < MAXIMO_DE_MAGIAS_TOTAL; i++) {
                        if (magias[i].ativa && magias[i].tipo_magia == FASE_3) {
                            contador_magias_ativas++;
                        }
                    }

                    if (contador_magias_ativas < MAX_MAGIAS_GELO) {
                        for (int i = 0; i < MAXIMO_DE_MAGIAS_TOTAL; i++) {
                            if (!magias[i].ativa) {
                                magias[i].ativa = true;
                                magias[i].tipo_magia = FASE_3;
                                timer_magia = COOLDOWN_MAGIA;
                                magias[i].largura = al_get_bitmap_width(magia_gelo_img);
                                magias[i].altura = al_get_bitmap_height(magia_gelo_img);
                                magias[i].y = jogador_y + jogador_altura / 2;
                                if (virado_para_direita) {
                                    magias[i].x = jogador_x + jogador_largura;
                                    magias[i].velocidade_x = VELOCIDADE_GELO;
                                }
                                else {
                                    magias[i].x = jogador_x;
                                    magias[i].velocidade_x = -VELOCIDADE_GELO;
                                }
                                break;

                            }
                        }
                    }
                }
                break;
            }

            case FASE_4: { // Raio
                if (timer_magia <= 0) { // Verifica o cooldown 
                    int contador_magias_ativas = 0;
                    for (int i = 0; i < MAXIMO_DE_MAGIAS_TOTAL; i++) {
                        if (magias[i].ativa && magias[i].tipo_magia == FASE_4) {
                            contador_magias_ativas++;
                        }
                    }

                    if (contador_magias_ativas < MAX_MAGIAS_raio) {
                        for (int i = 0; i < MAXIMO_DE_MAGIAS_TOTAL; i++) {
                            if (!magias[i].ativa) {
                                magias[i].ativa = true;
                                magias[i].tipo_magia = FASE_4;
                                timer_magia = COOLDOWN_MAGIA; // Reseta o timer
                                magias[i].largura = al_get_bitmap_width(magia_raio_img);
                                magias[i].altura = al_get_bitmap_height(magia_raio_img);
                                magias[i].y = jogador_y + jogador_altura / 2;
                                if (virado_para_direita) {

                                    magias[i].x = jogador_x + jogador_largura;
                                    magias[i].velocidade_x = VELOCIDADE_raio;
                                }
                                else {
                                    magias[i].x = jogador_x;
                                    magias[i].velocidade_x = -VELOCIDADE_raio; 
                                }
                                break;
                            }
                        }
                    }
                }
                break;
                }
            }
        }

        // Desenho de tudo
        if (redesenhar && al_is_event_queue_empty(fila_eventos))
        {
            redesenhar = false;

            // Limpa a tela inteira com preto.
            al_clear_to_color(al_map_rgb(0, 0, 0));

            // Lógica de desenho 
            switch (estado_atual)
            {
            // Telas Estaticas 
            case MENU:
            case INSTRUCOES:
            case TRANSICAO_1:
            case TRANSICAO_2:
            case TRANSICAO_3:
            case TRANSICAO_4:
            case ESCOLHA:
            case NARRADOR1_F1:
            case NARRADOR2_F1:
            case FINAL_2:
            case TELA_OBRIGADO:
            case TELA_GAMEOVER:
                al_draw_bitmap(background_atual, 0, 0, 0);
                break;

            case FASE_1:
                // Desenha o fundo e o Mestre
                al_draw_bitmap(background_atual, 0, 0, 0);
                al_draw_bitmap(mestre_imagem, 100, chao_y - al_get_bitmap_height(mestre_imagem), 0);

                // Desenha o jogador
                int flags_f1 = virado_para_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                al_draw_bitmap(jogador_imagem_atual, jogador_x, jogador_y, flags_f1);

                // Desenha as text bubbles
                if (em_dialogo)
                {
                    // Igual para todos
                    float img_largura = 500;
                    float img_altura = 450;

                    // Posição dos balões de texo do mestre
                    float img_x = 180;
                    float img_y = 480;

                    // Posição x dos balões de texo do Grigori
                    if (indice_dialogo_atual >= 3)
                    {
                        img_x = 850;
                    }

                    ALLEGRO_BITMAP *img_dialogo_atual = dialogo_fase1_imgs[indice_dialogo_atual];

                    al_draw_scaled_bitmap(img_dialogo_atual,
                                          0, 0, al_get_bitmap_width(img_dialogo_atual), al_get_bitmap_height(img_dialogo_atual),
                                          img_x, img_y,
                                          img_largura, img_altura,
                                          0);

                    // Imagem que indica o botão P
                    float texto_p_largura_original = al_get_bitmap_width(texto_p);
                    float texto_p_altura_original = al_get_bitmap_height(texto_p);

                    float largura_texto_p = texto_p_largura_original * 0.5f;
                    float altura_texto_p = texto_p_altura_original * 0.5f;

                    // Posição
                    float pos_x_texto_p = (LARGURA_JOGO - largura_texto_p) / 2.0f;
                    float pos_y_texto_p = (ALTURA_JOGO - altura_texto_p) + 50;

                    al_draw_scaled_bitmap(texto_p,
                                          0, 0, texto_p_largura_original, texto_p_altura_original,
                                          pos_x_texto_p, pos_y_texto_p, largura_texto_p, altura_texto_p,
                                          0);

                }
                else {
                    al_draw_scaled_bitmap(avance,
                        0, 0, al_get_bitmap_width(avance), al_get_bitmap_height(avance),
                        300, 80, // Posição X, Y
                        900, 600,  // Largura e altura 
                        0);
                }

                break;

            case FINAL_1:
                // Desenha o fundo
                al_draw_bitmap(background_atual, 0, 0, 0);

                // Desenha o Mestre Doente
                float largura_original = al_get_bitmap_width(mestre_doente);
                float altura_original = al_get_bitmap_height(mestre_doente);
                float nova_largura = largura_original * 6.5;
                float nova_altura = altura_original * 6.0;
                float pos_x = 15;
                float pos_y = chao_y - nova_altura + 150;
                al_draw_scaled_bitmap(mestre_doente,
                                      0, 0, largura_original, altura_original,
                                      pos_x, pos_y,
                                      nova_largura, nova_altura,
                                      0);

                // Desenha o jogador
                int flags_final1 = virado_para_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                al_draw_bitmap(jogador_imagem_atual, jogador_x, jogador_y, flags_final1);

                // Se estiver em diálogo, desenha o balão de fala
                if (em_dialogo)
                {
                    float img_largura = 500;
                    float img_altura = 450;
                    float img_x = 180;
                    float img_y = 400;

                    // Lógica para alternar a posição da fala (Mestre vs Grigori)
                    if ((indice_dialogo_atual >= 3 && indice_dialogo_atual <= 5) ||
                        (indice_dialogo_atual >= 8 && indice_dialogo_atual <= 9))
                    {
                        img_x = 570; // Posição do Grigori (direita)
                    }

                    // Pega a imagem de diálogo do array do FINAL 1
                    ALLEGRO_BITMAP *img_dialogo_atual = dialogo_final1_imgs[indice_dialogo_atual];

                    // Desenha o balão
                    al_draw_scaled_bitmap(img_dialogo_atual,
                                          0, 0, al_get_bitmap_width(img_dialogo_atual), al_get_bitmap_height(img_dialogo_atual),
                                          img_x, img_y,
                                          img_largura, img_altura,
                                          0);

                    // Desenha o aviso "Aperte P"
                    float texto_p_largura_original = al_get_bitmap_width(texto_p);
                    float texto_p_altura_original = al_get_bitmap_height(texto_p);
                    float largura_texto_p = texto_p_largura_original * 0.5f;
                    float altura_texto_p = texto_p_altura_original * 0.5f;
                    float pos_x_texto_p = (LARGURA_JOGO - largura_texto_p) / 2.0f;
                    float pos_y_texto_p = (ALTURA_JOGO - altura_texto_p) + 50;
                    al_draw_scaled_bitmap(texto_p,
                                          0, 0, texto_p_largura_original, texto_p_altura_original,
                                          pos_x_texto_p, pos_y_texto_p, largura_texto_p, altura_texto_p,
                                          0);
                }
                break;

                // Desenha as fases de jogabilidade
            case FASE_2:
            case FASE_3:
            case FASE_4:
                // Desenha o fundo da fase
                al_draw_bitmap(background_atual, 0, 0, 0);

                if (estado_atual == FASE_4) {
                    for (int i = 0; i < MAX_BOLAS_DE_FOGO; i++) {
                        if (bolas_de_fogo[i].ativo) {
                            float largura_original = al_get_bitmap_width(bola_de_fogo);
                            float altura_original = al_get_bitmap_height(bola_de_fogo);

                            float nova_largura_bola = largura_original * 2.0f;
                            float nova_altura_bola = altura_original * 2.0f;

                            al_draw_scaled_bitmap(bola_de_fogo,
                                0, 0, largura_original, altura_original, 
                                bolas_de_fogo[i].x, bolas_de_fogo[i].y, 
                                nova_largura_bola, nova_altura_bola, 
                                0);
                        }
                    }
                }

                // Desenha todos os ITENS ativos 
                for (int i = 0; i < MAX_ITENS; i++)
                {
                    if (itens[i].ativo)
                    {
                        al_draw_scaled_bitmap(itens[i].sprite,
                                              0, 0, al_get_bitmap_width(itens[i].sprite), al_get_bitmap_height(itens[i].sprite),
                                              itens[i].x, itens[i].y,
                                              itens[i].largura, itens[i].altura, // Usa o tamanho definido no spawn
                                              0);

                        //Hitbox
                        //al_draw_rectangle(itens[i].x, itens[i].y, itens[i].x + itens[i].largura, itens[i].y + itens[i].altura, al_map_rgba(255, 255, 0, 100), 1);
                    }
                }

                // Desenha todos os INIMIGOS ativos
                for (int i = 0; i < MAX_INIMIGOS; i++)
                {
                    if (inimigos[i].ativo)
                    {
                        int flags_inimigo = inimigos[i].virado_para_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                        al_draw_scaled_bitmap(inimigos[i].sprite,
                                              0, 0, al_get_bitmap_width(inimigos[i].sprite), al_get_bitmap_height(inimigos[i].sprite),
                                              inimigos[i].x, inimigos[i].y,
                                              inimigos[i].largura_desenho, inimigos[i].altura_desenho,
                                              flags_inimigo);

                        float hitbox_x = inimigos[i].x + inimigos[i].hitbox_offset_x;
                        float hitbox_y = inimigos[i].y + inimigos[i].hitbox_offset_y;
                        //Hitbox
                        //al_draw_rectangle(hitbox_x, hitbox_y, hitbox_x + inimigos[i].hitbox_largura, hitbox_y + inimigos[i].hitbox_altura, al_map_rgba(255, 0, 0, 100), 1);
                    }
                }

                // Desenha o JOGADOR (piscando, se invencível)
                bool desenhar_jogador = true;
                if (jogador_invencivel)
                {
                    if ((timer_invencibilidade / 4) % 2 == 1)
                    {
                        desenhar_jogador = false;
                    }
                }
                if (desenhar_jogador)
                {
                    int flags = virado_para_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                    al_draw_bitmap(jogador_imagem_atual, jogador_x, jogador_y, flags);

                    float hitbox_real_x = jogador_x + jogador_hitbox_offset_x;
                    float hitbox_real_y = jogador_y + jogador_hitbox_offset_y;
                    //Hitbox
                    /*al_draw_rectangle(hitbox_real_x, hitbox_real_y,
                                      hitbox_real_x + jogador_hitbox_largura, hitbox_real_y + jogador_hitbox_altura,
                                      al_map_rgba(255, 0, 0, 100), 1);*/
                }

                // Desenha todas as MAGIAS ativas
                for (int i = 0; i < MAXIMO_DE_MAGIAS_TOTAL; i++)
                {
                    if (magias[i].ativa)
                    {
                        ALLEGRO_BITMAP *sprite_magia_atual = NULL;
                        switch (magias[i].tipo_magia)
                        {
                        case FASE_2:
                            sprite_magia_atual = magia_fogo_img;
                            break;
                        case FASE_3:
                            sprite_magia_atual = magia_gelo_img;
                            break;
                        case FASE_4:
                            sprite_magia_atual = magia_raio_img;
                            break;
                        }
                        if (sprite_magia_atual)
                        {
                            int magia_flags = (magias[i].velocidade_x > 0) ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                            al_draw_bitmap(sprite_magia_atual, magias[i].x, magias[i].y, magia_flags);

                            //Hitbox
                            //al_draw_rectangle(magias[i].x, magias[i].y, magias[i].x + magias[i].largura, magias[i].y + magias[i].altura, al_map_rgba(0, 255, 0, 100), 1);
                        }
                    }
                }

                // Desenha os corações 
                float coracao_largura_nova = 80;
                float coracao_altura_nova = 80;
                float coracao_largura_original = al_get_bitmap_width(coracao_img);
                float coracao_altura_original = al_get_bitmap_height(coracao_img);
                float espacamento = 10;
                for (int i = 0; i < vida_jogador; i++)
                {
                    al_draw_scaled_bitmap(coracao_img,
                                          0, 0, coracao_largura_original, coracao_altura_original,
                                          20 + (coracao_largura_nova + espacamento) * i, 20,
                                          coracao_largura_nova, coracao_altura_nova, 0);
                }

                bool mostrar_avance = false;

                // Condições da fase 2 e 3 para a imagem ""avançar" aparecer
                if (estado_atual == FASE_2 || estado_atual == FASE_3) {
                    if (tem_item_minion_atual && tem_item_chefe_atual) {
                        mostrar_avance = true;
                    }
                }
                // Condições da fase 4 para a imagem "avançar" aparecer
                else if (estado_atual == FASE_4) {
                    if (boss_final_spawnado) {
                        bool boss_vivo = false;
                        for (int i = 0; i < MAX_INIMIGOS; i++) {
                            // Verifica se o Titan ainda está ativo no array
                            if (inimigos[i].ativo && inimigos[i].sprite == sprite_titan_lava) {
                                boss_vivo = true;
                                break;
                            }
                        }
                        // Se o boss spawnou e não está vivo, mostra o aviso
                        if (!boss_vivo) {
                            mostrar_avance = true;
                        }
                    }
                }

                if (mostrar_avance) {
                    al_draw_scaled_bitmap(avance,
                        0, 0, al_get_bitmap_width(avance), al_get_bitmap_height(avance),
                        300, 80, // Posição X, Y
                        900, 600,  // Largura e altura 
                        0);
                }
                break;
            }

            al_flip_display();
        }
    }
    // Finalização
    if (background_atual)
        al_destroy_bitmap(background_atual);

    // Destroi APENAS os bitmaps que foram realmente carregados
    al_destroy_bitmap(jogador_fase1);
    al_destroy_bitmap(jogador_fase2);
    al_destroy_bitmap(jogador_fase3);
    al_destroy_bitmap(jogador_fase4);
    al_destroy_bitmap(coracao_img);

    al_destroy_bitmap(sprite_slime_normal);
    al_destroy_bitmap(sprite_slime_bravo);
    al_destroy_bitmap(sprite_golem_gelo);
    al_destroy_bitmap(sprite_ice_cyclop);
    al_destroy_bitmap(sprite_golem_lava);
    al_destroy_bitmap(sprite_titan_lava);

    al_destroy_bitmap(mestre_imagem);
    al_destroy_bitmap(mestre_doente);

    al_destroy_bitmap(magia_fogo_img);
    al_destroy_bitmap(magia_gelo_img);
    al_destroy_bitmap(magia_raio_img);
    al_destroy_bitmap(bola_de_fogo);

    al_destroy_bitmap(sprite_item_minion_f2);
    al_destroy_bitmap(sprite_item_chefe_f2);
    al_destroy_bitmap(sprite_item_minion_f3);
    al_destroy_bitmap(sprite_item_chefe_f3);

    // Destroi as falas da fase 1
    for (int i = 0; i < NUM_DIALOGOS_FASE1; i++)
    {
        al_destroy_bitmap(dialogo_fase1_imgs[i]);
    }
    // Destroi as falas do final 1
    for (int i = 0; i < NUM_DIALOGOS_FINAL1; i++)
    {
        if (dialogo_final1_imgs[i])
        {
            al_destroy_bitmap(dialogo_final1_imgs[i]);
        }
    }

    al_destroy_bitmap(texto_p);

    al_destroy_timer(timer);
    al_destroy_event_queue(fila_eventos);
    al_destroy_display(janela);

    return 0;
}

// Implementação das Funções de Fase 

void carregar_menu()
{
    background_atual = al_load_bitmap("imagens/Menu.png");
    if (!background_atual)
    {
        printf("Erro ao carregar o menu\n");
        exit(-1);
    }
}
void descarregar_menu()
{
    al_destroy_bitmap(background_atual);
    background_atual = NULL;
}

void carregar_instrucoes()
{
    background_atual = al_load_bitmap("imagens/Instrucao.png");
    if (!background_atual)
    {
        printf("Erro ao carregar as instruções\n");
        exit(-1);
    }
}
void descarregar_instrucoes()
{
    al_destroy_bitmap(background_atual);
    background_atual = NULL;
}

// FASE 1
void carregar_fase_1(float *jogador_x_ptr, float *jogador_y_ptr, bool *virado_dir_ptr, float *chao_y_ptr)
{
    background_atual = al_load_bitmap("imagens/Tela_inicial.png");

    if (!background_atual)
    {
        printf("Erro ao carregar fundo da fase 1!\n");
        exit(-1);
    }
    *jogador_x_ptr = 1150;
    *virado_dir_ptr = false;
    *chao_y_ptr = 990.0f;

    em_dialogo = true;
    indice_dialogo_atual = 0;
    total_dialogos = NUM_DIALOGOS_FASE1;
}
void descarregar_fase_1()
{
    if (background_atual)
        al_destroy_bitmap(background_atual);
    background_atual = NULL;
}
// FASE 2
void carregar_fase_2(float *jogador_x_ptr, float *jogador_y_ptr, bool *virado_dir_ptr, float *chao_y_ptr, Inimigo array_inimigos[])
{
    background_atual = al_load_bitmap("imagens/Floresta.png");
    if (!background_atual)
    {
        printf("Erro ao carregar fundo da fase 2!\n");
        exit(-1);
    }

    *jogador_x_ptr = 50.0f;
    *jogador_y_ptr = 850.0f;
    *virado_dir_ptr = true;
    *chao_y_ptr = 905.0f;

    limpar_inimigos(array_inimigos); // Limpa os inimigos da fase anterior

    // Zera os contadores de spawn
    timer_spawn_slime = 0;
    slimes_spawnados = 0;
    timer_fase_2 = 0;

    minions_para_derrotar = TOTAL_SLIMES_FASE_2; // Define o objetivo
    minions_derrotados_atual = 0;
    chefe_derrotado_atual = false;
    tem_item_minion_atual = false;
    tem_item_chefe_atual = false;
}

void descarregar_fase_2()
{
    if (background_atual)
        al_destroy_bitmap(background_atual);
    background_atual = NULL;
}

// FASE 3
void carregar_fase_3(float *jogador_x_ptr, float *jogador_y_ptr, bool *virado_dir_ptr, float *chao_y_ptr, Inimigo array_inimigos[])
{
    background_atual = al_load_bitmap("imagens/Montanha.png");
    if (!background_atual)
    {
        printf("Erro ao carregar fundo da fase 3!\n");
        exit(-1);
    }
    *jogador_x_ptr = 50.0f;
    *jogador_y_ptr = 700.0f;
    *virado_dir_ptr = true;
    *chao_y_ptr = 750.0f;

    limpar_inimigos(array_inimigos); // Limpa os inimigos da fase anterior

    // Zera os contadores de spawn
    timer_spawn_golem = 0; // Spawna o Golem de gelo
    golems_spawnados = 0;
    timer_fase_3 = 0;

    minions_para_derrotar = TOTAL_GOLEMS_FASE_3; 
    minions_derrotados_atual = 0;
    chefe_derrotado_atual = false;
    tem_item_minion_atual = false;
    tem_item_chefe_atual = false;
}

void descarregar_fase_3()
{
    if (background_atual)
        al_destroy_bitmap(background_atual);
    background_atual = NULL;
}
// FASE 4
void carregar_fase_4(float *jogador_x_ptr, float *jogador_y_ptr, bool *virado_dir_ptr, float *chao_y_ptr, Inimigo array_inimigos[])
{
    background_atual = al_load_bitmap("imagens/Vulcão.png");
    if (!background_atual)
    {
        printf("Erro ao carregar fundo da fase 4!\n");
        exit(-1);
    }
    *jogador_x_ptr = 50.0f;
    *jogador_y_ptr = 900.0f;
    *virado_dir_ptr = true;
    *chao_y_ptr = 820.0f;

    limpar_bolas_de_fogo(); // Remove bolas de fogo que tenham ficado
    timer_spawn_fogo = 0;   // Reseta o timer

    limpar_inimigos(array_inimigos);

    // Zera os contadores de spawn
    timer_spawn_golem_lava = 0; // Spawna o primeiro Golem de Lava 
    golems_lava_spawnados = 0;
    timer_fase_4 = 0;
    spawn_ativo_fase_4 = true;   
    boss_final_spawnado = false; 

    // NÃO spawna nenhum inimigo inicial - eles vão aparecer dinamicamente
}

void descarregar_fase_4()
{
    if (background_atual)
        al_destroy_bitmap(background_atual);
    background_atual = NULL;
}

void carregar_transicao_1() { background_atual = al_load_bitmap("imagens/Explicação-fogo.png"); }
void descarregar_transicao_1()
{
    al_destroy_bitmap(background_atual);
    background_atual = NULL;
}

void carregar_transicao_2() { background_atual = al_load_bitmap("imagens/Explicação-gelo.png"); }
void descarregar_transicao_2()
{
    al_destroy_bitmap(background_atual);
    background_atual = NULL;
}

void carregar_transicao_3() { background_atual = al_load_bitmap("imagens/Explicação-raio.png"); }
void descarregar_transicao_3()
{
    al_destroy_bitmap(background_atual);
    background_atual = NULL;
}

void carregar_transicao_4() { background_atual = al_load_bitmap("imagens/Explicação-plasma.png"); }
void descarregar_transicao_4()
{
    al_destroy_bitmap(background_atual);
    background_atual = NULL;
}

void carregar_escolha()
{
    background_atual = al_load_bitmap("imagens/Escolha.png");
    if (!background_atual)
    {
        printf("Erro ao carregar a tela de escolha!\n");
        exit(-1);
    }
}
void descarregar_escolha()
{
    if (background_atual)
        al_destroy_bitmap(background_atual);
    background_atual = NULL;
}

void carregar_narrador1_f1()
{
    background_atual = al_load_bitmap("imagens/narrador1_f1.png");
    if (!background_atual)
    {
        printf("Erro ao carregar narrador1_f1!\n");
        exit(-1);
    }
}
void descarregar_narrador1_f1()
{
    if (background_atual)
        al_destroy_bitmap(background_atual);
    background_atual = NULL;
}

void carregar_final_1(float *jogador_x_ptr, float *jogador_y_ptr, bool *virado_dir_ptr, float *chao_y_ptr)
{
    background_atual = al_load_bitmap("imagens/Tela_final.png");
    if (!background_atual)
    {
        printf("Erro ao carregar a tela do Final 1!\n");
        exit(-1);
    }

    *jogador_x_ptr = 1200;
    *virado_dir_ptr = false;
    *chao_y_ptr = 940.0f;

    // Resetar itens e inimigos
    limpar_inimigos(inimigos);
    limpar_itens(itens);

    // Prepara o diálogo, mas não o inicia
    em_dialogo = false;
    indice_dialogo_atual = 0;
    total_dialogos = NUM_DIALOGOS_FINAL1;
    dialogo_final1_iniciado = false;
}
void descarregar_final_1()
{
    if (background_atual)
        al_destroy_bitmap(background_atual);
    background_atual = NULL;
}

void carregar_narrador2_f1()
{
    background_atual = al_load_bitmap("imagens/narrador2_f1.png");
    if (!background_atual)
    {
        printf("Erro ao carregar narrador2_f1!\n");
        exit(-1);
    }
}
void descarregar_narrador2_f1()
{
    if (background_atual)
        al_destroy_bitmap(background_atual);
    background_atual = NULL;
}

void carregar_final_2()
{
    background_atual = al_load_bitmap("imagens/Texto_final1.png");
    if (!background_atual)
    {
        printf("Erro ao carregar a tela Final 2 Texto!\n");
        exit(-1);
    }
}
void descarregar_final_2()
{
    if (background_atual)
        al_destroy_bitmap(background_atual);
    background_atual = NULL;
}

void carregar_tela_obrigado()
{
    background_atual = al_load_bitmap("imagens/Final_2.png");
    if (!background_atual)
    {
        printf("Erro ao carregar a tela de Obrigado!\n");
        exit(-1);
    }
}
void descarregar_tela_obrigado()
{
    if (background_atual)
        al_destroy_bitmap(background_atual);
    background_atual = NULL;
}

void limpar_inimigos(Inimigo array_inimigos[])
{
    for (int i = 0; i < MAX_INIMIGOS; i++)
    {
        array_inimigos[i].ativo = false;
    }
}

void spawn_item(float x, float chao_y_atual, ALLEGRO_BITMAP *sprite, int tipo_item)
{
    // Procura o primeiro espaço de item vazio
    for (int i = 0; i < MAX_ITENS; i++)
    {
        if (!itens[i].ativo)
        {
            itens[i].ativo = true;
            itens[i].sprite = sprite;
            itens[i].tipo_item = tipo_item;

            // tamanho dos itens
            itens[i].largura = 100.0f;
            itens[i].altura = 100.0f;

            // Define a posição
            itens[i].x = x;
            itens[i].y = chao_y_atual - itens[i].altura;

            printf("Item %d criado na posicao %.1f, %.1f\n", tipo_item, itens[i].x, itens[i].y);

            break; // Sai do loop, pois já criou o item
        }
    }
}

void limpar_itens(Item array_itens[])
{
    for (int i = 0; i < MAX_ITENS; i++)
    {
        array_itens[i].ativo = false;
    }
}

void limpar_bolas_de_fogo() {
    for (int i = 0; i < MAX_BOLAS_DE_FOGO; i++) {
        bolas_de_fogo[i].ativo = false;
    }
}

void spawn_inimigo(Inimigo array_inimigos[], ALLEGRO_BITMAP *sprite,
                   float x, float y, float y_velocidade, bool no_chao,
                   float vel_x, float forca_pulo, int freq_pulo,
                   float p_inicio, float p_fim,
                   float escala,
                   float h_offset_x, float h_offset_y, float h_ajuste_w, float h_ajuste_h, int vida_inicial)
{
    // Procura o primeiro espaço vazio (inimigo inativo) no array
    for (int i = 0; i < MAX_INIMIGOS; i++)
    {
        if (!array_inimigos[i].ativo)
        {

            // Configurações visuais
            array_inimigos[i].ativo = true;
            array_inimigos[i].sprite = sprite;
            array_inimigos[i].largura_desenho = al_get_bitmap_width(sprite) * escala;
            array_inimigos[i].altura_desenho = al_get_bitmap_height(sprite) * escala;

            // Posição e Física 
            array_inimigos[i].x = x;
            array_inimigos[i].y = y;
            array_inimigos[i].y_velocidade = y_velocidade;
            array_inimigos[i].no_chao = no_chao;
            array_inimigos[i].velocidade_x = vel_x;
            array_inimigos[i].virado_para_direita = (vel_x > 0);

            // Comportamento de IA 
            array_inimigos[i].forca_pulo = forca_pulo;
            array_inimigos[i].frequencia_pulo = freq_pulo;
            array_inimigos[i].timer_pulo = freq_pulo;
            array_inimigos[i].patrol_x_inicio = p_inicio;
            array_inimigos[i].patrol_x_fim = p_fim;

            // Hitbox 
            array_inimigos[i].hitbox_offset_x = h_offset_x;
            array_inimigos[i].hitbox_offset_y = h_offset_y;
            array_inimigos[i].hitbox_largura = array_inimigos[i].largura_desenho - h_ajuste_w;
            array_inimigos[i].hitbox_altura = array_inimigos[i].altura_desenho - h_ajuste_h;

            array_inimigos[i].vida = vida_inicial;

            break; // Encontrou um espaço, então sai do loop
        }
    }
}

void carregar_gameover()
{
    background_atual = al_load_bitmap("imagens/gameover.png");
    if (!background_atual)
    {
        printf("Erro ao carregar a tela de Game Over!\n");
        exit(-1);
    }
}
void descarregar_gameover()
{
    if (background_atual)
        al_destroy_bitmap(background_atual);
    background_atual = NULL;
}

bool checa_colisao(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2)
{
    // Verifica se dois retângulos (hitboxes) estão se sobrepondo.

    // Retorna 'falso' se o retângulo 1 está totalmente à direita do retângulo 2,
    // ou se o retângulo 1 está totalmente à esquerda do retângulo 2.
    if (x1 > x2 + w2 || x1 + w1 < x2)
    {
        return false;
    }

    // Retorna 'falso' se o retângulo 1 está totalmente abaixo do retângulo 2,
    // ou se o retângulo 1 está totalmente acima do retângulo 2.
    if (y1 > y2 + h2 || y1 + h1 < y2)
    {
        return false;
    }

    // Se nenhuma das condições acima for verdadeira, então eles devem estar colidindo.
    return true;
}