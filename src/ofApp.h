#pragma once

#include "ofMain.h"
#include "midiSender.h"
#include "pelota.h"
#include "ofxGui.h"
#include "controlGui.h"

/*
--------------------------------------------------------------
 ofApp.h

 Clase principal de la aplicación openFrameworks.

 Gestiona:

   - Setup general
   - Update por frame
   - Draw de elementos visuales
   - Creación y actualización de pelotas
   - Sincronización con MIDI
   - Detección de colisiones
   - Manejo de GUI, teclado, ventanas y FBOs
 
--------------------------------------------------------------
*/

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void exit();
	
	// Utilidades
	void aplicarPixelado(float valor, bool usarLineal);
	void nacenPelotas();        // generación de pelotas
	void detectarChoques();     // detección de choques
	void windowResized(int w, int h);
	
	void keyPressed(int key);
	
	// Variables generales
	float tiempoDefuncion = 0;      // momento en que murió la última pelota
	float dulceEspera = 2.0f;       // segundos a esperar antes del reseteo o nacimiento
	int NUM_PELOTAS;                // número de pelotas en el próximo nacimiento

	bool laNada = false;            // si false = hay pelotas vivas
	bool showGUI;
	bool info;
	bool hacerNacer = false;
	bool tiempoCumplido;            // ya pasó el tiempo de dulce espera, a nacer.

	ofFbo fbo;
	ofFbo fboPixelado;
	
	ofxPanel gui;
	ofRectangle marco;
	
	Controles control;
	
	
private:
	
	vector<Pelota> pelotas;         // vector que contiene todas las pelotas en pantalla
	MidiSender midi;                // módulo MIDI
	
	
};
