//****************Librerías*****************

#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <iostream>
#include <conio.h>
#include <ctype.h>
using namespace std;

//******************************************
//Definicion de las teclas que se utilizaran para el funcionamiento del juego
#define _ENTER 0x10d	// ENTER 
#define _IZQ  0x4b00	//Tecla izquierda
#define _DER  0x4d00	//Tecla derecha

typedef struct _bola {
	int posx, posy, vx, vy;
};
typedef struct _bloque {
	int posx, posy;
	bool activo;
	ALLEGRO_BITMAP *bitmap;
};

//**************Variables*******************

_bola bola;
_bloque Bloques[65];
_bloque plataforma_;

//DEFINICION DE PARAMETROS BASICOS
int posx_inicial = 330, posy_inicial = 517, tamanox = 45, tamanoy = 18, limite_izq = tamanox, limite_der = tamanox * 13 - 1 + limite_izq,
limite_sup = 23, limite_inf = tamanoy * 5 + 435, vidas = 1;

//***************Allegro********************

ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_BITMAP *buffer = NULL, *horizontal = NULL, *vertical = NULL, *b_morado = NULL, *b_rosa = NULL, *b_azul = NULL,
*b_naranja = NULL, *b_verde = NULL, *plataforma = NULL, *esq_si = NULL, *esq_sd = NULL, *esq_ii = NULL, *esq_id = NULL,
*marco_vida = NULL, *una_vida = NULL, *dos_vidas = NULL, *tres_vidas = NULL, *inicio = NULL, *presione = NULL;

const ALLEGRO_COLOR transparente = al_map_rgb(0, 0, 0);

//************************FUNCIONES************************

//Segun el mapa del juego
int const maxFilas = 31; //Medimos el eje x
int const maxColumnas = 31; //Medimos el eje y 

//Creación del mapa del juego
//Cada letra representa un color distinto de bloque, las esquinas, los marcos, las vidas y la plataforma

char mapa[maxFilas][maxColumnas] = {
	"stttttttttttttd",
	"cMMMMMMMMMMMMMc",
	"cRRRRRRRRRRRRRc",
	"cAAAAAAAAAAAAAc",
	"cVVVVVVVVVVVVVc",
	"cNNNNNNNNNNNNNc",
	"c             c",
	"c             c",
	"c             c",
	"c             c",
	"c             c",
	"c             c",
	"c             c",
	"c             c",
	"c             c",
	"c             c",
	"c  x          c",
	"c             c",
	"c             c",
	"c             c",
	"c             c",
	"c             c",
	"c             c",
	"c             c",
	"c             c",
	"c             c",
	"c             c",
	"c             c",
	"c             c",
	"c      P      c",
	"itttttttttttk g",
};

void inicializar() {
	bola.posx = posx_inicial;
	bola.posy = posy_inicial;
	bola.vx = 1;
	bola.vy = 1;

	for (int i = 0; i <= 4; i++) {
		for (int j = 0; j <= 12; j++) {
			Bloques[i * 13 + j].posy = i * tamanoy + limite_sup;
			Bloques[i * 13 + j].posx = j * tamanox + limite_izq;
			Bloques[i * 13 + j].activo = true;
			// Asignacion de los colores a cada bloque segun la fila
			if (i == 0) {
				Bloques[i * 13 + j].bitmap = b_morado;
			}
			else if (i == 1) {
				Bloques[i * 13 + j].bitmap = b_rosa;
			}
			else if (i == 2) {
				Bloques[i * 13 + j].bitmap = b_azul;
			}
			else if (i == 3) {
				Bloques[i * 13 + j].bitmap = b_naranja;
			}
			else {
				Bloques[i * 13 + j].bitmap = b_verde;
			}
		}
	}
	plataforma_.activo = true;
	plataforma_.posy = (limite_inf);
	plataforma_.posx = 300;
}


//Funciones para los dibujos de la pantalla


//Instruccion para que el usuario presione la X
void presioneX() {
	int fila, columna;
	for (fila = 0; fila < maxFilas; fila++) {
		for (columna = 0; columna < maxColumnas; columna++) {
			if (mapa[fila][columna] == 'x') { //Para imprimir x para continuar
				al_set_target_bitmap(buffer);
				al_draw_bitmap(presione, columna * 48, fila * 18, NULL); //estos numeros por los que se multiplica, definen el tamano de la imagen que cree

			}
		}
	}
}

void asignarVidas(int vidas) {	//Esta funcion es para evaluar la cantidad de vidas restante, y seleccionar el marco con los corazones pintados que sea apropiado
	if (vidas == 1) {
		marco_vida = una_vida;
	}

	else if (vidas == 2) {
		marco_vida = dos_vidas;
	}

	else if (vidas == 3) {
		marco_vida = tres_vidas;
	}
}

