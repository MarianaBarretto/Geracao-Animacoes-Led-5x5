#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"
#include "pio_matrix.pio.h" //arquivo .pio

//configurando a matriz de LEDs
#define NUM_PIXELS 25 //número de LEDs
#define OUT_PIN 19 //pino de onde os LEDs estão conectados

//configurando o teclado matricial
#define ROWS 4 // 4 linhas
#define COLS 4 // 4 colunas

uint row_pins[ROWS] = {8, 7, 6, 5}; // GPIOs conectadas as linhas (R1 a R4)
uint col_pins[COLS] = {4, 3, 2, 1}; // GPIOs conectadas as colunas (C1 a C4)

//mapeando as teclas 
const char key_map[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

//preparando as linhas e colunas do teclado
void init_gpio(){
    for (int i = 0; i < ROWS; i++) {
        gpio_init(row_pins[i]);              // configura a linha
        gpio_set_dir(row_pins[i], GPIO_OUT); // preparando as linhas
        gpio_put(row_pins[i], 1);            // começamos com as linhas "desligadas"
    }

    for (int i = 0; i < COLS; i++) {
        gpio_init(col_pins[i]);              // configura a coluna
        gpio_set_dir(col_pins[i], GPIO_OUT); // colunas receberão sinal das linhas
        gpio_put(col_pins[i], 1);            // habilita pull-up nas colunas
    }

}

// função para ler teclas

char leitura_teclado(){
    for (int row = 0; row < ROWS; row++){

    }
}




