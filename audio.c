#include <alsa/asoundlib.h>
#include <math.h>
#include <stdio.h>
#include <complex.h>
#include <fftw3.h>

#define SAMPLE_RATE 44100
#define BUFFER_SIZE 1024
#define NUM_CHANNELS 1

// Función para calcular la frecuencia fundamental usando FFT
float calculate_fundamental_frequency(short *buffer, int size) {
    fftw_complex *in, *out;
    fftw_plan p;
    double max_magnitude = 0;
    int max_index = 0;
    
    // Preparar arrays para FFT
    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * size);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * size);
    p = fftw_plan_dft_1d(size, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    
    // Llenar el array de entrada
    for(int i = 0; i < size; i++) {
        in[i] = buffer[i] / 32768.0; // Normalizar
    }
    
    // Ejecutar FFT
    fftw_execute(p);
    
    // Encontrar el pico máximo (excluyendo DC)
    for(int i = 1; i < size/2; i++) {
        double magnitude = cabs(out[i]);
        if(magnitude > max_magnitude) {
            max_magnitude = magnitude;
            max_index = i;
        }
    }
    
    // Calcular frecuencia
    float frequency = (float)max_index * SAMPLE_RATE / size;
    
    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);
    
    return frequency;
}

int main() {
    int err;
    snd_pcm_t *capture_handle;
    snd_pcm_hw_params_t *hw_params;
    short buffer[BUFFER_SIZE];
    
    // Abrir dispositivo de captura
    if ((err = snd_pcm_open(&capture_handle, "default", 
                           SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        fprintf(stderr, "No se puede abrir dispositivo: %s\n", 
                snd_strerror(err));
        return 1;
    }
    
    // Configurar parámetros de hardware
    snd_pcm_hw_params_alloca(&hw_params);
    snd_pcm_hw_params_any(capture_handle, hw_params);
    snd_pcm_hw_params_set_access(capture_handle, hw_params,
                                SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(capture_handle, hw_params,
                                SND_PCM_FORMAT_S16_LE);
    unsigned int rate = SAMPLE_RATE;
    snd_pcm_hw_params_set_rate_near(capture_handle, hw_params,
                                   &rate, 0);
    snd_pcm_hw_params_set_channels(capture_handle, hw_params,
                                  NUM_CHANNELS);
    
    if ((err = snd_pcm_hw_params(capture_handle, hw_params)) < 0) {
        fprintf(stderr, "Error configurando parámetros: %s\n",
                snd_strerror(err));
        return 1;
    }
    
    printf("Capturando audio... Presiona Ctrl+C para detener.\n");
    
    while(1) {
        // Capturar audio
        if ((err = snd_pcm_readi(capture_handle, buffer, BUFFER_SIZE)) != BUFFER_SIZE) {
            fprintf(stderr, "Error leyendo audio: %s\n", snd_strerror(err));
            break;
        }
        
        // Calcular frecuencia
        float freq = calculate_fundamental_frequency(buffer, BUFFER_SIZE);
        
        if(freq > 50 && freq < 2000) { // Filtrar frecuencias razonables
            printf("Frecuencia detectada: %.2f Hz\n", freq);
        }
        
        // Pequeña pausa
        usleep(100000);
    }
    
    snd_pcm_close(capture_handle);
    return 0;
}
