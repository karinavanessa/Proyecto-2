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
typedef struct _game {
	bool activo2;
	bool activo3;
};
typedef struct _plataforma {
	int pospx, pospy;
};


//**************Variables*******************

_bola bola;
_bloque Bloques[65];
_bloque plataforma_;
_game juego_;
_plataforma plataforma2;

//DEFINICION DE PARAMETROS BASICOS
int posx_inicial = 330, posy_inicial = 517, ancho_bloque = 45, altura_bloque = 18, limite_izq = ancho_bloque, limite_der = ancho_bloque * 13 - 1 + limite_izq,
limite_sup = altura_bloque, limite_inf = altura_bloque * 5 + 435, vidas = 4, altura_inicial_bloques = 30, FPS = 60, cont_final = 0;
int const maxFilas = 31, maxColumnas = 31;
char opc, plat = 201;
bool juego;

//***************Allegro********************

ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_BITMAP *buffer = NULL, *horizontal = NULL, *vertical = NULL, *b_morado = NULL, *b_rosa = NULL, *b_azul = NULL,
*b_naranja = NULL, *b_verde = NULL, *plataforma = NULL, *esq_si = NULL, *esq_sd = NULL, *esq_ii = NULL, *esq_id = NULL,
*marco_vida = NULL, *perdio= NULL, *gano = NULL, *final_screen = NULL, *fondo = NULL, *cero_vidas = NULL, *una_vida = NULL, *dos_vidas = NULL, *tres_vidas = NULL, *inicio = NULL, *presione = NULL;

const ALLEGRO_COLOR transparente = al_map_rgb(0, 0, 0);
const ALLEGRO_COLOR verde = al_map_rgb(0, 1, 0);

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

//************************FUNCIONES************************

