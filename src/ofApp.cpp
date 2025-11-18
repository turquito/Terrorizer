#include "ofApp.h"

void ofApp::setup(){
	
	showGUI = true;
	
	ofSetWindowTitle("Terrorizer");
	ofSetFrameRate(60);
	ofBackground(0);
	
	fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
	fboPixelado.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
	
	gui.setup("Controles (z)");
	creacion.setup("Setup pelotero");
	creacion.add(rangoRandom.setup("Nro. de bolas (q)(w) ", 3, 1, 12));
	creacion.add(random.setup     ("Cant. aleatoria (k)", false));
	creacion.add(renovar.setup    ("Renovar bolas (r) (n)", true));
	creacion.add(centro.setup     ("Centro / Mouse (c)", true));
	creacion.add(lifeSpan.setup   ("Tiempo de vida (v)", 1000, 1, 2000));
	creacion.add(factorVel.setup  ("Velocidad (up)(down)", 1, 0.1, 3));
	creacion.add(record.setup     ("Grabar    (g)", false));
	gui.add( &creacion);
	efectos.setup("Efectos");
	efectos.add(ataque.setup("ataque (a)(A)", 4, 0.0, 20.0));
	efectos.add(release.setup("release (l)(L)", 40, 0.0, 60));
	efectos.add(distorsion.setup("Saturacion (s)(S)", 0, 0, 11));
	efectos.add(filtro.setup("Filtro (i)(I)", 40, 0, 100));
	efectos.add(tiempo.setup("Tiempo (t)(T)", 0, 0.0, 2500));
	efectos.add(delay.setup("delay wet (d)(D)", 0, 0.0, 100));
	efectos.add(feedback.setup("feedback (f)(F)", 0, 0.0, 150));
	efectos.add(reverb.setup("reverb (izq)(der)", 16, 0.0, 100));
	gui.add( &efectos);
	
	midi.setup(0, 1);  //puerto 0 y canal MIDI 1
	resetPelotas();  // Inicializar pelotas por primera vez
}


static void safeAllNotesOff(MidiSender &midi) {
	// enviar Note Off para un rango razonable de notas usadas
	for (int n = 0; n < 128; ++n) { midi.sendNoteOff(n); }
}

void ofApp::update() {
	
	//mensajes para Ableton #cc + valores
	
	midi.sendControlChange(30, ofMap( distorsion, 0.0, 11, 0, 127 ));
	midi.sendControlChange(31, ofMap( ataque, 0.0, 20, 0, 127 ));
	midi.sendControlChange(32, ofMap(release, 0.0, 60, 0, 127 ));
	midi.sendControlChange(33, ofMap(reverb, 0, 100, 0, 127 ));
	midi.sendControlChange(34, ofMap(feedback, 0, 150, 0, 127 ));
	midi.sendControlChange(35, ofMap(delay, 0, 100, 0, 127 ));
	midi.sendControlChange(36, ofMap(tiempo, 0, 2500, 0, 127 ));
	midi.sendControlChange(70, ofMap(filtro, 0, 100, 0, 127));
	
	bool algunaMurio = false;
	int pelotasMuertas = 0;
	
//Actualizo pelotas
	for (int i = 0; i < pelotas.size(); i++) {
		pelotas[i].update(factorVel);

		if (pelotas[i].isDead()) {
			algunaMurio = true;
			pelotasMuertas++;
		}
	}
//Si alguna murió, iniciar timer reset global
	
	if (algunaMurio && !esperandoReset) {
		lastDeathTime = ofGetElapsedTimef();
		esperandoReset = true;
		ofLogNotice() << "Número actual de pelotas: " << pelotas.size();
	}
//verificar si pasó el delay y reiniciar
	
	if (esperandoReset && (ofGetElapsedTimef() - lastDeathTime >= resetDelay)) {
		ofLogNotice() << "Pelotas ANTES del reset: " << pelotas.size();
	//Apago notas por seguridad
		safeAllNotesOff(midi);
		resetPelotas();
		ofLogNotice() << "Pelotas DESPUÉS del reset: " << pelotas.size();
	}

	detectarColisiones();
}




