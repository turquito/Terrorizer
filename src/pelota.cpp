/*
--------------------------------------------------------------
 Pelota.cpp

 Implementación de la clase Pelota
 Cada objeto de de esta clase cuenta con:
   - La misma Posición inicial
   - velocidad y radio individuales.
   - un tiempo de vida (tiempoVital)
   - Límite espacial (limites)
   - Comunicación MIDI al chocar contra paredes

 Las pelotas viven, chocan y rebotan dentro del rectangulo establecido por límites,
 emitiendo notas MIDI al "rebotar" contra las paredes del rectangulo.
 También pueden emitir otros mensajes.
 Mueren según un tiempo predefinido y común para todas las pelotas de una misma generación.
 La vida de las pelotas termina cuando se van las bolas se van volviendo transparentes hasta desaparecer
 luego puede venir una nueva generación de pelotas o no,
 esto se maneja en la clase ofApp a través del GUI o la barra espaciadora.
 
--------------------------------------------------------------
*/

#include "pelota.h"


/*
--------------------------------------------------------------
 setup()

 Configura valores iniciales por defecto cuando aún no se envía
 ningún parámetro externo:

  - Posición centrada al medio del rectangulo delimitador.
  - Velocidad aleatoria
  - Radio aleatorio
  - Límtes igual al tamaño de la ventana
  - Tiempo de vida de las partículas. Esta variable va a controlar la transparencia de las pelotas
 --------------------------------------------------------------
 */

void Pelota::setup() {
	pos.set(ofGetWidth()/2, ofGetHeight()/2);
	vel.set(ofRandom(-15, 15), ofRandom(-15, 15));
	radio = ofRandom(10,30);
	
	limites = ofRectangle(0, 0, ofGetWidth(), ofGetHeight());
	
	midi = nullptr;      // Asigna el apuntador midi a un puntero nulo para que no genere problemas
	tiempoVital = 1000;  // Inicializa el tiempo de vida de las pelotas
}

/*
--------------------------------------------------------------
 setup(marco, midiSender, nota, radioParam, vida)

 Es la anterior función setup pero sobrecargada.
 inicializa una pelota con los siguientes argumentos:

   marco         - rectángulo donde vive cada pelota, todas viven en un mismo espacio.
   midiSender    - canal MIDI para enviar notas, es el mismo para todas las pelotas.
   nota          - nota MIDI asignada por pelota, el rango es de unas cinco octavas, entre 24 y 96.
   radioParam    - tamaño de la pelota. El radio y la nota son inversamente proporcionales
   vida          - tiempo de vida en milisegundos
 
   Seteo de variables internas de estado.
 --------------------------------------------------------------
 */


void Pelota::setup(ofRectangle marco, MidiSender* midiSender, int midiNote, float radioParam, int vida)
{
	limites = marco;
	radio = radioParam;
	pos.set(marco.getCenter());
	vel.set(ofRandom(-15, 15), ofRandom(-15, 15));
	
	midi = midiSender;
	note = midiNote;
	tiempoVital = vida;
	
	esperandoNacer = false;  // La pelota sigue viva. Si fuera true la pelota murió.
	noteOn = false;          // No se está mandando ningún mensaje = no está sonando nada
}


/*
--------------------------------------------------------------
reset()
 
 Funcion para reiniciar las pelotas luego de haber "muerto".
 
 Resetea una pelota cuando "renace":
   - Nuevo radio aleatorio, valores entre 10 y 50
   - Nota calculada según radio a traves de una función de mapeo
   - Nueva posición en mouse
   - Nueva velocidad
   - Setea variables internas, nuevamente.
--------------------------------------------------------------
 */

void Pelota::reset(ofRectangle marco) {
	
	radio = ofRandom(10,50);                     // Radio aleatorio al renacer
	pos.set(ofGetMouseX(), ofGetMouseY());       // Renace donde está el mouse
	vel.set(ofRandom(-15,15), ofRandom(-15,15));
	tiempoVital = 1000;                          // Tiempo de vida de la pelota
	esperandoNacer = false;
	noteOn = false;
	
	// Nota = mapea valores entre [10, 50] un rango de notas midi en de [96, 24]
	// menor radio => nota de valor más alto = mas aguda
	note = (int)ofMap(radio, 10, 50, 96, 24);
	
}

