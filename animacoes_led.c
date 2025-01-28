#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"

//arquivo .pio
#include "animacoes_led.pio.h"

//número de LEDs
#define NUM_PIXELS 25

//pino de saída
#define OUT_PIN 19

// GPIO do buzzer
#define BUZZER 21 // Buzzer na GPIO 21

//Variáveis Globais
PIO pio;
uint sm;

// Frequências das notas (em Hz)
#define DO 261
#define RE 293
#define MI 329
#define FA 349
#define SOL 392
#define LA 440
#define SI 493
#define DO_OCTAVE 523

// Definições do teclado matricial
#define ROWS 4
#define COLS 4

// Define as linhas e colunas do teclado matricial (ajustado com os novos pinos)
uint row_pins[ROWS] = {8, 7, 6, 5};    // GPIOs para as linhas (R1 a R4)
uint col_pins[COLS] = {4, 3, 2, 1};    // GPIOs para as colunas (C1 a C4)

// Mapa das teclas do teclado matricial
char KEY_MAP[16] = {
    '1', '2', '3', 'A',
    '4', '5', '6', 'B',
    '7', '8', '9', 'C',
    '*', '0', '#', 'D'
};

//Vetores de imagens - Valores devem variar de 0 a 1 (de acordo com a porcentagem de intensidade)

//Animação 1
double seta_1[25] =   {0.0, 0.0, 1.0, 0.0, 0.0,
                       0.0, 1.0, 1.0, 1.0, 0.0, 
                       1.0, 1.0, 1.0, 1.0, 1.0,
                       0.0, 0.0, 1.0, 0.0, 0.0,
                       0.0, 0.0, 1.0, 0.0, 0.0};

double seta_2[25] =   {0.0, 0.0, 1.0, 0.0, 0.0,
                       0.0, 0.0, 1.0, 0.0, 0.0, 
                       1.0, 1.0, 1.0, 1.0, 1.0,
                       0.0, 1.0, 1.0, 1.0, 0.0,
                       0.0, 0.0, 1.0, 0.0, 0.0};

double carinha_feliz[25] =  {0.0, 0.0, 0.0, 0.0, 0.0,
                             0.0, 1.0, 0.0, 1.0, 0.0, 
                             0.0, 0.0, 0.0, 0.0, 0.0,
                             1.0, 0.0, 0.0, 0.0, 1.0,
                             0.0, 1.0, 1.0, 1.0, 0.0};

double letra_t[25] =   {1.0, 1.0, 1.0, 1.0, 1.0,
                        0.0, 0.0, 1.0, 0.0, 0.0, 
                        0.0, 0.0, 1.0, 0.0, 0.0,
                        0.0, 0.0, 1.0, 0.0, 0.0,
                        0.0, 0.0, 1.0, 0.0, 0.0};

double letra_m[25] =   {1.0, 0.0, 0.0, 0.0, 1.0,
                        1.0, 1.0, 0.0, 1.0, 1.0, 
                        1.0, 0.0, 1.0, 0.0, 1.0,
                        1.0, 0.0, 0.0, 0.0, 1.0,
                        1.0, 0.0, 0.0, 0.0, 1.0};
                   
double coracao1[25] = {0.0, 1.0, 0.0, 1.0, 0.0,
                       1.0, 0.0, 1.0, 0.0, 1.0, 
                       1.0, 0.0, 0.0, 0.0, 1.0,
                       0.0, 1.0, 0.0, 1.0, 0.0,
                       0.0, 0.0, 1.0, 0.0, 0.0};

double coracao2[25] = {0.0, 1.0, 0.0, 1.0, 0.0,
                       1.0, 0.5, 1.0, 0.5, 1.0, 
                       1.0, 0.5, 0.5, 0.5, 1.0,
                       0.0, 1.0, 0.5, 1.0, 0.0,
                       0.0, 0.0, 1.0, 0.0, 0.0};

double coracao3[25] =  {0.3, 1.0, 0.3, 1.0, 0.3,
                        0.3, 1.0, 1.0, 1.0, 0.3, 
                        0.3, 1.0, 1.0, 1.0, 0.3,
                        0.3, 0.0, 0.0, 0.0, 0.3,
                        0.3, 0.3, 1.0, 0.3, 0.3};

