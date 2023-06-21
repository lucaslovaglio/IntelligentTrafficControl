#include <Arduino.h>


unsigned long timeToRead = 2000;
unsigned long lastMeasure = 0;
unsigned long lastInterruption = 0;
unsigned long deltaUpdate = 0;

unsigned long timeInGreen = 5000; // 5 seg
unsigned long timeInYellow = 2000; // 2 seg
unsigned long timeInRed = timeInGreen;

bool resetStartTime = true;
unsigned long startTime = 0;

const int minDistance = 5;
const int minTime = 10000;

const int trafficLightsLength = 3;

bool trafficLightsState[trafficLightsLength] = {true, false, false};
int trafficLights[trafficLightsLength][trafficLightsLength] = {     // Tenemos 3 semaforos y 3 luces por cada uno. Luces del semaforo en orden G, Y, R. Semaforo 1 es de la ruta. 2 y 3 de la calle secundaria
  { 21, 19, 18}, //aca se deben poner los numeros de los pines
  { 5, 17, 16}, 
  { 4, 15, 2}
};

bool isInterrupt = false;
bool wasInterrupted = isInterrupt;
int interruptIndex = 0;

bool areTurnedOff = false;

const double sound_speed = 0.0343;    // in cm/microsecs

const int sensorsQty = 2;

const int sensors[sensorsQty][sensorsQty] = {
    {27, 14}, //{trig, echo} del sensor 1
    {26, 25}
};

/*
 *  Private functions
 */

static void
send_trigger( int index )
{
    digitalWrite(sensors[index][0], HIGH);
    delayMicroseconds(10);
    digitalWrite(sensors[index][0], LOW);
}

static long
get_pulse( int index )
{
    return pulseIn( sensors[index][1], HIGH );    // in microseconds
}

/*
 *  Public functions
 */

void trafficLightsOff(int indexes[], int size) 
{
    unsigned long currentTime = millis() - startTime;
    // Auxiliary variables for successive sums
    unsigned long yellowTime = timeInYellow;


    

    // Traffic signal logic:
    if (currentTime < yellowTime) {
        for (int i = 0; i < size; i++) {
            digitalWrite(trafficLights[indexes[i]][0], LOW);        // turn off the green LED
            digitalWrite(trafficLights[indexes[i]][1], HIGH);        // turn on the yellow LED
        }
    } else {
        for (int i = 0; i < size; i++) {
            trafficLightsState[indexes[i]] = false;
            digitalWrite(trafficLights[indexes[i]][1], LOW);         // turn off the yellow LED
            digitalWrite(trafficLights[indexes[i]][2], HIGH);        // turn on the red LED
        }
        areTurnedOff = true;
        startTime = millis(); // Reset the time at the end of the cycle
    }
}

void trafficFlow(int index) // si turn off es true, solo se ejecutaran las lineas para apagar el semaforo
{ 
    unsigned long currentTime = millis() - startTime;
    // Auxiliary variables for successive sums
    unsigned long yellowTime = timeInYellow;
    unsigned long greenTime = yellowTime + timeInGreen;
    unsigned long secondYellowTime = greenTime + timeInYellow;

    // Traffic signal logic:
    if (currentTime < yellowTime) {
        digitalWrite(trafficLights[index][2], LOW);         // turn off the red LED
        digitalWrite(trafficLights[index][1], HIGH);        // turn on the yellow LED
    } else if (currentTime < greenTime) {
        trafficLightsState[index] = true;
        digitalWrite(trafficLights[index][1], LOW);         // turn off the yellow LED
        digitalWrite(trafficLights[index][0], HIGH);         // turn on the green LED
    } else if (currentTime < secondYellowTime) {
        digitalWrite(trafficLights[index][0], LOW);        // turn off the green LED
        digitalWrite(trafficLights[index][1], HIGH);        // turn on the yellow LED
    } else {
        trafficLightsState[index] = false;
        digitalWrite(trafficLights[index][1], LOW);         // turn off the yellow LED
        digitalWrite(trafficLights[index][2], HIGH);        // turn on the red LED
        isInterrupt = false; // en caso que el que me haya llamado sea la interrupcion, aviso que llego al final
        startTime = millis(); // Reset the time at the end of the cycle
    }
}

void normalTrafficFlow (int index)
{
    unsigned long currentTime = millis() - startTime;
    // Auxiliary variables for successive sums
    unsigned long yellowTime = timeInYellow;

    // Traffic signal logic:
    if (currentTime < yellowTime) {
        digitalWrite(trafficLights[index][2], LOW);         // turn off the red LED
        digitalWrite(trafficLights[index][1], HIGH);        // turn on the yellow LED
    } else {
        trafficLightsState[index] = true;
        digitalWrite(trafficLights[index][1], LOW);         // turn off the yellow LED
        digitalWrite(trafficLights[index][0], HIGH);         // turn on the green LED
    }
}


double getDistance(int i) {
    send_trigger(i);
    long duration = get_pulse(i);
    double distance = duration * sound_speed / 2;
    return distance;
}


bool isEmpty(int i) 
{
    return getDistance(i) > minDistance;
}

void interruption(int index) 
{
    int lightsToTurnOffSize = trafficLightsLength - 1;
    int lightsToTurnOff[lightsToTurnOffSize];

    int j = 0;
    for (int i = 0; i < trafficLightsLength; i++) {
        if (j<lightsToTurnOffSize){
            if (i != index && trafficLightsState[i] == true) {
                lightsToTurnOff[j] = i;
                j++;
            }
        }
        else {
            break;
        }
    }



    if (!areTurnedOff) {
        trafficLightsOff(lightsToTurnOff, lightsToTurnOffSize);
    }
    
    trafficFlow(index);

    //una vez que se atendio la interrupcion, se resetean los valores
    if (!isInterrupt) {
        lastInterruption = millis();
        areTurnedOff = false;
    }
}


void setup()
{
    for (int i = 0; i < sensorsQty; i++) {
        pinMode(sensors[i][0], OUTPUT);          // trigPin as output
        digitalWrite(sensors[i][0], LOW);        // trigPin to low
        pinMode(sensors[i][1], INPUT);           // echoPin as input
    }

    for (int i = 0; i < trafficLightsLength; i++) {
        for (int j = 0; j < trafficLightsLength; j++) {
            pinMode(trafficLights[i][j], OUTPUT);
            if (j == trafficLightsLength - 1) {
                digitalWrite(trafficLights[i][j], HIGH); //arrancan todos en rojo
            } else {
                digitalWrite(trafficLights[i][j], LOW);
            }
            
        }
    }
    
    Serial.begin(BAUD);
    startTime = millis();
}

void loop() 
{
    deltaUpdate = millis() - lastInterruption; // tiempo para volver a revisar si hay un auto cerca
    
    if ((millis() - lastMeasure) > timeToRead) {
        for (int i = 0; i < sensorsQty; i++) {
            if(!isEmpty(i) && deltaUpdate >= minTime) { //si no se atendio ninguna interrupcion en los ultimos 30 seg, vuelve a interrumpir
                isInterrupt = true;
                interruptIndex = i + 1;
            }
        } 
        lastMeasure = millis();
    }

    if (wasInterrupted == false && isInterrupt == true) { // si la interrupcion o comenzo en este ciclo, se reinicia el tiempo
        startTime = millis();
    }   
    
    if (isInterrupt) {
        interruption(interruptIndex);
    } else {
        normalTrafficFlow(0);
    }

    wasInterrupted = isInterrupt;
}