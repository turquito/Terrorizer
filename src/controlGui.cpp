/*
--------------------------------------------------------------
 controlGui.cpp

 Implementación de la clase Controles
 
 Encargada de:

   - Crear y organizar el panel GUI
   - Gestionar sliders y el control de los valores a recorrer, y toggles.
   - Mapear parámetros a valores MIDI
   - Calcular notas según escala musical
   - Mostrar información y manejar las entradas del teclado
--------------------------------------------------------------
*/

# include "controlGui.h"


/*
--------------------------------------------------------------
 setup(gui, midiCh)

 Inicializa todos los elementos del panel gráfico.

   gui    - panel principal de ofxGui
   midiCh - canal MIDI donde se enviarán los CC
 
 - acordes -> Situa el origen de un nuevo naciemiento en un rincón donde las pelotas al nacer chocan
 las paredes casi en simultaneo sonando en simultáneo, simulando acordes.
 
 - factorVital -> se configura antes de cada nacimiento no tiene efecto en tiempo real, tiene un rango entre 1 y 2000 milisegundos.
 
 - record -> activa en Ableton la grabación del canal MIDI que soporta el VSTi que genera el sonido
 y también graba el canal de audio que recibe el audio del instrumento virtual del canal anterior.

 ¡¡¡¡¡---> PARA ESTO ABLETON DEBE ESTAR EN MODO ARRANGEMENT <-----!!!!!
 
 Con la tecla 'g' arrancamos y paramos la grabación. Luego hay que ir a Ableton y guardar la sesion.
 Cuando empecemos otra sesion deberemos tomar la precaución de borrar de las pistas la grabacion anterior.
 
--------------------------------------------------------------
 */

void Controles::setup(ofxPanel& gui, MidiSender* midiCh)
{
	midi = midiCh;
	
    // Panel de control de la creacion de pelotas y su comportamiento
	creacion.setup("Setup pelotero");
	creacion.add(rangoRandom.setup("Nro. de pelotas (q)(w) ", 3, 1, 12));   // Cuantas pelotas queremos que nazcan
	creacion.add(random.setup     ("Nro. aleatorio (k)", true));       // si está activo el Nro. de pelotas por nacer será random
	creacion.add(regeneracion.setup     ("AutoRegeneraciOn (o)", true));   // ciclo de nacimiento infinito
	creacion.add(sumar.setup ("Sumar pelotas (s)", false));                  // false = limpia el vector pelotas o se suman
	creacion.add(factorVital.setup   ("Tiempo de Vida (v)", 1200, 1, 2000)); // tiempo de vida, aplica el mismo a cada generación de pelotas.
	creacion.add(factorVel.setup  ("Velocidad (up)(down)", 0.5, 0.1, 3));
	creacion.add(centro.setup     ("Centro / Mouse (c)", true));             // nacimiento en el centro o donde está el mouse
	creacion.add(acordes.setup     ("aCordes (C)", false));                  // nacimiento en un rincón del rectángulo
	creacion.add(tipoDeEscala.setup ("Tipo de Escala (E)(e)", 4, 0, 4));     // tipo de escala, tenemos cinco opciones.
	
	
	gui.add( &creacion);
	
    // Panel de envío de mensajes MIDI para el control de efectos y grabación en Ableton
	efectos.setup("Efectos");
	efectos.add(ataque.setup("Ataque (A)(a)", 0, 0.0, 20.0));
	efectos.add(release.setup("ReLease (L)(l)", 40, 0.0, 60));
	efectos.add(distorsion.setup("Distorsion (D)(d)", 0, 0, 11));
	efectos.add(filtro.setup("Filtro Pb (P)(p)", 40, 0, 100));
	efectos.add(delay.setup("Delay - wet (R)(r)", 0, 0.0, 100));
	efectos.add(tiempo.setup("Tiempo (T)(t)", 0, 0.0, 2500));
	efectos.add(feedback.setup("Feedback (F)(f)", 0, 0.0, 150));
	efectos.add(reverb.setup("Reverb (izq)(der)", 16, 0.0, 100));
	efectos.add(record.setup     ("Grabar (g)", false));          // Comienzo y detención de la grabación en Ableton
	
	gui.add(&efectos);
}

/*
--------------------------------------------------------------
update()
 Envía valores MIDI CC en cada frame, mapeados desde sliders.
 el primer número entre parentesis es el continous controller asginado para controlar
 y el segundo es el valor que mandamos.
 el control de gain de distorsión y está asignado al #cc29 y el slider manda valores entre 0 y 11
 y se mapean entre 0 y 127 que es lo que recibe Ableton
--------------------------------------------------------------
*/

