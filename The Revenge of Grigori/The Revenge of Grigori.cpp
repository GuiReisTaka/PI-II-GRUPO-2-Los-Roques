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
    FASE_2
} EstadoJogo;

// --- Estrutura da Magia ---
typedef struct {
    float x, y;
    float velocidade_x;
    bool ativa;
} Magia;

#define MAXIMO_DE_MAGIAS 10
#define VELOCIDADE_MAGIA 12.0f

// --- Variável Global para o fundo atual ---
ALLEGRO_BITMAP* background_atual = NULL;

// --- Declaração das Funções ---
void carregar_menu();
void descarregar_menu();
void carregar_instrucoes();
void descarregar_instrucoes();
void carregar_fase_1(float* jogador_x_ptr, bool* virado_dir_ptr);
void descarregar_fase_1();
void carregar_fase_2(float* jogador_x_ptr, bool* virado_dir_ptr);
void descarregar_fase_2();

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
    ALLEGRO_BITMAP* jogador_imagem = NULL;
    ALLEGRO_BITMAP* mestre_imagem = NULL;
    ALLEGRO_BITMAP* magia_imagem = NULL;

    Magia magias[MAXIMO_DE_MAGIAS];

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
    jogador_imagem = al_load_bitmap("imagens/p1.png");
    mestre_imagem = al_load_bitmap("imagens/Mestre1.png");
    magia_imagem = al_load_bitmap("imagens/fogo.png");
    if (!jogador_imagem || !mestre_imagem || !magia_imagem) {
        printf("Erro ao carregar imagens permanentes!\n");
        return -1;
    }

    // Inicializa magias como inativas
    for (int i = 0; i < MAXIMO_DE_MAGIAS; i++) {
        magias[i].ativa = false;
    }

    // --- Registra fontes de eventos ---
    al_register_event_source(fila_eventos, al_get_display_event_source(janela));
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
    al_register_event_source(fila_eventos, al_get_mouse_event_source());

    // --- COORDENADAS DOS BOTÕES (LEIA O AVISO NO TOPO!) ---
    float botao_iniciar_x1 = 618, botao_iniciar_y1 = 400, botao_iniciar_x2 = 918, botao_iniciar_y2 = 480;
    float botao_instrucoes_x1 = 618, botao_instrucoes_y1 = 520, botao_instrucoes_x2 = 918, botao_instrucoes_y2 = 600;
    float botao_voltar_x1 = 618, botao_voltar_y1 = 800, botao_voltar_x2 = 918, botao_voltar_y2 = 880;

    // --- Variáveis do Jogo ---
    float chao_y = 990;
    float jogador_largura = al_get_bitmap_width(jogador_imagem);
    float jogador_altura = al_get_bitmap_height(jogador_imagem);
    float jogador_x = 0, jogador_y = 0; // Posições serão definidas no load de cada fase
    float jogador_velocidade_y = 0;
    float velocidade_horizontal = 5.0;
    float gravidade = 0.5;
    float forca_pulo = 10.0;
    bool no_chao = false;
    bool tecla_a = false, tecla_d = false;
    bool virado_para_direita = false;

    // --- Carrega o estado inicial (Menu) ---
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
            if (estado_atual == FASE_1 || estado_atual == FASE_2) {
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
                for (int i = 0; i < MAXIMO_DE_MAGIAS; i++) {
                    if (magias[i].ativa) {
                        magias[i].x += magias[i].velocidade_x;
                        if (magias[i].x < 0 || magias[i].x > 1536) {
                            magias[i].ativa = false;
                        }
                    }
                }

                // Gatilho de mudança de fase
                if (estado_atual == FASE_1 && jogador_x > 1536) {
                    descarregar_fase_1();
                    estado_atual = FASE_2;
                    carregar_fase_2(&jogador_x, &virado_para_direita);
                }
            }
            redesenhar = true;
        }
        else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { sair = true; }
        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            // Teclado só funciona durante as fases
            if (estado_atual == FASE_1 || estado_atual == FASE_2) {
                switch (evento.keyboard.keycode) {
                case ALLEGRO_KEY_A: tecla_a = true; virado_para_direita = false; break;
                case ALLEGRO_KEY_D: tecla_d = true; virado_para_direita = true; break;
                case ALLEGRO_KEY_W: if (no_chao) { jogador_velocidade_y = -forca_pulo; } break;
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
                    carregar_fase_1(&jogador_x, &virado_para_direita);
                }
                else if (mouse_dentro_da_area(evento.mouse.x, evento.mouse.y, botao_instrucoes_x1, botao_instrucoes_y1, botao_instrucoes_x2, botao_instrucoes_y2)) {
                    descarregar_menu();
                    estado_atual = INSTRUCOES;
                    carregar_instrucoes();
                }
                break;
            case INSTRUCOES:
                if (mouse_dentro_da_area(evento.mouse.x, evento.mouse.y, botao_voltar_x1, botao_voltar_y1, botao_voltar_x2, botao_voltar_y2)) {
                    descarregar_instrucoes();
                    estado_atual = MENU;
                    carregar_menu();
                }
                break;
            case FASE_2:
                for (int i = 0; i < MAXIMO_DE_MAGIAS; i++) {
                    if (!magias[i].ativa) {
                        magias[i].ativa = true;
                        magias[i].y = jogador_y + jogador_altura / 2;
                        if (virado_para_direita) {
                            magias[i].x = jogador_x + jogador_largura;
                            magias[i].velocidade_x = VELOCIDADE_MAGIA;
                        }
                        else {
                            magias[i].x = jogador_x;
                            magias[i].velocidade_x = -VELOCIDADE_MAGIA;
                        }
                        break;
                    }
                }
                break;
            }
        }

        if (redesenhar && al_is_event_queue_empty(fila_eventos)) {
            redesenhar = false;
            switch (estado_atual) {
            case MENU:
                al_draw_bitmap(background_atual, 0, 0, 0);
                break;
            case INSTRUCOES:
                al_draw_bitmap(background_atual, 0, 0, 0);
                al_draw_filled_rectangle(botao_voltar_x1, botao_voltar_y1, botao_voltar_x2, botao_voltar_y2, al_map_rgb(200, 0, 0)); // Botão de teste
                break;
            case FASE_1:
            case FASE_2:
                al_draw_bitmap(background_atual, 0, 0, 0);
                if (estado_atual == FASE_1) al_draw_bitmap(mestre_imagem, 100, chao_y - al_get_bitmap_height(mestre_imagem), 0);

                int flags = virado_para_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                al_draw_bitmap(jogador_imagem, jogador_x, jogador_y, flags);

                for (int i = 0; i < MAXIMO_DE_MAGIAS; i++) {
                    if (magias[i].ativa) al_draw_bitmap(magia_imagem, magias[i].x, magias[i].y, 0);
                }
                break;
            }
            al_flip_display();
        }
    }

    // --- Finalização ---
    if (background_atual) al_destroy_bitmap(background_atual);
    al_destroy_bitmap(jogador_imagem);
    al_destroy_bitmap(mestre_imagem);
    al_destroy_bitmap(magia_imagem);
    al_destroy_timer(timer);
    al_destroy_event_queue(fila_eventos);
    al_destroy_display(janela);

    return 0;
}

