#include <alsa/asoundlib.h>
#include <math.h>
#include <stdio.h>
#include <complex.h>
#include <fftw3.h>
#include <signal.h>
#include <unistd.h>

#define SAMPLE_RATE  44100
#define PERIOD_SIZE  512    // ~11.6 ms @ 44.1 kHz → ~86 frames/seg
#define NUM_CHANNELS 1

static volatile int keep_running = 1;

void sigint_handler(int sig) {
    (void)sig;
    keep_running = 0;
}

float calculate_fundamental_frequency(short *buffer, int size) {
    fftw_complex *in, *out;
    fftw_plan p;
    double max_magnitude = 0;
    int max_index = 0;

    in  = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * size);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * size);
    p   = fftw_plan_dft_1d(size, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    for (int i = 0; i < size; i++)
        in[i] = buffer[i] / 32768.0;

    fftw_execute(p);

    for (int i = 1; i < size / 2; i++) {
        double mag = cabs(out[i]);
        if (mag > max_magnitude) {
            max_magnitude = mag;
            max_index = i;
        }
    }

    float freq = (float)max_index * SAMPLE_RATE / size;

    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);

    return freq;
}

int main() {
    snd_pcm_t *capture_handle;
    snd_pcm_hw_params_t *hw;
    snd_pcm_sw_params_t *sw;
    short buffer[PERIOD_SIZE];
    int err;

    // Señal para salir limpio con Ctrl+C
    signal(SIGINT, sigint_handler);

    // --- Abrir dispositivo ---
    if ((err = snd_pcm_open(&capture_handle, "default",
                            SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        fprintf(stderr, "Error abriendo dispositivo: %s\n", snd_strerror(err));
        return 1;
    }

    // --- Configurar HW ---
    snd_pcm_hw_params_alloca(&hw);
    snd_pcm_hw_params_any(capture_handle, hw);
    snd_pcm_hw_params_set_access(capture_handle, hw,
                                 SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(capture_handle, hw,
                                 SND_PCM_FORMAT_S16_LE);

    unsigned int rate = SAMPLE_RATE;
    snd_pcm_hw_params_set_rate_near(capture_handle, hw, &rate, 0);

    snd_pcm_hw_params_set_channels(capture_handle, hw, NUM_CHANNELS);

    // *** DIFERENCIA CLAVE: configurar period_size ***
    // ALSA maneja un buffer circular dividido en "periods".
    // snd_pcm_readi() se desbloquea cuando UN period está lleno.
    // Así el kernel nos despierta automáticamente cada ~11.6 ms.
    snd_pcm_uframes_t period_size = PERIOD_SIZE;
    snd_pcm_hw_params_set_period_size_near(capture_handle, hw,
                                           &period_size, 0);

    // Buffer de 4 periods (para evitar underruns)
    snd_pcm_uframes_t buffer_size = PERIOD_SIZE * 4;
    snd_pcm_hw_params_set_buffer_size_near(capture_handle, hw,
                                           &buffer_size);

    if ((err = snd_pcm_hw_params(capture_handle, hw)) < 0) {
        fprintf(stderr, "Error configurando HW: %s\n", snd_strerror(err));
        return 1;
    }

    // --- Configurar SW (start threshold) ---
    snd_pcm_sw_params_alloca(&sw);
    snd_pcm_sw_params_current(capture_handle, sw);
    snd_pcm_sw_params_set_start_threshold(capture_handle, sw,
                                          PERIOD_SIZE);
    snd_pcm_sw_params(capture_handle, sw);

    // --- Preparar ---
    snd_pcm_prepare(capture_handle);

    printf("Capturando con period_size=%lu frames (~%.1f ms)\n",
           (unsigned long)period_size,
           1000.0 * period_size / SAMPLE_RATE);
    printf("Sin usleep — snd_pcm_readi bloquea hasta tener datos.\n");
    printf("Presiona Ctrl+C para detener.\n\n");

    // --- Bucle principal ---
    // snd_pcm_readi BLOQUEA naturalmente hasta que el period está lleno.
    // NO necesita usleep — el kernel nos despierta cuando hay datos.
    // Esto es idéntico a cómo funcionan Zoom/Teams/Meet por debajo.
    while (keep_running) {
        err = snd_pcm_readi(capture_handle, buffer, PERIOD_SIZE);
        if (err == -EPIPE) {
            // Buffer overrun — recuperar
            fprintf(stderr, "Overrun, recuperando...\n");
            snd_pcm_prepare(capture_handle);
            continue;
        } else if (err < 0) {
            fprintf(stderr, "Error leyendo: %s\n", snd_strerror(err));
            break;
        }

        float freq = calculate_fundamental_frequency(buffer, PERIOD_SIZE);

        if (freq > 50.0f && freq < 2000.0f)
            printf("\rFrecuencia: %7.2f Hz  ", freq);
    }

    printf("\n\nCerrando...\n");
    snd_pcm_close(capture_handle);
    return 0;
}