//dibujando FBO
void ofApp::draw(){
	
	float value = distorsion;
	float camara = reverb;
	float reverbNorm = reverb / 100.0;
	
	// FONDO
	float r = ofMap(value, 0, 11, 0, 150);
	float fondoBrillo = ofMap(reverbNorm, 0, 1, 0, 255);
	//ofBackground(r/1.5, fondoBrillo/8, fondoBrillo);
	ofBackground(r/2, 0, 0);
	
	// Dibujar pelotas en FBO normal
	fbo.begin();
	ofClear(0, 0, 0, 0);
	//ofEnableAlphaBlending();
	for(int i = 0; i < pelotas.size(); i++){
		pelotas[i].draw();
	}
	//ofDisableAlphaBlending();
	fbo.end();
	
	// Aplicar pixelado
	ofSetColor(255);
	ofEnableAlphaBlending();
	//pixelado por distor
	if(value > 1) {
		// Calcular resolución reducida según distorsión
		float pixelFactor = ofMap(value, 1, 11, 1.0, 0.1);
		int lowW = ofGetWidth() * pixelFactor;
		int lowH = ofGetHeight() * pixelFactor;
		
		// Re-allocar FBO pixelado con nueva resolución
		if(fboPixelado.getWidth() != lowW || fboPixelado.getHeight() != lowH) {
			fboPixelado.allocate(lowW, lowH, GL_RGBA);
		}
		
		// Dibujar FBO original en FBO pequeño
		fboPixelado.begin();
		ofClear(0, 0, 0, 0);
		fbo.draw(0, 0, lowW, lowH);
		fboPixelado.end();
		
		// Escalar el FBO pequeño a pantalla completa (esto crea el pixelado) GL_LINEAR
		fboPixelado.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
		//fboPixelado.getTexture().setTextureMinMagFilter(GL_LINEAR, GL_LINEAR);
		fboPixelado.draw(0, 0, ofGetWidth(), ofGetHeight());
		
	} else {
		// Sin pixelado
		fbo.draw(0, 0, ofGetWidth(), ofGetHeight());
	}
	//pixelado por Reverb
	if(reverb > 10) {
		// achicamos la resolución según reverb
		float pixelFactor = ofMap(reverb, 10, 100, 1.0, 0.02);
		int lowW = ofGetWidth() * pixelFactor;
		int lowH = ofGetHeight() * pixelFactor;
		
		// Re-allocar FBO pixelado con nueva resolución
		if(fboPixelado.getWidth() != lowW || fboPixelado.getHeight() != lowH) {
			fboPixelado.allocate(lowW, lowH, GL_RGBA);
		}
		
		// Dibujar FBO original en FBO pequeño
		fboPixelado.begin();
		ofClear(0, 0, 0, 0);
		fbo.draw(0, 0, lowW, lowH);
		fboPixelado.end();
		
		// Escalar el FBO pequeño a pantalla completa (esto crea el pixelado) GL_LINEAR
		//fboPixelado.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
		fboPixelado.getTexture().setTextureMinMagFilter(GL_LINEAR, GL_LINEAR);
		fboPixelado.draw(0, 0, ofGetWidth(), ofGetHeight());
		
	} else {
		// Sin pixelado
		fbo.draw(0, 0, ofGetWidth(), ofGetHeight());
	}
	
	//pixelado por reverb
	
	ofDisableAlphaBlending();
	
	if ( showGUI ) gui.draw();
}

void ofApp::detectarColisiones() {
	// Comparar cada pelota con todas las demás
	for(int i = 0; i < pelotas.size(); i++) {
		for(int j = i + 1; j < pelotas.size(); j++) {
			// Obtener posiciones y radios
			ofVec2f pos1 = pelotas[i].getPos();
			ofVec2f pos2 = pelotas[j].getPos();
			float r1 = pelotas[i].getRadio();
			float r2 = pelotas[j].getRadio();
			
			// Calcular distancia entre centros
			float distancia = pos1.distance(pos2);
			float sumaRadios = r1 + r2;
			
			// Si la distancia es menor que la suma de radios, chocan
			if (distancia < sumaRadios) {
				// Calcular dirección de colisión
				ofVec2f direccion = (pos2 - pos1).normalize();
				
				// Intercambiar velocidades (rebote simple)
				ofVec2f vel1 = pelotas[i].getVel();
				ofVec2f vel2 = pelotas[j].getVel();
				
				pelotas[i].setVel(vel2);
				pelotas[j].setVel(vel1);
				
				// Separar pelotas para evitar que se peguen
				float solapamiento = sumaRadios - distancia;
				ofVec2f separacion = direccion * (solapamiento / 2 + 1);
				
				pelotas[i].setPos(pos1 - separacion);
				pelotas[j].setPos(pos2 + separacion);
			}
		}
	}
}

