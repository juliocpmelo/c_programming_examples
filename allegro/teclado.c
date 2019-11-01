/*corrigido de https://aprendendoallegro.tk/teclado.php*/

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>
 
#define LARGURA_TELA 640
#define ALTURA_TELA 480
 
ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
ALLEGRO_FONT *fonte = NULL;
 
void error_msg(char *text){
	al_show_native_message_box(janela,"ERRO",
		"Ocorreu o seguinte erro e o programa sera finalizado:",
		text,NULL,ALLEGRO_MESSAGEBOX_ERROR);
}
 
int inicializar(){
    if (!al_init()){
        error_msg("Falha ao inicializar a Allegro");
        return 0;
    }
 
    al_init_font_addon();
 
    if (!al_init_ttf_addon()){
        error_msg("Falha ao inicializar add-on allegro_ttf");
        return 0;
    }
 
    if (!al_init_image_addon()){
        error_msg("Falha ao inicializar add-on allegro_image");
        return 0;
    }
 
    //inicializa addon do teclado
    if (!al_install_keyboard()){
        error_msg("Falha ao inicializar o teclado");
        return 0;
    }
 
    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!janela){
        error_msg("Falha ao criar janela");
        return 0;
    }
 
    al_set_window_title(janela, "Utilizando o Teclado");
 
    fonte = al_load_font("arial.ttf", 72, 0);
    if (!fonte){
        error_msg("Falha ao carregar \"arial.ttf\"");
        al_destroy_display(janela);
        return 0;
    }
 
    fila_eventos = al_create_event_queue();
    if (!fila_eventos){
        error_msg("Falha ao criar fila de eventos");
        al_destroy_display(janela);
        return 0;
    }
 
    //registra duas fontes de eventos na fila. o da janela, e do teclado
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_display_event_source(janela));
 
    return 1;
}
 
int main(void)
{
    int sair = 0;
    int tecla = 0;
 
    if (!inicializar()){
        return -1;
    }
 
    al_clear_to_color(al_map_rgb(255, 255, 255));
 
    while (!sair){
        while(!al_is_event_queue_empty(fila_eventos)){
            ALLEGRO_EVENT evento;
            //espera ate que algum evento esteja na fila
            al_wait_for_event(fila_eventos, &evento);
 
            //se o evento for pressionar uma tecla
            if (evento.type == ALLEGRO_EVENT_KEY_DOWN){
                //verifica qual tecla foi pressionada
                switch(evento.keyboard.keycode){
                //seta para cima
                case ALLEGRO_KEY_UP:
                    tecla = 1;
                    break;
                //seta para baixo
                case ALLEGRO_KEY_DOWN:
                    tecla = 2;
                    break;
                //seta para esquerda
                case ALLEGRO_KEY_LEFT:
                    tecla = 3;
                    break;
                //seta para direita.
                case ALLEGRO_KEY_RIGHT:
                    tecla = 4;
                    break;
                //esc. sair=1 faz com que o programa saia do loop principal
                case ALLEGRO_KEY_ESCAPE:
                    sair = 1;
                }
            }
            //se o evento foi o soltar de uma tecla
            //tecla=5 significa que alguma foi pressionada mas nao e nenhuma das setas
            else if (evento.type == ALLEGRO_EVENT_KEY_UP){
                tecla = 5;
            }
            //se clicou para fechar a janela
            else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                sair = 1;
            }
        }
 
        //tecla!=0 significa que algo foi pressionado
        if (tecla){
            al_clear_to_color(al_map_rgb(255, 255, 255));
 
            //desenha na tela as palavras relativas a seta preesionada
            switch (tecla){
            case 1:
                al_draw_text(fonte, al_map_rgb(0, 255, 0), LARGURA_TELA / 2,
                        ALTURA_TELA / 2 - al_get_font_ascent(fonte) / 2,
                        ALLEGRO_ALIGN_CENTRE, "CIMA");
                break;
            case 2:
                al_draw_text(fonte, al_map_rgb(255, 0, 255), LARGURA_TELA / 2,
                        ALTURA_TELA / 2 - al_get_font_ascent(fonte) / 2,
                        ALLEGRO_ALIGN_CENTRE, "BAIXO");
                break;
            case 3:
                al_draw_text(fonte, al_map_rgb(255, 0, 0), LARGURA_TELA / 2,
                        ALTURA_TELA / 2 - al_get_font_ascent(fonte) / 2,
                        ALLEGRO_ALIGN_CENTRE, "ESQUERDA");
                break;
            case 4:
                al_draw_text(fonte, al_map_rgb(0, 0, 255), LARGURA_TELA / 2,
                        ALTURA_TELA / 2 - al_get_font_ascent(fonte) / 2,
                        ALLEGRO_ALIGN_CENTRE, "DIREITA");
                break;
            }
 
            //zera a tecla para a proxima vez nao entrar aqui de novo
            tecla = 0;
        }
 
        al_flip_display();
    }
 
    al_destroy_display(janela);
    al_destroy_event_queue(fila_eventos);
 
    return 0;
}
