/*
================================================

 ofApp.cpp
 
 Proyecto: "Terrorizer" Instrumento generador Audiovisualmusical.

 Descripcion general:
 En este archivo se implementa la idea principal de la aplicación.
 - Inicializa el entorno (setup)
 - Actualización cuadro por cuadro (update).
 - Detecta cambios en el tamaño de la pantalla (windowsresize)
 - Dibuja y renderiza gráficos (draw)
 - Gestiona el nacimiento de pelotas, y su regeneracion (nacenPelotas)
 - Adminitra la interacción entre pelotas(detectarColisiones)
 - Asignacion de teclas a funciones(keyPressed)
 - Gestión para la salida del programa. (exit)
 
 Autor: Luis Maria El Halli Obeid
 Materia: Programación y Algoritmos 1
 Profesor: Dr. Fabián Sguiglia
 Fecha: 17 de Noviembre 2025
 
================================================
*/

#include "ofApp.h"

/*
--------------------------------------------------------------
setup()
 - Inicializa la aplicación
 - Configura la ventana y el framerate
 - Prepara los framebuffers para dibujar y pixelar las pelotas
 - Incializa la GUI y el protcocolo MIDI, y sus parametros respectivos
 - Setea variables internas de estado
 --------------------------------------------------------------
 */


void ofApp::setup()
{
	hacerNacer = false;
	showGUI = true;
	info = true;
	
	ofSetWindowTitle("Terrorizer");
	ofSetFrameRate(60);
	ofBackground(0);
	
// Uso de frame buffers para el dibujo y el pixelado
	fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA); // allocate = reserva memoria para un objeto fbo
	fboPixelado.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
	
// Configuración del panel GUI
	gui.setup("Controles");
	control.setup(gui, &midi);
	
// inicializa el MIDI (puerto, canal)
	midi.setup(0, 1);

}


/*
--------------------------------------------------------------
 update()
 - Actualiza el estado de los elementos
 - Actualiza valores MIDI y GUI
 - Actualiza el estado de las pelotas, posición, velocidad, notas, rebotes, y colisiones.
 - Si alguna murió inicia la regeneración.
 - Elige entre dos modos, "Centro" y "Acordes", que definen donde
   nacerán las pelotas en su regeneración proxima:
   "Acordes": salen de un rincón, "Centro": desde el
   centro si está clickeado, si no, desde donde esté el mouse.
 --------------------------------------------------------------
 */


void ofApp::update()
{
// Actualiza valores MIDI segun el tablero GUI
	control.update();
	
// Estados excluyente, Se elige en el GUI donde van a nacer las pelotas.
	bool Centro = false;
	bool Acordes = false;
	
	if(control.centro && control.centro != Centro)
	   control.acordes = false;
	
	if(control.acordes && control.acordes != Acordes)
	   control.centro = false;
	
	Centro = control.centro;
	Acordes = control.acordes;
	
	bool algunaMurio = false;
	int pelotasMuertas = 0;
	
// Actualiza el estado individual de las pelotas
	
	for (int i = 0; i < pelotas.size(); i++) {
		pelotas[i].update(control.factorVel);
		
		if (pelotas[i].isDead()) {
			algunaMurio = true;
			pelotasMuertas++;
		}
	}

// Registra el momento en que murió la primera pelotas
	if (algunaMurio && !laNada) {
		tiempoDefuncion = ofGetElapsedTimef();
		laNada = true;
		ofLogNotice() << "Número actual de pelotas: " << pelotas.size(); // imprimo nro de pelotas
	}

	
// Activa la regeneración continua de nuevas pelotas!
	if(control.regeneracion) {
		bool tiempoCumplido = (ofGetElapsedTimef() - tiempoDefuncion >= dulceEspera);
		
		if (laNada && tiempoCumplido) {
			midi.allNotesOff();  // apago las notas
			nacenPelotas();      // genero las nuevas pelotas
		}
	}
	
// gestiona los choques entre pelotas
	detectarChoques();
}


// -----------------------------------------------
// windowResized()
// se ejecuta si hay algún cambio en el tamaño de la pantalla
// -----------------------------------------------
void ofApp::windowResized(int w, int h)
{
	fbo.allocate(w, h, GL_RGBA);
	marco.set(0, 0, w, h);
}



/*
-----------------------------------------------
draw()
 
Dibuja:
 - El fondo en base a los efecto de la distorsion
 - Las pelotas en un FBO
 - El pixelado según la distor y la reverb
 - El panel GUI
 - E información general en texto
-----------------------------------------------
 */

