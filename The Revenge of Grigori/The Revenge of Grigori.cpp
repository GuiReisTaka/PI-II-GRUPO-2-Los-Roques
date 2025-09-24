#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h> 

int main()
{
    //variaveis de representação
    ALLEGRO_DISPLAY* janela = NULL;
    ALLEGRO_EVENT_QUEUE* fila_eventos = NULL;
    ALLEGRO_TIMER* timer = NULL;
    ALLEGRO_BITMAP* background = NULL;
    ALLEGRO_BITMAP* jogador_imagem = NULL;
    ALLEGRO_BITMAP* mestre_imagem = NULL;

    // Inicialização do Allegro e dos addons
    al_init();
    al_init_primitives_addon();
    al_init_image_addon();
    al_install_keyboard();

    // Criação da janela, timer e fila de eventos
    timer = al_create_timer(1.0 / 30.0);
    janela = al_create_display(1536, 1024);
    fila_eventos = al_create_event_queue();

    //Carrega a imagem de fundo
    background = al_load_bitmap("Imagens/Tela_inicial.png");
    if (!background) {
        printf("Erro ao carregar o arquivo 'background.png'\n");
        return -1;
    }

    //Carrega a imagem do jogador
    jogador_imagem = al_load_bitmap("imagens/p1.png");
    if (!jogador_imagem) {
        printf("Erro ao carregar o arquivo 'imagens/Personagem1.png'\n");
        al_destroy_bitmap(background);
        return -1;
    }

    //Carrega a imagem do mestre
    mestre_imagem = al_load_bitmap("imagens/Mestre1.png");
    if (!mestre_imagem) {
        printf("Erro ao carregar o arquivo 'imagens/Personagem1.png'\n");
        al_destroy_bitmap(background);
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
    float jogador_x = 1150;
    float jogador_y = chao_y - jogador_altura;
    float velocidade_horizontal = 5.0;

    // Variaveis do mestre
    float mestre_x = 100;
    float mestre_y = chao_y - al_get_bitmap_height(mestre_imagem);

    // Variaveis do pulo
    float jogador_velocidade_y = 0;
    float gravidade = 0.5;
    float forca_pulo = 12.5;
    bool no_chao = false;

    // Variáveis para verificar as teclas pressionadas
    bool tecla_a = false;
    bool tecla_d = false;
    bool virado_para_direita = false;

    bool sair = false;
    bool redesenhar = true;
    // redesenha tudo 30 vezes por segundo

    al_start_timer(timer);

    //Inicio do loop do jogo
    while (!sair)
    {
        // inicia o evento
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        // Inicia o processamento do timer
        if (evento.type == ALLEGRO_EVENT_TIMER) {

            // Atualiza o movimento da direita e esquerda
            if (tecla_a) {
                jogador_x -= velocidade_horizontal;
            }
            if (tecla_d) {
                jogador_x += velocidade_horizontal;
            }


            // Velocidade da queda, considerando a gravidade
            jogador_velocidade_y += gravidade;

            // Atualiza o movimento para cima e para baixo
            jogador_y += jogador_velocidade_y;
            no_chao = false; // Assume que está no ar, até checar a colisão


            // Verifica a colisão com o chão
            if (jogador_y + jogador_altura >= chao_y) {
                jogador_y = chao_y - jogador_altura; // Corrige a posição para ficar em cima do chão
                jogador_velocidade_y = 0; // Para de cair
                no_chao = true; // Confirma que está no chão
            }

            redesenhar = true;
        }
        else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            sair = true;
        }
        // --- PROCESSAMENTO DE ENTRADA (TECLADO) ---
        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
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

                // Só pode pular se estiver no chão
                if (no_chao) {
                    jogador_velocidade_y = -forca_pulo;
                }
                break;
            }
        }
        else if (evento.type == ALLEGRO_EVENT_KEY_UP) {
            switch (evento.keyboard.keycode) {
            case ALLEGRO_KEY_A:
                tecla_a = false;
                break;
            case ALLEGRO_KEY_D:
                tecla_d = false;
                break;
            case ALLEGRO_KEY_ESCAPE:
                sair = true;
                break;
            }
        }


        if (redesenhar) {
            redesenhar = false;
            // Imagem de fundo.
            al_draw_bitmap(background, 0, 0, 0);

            // Imagem do jogador
            if (virado_para_direita) {
                // Virado para a direita
                al_draw_bitmap(jogador_imagem, jogador_x, jogador_y, ALLEGRO_FLIP_HORIZONTAL);
            }
            else {
                // Virado para a esquerda
                al_draw_bitmap(jogador_imagem, jogador_x, jogador_y, 0);
            }

            // Imagem do mestre
            al_draw_bitmap(mestre_imagem, mestre_x, mestre_y, 0);

            al_flip_display();
        }
    }

    //Destroi tudo no final
    al_destroy_bitmap(jogador_imagem);
    al_destroy_bitmap(mestre_imagem);
    al_destroy_bitmap(background);
    al_destroy_timer(timer);
    al_destroy_event_queue(fila_eventos);
    al_destroy_display(janela);
    al_shutdown_primitives_addon();


    return 0;
}