void mostrar_bola() { //analiza las coordenadas en X y en Y para pintar la bola

					  //ACTUALIZA LA POSICION DE LA BOLA
	bola.posx += bola.vx;
	bola.posy += bola.vy;
	//MUESTRA LA BOLA
	al_draw_filled_circle(bola.posx, bola.posy, 5.0, al_map_rgb_f(1.0, 0.0, 1.0));
	//REVISA SI LA BOLA SE ENCUENTRA EN LOS LIMITES DE LA PANTALLA
	if ((bola.posx >= limite_der) && (bola.vx > 0)) {
		bola.vx = -1;
	}
	if ((bola.posx <= limite_izq) && (bola.vx < 0)) {
		bola.vx = 1;
	}
	if ((bola.posy <= limite_sup) && (bola.vy < 0)) {
		bola.vy = 1;
	}
	if ((bola.posy >= limite_inf) && (bola.vy > 0)) {
		bola.vy = -1;
	}
}

void mostrar_bloques() {
	for (int i = 0; i < 65; i++) {
		if (Bloques[i].activo == true) {
			al_set_target_bitmap(buffer);
			al_draw_bitmap(Bloques[i].bitmap, Bloques[i].posx, Bloques[i].posy, NULL);
		}
	}
}

void mostrar_plataforma() {   //se deben colocar argumentos en la funcion que pinten la plataforma en otro lugar dependiendo de la tecla que se presiono
	al_set_target_bitmap(buffer);
	al_draw_bitmap(plataforma, plataforma_.posx, plataforma_.posy, NULL); //estos numeros por los que se multiplica, definen el tamano de la imagen que cree
}

//Para la pantalla inicial
void PantallaInicial() {
	al_set_target_bitmap(buffer);
	al_draw_bitmap(inicio, 1, 1, NULL);
}

void dibujarMarcos() {	//Dibuja el marco con la vida correspondiente
	int fila, columna;
	for (fila = 0; fila < maxFilas; fila++) {
		for (columna = 0; columna < maxColumnas; columna++) {
			asignarVidas(vidas); //para saber que marco se debe usar
			if (mapa[fila][columna] == 's') { //Para imprimir la esquina superior izquierda
				al_set_target_bitmap(buffer);
				al_draw_bitmap(esq_si, columna * 45, fila * 18, NULL); //estos numeros por los que se multiplica, definen el tamano de la imagen que cree

			}

			else if (mapa[fila][columna] == 'd') { //Para imprimir la esquina superior derecha
				al_set_target_bitmap(buffer);
				al_draw_bitmap(esq_sd, columna * 45, fila * 18, NULL); //estos numeros por los que se multiplica, definen el tamano de la imagen que cree

			}

			else if (mapa[fila][columna] == 'i') { //Para imprimir la esquina inferior izquierda
				al_set_target_bitmap(buffer);
				al_draw_bitmap(esq_ii, columna * 45, fila * 18, NULL); //estos numeros por los que se multiplica, definen el tamano de la imagen que cree

			}

			else if (mapa[fila][columna] == 'g') { //Para imprimir la esquina inferior derecha
				al_set_target_bitmap(buffer);
				al_draw_bitmap(esq_id, columna * 45, fila * 18, NULL); //estos numeros por los que se multiplica, definen el tamano de la imagen que cree

			}

			else if (mapa[fila][columna] == 'c') {
				al_set_target_bitmap(buffer);
				al_draw_bitmap(vertical, columna * 45, fila * 18, NULL); //lo multiplico por las dimensiones de la imagen

			}
			else if (mapa[fila][columna] == 't') { //Esto para dibujar el techo
				al_set_target_bitmap(buffer);
				al_draw_bitmap(horizontal, columna * 45, fila * 18, NULL);

			}
			else if (mapa[fila][columna] == 'k') { //Esto para dibujar el techo
				al_set_target_bitmap(buffer);
				al_draw_bitmap(marco_vida, columna * 45, fila * 18, NULL);

			}
		}
	}

}

void revisar_colision() {
	//ESTA FUNCION REVISA SI HAY COLISION CON UN BLOQUE Y LO MODIFICA .activo=FALSE
	//AUN NO GENERA EL REBOTE CON CADA BLOQUE
	for (int i = 0; i < 65; i++) {
		if ((bola.posx >= Bloques[i].posx) && (bola.posx <= (Bloques[i].posx + tamanox)) && (bola.posy >= Bloques[i].posy) && (bola.posy <= (Bloques[i].posy + tamanoy)) && (Bloques[i].activo == true)) {
			Bloques[i].activo = false;
		}
	}
}


//pantalla
//Función que se encarga de asignarle el buffer a la pantalla
void actualizar_pantalla() {
	al_set_window_title(display, "Laboratorio de Estructura de Microprocesadores - Proyecto 2");
	al_set_target_bitmap(al_get_backbuffer(display));
	al_draw_bitmap(buffer, 0, 0, 0);
	al_flip_display();  //Necesario para que actualice la interfaz gráfica en la pantalla
	al_clear_to_color(transparente); //Limpia la pantalla
}


//**************Estado principal del juego********************