void ofApp::draw()
{
	float distorsion = control.distorsion;
	float reverb = control.reverb;
	
	// Fondo dinámico
	float r = ofMap(distorsion, 0, 11, 0, 150);
	ofBackground(r/2, 0, 0);
	
	// Dibujar pelotas en FBO normal (no pixelado)
	 fbo.begin();
	 ofClear(0, 0, 0, 0);
	
	 for(int i = 0; i < pelotas.size(); i++)
		 pelotas[i].draw();
	 fbo.end();
	
	// De acá en adelante, se produce el pixelado de las pelotitas
	 ofSetColor(255);
	 ofEnableAlphaBlending(); // activa transparencia
	
	 //pixelado por distor
	 if(distorsion > 1)
	 {
	 float pixelFactor = ofMap(distorsion, 1, 11, 1.0, 0.1);
	 // Calcular el tamaño del nuevo marco según la distorsión
	 int lowW = ofGetWidth() * pixelFactor;
	 int lowH = ofGetHeight() * pixelFactor;
	 // Re-alloca (reserva memoria) el FBO pixelado con el nuevo ajuste de tamaño
	 if(fboPixelado.getWidth() != lowW || fboPixelado.getHeight() != lowH)
		 fboPixelado.allocate(lowW, lowH, GL_RGBA);
	 
	 // Dibujar FBO original en un FBO mas chico
	 fboPixelado.begin();        // comienzo
	 ofClear(0, 0, 0, 0);        // limpia el buffer
	 fbo.draw(0, 0, lowW, lowH); // dibuja encima
	 fboPixelado.end();          // termino
		 
	 // Escalar el FBO pequeño al tamaño actual,
	 // Usa pixelado de cuadraditos nítidos: GL_NEAREST
	 fboPixelado.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST); //
	 fboPixelado.draw(0, 0, ofGetWidth(), ofGetHeight());
	 
	 }
	
	 // Pixelado por Reverb
	 if(control.reverb > 10) {
	 float pixelFactor = ofMap(control.reverb, 10, 100, 1.0, 0.02);
	 int lowW = ofGetWidth() * pixelFactor;
	 int lowH = ofGetHeight() * pixelFactor;
		 
	 if(fboPixelado.getWidth() != lowW || fboPixelado.getHeight() != lowH)
		 fboPixelado.allocate(lowW, lowH, GL_RGBA);
	 
	 fboPixelado.begin();
	 ofClear(0, 0, 0, 0);
	 fbo.draw(0, 0, lowW, lowH);
	 fboPixelado.end();
	 // Uso de un pixelado de bordes suaves y blureados: GL_LINEAR
	 fboPixelado.getTexture().setTextureMinMagFilter(GL_LINEAR, GL_LINEAR);
	 fboPixelado.draw(0, 0, ofGetWidth(), ofGetHeight());
	 
	 }
	 else
	 {
	 // sin pixelado
	 fbo.draw(0, 0, ofGetWidth(), ofGetHeight());
	 }
	
	ofDisableAlphaBlending(); // Desactivo transparencia
	
	// Dibujo del GUI
	if ( showGUI ) gui.draw();
	
	// Mensaje informativo
	if(info) ofDrawBitmapString(control.mensaje(), 10, ofGetHeight() - 40);
}


/*
-----------------------------------------------
nacenPelotas()
 - Limpia el vector pelotas, segun el GUI
 - Calcula la cantidad incial de pelotas
 - Define radio, vida, notas, escala y posicion inicial
 - Crea cada pelota y la agrego al vector
-----------------------------------------------
 */

void ofApp::nacenPelotas()
{
	
	ofLogNotice() << "resetPelotas() -> limpiando y creando nuevas pelotas";
	
	midi.allNotesOff();
	
	if(!control.sumar)
		pelotas.clear();
	
	// Configura el o el espacio donde vivirán las pelotas
	marco.set( 0, 0,ofGetWidth(),ofGetHeight());
	
	// Determina el numero de pelotas
	if (control.random)
		NUM_PELOTAS = (int)ofRandom(1,12);
	else
		NUM_PELOTAS = control.rangoRandom;
	
	ofLogNotice() << "Nuevo NUM_PELOTAS: " << NUM_PELOTAS;

	int tipoEscala = control.tipoDeEscala;
	
	// Creo cada pelota
	for (int i = 0; i < NUM_PELOTAS; i++) {                           // crea las pelotas
		
		float radio = ofRandom(10.0f,50.0f);
		float& radioRefe = radio;
		
		int nota = control.escalas(tipoEscala, radioRefe);
		int vida = control.factorVital;
		 
		Pelota p;                                                     // crea un objeto de la clase Pelota
		p.setup(marco, &midi, nota, radio, vida);                     // setup del objeto
		
		// Posición inicial según el modo elegido
		if(control.acordes)
			p.pos.set(ofGetWidth() , 0);         // Configuración del origen de las pelotas
		else {
			if(control.centro)
				p.setPos(marco.getCenter());
			else
				p.pos.set(ofGetMouseX(), ofGetMouseY());
		}
		
		pelotas.push_back(p);
		
		control.infoPelotas(i, radio, nota);             // imprime informacion sobre cada pelota
	}
	
	control.nombreEscalas(tipoEscala);  //imprime el nombre de la escala
	
	laNada = false;                     // Hay pelotas, la nada ya no es Nada.
	
}


