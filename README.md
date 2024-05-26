# Sistemas de Computacion
Trabajo Practico 4 - Driver GPIO

## Character Device Driver (CDD)

**Integrantes:**
- [Gil Cernich, Manuel](https://github.com/manugcr/)
- [Pallardo, Agustin](https://github.com/djpallax)
- [Saporito, Franco](https://github.com/fasaporito)

---

## Objetivo
Para superar este TP tendrán que diseñar y construir un CDD que permita **sensar dos señales externas con un periodo de UN segundo**. Luego una aplicación a nivel de usuario deberá **leer UNA de las dos señales y graficarla** en función del tiempo. La aplicación tambien debe poder **indicarle al CDD cuál de las dos señales leer**. Las correcciones de escalas de las mediciones, de ser necesario, se harán a nivel de usuario. Los gráficos de la señal deben indicar el tipo de señal que se está sensando, unidades en abcisas y tiempo en ordenadas. Cuando se cambie de señal el gráfico se debe "resetear" y acomodar a la nueva medición.

Se recomienda utilizar una **Raspberry Pi** para desarrollar este TP.

Algunos ejemplos de drivers fueron proporcionados por la catedra desde el repositorio en gitlab [device-drivers-main](https://gitlab.com/sistemas-de-computacion-unc/device-drivers/)

---

## Que es un Driver?
Un **driver** es un software que permite que el sistema operativo se comunique con el hardware de la computadora. Actúa como un traductor entre el sistema operativo y el hardware, facilitando la ejecución de comandos y la transferencia de datos. Los drivers son esenciales para el funcionamiento correcto de los dispositivos, ya que sin ellos el sistema operativo no podría reconocer ni utilizar el hardware.

### Que es un Device Driver?
Un **device driver** es un tipo de driver que permite que el sistema operativo controle y utilice un dispositivo hardware específico, como una impresora, una tarjeta gráfica o un disco duro. Cada tipo de hardware tiene su propio device driver, que proporciona la interfaz necesaria para que el sistema operativo y el dispositivo se entiendan mutuamente.

El device driver tiene dos partes, una parte específica del dispositivo y otra parte específica del sistema operativo.

### Que es un Device Controller?
Un **device controller** es un componente de hardware que actúa como intermediario entre el dispositivo físico y el sistema operativo. Este controlador maneja las operaciones del dispositivo, traduce las señales de los drivers en acciones físicas (como mover un cabezal de lectura/escritura en un disco duro) y gestiona la transferencia de datos entre el dispositivo y la memoria principal de la computadora.

El device driver se comunica con el device controller para enviar comandos y recibir datos desde el dispositivo. El driver traduce las instrucciones del sistema operativo en comandos que el controller puede entender y ejecutar.

El device controller muchas veces necesita de un driver propio, llamado **bus driver**. Este es un tipo de driver que permite que el sistema operativo se comunique con un bus de hardware, como un bus PCI o USB.

### Que es un Bus Driver?
Un **bus driver** es un tipo de driver que permite que el sistema operativo se comunique con un bus de hardware, como un bus PCI o USB. Los buses de hardware son canales de comunicación que conectan los dispositivos de hardware con la CPU y la memoria de la computadora. Los bus drivers proporcionan la interfaz necesaria para que el sistema operativo pueda detectar, configurar y comunicarse con los dispositivos conectados al bus.

<p align="center">
  <img src="./imgs/bus_drivers.png"><br>
  <em>Fig 1. Diagrama drivers y buses.</em>
</p>

---

## Character Device Driver
Un character device driver (CDD) es un tipo de driver en un sistema operativo que gestiona dispositivos que transfieren datos de forma secuencial, carácter por carácter. Los ejemplos comunes de dispositivos de carácter incluyen teclados, mouse y puertos serie . A diferencia de los block device drivers que manejan grandes bloques de datos a la vez (como discos duros), los character device drivers gestionan la transmisión y recepción de datos en pequeñas unidades, típicamente bytes.

Para utilizar un CDD, se utilizan los Character Device Files (CDF) que son archivos especiales en el sistema de archivos que representan dispositivos de carácter. Estos archivos se utilizan para enviar comandos y datos al dispositivo, y para recibir datos del dispositivo. Los CDF se crean y gestionan mediante el sistema de archivos virtual /dev en sistemas basados en Unix.

### Indice <Major, Minor>
En los sistemas operativos Unix y Linux, los dispositivos se representan mediante archivos especiales en el directorio /dev. Estos archivos especiales permiten que los programas interactúen con los dispositivos de hardware como si fueran archivos normales. Cada uno de estos archivos especiales tiene asignado un par de números conocido como major number y minor number. 
- **Major Number**: Identifica el driver del dispositivo. Cada tipo de dispositivo tiene un driver asociado que maneja las operaciones de entrada y salida. El número mayor le dice al kernel de Linux cuál driver usar para comunicarse con el dispositivo. Por ejemplo, todos los discos duros pueden tener el mismo major number porque están gestionados por el mismo driver de disco.
- **Minor Number**: Se utiliza para identificar instancias específicas del dispositivo que el driver maneja. Si un driver maneja múltiples dispositivos, el número menor ayuda a distinguir entre ellos. Por ejemplo, si tienes varios discos duros, cada uno tendrá el mismo major number pero un minor number diferente para diferenciarlos.

Podemos verlos ejecutando el comando `ls -l /dev` en la terminal, donde `b` o `c` identifican si es un block device o un character device respectivamente, y el par major-minor se encuentra en las columnas 5 y 6.

```bash
manu@manu:~$ ls -l /dev
brw-rw----  1 root disk      8,    17 May 26 12:07 sdb1
brw-rw----  1 root disk      8,    18 May 26 12:07 sdb2
brw-rw----  1 root disk      8,    19 May 26 12:07 sdb3
crw--w----  1 root tty       4,    10 May 26 12:07 tty10
crw--w----  1 root tty       4,    11 May 26 12:07 tty11
crw--w----  1 root tty       4,    12 May 26 12:07 tty12
```
## Escritura y Lectura de un CDD
Para escribir y leer un CDD se utilizan las funciones `write()` y `read()` respectivamente. Estas funciones se utilizan para enviar y recibir datos desde el dispositivo. La función `write()` se utiliza para enviar datos al dispositivo, mientras que la función `read()` se utiliza para recibir datos del dispositivo.

```bash
open("/dev/ttyS0", O_RDWR);
write(fd, "Hello, Device!\n", 15);
read(fd, buffer, sizeof(buffer));
```

Tambien es posible utilizar la terminal para interactuar con el CDD, utilizando el comando `echo` para escribir en el dispositivo y `cat` para leer del dispositivo.

```bash
cat /dev/ttyS0
echo "Hello, Device!" > /dev/ttyS0
```

---

## Creacion de un Character Device Driver
Para crear y probar nuestro CDD, vamos a utilizar una Raspberry Pi 3B+ con el sistema operativo Raspbian 64bits.



---

## Bibliografia

- [Raspberry Pi Kernel Development - LowLevelLearning](https://www.youtube.com/watch?v=lWzFFusYg6g&list=PLc7W4b0WHTAX4F1Byvs4Bp7c8yCDSiKa9)
- [Raspberry Pi C Kernel - LowLevelLearning](https://www.youtube.com/watch?v=mshVdGlGwBs)