
#   Project SemaforoInteligente

##  Descripcion
    El proyecto tiene como objetivo el control eficiente de los semáforos en una ruta principal que cruza con una calle intermedia, asegurando un flujo de tráfico fluido y seguro. Utilizaremos una placa ESP32 y sensores HC-SR04 para detectar la presencia de vehículos en la calle intermedia y ajustar automáticamente el funcionamiento de los semáforos.

###  Hardware

    2 Ultrasonic Sensor HC-SR04
    Jumper wires
    9 leds
    9 resistencias


###  Verification

    El semaforo 1 estara siempre en verde hasta que alguno de los sensores detecten una distancia menor a la minima establecida. En este momento se pondra el semaforo 1 en rojo y se activara la secuencia del semaforo correspondiente al sensor que se activo