int main(int argc, char **argv) {
	if (!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}

	//para manejar el teclado
	//ALLEGRO_KEYBOARD_STATE keyState;
	//ALLEGRO_TIMER *timer = al_create_timer(0.01 / 60);
	//ALLEGRO_EVENT_QUEUE *evento_cola = al_create_event_queue();
	//al_register_event_source(evento_cola, al_get_keyboard_event_source());
	//al_register_event_source(evento_cola, al_get_timer_event_source(timer));

	al_init_acodec_addon();
	al_init_image_addon(); //Necesario para que la imagen se cargue
						   //Teclado y timer
	al_init_primitives_addon();
	al_install_keyboard();
	//al_start_timer(timer);

	display = al_create_display(680, 610);	//dimensiones de la pantalla de juego x,y

	buffer = al_create_bitmap(680, 610);	//Buffer

											//Carga de imágenes
	b_morado = al_load_bitmap("b_morado.bmp");
	b_rosa = al_load_bitmap("b_rosa.bmp");
	b_verde = al_load_bitmap("b_verde.bmp");
	b_azul = al_load_bitmap("b_azul.bmp");
	b_naranja = al_load_bitmap("b_naranja.bmp");
	plataforma = al_load_bitmap("Plataforma.bmp");
	horizontal = al_load_bitmap("horizontal.png");
	vertical = al_load_bitmap("vertical.png");
	esq_si = al_load_bitmap("esq_SI.png");
	esq_id = al_load_bitmap("esq_ID.png");
	esq_ii = al_load_bitmap("esq_II.png");
	esq_sd = al_load_bitmap("esq_SD.png");
	una_vida = al_load_bitmap("una_vida.png");
	dos_vidas = al_load_bitmap("dos_vidas.png");
	tres_vidas = al_load_bitmap("tres_vidas.png");
	inicio = al_load_bitmap("inicio.png");
	presione = al_load_bitmap("presione.png");

	ALLEGRO_KEYBOARD_STATE estadoTeclado;
	ALLEGRO_TIMER *timer = al_create_timer(0.01 / 60);
	ALLEGRO_EVENT_QUEUE *evento_cola = al_create_event_queue();
	ALLEGRO_EVENT eventos;

	al_register_event_source(evento_cola, al_get_keyboard_event_source());
	al_register_event_source(evento_cola, al_get_timer_event_source(timer));

	PantallaInicial(); //aqui debemos llamar a la funcion que imprima la pantalla inicial
	actualizar_pantalla();
	//al_rest(2.0); //este al rest se debe sustituir por un: digite el nombre de usuario
				  //entonces cuando se le da ENTER dirige automaticamente a la pantalla inicial
	while (true) {
		al_wait_for_event(evento_cola, &eventos);
		al_get_keyboard_state(&estadoTeclado);
		if (al_key_down(&estadoTeclado, ALLEGRO_KEY_ENTER)) {
			inicializar();

			al_set_target_bitmap(buffer); //Selecciona el bitmap que se va a utilizar
			al_clear_to_color(transparente); //Limpia el buffer que se seleccionó
			dibujarMarcos();
			mostrar_bloques();
			mostrar_plataforma();
			mostrar_bola();
			presioneX();
			actualizar_pantalla(); //Se mantiene acá hasta presionar X


			while (true) {

				//*****************************
				//Aqui va el codigo del juego
				//*****************************

				al_wait_for_event(evento_cola, &eventos);
				al_get_keyboard_state(&estadoTeclado);
				if (al_key_down(&estadoTeclado, ALLEGRO_KEY_X)) {
					for (int h = 0; h < 5000; h++) { //cierra el juego despues de cierto momento
						al_set_target_bitmap(buffer); //Selecciona el bitmap que se va a utilizar
						al_clear_to_color(transparente); //Limpia el buffer que se seleccionó
						dibujarMarcos();
						mostrar_bloques();
						mostrar_plataforma();
						mostrar_bola();
						actualizar_pantalla();
						revisar_colision();
						al_rest(0.001);
					}

				}
			}

		}
	}

	

	//Se deben aplicar estos destroy para liberar la memoria que se utiliza en los bitmap, el display y el timer
	al_destroy_bitmap(b_morado);
	al_destroy_bitmap(b_azul);
	al_destroy_bitmap(b_rosa);
	al_destroy_bitmap(b_naranja);
	al_destroy_bitmap(b_verde);
	al_destroy_bitmap(plataforma);
	al_destroy_bitmap(horizontal);
	al_destroy_bitmap(vertical);
	al_destroy_bitmap(buffer);
	al_destroy_bitmap(esq_si);
	al_destroy_bitmap(esq_sd);
	al_destroy_bitmap(esq_ii);
	al_destroy_bitmap(esq_id);
	al_destroy_bitmap(una_vida);
	al_destroy_bitmap(dos_vidas);
	al_destroy_bitmap(tres_vidas);
	al_destroy_display(display);
	al_destroy_bitmap(inicio);
	al_destroy_bitmap(presione);
	return 0;
}

//************************************************************