void ofApp::resetPelotas() {
	
	ofLogNotice() << "resetPelotas() -> limpiando y creando nuevas pelotas";
	
	safeAllNotesOff(midi);
	
	if(renovar) { pelotas.clear(); }

	ofRectangle marco(0,0,ofGetWidth(),ofGetHeight());
	
	// alternativa para el num de pelotas
	if (random) NUM_PELOTAS = (int)ofRandom(1,12);
	else NUM_PELOTAS = rangoRandom;
	
	ofLogNotice() << "Nuevo NUM_PELOTAS: " << NUM_PELOTAS;

	for (int i = 0; i < NUM_PELOTAS; i++) {
		//int tiempo = ofGetElapsedTimef();
		float radio = ofRandom(10.0f,50.0f);
		int nota = (int)ofMap(radio, 10.0f, 50.0f, 96, 24, true);
		
		int life = lifeSpan;
		Pelota p;
		p.setup(marco, &midi, nota, radio, life);
		
		if(centro){p.setPos(marco.getCenter());}
		else {p.pos.set(ofGetMouseX(), ofGetMouseY());}
		//p.setPos(marco.getCenter());
		//p.pos.set(ofGetMouseX(), ofGetMouseY()); // nacen en la posicion del mouse
		pelotas.push_back(p);

		ofLogNotice() << "Pelota " << i << " | radio: " << radio << " -> nota: " << nota;
	}

	esperandoReset = false;
}

void ofApp::keyPressed(int key){
	
	if(key == 'c') centro = !centro;
	if(key == 'r') renovar = !renovar;
	// rec y stop
	if(key == 'g') { record = !record;
		if(record) { midi.sendControlChange(11, 127);}
		else {
			midi.sendControlChange(11, 0);
			midi.sendControlChange(12, 127);
		     }
	}
	
	if(key == 'k') random = !random;
	if(key == 'n' || key == 'N') { resetPelotas(); }
	if(key == 'z') showGUI = !showGUI;
	if(key == 'w') { rangoRandom = ofClamp(rangoRandom + 1, 1, 12); }
	if(key == 'q') { rangoRandom = ofClamp(rangoRandom  - 1, 1, 12); }
	if(key == OF_KEY_UP) { factorVel = ofClamp(factorVel + 0.1, 0.1, 3); }
	if(key == OF_KEY_DOWN) { factorVel = ofClamp(factorVel - 0.1, 0.1, 3); }
	if(key == OF_KEY_RIGHT) { reverb = ofClamp(reverb + 1, 0, 100); }
	if(key == OF_KEY_LEFT) { reverb = ofClamp(reverb - 1, 0, 100); }
	if (key == 'a') {
			ataque = ofClamp(ataque + 0.1, 0.0, 20);
		}
	if (key == 'A') {
			ataque = ofClamp(ataque - 0.1, 0.0, 20);
		}
	if (key == 'd') {
			delay = ofClamp(delay + 1, 0.0, 100);
		}
	if (key == 'D') {
			delay = ofClamp(delay - 1, 0.0, 100);
		}
	if (key == 'f') {
			feedback = ofClamp(feedback + 1, 0.0, 150);
		}
	if (key == 'F') {
			feedback = ofClamp(feedback - 1, 0.0, 150);
		}
	
	if (key == 'i') {
			filtro = ofClamp(filtro + 1, 0.0, 100);
		}
	if (key == 'I') {
			filtro = ofClamp(filtro - 1, 0.0, 100);
		}
	
	if (key == 'l') {
			release = ofClamp(release + 0.5, 0.0, 60);
		}
		if (key == 'L') {
			release = ofClamp(release - 0.5, 0.0, 60);
		}
	if (key == 's') {
			distorsion = ofClamp(distorsion + 0.1, 0.0, 11);
		}
	if (key == 'S') {
			distorsion = ofClamp(distorsion - 0.1, 0.0, 11);
		}
	if (key == 't') {
			tiempo = ofClamp(tiempo + 40, 0, 2500);
		}
		if (key == 'T') {
			tiempo = ofClamp(tiempo - 40, 0, 2500);
		}
	if (key == 'v') {
			lifeSpan = ofClamp(lifeSpan + 10, 0, 2000);
		}
		if (key == 'V') {
			lifeSpan = ofClamp(lifeSpan - 10, 0, 2000);
		}
}

void ofApp::exit(){
	safeAllNotesOff(midi);
	midi.exit();}