double coracao4[25] =  {1.0, 1.0, 0.0, 1.0, 1.0,
                        1.0, 0.0, 1.0, 0.0, 1.0, 
                        0.0, 1.0, 0.0, 1.0, 0.0,
                        0.0, 0.0, 1.0, 0.0, 0.0,
                        0.0, 0.0, 0.0, 0.8, 0.8};

double coracao5[25] =  {0.5, 0.7, 0.5, 0.7, 0.5,
                        0.7, 0.0, 0.7, 0.0, 0.7, 
                        0.7, 0.0, 0.5, 0.0, 0.7,
                        0.0, 0.7, 0.5, 0.7, 0.0,
                        0.5, 0.5, 0.7, 0.5, 0.5}; 

//frames paran animação de setas
double setas1[25] = {         0.0, 0.0, 1.0, 0.0, 0.0,
                              0.0, 1.0, 1.0, 1.0, 0.0,
                              1.0, 1.0, 1.0, 1.0, 1.0,
                              0.0, 0.0, 1.0, 0.0, 0.0,
                              0.0, 0.0, 1.0, 0.0, 0.0};

double setas2[25] = {         0.0, 0.0, 1.0, 0.0, 0.0,
                              0.0, 1.0, 1.0, 1.0, 0.0,
                              1.0, 1.0, 1.0, 1.0, 1.0,
                              0.0, 1.0, 1.0, 1.0, 0.0,
                              0.0, 0.0, 1.0, 0.0, 0.0};

double setas3[25] = {         0.0, 0.0, 1.0, 0.0, 0.0,
                              0.0, 1.0, 1.0, 0.0, 0.0,
                              1.0, 1.0, 1.0, 1.0, 1.0,
                              0.0, 1.0, 1.0, 0.0, 0.0,
                              0.0, 0.0, 1.0, 0.0, 0.0};

double setas4[25] = {         0.0, 0.0, 1.0, 0.0, 0.0,
                              0.0, 0.0, 1.0, 0.0, 0.0,
                              1.0, 1.0, 1.0, 1.0, 1.0,
                              0.0, 1.0, 1.0, 1.0, 0.0,
                              0.0, 0.0, 1.0, 0.0, 0.0};

double setas5[25] = {         0.0, 0.0, 1.0, 0.0, 0.0,
                              0.0, 0.0, 1.0, 1.0, 0.0,
                              1.0, 1.0, 1.0, 1.0, 1.0,
                              0.0, 0.0, 1.0, 1.0, 0.0,
                              0.0, 0.0, 1.0, 0.0, 0.0}; 


// ADICIONE SUAS IMAGENS  

// DESENHO PARA AS LEDs BRANCAS COM 20% DE INTENSIDADE
double leds_cor_branca[25] = {0.2, 0.2, 0.2, 0.2, 0.2,
                             0.2, 0.2, 0.2, 0.2, 0.2, 
                             0.2, 0.2, 0.2, 0.2, 0.2,
                             0.2, 0.2, 0.2, 0.2, 0.2,
                             0.2, 0.2, 0.2, 0.2, 0.2};

// DESENHO PARA APAGAR AS LEDS
double leds_desligados[25] = {0.0, 0.0, 0.0, 0.0, 0.0,
                              0.0, 0.0, 0.0, 0.0, 0.0, 
                              0.0, 0.0, 0.0, 0.0, 0.0,         
                              0.0, 0.0, 0.0, 0.0, 0.0,          
                              0.0, 0.0, 0.0, 0.0, 0.0};    

// ANIMAÇÃO QUADRADO CRESCENTE - VERMELHO
double desenhar_quadrado_V1[25] = {0.0, 0.0, 0.0, 0.0, 0.0,
                                  0.0, 0.0, 0.0, 0.0, 0.0, 
                                  0.0, 0.0, 1.0, 0.0, 0.0,         
                                  0.0, 0.0, 0.0, 0.0, 0.0,          
                                  0.0, 0.0, 0.0, 0.0, 0.0};  

                              
double desenhar_quadrado_V2[25] = {0.0, 0.0, 0.0, 0.0, 0.0,
                                  0.0, 1.0, 1.0, 1.0, 0.0, 
                                  0.0, 1.0, 0.0, 1.0, 0.0,         
                                  0.0, 1.0, 1.0, 1.0, 0.0,          
                                  0.0, 0.0, 0.0, 0.0, 0.0}; 

 
