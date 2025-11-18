#pragma once
#include "ofMain.h"
#include "MidiSender.h"
#include "ofxGui.h"
#include "controlGui.h"

/*
--------------------------------------------------------------
 pelota.h

 Clase Pelota
 
 Cada pelota gestiona:

   - Posición y velocidad
   - Rebotes contra límites
   - Color y tamaño (radio)
   - Tiempo de vida / muerte / renacimiento
   - Emisión de notas MIDI al chocar
   - Parámetros internos de control

 El comportamiento audiovisual surge dinámicamente según su
 movimiento y la interacción con el entorno.
--------------------------------------------------------------
*/

class Pelota
{
public:
	
	// Inicialización basica
	void setup();
	
	// Inicialización completa, con MIDI + vida + posición
	void setup(ofRectangle marco, MidiSender* midiSender, int midiNote, float radio, int vida);
	
	// Actualiza movimiento, velocidad, rebotes, tiempo de vida y MIDI
	void update(float factorVel = 1.0);
	
	// Dibuja en pantalla
	void draw();
	
	// Renacimiento con nuevos valores
	void reset(ofRectangle marco);
	
	// Setters
	void setPos(ofVec2f nuevaPos) { pos = nuevaPos; }
	void setVel(ofVec2f nuevaVel) { vel = nuevaVel; }
	
	// Getters
	bool isDead();
	float getRadio() { return radio; }
	ofVec2f getPos() { return pos; }
	ofVec2f getVel() { return vel; }
	
	// Variables
	ofVec2f pos, vel;
	
	// Estado de vida de la pelota
	bool esperandoNacer = false;  // si espera nacer, no está viva.
	float tiempoVital;
	
	// Tiempos de muerte y renacimiento
	float tiempoDefuncion = 0;
	float dulceEspera = 4.0f; 

	int ccOpenTime_1;
	
private:
	
	ofRectangle limites;    // límites de movimiento
	MidiSender* midi;       // puntero MIDI
	bool noteOn = false;    // Si está sonando la nota
	float radio;            // tamaño pelota
	int note = 60;          // nota MIDI inicializada
};
