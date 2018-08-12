const bool DEVE_TESTAR = false;
const int DISTANCIA_SEGURA = 10;
const int PINO_MOTOR_DIREITA = 5; //pwm direito - cinza
const int PINO_MOTOR_ESQUERDA = 6; //pwm esquerdo - laranja
const int VOLTAGEM_MAXIMA_EM_SUPERFICIE_CLARA = 380;
const int VOLTAGEM_MINIMA_EM_CIMA_DA_LINHA_AZUL = 400; 
const int VOLTAGEM_MINIMA_EM_CIMA_DA_LINHA_PRETA = 700;
const float VELOCIDADE_DO_SOM = 29.4117647;

int para = 0;


//variáveis para o sensor ultrassônico
int gatilho = 2; // pino TRIG do sensor ultrassônico
int echo = 3; // pino ECHO do sensor ultrassônico
float tempo; // para armazenar o tempo de ida e volta do sinal em microsegundos
float distanciaEmCentimetros; // para armazenar a distância em centímetros


//variáveis para o sensor infravermelho
int sensorVelocidade = 0;
int sensorExternoEsquerda = 0;
int sensorInternoDireita = 0;
int sensorInternoEsquerda = 0;
int sensorExternoDireta = 0;

int valorMaximoSensorVelocidade;



void setup() {

    Serial.begin(38400);

    // Configuração do Arduino para o comando do Driver Motor L298N
    pinMode(9, OUTPUT); // Define pino D9 como saída - entrada I1 - motor direito - roxo
    pinMode(10, OUTPUT); // Define pino D10 como saída - entrada I2 - motor direito - azul
    pinMode(12, OUTPUT); // Define pino D12 como saída - entrada I3 - motor esquerdo - verde
    pinMode(11, OUTPUT); // Define pino D11 como saída - entrada I4 - motor esquerdo - amarelo
    pinMode(PINO_MOTOR_DIREITA, OUTPUT); // Define pino D5 como saída
    pinMode(PINO_MOTOR_ESQUERDA, OUTPUT); // define pine D6 como saída


    // Configuração do Arduino para o sensor ultrassônico HC-SR04
    pinMode(gatilho, OUTPUT); // Configura pino GATILHO como saída
    digitalWrite(gatilho, LOW); // Deixa pino em LOW
    delayMicroseconds(10);
    pinMode(echo, INPUT); // Configura pino ECHO como entrada
}

void TestarValoresDeEntradaDosSensores() {
    // Apresentar resultados no monitor serial
    Serial.print("sensor velocidade: ");
    Serial.println(sensorVelocidade);

    Serial.print("sensor Externo Direita: ");
    Serial.println(sensorExternoDireta);

    Serial.print("sensor Externo esquerda: ");
    Serial.println(sensorExternoEsquerda);

    Serial.print("sensor Interno Direita: ");
    Serial.println(sensorInternoDireita);

    Serial.print("sensor Interno Esquerda: ");
    Serial.println(sensorInternoEsquerda);
}

void TestarUltrassom(){
    // Apresentar resultados no monitor serial
    Serial.print("Distancia: ");
    Serial.print(distanciaEmCentimetros);
    Serial.println(" cm");
}

void loop() {

    LerEntradaDosSensores();
    DispararUltrasom();
    distanciaEmCentimetros = CalcularDistancia();
    

    if (distanciaEmCentimetros > DISTANCIA_SEGURA) {
        AjustarVelocidade();
        VerificarSensores(sensorInternoEsquerda, sensorInternoDireita, VirarParaDireita, VirarParaEsquerda);
        VerificarSensores(sensorExternoEsquerda, sensorExternoDireta, Virar90GrausDireita, Virar90GrausEsquerda);
    }
    else if (distanciaEmCentimetros < DISTANCIA_SEGURA) {
        Parar();
    }
    
    if(DEVE_TESTAR){
        TestarValoresDeEntradaDosSensores();
        TestarUltrassom();
    }
    
}