void Controles::update()
{
	midi->sendControlChange(29, ofMap( distorsion, 0.0, 11, 0, 127 ));
	midi->sendControlChange(30, ofMap(filtro, 0, 100, 0, 127));
	midi->sendControlChange(31, ofMap( ataque, 0.0, 20, 0, 127 ));
	midi->sendControlChange(32, ofMap(release, 0.0, 60, 0, 127 ));
	midi->sendControlChange(33, ofMap(reverb, 0, 100, 0, 127 ));
	midi->sendControlChange(34, ofMap(delay, 0, 100, 0, 127 ));
	midi->sendControlChange(35, ofMap(tiempo, 0, 2500, 0, 127 ));
	midi->sendControlChange(36, ofMap(feedback, 0, 150, 0, 127 ));
}


/*
--------------------------------------------------------------
escalas(nroNota, radioRefe)

 Devuelve una nota MIDI calculada según:
 
   - Tipo de escala seleccionado (cromática, diatónica, etc.)
   - Tamaño (radio) de la pelota

  Esto genera correspondencias visual–sonoras. Mientras mayor el radio, mas grave la nota.
  Las escalas elegidas son todas de siete notas, excepto la escala cromática.
  la nota central a partir de la cual contruyo las escalas es do.
  Esto lo podemos variar con un ofRandom en notaBase pero elegí mantenerlo en do,
  para facilitar la 'auditoria'.
  :)
--------------------------------------------------------------
 */

int Controles::escalas(int nroNota, float radioRefe)
{
	
	int nota;
	
	int escalaDiatonica[]      = {0, 2, 4, 5, 7, 9, 11};
	int escalaMenorMelodica[]  = {0, 2, 3, 5, 7, 9, 11};
	int escalaMenorArmonica[]  = {0, 2, 3, 5, 7, 8, 11};
	int escalaMayorArmonica[]  = {0, 2, 4, 5, 7, 8, 11};
	
	int notaBase = 24;                  // Do central (C2)
	int rangoOctavas = 6;               // 6 octavas de rango
	int totalGrados = rangoOctavas * 7; // 7 notas por octava
	
	int gradoIndex = (int)ofMap(radioRefe, 10.0f, 50.0f, totalGrados-1, 0, true);
	int octava = gradoIndex / 7;        // divido por 7 porque esos 42 grados son 7 por octava
	int grado = gradoIndex % 7;         // me va a dar el grado, entre 0 y 7.

	switch (nroNota){
			
		case 0 :nota = (int)ofMap(radioRefe, 10.0f, 50.0f, 96, 24, true); break;
		case 1 :nota = notaBase + (octava * 12) + escalaDiatonica[grado]; break;
		case 2: nota = notaBase + (octava * 12) + escalaMenorMelodica[grado]; break;
		case 3: nota = notaBase + (octava * 12) + escalaMenorArmonica[grado]; break;
		case 4: nota = notaBase + (octava * 12) + escalaMayorArmonica[grado]; break;
		default: nota = 24; break;
	}
	return nota;
}


/*
--------------------------------------------------------------
 teclado()
 Recibe teclas del usuario y ajusta parámetros del GUI.
 Es una forma mas performativa de controlar los sliders y botones
 La idea es controlar la aplicación de un modo mas parecido a un instrumento musical,
 y no depender del mouse, excepto para elegir puntos de nacimiento.
 A esto le dediqué bastante tiempo.
 Cada función está asignadas a una misma letra: minúscula incrementa el valor,
 mayúscula lo disminuye. Y en otros casos, como reverb o número de notas, la tecla de la derecha
 aumenta el valor, la de la izquierda lo reduce.
--------------------------------------------------------------
*/