double desenhar_quadrado_V3[25] = {1.0, 1.0, 1.0, 1.0, 1.0,
                                   1.0, 0.0, 0.0, 0.0, 1.0, 
                                   1.0, 0.0, 0.0, 0.0, 1.0,         
                                   1.0, 0.0, 0.0, 0.0, 1.0,          
                                   1.0, 1.0, 1.0, 1.0, 1.0}; 

// ANIMAÇÃO QUADRADO CRESCENTE - AZUL
double desenhar_quadrado_A1[25] = {0.0, 0.0, 0.0, 0.0, 0.0,
                                  0.0, 0.0, 0.0, 0.0, 0.0, 
                                  0.0, 0.0, 1.0, 0.0, 0.0,         
                                  0.0, 0.0, 0.0, 0.0, 0.0,          
                                  0.0, 0.0, 0.0, 0.0, 0.0};  

                              
double desenhar_quadrado_A2[25] = {0.0, 0.0, 0.0, 0.0, 0.0,
                                  0.0, 1.0, 1.0, 1.0, 0.0, 
                                  0.0, 1.0, 0.0, 1.0, 0.0,         
                                  0.0, 1.0, 1.0, 1.0, 0.0,          
                                  0.0, 0.0, 0.0, 0.0, 0.0}; 

 
double desenhar_quadrado_A3[25] = {1.0, 1.0, 1.0, 1.0, 1.0,
                                   1.0, 0.0, 0.0, 0.0, 1.0, 
                                   1.0, 0.0, 0.0, 0.0, 1.0,         
                                   1.0, 0.0, 0.0, 0.0, 1.0,          
                                   1.0, 1.0, 1.0, 1.0, 1.0};                  


//Animação 6
    double raio [25] =      {0.0, 0.0, 0.0, 1.0, 0.0,
                            0.0, 0.0, 1.0, 0.0, 0.0,
                            0.0, 1.0, 1.0, 1.0, 1.0,
                            0.0, 0.0, 1.0, 0.0, 0.0,
                            0.0, 0.0, 0.0, 0.0, 1.0};
double carinha_triste[25] = {1.0, 1.0, 0.0, 1.0, 1.0,
                             1.0, 1.0, 0.0, 1.0, 1.0, 
                            0.0, 0.0, 0.0, 1.0, 0.0,
                             0.0, 1.0, 1.0, 1.0, 0.0,
                            1.0, 0.0, 0.0, 0.0, 1.0};

double fogo_pequeno[25] =   {0.0, 0.0, 1.0, 0.0, 0.0, 
                            0.0, 1.0, 1.0, 1.0, 0.0, 
                            0.0, 1.0, 1.0, 1.0, 0.0, 
                            0.0, 0.0, 1.0, 0.0, 0.0,
                             0.0, 0.0, 0.0, 0.0, 0.0};
double fogo_medio[25] =     {0.0, 1.0, 1.0, 1.0, 0.0,
                            1.0, 1.0, 1.0, 1.0, 1.0,
                            1.0, 1.0, 1.0, 1.0, 1.0,
                            0.0, 1.0, 1.0, 1.0, 0.0,
                            0.0, 0.0, 1.0, 0.0, 0.0};
double carinha_alegre[25] = {1.0, 1.0, 0.0, 1.0, 1.0,
                            1.0, 1.0, 0.0, 1.0, 1.0,
                            0.0, 0.0, 1.0, 0.0, 0.0,
                            1.0, 1.0, 1.0, 1.0, 1.0,
                            0.0, 1.0, 1.0, 1.0, 0.0};


// Função para tocar uma nota específica
void Tocar_nota(int nota, int duracao_ms) {
    int periodo = 1000000 / nota; // Calcula o período da onda (em microssegundos)
    int ciclos = (nota * duracao_ms) / 1000; // Calcula o número de ciclos a serem tocados

    for (int i = 0; i < ciclos; i++) {
        gpio_put(BUZZER, true);  // Liga o buzzer
        sleep_us(periodo / 2);    // Meio ciclo (high)
        gpio_put(BUZZER, false); // Desliga o buzzer
        sleep_us(periodo / 2);    // Meio ciclo (low)
    }
}

