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

// função para definir cor RGB para LEDs da Matriz
uint32_t matriz_RGB(double r, double g, double b){
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

//função para imprimir binários
void imprimir_binario(int num){
    int i;
    for(i = 31; i >= 0; i--){
        (num & (1 << i)) ? printf("1") : printf("0");
    }
}                       

void desenho_pio(double *desenho, uint32_t valor_led, PIO pio, uint sm) {
    for (int16_t i = 0; i < NUM_PIXELS; i++) {
        //aplicando cores com base no valor do desenho[i]
        if (desenho[i] == 1.0) {
            valor_led = matriz_RGB(0.0, 1.0, 0.0); //vermelho
        } else if (desenho[i] == 0.5) {
            valor_led = matriz_RGB(1.0, 0.0, 0.0); //azul
        } else if (desenho[i] == 0.3) {
            valor_led = matriz_RGB(0.0, 1.0, 1.0); //amarelo
        } else if (desenho[i] == 0.8) {
            valor_led = matriz_RGB(1.0, 1.0, 0.0); //ciano
        } else if (desenho[i] == 0.7) {
            valor_led = matriz_RGB(0.0, 0.0, 1.0); //verde
        } else {
            //caso contrário, usa escala de cinza (por exemplo, se o valor for entre 0 e 1)
            valor_led = matriz_RGB(desenho[i], desenho[i], desenho[i]);
        }

        //envia o valor RGB para o LED
        pio_sm_put_blocking(pio, sm, valor_led);
        //imprime a cor em binário
        imprimir_binario(valor_led);
    }
}

// função para desligar todos os LEDs
void desligar_LEDs(double *desenho){
    for (int i = 0; i < NUM_PIXELS; i++){
        desenho[i] = 0.0; //irá definir todos os LEDs como desligados
    }
}

//ligar LEDs brancas com 20% de intensidade
void ligar_LEDs_brancos(double *desenho){
    for (int i = 0; i < NUM_PIXELS; i++){
        desenho[i] = 0.2; //cor branca com 20% de intensidade
    }
}

//configurando o teclado matricial
#define ROWS 4 //4 linhas
#define COLS 4 //4 colunas

uint row_pins[ROWS] = {8, 7, 6, 5}; //GPIOs conectadas as linhas (R1 a R4)
uint col_pins[COLS] = {4, 3, 2, 1}; //GPIOs conectadas as colunas (C1 a C4)

//mapeando as teclas 
const char mapear_teclas[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

//preparando as linhas e colunas do teclado
void configurar_teclado(){
    for (int i = 0; i < ROWS; i++) {
        gpio_init(row_pins[i]);              //configura a linha
        gpio_set_dir(row_pins[i], GPIO_OUT); //preparando as linhas
        gpio_put(row_pins[i], 1);            //começamos com as linhas "desligadas"
    }

    for (int i = 0; i < COLS; i++) {
        gpio_init(col_pins[i]);              //configura a coluna
        gpio_set_dir(col_pins[i], GPIO_OUT); //colunas receberão sinal das linhas
        gpio_put(col_pins[i], 1);            //habilita pull-up nas colunas
    }

}

// função para ler teclas
char leitura_teclas(){
    for (int row = 0; row < ROWS; row++){
        gpio_put(row_pins[row], 0); //ativa a linha atual
        for (int col = 0; col < COLS; col++){ //verifica cada coluna
            if (!gpio_get(col_pins[col])){ //detecta tecla pressionada
                char tecla = mapear_teclas[row * COLS + col]; // armazena o valor da tecla
                sleep_ms(500); //aguarda debounce
                return mapear_teclas[row][col]; //retorna tecla pressionada
            }
        }

        gpio_put(row_pins[row], 1); //desativa a linha atual
    } 
    return '\0'; //retorna isso quando nenhuma tecla pressionada
}

//vetores com frames do meu desenho
double desenho[25] =   {0.0, 1.0, 0.0, 1.0, 0.0,
                        1.0, 0.0, 1.0, 0.0, 1.0, 
                        1.0, 0.0, 0.0, 0.0, 1.0,
                        0.0, 1.0, 0.0, 1.0, 0.0,
                        0.0, 0.0, 1.0, 0.0, 0.0};


double desenho2[25] =  {0.0, 1.0, 0.0, 1.0, 0.0,
                        1.0, 0.5, 1.0, 0.5, 1.0, 
                        1.0, 0.5, 0.5, 0.5, 1.0,
                        0.0, 1.0, 0.5, 1.0, 0.0,
                        0.0, 0.0, 1.0, 0.0, 0.0};

double desenho3[25] =  {0.3, 1.0, 0.3, 1.0, 0.3,
                        0.3, 1.0, 1.0, 1.0, 0.3, 
                        0.3, 1.0, 1.0, 1.0, 0.3,
                        0.3, 0.0, 0.0, 0.0, 0.3,
                        0.3, 0.3, 1.0, 0.3, 0.3};

double desenho4[25] =  {1.0, 1.0, 0.0, 1.0, 1.0,
                        1.0, 0.0, 1.0, 0.0, 1.0, 
                        0.0, 1.0, 0.0, 1.0, 0.0,
                        0.0, 0.0, 1.0, 0.0, 0.0,
                        0.0, 0.0, 0.0, 0.8, 0.8,};

double desenho5[25] =  {0.5, 0.7, 0.5, 0.7, 0.5,
                        0.7, 0.0, 0.7, 0.0, 0.7, 
                        0.7, 0.0, 0.5, 0.0, 0.7,
                        0.0, 0.7, 0.5, 0.7, 0.0,
                        0.5, 0.5, 0.7, 0.5, 0.5,};

int main(){

}