void Controles::teclado(int key)
{
	switch(key) {
		
		case 'a': ataque = ofClamp(ataque + 0.1, 0.0, 20); break;
		case 'A': ataque = ofClamp(ataque - 0.1, 0.0, 20); break;
			
		case 'c':
			centro = !centro;
			acordes = false;
			break;
		case 'C':
			acordes = !acordes;
			centro = false;
			break;
			
		case 'd': distorsion = ofClamp(distorsion + 0.1, 0.0, 11); break;
			
		case 'D': distorsion = ofClamp(distorsion - 0.1, 0.0, 11); break;
			
			
		case 'e':
			tipoDeEscala = ofClamp(tipoDeEscala + 1, 0, 4); break;
		case 'E':
			tipoDeEscala = ofClamp(tipoDeEscala - 1, 0, 4); break;
			
		case 'f': feedback = ofClamp(feedback + 1, 0.0, 150); break;
		case 'F': feedback = ofClamp(feedback - 1, 0.0, 150); break;
			
		case 'g':
			record = !record;
			if(record) midi->sendControlChange(11, 127);
			else {
				midi->sendControlChange(11, 0);
				midi->sendControlChange(12, 127);
			}
			break;
			
		case 'k': random = !random; break;
			
		case 'l': release = ofClamp(release + 0.5, 0.0, 60); break;
		case 'L': release = ofClamp(release - 0.5, 0.0, 60); break;
			
		case 'o': regeneracion = !regeneracion; break;
			
		case 'p': filtro = ofClamp(filtro + 1, 0.0, 100); break;
		case 'P': filtro = ofClamp(filtro - 1, 0.0, 100); break;
			
		case 'q': rangoRandom = ofClamp(rangoRandom - 1, 1, 12); break;
		case 'w': rangoRandom = ofClamp(rangoRandom + 1, 1, 12); break;
			
		case 'r': delay = ofClamp(delay + 1, 0.0, 100); break;
		case 'R': delay = ofClamp(delay - 1, 0.0, 100); break;
			
		case 's': sumar = !sumar; break;
			
		case 't': tiempo = ofClamp(tiempo + 40, 0, 2500); break;
		case 'T': tiempo = ofClamp(tiempo - 40, 0, 2500); break;
			
		case 'v': factorVital = ofClamp(factorVital + 10, 0, 2000); break;
		case 'V': factorVital = ofClamp(factorVital - 10, 0, 2000); break;
			
		
		case OF_KEY_UP:   factorVel = ofClamp(factorVel + 0.1, 0.1, 3); break;
		case OF_KEY_DOWN: factorVel = ofClamp(factorVel - 0.1, 0.1, 3); break;
		case OF_KEY_RIGHT:   reverb = ofClamp(reverb + 1, 0, 100); break;
		case OF_KEY_LEFT:    reverb = ofClamp(reverb - 1, 0, 100); break;
	}
}

//--------------------------------------------------------------
// infoPelotas()
//  Imprime en consola información de depuración sobre cada pelota creada.
//--------------------------------------------------------------
void Controles::infoPelotas(int i, float radio, int nota)
{
	ofLogNotice() << "Pelota " << i + 1
				  << " | radio: " << radio
				  << " -> nota: " << nota << " = "
				  << nombreNotas(nota);
}

//--------------------------------------------------------------
// nombreEscalas()
//  Imprime el nombre de la escala actual en consola.
//--------------------------------------------------------------

void Controles::nombreEscalas(int nombre)
{
	switch(nombre){
		case 0: ofLogNotice() << "Escala Cromática"; break;
		case 1: ofLogNotice() << "Escala Diatonica"; break;
		case 2: ofLogNotice() << "Escala Menor Melódica"; break;
		case 3: ofLogNotice() << "Escala Menor Armónica"; break;
		case 4: ofLogNotice() << "Escala Mayor Armónica"; break;
	}
}

//--------------------------------------------------------------
// nombreNotas()
//  Le asigna al número de la nota MIDI su respectiva nota musical (Do, Re, Mi...)
//  junto con la octava correspondiente.
//--------------------------------------------------------------
string Controles::nombreNotas(int nota)
{
	int octava = nota/12 - 1;
	switch ( nota%12 )
	{
		case 0: return "Do" + to_string(octava);
		case 1: return "Do#" + to_string(octava);
		case 2: return "Re" + to_string(octava);
		case 3: return "Re#" + to_string(octava);
		case 4: return "Mi" + to_string(octava);
		case 5: return "Fa" + to_string(octava);
		case 6: return "Fa#" + to_string(octava);
		case 7: return "Sol" + to_string(octava);
		case 8: return "Sol#" + to_string(octava);
		case 9: return "La" + to_string(octava);
		case 10: return "La#" + to_string(octava);
		case 11: return "Si" + to_string(octava);
		default: return "?" + to_string(octava);
	}
}

//--------------------------------------------------------------
// mensaje()
// Imprime en la pantalla una cadena de texto con información de utilidad.
//--------------------------------------------------------------

string Controles::mensaje()
{
	return ("'Barra Espaciadora' Para generar pelotas, 'n' o 'N' Para matarlas!!!\n"
			"'x' Guarda preset actual, 'b' Carga presets \n"
			"'z' Oculta Panel GUI,     'i' Oculta esta info\n"
	);
}
