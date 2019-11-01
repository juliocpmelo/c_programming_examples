#include <stdlib.h>
#include <stdio.h>

// Inclui o cabeçalho da bibilioteca Allegro 5
#include <allegro5/allegro.h>
 
// Inclui o cabeçalho do add-on para uso de primitivas
#include <allegro5/allegro_primitives.h>
 
int main(void)
{
    // Variável representando a janela principal
    ALLEGRO_DISPLAY *janela = NULL;
 
    // Variável representando a imagem
    ALLEGRO_BITMAP *imagem = NULL;
 
    // Inicializa a Allegro
    al_init();
 
		// Inicializa o add-on para utilização de primitivas
    al_init_primitives_addon();
 
    // Configura a janela
    janela = al_create_display(640, 480);
 
		int op;
		printf("Escolha qual tipo de primitiva você quer desenhar:\n");
		printf("1-Linhas\n"
					 "2-Retangulos\n"
					 "3-Circulos\n");
		scanf("%d", &op);


		switch(op){
			case 1:
			/*desenha 10 linhas em posicoes aleatorias da tela*/
			for(int i =0 ; i<10; i++){
				int x1 = rand()%641, y1 = rand()%480;
				int x2 = rand()%641, y2 = rand()%480;
				ALLEGRO_COLOR c = al_map_rgb(255,0,0);
				al_draw_line(x1, y1, x2, y2, c, 10);
			}
			break;
			case 2:
			/*desenha 10 retangulos em posicoes aleatorias da tela*/
			for(int i =0 ; i<10; i++){
				int x1 = rand()%641, y1 = rand()%480;
				int x2 = rand()%641, y2 = rand()%480;
				ALLEGRO_COLOR c = al_map_rgb(255,0,0);
				al_draw_rectangle(x1, y1, x2, y2, c, 10);
			}
			break;
			case 3:
			/*desenha 10 circulo em posicoes aleatorias da tela*/
			for(int i =0 ; i<10; i++){
				int x1 = rand()%641, y1 = rand()%480;
				int r = rand()%10 + 10; //raio entre 10 e 20 pixels
				ALLEGRO_COLOR c = al_map_rgb(0,255,0);
				al_draw_circle(x1, y1, r, c, 10);
			}
			break;
			default:
			/*cria 10 linhas em posicoes aleatorias da tela*/
			for(int i =0 ; i<10; i++){
				int x1 = rand()%641, y1 = rand()%480;
				int x2 = rand()%641, y2 = rand()%480;
				ALLEGRO_COLOR c = al_map_rgb(255,0,0);
				al_draw_line(x1, y1, x2, y2, c, 10);
			}
			break;
		}
 
    // Atualiza a tela
    al_flip_display();
 
    // Segura a execução por 10 segundos
    al_rest(10.0);
 
    // Finaliza a janela
    al_destroy_display(janela);
 
    return 0;
}
