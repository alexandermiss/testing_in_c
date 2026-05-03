# README - Programa de Captura y Análisis de Audio en C

Este programa en C utiliza la biblioteca ALSA (Advanced Linux Sound Architecture) para capturar audio desde el dispositivo predeterminado y realiza un análisis de frecuencia en tiempo real usando FFT (Transformada Rápida de Fourier).

## Descripción del Código

### Encabezados e Inclusiones
El programa incluye varios encabezados importantes:

- `<alsa/asoundlib.h>`: Proporciona funciones para la interfaz ALSA.
- `<math.h>`: Funciones matemáticas básicas.
- `<stdio.h>`: Entrada/salida estándar en C.
- `<complex.h>`: Manejo de números complejos.
- `<fftw3.h>`: Biblioteca FFTW para operaciones rápidas de Fourier.

### Constantes Predefinidas
- `SAMPLE_RATE`: Frecuencia de muestreo configurada en 44.1 kHz (calidad de audio estándar).
- `BUFFER_SIZE`: Cantidad de muestras leídas por iteración (1024).
- `NUM_CHANNELS`: Número de canales configurados, en este caso, mono (1 canal).

### Función `calculate_fundamental_frequency`
Esta función calcula la frecuencia fundamental del audio capturado mediante FFT. Sus pasos principales son:
1. Reservar memoria para los arreglos de entrada (`in`) y salida (`out`) de la FFT.
2. Normalizar los datos de entrada dividiendo por 32768.0.
3. Ejecutar el algoritmo de la FFT.
4. Identificar el índice con la mayor magnitud (excluyendo el componente DC).
5. Calcular la frecuencia correspondiente usando la fórmula:

   \[ \text{frecuencia} = \frac{\text{índice")}{\text{size}} \times \text{SAMPLE\_RATE} \]
6. Liberar los recursos utilizados y devolver el resultado calculado.

### Función principal `main`
La función principal realiza los siguientes pasos:

1. **Abrir Dispositivo de Captura:**
   - Usando `snd_pcm_open`, se abre el dispositivo predeterminado de captura de audio.
   - En caso de error, el programa termina y muestra un mensaje.

2. **Configurar Parámetros de Hardware:**
   - Se configuran el acceso, formato de datos, frecuencia de muestreo, y número de canales usando las funciones ALSA.

3. **Captura de Audio en un Bucle:**
   - Dentro de un bucle infinito:
     - Captura un fragmento de audio con `snd_pcm_readi`.
     - Calcula la frecuencia fundamental llamando a `calculate_fundamental_frequency`.
     - Filtra las frecuencias "razonables" (entre 50 Hz y 2000 Hz).
     - Muestra la frecuencia detectada en la salida estándar.

4. **Liberar Recursos:**
   - Cierra el dispositivo de captura con `snd_pcm_close` al finalizar.

### Límite Superior e Inferior de Frecuencia Detectada
Se establecen límites de 50 Hz y 2000 Hz para filtrar valores irrelevantes o fuera del rango típico de audio.

## Requisitos de Dependencias
- **ALSA**: Biblioteca de audio en sistemas Linux. Instalar usando `sudo apt-get install libasound2-dev` en sistemas basados en Debian.
- **FFTW**: Biblioteca para cálculos FFT. Instalar usando `sudo apt-get install libfftw3-dev` en sistemas basados en Debian.

## Ejecución
1. Compilar el programa usando `gcc` con las banderas necesarias:

   ```bash
   gcc -o audio audio.c -lasound -lfftw3 -lm
   ```

2. Ejecutar el binario generado:

   ```bash
   ./audio
   ```

   Esto iniciará la captura de audio y mostrará la frecuencia detectada en tiempo real.

3. Detener la ejecución usando `Ctrl + C`.

## Notas Adicionales
- Este programa está diseñado para dispositivos de entrada de audio monocanal (mono).
- Asegúrate de que el dispositivo predeterminado de audio esté correctamente configurado en tu sistema antes de ejecutar este programa.
- Una pausa (`usleep(100000)`) asegura que no se saturen los tiempos de lectura/escritura.

---

¡Disfruta usando este programa para análisis básico de frecuencias de audio!