//--------------------------------------------------------------
// isDead()
// Devuelve true si la pelota terminó su ciclo de vida y está
// esperando volver a nacer.
//--------------------------------------------------------------

bool Pelota::isDead(){
	return esperandoNacer;
}


/*
 --------------------------------------------------------------
 update(factorVel)
 
 Actualiza la física:
   - Movimiento
   - Cuenta regresiva del tiempo de vida
   - Rebotes contra paredes
   - Envío de mensaje MIDI NoteOn / NoteOff
   - Manejo de muerte / renacimiento
 --------------------------------------------------------------
 */

void Pelota::update(float factorVel) {
// Solo actualizar si tenemos MIDI válido
	

	if (midi == nullptr) return;  // si midi no tiene canal, la ejecución termina.
    
	// Si está esperando nacer, no hacer nada más
	// si lifespan < 0, la pelota ya murio, empieza la cuenta del tiempo desde que murió y se apaga la nota, envío un "sendNoteOff". Termina la función.
	if(tiempoVital <=0){
		esperandoNacer = true;
		tiempoDefuncion = ofGetElapsedTimef();
		
		if(noteOn){
			midi->sendNoteOff(note);
			noteOn = false;
		}
		return;
	}
	// Movimiento
	pos += vel * factorVel;     // Control de la velocidad y la posición, se puede ajustar en tiempo real
	tiempoVital -= 2 ;		    // cuenta regresiva desde el nacimiento a la muerte
	
	bool rebote = false;        // inicializo la variable rebote. 
		
// --------------------------------
// REBOTES
// Si una de las pelotas "toca" alguno de los bordes del rectangulo donde viven,
// el centro de la bola "frena" a un radio de distancia del marco e invierte su velocidad,
// sale para el otro lado, eso se cuenta como un rebote => rebote = true. Esta condición va a enviar un noteOn.
//---------------------------------
	
    // Pared izquierda
	if (pos.x - radio < limites.getLeft()) {
		vel.x *= -1;
		pos.x = limites.getLeft() + radio;
		rebote = true;
	}
	
    // Pared derecha
	if (pos.x + radio > limites.getRight()) {
		vel.x *= -1;
		pos.x = limites.getRight() - radio;
		rebote = true;
	}
	
    // Pared de arriba
	if (pos.y - radio < limites.getTop()) {
		vel.y *= -1;
		pos.y = limites.getTop() + radio;
		rebote = true;
	}
	
    // Pared de abajo
	if (pos.y + radio > limites.getBottom()) {
		vel.y *= -1;
		pos.y = limites.getBottom() - radio;
		rebote = true;
	}
	
	// Manejo del envío de NOTE ON / NOTE OFF
	// manda la nota al momento del rebote
	if (rebote && !noteOn) {
		midi->sendNoteOn(note, 100);
		noteOn = true;
	}
	// "Suelta la tecla" (Note Off) en el momento posterior al rebote
	else if (!rebote && noteOn) {
		midi->sendNoteOff(note);
		noteOn = false;
	}
	
/*
 Si nació (esparandoNacer = false) la funcion termina.
 Si el tiempoCumplido es mayor a dulce espera,
 se resetean la variables caracteristicas y vitales de la nueva pelota
 la pelota ya nació. la función termina su ejecución.
*/
	
	bool tiempoCumplido = (ofGetElapsedTimef() - tiempoDefuncion >= dulceEspera);
	if (esperandoNacer) {
		if (tiempoCumplido) {
			reset(limites);
			esperandoNacer = false;
		}
		return;
	}
	
}

/*
--------------------------------------------------------------
draw()
 Dibuja un círculo colorido cuyo color depende de la nota MIDI.
 La transparencia está basada en tiempoVital para un efecto dinámico
 acompañando la desaparición de la pelota.
--------------------------------------------------------------
 */

void Pelota::draw() {
	
	if(esperandoNacer) return; // Si está esperando nacer, no dibuja nada.
	
	int colorHue = (note * 8) % 360;  // Colores súper saturados
	
	ofSetColor(ofColor::fromHsb(colorHue, 255, 255), tiempoVital);  // acá manejamos el tiempo de vida
	ofFill();
	ofDrawCircle(pos, radio);
}