// Função para tocar uma melodia
void Tocar_melodia() {
    // Melodia (sequência de notas e duração em ms)
    int melodia[][2] = {
        {DO, 400}, {RE, 400}, {MI, 400}, {FA, 400},
        {SOL, 400}, {LA, 400}, {SI, 400}, {DO_OCTAVE, 800},
        {SI, 400}, {LA, 400}, {SOL, 400}, {FA, 400},
        {MI, 400}, {RE, 400}, {DO, 800}
    };

    for (int i = 0; i < sizeof(melodia) / sizeof(melodia[0]); i++) {
        int nota = melodia[i][0];
        int duracao = melodia[i][1];
        Tocar_nota(nota, duracao); // Toca cada nota
    }
}

// Função para tocar apagando
void Tocar_apagando() {
    // Melodia (sequência de notas e duração em ms)
    int melodia[][2] = {
        {DO, 400}
    };

    for (int i = 0; i < sizeof(melodia) / sizeof(melodia[0]); i++) {
        int nota = melodia[i][0];
        int duracao = melodia[i][1];
        Tocar_nota(nota, duracao); // Toca cada nota
    }
}
// possível colocar outras funções para buzzer

//imprimir valor binário
void imprimir_binario(int num) {
    for (int i = 31; i >= 0; i--) {
        (num & (1 << i)) ? printf("1") : printf("0");
    }
}

//rotina da interrupção
static void gpio_irq_handler(uint gpio, uint32_t events){
    printf("Interrupção ocorreu no pino %d, no evento %d\n", gpio, events);
    printf("HABILITANDO O MODO GRAVAÇÃO");
    reset_usb_boot(0, 0); //habilita o modo de gravação do microcontrolador
}