void inicializar() {
	bola.posx = posx_inicial;
	bola.posy = posy_inicial;
	bola.vx = 1;
	bola.vy = 1;
	plataforma2.pospx = 300;
	plataforma2.pospy = limite_inf;

	for (int i = 0; i <= 4; i++) {
		for (int j = 0; j <= 12; j++) {
			Bloques[i * 13 + j].posy = i * altura_bloque + altura_inicial_bloques;
			Bloques[i * 13 + j].posx = j * ancho_bloque + limite_izq;
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
	juego_.activo2 = true;
}



void mostrar_plataforma(/*int pospx, int pospy*/) {   //se deben colocar argumentos en la funcion que pinten la plataforma en otro lugar dependiendo de la tecla que se presiono
	opc = 'c';
	ALLEGRO_KEYBOARD_STATE estadoTeclado;
	al_get_keyboard_state(&estadoTeclado);
	if (al_key_down(&estadoTeclado, ALLEGRO_KEY_LEFT)) {
		if (plataforma2.pospx > limite_izq) {
			plataforma2.pospx = plataforma2.pospx - 1;
			plataforma_.posy = (limite_inf);
		}
		if (plataforma2.pospx <= limite_izq) {
			plataforma2.pospx = plataforma2.pospx;
			plataforma_.posy = (limite_inf);
			plataforma_.posx = plataforma2.pospx;
		}
	}
	if (al_key_down(&estadoTeclado, ALLEGRO_KEY_RIGHT)) {
		if (plataforma2.pospx >= limite_der - 3) {
			plataforma2.pospx = plataforma2.pospx;
			plataforma_.posy = (limite_inf);
			plataforma_.posx = plataforma2.pospx;
		}
		if (plataforma2.pospx < 580) {
			plataforma2.pospx = plataforma2.pospx + 1;
			plataforma_.posy = (limite_inf);
			plataforma_.posx = plataforma2.pospx;
		}
	}
	if (opc != 'a' && opc != 'd') {
		//MUESTRA LA BOLA
		plataforma_.posy = (limite_inf);
		plataforma_.posx = plataforma2.pospx;
	}
	al_set_target_bitmap(buffer);
	al_draw_bitmap(plataforma, plataforma_.posx, plataforma_.posy, NULL); //estos numeros por los que se multiplica, definen el tamano de la imagen que cree
}


void colision_plataforma() {
	plataforma_.posx = plataforma2.pospx;
	if (bola.posy == limite_inf) {
		vidas = vidas;
		if ((bola.posx >= (plataforma2.pospx - 31)) && (bola.posx <= (plataforma2.pospx + 31))) {
			vidas = vidas;
		}
		else if ((bola.posx < (plataforma2.pospx - 31)) || (bola.posx >(plataforma2.pospx + 31))) {
			vidas = vidas - 1;

			if (vidas == 0) {
				juego_.activo2 = false;
				
			}
			else if (vidas != 0) {
				juego_.activo2 = true;
			}
		}
	}
	else if (bola.posy != limite_inf) {
		vidas = vidas;
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
	else if (vidas == 4) {
		marco_vida = tres_vidas;
	}
	else if (vidas == 0) {
		marco_vida = cero_vidas;
	}
}

void mostrar_bola() { //analiza las coordenadas en X y en Y para pintar la bola

					  //ACTUALIZA LA POSICION DE LA BOLA
	bola.posx += bola.vx;
	bola.posy += bola.vy;
	//MUESTRA LA BOLA
	al_draw_filled_circle(bola.posx, bola.posy, 5.0, al_map_rgb_f(0.5, 0.5, 1.0));
	//REVISA SI LA BOLA SE ENCUENTRA EN LOS LIMITES DE LA PANTALLA
	if ((bola.posx >= limite_der) && (bola.vx > 0)) {
		bola.vx = bola.vx*-1;
	}
	if ((bola.posx <= limite_izq) && (bola.vx < 0)) {
		bola.vx = bola.vx*-1;
	}
	if ((bola.posy <= limite_sup) && (bola.vy < 0)) {
		bola.vy = bola.vy*-1;
	}
	if ((bola.posy >= limite_inf) && (bola.vy > 0)) {
		bola.vy = bola.vy*-1;
	}
}

void mostrar_bloques(int cont_final) {
	cont_final = 0;
	for (int i = 0; i < 65; i++) {
		if (Bloques[i].activo == true) {
			al_set_target_bitmap(buffer);
			al_draw_bitmap(Bloques[i].bitmap, Bloques[i].posx, Bloques[i].posy, NULL);
			cont_final = cont_final + 1;
		}
	}
	if (cont_final == 1) {
		juego_.activo3 = false;
	}
	else if (cont_final != 1) {
		juego_.activo3 = true;
	}
}

bool revisar_final() {
	// ESTA FUNCION REVISA SI TODOS LOS BLOQUES ESTAN .activo=FALSE
	int end = 0;
	for (int k = 0; k < 65; k++) {
		if (Bloques[k].activo == true) {
			end++;
		}
	}
	if (end == 0) { return true; }
	else { return false; }
}

void PantallaInicial() {
	al_set_target_bitmap(buffer);
	al_draw_bitmap(inicio, 1, 1, NULL);
}

void dibujarFondo() {
	al_set_target_bitmap(buffer);
	al_draw_bitmap(fondo, 1, 1, NULL);
}

void ganoJuego() {
	al_set_target_bitmap(buffer);
	al_draw_bitmap(gano, 150, 200, NULL);
}

void perdioJuego() {
	al_set_target_bitmap(buffer);
	al_draw_bitmap(perdio, 150, 200, NULL);
}

void pantallaFinal() {
	al_set_target_bitmap(buffer);
	al_draw_bitmap(final_screen, 1, 1, NULL);
}
void revisar_colision() {

	for (int i = 0; i < 65; i++) {
		if ((bola.posx + bola.vx >= Bloques[i].posx) && (bola.posx + bola.vx <= (Bloques[i].posx + ancho_bloque - 1)) &&
			(bola.posy + bola.vy >= Bloques[i].posy) && (bola.posy + bola.vy <= (Bloques[i].posy + altura_bloque - 1)) && (Bloques[i].activo == true)) {
			Bloques[i].activo = false;

			if (((bola.posx + bola.vx == Bloques[i].posx) || (bola.posx + bola.vx == Bloques[i].posx + ancho_bloque - 1)) &&
				(bola.posy + bola.vy >= Bloques[i].posy) && (bola.posy + bola.vy <= Bloques[i].posy + altura_bloque)) {
				bola.vx = bola.vx*-1;
			}
			else if (((bola.posy + bola.vy == Bloques[i].posy) || (bola.posy + bola.vy == Bloques[i].posy + altura_bloque - 1)) &&
				(bola.posx + bola.vx >= Bloques[i].posx) && (bola.posx + bola.vx <= Bloques[i].posx + ancho_bloque)) {
				bola.vy = bola.vy*-1;
			}
		}
	}
}

void presioneX() {
	int fila, columna;
	for (fila = 0; fila < maxFilas; fila++) {
		for (columna = 0; columna < maxColumnas; columna++) {
			if (mapa[fila][columna] == 'x') { //Para imprimir x para continuar
				al_set_target_bitmap(buffer);
				al_draw_bitmap(presione, columna * 50, fila * 18, NULL); //estos numeros por los que se multiplica, definen el tamano de la imagen que cree

			}
		}
	}
}

void dibujarMarcos() {	//Dibuja el marco con la vida correspondiente
	int fila, columna;
	for (fila = 0; fila < maxFilas; fila++) {
		for (columna = 0; columna < maxColumnas; columna++) {
			asignarVidas(vidas); //para saber que marco se debe usar
			if (mapa[fila][columna] == 's') { //Para imprimir la esquina superior izquierda
				al_set_target_bitmap(buffer);
				al_draw_bitmap(esq_si, columna * ancho_bloque, fila * altura_bloque, NULL);
			}
			else if (mapa[fila][columna] == 'd') { //Para imprimir la esquina superior derecha
				al_set_target_bitmap(buffer);
				al_draw_bitmap(esq_sd, columna * ancho_bloque, fila * altura_bloque, NULL);
			}
			else if (mapa[fila][columna] == 'i') { //Para imprimir la esquina inferior izquierda
				al_set_target_bitmap(buffer);
				al_draw_bitmap(esq_ii, columna * ancho_bloque, fila * altura_bloque, NULL);
			}
			else if (mapa[fila][columna] == 'g') { //Para imprimir la esquina inferior derecha
				al_set_target_bitmap(buffer);
				al_draw_bitmap(esq_id, columna * ancho_bloque, fila * altura_bloque, NULL);
			}
			else if (mapa[fila][columna] == 'c') { //Esto para dibujar la pared
				al_set_target_bitmap(buffer);
				al_draw_bitmap(vertical, columna * ancho_bloque, fila * altura_bloque, NULL);
			}
			else if (mapa[fila][columna] == 't') { //Esto para dibujar el techo
				al_set_target_bitmap(buffer);
				al_draw_bitmap(horizontal, columna * ancho_bloque, fila * altura_bloque, NULL);
			}
			else if (mapa[fila][columna] == 'k') { //Esto para dibujar el techo
				al_set_target_bitmap(buffer);
				al_draw_bitmap(marco_vida, columna * ancho_bloque, fila * altura_bloque, NULL);
			}
		}
	}

}

void actualizar_pantalla() {
	//pantalla
	//Función que se encarga de asignarle el buffer a la pantalla
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
	al_init_image_addon();
	al_init_primitives_addon();
	al_install_keyboard();
	//al_start_timer(timer);

	display = al_create_display(680, 600);	//dimensiones de la pantalla de juego x,y
	buffer = al_create_bitmap(680, 600);	//Buffer

											//Carga de imágenes
	b_morado = al_load_bitmap("b_morado.png"); b_rosa = al_load_bitmap("b_rosa.png"); b_verde = al_load_bitmap("b_verde.png"); b_azul = al_load_bitmap("b_azul.png");
	b_naranja = al_load_bitmap("b_naranja.png"); plataforma = al_load_bitmap("Plataforma.png"); horizontal = al_load_bitmap("horizontal.png");
	vertical = al_load_bitmap("vertical.png"); esq_si = al_load_bitmap("esq_SI.png"); esq_id = al_load_bitmap("esq_ID.png"); esq_ii = al_load_bitmap("esq_II.png");
	esq_sd = al_load_bitmap("esq_SD.png"); una_vida = al_load_bitmap("una_vida.png"); dos_vidas = al_load_bitmap("dos_vidas.png");
	tres_vidas = al_load_bitmap("tres_vidas.png"); cero_vidas = al_load_bitmap("cero_vidas.png"); perdio = al_load_bitmap("perdio.png"); final_screen = al_load_bitmap("final.png"); gano = al_load_bitmap("gano.png"); fondo = al_load_bitmap("fondo.png"); inicio = al_load_bitmap("inicio.png"); presione = al_load_bitmap("presione.png");

	ALLEGRO_KEYBOARD_STATE estadoTeclado;
	ALLEGRO_TIMER *timer = al_create_timer(1 / FPS);
	ALLEGRO_EVENT_QUEUE *evento_cola = al_create_event_queue();
	ALLEGRO_EVENT eventos;
	al_register_event_source(evento_cola, al_get_keyboard_event_source());
	//al_register_event_source(evento_cola, al_get_timer_event_source(timer));
	//al_register_event_source(evento_cola, al_get_display_event_source(display));


	//COMIENZA PANTALLA INICIAL Y EL CICLO DEL JUEGO

	PantallaInicial(); //aqui debemos llamar a la funcion que imprima la pantalla inicial
	actualizar_pantalla();

	//INICIALIZACION DE LOS BLOQUES, BOLA Y PLATAFORMA

	//Ciclo que me saca del ENTER
	bool ciclo = 1;

	while (ciclo) {
		al_wait_for_event(evento_cola, &eventos);
		al_get_keyboard_state(&estadoTeclado);
		if (al_key_down(&estadoTeclado, ALLEGRO_KEY_ENTER)) {
			ciclo = 0;
		}
	}

	inicializar();
	al_set_target_bitmap(buffer); //Selecciona el bitmap que se va a utilizar
	al_clear_to_color(transparente); //Limpia el buffer que se selecciono
	dibujarFondo();
	dibujarMarcos();
	mostrar_bloques(cont_final);
	mostrar_plataforma(/*plataforma2.pospx, plataforma2.pospy*/);
	mostrar_bola();
	presioneX();
	actualizar_pantalla(); //Se mantiene acá hasta presionar X

						   //Evento presionar X
	ciclo = 1;
	while (ciclo) {
		al_wait_for_event(evento_cola, &eventos);
		al_get_keyboard_state(&estadoTeclado);
		if (al_key_down(&estadoTeclado, ALLEGRO_KEY_X)) {
			ciclo = 0;
		}
	}
	
	while (!revisar_final()) {
		//LIMPIAR PANTALLA
		al_set_target_bitmap(buffer); //Selecciona el bitmap que se va a utilizar
		al_clear_to_color(transparente); //Limpia el buffer que se seleccionó
										 //PINTAR OBJETOS
		dibujarFondo();
		mostrar_bloques(cont_final);
		dibujarMarcos();
		colision_plataforma();
		mostrar_bola();
		mostrar_plataforma();
		//EJECUCION DE LA LOGICA Y ACTUALIZAR LA PANTALLA
		revisar_colision();
		actualizar_pantalla();
		al_rest(0.0001);
		if (!juego_.activo2) {
			//si esta variable es falsa, es porque se destruyeron todos los bloques
			dibujarFondo();
			dibujarMarcos();
			mostrar_plataforma();
			perdioJuego();
			actualizar_pantalla();
			break;
		}

	}

	//En cualquiera de los casos

	//Esperar el ENTER para continuar a la pantalla final
	ciclo = 1;
	while (ciclo) {
		al_wait_for_event(evento_cola, &eventos);
		al_get_keyboard_state(&estadoTeclado);
		if (al_key_down(&estadoTeclado, ALLEGRO_KEY_ENTER)) {
			ciclo = 0;
		}
	}

	//llamar a la funcion que imprime la pantalla final

	//pantallaFinal();
	//actualizar_pantalla();
	ciclo = 1;
	while (ciclo) {
		pantallaFinal();
		actualizar_pantalla();
		al_rest(1.0);
		al_wait_for_event(evento_cola, &eventos);
		al_get_keyboard_state(&estadoTeclado);
		if (al_key_down(&estadoTeclado, ALLEGRO_KEY_ENTER)) {
			ciclo = 0;
		}
	}


	//Se deben aplicar estos destroy para liberar la memoria que se utiliza en los bitmap, el display y el timer
	al_destroy_bitmap(b_morado); al_destroy_bitmap(b_azul); al_destroy_bitmap(b_rosa); al_destroy_bitmap(b_naranja); al_destroy_bitmap(b_verde);
	al_destroy_bitmap(plataforma); al_destroy_bitmap(horizontal); al_destroy_bitmap(vertical); al_destroy_bitmap(buffer); al_destroy_bitmap(esq_si);
	al_destroy_bitmap(esq_sd); al_destroy_bitmap(esq_ii); al_destroy_bitmap(esq_id); al_destroy_bitmap(fondo); al_destroy_bitmap(una_vida); al_destroy_bitmap(dos_vidas);
	al_destroy_bitmap(tres_vidas); al_destroy_bitmap(cero_vidas); al_destroy_bitmap(perdio); al_destroy_bitmap(final_screen); al_destroy_bitmap(gano); al_destroy_display(display); al_destroy_bitmap(inicio); al_destroy_bitmap(presione); al_destroy_timer(timer);
	al_destroy_event_queue(evento_cola);
	return 0;
}

//************************************************************
