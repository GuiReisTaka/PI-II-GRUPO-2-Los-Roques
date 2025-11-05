#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

// --- Define os estados do jogo ---
typedef enum {
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
    FINAL_1,
	FINAL_2,
    TELA_OBRIGADO,
    TELA_GAMEOVER
} EstadoJogo;

// Variaveis das magias
typedef struct {
    float x, y;
    float velocidade_x;
    bool ativa;
    EstadoJogo tipo_magia;
    float largura; 
    float altura;
} Magia;

#define MAXIMO_DE_MAGIAS_TOTAL 15 

// Variaveis dos inimigos
typedef struct {
    float x, y;                 
    ALLEGRO_BITMAP* sprite;     

    float y_velocidade;         
    float velocidade_x;         
    bool no_chao;               
    float forca_pulo;           
    int timer_pulo;             
    int frequencia_pulo;        

    bool virado_para_direita;   // Direção individual de cada inimigo
    float patrol_x_inicio;      
    float patrol_x_fim;

    // Hitbox e Desenho
    float hitbox_offset_x;
    float hitbox_offset_y;
    float hitbox_largura;
    float hitbox_altura;
    float largura_desenho;      
    float altura_desenho;       

    bool ativo; // Diz de o inimigo está na tela, "vivo"            
} Inimigo;

#define MAX_INIMIGOS 10

#define TEMPO_INVENCIBILIDADE (60 * 2)

//Inimigos da fase da floresta
#define FREQUENCIA_SPAWN_SLIME (60 * 0.5) // 2 segundos
#define TEMPO_SPAWN_CHEFE (60 * 20)      // 20 segundos
#define TOTAL_SLIMES_FASE_2 25

int timer_spawn_slime = 0;  // Contador para o próximo slime normal
int slimes_spawnados = 0;   // Quantos slimes já apareceram
int timer_fase_2 = 0;

Inimigo inimigos[MAX_INIMIGOS];
ALLEGRO_BITMAP* sprite_slime_normal = NULL;
ALLEGRO_BITMAP* sprite_slime_bravo = NULL;
ALLEGRO_BITMAP* sprite_golem_gelo = NULL;
ALLEGRO_BITMAP* sprite_ice_cyclop = NULL;
ALLEGRO_BITMAP* sprite_golem_lava = NULL;
ALLEGRO_BITMAP* sprite_titan_lava = NULL;

// Magia de Fogo (Fase 2)
#define MAX_MAGIAS_FOGO 5
#define VELOCIDADE_FOGO 12.0f

// Magia de Gelo 
#define MAX_MAGIAS_GELO 5
#define VELOCIDADE_GELO 8.0f 

// Magia de raio
#define MAX_MAGIAS_raio 3
#define VELOCIDADE_raio 15.0f 

// Cooldown das magias
#define COOLDOWN_MAGIA 100

//Define a altura e a largura da tela
#define LARGURA_JOGO 1536
#define ALTURA_JOGO 1024

// --- Variável Global para o fundo atual ---
ALLEGRO_BITMAP* background_atual = NULL;

// --- Declaração das Funções ---
void carregar_menu();
void descarregar_menu();
void carregar_instrucoes();
void descarregar_instrucoes();

void carregar_fase_1(float* jogador_x_ptr, float* jogador_y_ptr, bool* virado_dir_ptr, float* chao_y_ptr);
void descarregar_fase_1();
void carregar_fase_2(float* jogador_x_ptr, float* jogador_y_ptr, bool* virado_dir_ptr, float* chao_y_ptr, Inimigo array_inimigos[]);
void descarregar_fase_2();
void carregar_fase_3(float* jogador_x_ptr, float* jogador_y_ptr, bool* virado_dir_ptr, float* chao_y_ptr, Inimigo array_inimigos[]);
void descarregar_fase_3();
void carregar_fase_4(float* jogador_x_ptr, float* jogador_y_ptr, bool* virado_dir_ptr, float* chao_y_ptr, Inimigo array_inimigos[]);
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

void carregar_final_1();
void descarregar_final_1();

void carregar_final_2();
void descarregar_final_2();
void carregar_tela_obrigado();
void descarregar_tela_obrigado();

void carregar_gameover();
void descarregar_gameover();

void limpar_inimigos(Inimigo array_inimigos[]);

void spawn_inimigo(Inimigo array_inimigos[], ALLEGRO_BITMAP* sprite,
    float x, float y, float y_velocidade, bool no_chao,
    float vel_x, float forca_pulo, int freq_pulo,
    float p_inicio, float p_fim,
    float escala,
    float h_offset_x, float h_offset_y, float h_ajuste_w, float h_ajuste_h);

bool checa_colisao(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);


// --- Função para checar clique no botão ---
bool mouse_dentro_da_area(float mouse_x, float mouse_y, float x1, float y1, float x2, float y2) {
    return (mouse_x >= x1 && mouse_x <= x2 && mouse_y >= y1 && mouse_y <= y2);
}

