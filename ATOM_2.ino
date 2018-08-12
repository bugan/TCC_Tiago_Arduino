const bool DEVE_TESTAR = false;

const int velr = 5; //pwm direito - cinza
const int vell = 6; //pwm esquerdo - laranja
int para = 0;


//variáveis para o sensor ultrassônico
int gatilho = 2; // pino TRIG do sensor ultrassônico
int echo = 3; // pino ECHO do sensor ultrassônico
float tempo; // para armazenar o tempo de ida e volta do sinal em microsegundos
float distancia_cm; // para armazenar a distância em centímetros


//variáveis para o sensor infravermelho
int sensorValue0 = 0;
int sensorValue1 = 0;
int sensorValue2 = 0;
int sensorValue3 = 0;
int sensorValue4 = 0;

#define linha 380 //cor máxima para a superfície
#define LinhaBaixa 400 //cor mínima para a faixa azul
#define LinhaAlta 700 //cor mínima para a faixa preta

void setup() {

    Serial.begin(38400);

    // Configuração do Arduino para o comando do Driver Motor L298N
    pinMode(9, OUTPUT); // Define pino D9 como saída - entrada I1 - motor direito - roxo
    pinMode(10, OUTPUT); // Define pino D10 como saída - entrada I2 - motor direito - azul
    pinMode(12, OUTPUT); // Define pino D12 como saída - entrada I3 - motor esquerdo - verde
    pinMode(11, OUTPUT); // Define pino D11 como saída - entrada I4 - motor esquerdo - amarelo
    pinMode(velr, OUTPUT); // Define pino D5 como saída
    pinMode(vell, OUTPUT); // define pine D6 como saída


    // Configuração do Arduino para o sensor ultrassônico HC-SR04
    pinMode(gatilho, OUTPUT); // Configura pino GATILHO como saída
    digitalWrite(gatilho, LOW); // Deixa pino em LOW
    delayMicroseconds(10);
    pinMode(echo, INPUT); // Configura pino ECHO como entrada
}

void TestarValoresDeEntradaDosSensores(){
    // Apresentar resultados no monitor serial
    Serial.print("sensor0: ");
    Serial.println(sensorValue0);
    Serial.print("sensor1: ");
    Serial.println(sensorValue1);
    Serial.print("sensor2: ");
    Serial.println(sensorValue2);
    Serial.print("sensor3: ");
    Serial.println(sensorValue3);
    Serial.print("sensor4: ");
    Serial.println(sensorValue4);
}

void loop() {

    sensorValue0 = analogRead(A0);
    sensorValue1 = analogRead(A1);
    sensorValue2 = analogRead(A2);
    sensorValue3 = analogRead(A3);
    sensorValue4 = analogRead(A4);




    // Disparo do pulso ultrassônico
    digitalWrite(gatilho, HIGH);
    delayMicroseconds(10);
    digitalWrite(gatilho, LOW);

    tempo = pulseIn(echo, HIGH); // Mede o tempo de ida e volta do pulso ultrassônico

    distancia_cm = tempo / 29.4117647 / 2; // Calcula as distâncias em centímetros

    /*// Apresentar resultados no monitor serial
    Serial.print("Distancia: ");
    Serial.print(distancia_cm);
    Serial.println(" cm");*/

    // Verificação e comando do protótipo de AGV
    if (distancia_cm > 10) {

        if (sensorValue0 < LinhaBaixa) {
            //condição de arranque

            analogWrite(velr, 102); //PWM configurado para 2V (40%) para o motor A
            analogWrite(vell, 102); //PWM configurado para 2V (40%) para o motor B


        }

        if (sensorValue0 >= LinhaBaixa && sensorValue0 < LinhaAlta) {

            analogWrite(velr, 102); //PWM configurado para 2V (40%) para o motor A
            analogWrite(vell, 102); //PWM configurado para 2V (40%) para o motor B
        }
        else if (sensorValue0 >= LinhaAlta) {

            analogWrite(velr, 120); //PWM configurado para 3V (60%) para o motor A
            analogWrite(vell, 120); //PWM configurado para 3V (60%) para o motor B
        }
        VerificarSensoresInternos();
        VerificarSensoresExternos();

    }
    else if (distancia_cm < 10) {
        //para
        analogWrite(velr, 0); //PWM configurado para 0V (0%) para o motor A
        analogWrite(vell, 0); //PWM configurado para 0V (0%) para o motor B
    }
}

void VerificarSensoresInternos(){
    if (!EstaForaDaLinha(sensorValue2) && !EstaForaDaLinha(sensorValue3)) {
        //anda para frente
        digitalWrite(9, HIGH);
        digitalWrite(10, LOW);
        digitalWrite(11, HIGH);
        digitalWrite(12, LOW);
    }
    else if (EstaForaDaLinha(sensorValue2) && !EstaForaDaLinha(sensorValue3)) {
        //vira para esquerda
        digitalWrite(9, HIGH);
        digitalWrite(10, LOW);
        digitalWrite(11, LOW);
        digitalWrite(12, LOW);
    }
    else if (!EstaForaDaLinha(sensorValue2) && EstaForaDaLinha(sensorValue3)) {
        //vira para direita
        digitalWrite(9, LOW);
        digitalWrite(10, LOW);
        digitalWrite(11, HIGH);
        digitalWrite(12, LOW);
    }
}

void VerificarSensoresExternos(){

    if (EstaForaDaLinha(sensorValue1) && !EstaForaDaLinha(sensorValue4)) {
        //vira para esquerda (90º)
        analogWrite(velr, 102); //PWM configurado para 2V (40%) para o motor A
        analogWrite(vell, 102); //PWM configurado para 2V (40%) para o motor B
        digitalWrite(9, HIGH);
        digitalWrite(10, LOW);
        digitalWrite(11, LOW);
        digitalWrite(12, HIGH);
    }
    else if (!EstaForaDaLinha(sensorValue1)&& EstaForaDaLinha(sensorValue4)) {
        //vira para direita (90º)
        analogWrite(velr, 102); //PWM configurado para 2V (40%) para o motor A
        analogWrite(vell, 102); //PWM configurado para 2V (40%) para o motor B
        digitalWrite(9, LOW);
        digitalWrite(10, HIGH);
        digitalWrite(11, HIGH);
        digitalWrite(12, LOW);
    }
    else if (EstaForaDaLinha(sensorValue1) && EstaForaDaLinha(sensorValue4)) {
        //para
        analogWrite(velr, 0); //PWM configurado para 0V (0%) para o motor A
        analogWrite(vell, 0); //PWM configurado para 0V (0%) para o motor B
        if (para == 0){
            delay(1000);
            //frente
            analogWrite(velr, 120); //PWM configurado para 3V (60%) para o motor A
            analogWrite(vell, 120); //PWM configurado para 3V (60%) para o motor B
            digitalWrite(9, HIGH);
            digitalWrite(10, LOW);
            digitalWrite(11, HIGH);
            digitalWrite(12, LOW);
            delay(500);
            para = 0;
        }
    }
}

bool EstaForaDaLinha(float valorDoSensor){
    return valorDoSensor > linha;
}
