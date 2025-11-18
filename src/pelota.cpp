// Pelota.cpp
#include "pelota.h"

void Pelota::setup() {
	pos.set(ofGetWidth()/2, ofGetHeight()/2);
	vel.set(ofRandom(-15, 15), ofRandom(-15, 15));
	radio = ofRandom(10,30);
	bounds = ofRectangle(0, 0, ofGetWidth(), ofGetHeight());
	midi = nullptr;
	lifeSpan = 1000;
	
}

void Pelota::setup(ofRectangle marco, MidiSender* midiSender, int midiNote, float radioParam, int life) {
	bounds = marco;
	radio = radioParam;              // usar el radio que recibe
	pos.set(marco.getCenter()); // empieza en el centro
	vel.set(ofRandom(-15, 15), ofRandom(-15, 15));
	
	midi = midiSender;
	note = midiNote;
	lifeSpan = life;
	esperandoNacer = false;
	noteOn = false;
}

void Pelota::reset(ofRectangle marco) {
	bounds = marco;
	radio = ofRandom(10,50);        // radio aleatorio al renacer
	pos.set(ofGetMouseX(), ofGetMouseY()); // renace donde el mouse
	vel.set(ofRandom(-15,15), ofRandom(-15,15));
	lifeSpan = 1000;
	esperandoNacer = false;
	noteOn = false;
	note = (int)ofMap(radio, 10, 50, 96, 24); // recalcular nota inversa según el radio
}


bool Pelota::isDead(){ return esperandoNacer; } //

void Pelota::update(float factorVel) {
	// Solo actualizar si tenemos MIDI válido
	if (midi == nullptr) return;

	// Si está esperando nacer, no hacer nada más
	if (esperandoNacer) {
			if (ofGetElapsedTimef() - deathTime > nacerDelay) {
				reset(bounds);
				esperandoNacer = false;
			}
			return; // mientras espera no se mueve
		}
//si ya murio, marcar y salir
	if(lifeSpan <=0){
		esperandoNacer = true;
		deathTime = ofGetElapsedTimef();
		if(noteOn){
			midi->sendNoteOff(note);
			noteOn = false;
		}
		return;
	}
	
	pos += vel * factorVel;
	lifeSpan -= 2;

	bool rebote = false;
	
	// Rebote con reposicionamiento para evitar que se peguen ---- 
	if (pos.x - radio < bounds.getLeft()) {
		vel.x *= -1;
		pos.x = bounds.getLeft() + radio;
		rebote = true;
	}
	if (pos.x + radio > bounds.getRight()) {
		vel.x *= -1;
		pos.x = bounds.getRight() - radio;
		rebote = true;
		//midi->sendControlChange(7, 127);
		ccOpenTime_1 = ofGetElapsedTimeMillis();

	}
	if (pos.y - radio < bounds.getTop()) {
		vel.y *= -1;
		pos.y = bounds.getTop() + radio;
		rebote = true;
	
	}
	if (pos.y + radio > bounds.getBottom()) {
		vel.y *= -1;
		pos.y = bounds.getBottom() - radio;
		rebote = true;
		//midi->sendControlChange(9, 127);
		ccOpenTime_2 = ofGetElapsedTimeMillis();
	}

	// MIDI logic
	if (rebote && !noteOn) {
		midi->sendNoteOn(note, 100);
	
		noteOn = true;
	
	} else if (!rebote && noteOn) {
		midi->sendNoteOff(note);
		noteOn = false;
	}
	if (ccOpenTime_1 > 0 &&
			(ofGetElapsedTimeMillis() - ccOpenTime_1) > 400) {
			midi->sendControlChange(7, 0);  // cierra CC
			ccOpenTime_1 = 0;
		}

	if (ccOpenTime_2 > 0 &&
			(ofGetElapsedTimeMillis() - ccOpenTime_2) > 400) {
			midi->sendControlChange(9, 0);  // cierra CC
			ccOpenTime_2 = 0;
		}
	
	if (lifeSpan <= 0) {
			esperandoNacer = true;
			deathTime = ofGetElapsedTimef();
		}
}



void Pelota::draw() {
	if(esperandoNacer) return;
	
	// Colores súper saturados
	int colorHue = (note * 8) % 360;  // más variación
	ofSetColor(ofColor::fromHsb(colorHue, 255, 255), lifeSpan);
	
	// Dibujar con borde brillante
	ofNoFill();
	ofSetLineWidth(3);
	ofDrawCircle(pos, radio);
	
	ofFill();
	ofDrawCircle(pos, radio);
}