int main()
{
    // --- Variáveis de representação ---
    ALLEGRO_DISPLAY* janela = NULL;
    ALLEGRO_EVENT_QUEUE* fila_eventos = NULL;
    ALLEGRO_TIMER* timer = NULL;
    ALLEGRO_BITMAP* mestre_imagem = NULL;
    ALLEGRO_BITMAP* magia_imagem = NULL;

    ALLEGRO_BITMAP* jogador_fase1 = NULL;
    ALLEGRO_BITMAP* jogador_fase2 = NULL;
    ALLEGRO_BITMAP* jogador_fase3 = NULL;
    ALLEGRO_BITMAP* jogador_fase4 = NULL;
    ALLEGRO_BITMAP* jogador_final1 = NULL;
    ALLEGRO_BITMAP* jogador_imagem_atual = NULL;
    ALLEGRO_BITMAP* coracao_img = NULL;

    ALLEGRO_BITMAP* magia_fogo_img = NULL;
    ALLEGRO_BITMAP* magia_gelo_img = NULL;
    ALLEGRO_BITMAP* magia_raio_img = NULL;

    Magia magias[MAXIMO_DE_MAGIAS_TOTAL];

    // --- O jogo começa no estado MENU ---
    EstadoJogo estado_atual = MENU;

    // --- Inicialização do Allegro ---
    al_init();
    al_init_primitives_addon();
    al_init_image_addon();
    al_install_keyboard();
    al_install_mouse();

    // --- Criação da Janela, Timer e Fila de Eventos ---
    timer = al_create_timer(1.0 / 60.0);
    al_set_new_display_flags(ALLEGRO_RESIZABLE);
    janela = al_create_display(LARGURA_JOGO, ALTURA_JOGO);
    fila_eventos = al_create_event_queue();

    // --- Carrega imagens permanentes ---
    jogador_fase1 = al_load_bitmap("imagens/p1.png"); 
    jogador_fase2 = al_load_bitmap("imagens/p2.png"); 
    jogador_fase3 = al_load_bitmap("imagens/p3.png"); 
    jogador_final1 = al_load_bitmap("imagens/p4.png");
    mestre_imagem = al_load_bitmap("imagens/Mestre1.png");
    coracao_img = al_load_bitmap("imagens/Coracao-normal.png");
    
    sprite_slime_normal = al_load_bitmap("imagens/slime-normal.png");
    sprite_slime_bravo = al_load_bitmap("imagens/slime-bravo.png");
    sprite_golem_gelo = al_load_bitmap("imagens/golem-gelo.png");
    sprite_ice_cyclop = al_load_bitmap("imagens/ice-cyclope.png");
    sprite_golem_lava = al_load_bitmap("imagens/golem-lava.png");
    sprite_titan_lava = al_load_bitmap("imagens/titan-lava.png");
   
    magia_fogo_img = al_load_bitmap("imagens/Fogo.png");
    magia_gelo_img = al_load_bitmap("imagens/Gelo.png");
    magia_raio_img = al_load_bitmap("imagens/Raio.png");
    

    // Verificação de erro para todas as imagens
    if (!jogador_fase1 || !jogador_fase2 || !jogador_fase3 || !mestre_imagem || !magia_fogo_img || !magia_gelo_img || !magia_raio_img || !coracao_img ||
        !sprite_slime_normal || !sprite_slime_bravo || !sprite_golem_gelo || !sprite_ice_cyclop || !sprite_golem_lava || !sprite_titan_lava) {
        printf("Erro ao carregar uma ou mais imagens permanentes!\n");
        return -1;
    }

    // Define a imagem inicial a ser usada
    jogador_imagem_atual = jogador_fase1;

    // Inicializa magias como inativas
    for (int i = 0; i < MAXIMO_DE_MAGIAS_TOTAL; i++) {
        magias[i].ativa = false;
    }

    // Inicializa inimigos como inativos
    for (int i = 0; i < MAX_INIMIGOS; i++) {
        inimigos[i].ativo = false;
    }

    // --- Registra fontes de eventos ---
    al_register_event_source(fila_eventos, al_get_display_event_source(janela));
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
    al_register_event_source(fila_eventos, al_get_mouse_event_source());


	// botões do menu
    float botao_iniciar_x1 = 580, botao_iniciar_y1 = 790, botao_iniciar_x2 = 925, botao_iniciar_y2 = 860;
    float botao_instrucoes_x1 = 520, botao_instrucoes_y1 = 820, botao_instrucoes_x2 = 1014, botao_instrucoes_y2 = 950;
    float botao_voltar_x1 = 618, botao_voltar_y1 = 800, botao_voltar_x2 = 918, botao_voltar_y2 = 880;

	// Botão de avançar nas transições
    float botao_avancar_x1 = 570, botao_avancar_y1 = 785, botao_avancar_x2 = 967, botao_avancar_y2 = 896; 

	// Botões da tela de escolha 
    float botao_final1_x1 = 350, botao_final1_y1 = 390, botao_final1_x2 = 1180, botao_final1_y2 = 564;
    float botao_final2_x1 = 350, botao_final2_y1 = 590, botao_final2_x2 = 1180, botao_final2_y2 = 750;

	// Botão da tela de obrigado
    float botao_para_obrigado_x1 = 592, botao_para_obrigado_y1 = 810, botao_para_obrigado_x2 = 950, botao_para_obrigado_y2 = 933;

    // Botão da tela de gameover
    float botao_reiniciar_x1 = 520, botao_reiniciar_y1 = 630, botao_reiniciar_x2 = 995, botao_reiniciar_y2 = 755;

    // --- Variáveis do Jogo ---
    float chao_y = 990;
    float jogador_largura = al_get_bitmap_width(jogador_imagem_atual);
    float jogador_altura = al_get_bitmap_height(jogador_imagem_atual);
    float jogador_x = 550, jogador_y = 800; 
    float jogador_velocidade_y = 0;
    float velocidade_horizontal = 5.0;
    float gravidade = 0.5;
    float forca_pulo = 10.0;
    bool no_chao = false;
    bool tecla_a = false, tecla_d = false;
    bool virado_para_direita = false; 

    int vida_jogador = 3;

    int timer_magia = 0;

    bool jogador_invencivel = false;
    int timer_invencibilidade = 0;

    float jogador_hitbox_offset_x = 10; 
    float jogador_hitbox_offset_y = 5;  
    float jogador_hitbox_largura = jogador_largura - 40; 
    float jogador_hitbox_altura = jogador_altura - 10;

    // Carrega o menu
    carregar_menu();

    al_start_timer(timer);
    bool sair = false;
    bool redesenhar = true;

    while(!sair)
    {
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_TIMER) {

            if (timer_magia > 0) {
                timer_magia--;
            }

            if (timer_invencibilidade > 0) {
                timer_invencibilidade--;
                if (timer_invencibilidade == 0) {
                    jogador_invencivel = false; 
                }
            }

            // A lógica de física e movimento só roda durante as fases
            if ((estado_atual >= FASE_1 && estado_atual <= FASE_4) || estado_atual == FINAL_1) {
                // Movimento Horizontal
                if (tecla_a) { jogador_x -= velocidade_horizontal; }
                if (tecla_d) { jogador_x += velocidade_horizontal; }

                // Gravidade e Pulo
                jogador_velocidade_y += gravidade;
                jogador_y += jogador_velocidade_y;
                no_chao = false;
                if (jogador_y + jogador_altura >= chao_y) {
                    jogador_y = chao_y - jogador_altura;
                    jogador_velocidade_y = 0;
                    no_chao = true;
                }

                // Movimento das magias (só na fase 2, mas a verificação já está no clique)
                for (int i = 0; i < MAXIMO_DE_MAGIAS_TOTAL; i++) {
                    if (magias[i].ativa) {
                        magias[i].x += magias[i].velocidade_x;
                        if (magias[i].x < 0 || magias[i].x > 1536) {
                            magias[i].ativa = false;
                        }
                    }
                }

                //Movimentação dos inimigos
                switch (estado_atual) {
                case FASE_2: 
                    timer_fase_2++; // Incrementa o timer principal da fase

                    // Spawna o Slime Bravo uma única vez
                    if (timer_fase_2 == TEMPO_SPAWN_CHEFE) {
                        printf("CHEFE SLIME BRAVO SPAWNOU!\n");
                        
                        spawn_inimigo(inimigos, sprite_slime_bravo,
                            1000, 500, 0, false,
                            0, 9.0f, 150,
                            0, 0,
                            1.5f,
                            15, 140, 30, 180);
                    }

                    // Spawna os Slimes Normais 
                    if (slimes_spawnados < TOTAL_SLIMES_FASE_2) {
                        timer_spawn_slime--; 
                        if (timer_spawn_slime <= 0) {
                            printf("Slime normal #%d spawnou!\n", slimes_spawnados + 1);

                            // Spawna o slime em um X aleatório (entre 500 e 1299)
                            float spawn_x = 500 + (rand() % 800);

                            spawn_inimigo(inimigos, sprite_slime_normal,
                                spawn_x, 600, 10.0f, false,
                                0, 15.0f, 65,
                                0, 0,
                                1.0f,
                                5, 80, 10, 85);

                            slimes_spawnados++; // Incrementa a contagem
                            timer_spawn_slime = FREQUENCIA_SPAWN_SLIME; // Reseta o timer para o próximo
                        }
                    }

                    for (int i = 0; i < MAX_INIMIGOS; i++) {
                        if (inimigos[i].ativo) {
                            // Aplica gravidade
                            inimigos[i].y_velocidade += gravidade;
                            inimigos[i].y += inimigos[i].y_velocidade;
                            inimigos[i].x += inimigos[i].velocidade_x;

                            // Checa colisão com o chão
                            if (inimigos[i].y + inimigos[i].altura_desenho >= chao_y) {
                                inimigos[i].y = chao_y - inimigos[i].altura_desenho;
                                inimigos[i].y_velocidade = 0;
                                inimigos[i].velocidade_x = 0; // Para de andar quando está no chão
                                inimigos[i].no_chao = true;
                            }

                            // Lógica de Pulo
                            if (inimigos[i].no_chao) {
                                inimigos[i].timer_pulo--; // Diminui o contador
                                if (inimigos[i].timer_pulo <= 0) {
                                    // Deu o tempo de pular!
                                    inimigos[i].y_velocidade = -inimigos[i].forca_pulo;
                                    if (jogador_x > inimigos[i].x) {
                                        inimigos[i].velocidade_x = (inimigos[i].frequencia_pulo == 75) ? 3.0f : 1.5f;
                                    }
                                    else {
                                        inimigos[i].velocidade_x = (inimigos[i].frequencia_pulo == 75) ? -3.0f : -1.5f;
                                    }
                                    inimigos[i].no_chao = false;
                                    inimigos[i].timer_pulo = inimigos[i].frequencia_pulo;
                                }
                            }
                        }
                    }
                    break;

                case FASE_3:
                    for (int i = 0; i < MAX_INIMIGOS; i++) {
                        if (inimigos[i].ativo) {
                            // 1. Aplica o movimento horizontal
                            inimigos[i].x += inimigos[i].velocidade_x;

                            // 2. Verifica se atingiu os limites da patrulha
                            if (inimigos[i].x < inimigos[i].patrol_x_inicio) {
                                inimigos[i].x = inimigos[i].patrol_x_inicio; 
                                inimigos[i].velocidade_x *= -1; 
                                inimigos[i].virado_para_direita = true; 
                            }
                            else if (inimigos[i].x + inimigos[i].largura_desenho > inimigos[i].patrol_x_fim) {
                                inimigos[i].x = inimigos[i].patrol_x_fim - inimigos[i].largura_desenho; 
                                inimigos[i].velocidade_x *= -1; 
                                inimigos[i].virado_para_direita = false; 
                            }
                        }
                    }
                    break;

                case FASE_4:
                    // Futuramente, aqui entrará a lógica de movimento dos monstros de Lava
                    break;
                }

                //Colisão entre as magias e os inimigos
                for (int i = 0; i < MAXIMO_DE_MAGIAS_TOTAL; i++) {
                    // Se a magia atual estiver ativa percorre todos os inimigos
                    if (magias[i].ativa) {

                        for (int j = 0; j < MAX_INIMIGOS; j++) {
                            // Se o inimigo atual estiver ativo
                            if (inimigos[j].ativo) {

                                // Calcula as coordenadas reais das hitboxes na tela
                                float magia_hitbox_x = magias[i].x;
                                float magia_hitbox_y = magias[i].y;

                                float inimigo_hitbox_x = inimigos[j].x + inimigos[j].hitbox_offset_x;
                                float inimigo_hitbox_y = inimigos[j].y + inimigos[j].hitbox_offset_y;

                                // Usa a função para checar se as hitboxes se tocam
                                if (checa_colisao(magia_hitbox_x, magia_hitbox_y, magias[i].largura, magias[i].altura,
                                    inimigo_hitbox_x, inimigo_hitbox_y, inimigos[j].hitbox_largura, inimigos[j].hitbox_altura))
                                {

                                    magias[i].ativa = false;
                                    inimigos[j].ativo = false;

                                    printf("COLISAO! Magia atingiu um inimigo.\n");
                                }
                            }
                        }
                    }
                }

                // --- Colisão Jogador vs Inimigos ---
                float jogador_hitbox_x_real = jogador_x + jogador_hitbox_offset_x;
                float jogador_hitbox_y_real = jogador_y + jogador_hitbox_offset_y;

                for (int i = 0; i < MAX_INIMIGOS; i++) {
                    if (inimigos[i].ativo) {
                        float inimigo_hitbox_x_real = inimigos[i].x + inimigos[i].hitbox_offset_x;
                        float inimigo_hitbox_y_real = inimigos[i].y + inimigos[i].hitbox_offset_y;

                        // Verifica a colisão e se o jogador NÃO está invencível
                        if (checa_colisao(jogador_hitbox_x_real, jogador_hitbox_y_real, jogador_hitbox_largura, jogador_hitbox_altura,
                            inimigo_hitbox_x_real, inimigo_hitbox_y_real, inimigos[i].hitbox_largura, inimigos[i].hitbox_altura)
                            && !jogador_invencivel)
                        {
                            // Aplica o dano e ativa a invencibilidade
                            vida_jogador--;
                            jogador_invencivel = true;
                            timer_invencibilidade = TEMPO_INVENCIBILIDADE; // Ativa o timer
                            printf("JOGADOR ATINGIDO! Vidas restantes: %d\n", vida_jogador);

                            // KNOCKBACK - O inimigo não desaparece, é jogado para trás
                            inimigos[i].y_velocidade = -5.0f; // Joga o inimigo para cima
                            inimigos[i].no_chao = false;
                            // Empurra o inimigo para longe do jogador
                            if (inimigos[i].x < jogador_x) {
                                inimigos[i].velocidade_x = -5.0f; // Empurra para esquerda
                            }
                            else {
                                inimigos[i].velocidade_x = 5.0f; // Empurra para direita
                            }

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

                // Gatilho de mudança de fase
                if (jogador_x > 1536) {
                    switch (estado_atual) {
                    case FASE_1:
                        descarregar_fase_1();
                        estado_atual = TRANSICAO_1;
                        limpar_inimigos(inimigos);
                        carregar_transicao_1();
                        break;
                    case FASE_2:
                        descarregar_fase_2();
                        estado_atual = TRANSICAO_2;
                        limpar_inimigos(inimigos);
                        carregar_transicao_2();
                        break;
                    case FASE_3:
                        descarregar_fase_3();
                        estado_atual = TRANSICAO_3;
                        limpar_inimigos(inimigos);
                        carregar_transicao_3();
                        break;
                    case FASE_4:
                        descarregar_fase_4();
                        estado_atual = TRANSICAO_4;
                        limpar_inimigos(inimigos);
                        carregar_transicao_4();
                        break;
                    }
                }
            }
            redesenhar = true;
        }
        else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { sair = true; }


        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            // Teclado só funciona durante as fases
            if ((estado_atual >= FASE_1 && estado_atual <= FASE_4) || estado_atual == FINAL_1) {

                switch (evento.keyboard.keycode) {
                case ALLEGRO_KEY_A:
                    tecla_a = true;
                    virado_para_direita = false;
                    break;
                case ALLEGRO_KEY_D:
                    tecla_d = true;
                    virado_para_direita = true;
                    break;
                case ALLEGRO_KEY_W:
                    if (no_chao) { jogador_velocidade_y = -forca_pulo; }
                    break;
                }
            }
            // ESC para sair de qualquer tela
            if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) { sair = true; }
        }
        else if (evento.type == ALLEGRO_EVENT_KEY_UP) {
            switch (evento.keyboard.keycode) {
            case ALLEGRO_KEY_A: tecla_a = false; break;
            case ALLEGRO_KEY_D: tecla_d = false; break;
            }
        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {

            ALLEGRO_TRANSFORM transform_atual;
            al_copy_transform(&transform_atual, al_get_current_transform());
            al_invert_transform(&transform_atual);

            float mouseX_transformado = evento.mouse.x;
            float mouseY_transformado = evento.mouse.y;
            al_transform_coordinates(&transform_atual, &mouseX_transformado, &mouseY_transformado);

            switch (estado_atual) {
            case MENU:
                if (mouse_dentro_da_area(mouseX_transformado, mouseY_transformado, botao_iniciar_x1, botao_iniciar_y1, botao_iniciar_x2, botao_iniciar_y2)) {
                    descarregar_menu();
                    estado_atual = FASE_1;
                    jogador_imagem_atual = jogador_fase1; // Garante que o sprite inicial seja usado
                    carregar_fase_1(&jogador_x, &jogador_y, &virado_para_direita, &chao_y);
                }
                else if (mouse_dentro_da_area(mouseX_transformado, mouseY_transformado, botao_instrucoes_x1, botao_instrucoes_y1, botao_instrucoes_x2, botao_instrucoes_y2)) {
                    descarregar_menu();
                    estado_atual = INSTRUCOES;
                    carregar_instrucoes();
                }
                break;

            case TRANSICAO_1:
                if (mouse_dentro_da_area(mouseX_transformado, mouseY_transformado, botao_avancar_x1, botao_avancar_y1, botao_avancar_x2, botao_avancar_y2)) {
                    descarregar_transicao_1();
                    estado_atual = FASE_2;
                    jogador_imagem_atual = jogador_fase1;
                    limpar_inimigos(inimigos);
                    carregar_fase_2(&jogador_x, &jogador_y, &virado_para_direita, &chao_y, inimigos);
                }
                break;
            case TRANSICAO_2:
                if (mouse_dentro_da_area(mouseX_transformado, mouseY_transformado, botao_avancar_x1, botao_avancar_y1, botao_avancar_x2, botao_avancar_y2)) {
                    descarregar_transicao_2();
                    estado_atual = FASE_3;
                    jogador_imagem_atual = jogador_fase2;
                    limpar_inimigos(inimigos);
                    carregar_fase_3(&jogador_x, &jogador_y, &virado_para_direita, &chao_y, inimigos);
                }
                break;
            case TRANSICAO_3:
                if (mouse_dentro_da_area(mouseX_transformado, mouseY_transformado, botao_avancar_x1, botao_avancar_y1, botao_avancar_x2, botao_avancar_y2)) {
                    descarregar_transicao_3();
                    estado_atual = FASE_4;
                    jogador_imagem_atual = jogador_fase3;
                    limpar_inimigos(inimigos);
                    carregar_fase_4(&jogador_x, &jogador_y, &virado_para_direita, &chao_y, inimigos);
                }
                break;
            case TRANSICAO_4:
                if (mouse_dentro_da_area(mouseX_transformado, mouseY_transformado, botao_avancar_x1, botao_avancar_y1, botao_avancar_x2, botao_avancar_y2)) {
                    descarregar_transicao_4();
                    estado_atual = ESCOLHA;
                    limpar_inimigos(inimigos);
                    carregar_escolha();
                }
                break;

            case ESCOLHA:
                if (mouse_dentro_da_area(mouseX_transformado, mouseY_transformado, botao_final1_x1, botao_final1_y1, botao_final1_x2, botao_final1_y2)) {
                    printf("Botao para o FINAL 1 foi clicado!\n");
                    descarregar_escolha();
                    estado_atual = FINAL_1;
                    carregar_final_1();
                    jogador_imagem_atual = jogador_final1;
                    jogador_x = 1200;
                    jogador_y = 500;
                    chao_y = 940;
                    virado_para_direita = true;
                }
                else if (mouse_dentro_da_area(mouseX_transformado, mouseY_transformado, botao_final2_x1, botao_final2_y1, botao_final2_x2, botao_final2_y2)) {
                    printf("Botao para o FINAL 2 foi clicado!\n");
                    descarregar_escolha();
                    estado_atual = FINAL_2;
                    carregar_final_2();
                }
                break;

            case FINAL_2:
                if (mouse_dentro_da_area(mouseX_transformado, mouseY_transformado, botao_reiniciar_x1, botao_reiniciar_y1, botao_reiniciar_x2, botao_reiniciar_y2)) {
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
                if (mouse_dentro_da_area(mouseX_transformado, mouseY_transformado, botao_reiniciar_x1, botao_reiniciar_y1, botao_reiniciar_x2, botao_reiniciar_y2)) {
                    descarregar_gameover();
                    estado_atual = MENU;
                    carregar_menu();

                    //Reseta a vida do jogador 
                    vida_jogador = 3;
                }
                break;


                // --- LÓGICA DAS INSTRUÇÕES ---
            case INSTRUCOES:
                if (mouse_dentro_da_area(mouseX_transformado, mouseY_transformado, botao_voltar_x1, botao_voltar_y1, botao_voltar_x2, botao_voltar_y2)) {
                    descarregar_instrucoes();
                    estado_atual = MENU;
                    carregar_menu();
                }
                break;


                // --- LÓGICA DE MAGIA POR FASE ---
            case FASE_2: { // Magia de Fogo
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
                            timer_magia = COOLDOWN_MAGIA;
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
                break;
            }

            case FASE_3: { // Magia de Gelo
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
                break;
            }

            case FASE_4: { // Raio
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
                break;
            }
            }
        }
        //Desenho de tudo
        if (redesenhar && al_is_event_queue_empty(fila_eventos)) {
            redesenhar = false;

            // 1. Limpa a tela inteira com preto. 
            al_clear_to_color(al_map_rgb(0, 0, 0));

            // --- LÓGICA DE DESENHO ---
            switch (estado_atual) {
                // --- TELAS ESTÁTICAS ---
            case MENU:
            case INSTRUCOES:
            case TRANSICAO_1:
            case TRANSICAO_2:
            case TRANSICAO_3:
            case TRANSICAO_4:
            case ESCOLHA:
            case FINAL_2:
            case TELA_OBRIGADO:
                al_draw_bitmap(background_atual, 0, 0, 0);
                break;

                // --- TELAS COM JOGADOR (MAS SEM INIMIGOS/UI) ---
            case FASE_1:
                al_draw_bitmap(background_atual, 0, 0, 0);
                al_draw_bitmap(mestre_imagem, 100, chao_y - al_get_bitmap_height(mestre_imagem), 0);

                // Desenha jogador 
                int flags_f1 = virado_para_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                al_draw_bitmap(jogador_imagem_atual, jogador_x, jogador_y, flags_f1);
                float hx_f1 = jogador_x + jogador_hitbox_offset_x;
                float hy_f1 = jogador_y + jogador_hitbox_offset_y;

                // Desenha a hitbox
                al_draw_rectangle(hx_f1, hy_f1, hx_f1 + jogador_hitbox_largura, hy_f1 + jogador_hitbox_altura, al_map_rgba(255, 0, 0, 100), 1);
                break;
            case FINAL_1:
                al_draw_bitmap(background_atual, 0, 0, 0);
                int flags_final1 = virado_para_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                al_draw_bitmap(jogador_imagem_atual, jogador_x, jogador_y, flags_final1);
                
                break;

                // --- FASES COMPLETAS DE JOGABILIDADE ---
            case FASE_2:
            case FASE_3:
            case FASE_4:
                // Desenha o fundo da fase
                al_draw_bitmap(background_atual, 0, 0, 0);

                // Desenha todos os inimigos ativos
                for (int i = 0; i < MAX_INIMIGOS; i++) {
                    if (inimigos[i].ativo) {
                        
                        int flags_inimigo = inimigos[i].virado_para_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;

                        al_draw_scaled_bitmap(inimigos[i].sprite,
                            0, 0, al_get_bitmap_width(inimigos[i].sprite), al_get_bitmap_height(inimigos[i].sprite),
                            inimigos[i].x, inimigos[i].y,
                            inimigos[i].largura_desenho, inimigos[i].altura_desenho,
                            flags_inimigo); 

                        float hitbox_x = inimigos[i].x + inimigos[i].hitbox_offset_x;
                        float hitbox_y = inimigos[i].y + inimigos[i].hitbox_offset_y;
                        al_draw_rectangle(hitbox_x, hitbox_y, hitbox_x + inimigos[i].hitbox_largura, hitbox_y + inimigos[i].hitbox_altura, al_map_rgba(255, 0, 0, 100), 1);
                    }
                }

                bool desenhar_jogador = true;
                if (jogador_invencivel) {
                    // Pisca a cada 4 frames (mude '4' para mais/menos piscadas)
                    if ((timer_invencibilidade / 4) % 2 == 1) { // % 2 == 1 fará ele ficar visível/invisível
                        desenhar_jogador = false;
                    }
                }

                if (desenhar_jogador) {
                    int flags = virado_para_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                    al_draw_bitmap(jogador_imagem_atual, jogador_x, jogador_y, flags);

                    float hitbox_real_x = jogador_x + jogador_hitbox_offset_x;
                    float hitbox_real_y = jogador_y + jogador_hitbox_offset_y;

                    // Visualização da hitbox do personagem
                    al_draw_rectangle(hitbox_real_x, hitbox_real_y,
                        hitbox_real_x + jogador_hitbox_largura, hitbox_real_y + jogador_hitbox_altura,
                        al_map_rgba(255, 0, 0, 100), 1);
                }

                // Desenha todas as magias ativas 
                for (int i = 0; i < MAXIMO_DE_MAGIAS_TOTAL; i++) {
                    if (magias[i].ativa) {
                        ALLEGRO_BITMAP* sprite_magia_atual = NULL;
                        switch (magias[i].tipo_magia) {
                        case FASE_2: sprite_magia_atual = magia_fogo_img; break;
                        case FASE_3: sprite_magia_atual = magia_gelo_img; break;
                        case FASE_4: sprite_magia_atual = magia_raio_img; break;
                        }
                        if (sprite_magia_atual) {
                            int magia_flags = (magias[i].velocidade_x > 0) ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                            al_draw_bitmap(sprite_magia_atual, magias[i].x, magias[i].y, magia_flags);
                            al_draw_rectangle(magias[i].x, magias[i].y, magias[i].x + magias[i].largura, magias[i].y + magias[i].altura, al_map_rgba(0, 255, 0, 100), 1);
                        }
                    }
                }

                // Desenha os corações
                float coracao_largura_nova = 80;
                float coracao_altura_nova = 80;
                float coracao_largura_original = al_get_bitmap_width(coracao_img);
                float coracao_altura_original = al_get_bitmap_height(coracao_img);
                float espacamento = 10;
                for (int i = 0; i < vida_jogador; i++) {
                    al_draw_scaled_bitmap(coracao_img,
                        0, 0, coracao_largura_original, coracao_altura_original,
                        20 + (coracao_largura_nova + espacamento) * i, 20,
                        coracao_largura_nova, coracao_altura_nova, 0);
                }
                break;
            }

            al_flip_display();
        }
    }
    // --- Finalização ---
    if (background_atual) al_destroy_bitmap(background_atual);

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

    al_destroy_bitmap(magia_fogo_img);
    al_destroy_bitmap(magia_gelo_img);
    al_destroy_bitmap(magia_raio_img);

    al_destroy_timer(timer);
    al_destroy_event_queue(fila_eventos);
    al_destroy_display(janela);

    return 0;
}

// --- Implementação das Funções de Fase ---

void carregar_menu() {
    background_atual = al_load_bitmap("imagens/Menu.png");
    if (!background_atual) { printf("Erro ao carregar o menu\n"); exit(-1); }
}
void descarregar_menu() { al_destroy_bitmap(background_atual); background_atual = NULL; }

void carregar_instrucoes() {
    background_atual = al_load_bitmap("imagens/Instrucao.png");
    if (!background_atual) { printf("Erro ao carregar as instruções\n"); exit(-1); }
}
void descarregar_instrucoes() { al_destroy_bitmap(background_atual); background_atual = NULL; }

// FASE 1
void carregar_fase_1(float* jogador_x_ptr, float* jogador_y_ptr, bool* virado_dir_ptr, float* chao_y_ptr) {
    background_atual = al_load_bitmap("imagens/Tela_inicial.png");
    
    if (!background_atual) { printf("Erro ao carregar fundo da fase 1!\n"); exit(-1); }
    *jogador_x_ptr = 1150;
    *jogador_y_ptr = 800;
    *virado_dir_ptr = false;
    *chao_y_ptr = 990.0f;
}
void descarregar_fase_1() {
    if (background_atual) al_destroy_bitmap(background_atual);
    background_atual = NULL;
}
// FASE 2
void carregar_fase_2(float* jogador_x_ptr, float* jogador_y_ptr, bool* virado_dir_ptr, float* chao_y_ptr, Inimigo array_inimigos[]) {
    background_atual = al_load_bitmap("imagens/Floresta.png");
    if (!background_atual) { printf("Erro ao carregar fundo da fase 2!\n"); exit(-1); }

    *jogador_x_ptr = 50.0f;
    *jogador_y_ptr = 850.0f;
    *virado_dir_ptr = true;
    *chao_y_ptr = 905.0f;

    limpar_inimigos(array_inimigos); // Limpa os inimigos da fase anterior

    // --- ZERA OS CONTADORES DE SPAWN DA FASE 2 ---
    timer_spawn_slime = 0; // Define como 0 para spawnar o primeiro slime imediatamente
    slimes_spawnados = 0;
    timer_fase_2 = 0;
}

void descarregar_fase_2() {
    if (background_atual) al_destroy_bitmap(background_atual);
    background_atual = NULL;

}

// FASE 3
void carregar_fase_3(float* jogador_x_ptr, float* jogador_y_ptr, bool* virado_dir_ptr, float* chao_y_ptr, Inimigo array_inimigos[]) {
    background_atual = al_load_bitmap("imagens/Montanha.png");
    if (!background_atual) { printf("Erro ao carregar fundo da fase 3!\n"); exit(-1); }
    *jogador_x_ptr = 50.0f;
    *jogador_y_ptr = 700.0f;
    *virado_dir_ptr = true;
    *chao_y_ptr = 750.0f;

    limpar_inimigos(array_inimigos);

    // Golem de Gelo (Patrulheiro)
    spawn_inimigo(array_inimigos, sprite_golem_gelo,
        500, (*chao_y_ptr - al_get_bitmap_height(sprite_golem_gelo)), 0, true, // Posição (no chão)
        -1.0f, 0, 0,             // IA Pulo (ignorado)
        0, 1536,                 // IA Patrulha (tela inteira)
        1.0f,                      // Escala (100%)
        5, 100, 10, 105);          // Hitbox (offset 5,100 | ajuste 10,105)

    // Cyclope de Gelo (Patrulheiro)
    spawn_inimigo(array_inimigos, sprite_ice_cyclop,
        1200, (*chao_y_ptr - al_get_bitmap_height(sprite_ice_cyclop)), 0, true, // Posição (no chão)
        -1.5f, 0, 0,             // IA Pulo (ignorado)
        0, 1536,                 // IA Patrulha (tela inteira)
        1.0f,                      // Escala (100%)
        95, 5, 150, 30);           // Hitbox (offset 95,5 | ajuste 150,30)
}

void descarregar_fase_3() {
    if (background_atual) al_destroy_bitmap(background_atual);
    background_atual = NULL;
}
// FASE 4
void carregar_fase_4(float* jogador_x_ptr, float* jogador_y_ptr, bool* virado_dir_ptr, float* chao_y_ptr, Inimigo array_inimigos[]) {
    background_atual = al_load_bitmap("imagens/Vulcão.png");
    if (!background_atual) { printf("Erro ao carregar fundo da fase 4!\n"); exit(-1); }
    *jogador_x_ptr = 50.0f;
    *jogador_y_ptr = 900.0f;
    *virado_dir_ptr = true;
    *chao_y_ptr = 820.0f;

    limpar_inimigos(array_inimigos);

    // Golem de Lava (Estático)
    spawn_inimigo(array_inimigos, sprite_golem_lava,
        500, (*chao_y_ptr - al_get_bitmap_height(sprite_golem_lava)), 0, true, // Posição (no chão)
        0, 0, 0,                 // IA Pulo (sem pulo)
        0, 0,                     // IA Patrulha (sem patrulha)
        1.0f,                      // Escala (100%)
        40, 50, 140, 155);         // Hitbox (offset 40,50 | ajuste 140,155)

    // Titan de Lava (Estático)
    spawn_inimigo(array_inimigos, sprite_titan_lava,
        950, (*chao_y_ptr - al_get_bitmap_height(sprite_titan_lava)), 0, true, // Posição (no chão)
        0, 0, 0,                 // IA Pulo (sem pulo)
        0, 0,                     // IA Patrulha (sem patrulha)
        1.0f,                      // Escala (100%)
        10, 5, 20, 10);            // Hitbox (offset 10,5 | ajuste 20,10)
}

void descarregar_fase_4() {
    if (background_atual) al_destroy_bitmap(background_atual);
    background_atual = NULL;
}

void carregar_transicao_1() { background_atual = al_load_bitmap("imagens/Explicação-fogo.png"); }
void descarregar_transicao_1() { al_destroy_bitmap(background_atual); background_atual = NULL; }

void carregar_transicao_2() { background_atual = al_load_bitmap("imagens/Explicação-gelo.png"); }
void descarregar_transicao_2() { al_destroy_bitmap(background_atual); background_atual = NULL; }

void carregar_transicao_3() { background_atual = al_load_bitmap("imagens/Explicação-raio.png"); }
void descarregar_transicao_3() { al_destroy_bitmap(background_atual); background_atual = NULL; }

void carregar_transicao_4() { background_atual = al_load_bitmap("imagens/Explicação-plasma.png"); }
void descarregar_transicao_4() { al_destroy_bitmap(background_atual); background_atual = NULL; }


void carregar_escolha() {
    background_atual = al_load_bitmap("imagens/Escolha.png");
    if (!background_atual) { printf("Erro ao carregar a tela de escolha!\n"); exit(-1); }
}
void descarregar_escolha() {
    if (background_atual) al_destroy_bitmap(background_atual);
    background_atual = NULL;
}


void carregar_final_1() {
    background_atual = al_load_bitmap("imagens/Tela_final.png");
    if (!background_atual) { printf("Erro ao carregar a tela do Final 1!\n"); exit(-1); }
}
void descarregar_final_1() {
    if (background_atual) al_destroy_bitmap(background_atual);
    background_atual = NULL;
}

void carregar_final_2() {
    background_atual = al_load_bitmap("imagens/Texto_final1.png");
    if (!background_atual) { printf("Erro ao carregar a tela Final 2 Texto!\n"); exit(-1); }
}
void descarregar_final_2() {
    if (background_atual) al_destroy_bitmap(background_atual);
    background_atual = NULL;
}

void carregar_tela_obrigado() {
    background_atual = al_load_bitmap("imagens/Final_2.png");
    if (!background_atual) { printf("Erro ao carregar a tela de Obrigado!\n"); exit(-1); }
}
void descarregar_tela_obrigado() {
    if (background_atual) al_destroy_bitmap(background_atual);
    background_atual = NULL;
}

void limpar_inimigos(Inimigo array_inimigos[]) {
    for (int i = 0; i < MAX_INIMIGOS; i++) {
        array_inimigos[i].ativo = false;
    }
}

void spawn_inimigo(Inimigo array_inimigos[], ALLEGRO_BITMAP* sprite,
    float x, float y, float y_velocidade, bool no_chao,
    float vel_x, float forca_pulo, int freq_pulo,
    float p_inicio, float p_fim,
    float escala,
    float h_offset_x, float h_offset_y, float h_ajuste_w, float h_ajuste_h)
{
    // Procura o primeiro espaço vazio (inimigo inativo) no array
    for (int i = 0; i < MAX_INIMIGOS; i++) {
        if (!array_inimigos[i].ativo) {

            // Configurações visuais
            array_inimigos[i].ativo = true;
            array_inimigos[i].sprite = sprite;
            array_inimigos[i].largura_desenho = al_get_bitmap_width(sprite) * escala;
            array_inimigos[i].altura_desenho = al_get_bitmap_height(sprite) * escala;

            // Posição e Física (usa os valores exatos que você passou)
            array_inimigos[i].x = x;
            array_inimigos[i].y = y;
            array_inimigos[i].y_velocidade = y_velocidade;
            array_inimigos[i].no_chao = no_chao;
            array_inimigos[i].velocidade_x = vel_x;
            array_inimigos[i].virado_para_direita = (vel_x > 0);

            // Comportamento de IA (pulo ou patrulha)
            array_inimigos[i].forca_pulo = forca_pulo;
            array_inimigos[i].frequencia_pulo = freq_pulo;
            array_inimigos[i].timer_pulo = freq_pulo;
            array_inimigos[i].patrol_x_inicio = p_inicio;
            array_inimigos[i].patrol_x_fim = p_fim;

            // Hitbox (usa os seus valores personalizados)
            array_inimigos[i].hitbox_offset_x = h_offset_x;
            array_inimigos[i].hitbox_offset_y = h_offset_y;
            array_inimigos[i].hitbox_largura = array_inimigos[i].largura_desenho - h_ajuste_w;
            array_inimigos[i].hitbox_altura = array_inimigos[i].altura_desenho - h_ajuste_h;

            break; // Encontrou um espaço, então sai do loop
        }
    }
}

void carregar_gameover() {
    background_atual = al_load_bitmap("imagens/gameover.png");
    if (!background_atual) { printf("Erro ao carregar a tela de Game Over!\n"); exit(-1); }
}
void descarregar_gameover() {
    if (background_atual) al_destroy_bitmap(background_atual);
    background_atual = NULL;
}

bool checa_colisao(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
    // Verifica se dois retângulos (hitboxes) estão se sobrepondo.

    // Retorna 'falso' se o retângulo 1 está totalmente à direita do retângulo 2,
    // ou se o retângulo 1 está totalmente à esquerda do retângulo 2.
    if (x1 > x2 + w2 || x1 + w1 < x2) {
        return false;
    }

    // Retorna 'falso' se o retângulo 1 está totalmente abaixo do retângulo 2,
    // ou se o retângulo 1 está totalmente acima do retângulo 2.
    if (y1 > y2 + h2 || y1 + h1 < y2) {
        return false;
    }

    // Se nenhuma das condições acima for verdadeira, então eles devem estar colidindo.
    return true;
}