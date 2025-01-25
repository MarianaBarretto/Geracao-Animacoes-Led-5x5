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

//vetor para criar imagem na matriz de led - 1
double carinha_feliz_piscando[25] =   {0.0, 0.0, 0.0, 0.0, 0.0,
                                       0.0, 3.0, 0.0, 3.0, 0.0, 
                                       0.0, 0.0, 0.0, 0.0, 0.0,
                                       3.0, 0.0, 0.0, 0.0, 3.0,
                                       0.0, 3.0, 3.0, 3.0, 0.0};

double carinha_feliz_piscando_1[25] =   {0.0, 0.0, 0.0, 0.0, 0.0,
                                         0.0, 0.0, 0.0, 3.0, 0.0, 
                                         0.0, 0.0, 0.0, 0.0, 0.0,
                                         3.0, 0.0, 0.0, 0.0, 3.0,
                                         0.0, 3.0, 3.0, 3.0, 0.0};

double carinha_feliz_piscando_2[25] =   {0.0, 0.0, 0.0, 0.0, 0.0,
                                         0.0, 3.0, 0.0, 3.0, 0.0, 
                                         0.0, 0.0, 0.0, 0.0, 0.0,
                                         3.0, 0.0, 0.0, 0.0, 3.0,
                                         0.0, 3.0, 3.0, 3.0, 0.0};

double carinha_feliz_piscando_3[25] =   {0.0, 0.0, 0.0, 0.0, 0.0,
                                         0.0, 0.0, 0.0, 3.0, 0.0, 
                                         0.0, 0.0, 0.0, 0.0, 0.0,
                                         3.0, 0.0, 0.0, 0.0, 3.0,
                                         0.0, 3.0, 3.0, 3.0, 0.0};

double carinha_feliz_piscando_4[25] =  {0.0, 0.0, 0.0, 0.0, 0.0,
                                        0.0, 3.0, 0.0, 3.0, 0.0, 
                                        0.0, 0.0, 0.0, 0.0, 0.0,
                                        3.0, 0.0, 0.0, 0.0, 3.0,
                                        0.0, 3.0, 3.0, 3.0, 0.0};

double carinha_feliz_piscando_5[25] =  {0.0, 0.0, 0.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 3.0, 0.0, 
                                        0.0, 0.0, 0.0, 0.0, 0.0,
                                        3.0, 0.0, 0.0, 0.0, 3.0,
                                        0.0, 3.0, 3.0, 3.0, 0.0};

double carinha_feliz_piscando_6[25] =  {0.0, 0.0, 0.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 0.0, 
                                        0.0, 0.0, 0.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 0.0};                                                                                                                                  

                   
// ADICIONE SUAS IMAGENS



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

// Função para tocar uma melodia
void Tocar_piscando() {
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

//rotina para definição da intensidade de cores do led
uint32_t matrix_rgb(double b, double r, double g) {
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

//rotina para acionar a matrix de leds - ws2812b
void desenho_pio(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b){
    for (int16_t i = 0; i < NUM_PIXELS; i++) {
        if (desenho[24 - i] == 1) { 
            valor_led = matrix_rgb(b = 1.0, r = 0.0, g = 0.0); // Led Azul
        } else if (desenho[24 - i] == 2) { 
            valor_led = matrix_rgb(b = 0.0, r = 1.0, g = 0.0); // Led Vermelho
        } else if (desenho[24 - i] == 3) { 
            valor_led = matrix_rgb(b = 0.0, r = 0.0, g = 1.0); // Led Verde 
        } else if (desenho[24 - i] == 4) { 
            valor_led = matrix_rgb(b = 0.0, r = 1.0, g = 1.0); // Led Amarelo
        } else if (desenho[24 - i] == 5) { 
            valor_led = matrix_rgb(b = 1.0, r = 0.0, g = 1.0); // Led Ciano
        } else if (desenho[24 - i] == 6) { 
            valor_led = matrix_rgb(b = 1.0, r = 1.0, g = 0.0); // Led Magenta                      
        } else {
            valor_led = matrix_rgb(0.0, 0.0, 0.0);  // Led Apagado          
        }
        pio_sm_put_blocking(pio, sm, valor_led);
    }
    imprimir_binario(valor_led);
}

// Verifica se uma tecla foi pressionada no teclado matricial
char scan_keypad() {
    for (int row = 0; row < ROWS; row++) {
        gpio_put(row_pins[row], 0); // Ativa a linha atual
        for (int col = 0; col < COLS; col++) {
            if (gpio_get(col_pins[col]) == 0) { // Verifica se a coluna está ativa
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
    printf("1 - Carinha Feliz Piscando\n");
    printf("A - Desenho a definir\n");
    printf("B - Desenho a definir\n");
    printf("C - Desenho a definir\n"); // ADICIONE O NOME DA SUA IMAGEM
    printf("D - Desenho a definir\n");
    printf("* - Desenho a definir\n");
    printf("# - Desenho a definir\n");
}

//função principal
int main() {
    PIO pio = pio0; 
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
        if (key != '\0') {
            printf("\n>> Você pressionou a tecla: %c <<\n", key);
            
             

            if (key == '1') {
                desenho_pio(carinha_feliz_piscando, valor_led, pio, sm, r, g, b);
                sleep_ms(300);
                Tocar_piscando(); // Toca o barulhinho piscando
                desenho_pio(carinha_feliz_piscando_1, valor_led, pio, sm, r, g, b);
                sleep_ms(300);                
                desenho_pio(carinha_feliz_piscando_2, valor_led, pio, sm, r, g, b);
                sleep_ms(300);
                Tocar_piscando(); // Toca o barulhinho piscando
                desenho_pio(carinha_feliz_piscando_3, valor_led, pio, sm, r, g, b);
                sleep_ms(300);                
                desenho_pio(carinha_feliz_piscando_4, valor_led, pio, sm, r, g, b);
                sleep_ms(300);
                Tocar_piscando(); // Toca o barulhinho piscando
                desenho_pio(carinha_feliz_piscando_5, valor_led, pio, sm, r, g, b);
                sleep_ms(300);                
                desenho_pio(carinha_feliz_piscando_6, valor_led, pio, sm, r, g, b);                 
            } else if (key == '2') {            
                Tocar_melodia(); // Toca a melodia             
            } // ADICIONE SUAS TECLAS
        }

        sleep_ms(100);
        printf("\nFrequência de clock %ld\r\n", clock_get_hz(clk_sys));
    }
}