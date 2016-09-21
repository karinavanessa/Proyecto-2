//****************Librerías*****************

#include <stdio.h>
#include <allegro5\allegro.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_image.h>
#include <allegro5\allegro_ttf.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_audio.h>
#include <allegro5\allegro_acodec.h>
#include <allegro5\allegro_native_dialog.h>
#include <iostream>
#include <conio.h>
#include <ctype.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
using namespace std;

//******************************************
//Definicion de las teclas que se utilizaran para el funcionamiento del juego
#define _ENTER 0x10d	// ENTER 
#define _IZQ  0x4b00	//Tecla izquierda
#define _DER  0x4d00	//Tecla derecha

struct _bola {
	float posx, posy, vx, vy;
};
struct _bloque {
	float posx, posy;
	bool activo;
	ALLEGRO_BITMAP *bitmap;
};

//**************Variables*******************

_bola bola;
_bloque Bloques[65];
_bloque plataforma_;

//DEFINICION DE PARAMETROS BASICOS
int cont = 250;
int userPos = 9; //esto es para ubicar la posicion en la que se imprimira el nombre de usuario por debajo de la pantalla
int posx_inicial = 330, posy_inicial = 517, ancho_bloque = 45, altura_bloque = 18, limite_izq = ancho_bloque, limite_der = ancho_bloque * 13 - 1 + limite_izq,
limite_sup = altura_bloque, limite_inf = altura_bloque * 5 + 435, vidas = 3, altura_inicial_bloques = 30, FPS = 500, cont_final = 0;
int const maxFilas = 31, maxColumnas = 31;
unsigned char opc, plat = 201;	//para que no de problemas se coloca el unsigned, de lo contrario se genera un warning pues se interpreta un numero negativo
bool intento = false;	//esto es para evaluar si se perdio una vida
								//***************Allegro********************
bool reset, salir = false;	//Estas variables eventualmente se reemplazan con la senal proveniente de los botones

char username[33] = "Usuario:                        ";

ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_BITMAP *buffer = NULL, *horizontal = NULL, *vertical = NULL, *b_morado = NULL, *b_rosa = NULL, *b_azul = NULL,
*b_naranja = NULL, *b_verde = NULL, *plataforma = NULL, *esq_si = NULL, *esq_sd = NULL, *esq_ii = NULL, *esq_id = NULL,
*marco_vida = NULL, *fallido = NULL, *perdio = NULL, *gano = NULL, *final_screen = NULL, *fondo = NULL, *cero_vidas = NULL, *una_vida = NULL, *dos_vidas = NULL, *tres_vidas = NULL, *inicio = NULL, *presione = NULL;

ALLEGRO_FONT *fuente = al_load_font("stencil.ttf", 45, 0);
ALLEGRO_FONT *user = al_load_font("courier.ttf", 23, 0);

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

	srand(time(0));
	bola.vx = rand() % 128;
	bola.vx = (bola.vx + 12) / 100;
	//cout << bola.vx << endl;

	bola.vy = sqrt(2 - bola.vx*bola.vx); // La magnitud de la velocidad siempre sera sqrt(2)=1.4142
	//cout << bola.vy;

	plataforma_.posx = 300;
	plataforma_.posy = limite_inf;
	plataforma_.activo = true;
	plataforma_.bitmap = plataforma;

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
}

void cargaArchivos() {
	//librerias necesarias para inicializar el codigo
	al_init_acodec_addon();
	al_init_image_addon();
	al_init_font_addon(); //Esta y la siguiente son para lograr mostrar texto en pantalla en Allegro
	al_init_ttf_addon();
	al_init_primitives_addon();
	al_install_keyboard();

	display = al_create_display(680, 600);	//dimensiones de la pantalla de juego x,y
	buffer = al_create_bitmap(680, 600);	//Buffer

											//Carga de imágenes
	b_morado = al_load_bitmap("b_morado.png"); b_rosa = al_load_bitmap("b_rosa.png"); b_verde = al_load_bitmap("b_verde.png"); b_azul = al_load_bitmap("b_azul.png");
	b_naranja = al_load_bitmap("b_naranja.png"); plataforma = al_load_bitmap("Plataforma.png"); horizontal = al_load_bitmap("horizontal.png");
	vertical = al_load_bitmap("vertical.png"); esq_si = al_load_bitmap("esq_SI.png"); esq_id = al_load_bitmap("esq_ID.png"); esq_ii = al_load_bitmap("esq_II.png");
	esq_sd = al_load_bitmap("esq_SD.png"); fallido = al_load_bitmap("fallido.png"); una_vida = al_load_bitmap("una_vida.png"); dos_vidas = al_load_bitmap("dos_vidas.png");
	tres_vidas = al_load_bitmap("tres_vidas.png"); cero_vidas = al_load_bitmap("cero_vidas.png"); perdio = al_load_bitmap("perdio.png"); final_screen = al_load_bitmap("final.png"); gano = al_load_bitmap("gano.png"); fondo = al_load_bitmap("fondo.png"); inicio = al_load_bitmap("inicio.png"); presione = al_load_bitmap("presione.png");
}

