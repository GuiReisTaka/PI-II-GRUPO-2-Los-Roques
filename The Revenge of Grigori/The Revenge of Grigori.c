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

// --- Estrutura da Magia ---
typedef struct {
    float x, y;
    float velocidade_x;
    bool ativa;
    EstadoJogo tipo_magia;
    float largura; 
    float altura;
} Magia;

#define MAXIMO_DE_MAGIAS_TOTAL 15 

typedef struct {
    float x, y;                 
    ALLEGRO_BITMAP* sprite;     

    // Hitbox
    float hitbox_offset_x;
    float hitbox_offset_y;
    float hitbox_largura;
    float hitbox_altura;

    bool ativo; // Diz de o inimigo está na tela, "vivo"            
} Inimigo;

#define MAX_INIMIGOS 10

Inimigo inimigos[MAX_INIMIGOS];
ALLEGRO_BITMAP* sprite_slime_normal = NULL;
ALLEGRO_BITMAP* sprite_slime_bravo = NULL;
ALLEGRO_BITMAP* sprite_golem_gelo = NULL;
ALLEGRO_BITMAP* sprite_ice_cyclop = NULL;
ALLEGRO_BITMAP* sprite_golem_lava = NULL;
ALLEGRO_BITMAP* sprite_titan_lava = NULL;

// Magia de Fogo (Fase 2)
#define MAX_MAGIAS_FOGO 10
#define VELOCIDADE_FOGO 12.0f

// Magia de Gelo 
#define MAX_MAGIAS_GELO 5
#define VELOCIDADE_GELO 8.0f 