/*
-----------------------------------------------

 detectarColisiones()
 
  Compara las pelotas de a pares y si se acercan a una distancia menor a la suma de sus radios, chocan.
  Entonces:
   - Intercambian velocidades
   - Se separan ligeramente las pelotas para evitar pegado
--------------------------------------------------------------
*/


void ofApp::detectarChoques()
{
	// Compara la pelota i con todas las demás
	for(int i = 0; i < pelotas.size(); i++) {
		for(int j = i + 1; j < pelotas.size(); j++) {
		
			ofVec2f pos1 = pelotas[i].getPos();
			ofVec2f pos2 = pelotas[j].getPos();
			float r1 = pelotas[i].getRadio();
			float r2 = pelotas[j].getRadio();
			
			float distancia = pos1.distance(pos2);
			float sumaRadios = r1 + r2;
			
			// Si la distancia es menor que la suma de radios, chocan
			if (distancia < sumaRadios) {
				
				ofVec2f direccion = (pos2 - pos1).normalize();
				
				// Intercambiar velocidades (rebote simple)
				ofVec2f vel1 = pelotas[i].getVel();
				ofVec2f vel2 = pelotas[j].getVel();
				
				pelotas[i].setVel(vel2);
				pelotas[j].setVel(vel1);
				
				// Separa las pelotas para evitar que se solapen
				float solapamiento = sumaRadios - distancia;
				ofVec2f separacion = direccion * (solapamiento / 2 + 1);
				// si separacion tuviera el mismo signo, las pelotas marcharían juntas y solapadas.
				pelotas[i].setPos(pos1 - separacion);
				pelotas[j].setPos(pos2 + separacion);
			}
		}
	}
}


/*
--------------------------------------------------------------
 keyPressed()

 Maneja ciertas acciones desde el teclado:
  - Guardar o cargar presets
  - Mostrar o ocultar GUI, en información útil en pantalla.
  - Crear o eliminar pelotas
--------------------------------------------------------------
 */


// Acá se asignan los controles que tienen que ver con la lógica de funcionamiento, guardar y cargar presets,hacer nacer pelotas, o matarlas.
// El control de sliders, botones y parámetros propios de sonido y comportamiento está en la clase controlGUI

void ofApp::keyPressed(int key) {
	control.teclado(key);

	switch(key) {
			
		case 'b': {
			ofFileDialogResult res = ofSystemLoadDialog("Cargar Preset");
			if (res.bSuccess) gui.loadFromFile(res.filePath);
			break;
		}
			
		case 'i':
			info = !info;
			break;
			
		case 'n':
		case 'N':
			pelotas.clear();
			break;
			
		case 'x':{
			ofFileDialogResult res = ofSystemSaveDialog("preset.xml", "Saving Preset");
			if (res.bSuccess) gui.saveToFile(res.filePath);
			break;
		}
			
		case 'z':
			showGUI = !showGUI;
			break;
	}
	
	if(key == OF_KEY_SPACE) nacenPelotas();
}


/*
--------------------------------------------------------------
exit()
 Se llama al cerrar la aplicación.
 Guarda la configuración actual del GUI y nos avisa,
 y apaga cualquier nota MIDI que siga sonando.
--------------------------------------------------------------
*/


void ofApp::exit()
{
	gui.saveToFile("Preset_de_cierre.xml"); // Guarda la configuración previa al cerrar el proyecto
	ofLogNotice() << "Se cerró de forma correcta y se salvó el ultimo seteo GUI";
	midi.allNotesOff(); ;          // Corta todas las notas, mando un Note Off para todas las notas que estén sonando
	midi.exit();                    // Sale y cierra el puerto MIDI en uso
}
