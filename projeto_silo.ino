#include <dht.h> //inclui biblioteca DHT

dht DHT;

#define DHT_pin A1 //DHT11 ligado ao pino analógico A1

// Conecte pino 1 do sensor (esquerda) ao +5V
// Conecte pino 2 do sensor ao pino de dados definido em seu Arduino
// Conecte pino 4 do sensor ao GND
// Conecte o resistor de 10K entre pin 2 (dados)
// e ao pino 1 (VCC) do sensor

#define Cooler 10 // o cooler tem que ser ligado na porta PWM
#define Vermelho 2 // LED vermelho
#define Verde 3 // LED verde
#define Chuva 4 // sensor de chuva no pino 4
#define Buzzer 5 //buzzer no pino 5
#define SensorInt A0   //definindo o sensor LM35 que ficará dentro do silo

int LeituraInt = 0; //variável inteira para representar a leitura do sensor de temperatura interna
int LeituraChuva = 0; //variável inteira para representar a leitura do sensor de chuva

float text = 0; //variável float para representar o valor de temperatura externa
float tint = 0; //variável float para representar o valor de temperatura interna
float umidade = 0; //variável float para representar o valor de umidade
float UE = 0; //variável float para representar a umidade de equilíbrio

#define a 0.339
#define b 0.059
#define c 30.205

void setup()
{
  pinMode(Vermelho, OUTPUT); //define os pinos de saída
  pinMode(Verde, OUTPUT);
  pinMode(Cooler, OUTPUT);
  pinMode(Chuva, INPUT);
  pinMode(Buzzer, OUTPUT);

  Serial.begin(9600); //inicia a comuniação serial numa taxa de dados de 9600 bps
  Serial.println("Aplicativo de manejo da aeração para o milho");

}

void loop() {

    LeituraInt = analogRead (SensorInt); //realiza a leitura analógica dentro do silo

    tint = (LeituraInt * ((float)5 / (float)1023) * 100); //converte a leitura em temperatura (°C)

    //leitura do DHT11 das condições externas
    DHT.read11(DHT_pin);
    text = DHT.temperature;
    umidade = DHT.humidity;

    //lê o sensor de chuva
    LeituraChuva = digitalRead(Chuva);

    UE = (a - (b * (log(-(text + 2 + c) * log(umidade / 100))))) * 100;

    // testa se retorno é valido, caso contrário aciona o buzzer
    if (tint > 70 or tint <= 0)
    {
      digitalWrite(Buzzer, HIGH);
      Serial.println("Rompimento de cabo interno!");
    }

    //se estiver funcionando ok cai nesse else
    else
    {
      digitalWrite(Buzzer, LOW);
      Serial.print("Temperatura interna: ");  //imprime todos os valores lidos no monitor serial
      Serial.print(tint);
      Serial.print("°C ");
      Serial.print("// Temperatura externa: ");
      Serial.print(text);
      Serial.print("°C");
      Serial.print("// Umidade relativa do ar externo: ");
      Serial.print(umidade);
      Serial.print("% ");
      Serial.print("// Chuva: ");
      if (LeituraChuva == 0) {
        Serial.print("Sim");
      }
      else {
        Serial.print("Não");
      }
      Serial.print("// Temperatura do ar de aeração: ");
      Serial.print(text + 2);
      Serial.print("°C");
      Serial.print("// Umidade de equilíbrio: ");
      Serial.print(UE);
      Serial.println("%");

      if ( 11.5 < UE and UE < 14.5 and Chuva == 0 and (text + 2) <= (tint + 4)) {
        digitalWrite(Cooler, HIGH);
        digitalWrite(Verde, HIGH);
        digitalWrite(Vermelho, LOW);
      }
      else {
        digitalWrite(Cooler, LOW);
        digitalWrite(Vermelho, HIGH);
        digitalWrite(Verde, LOW);
      }
    }
  delay(5000);
}
