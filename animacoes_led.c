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

uint32_t valor_led;
PIO pio;
uint sm;
double r;
double g;
double b;

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
double carinha_feliz_piscando[25] =   {0, 0, 0, 0, 0,
                                       0, 3, 0, 3, 0, 
                                       0, 0, 0, 0, 0,
                                       3, 0, 0, 0, 3,
                                       0, 3, 3, 3, 0};
double carinha_feliz_piscando_1[25] = {0, 0, 0, 0, 0,
                                       0, 0, 0, 3, 0, 
                                       0, 0, 0, 0, 0,
                                       3, 0, 0, 0, 3,
                                       0, 3, 3, 3, 0};
double carinha_feliz_piscando_2[25] = {0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0};                                                                                                                                  
                   
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
void desenho_pio(double *desenho, double r, double g, double b){
    
    uint32_t cores[7] = {
        matrix_rgb(0.0, 0.0, 0.0), // Índice 0: LED apagado
        matrix_rgb(1.0, 0.0, 0.0), // Índice 1: Azul
        matrix_rgb(0.0, 1.0, 0.0), // Índice 2: Vermelho
        matrix_rgb(0.0, 0.0, 1.0), // Índice 3: Verde
        matrix_rgb(0.0, 1.0, 1.0), // Índice 4: Amarelo
        matrix_rgb(1.0, 0.0, 1.0), // Índice 5: Ciano
        matrix_rgb(1.0, 1.0, 0.0)  // Índice 6: Magenta
    };

    for (int16_t i = 0; i < NUM_PIXELS; i++) {
        // Defina a cor com base no valor de desenho[24 - i]
        uint32_t valor_led = cores [(int)desenho[24 - i]];
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

void animação_mariana() {
    desenho_pio(carinha_feliz_piscando, 1.0, 1.0, 1.0);
    sleep_ms(400);
    Tocar_piscando(); // Toca o barulhinho piscando
    desenho_pio(carinha_feliz_piscando_1, 1.0, 1.0, 1.0);
    sleep_ms(400);                
    desenho_pio(carinha_feliz_piscando, 1.0, 1.0, 1.0);
    sleep_ms(400);
    Tocar_piscando(); // Toca o barulhinho piscando
    desenho_pio(carinha_feliz_piscando_1, 1.0, 1.0, 1.0);
    sleep_ms(400);                
    desenho_pio(carinha_feliz_piscando, 1.0, 1.0, 1.0);
    sleep_ms(400);
    Tocar_piscando(); // Toca o barulhinho piscando
    desenho_pio(carinha_feliz_piscando_1, 1.0, 1.0, 1.0);
    sleep_ms(400);                
    desenho_pio(carinha_feliz_piscando, 1.0, 1.0, 1.0);
    sleep_ms(400);
    Tocar_piscando(); // Toca o barulhinho piscando                
    desenho_pio(carinha_feliz_piscando_2, 1.0, 1.0, 1.0);
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
        
        if (key != 0) {  // Verifica se alguma tecla foi pressionada
            switch (key) {

            case 'A':  // Desliga todos os LEDs
                // Adiconar rotina aqui.
                printf("LEDs desligados.\n");   
            break;

            case 'B':  // Liga todos os LEDs como azul com intensidade 100%
                // Adiconar rotina aqui.
                printf("LEDs azuis ligados com intensidade de 100%%.\n");   
            break;

            case 'C':  // Liga todos os LEDs como vermelho com intensidade 80%
                // Adiconar rotina aqui.
                printf("LEDs vermelhos ligados com intensidade de 80%%.\n");   
            break;

            case 'D':  // Liga todos os LEDs como verde com intensidade 50%
                // Adiconar rotina aqui.
                printf("LEDs verdes ligados com intensidade de 50%%.\n");   
            break;

            case '#':  // Liga todos os LEDs como branco com intensidade 20%
                // Adiconar rotina aqui.
                printf("LEDs brancos ligados com intensidade de 20%%.\n");   
            break;
            
            case '*':  // Raspberry sai do modo de execução e habilita o modo de gravação (reboot)
                // Adiconar rotina aqui.
                printf("Regravação do Raspberry Pi Pico W inciada.\n");   
            break;
            
            case '1':  // Animação da Mariana
                animação_mariana(); // Adiconada a rotina aqui
                printf("Animação do botão 1 foi acionada.\n");
            break;

            case '2':  // Animação da Helen
                // Adiconar rotina aqui.
                printf("Animação do botão 2 foi acionada.\n");
            break;

            case '3':  // Animação do Kauan
                // Adiconar rotina aqui.
                printf("Animação do botão 3 foi acionada.\n");
            break;

            case '4':  // Animação do Lucas
                // Adiconar rotina aqui.
                printf("Animação do botão 4 foi acionada.\n");
            break;

            case '5':  // Animação da Edna
                // Adiconar rotina aqui.
                printf("Animação do botão 5 foi acionada.\n");
            break;

            case '6':  // Animação do Daniel
                // Adiconar rotina aqui.
                printf("Animação do botão 6 foi acionada.\n");
            break;

            case '7':  // Animação do Alexandro
                // Adiconar rotina aqui.
                printf("Animação do botão 7 foi acionada.\n");
            break;

            case '8':  // Animação do Israel
                // Adiconar rotina aqui.
                printf("Animação do botão 8 foi acionada.\n");
            break;

            case '9':  // Animação do Ylo
                // Adiconar rotina aqui.
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