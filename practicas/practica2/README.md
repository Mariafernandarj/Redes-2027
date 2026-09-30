# Practica 2 - Capa 2

## Compilación

Desde la carpeta `practica2`, ejecutar el siguiente comando:

```bash
gcc receptor.c -o receptor
```

```bash
gcc emisor.c -o emisor 
```

## Ejecución
Antes de ejecutar necesitas saber la interfaz y Mac que debes de usar:
Ejecuta comando para ver la interfaz:
```bash
$ ip link
```
Lo que esperamos ver:
```bash
...
3: wlo1:...
    link/ether 56:0c:71:db:87:f1 ...
```


Computadora 1
```bash
sudo ./receptor 'interfaz del mismo equipo'
```

Ejemplo
```bash
sudo ./receptor wlp1s0
```

Computadora 2
```bash
sudo ./emisor 'interfaz' "Mac_que_se_debe_usar" "Hola capa 2"
```

Ejemplo
```bash
sudo ./emisor wlo1 52:99:46:7f:de:94 "Hola desde la computadora 2"
```
Nota : Recuerda que la si vas a usar el emisor en la computadora 2 el Mac que debes de usar es el de la computadora 1 y viceversa

## Investigación
# pcap_open_live
Permite abrir una interfaz de red para comenzar a capturar paquetes

# pcap_sendpacket
Envía un paquete de datos directamente a través de la interfaz de red abierta

# pcap_loop
Ejecuta una función de procesamiento cada vez que se captura un paquete.
Puede continuar indefinidamente o hasta alcanzar un número determinado de paquetes

# cap_close
Libera los recursos asociados con una captura abierta mediante pcap_open_live()

# pcap_geterr
Devuelve un mensaje describiendo el último error asociado con una operación de libpcap