void mostrar_plataforma() {   //se deben colocar argumentos en la funcion que pinten la plataforma en otro lugar dependiendo de la tecla que se presiono
	ALLEGRO_KEYBOARD_STATE estadoTeclado;
	al_get_keyboard_state(&estadoTeclado);
	if (al_key_down(&estadoTeclado, ALLEGRO_KEY_LEFT)) {
		if (plataforma_.posx > limite_izq) {
			plataforma_.posx--;
		}
	}
	if (al_key_down(&estadoTeclado, ALLEGRO_KEY_RIGHT)) {
		if (plataforma_.posx < 580) {
			plataforma_.posx++;
		}
	}
	al_set_target_bitmap(buffer);
	al_draw_bitmap(plataforma_.bitmap, plataforma_.posx, plataforma_.posy, NULL);
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
		}
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

void falloVida() {
	al_set_target_bitmap(buffer);
	al_draw_bitmap(fallido, 150, 170, NULL);
}

void pantallaFinal() {
	al_set_target_bitmap(buffer);
	al_draw_bitmap(final_screen, 1, 1, NULL);
}

float valor_absoluto(float eval) {
	if (eval < 0) return -1 * eval;
	else return eval;
}

float signo(float eval) {
	if (eval < 0) return -1;
	else return 1;
}

void rebote_90(bool izq, bool der, bool up, bool down) {
	float vx = bola.vx, vy = bola.vy;
	if (izq) {
		bola.vx = valor_absoluto(vy)*-1;
		bola.vy = valor_absoluto(vx)*signo(vy);
	}
	if (der) {
		bola.vx = valor_absoluto(vy);
		bola.vy = valor_absoluto(vx)*signo(vy);
	}
	if (up) {
		bola.vy = valor_absoluto(vx)*-1;
		bola.vx = valor_absoluto(vy)*signo(vx);
	}
	if (down) {
		bola.vy = valor_absoluto(vx);
		bola.vx = valor_absoluto(vy)*signo(vx);
	}
}