void LerEntradaDosSensores(){
    sensorVelocidade = analogRead(A0);
    sensorExternoEsquerda = analogRead(A1);
    sensorInternoDireita = analogRead(A2);
    sensorInternoEsquerda = analogRead(A3);
    sensorExternoDireta = analogRead(A4);
}

void MudarVelocidade(int velocidade){
    analogWrite(PINO_MOTOR_DIREITA, velocidade); //PWM configurado para 2V (40%) para o motor A
    analogWrite(PINO_MOTOR_ESQUERDA, velocidade); //PWM configurado para 2V (40%) para o motor B
}

float CalcularDistancia(){
    tempo = pulseIn(echo, HIGH); // Mede o tempo de ida e volta do pulso ultrassônico
    return tempo / VELOCIDADE_DO_SOM / 2; // Calcula as distâncias em centímetros    
}

void DispararUltrasom(){
    digitalWrite(gatilho, HIGH);
    delayMicroseconds(10);
    digitalWrite(gatilho, LOW);
}

void AjustarVelocidade(){
    if (sensorVelocidade < VOLTAGEM_MINIMA_EM_CIMA_DA_LINHA_PRETA) {
        MudarVelocidade(102);
    }
    else{
        MudarVelocidade(120);
    }
}

void VerificarSensores(int sensorEsquerda, int sensorDireita, void (*VirarDireta)(), void (*VirarEsquerda)())
{
    if (EstaDentroDaLinha(sensorEsquerda) && EstaDentroDaLinha(sensorDireita)) {
        Parar();
    } else {
        if (EstaDentroDaLinha(sensorEsquerda)) {
            VirarDireta();
        } else if (EstaDentroDaLinha(sensorDireita)) {
            VirarEsquerda();
        }
        else {
            AndarParaFrente();
        }
    }
}


void AguardarNaEstacao() {
    if (para == 0) {
        delay(1000);
        //frente
        analogWrite(PINO_MOTOR_DIREITA, 120); //PWM configurado para 3V (60%) para o motor A
        analogWrite(PINO_MOTOR_ESQUERDA, 120); //PWM configurado para 3V (60%) para o motor B
        digitalWrite(9, HIGH);
        digitalWrite(10, LOW);
        digitalWrite(11, HIGH);
        digitalWrite(12, LOW);
        delay(500);
        para = 0;
    }
}

void Parar() {
    analogWrite(PINO_MOTOR_DIREITA, 0); //PWM configurado para 0V (0%) para o motor A
    analogWrite(PINO_MOTOR_ESQUERDA, 0); //PWM configurado para 0V (0%) para o motor B
}

void VirarParaDireita() {
    digitalWrite(9, LOW);
    digitalWrite(10, LOW);
    digitalWrite(11, HIGH);
    digitalWrite(12, LOW);
}

void VirarParaEsquerda() {
    digitalWrite(9, HIGH);
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
    digitalWrite(12, LOW);
}

void Virar90GrausEsquerda() {
    analogWrite(PINO_MOTOR_DIREITA, 102); //PWM configurado para 2V (40%) para o motor A
    analogWrite(PINO_MOTOR_ESQUERDA, 102); //PWM configurado para 2V (40%) para o motor B
    digitalWrite(9, HIGH);
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
    digitalWrite(12, HIGH);
}

void Virar90GrausDireita() {
    analogWrite(PINO_MOTOR_DIREITA, 102); //PWM configurado para 2V (40%) para o motor A
    analogWrite(PINO_MOTOR_ESQUERDA, 102); //PWM configurado para 2V (40%) para o motor B
    digitalWrite(9, LOW);
    digitalWrite(10, HIGH);
    digitalWrite(11, HIGH);
    digitalWrite(12, LOW);
}

void AndarParaFrente() {
    digitalWrite(9, HIGH);
    digitalWrite(10, LOW);
    digitalWrite(11, HIGH);
    digitalWrite(12, LOW);
}

bool EstaDentroDaLinha(float valorDoSensor) {
    return valorDoSensor >= VOLTAGEM_MINIMA_EM_CIMA_DA_LINHA_AZUL;
}
