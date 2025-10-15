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
    TELA_OBRIGADO
} EstadoJogo;

// --- Estrutura da Magia ---
typedef struct {
    float x, y;
    float velocidade_x;
    bool ativa;
    EstadoJogo tipo_magia;
} Magia;


#define MAXIMO_DE_MAGIAS_TOTAL 15 // O maior número de magias que podem existir no total

// Magia de Fogo (Fase 2)
#define MAX_MAGIAS_FOGO 10
#define VELOCIDADE_FOGO 12.0f

// Magia de Gelo 
#define MAX_MAGIAS_GELO 5
#define VELOCIDADE_GELO 8.0f 

// Magia de raio
#define MAX_MAGIAS_raio 3
#define VELOCIDADE_raio 15.0f 

// --- Variável Global para o fundo atual ---
ALLEGRO_BITMAP* background_atual = NULL;

// --- Declaração das Funções ---
void carregar_menu();
void descarregar_menu();
void carregar_instrucoes();
void descarregar_instrucoes();

void carregar_fase_1(float* jogador_x_ptr, float* jogador_y_ptr, bool* virado_dir_ptr, float* chao_y_ptr);
void descarregar_fase_1();
void carregar_fase_2(float* jogador_x_ptr, float* jogador_y_ptr, bool* virado_dir_ptr, float* chao_y_ptr);
void descarregar_fase_2();
void carregar_fase_3(float* jogador_x_ptr, float* jogador_y_ptr, bool* virado_dir_ptr, float* chao_y_ptr);
void descarregar_fase_3();
void carregar_fase_4(float* jogador_x_ptr, float* jogador_y_ptr, bool* virado_dir_ptr, float* chao_y_ptr);
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

    ALLEGRO_BITMAP* inimigo1_fase2 = NULL;
    ALLEGRO_BITMAP* inimigo2_fase2 = NULL;
    ALLEGRO_BITMAP* inimigo1_fase3 = NULL;
    ALLEGRO_BITMAP* inimigo2_fase3 = NULL;
    ALLEGRO_BITMAP* inimigo1_fase4 = NULL;
    ALLEGRO_BITMAP* inimigo2_fase4 = NULL;

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
    janela = al_create_display(1536, 1024);
    fila_eventos = al_create_event_queue();

    // --- Carrega imagens permanentes ---
    jogador_fase1 = al_load_bitmap("imagens/p1.png"); 
    jogador_fase2 = al_load_bitmap("imagens/p2.png"); 
    jogador_fase3 = al_load_bitmap("imagens/p3.png"); 
    jogador_final1 = al_load_bitmap("imagens/p4.png");
    mestre_imagem = al_load_bitmap("imagens/Mestre1.png");
    coracao_img = al_load_bitmap("imagens/Coracao-normal.png");
  
    
    inimigo1_fase2 = al_load_bitmap("imagens/slime-normal.png"); 
    inimigo2_fase2 = al_load_bitmap("imagens/slime-bravo.png");
    inimigo1_fase3 = al_load_bitmap("imagens/golem-gelo.png");
    inimigo2_fase3 = al_load_bitmap("imagens/ice-cyclope.png");
    inimigo1_fase4 = al_load_bitmap("imagens/golem-lava.png");
    inimigo2_fase4 = al_load_bitmap("imagens/titan-lava.png");

    magia_fogo_img = al_load_bitmap("imagens/Fogo.png");
    magia_gelo_img = al_load_bitmap("imagens/Gelo.png");
    magia_raio_img = al_load_bitmap("imagens/Raio.png");
    

    // Verificação de erro para todas as imagens
    if (!jogador_fase1 || !jogador_fase2 || !jogador_fase3 || !mestre_imagem || !magia_fogo_img || !magia_gelo_img || !magia_raio_img || !coracao_img ||
        !inimigo1_fase2 || !inimigo2_fase2 || !inimigo1_fase3 || !inimigo2_fase3 || !inimigo1_fase4 || !inimigo2_fase4) { 
        printf("Erro ao carregar uma ou mais imagens permanentes!\n");
        return -1;
    }

    // Define a imagem inicial a ser usada
    jogador_imagem_atual = jogador_fase1;

    // Inicializa magias como inativas
    for (int i = 0; i < MAXIMO_DE_MAGIAS_TOTAL; i++) {
        magias[i].ativa = false;
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

                // Gatilho de mudança de fase
                if (jogador_x > 1536) { 
                    switch (estado_atual) {
                    case FASE_1:
                        descarregar_fase_1();
                        estado_atual = TRANSICAO_1; 
                        carregar_transicao_1();
                        break;
                    case FASE_2:
                        descarregar_fase_2();
                        estado_atual = TRANSICAO_2; 
                        carregar_transicao_2();
                        break;
                    case FASE_3:
                        descarregar_fase_3();
                        estado_atual = TRANSICAO_3; 
                        carregar_transicao_3();
                        break;
                    case FASE_4:
                        descarregar_fase_4();
                        estado_atual = TRANSICAO_4; 
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
                    carregar_fase_2(&jogador_x, &jogador_y, &virado_para_direita, &chao_y);
                }
                break;
            case TRANSICAO_2:
                if (mouse_dentro_da_area(evento.mouse.x, evento.mouse.y, botao_avancar_x1, botao_avancar_y1, botao_avancar_x2, botao_avancar_y2)) {
                    descarregar_transicao_2();
                    estado_atual = FASE_3;
                    jogador_imagem_atual = jogador_fase2;
                    carregar_fase_3(&jogador_x, &jogador_y, &virado_para_direita, &chao_y);
                }
                break;
            case TRANSICAO_3:
                if (mouse_dentro_da_area(evento.mouse.x, evento.mouse.y, botao_avancar_x1, botao_avancar_y1, botao_avancar_x2, botao_avancar_y2)) {
                    descarregar_transicao_3();
                    estado_atual = FASE_4;
                    jogador_imagem_atual = jogador_fase3;
                    carregar_fase_4(&jogador_x, &jogador_y, &virado_para_direita, &chao_y);
                }
                break;
            case TRANSICAO_4:
                if (mouse_dentro_da_area(evento.mouse.x, evento.mouse.y, botao_avancar_x1, botao_avancar_y1, botao_avancar_x2, botao_avancar_y2)) {
                    descarregar_transicao_4();
                    estado_atual = ESCOLHA; 
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

                // Inimigo 1 - floresta
                float inimigo1_largura = 300;
                float inimigo1_altura = 265;

                al_draw_scaled_bitmap(inimigo1_fase2,
                    0, 0, al_get_bitmap_width(inimigo1_fase2), al_get_bitmap_height(inimigo1_fase2),
                    500, chao_y - inimigo1_altura,
                    inimigo1_largura, inimigo1_altura,
                    0);

				// Inimigo 2 - floresta
                float inimigo2_largura = 550;
                float inimigo2_altura = 440;
                al_draw_scaled_bitmap(inimigo2_fase2,
                    0, 0, al_get_bitmap_width(inimigo2_fase2), al_get_bitmap_height(inimigo2_fase2),
                    950, chao_y - inimigo2_altura,
                    inimigo2_largura, inimigo2_altura,
                    0);

                break;

            case FASE_3:
                al_draw_bitmap(background_atual, 0, 0, 0);
				// Inimigo 1 - gelo
                float inimigo3_largura = 300;
                float inimigo3_altura = 265;

                al_draw_scaled_bitmap(inimigo1_fase3,
                    0, 0, al_get_bitmap_width(inimigo1_fase3), al_get_bitmap_height(inimigo1_fase3),
                    500, chao_y - inimigo3_altura,
                    inimigo3_largura, inimigo3_altura,
                    0);

                // Inimigo 2 - gelo
                float inimigo4_largura = 550;
                float inimigo4_altura = 440;

                al_draw_scaled_bitmap(inimigo2_fase3,
                    0, 0, al_get_bitmap_width(inimigo2_fase3), al_get_bitmap_height(inimigo2_fase3),
                    950, chao_y - inimigo4_altura,
                    inimigo4_largura, inimigo4_altura,
                    0);
                break;

            case FASE_4:
                al_draw_bitmap(background_atual, 0, 0, 0);
				// Inimigo 1 - vulcão
                float inimigo5_largura = 405;
                float inimigo5_altura = 385;

                al_draw_scaled_bitmap(inimigo1_fase4,
                    0, 0, al_get_bitmap_width(inimigo1_fase4), al_get_bitmap_height(inimigo1_fase4),
                    500, chao_y - inimigo5_altura,
                    inimigo5_largura, inimigo5_altura,
                    0);

				// Inimigo 2 - vulcão 
                float inimigo6_largura = 900;
                float inimigo6_altura = 800;

                al_draw_scaled_bitmap(inimigo2_fase4,
                    0, 0, al_get_bitmap_width(inimigo2_fase4), al_get_bitmap_height(inimigo2_fase4),
                    800, chao_y - inimigo6_altura,
                    inimigo6_largura, inimigo6_altura,
                    0);
                break;
            }
            
            if (estado_atual == FASE_1 || estado_atual == FASE_2 || estado_atual == FASE_3 || estado_atual == FASE_4) {
               
                int flags = virado_para_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                al_draw_bitmap(jogador_imagem_atual, jogador_x, jogador_y, flags);


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
                        }
                    }
                }
            }
			// Desenha os corações 
            if (estado_atual == FASE_2 || estado_atual == FASE_3 || estado_atual == FASE_4) {
                float coracao_largura = al_get_bitmap_width(coracao_img);
                float espacamento = 10; 

                al_draw_bitmap(coracao_img, 20, 20, 0); 
                al_draw_bitmap(coracao_img, 20 + coracao_largura + espacamento, 20, 0); 
                al_draw_bitmap(coracao_img, 20 + (coracao_largura + espacamento) * 2, 20, 0); 

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

    al_destroy_bitmap(inimigo1_fase2);
    al_destroy_bitmap(inimigo2_fase2);
    al_destroy_bitmap(inimigo1_fase3);
    al_destroy_bitmap(inimigo2_fase3);
    al_destroy_bitmap(inimigo1_fase4);
    al_destroy_bitmap(inimigo2_fase4);

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
void carregar_fase_2(float* jogador_x_ptr, float* jogador_y_ptr, bool* virado_dir_ptr, float* chao_y_ptr) {
    background_atual = al_load_bitmap("imagens/Floresta.png");
    
    if (!background_atual) { printf("Erro ao carregar fundo da fase 2!\n"); exit(-1); }
    *jogador_x_ptr = 50.0f;
    *jogador_y_ptr = 850.0f;
    *virado_dir_ptr = true;
    *chao_y_ptr = 905.0f; 
}
void descarregar_fase_2() {
    if (background_atual) al_destroy_bitmap(background_atual);
    background_atual = NULL;

}

// FASE 3
void carregar_fase_3(float* jogador_x_ptr, float* jogador_y_ptr, bool* virado_dir_ptr, float* chao_y_ptr) {
    background_atual = al_load_bitmap("imagens/Montanha.png");
    
    if (!background_atual) { printf("Erro ao carregar fundo da fase 3!\n"); exit(-1); }
    *jogador_x_ptr = 50.0f;
    *jogador_y_ptr = 700.0f;
    *virado_dir_ptr = true;
    *chao_y_ptr = 750.0f; 
}

void descarregar_fase_3() {
    if (background_atual) al_destroy_bitmap(background_atual);
    background_atual = NULL;
}
// FASE 4
void carregar_fase_4(float* jogador_x_ptr, float* jogador_y_ptr, bool* virado_dir_ptr, float* chao_y_ptr) {
    background_atual = al_load_bitmap("imagens/Vulcão.png");
    
    if (!background_atual) { printf("Erro ao carregar fundo da fase 4!\n"); exit(-1); }
    *jogador_x_ptr = 50.0f;
    *jogador_y_ptr = 900.0f;
    *virado_dir_ptr = true;
    *chao_y_ptr = 820.0f; 
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