//Rotina para definição de cores do led
uint32_t matrix_rgb(double r, double g, double b) {
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

//Rotina para acionar a matriz de LEDs - ws2812b
//Basta colocar a matriz desenho[25] e escolher uma das cores
void desenho_pio(double *desenho, int cor){
    uint32_t valor_led;
    if (cor == 1) { //liga todos os LEDs na cor vermelha
        for (int16_t i = 0; i < NUM_PIXELS; i++) {
        uint32_t valor_led = matrix_rgb(desenho[24-i], 0.0, 0.0);
        pio_sm_put_blocking(pio, sm, valor_led);
        }
    } else if (cor==2) { //liga todos os LEDs na cor amarela
        for (int16_t i = 0; i < NUM_PIXELS; i++) {
        valor_led = matrix_rgb(desenho[24-i], desenho[24-i], 0.0);
        pio_sm_put_blocking(pio, sm, valor_led);
        } 
    } else if (cor==3) { //liga todos os LEDs na cor verde
        for (int16_t i = 0; i < NUM_PIXELS; i++) {
            valor_led = matrix_rgb(0.0, desenho[24-i], 0.0);
            pio_sm_put_blocking(pio, sm, valor_led);
        }
    } else if (cor==4) { //liga todos os LEDs na cor ciano
        for (int16_t i = 0; i < NUM_PIXELS; i++) {
            valor_led = matrix_rgb(0.0, desenho[24-i], desenho[24-i]);
            pio_sm_put_blocking(pio, sm, valor_led);
        }
    } else if (cor==5) { //liga todos os LEDs na cor azul
        for (int16_t i = 0; i < NUM_PIXELS; i++) {
            valor_led = matrix_rgb(0.0, 0.0, desenho[24-i]);
            pio_sm_put_blocking(pio, sm, valor_led);
        }
    } else if (cor==6) { //liga todos os LEDs na cor magenta
        for (int16_t i = 0; i < NUM_PIXELS; i++) {
            valor_led = matrix_rgb(desenho[24-i], 0.0, desenho[24-i]);
            pio_sm_put_blocking(pio, sm, valor_led);
        }
    } else if (cor==7) { //liga todos os LEDs na cor branca
        for (int16_t i = 0; i < NUM_PIXELS; i++) {
            valor_led = matrix_rgb(desenho[24-i], desenho[24-i], desenho[24-i]);
            pio_sm_put_blocking(pio, sm, valor_led);
        }
    } else { //Desliga todos os LEDs
        for (int16_t i = 0; i < NUM_PIXELS; i++) {
            valor_led = matrix_rgb(0.0, 0.0, 0.0);
            pio_sm_put_blocking(pio, sm, valor_led);
        }
    }
    imprimir_binario(valor_led);
}

//Animação de Kauan
const double ondas_coloridas[25][3] = { // Matriz de cores para a animação "Ondas Coloridas"
    {1.0, 0.0, 0.0}, {0.8, 0.2, 0.0}, {0.6, 0.4, 0.0}, {0.4, 0.6, 0.0}, {0.2, 0.8, 0.0},
    {0.0, 1.0, 0.0}, {0.0, 0.8, 0.2}, {0.0, 0.6, 0.4}, {0.0, 0.4, 0.6}, {0.0, 0.2, 0.8},
    {0.0, 0.0, 1.0}, {0.2, 0.0, 0.8}, {0.4, 0.0, 0.6}, {0.6, 0.0, 0.4}, {0.8, 0.0, 0.2},
    {1.0, 0.0, 0.0}, {0.8, 0.2, 0.0}, {0.6, 0.4, 0.0}, {0.4, 0.6, 0.0}, {0.2, 0.8, 0.0},
    {0.0, 1.0, 0.0}, {0.0, 0.8, 0.2}, {0.0, 0.6, 0.4}, {0.0, 0.4, 0.6}, {0.0, 0.2, 0.8},
};

void animacao_kauan(uint16_t ciclos, uint16_t delay_ms) { // Função para exibir uma animação de ondas coloridas nos LEDs
    uint16_t num_cores = sizeof(ondas_coloridas) / sizeof(ondas_coloridas[0]);

    for (uint16_t ciclo = 0; ciclo < ciclos; ciclo++) {
        for (uint16_t frame = 0; frame < num_cores; frame++) {
            // Configura cada LED com base no deslocamento atual
            for (uint16_t i = 0; i < NUM_PIXELS; i++) {
                uint32_t valor_led = matrix_rgb(
                    ondas_coloridas[(frame + i) % num_cores][0],
                    ondas_coloridas[(frame + i) % num_cores][1],
                    ondas_coloridas[(frame + i) % num_cores][2]
                );
                pio_sm_put_blocking(pio, sm, valor_led);
            }
            // Pausa para criar o efeito de animação
            sleep_ms(delay_ms);
        }
    }

    printf("Animação 'Ondas Coloridas' finalizada após %d ciclos.\n", ciclos);
}

// Implementação da opção 'B'
void leds_azuis_100() {
    uint32_t buffer[NUM_PIXELS];// Cria um buffer
    uint32_t valor_led = matrix_rgb(0.0, 0.0, 1.0); // Preenche o buffer com a cor azul (100%)
    for (int16_t i = 0; i < NUM_PIXELS; i++) {
        buffer[i] = valor_led;
    }
    for (int16_t i = 0; i < NUM_PIXELS; i++) { // Envia os dados para todos os LEDs ao mesmo tempo
        pio_sm_put_blocking(pio, sm, buffer[i]);
    }
    printf("Todos os LEDs foram configurados para azul com intensidade de 100%%.\n");
}

// Implementação da opção 'C'
void leds_vermelhos_80() {
    uint32_t buffer[NUM_PIXELS]; // Cria um buffer
    uint32_t valor_led = matrix_rgb(0.8, 0.0, 0.0); // Preenche o buffer com a cor vermelha (80%)
    for (int16_t i = 0; i < NUM_PIXELS; i++) {
        buffer[i] = valor_led;
    }
    for (int16_t i = 0; i < NUM_PIXELS; i++) { // Envia os dados para todos os LEDs ao mesmo tempo
        pio_sm_put_blocking(pio, sm, buffer[i]);
    }
    printf("Todos os LEDs foram configurados para vermelho com intensidade de 80%%.\n");
}

// Implementação da opção 'D'
void leds_verdes_50() {
    uint32_t buffer[NUM_PIXELS]; // Cria um buffer
    uint32_t valor_led = matrix_rgb(0.0, 0.5, 0.0); // Preenche o buffer com a cor verde (50%)
    for (int16_t i = 0; i < NUM_PIXELS; i++) {
        buffer[i] = valor_led;
    }
    for (int16_t i = 0; i < NUM_PIXELS; i++) { // Envia os dados para todos os LEDs ao mesmo tempo
        pio_sm_put_blocking(pio, sm, buffer[i]);
    }
    printf("Todos os LEDs foram configurados para verde com intensidade de 50%%.\n");
}

//Função para dar reboot na placa (tecla '*' ).
    void reboot() {
    reset_usb_boot(0,0);
}

// Verifica se uma tecla foi pressionada no teclado matricial
char scan_keypad() {
    for (int row = 0; row < ROWS; row++) {
        gpio_put(row_pins[row], 0); // Ativa a linha atual
        for (int col = 0; col < COLS; col++) {
            if (gpio_get(col_pins[col]) == 0) { // Verifica se a coluna está ativa
                printf("Tecla detectada: %c\n", KEY_MAP[row * COLS + col]);
                gpio_put(row_pins[row], 1); // Restaura a linha
                return KEY_MAP[row * COLS + col];
            }
        }
        gpio_put(row_pins[row], 1); // Restaura a linha
    }
    return '\0'; // Nenhuma tecla pressionada
}

// Inicializa os pinos do teclado matricial
void init_keypad() {
    for (int i = 0; i < ROWS; i++) {
        gpio_init(row_pins[i]);
        gpio_set_dir(row_pins[i], GPIO_OUT);
        gpio_put(row_pins[i], 1);
    }
    for (int i = 0; i < COLS; i++) {
        gpio_init(col_pins[i]);
        gpio_set_dir(col_pins[i], GPIO_IN);
        gpio_pull_up(col_pins[i]);
    }
}

// Função para mostrar o menu
void menu() {
    printf("\nMenu de Opcoes:\n");
    printf("Escolha uma opcao pressionando a tecla correspondente...\n");
    printf("1 - Seta para Cima, Seta para Baixo, Carinha Feliz, Letra T e Letra M\n");
    printf("2 - Coracão Piscando\n"); 
    printf("6 - Rosto triste para alegre\n"); 
    printf("7 - Quadrado Crescente\n");  
    printf("8 - Pong ATARI 1972\n");
    printf("9 - Circulo Giratório\n");
    printf("4 - Setas\n");
    printf("B - Desenho a definir\n");
    printf("C - Desenho a definir\n"); // ADICIONE O NOME DA SUA IMAGEM
    printf("D - Desenho a definir\n");
    printf("* - Desenho a definir\n");
    printf("# - LEDs \n");
}

// Função para ligar LEDs brancas com 20% de intensidade
void leds_brancas(int n){
    desenho_pio(leds_cor_branca, n);
}
// Função para desligar LEDs 
void desligar_leds(){
    desenho_pio(leds_desligados, 0);
}

void animação_alexandro(int n) {
    desenho_pio(desenhar_quadrado_V1, n);
    sleep_ms(350);
    desenho_pio(desenhar_quadrado_V2, n);
    sleep_ms(350);
    desenho_pio(desenhar_quadrado_V3, n);
    sleep_ms(350);
    desenho_pio(desenhar_quadrado_A1, n);
    sleep_ms(350);
    desenho_pio(desenhar_quadrado_A2, n);
    sleep_ms(350);
    desenho_pio(desenhar_quadrado_A3, n);
    sleep_ms(350);
}
void animação_mariana(int n) {
    desenho_pio(seta_1, n);
    sleep_ms(600);
    Tocar_apagando(); 
    desenho_pio(leds_desligados, n);
    sleep_ms(100);
    desenho_pio(seta_2, n);
    sleep_ms(600);
    Tocar_apagando();
    desenho_pio(leds_desligados, n);
    sleep_ms(100);                 
    desenho_pio(carinha_feliz, n);
    sleep_ms(600);
    Tocar_apagando(); 
    desenho_pio(leds_desligados, n);
    sleep_ms(100);
    desenho_pio(letra_t, n);
    sleep_ms(600);
    desenho_pio(leds_desligados, n);
    sleep_ms(100);
    Tocar_apagando();                
    desenho_pio(letra_m, n);    
}

void animacao_helen(){
    desenho_pio(coracao1, 1);  
    sleep_ms(500);              
    desenho_pio(coracao2, 2);  
    sleep_ms(500);              
    desenho_pio(coracao3, 3);  
    sleep_ms(500);              
    desenho_pio(coracao4, 4);  
    sleep_ms(500);              
    desenho_pio(coracao5, 5);  
}

// Animação das Setas
void animacao_Lucas(){

    desenho_pio(setas1, 1);  
    sleep_ms(500);   
    Tocar_apagando();          
    desenho_pio(setas3, 2);  
    sleep_ms(500);    
    Tocar_apagando();         
    desenho_pio(setas4, 3);  
    sleep_ms(500);    
   Tocar_apagando();         
    desenho_pio(setas5, 4);  
    sleep_ms(500);     
    Tocar_apagando();         
    desenho_pio(setas2, 3); 
    sleep_ms(500);        
    Tocar_apagando();      

    }

void animação_daniel(int n) {
    desenho_pio(raio, n+3);
    sleep_ms(400);
    desenho_pio(carinha_triste, n+4);
    sleep_ms(400);                
    desenho_pio(fogo_pequeno, n+1);
    sleep_ms(400);
    desenho_pio(fogo_medio, n+1);
    sleep_ms(400);                
    desenho_pio(carinha_alegre, n+1);
    sleep_ms(400);
    
}

void animacao_ylo(uint16_t ciclos, uint16_t delay_ms) {
    const double circulo[5][25] = {
        {0.0, 0.0, 1.0, 0.0, 0.0,
         0.0, 1.0, 0.0, 1.0, 0.0,
         1.0, 0.0, 0.0, 0.0, 1.0,
         0.0, 1.0, 0.0, 1.0, 0.0,
         0.0, 0.0, 1.0, 0.0, 0.0},

        {0.0, 1.0, 1.0, 1.0, 0.0,
         1.0, 0.0, 0.0, 0.0, 1.0,
         1.0, 0.0, 0.0, 0.0, 1.0,
         1.0, 0.0, 0.0, 0.0, 1.0,
         0.0, 1.0, 1.0, 1.0, 0.0},

        {1.0, 1.0, 1.0, 1.0, 1.0,
         1.0, 0.0, 0.0, 0.0, 1.0,
         1.0, 0.0, 0.0, 0.0, 1.0,
         1.0, 0.0, 0.0, 0.0, 1.0,
         1.0, 1.0, 1.0, 1.0, 1.0},

        {0.5, 0.5, 0.5, 0.5, 0.5,
         0.5, 0.0, 0.0, 0.0, 0.5,
         0.5, 0.0, 0.0, 0.0, 0.5,
         0.5, 0.0, 0.0, 0.0, 0.5,
         0.5, 0.5, 0.5, 0.5, 0.5},

        {0.2, 0.2, 0.2, 0.2, 0.2,
         0.2, 0.0, 0.0, 0.0, 0.2,
         0.2, 0.0, 0.0, 0.0, 0.2,
         0.2, 0.0, 0.0, 0.0, 0.2,
         0.2, 0.2, 0.2, 0.2, 0.2}
    };

    for (uint16_t ciclo = 0; ciclo < ciclos; ciclo++) {
        for (uint16_t frame = 0; frame < 5; frame++) {
            desenho_pio((double *)circulo[frame], (frame % 7) + 1); // Alterna cores entre as disponíveis
            sleep_ms(delay_ms);
        }
    }
    printf("Animação 'Círculo Giratório' finalizada após %d ciclos.\n", ciclos);
}

// Função para converter a posição do matriz para uma posição do vetor.
int getIndex(int x, int y) {
    // Se a linha for par (0, 2, 4), percorremos da esquerda para a direita.
    // Se a linha for ímpar (1, 3), percorremos da direita para a esquerda.
    if (y % 2 == 0) {
        return 24-(y * 5 + x); // Linha par (esquerda para direita).
    } else {
        return 24-(y * 5 + (4 - x)); // Linha ímpar (direita para esquerda).
    }
}

void animacaoPONG_ISRAELFALCAO() {
    // Definição das matrizes de cores para a animação
    int matrizes[8][5][5][3] = {
        {
            {{0, 0, 0}, {0, 0, 255}, {0, 0, 255}, {0, 0, 255}, {0, 0, 0}},
            {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
            {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
            {{0, 0, 0}, {0, 0, 0}, {0, 0, 255}, {0, 0, 0}, {0, 0, 0}},
            {{0, 0, 0}, {0, 0, 255}, {0, 0, 255}, {0, 0, 255}, {0, 0, 0}}
        },
        // Adicione as outras matrizes aqui...
    };

    // Limite de ciclos para evitar loop infinito
    int ciclos = 10;

    for (int ciclo = 0; ciclo < ciclos; ciclo++) {
        for (int quadro = 0; quadro < 8; quadro++) {
            // Exibe o quadro atual
            for (int linha = 0; linha < 5; linha++) {
                for (int coluna = 0; coluna < 5; coluna++) {
                    int posicao = getIndex(linha, coluna);
                    uint32_t valor_led = matrix_rgb(
                        matrizes[quadro][coluna][linha][0] / 255.0,
                        matrizes[quadro][coluna][linha][1] / 255.0,
                        matrizes[quadro][coluna][linha][2] / 255.0
                    );
                    pio_sm_put_blocking(pio, sm, valor_led);
                }
            }
            sleep_ms(500); // Tempo de exibição do quadro

            // Limpa os LEDs
            desenho_pio(leds_desligados, 0);
        }
    }

    printf("Animação Pong finalizada.\n");
}
//função principal
int main() {
    pio = pio0; 
    bool ok;
    uint32_t valor_led;
    double r = 0.0, b = 0.0, g = 1.0;
    
   // Configura clock
    ok = set_sys_clock_khz(128000, false);
    stdio_init_all();

    printf("Iniciando a transmissão PIO\n");
    if (ok) printf("Clock set to %ld\n", clock_get_hz(clk_sys));

    // Configurações da PIO
    uint offset = pio_add_program(pio, &animacoes_led_program);
    uint sm = pio_claim_unused_sm(pio, true);
    animacoes_led_program_init(pio, sm, offset, OUT_PIN);

    // Configura buzzer
    gpio_init(BUZZER);
    gpio_set_dir(BUZZER, GPIO_OUT);

    // Inicializa teclado matricial
    init_keypad();

    // Mostra o menu
    menu();

    while (true) {
        char key = scan_keypad();
        
        if (key != 0) {  // Verifica se alguma tecla foi pressionada
            switch (key) {

            case 'A':  // Desliga todos os LEDs
                desligar_leds();
                printf("LEDs desligados.\n");   
            break;

            case 'B':  // Liga todos os LEDs como azul com intensidade 100%
                leds_azuis_100();
                printf("LEDs azuis ligados com intensidade de 100%%.\n");   
            break;

            case 'C':  // Liga todos os LEDs como vermelho com intensidade 80%
                leds_vermelhos_80();
                printf("LEDs vermelhos ligados com intensidade de 80%%.\n");   
            break;

            case 'D':  // Liga todos os LEDs como verde com intensidade 50%
                leds_verdes_50();
                printf("LEDs verdes ligados com intensidade de 50%%.\n");   
            break;

            case '#':  // Liga todos os LEDs como branco com intensidade 20%
                leds_brancas(7);
                printf("LEDs brancos ligados com intensidade de 20%%.\n");   
            break;
            
            case '*':  // Raspberry sai do modo de execução e habilita o modo de gravação (reboot)
                reboot();
                printf("Regravação do Raspberry Pi Pico W inciada.\n");   
            break;
            
            case '1':  // Animação da Mariana
                animação_mariana(5); // Adiconada a rotina aqui
                printf("Animação do botão 1 foi acionada.\n");
            break;

            case '2':  // Animação da Helen
                animacao_helen(); 
                printf("Animação do botão 2 foi acionada.\n");
            break;

            case '3':  // Animação do Kauan
                animacao_kauan(2, 40);
                printf("Animação do botão 3 foi acionada.\n");
            break;

            case '4':  // Animação do Lucas
                animacao_Lucas();
                printf("Animação do botão 4 foi acionada.\n");
            break;

            case '5':  // Animação da Edna
                // Adiconar rotina aqui.
                printf("Animação do botão 5 foi acionada.\n");
            break;

            case '6':  // Animação do Daniel
                animação_daniel(1); 
                printf("Animação do botão 6 foi acionada.\n");
            break;

            case '7':  // Animação do Alexandro
                animação_alexandro(3); // Adiconada a rotina aqui 
                animação_alexandro(5); // Adiconada a rotina aqui
                printf("Animação do botão 7 foi acionada.\n");
            
            break;

            case '8':  // Animação do Israel
                animacaoPONG_ISRAELFALCAO();
                printf("Animação do botão 8 foi acionada.\n");
            break;

            case '9':  // Animação do Ylo
                animacao_ylo(3, 150); // 3 ciclos com 150 ms entre frames
                printf("Animação do botão 9 foi acionada.\n");
            break;

            case '0':  // Melodia criada pela Mariana
                Tocar_melodia(); // Toca a melodia
                printf("Melodia musical foi acionada.\n");
            break;

            default:
                printf("Tecla %c pressionada, sem acao atribuida.\n", key);
            break;
            } 
        }

        sleep_ms(100);
        printf("\nFrequência de clock %ld\r\n", clock_get_hz(clk_sys));
    }
}
