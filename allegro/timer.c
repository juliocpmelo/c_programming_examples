#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>

//fps = frames per second = atualizacoes por segundo
#define FPS 60.0
#define LARGURA_TELA 640
#define ALTURA_TELA 480

ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
//declara o timer do programa
ALLEGRO_TIMER *timer = NULL;

int min(int a, int b){
	return a < b ? a : b;
}

int max(int a, int b){
	return a > b ? a : b;
}



void error_msg(char *text){
	al_show_native_message_box(NULL,"ERRO",
			"Ocorreu o seguinte erro e o programa sera finalizado:",
			text,NULL,ALLEGRO_MESSAGEBOX_ERROR);
}

int inicializar(){
	if (!al_init()){
		error_msg("Falha ao inicializar a Allegro");
		return 0;
	}

	//inicializa addon do teclado
	if (!al_install_keyboard()){
		error_msg("Falha ao inicializar o teclado");
		return 0;
	}

	al_init_primitives_addon();

	//cria o timer com o intervalo de tempo que ele ira disparar
	timer = al_create_timer(1.0 / FPS);
	if(!timer) {
		error_msg("Falha ao criar temporizador");
		return 0;
	}

	janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
	if(!janela) {
		error_msg("Falha ao criar janela");
		al_destroy_timer(timer);
		return 0;
	}

	al_set_window_title(janela, "Utilizando timers");

	fila_eventos = al_create_event_queue();
	if(!fila_eventos) {
		error_msg("Falha ao criar fila de eventos");
		al_destroy_timer(timer);
		al_destroy_display(janela);
		return 0;
	}

	al_register_event_source(fila_eventos, al_get_display_event_source(janela));
	al_register_event_source(fila_eventos, al_get_keyboard_event_source());
	//registra o timer na fila de eventos
	al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
	al_clear_to_color(al_map_rgb(0,0,0));
	al_flip_display();
	//inicia a contagem do timer
	al_start_timer(timer);

	return 1;
}

/*struct player, representa a posicao do "jogador"
  por um retângulo na tela
*/
typedef struct player_t{
	int x, y;
	int w, h;

}player;

int main(void){
	//flag que determina quando a tela precisa ser atualizada
	int desenha = 1;
	//flag que determina quando devemos sair do loop principal
	int sair = 0;
	//vetor que guarda as componentes RGB que sera impresso no fundo da tela

	player jogador = {rand()%200, rand()%200, 20, 20};

	if (!inicializar()){
		return -1;
	}

	while(!sair){
		ALLEGRO_EVENT evento;
		al_wait_for_event(fila_eventos, &evento);

		/* -- EVENTOS -- */
		//se o tipo do evento for o disparo de um timer
		if(evento.type == ALLEGRO_EVENT_TIMER){
			desenha = 1;
		}
		//se o evento for pressionar uma tecla
		if (evento.type == ALLEGRO_EVENT_KEY_DOWN){
			//verifica qual tecla foi pressionada
			switch(evento.keyboard.keycode){
				//seta para cima
				case ALLEGRO_KEY_UP:
					jogador.y = max(0,jogador.y - 1);
					break;
					//seta para baixo
				case ALLEGRO_KEY_DOWN:
					jogador.y = min(ALTURA_TELA - jogador.h , jogador.y + 1);
					break;
					//seta para esquerda
				case ALLEGRO_KEY_LEFT:
					jogador.x = max(0,jogador.x - 1);
					break;
					//seta para direita.
				case ALLEGRO_KEY_RIGHT:
					jogador.x = min(LARGURA_TELA - jogador.w , jogador.x + 1);
					break;
					//esc. sair=1 faz com que o programa saia do loop principal
				case ALLEGRO_KEY_ESCAPE:
					sair = 1;
					break;
			}
		}
		else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			sair = 1;
		}

		/* -- ATUALIZA TELA -- */
		if(desenha && al_is_event_queue_empty(fila_eventos)) {
			/*limpa a tela*/
			al_clear_to_color(al_map_rgb(0,0,0));

			/*desenha a posicao atual do "jogador"*/
			al_draw_filled_rectangle(jogador.x, jogador.y, jogador.x+jogador.w, jogador.y+jogador.h, al_map_rgb(0,255,0));
			al_flip_display();
			//zera flag para nao entrar aqui no proximo loop
			desenha = 0;
		}
	}

	al_destroy_timer(timer);
	al_destroy_display(janela);
	al_destroy_event_queue(fila_eventos);

	return 0;
}