// Magia de raio
#define MAX_MAGIAS_raio 3
#define VELOCIDADE_raio 15.0f 

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

    float jogador_hitbox_offset_x = 10; 
    float jogador_hitbox_offset_y = 5;  
    float jogador_hitbox_largura = jogador_largura - 40; 
    float jogador_hitbox_altura = jogador_altura - 10;

    // Carrega o menu
    carregar_menu();

    al_start_timer(timer);
    bool sair = false;
    bool redesenhar = true;

    while (!sair)
    {
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_TIMER) {
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

                        if (checa_colisao(jogador_hitbox_x_real, jogador_hitbox_y_real, jogador_hitbox_largura, jogador_hitbox_altura,
                            inimigo_hitbox_x_real, inimigo_hitbox_y_real, inimigos[i].hitbox_largura, inimigos[i].hitbox_altura))
                        {
                            // --- AÇÃO DE COLISÃO DO JOGADOR ---
                            vida_jogador--; // Diminui 1 de vida
                            inimigos[i].ativo = false; // Faz o inimigo desaparecer para não dar dano contínuo

                            printf("JOGADOR ATINGIDO! Vidas restantes: %d\n", vida_jogador);

                            if (vida_jogador <= 0) {
                                printf("GAME OVER!\n");
                                estado_atual = TELA_GAMEOVER;
                                carregar_gameover();
                                limpar_inimigos(inimigos); // Limpa todos os inimigos restantes
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
                if (mouse_dentro_da_area(evento.mouse.x, evento.mouse.y, botao_iniciar_x1, botao_iniciar_y1, botao_iniciar_x2, botao_iniciar_y2)) {
                    descarregar_menu();
                    estado_atual = FASE_1;
                    jogador_imagem_atual = jogador_fase1; // Garante que o sprite inicial seja usado
                    carregar_fase_1(&jogador_x, &jogador_y, &virado_para_direita, &chao_y);
                }
                else if (mouse_dentro_da_area(evento.mouse.x, evento.mouse.y, botao_instrucoes_x1, botao_instrucoes_y1, botao_instrucoes_x2, botao_instrucoes_y2)) {
                    descarregar_menu();
                    estado_atual = INSTRUCOES;
                    carregar_instrucoes();
                }
                break;

            case TRANSICAO_1:
                if (mouse_dentro_da_area(evento.mouse.x, evento.mouse.y, botao_avancar_x1, botao_avancar_y1, botao_avancar_x2, botao_avancar_y2)) {
                    descarregar_transicao_1();
                    estado_atual = FASE_2;
                    jogador_imagem_atual = jogador_fase1;
                    limpar_inimigos(inimigos);
                    carregar_fase_2(&jogador_x, &jogador_y, &virado_para_direita, &chao_y, inimigos);
                }
                break;
            case TRANSICAO_2:
                if (mouse_dentro_da_area(evento.mouse.x, evento.mouse.y, botao_avancar_x1, botao_avancar_y1, botao_avancar_x2, botao_avancar_y2)) {
                    descarregar_transicao_2();
                    estado_atual = FASE_3;
                    jogador_imagem_atual = jogador_fase2;
                    limpar_inimigos(inimigos);
                    carregar_fase_3(&jogador_x, &jogador_y, &virado_para_direita, &chao_y, inimigos);
                }
                break;
            case TRANSICAO_3:
                if (mouse_dentro_da_area(evento.mouse.x, evento.mouse.y, botao_avancar_x1, botao_avancar_y1, botao_avancar_x2, botao_avancar_y2)) {
                    descarregar_transicao_3();
                    estado_atual = FASE_4;
                    jogador_imagem_atual = jogador_fase3;
                    limpar_inimigos(inimigos);
                    carregar_fase_4(&jogador_x, &jogador_y, &virado_para_direita, &chao_y, inimigos);
                }
                break;
            case TRANSICAO_4:
                if (mouse_dentro_da_area(evento.mouse.x, evento.mouse.y, botao_avancar_x1, botao_avancar_y1, botao_avancar_x2, botao_avancar_y2)) {
                    descarregar_transicao_4();
                    estado_atual = ESCOLHA; 
                    limpar_inimigos(inimigos);
                    carregar_escolha();
                }
                break;

            case ESCOLHA:
                if (mouse_dentro_da_area(evento.mouse.x, evento.mouse.y, botao_final1_x1, botao_final1_y1, botao_final1_x2, botao_final1_y2)) {
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
                else if (mouse_dentro_da_area(evento.mouse.x, evento.mouse.y, botao_final2_x1, botao_final2_y1, botao_final2_x2, botao_final2_y2)) {
                    printf("Botao para o FINAL 2 foi clicado!\n");
                    descarregar_escolha();
                    estado_atual = FINAL_2;
					carregar_final_2();
                }
                break;

            case FINAL_2:
                if (mouse_dentro_da_area(evento.mouse.x, evento.mouse.y, botao_para_obrigado_x1, botao_para_obrigado_y1, botao_para_obrigado_x2, botao_para_obrigado_y2)) {
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
                if (mouse_dentro_da_area(evento.mouse.x, evento.mouse.y, botao_voltar_x1, botao_voltar_y1, botao_voltar_x2, botao_voltar_y2)) {
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
            switch (estado_atual) {
            case MENU:
                al_draw_bitmap(background_atual, 0, 0, 0);
                break;
            case INSTRUCOES:
                al_draw_bitmap(background_atual, 0, 0, 0);
                break;

            case TRANSICAO_1:
            case TRANSICAO_2:
            case TRANSICAO_3:
            case TRANSICAO_4:
                al_draw_bitmap(background_atual, 0, 0, 0);
                break;

            case ESCOLHA:
                al_draw_bitmap(background_atual, 0, 0, 0);
                break;

            case FINAL_1:
                al_draw_bitmap(background_atual, 0, 0, 0);
                int flags_final1 = virado_para_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                al_draw_bitmap(jogador_imagem_atual, jogador_x, jogador_y, flags_final1);
                break;

            case FINAL_2:
                al_draw_bitmap(background_atual, 0, 0, 0);
                
                break;
            case TELA_OBRIGADO:
                al_draw_bitmap(background_atual, 0, 0, 0);
                break;

                // --- LÓGICA DE DESENHO DAS FASES ---
            case FASE_1:
                al_draw_bitmap(background_atual, 0, 0, 0);
                al_draw_bitmap(mestre_imagem, 100, chao_y - al_get_bitmap_height(mestre_imagem), 0);
                break;

            case FASE_2:
                al_draw_bitmap(background_atual, 0, 0, 0);

                break;

            case FASE_3:
                al_draw_bitmap(background_atual, 0, 0, 0);
				
                break;

            case FASE_4:
                al_draw_bitmap(background_atual, 0, 0, 0);
				
                break;

            case TELA_GAMEOVER:
                al_draw_bitmap(background_atual, 0, 0, 0);
                // Opcional: desenhe um retângulo para ver o botão de reiniciar
                // al_draw_rectangle(botao_reiniciar_x1, botao_reiniciar_y1, botao_reiniciar_x2, botao_reiniciar_y2, al_map_rgb(255, 255, 0), 2);
                break;
            }
            
            // 
            for (int i = 0; i < MAX_INIMIGOS; i++) {
                if (inimigos[i].ativo) {
                    // Desenha o sprite do inimigo
                    al_draw_bitmap(inimigos[i].sprite, inimigos[i].x, inimigos[i].y, 0);

                    // Calcula a posição real da hitbox
                    float hitbox_x = inimigos[i].x + inimigos[i].hitbox_offset_x;
                    float hitbox_y = inimigos[i].y + inimigos[i].hitbox_offset_y;

                    // Desenha a hitbox para visualização
                    al_draw_rectangle(hitbox_x, hitbox_y,
                        hitbox_x + inimigos[i].hitbox_largura, hitbox_y + inimigos[i].hitbox_altura,
                        al_map_rgba(255, 0, 0, 100), 1);
                }
            }

            if (estado_atual == FASE_1 || estado_atual == FASE_2 || estado_atual == FASE_3 || estado_atual == FASE_4) {
               
                int flags = virado_para_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                al_draw_bitmap(jogador_imagem_atual, jogador_x, jogador_y, flags);

                float hitbox_real_x = jogador_x + jogador_hitbox_offset_x;
                float hitbox_real_y = jogador_y + jogador_hitbox_offset_y;

                
                // Visualização da hitbox do personagem
                al_draw_rectangle(hitbox_real_x, hitbox_real_y,
                    hitbox_real_x + jogador_hitbox_largura, hitbox_real_y + jogador_hitbox_altura,
                    al_map_rgba(255, 0, 0, 100), 1); 
                    


               // --- LÓGICA DE DESENHO DAS MAGIAS ---
                for (int i = 0; i < MAXIMO_DE_MAGIAS_TOTAL; i++) {
                    if (magias[i].ativa) {
                        ALLEGRO_BITMAP* sprite_magia_atual = NULL;

                        // Escolhe o sprite correto baseado no tipo da magia
                        switch (magias[i].tipo_magia) {
                        case FASE_2: sprite_magia_atual = magia_fogo_img; break;
                        case FASE_3: sprite_magia_atual = magia_gelo_img; break;
                        case FASE_4: sprite_magia_atual = magia_raio_img; break;
                        }

                        if (sprite_magia_atual) {
                           
                            int magia_flags = (magias[i].velocidade_x > 0) ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                            al_draw_bitmap(sprite_magia_atual, magias[i].x, magias[i].y, magia_flags);

                            al_draw_rectangle(magias[i].x, magias[i].y,
                                magias[i].x + magias[i].largura, magias[i].y + magias[i].altura,
                                al_map_rgba(0, 255, 0, 100), 
                                1);
                        }
                    }
                }
            }
			// Desenha os corações 
            if (estado_atual == FASE_2 || estado_atual == FASE_3 || estado_atual == FASE_4) {
                float coracao_largura_nova = 80;
                float coracao_altura_nova = 80;
                float coracao_largura_original = al_get_bitmap_width(coracao_img);
                float coracao_altura_original = al_get_bitmap_height(coracao_img);
                float espacamento = 10;

                // Usa um laço para desenhar a quantidade certa de corações
                for (int i = 0; i < vida_jogador; i++) {
                    al_draw_scaled_bitmap(coracao_img,
                        0, 0, coracao_largura_original, coracao_altura_original,
                        20 + (coracao_largura_nova + espacamento) * i, 20, // A posição X muda a cada coração
                        coracao_largura_nova, coracao_altura_nova,
                        0);
                }
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

    // --- Spawna Inimigo 1 (Slime Normal) ---
    array_inimigos[0].ativo = true;
    array_inimigos[0].sprite = sprite_slime_normal; 
    array_inimigos[0].x = 500;
    array_inimigos[0].y = *chao_y_ptr - al_get_bitmap_height(sprite_slime_normal);
    // Define a hitbox (ajuste os valores para seu sprite)
    array_inimigos[0].hitbox_offset_x = 5;
    array_inimigos[0].hitbox_offset_y = 10;
    array_inimigos[0].hitbox_largura = al_get_bitmap_width(sprite_slime_normal) - 10;
    array_inimigos[0].hitbox_altura = al_get_bitmap_height(sprite_slime_normal) - 15;

    // --- Spawna Inimigo 2 (Slime Bravo) ---
    array_inimigos[1].ativo = true;
    array_inimigos[1].sprite = sprite_slime_bravo;
    array_inimigos[1].x = 950;
    array_inimigos[1].y = *chao_y_ptr - al_get_bitmap_height(sprite_slime_bravo);
    // Define a hitbox
    array_inimigos[1].hitbox_offset_x = 10;
    array_inimigos[1].hitbox_offset_y = 5;
    array_inimigos[1].hitbox_largura = al_get_bitmap_width(sprite_slime_bravo) - 20;
    array_inimigos[1].hitbox_altura = al_get_bitmap_height(sprite_slime_bravo) - 10;
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

    limpar_inimigos(array_inimigos); // Limpa os inimigos da fase anterior

    // --- Spawna Inimigo 1 (Golem de gelo) ---
    array_inimigos[0].ativo = true;
    array_inimigos[0].sprite = sprite_golem_gelo;
    array_inimigos[0].x = 500;
    array_inimigos[0].y = *chao_y_ptr - al_get_bitmap_height(sprite_golem_gelo);
    // Hitbox
    array_inimigos[0].hitbox_offset_x = 5;
    array_inimigos[0].hitbox_offset_y = 10;
    array_inimigos[0].hitbox_largura = al_get_bitmap_width(sprite_golem_gelo) - 10;
    array_inimigos[0].hitbox_altura = al_get_bitmap_height(sprite_golem_gelo) - 15;

    // --- Spawna Inimigo 2 (Cyclope de gelo) ---
    array_inimigos[1].ativo = true;
    array_inimigos[1].sprite = sprite_ice_cyclop;
    array_inimigos[1].x = 950;
    array_inimigos[1].y = *chao_y_ptr - al_get_bitmap_height(sprite_ice_cyclop);
    // Hitbox
    array_inimigos[1].hitbox_offset_x = 10;
    array_inimigos[1].hitbox_offset_y = 5;
    array_inimigos[1].hitbox_largura = al_get_bitmap_width(sprite_ice_cyclop) - 20;
    array_inimigos[1].hitbox_altura = al_get_bitmap_height(sprite_ice_cyclop) - 10;
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

    limpar_inimigos(array_inimigos); // Limpa os inimigos da fase anterior

    // --- Spawna Inimigo 1 (Golem de lava) ---
    array_inimigos[0].ativo = true;
    array_inimigos[0].sprite = sprite_golem_lava;
    array_inimigos[0].x = 500;
    array_inimigos[0].y = *chao_y_ptr - al_get_bitmap_height(sprite_golem_lava);
    // Hitbox
    array_inimigos[0].hitbox_offset_x = 5;
    array_inimigos[0].hitbox_offset_y = 10;
    array_inimigos[0].hitbox_largura = al_get_bitmap_width(sprite_golem_lava) - 10;
    array_inimigos[0].hitbox_altura = al_get_bitmap_height(sprite_golem_lava) - 15;

    // --- Spawna Inimigo 2 (Cyclope de gelo) ---
    array_inimigos[1].ativo = true;
    array_inimigos[1].sprite = sprite_titan_lava;
    array_inimigos[1].x = 950;
    array_inimigos[1].y = *chao_y_ptr - al_get_bitmap_height(sprite_titan_lava);
    // Hitbox
    array_inimigos[1].hitbox_offset_x = 10;
    array_inimigos[1].hitbox_offset_y = 5;
    array_inimigos[1].hitbox_largura = al_get_bitmap_width(sprite_titan_lava) - 20;
    array_inimigos[1].hitbox_altura = al_get_bitmap_height(sprite_titan_lava) - 10;
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

void carregar_gameover() {
    background_atual = al_load_bitmap("imagens/gameover.png");
    if (!background_atual) { printf("Erro ao carregar a tela de Game Over!\n"); exit(-1); }
}
void descarregar_gameover() {
    if (background_atual) al_destroy_bitmap(background_atual);
    background_atual = NULL;
}

bool checa_colisao(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
    // Esta função verifica se dois retângulos (hitboxes) estão se sobrepondo.

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