// --- Implementação das Funções de Fase ---

void carregar_menu() {
    background_atual = al_load_bitmap("Imagens/menu.png");
    if (!background_atual) { printf("Erro ao carregar menu.png!\n"); exit(-1); }
}
void descarregar_menu() { al_destroy_bitmap(background_atual); background_atual = NULL; }

void carregar_instrucoes() {
    background_atual = al_load_bitmap("Imagens/instrucoes.png");
    if (!background_atual) { printf("Erro ao carregar instrucoes.png!\n"); exit(-1); }
}
void descarregar_instrucoes() { al_destroy_bitmap(background_atual); background_atual = NULL; }

void carregar_fase_1(float* jogador_x_ptr, bool* virado_dir_ptr) {
    background_atual = al_load_bitmap("Imagens/Tela_inicial.png");
    if (!background_atual) { printf("Erro ao carregar fundo da fase 1!\n"); exit(-1); }
    *jogador_x_ptr = 1150;
    *virado_dir_ptr = false; // Começa virado para a esquerda, para o mestre
}
void descarregar_fase_1() { al_destroy_bitmap(background_atual); background_atual = NULL; }

void carregar_fase_2(float* jogador_x_ptr, bool* virado_dir_ptr) {
    background_atual = al_load_bitmap("Imagens/Tela 2.png");
    if (!background_atual) { printf("Erro ao carregar fundo da fase 2!\n"); exit(-1); }
    *jogador_x_ptr = 50.0f;
    *virado_dir_ptr = true; // Começa virado para a direita
}
void descarregar_fase_2() { al_destroy_bitmap(background_atual); background_atual = NULL; }