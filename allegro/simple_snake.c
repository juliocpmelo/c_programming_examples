#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>

#include <stdio.h>

//fps = frames per second = atualizacoes por segundo
#define FPS 30

/*largura e altura d atela em pixels*/
#define LARGURA_TELA 640
#define ALTURA_TELA 480


/*tamanho de discretizacao do mapa, o mapa é dividio em quadrados MAP_STEPxMAP_STEP*/
#define	MAP_STEP 20

/*um segmento da cobrinha, o jogador é composto por uma lista desses segmentos*/
typedef struct segment_t{
	int x, y;
	int w, h;
}segment;

/*comida*/
typedef struct food_t{
	int x, y;
	int w, h;
}food;

/*representa o estado "jogador", é composto por uma lista de segmentos(segs)
  step representa o quao rápido o jogador navega no tabuleiro
  n_segs o tamanho da lista de segmentos
  seg_increase e usado na logica de jogo para incrementar o tamanho da cobrinha
  ant_direction é usado para guardar a ultima direcao do movimento do jogador
*/
typedef struct player_t{
	int step;
	int n_segs;
	int seg_increase;
	int ant_direction;
	segment *segs;
}player;

/*representa o estado do "tabuleiro", é composto por uma lista de comidas (food_list) 
  food_list_max_len é usado para armazenar a quantidade maxima de comidas que o tabuleiro pode ter 
  food_list_len é usado para guardar o tamanho atual da lista de comidas
*/
typedef struct board_t{
	food *food_list;
	int food_list_max_len;
	int food_list_len;
}board;



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


/*cria estruturas básicas*/
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

/*move o jogador no tabuleiro para uma direcao especificada*/
void move(player *p, int direction){
	if(p->ant_direction == ALLEGRO_KEY_UP && direction == ALLEGRO_KEY_DOWN ||
	   p->ant_direction == ALLEGRO_KEY_DOWN && direction == ALLEGRO_KEY_UP)
		direction = p->ant_direction; //ignora se o jogador tentar fazer a cobrina andar de "ré"

	if(p->ant_direction == ALLEGRO_KEY_LEFT && direction == ALLEGRO_KEY_RIGHT ||
	  p->ant_direction == ALLEGRO_KEY_RIGHT && direction == ALLEGRO_KEY_LEFT)
		direction = p->ant_direction; //ignora se o jogador tentar fazer a cobrina andar de "ré"
	
	switch(direction){
		case ALLEGRO_KEY_UP:
			for(int i=p->n_segs - p->seg_increase; i>0; i--){
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
			for(int i=p->n_segs - p->seg_increase; i>0; i--){
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
			for(int i=p->n_segs - p->seg_increase; i>0; i--){
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
			for(int i=p->n_segs - p->seg_increase; i>0; i--){
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
	if(p->seg_increase > 0 )
		p->seg_increase--;
	p->ant_direction = direction;

}

/*checa se uma posicao é valida para comida i.e. nao tem cobrinha ou outra comida no lugar*/
int check_valid(food *f, player *jogador, board *b){
	for(int i=0; i<b->food_list_len; i++){
		if(b->food_list[i].x == f->x && b->food_list[i].y == f->y) //comida em cima de outra
			return 0;
	}
	for(int i=0; i<jogador->n_segs; i++){
		if(jogador->segs[i].x == f->x && jogador->segs[i].y == f->y) //comida dentro da cobrinha
			return 0;
	}
	return 1;
}


/*cria uma comida em uma localizacao aleatoria do tabuleiro*/
void spawn_food(board *b, player *jogador){
	/*se a lista for ter um overflow, aumenta o tamanho da lista de comidas*/	
	if(b->food_list_max_len < b->food_list_len + 1){
		b->food_list = realloc(b->food_list, (b->food_list_max_len + 5) * sizeof(food));
		b->food_list_max_len += 5;
	}

	food f = {0,0,MAP_STEP,MAP_STEP};
	f.x = rand()%(LARGURA_TELA/MAP_STEP) * MAP_STEP;
	f.y = rand()%(ALTURA_TELA/MAP_STEP) * MAP_STEP;
	
	while(!check_valid(&f, jogador, b)){
		f.x = rand()%(LARGURA_TELA/MAP_STEP) * MAP_STEP;
		f.y = rand()%(ALTURA_TELA/MAP_STEP) * MAP_STEP;
	}
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


	player jogador = {MAP_STEP, 3, 0, last_evt};
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
	double currentTime, lastUpdateTime=-1, lastFoodTime=-4;
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

		/*atualizacao dos estados do jogador e do mapa*/
		currentTime = al_get_time();
		if(currentTime - lastUpdateTime > 0.1){//atualiza a cobrinha a cada 0.1s
			move(&jogador, last_evt);
			check_collision(&b, &jogador);
			lastUpdateTime = currentTime;
		}
		if(currentTime - lastFoodTime > 3){//cria uma comida a cada 3s
			spawn_food(&b,&jogador);
			lastFoodTime = currentTime;
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
