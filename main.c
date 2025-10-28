#include <Adafruit_NeoPixel.h>

#define NUM_LEDS 30
#define DATA_PIN_1 3
#define DATA_PIN_2 4
#define DATA_PIN_3 5

Adafruit_NeoPixel fita1(NUM_LEDS, DATA_PIN_1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel fita2(NUM_LEDS, DATA_PIN_2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel fita3(NUM_LEDS, DATA_PIN_3, NEO_GRB + NEO_KHZ800);

// Cores para cultivo
uint32_t corVermelha = Adafruit_NeoPixel::Color(255, 40, 40);
uint32_t corAzul     = Adafruit_NeoPixel::Color(40, 40, 255);
uint32_t corRoxa     = Adafruit_NeoPixel::Color(180, 40, 255);

// Variáveis de tempo configuráveis
unsigned long tempoLigado   = 18UL * 60UL * 60UL * 1000UL; // padrão: 18h
unsigned long tempoDesligado = 6UL * 60UL * 60UL * 1000UL; // padrão: 6h

// Controle de estado
unsigned long tempoAnterior = 0;
bool ledsLigados = true;

void setup() {
  Serial.begin(9600);

  fita1.begin(); fita1.show();
  fita2.begin(); fita2.show();
  fita3.begin(); fita3.show();

  ligarLeds();
  tempoAnterior = millis();

  Serial.println("Sistema iniciado com tempo em escala 24000.");
  Serial.println("Use: Time_Set <tempoLigado> <tempoDesligado>");
  Serial.println("Exemplo: Time_Set 18000 6000  → Liga por 18h, desliga por 6h");
}

void loop() {
  verificarComandoSerial();

  unsigned long agora = millis();

  if (ledsLigados && (agora - tempoAnterior >= tempoLigado)) {
    apagarLeds();
    ledsLigados = false;
    tempoAnterior = agora;
    Serial.println("LEDs DESLIGADOS");
  }

  if (!ledsLigados && (agora - tempoAnterior >= tempoDesligado)) {
    ligarLeds();
    ledsLigados = true;
    tempoAnterior = agora;
    Serial.println("LEDs LIGADOS");
  }
}

void ligarLeds() {
  for (int i = 0; i < NUM_LEDS; i++) {
    fita1.setPixelColor(i, corVermelha);
    fita2.setPixelColor(i, corAzul);
    fita3.setPixelColor(i, corRoxa);
  }
  fita1.show(); fita2.show(); fita3.show();
}

void apagarLeds() {
  for (int i = 0; i < NUM_LEDS; i++) {
    fita1.setPixelColor(i, 0);
    fita2.setPixelColor(i, 0);
    fita3.setPixelColor(i, 0);
  }
  fita1.show(); fita2.show(); fita3.show();
}

// Lê comando do tipo Time_Set 18000 6000
void verificarComandoSerial() {
  if (Serial.available()) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();

    if (comando.startsWith("Time_Set")) {
      unsigned int ligadoUnidades, desligadoUnidades;
      if (sscanf(comando.c_str(), "Time_Set %u %u", &ligadoUnidades, &desligadoUnidades) == 2) {
        if (ligadoUnidades <= 24000 && desligadoUnidades <= 24000) {
          tempoLigado = ligadoUnidades * 3600UL;     // 1 unidade = 3600ms
          tempoDesligado = desligadoUnidades * 3600UL;

          Serial.print("Novo tempo configurado:\n - Ligado por ");
          Serial.print(ligadoUnidades);
          Serial.print(" unidades (~");
          Serial.print((ligadoUnidades * 36) / 60);
          Serial.println(" minutos)");

          Serial.print(" - Desligado por ");
          Serial.print(desligadoUnidades);
          Serial.print(" unidades (~");
          Serial.print((desligadoUnidades * 36) / 60);
          Serial.println(" minutos)");
        } else {
          Serial.println("Erro: valores devem ser de 0 a 24000.");
        }
      } else {
        Serial.println("Formato inválido. Use: Time_Set <tempoLigado> <tempoDesligado>");
      }
    }
  }
}
