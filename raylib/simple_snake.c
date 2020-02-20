#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>

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



int min(int a, int b){
	return a < b ? a : b;
}

int max(int a, int b){
	return a > b ? a : b;
}



/*cria estruturas básicas*/
int inicializar(){

  InitWindow(LARGURA_TELA, ALTURA_TELA, "raylib simple snake");
  SetTargetFPS(FPS);               // Set our game to run at 60 frames-per-second

	return 1;
}

/*move o jogador no tabuleiro para uma direcao especificada*/
void move(player *p, int direction){
	if(p->ant_direction == KEY_UP && direction == KEY_DOWN ||
	   p->ant_direction == KEY_DOWN && direction == KEY_UP)
		direction = p->ant_direction; //ignora se o jogador tentar fazer a cobrina andar de "ré"

	if(p->ant_direction == KEY_LEFT && direction == KEY_RIGHT ||
	  p->ant_direction == KEY_RIGHT && direction == KEY_LEFT)
		direction = p->ant_direction; //ignora se o jogador tentar fazer a cobrina andar de "ré"
	
	switch(direction){
		case KEY_UP:
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
		case KEY_DOWN:
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
		case KEY_LEFT:
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
		case KEY_RIGHT:
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

	int last_evt = KEY_UP;


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
  while (!WindowShouldClose()){    // Detect window close button or ESC key

		/* -- EVENTOS -- */
		//se o evento for pressionar uma tecla
		if (IsKeyDown(KEY_RIGHT)) last_evt = KEY_RIGHT;
		if (IsKeyDown(KEY_LEFT)) last_evt = KEY_LEFT;
		if (IsKeyDown(KEY_UP)) last_evt = KEY_UP;
		if (IsKeyDown(KEY_DOWN)) last_evt = KEY_DOWN;
	
		/*atualizacao dos estados do jogador e do mapa*/
		currentTime = GetTime();
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
		BeginDrawing();
			/*limpa a tela*/
			ClearBackground(BLACK);

			/*desenha a posicao atual do "jogador"*/
			for(int i=0; i<jogador.n_segs - jogador.seg_increase; i++){
				DrawRectangle(jogador.segs[i].x, jogador.segs[i].y, jogador.segs[i].w, jogador.segs[i].h, GREEN);
			}
			for(int i=0; i<b.food_list_len; i++){
				DrawRectangle(b.food_list[i].x, b.food_list[i].y, b.food_list[i].w, b.food_list[i].h, RED);
			}

		EndDrawing();

	}
    
	CloseWindow();        // Close window and OpenGL context

	return 0;
}
