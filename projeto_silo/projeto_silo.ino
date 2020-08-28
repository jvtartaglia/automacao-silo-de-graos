#include <DHT.h> //inclui biblioteca DHT

#define DHTPIN1 A1 //DHT externo
#define DHTPIN0 A0 //DHT interno
#define DHTTYPE DHT11 // define qual tipo de DHT será usado

// Conecte pino 1 do sensor (esquerda) ao +5V
// Conecte pino 2 do sensor ao pino de dados definido em seu Arduino
// Conecte pino 4 do sensor ao GND
// Conecte o resistor de 10K entre pin 2 (dados)
// e ao pino 1 (VCC) do sensor

DHT dht1(DHTPIN1, DHTTYPE);
DHT dht0(DHTPIN0, DHTTYPE);

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

// constantes para cálculo da umidade de equilíbrio higroscópico para milho
#define a 0.339
#define b 0.059
#define c 30.205

void setup()
{
  //define os pinos de saída e de entrada
  pinMode(Vermelho, OUTPUT);
  pinMode(Verde, OUTPUT);
  pinMode(Cooler, OUTPUT);
  pinMode(Chuva, INPUT);
  pinMode(Buzzer, OUTPUT);

  //inicia os sensores DHT
  dht0.begin();
  dht1.begin();

  Serial.begin(9600); //inicia a comuniação serial numa taxa de dados de 9600 bps
  Serial.println("Aplicativo de manejo da aeração para o milho"); // mensagem inicial

}

void loop() {

  LeituraInt = analogRead (SensorInt); //realiza a leitura analógica dentro do silo

  tint = (LeituraInt * ((float)5 / (float)1023) * 100); //converte a leitura em temperatura (°C)

  //leitura do DHT11 das condições externas
  umidade = dht1.readHumidity();
  text = dht1.readTemperature();

  //leitura do DHT11 das condições internas
  tint = dht0.readTemperature();
  float umidadeINT = dht0.readHumidity();

  //lê o sensor de chuva
  LeituraChuva = digitalRead(Chuva);

  //cálculo da umidade de equilíbrio
  UE = (a - (b * (log(-(text + 2 + c) * log(umidade / 100))))) * 100;

  // testa se retorno é valido, caso contrário aciona o buzzer
  // (condição do cabo)
  if (isnan(tint))
  {
    digitalWrite(Buzzer, HIGH);
    Serial.println("Rompimento de cabo interno!");
  }

  //se estiver funcionando ok, entra nessa lógica
  else
  {
    digitalWrite(Buzzer, LOW);

    //impressão dos valores lidos no monitor serial
    Serial.print("Temperatura interna: ");  
    Serial.print(tint);
    Serial.print("°C");
    Serial.print(" // UR interna: ");  
    Serial.print(umidadeINT);
    Serial.print("% ");
    Serial.print("// Temperatura externa: ");
    Serial.print(text);
    Serial.print("°C ");
    Serial.print("// UR externa: ");
    Serial.print(umidade);
    Serial.print("% ");
    Serial.print("// Chuva: ");

    //testa a leitura do sensor de chuva e imprime o resultado
    if (LeituraChuva == 0) { 
      Serial.print("Sim");
    }
    else {
      Serial.print("Não");
    }

    //continua as impressões dos demais valores
    Serial.print(" // Temperatura do ar de aeração: ");
    Serial.print(text + 2);
    Serial.print("°C ");
    Serial.print("// UE: ");
    Serial.print(UE);
    Serial.println("%");

    //esse SE controla se o ventilador irá ligar ou não
    if (11.5 < UE and UE < 14.5 and LeituraChuva == 1 and (text + 2) <= (tint + 4)) {
      digitalWrite(Cooler, HIGH);
      digitalWrite(Verde, HIGH);
      digitalWrite(Vermelho, LOW);

    // obs: se o ventilador for ligado o LED verde sera aceso
    // caso contrario, entre nesse ELSE e o LED vermelho sera ligado
    }
    else {
      digitalWrite(Cooler, LOW);
      digitalWrite(Vermelho, HIGH);
      digitalWrite(Verde, LOW);
    }
  }
  delay(5000); //delay de 5s para chamar void.loop novamente 
               //(intervalo entre leituras e tomada de decisão)
}
