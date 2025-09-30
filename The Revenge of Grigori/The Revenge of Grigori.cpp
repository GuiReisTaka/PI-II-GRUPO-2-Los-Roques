#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

// Define as fases do jogo
typedef enum {
    FASE_1,
    FASE_2,
    FASE_3,
    FASE_4,
	FASE_5
} EstadoJogo;

// Variavel Global que guarda o background atual
ALLEGRO_BITMAP* background_atual = NULL;

// Declaração das funções
void carregar_fase_1(float* jogador_x_ptr);
void descarregar_fase_1();
void carregar_fase_2(float* jogador_x_ptr);
void descarregar_fase_2();


int main()
{
    //variaveis de representação
    ALLEGRO_DISPLAY* janela = NULL;
    ALLEGRO_EVENT_QUEUE* fila_eventos = NULL;
    ALLEGRO_TIMER* timer = NULL;
    ALLEGRO_BITMAP* jogador_imagem = NULL;
    ALLEGRO_BITMAP* mestre_imagem = NULL;

    // Variável que controla a fase atual do jogo
    EstadoJogo estado_atual = FASE_1;

    // Inicialização do Allegro e dos addons
    al_init();
    al_init_primitives_addon();
    al_init_image_addon();
    al_install_keyboard();

    // Criação da janela, timer e fila de eventos
    timer = al_create_timer(1.0 / 60.0);
    janela = al_create_display(1536, 1024);
    fila_eventos = al_create_event_queue();

    // Carrega as imagens de personagem
    jogador_imagem = al_load_bitmap("imagens/p1.png");
    mestre_imagem = al_load_bitmap("imagens/Mestre1.png");
    if (!jogador_imagem || !mestre_imagem) {
        printf("Erro ao carregar imagens permanentes!\n");
        return -1;
    }

    //Registra as informaçoes de eventos para a fila de eventos
    al_register_event_source(fila_eventos, al_get_display_event_source(janela));
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));

    // PosiçãoY do chão
    float chao_y = 990;

    // Variaveis do jogador
    float jogador_largura = al_get_bitmap_width(jogador_imagem);
    float jogador_altura = al_get_bitmap_height(jogador_imagem);
    float jogador_x = 0; 
    float jogador_y = chao_y - jogador_altura;
    float velocidade_horizontal = 5.0;

    // Variaveis do mestre (posição é sempre a mesma na fase 1)
    float mestre_x = 100;
    float mestre_y = chao_y - al_get_bitmap_height(mestre_imagem);

    // Variaveis do pulo
    float jogador_velocidade_y = 0;
    float gravidade = 0.5;
    float forca_pulo = 10.0;
    bool no_chao = false;

    // Variáveis para verificar as teclas pressionadas
    bool tecla_a = false;
    bool tecla_d = false;
    bool virado_para_direita = false;

    bool sair = false;
    bool redesenhar = true;

    //Carrega a Fase 1 antes do loop principal começar
    carregar_fase_1(&jogador_x);

    al_start_timer(timer);

    //Inicio do loop do jogo
    while (!sair)
    {
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_TIMER) {

            // Lógica de movimento do jogador (funciona em qualquer fase)
            if (tecla_a) { jogador_x -= velocidade_horizontal; }
            if (tecla_d) { jogador_x += velocidade_horizontal; }

            jogador_velocidade_y += gravidade;
            jogador_y += jogador_velocidade_y;
            no_chao = false;
            if (jogador_y + jogador_altura >= chao_y) {
                jogador_y = chao_y - jogador_altura;
                jogador_velocidade_y = 0;
                no_chao = true;
            }

            // Mudança de fase
            if (estado_atual == FASE_1) {
				// Quando o jogador ultrapassa o limite direito da Fase 1, muda para a Fase 2
                if (jogador_x > 1536) {
                    descarregar_fase_1();       
                    estado_atual = FASE_2;          
                    carregar_fase_2(&jogador_x);    
                }
            }
            // (Aqui poderíamos adicionar a lógica de transição da Fase 2 para a próxima)

            redesenhar = true;
        }
        else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { sair = true; }
        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (evento.keyboard.keycode) {
            case ALLEGRO_KEY_A: tecla_a = true; virado_para_direita = false; break;
            case ALLEGRO_KEY_D: tecla_d = true; virado_para_direita = true; break;
            case ALLEGRO_KEY_W: if (no_chao) { jogador_velocidade_y = -forca_pulo; } break;
            }
        }
        else if (evento.type == ALLEGRO_EVENT_KEY_UP) {
            switch (evento.keyboard.keycode) {
            case ALLEGRO_KEY_A: tecla_a = false; break;
            case ALLEGRO_KEY_D: tecla_d = false; break;
            case ALLEGRO_KEY_ESCAPE: sair = true; break;
            }
        }

        if (redesenhar) {
            redesenhar = false;

            // Desenha o fundo da fase em que está
            al_draw_bitmap(background_atual, 0, 0, 0);

            // Desenha o mestre na Fase 1 
            if (estado_atual == FASE_1) {
                al_draw_bitmap(mestre_imagem, mestre_x, mestre_y, 0);
            }

            // Desenha o jogador 
            if (virado_para_direita) {
                al_draw_bitmap(jogador_imagem, jogador_x, jogador_y, ALLEGRO_FLIP_HORIZONTAL);
            }
            else {
                al_draw_bitmap(jogador_imagem, jogador_x, jogador_y, 0);
            }

            al_flip_display();
        }
    }

    // Descarrega os recursos da última fase que estava ativa 
    if (estado_atual == FASE_1) {
        descarregar_fase_1();
    }
    else if (estado_atual == FASE_2) {
        descarregar_fase_2();
    }

    // Destroi os recursos "permanentes"
    al_destroy_bitmap(jogador_imagem);
    al_destroy_bitmap(mestre_imagem);
    al_destroy_timer(timer);
    al_destroy_event_queue(fila_eventos);
    al_destroy_display(janela);
    al_shutdown_primitives_addon();

    return 0;
}


void carregar_fase_1(float* jogador_x_ptr) {
    printf("Carregando recursos da Fase 1...\n");
    background_atual = al_load_bitmap("Imagens/Tela_inicial.png");
    if (!background_atual) {
        printf("Erro ao carregar fundo da fase 1!\n");
        exit(-1); 
    }
    // Posição inicial do jogador na Fase 1 
    *jogador_x_ptr = 1150;
}

void descarregar_fase_1() {
    printf("Descarregando recursos da Fase 1...\n");
    al_destroy_bitmap(background_atual);
}

void carregar_fase_2(float* jogador_x_ptr) {
    printf("Carregando recursos da Fase 2...\n");
    background_atual = al_load_bitmap("Imagens/Tela 2.png");
    if (!background_atual) {
        printf("Erro ao carregar fundo da fase 2!\n");
        exit(-1);
    }
    // Posição inicial do jogador na Fase 2 (no canto esquerdo)
    *jogador_x_ptr = 50.0f;
}

void descarregar_fase_2() {
    printf("Descarregando recursos da Fase 2...\n");
    al_destroy_bitmap(background_atual);
}