void revisar_colision() {

	bool izq = false, der = false, down = false, up = false;
	for (int i = 0; i < 65; i++) {
		izq = false; der = false; down = false; up = false;
		if ((bola.posx + bola.vx >= Bloques[i].posx) && (bola.posx + bola.vx <= (Bloques[i].posx + ancho_bloque - 1)) &&
			(bola.posy + bola.vy >= Bloques[i].posy) && (bola.posy + bola.vy <= (Bloques[i].posy + altura_bloque - 1)) && (Bloques[i].activo == true)) {
			Bloques[i].activo = false;

			if (bola.posx < Bloques[i].posx + 1.42) {	//COLISION POR LA IZQUIERDA DEL BLOQUE
				izq = true; //colision por la izquierda
				bola.vx = bola.vx*-1;
			}
			if (bola.posx > Bloques[i].posx + ancho_bloque + -1.42) {	//COLISION POR LA IZQUIERDA DEL BLOQUE
				der = true; //colision por la derecha
				bola.vx = bola.vx*-1;
			}
			if (bola.posy < Bloques[i].posy + 1.42) {	//COLISION POR LA IZQUIERDA DEL BLOQUE
				up = true; //colision por arriba
				bola.vy = bola.vy*-1;
			}
			if (bola.posy > Bloques[i].posy + altura_bloque + -1.42) {	//COLISION POR LA IZQUIERDA DEL BLOQUE
				down = true; //colision por debajo
				bola.vy = bola.vy*-1;
			}
			rebote_90(izq, der, up, down);
			//cout << endl << "izq     der     up      down" << endl;
			//cout << izq << "\t" << der << "\t" << up << "\t" << down << endl;
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

void presionarEnter(ALLEGRO_KEYBOARD_STATE &estadoTeclado, ALLEGRO_EVENT_QUEUE *&evento_cola, ALLEGRO_EVENT &eventos) {
	bool ciclo = 1;
	while (ciclo) {
		al_wait_for_event(evento_cola, &eventos);
		al_get_keyboard_state(&estadoTeclado);
		if (al_key_down(&estadoTeclado, ALLEGRO_KEY_ENTER)) {
			ciclo = 0;
		}
	}
}

void perdioJuego() {
	al_set_target_bitmap(buffer);
	dibujarFondo();
	dibujarMarcos();
	mostrar_plataforma();
	al_draw_bitmap(perdio, 150, 200, NULL);
}

void presioneX(ALLEGRO_KEYBOARD_STATE &estadoTeclado, ALLEGRO_EVENT_QUEUE *&evento_cola, ALLEGRO_EVENT &eventos) {
	int fila, columna;

	al_set_target_bitmap(buffer); //Selecciona el bitmap que se va a utilizar
	al_clear_to_color(transparente); //Limpia el buffer que se selecciono
	dibujarFondo();
	dibujarMarcos();
	mostrar_bloques(cont_final);
	mostrar_plataforma();
	mostrar_bola();
	if (intento) {	//Esto es para evaluar cuando se debe imprimir el banner de intento fallido
		falloVida();
	}

	for (fila = 0; fila < maxFilas; fila++) {
		for (columna = 0; columna < maxColumnas; columna++) {
			if (mapa[fila][columna] == 'x') { //Para imprimir x para continuar
				al_set_target_bitmap(buffer);
				al_draw_bitmap(presione, columna * 50, fila * 18, NULL); //estos numeros por los que se multiplica, definen el tamano de la imagen que cree

			}
		}
	}

	actualizar_pantalla(); //Se mantiene acá hasta presionar X

						   //Evento presionar X
	bool ciclo = 1;
	while (ciclo) {
		al_wait_for_event(evento_cola, &eventos);
		al_get_keyboard_state(&estadoTeclado);
		if (al_key_down(&estadoTeclado, ALLEGRO_KEY_X)) {
			ciclo = 0;
		}
	}

	al_set_target_bitmap(buffer); //Selecciona el bitmap que se va a utilizar
	al_clear_to_color(transparente); //Limpia el buffer que se seleccionó

}

void colision_plataforma(ALLEGRO_KEYBOARD_STATE &estadoTeclado, ALLEGRO_EVENT_QUEUE *&evento_cola, ALLEGRO_EVENT &eventos) {
	if ((bola.posy >= limite_inf - 5) && (bola.posx >= (plataforma_.posx)) && (bola.posx <= (plataforma_.posx + 68))) {
		if (bola.vy>0) {
			vidas = vidas;
			intento = false;
		}
	}
	else if (bola.posy >= limite_inf) {
		vidas--;
		intento = true;
		if (vidas != 0) {
			inicializar();
			presioneX(estadoTeclado, evento_cola, eventos);
		}
	}
}

void usuario(ALLEGRO_KEYBOARD_STATE &estadoTeclado, ALLEGRO_EVENT_QUEUE *&evento_cola, ALLEGRO_EVENT &eventos, ALLEGRO_FONT *fuente) {
	
	//int userPos = 8; //esta variable es para modificar la matriz de usuario e imprimir el nombre debajo de la pantalla
	//funcion para imprimir el nombre de usuario
	//5char username[33] = "Usuario:                        ";
	while (true) {
			al_wait_for_event(evento_cola, &eventos);
			al_get_keyboard_state(&estadoTeclado);
			if (al_key_down(&estadoTeclado, ALLEGRO_KEY_ENTER)) {
				break;
			}
			else {
				//Se limitara a 12 caracteres para escribir el nombre de usuario
				//userPos++;
				al_set_target_bitmap(buffer);
				if (al_key_down(&estadoTeclado, ALLEGRO_KEY_A)) { al_draw_text(fuente, al_map_rgb(172, 190, 250), cont, 340, 0, "A"); username[userPos] = 'a'; }
				else if (al_key_down(&estadoTeclado, ALLEGRO_KEY_B)) { al_draw_text(fuente, al_map_rgb(172, 190, 250), cont, 340, 0, "B"); username[userPos] = 'b'; }
				else if (al_key_down(&estadoTeclado, ALLEGRO_KEY_C)) { al_draw_text(fuente, al_map_rgb(172, 190, 250), cont, 340, 0, "C"); username[userPos] = 'c'; }
				else if (al_key_down(&estadoTeclado, ALLEGRO_KEY_D)) { al_draw_text(fuente, al_map_rgb(172, 190, 250), cont, 340, 0, "D"); username[userPos] = 'd'; }
				else if (al_key_down(&estadoTeclado, ALLEGRO_KEY_E)) { al_draw_text(fuente, al_map_rgb(172, 190, 250), cont, 340, 0, "E"); username[userPos] = 'e'; }
				else if (al_key_down(&estadoTeclado, ALLEGRO_KEY_F)) { al_draw_text(fuente, al_map_rgb(172, 190, 250), cont, 340, 0, "F"); username[userPos] = 'f'; }
				else if (al_key_down(&estadoTeclado, ALLEGRO_KEY_G)) { al_draw_text(fuente, al_map_rgb(172, 190, 250), cont, 340, 0, "G"); username[userPos] = 'g'; }
				else if (al_key_down(&estadoTeclado, ALLEGRO_KEY_H)) { al_draw_text(fuente, al_map_rgb(172, 190, 250), cont, 340, 0, "H"); username[userPos] = 'h'; }
				else if (al_key_down(&estadoTeclado, ALLEGRO_KEY_I)) { al_draw_text(fuente, al_map_rgb(172, 190, 250), cont + 7, 340, 0, "I"); username[userPos] = 'i'; }
				else if (al_key_down(&estadoTeclado, ALLEGRO_KEY_J)) { al_draw_text(fuente, al_map_rgb(172, 190, 250), cont, 340, 0, "J"); username[userPos] = 'j'; }
				else if (al_key_down(&estadoTeclado, ALLEGRO_KEY_K)) { al_draw_text(fuente, al_map_rgb(172, 190, 250), cont, 340, 0, "K"); username[userPos] = 'k'; }
				else if (al_key_down(&estadoTeclado, ALLEGRO_KEY_L)) { al_draw_text(fuente, al_map_rgb(172, 190, 250), cont, 340, 0, "L"); username[userPos] = 'l'; }
				else if (al_key_down(&estadoTeclado, ALLEGRO_KEY_M)) { al_draw_text(fuente, al_map_rgb(172, 190, 250), cont, 340, 0, "M"); username[userPos] = 'm'; }
				else if (al_key_down(&estadoTeclado, ALLEGRO_KEY_N)) { al_draw_text(fuente, al_map_rgb(172, 190, 250), cont, 340, 0, "N"); username[userPos] = 'n'; }
				else if (al_key_down(&estadoTeclado, ALLEGRO_KEY_O)) { al_draw_text(fuente, al_map_rgb(172, 190, 250), cont, 340, 0, "O"); username[userPos] = 'o'; }
				else if (al_key_down(&estadoTeclado, ALLEGRO_KEY_P)) { al_draw_text(fuente, al_map_rgb(172, 190, 250), cont, 340, 0, "P"); username[userPos] = 'p'; }
				else if (al_key_down(&estadoTeclado, ALLEGRO_KEY_Q)) { al_draw_text(fuente, al_map_rgb(172, 190, 250), cont, 340, 0, "Q"); username[userPos] = 'q'; }
				else if (al_key_down(&estadoTeclado, ALLEGRO_KEY_R)) { al_draw_text(fuente, al_map_rgb(172, 190, 250), cont, 340, 0, "R"); username[userPos] = 'r'; }
				else if (al_key_down(&estadoTeclado, ALLEGRO_KEY_S)) { al_draw_text(fuente, al_map_rgb(172, 190, 250), cont, 340, 0, "S"); username[userPos] = 's'; }
				else if (al_key_down(&estadoTeclado, ALLEGRO_KEY_T)) { al_draw_text(fuente, al_map_rgb(172, 190, 250), cont, 340, 0, "T"); username[userPos] = 't'; }
				else if (al_key_down(&estadoTeclado, ALLEGRO_KEY_U)) { al_draw_text(fuente, al_map_rgb(172, 190, 250), cont, 340, 0, "U"); username[userPos] = 'u'; }
				else if (al_key_down(&estadoTeclado, ALLEGRO_KEY_V)) { al_draw_text(fuente, al_map_rgb(172, 190, 250), cont, 340, 0, "V"); username[userPos] = 'v'; }
				else if (al_key_down(&estadoTeclado, ALLEGRO_KEY_W)) { al_draw_text(fuente, al_map_rgb(172, 190, 250), cont, 340, 0, "W"); username[userPos] = 'x'; }
				else if (al_key_down(&estadoTeclado, ALLEGRO_KEY_X)) { al_draw_text(fuente, al_map_rgb(172, 190, 250), cont, 340, 0, "X"); username[userPos] = 'x'; }
				else if (al_key_down(&estadoTeclado, ALLEGRO_KEY_Y)) { al_draw_text(fuente, al_map_rgb(172, 190, 250), cont, 340, 0, "Y"); username[userPos] = 'y'; }
				else if (al_key_down(&estadoTeclado, ALLEGRO_KEY_Z)) { al_draw_text(fuente, al_map_rgb(172, 190, 250), cont, 340, 0, "Z"); username[userPos] = 'z'; }
				actualizar_pantalla();
				al_rest(0.2);
			}
			cont = cont + 10;
			userPos++;
		
	}
}

void juego(ALLEGRO_KEYBOARD_STATE &estadoTeclado, ALLEGRO_EVENT_QUEUE *&evento_cola, ALLEGRO_EVENT &eventos, ALLEGRO_FONT *fuente, ALLEGRO_FONT *user, ALLEGRO_EVENT_QUEUE *cola_timer, ALLEGRO_EVENT *evento_timer) {
	//COMIENZA PANTALLA INICIAL Y EL CICLO DEL JUEGO
	al_rest(0.0001); 
	PantallaInicial(); //aqui debemos llamar a la funcion que imprima la pantalla inicial
	actualizar_pantalla();
	//INICIALIZACION DE LOS BLOQUES, BOLA Y PLATAFORMA
	usuario(estadoTeclado, evento_cola, eventos, fuente);
	//Ciclo que me saca del ENTER
	presionarEnter(estadoTeclado, evento_cola, eventos);
	inicializar();
	presioneX(estadoTeclado, evento_cola, eventos);
	//char username[19] = "Usuario:          ";
	//username[9] = 'A';
	while ((!revisar_final()) && (vidas>0)) {
		//LIMPIAR PANTALLA
		al_get_display_refresh_rate(display);
		al_wait_for_event(cola_timer, evento_timer);
		if (evento_timer->type == ALLEGRO_EVENT_TIMER) {
			al_set_target_bitmap(buffer);	//Selecciona el bitmap que se va a utilizar
			al_clear_to_color(transparente); //Limpia el buffer que se seleccionó
											 //PINTAR OBJETOS
			dibujarFondo();
			mostrar_bloques(cont_final);
			dibujarMarcos();
			colision_plataforma(estadoTeclado, evento_cola, eventos);
			mostrar_bola();
			mostrar_plataforma();
			al_draw_text(user, al_map_rgb(172, 190, 250), 20, 560, 0, username);
			//EJECUCION DE LA LOGICA Y ACTUALIZAR LA PANTALLA
			revisar_colision();
			actualizar_pantalla();
			//al_rest(0.0001);
		}
	}

	if (vidas>0) {
		//si esta variable es falsa, es porque se destruyeron todos los bloques
		ganoJuego();
		actualizar_pantalla();
	}
	else {
		//Se llego al final de las vidas
		perdioJuego();
		actualizar_pantalla();
	}

	//En cualquiera de los casos

	//Esperar el ENTER para continuar a la pantalla final
	presionarEnter(estadoTeclado, evento_cola, eventos);

	//llamar a la funcion que imprime la pantalla final
	pantallaFinal();
	actualizar_pantalla();
	al_rest(0.2);
	
	////***************
}

void salirJuego(ALLEGRO_EVENT_QUEUE *&evento_cola, ALLEGRO_TIMER *timer) {
	//En este momento se borra todo lo que hay en pantalla
	//Se deben aplicar estos destroy para liberar la memoria que se utiliza en los bitmap, el display y el timer
	al_destroy_bitmap(b_morado); al_destroy_bitmap(b_azul); al_destroy_bitmap(b_rosa); al_destroy_bitmap(b_naranja); al_destroy_bitmap(b_verde);
	al_destroy_bitmap(plataforma); al_destroy_bitmap(horizontal); al_destroy_bitmap(vertical); al_destroy_bitmap(buffer); al_destroy_bitmap(esq_si);
	al_destroy_bitmap(esq_sd); al_destroy_bitmap(esq_ii); al_destroy_bitmap(esq_id); al_destroy_bitmap(fondo); al_destroy_bitmap(una_vida); al_destroy_bitmap(dos_vidas);
	al_destroy_bitmap(tres_vidas); al_destroy_bitmap(cero_vidas); al_destroy_bitmap(fallido); al_destroy_bitmap(perdio); al_destroy_font(fuente); al_destroy_font(user); al_destroy_bitmap(final_screen); al_destroy_bitmap(gano); al_destroy_display(display); al_destroy_bitmap(inicio); al_destroy_bitmap(presione); al_destroy_timer(timer);
	al_destroy_event_queue(evento_cola);
}


//**************Estado principal del juego********************

int main(int argc, char **argv) {
	if (!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
	cargaArchivos();

	//Rutinas del teclado
	ALLEGRO_KEYBOARD_STATE estadoTeclado;
	ALLEGRO_EVENT eventos;
	ALLEGRO_EVENT evento_timer;
	ALLEGRO_EVENT_QUEUE *evento_cola = al_create_event_queue();
	ALLEGRO_FONT *fuente = al_load_font("stencil.ttf", 45, 0);
	ALLEGRO_FONT *user = al_load_font("courier.ttf", 24, 0);
	al_register_event_source(evento_cola, al_get_keyboard_event_source());

	//PRUEBAS TIMER
	ALLEGRO_TIMER *timer = al_create_timer(1.0 / FPS); //1/30 por cada actualizacion
	ALLEGRO_EVENT_QUEUE *cola_timer = al_create_event_queue();
	al_start_timer(timer);
	al_register_event_source(cola_timer, al_get_timer_event_source(timer));

	//*******Para integrar las señales del Reset y el salir que vienen con la Raspberry Pi
	//bool reset, salir = false;	//Estas variables eventualmente se reemplazan con la senal proveniente de los botones
	

	while (true) {
		if (salir) {
			salirJuego(evento_cola, timer);
			return 0;
		}
		else if (reset || (!reset && !salir)) {
			//Esta funcion contiene toda la logica de funcionamiento del juego
			juego(estadoTeclado, evento_cola, eventos, fuente, user, cola_timer, &evento_timer);
			//Despues de imprimir la pantalla inicial, retorna al inicio del ciclo, donde vuelve a esperar por los botones
			vidas = 3;
			intento = false;
			cont = 250;
			//reiniciarUsuario();
			char username[33] = "Usuario:                        ";
			userPos = 8;
			al_draw_text(user, al_map_rgb(172, 190, 250), 20, 560, 0, username);
			al_set_target_bitmap(buffer); //Selecciona el bitmap que se va a utilizar
			al_clear_to_color(transparente); //Limpia el buffer que se selecciono
			pantallaFinal();
			actualizar_pantalla();
			if (!reset && !salir) { //si no hay ningun boton presionado, se queda alli hasta que presione alguno
				//con la maquina de estados
				//************ESTO SE IMPLEMENTA CON LA LOGICA DE LA FSM ****************
				//esto simula la espera por un boton al finalizar el programa
				al_rest(3.0); //Aqui debe esperar por un evento, pero no esta esperando porque se volvio loco el Allegro
				al_wait_for_event(evento_cola, &eventos);
			}
		}
		//Aqui vuelve a evaluar la condicion de los botones
	}

	//En este momento se cierra la consola
	return 0;
}

//************************************************************
