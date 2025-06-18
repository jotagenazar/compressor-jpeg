#ifndef ENTROPIA_HANDLER_H
#define ENTROPIA_HANDLER_H

/*

Biblioteca contendo as funções responsáveis por lidar com a codificação de imagens, contendo as funções de quantização para
compressão JPEG de imagens YCbCr no domínio das frequências e funções de codificação entrópica.

*/


/************************************
* INCLUDES
*************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#include "YCbCr_handler.h"


/************************************
* MACROS AND DEFINES
*************************************/

extern const int zigzag[64][2];

extern const HuffmanCode HUFFMAN_DC[12];


/************************************
* TYPEDEFS AND STRUCTS
*************************************/

typedef struct 
{
    int zeros; //Número de zeros seguidos
    int coeficiente; //Valor não-zero

} Par_RLE;


typedef struct {
    unsigned short bits; // código Huffman (até 16 bits)
    int tamanho;         // quantos bits esse código tem
} HuffmanCode;

extern const HuffmanCode HUFFMAN_DC[12];


/************************************
* GLOBAL FUNCTION PROTOTYPES
*************************************/

void quantizar_bloco(double bloco[8][8], const int Q[8][8], double k);

double** quantizar_matriz(double** matriz, int altura, int largura, const int Q[8][8], double k);

YCbCrImg quantizar_imagem(YCbCrImg img_dct, double k);

void desquantizar_bloco(double bloco[8][8], const int Q[8][8], double k);

double** desquantizar_matriz(double** matriz, int altura, int largura, const int Q[8][8], double k);

YCbCrImg desquantizar_imagem(YCbCrImg img_dct, double k);

int* aplicar_zigzag(double bloco[8][8]);

int calcular_diferenca_dc(int atual, int anteior);

int aplicar_rle_ac(int vetor[64], Par_RLE* pares);

int codificar_bloco(int bloco[8][8], int dc_anterior, int* diferenca_dc, Par_RLE* ac_pares);

    

#endif
