#pragma once
#include "ofMain.h"
#include "MidiSender.h"
#include "ofxGui.h"

/*
--------------------------------------------------------------
 controlGui.h

 Clase Controles

 Esta clase encapsula toda la lógica de:

   - Creación y organización del panel gráfico (GUI)
   - Controles de la interfaz (sliders, toggles, etc.)
   - Mapeo de parámetros visuales a mensajes MIDI CC
   - Cálculo de notas y escalas musicales
   - Control por teclado de parámetros
   - Comunicación de información importante

 Está diseñada para mantener separada la lógica de interacción,
 generando un código más claro y modular.
 
 La variable nroNota está relacionada a la elección de escalas:
   0 - Escala cromática
   1 - Escala diatónica
   2 - Escala menor melódica
   3 - Escala menor armónica
   4 - Escala mayor armónica
--------------------------------------------------------------
*/

class Controles
{
public:
	// Inicializa paneles y parámetros GUI
	void setup(ofxPanel& gui, MidiSender* midi);
	
	// Actualiza valores MIDI CC segun la posición de los sliders
	void update();
	
	// Cálculo de notas MIDI segun la escala y el radio de las pelotas
	int escalas(int nroNota, float radioRefe);
	
	// manejo del teclado para el control de parámetros
	void teclado(int key);
	
	// Información de depuración por cada pelota creada
	void infoPelotas(int i, float radio, int nota);
	
	// Imprime en consola el nombre de la escala usada
	void nombreEscalas(int nombre);
	
	// Convierte el número MIDI en nota musical
	string nombreNotas(int nota);
	
	// Mensaje informativo dibujado en pantalla
	string mensaje();
	
	// ELEMENTOS DE GUI
	
	ofxPanel gui;             // creacion del panel
	ofxGuiGroup creacion;     // Grupo de parámetros de creación y comportamiento
	ofxIntSlider rangoRandom;
	ofxIntSlider tipoDeEscala;
	ofxIntSlider factorVital;
	ofxToggle random, record, sumar, centro, acordes, cromatica;
	ofxToggle regeneracion;
	ofxFloatSlider factorVel;
	ofxGuiGroup efectos;      // Grupo de parámetros de efectos
	ofxFloatSlider distorsion;
	ofxFloatSlider ataque;
	ofxFloatSlider release;
	ofxFloatSlider reverb;
	ofxFloatSlider tiempo;
	ofxFloatSlider delay;
	ofxFloatSlider feedback;
	ofxFloatSlider filtro;
	
	// Getters
	float getDistorsion() { return distorsion; }
	float getAtaque() { return ataque; }
	float getRelease() { return release; }
	float getReverb() { return reverb; }
	float getFeedback() { return feedback; }
	float getDelay() { return delay; }
	float getTempo() { return tiempo;}
	float getFiltro() { return filtro;}
	
	// número selector de escala
	int nroNota;
	
private:
	MidiSender* midi;  // MIDI:puntero que apunta al canal activo
	
};

