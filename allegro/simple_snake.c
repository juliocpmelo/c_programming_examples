#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>

#include <stdio.h>

//fps = frames per second = atualizacoes por segundo
#define FPS 5
#define LARGURA_TELA 640
#define ALTURA_TELA 480

#define	MAP_STEP 20

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

/*a snake segment, player must hold a list of these*/
typedef struct segment_t{
	int x, y;
	int w, h;
}segment;

/*food spawns at the board every second*/
typedef struct food_t{
	int x, y;
	int w, h;
}food;

/*struct player, representa a posicao do "jogador"
  por um retângulo na tela
*/
typedef struct player_t{
	int step;
	int n_segs;
	int seg_increase;
	segment *segs;
}player;

typedef struct board_t{
	food *food_list;
	int food_list_max_len;
	int food_list_len;
}board;

void move(player *p, int direction){
	printf("%d\n",p->n_segs);
	switch(direction){
		case ALLEGRO_KEY_UP:
			for(int i=1; i<p->n_segs - p->seg_increase; i++){
				p->segs[i].x = p->segs[i-1].x;
				p->segs[i].y = p->segs[i-1].y;
			}
			if(p->segs[0].y - p->step < 0 ){
				p->segs[0].y = ALTURA_TELA - p->step;
			}
			else{
				p->segs[0].y -= p->step;
			}
			break;
		case ALLEGRO_KEY_DOWN:
			for(int i=1; i<p->n_segs - p->seg_increase; i++){
				p->segs[i].x = p->segs[i-1].x;
				p->segs[i].y = p->segs[i-1].y;
			}
			if(p->segs[0].y + p->step > ALTURA_TELA ){
				p->segs[0].y = 0;
			}
			else{
				p->segs[0].y += p->step;
			}
			break;
		case ALLEGRO_KEY_LEFT:
			for(int i=1; i<p->n_segs - p->seg_increase; i++){
				p->segs[i].x = p->segs[i-1].x;
				p->segs[i].y = p->segs[i-1].y;
			}
			if(p->segs[0].x - p->step < 0 ){
				p->segs[0].x = LARGURA_TELA - p->step;
			}
			else{
				p->segs[0].x -= p->step;
			}
			break;
		case ALLEGRO_KEY_RIGHT:
			for(int i=1; i<p->n_segs - p->seg_increase; i++){
				p->segs[i].x = p->segs[i-1].x;
				p->segs[i].y = p->segs[i-1].y;
			}
			if(p->segs[0].x + p->step > LARGURA_TELA ){
				p->segs[0].x = 0;
			}
			else{
				p->segs[0].x += p->step;
			}
			break;
	}
	if(p->seg_increase > 0){
		p->seg_increase--;
	}
}

/*cria uma comida em uma localizacao aleatoria do tabuleiro*/
void spawn_food(board *b){
	if(b->food_list_max_len < b->food_list_len + 1){
		b->food_list = realloc(b->food_list, (b->food_list_max_len + 5) * sizeof(food));
		b->food_list_max_len += 5;
	}

	food f = {0,0,MAP_STEP,MAP_STEP};
	f.x = rand()%(LARGURA_TELA/MAP_STEP) * MAP_STEP;
	f.y = rand()%(ALTURA_TELA/MAP_STEP) * MAP_STEP;
	b->food_list[b->food_list_len] =f;
	b->food_list_len += 1;
}

/*verifica se a cobrinha colidiu com seu proprio corpo ou com alguma comida*/
int check_collision(board *b, player* jogador){

	player *p = jogador;

	/*cobrina vs comida*/
	for(int i=0; i<b->food_list_len; i++){
		if(p->segs[0].x == b->food_list[i].x && p->segs[0].y == b->food_list[i].y){
			for(int j=i; j<b->food_list_len-1; j++){
				b->food_list[j] = b->food_list[j+1];
			}
			b->food_list_len--;

			/*sinaliza para aumentar o tamanho da cobrinha*/	
			p->seg_increase++;
			p->n_segs++;
			p->segs = realloc(p->segs, sizeof(segment)*p->n_segs);
			p->segs[p->n_segs-1].w = MAP_STEP; 
			p->segs[p->n_segs-1].h = MAP_STEP; 
			p->segs[p->n_segs-1].x = p->segs[p->n_segs-2].x; 
			p->segs[p->n_segs-1].y = p->segs[p->n_segs-2].y; 
			break;
		}
	}

	/*cobrina vs cobrinha*/
	for(int i=1; i<p->n_segs; i++){
		if(p->segs[0].x	 == p->segs[i].x && p->segs[0].x  == p->segs[i].x)
			return 1;
	}

	return 0;
}

int main(void){
	//flag que determina quando a tela precisa ser atualizada
	int desenha = 1;
	//flag que determina quando devemos sair do loop principal
	int sair = 0;

	int last_evt = ALLEGRO_KEY_UP;


	player jogador = {MAP_STEP, 3, 0};
	jogador.segs = malloc(sizeof(segment)*jogador.n_segs);
	/*posicao inicial do jogador*/
	jogador.segs[0].x = (3+rand()%3)*MAP_STEP;
	jogador.segs[0].y = (3+rand()%3)*MAP_STEP;
	jogador.segs[0].w = MAP_STEP; 
	jogador.segs[0].h = MAP_STEP; 
	/*criando os segmentos que compoem a snake*/
	for(int i=1; i<jogador.n_segs; i++){
		jogador.segs[i].x = jogador.segs[i-1].x+MAP_STEP;
		jogador.segs[i].y = jogador.segs[i-1].y;
		jogador.segs[i].w = jogador.segs[i-1].w;
		jogador.segs[i].h = jogador.segs[i-1].h;
	}

	board b = {NULL, 0, 0};

	if (!inicializar()){
		return -1;
	}

	int count = 0;
	while(!sair){
		ALLEGRO_EVENT evento;
		al_wait_for_event(fila_eventos, &evento);

		/* -- EVENTOS -- */
		//se o tipo do evento for o disparo de um timer
		if(evento.type == ALLEGRO_EVENT_TIMER){
			desenha = 1;
			move(&jogador, last_evt);
			check_collision(&b, &jogador);
			if(count == 0){
				spawn_food(&b);
			}
			count = (count + 1)%FPS;
		}
		//se o evento for pressionar uma tecla
		if (evento.type == ALLEGRO_EVENT_KEY_DOWN){
			//verifica qual tecla foi pressionada
			switch(evento.keyboard.keycode){
				case ALLEGRO_KEY_RIGHT:
				case ALLEGRO_KEY_LEFT:
				case ALLEGRO_KEY_DOWN:
				case ALLEGRO_KEY_UP:
					last_evt = evento.keyboard.keycode;
					break;
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
			for(int i=0; i<jogador.n_segs - jogador.seg_increase; i++){
				al_draw_filled_rectangle(jogador.segs[i].x, jogador.segs[i].y, jogador.segs[i].x+jogador.segs[i].w, jogador.segs[i].y+jogador.segs[i].h, al_map_rgb(0,255,0));
			}
			for(int i=0; i<b.food_list_len; i++){
				al_draw_filled_rectangle(b.food_list[i].x, b.food_list[i].y, b.food_list[i].x+b.food_list[i].w, b.food_list[i].y+b.food_list[i].h, al_map_rgb(255,0,0));
			}
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
