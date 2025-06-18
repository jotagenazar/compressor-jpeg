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

// Matrizes de quantização sugeridas para a matriz Y e para as matrizes Cb e Cr
extern const int Q_Y[8][8];
extern const int Q_C[8][8];

extern const int zigzag[64][2];

extern const char* huffman_dc_lum_codes[12];

extern const char* huffman_ac_lum_codes[16][11];

/************************************
* TYPEDEFS AND STRUCTS
*************************************/

typedef struct 
{
    int zeros; //Número de zeros seguidos
    int coeficiente; //Valor não-zero

} Par_RLE;

typedef  struct{

    int DC_dif; // Diferenca para o ultima valor de alta frequencia
    Par_RLE vetor_par_rle[64]; //vetor que guardas as trincas RLE. Cada trinca vai ser lida para saber como escrever o binario
    int qtd_pares;
    
} BLOCO_CODIFICADO;

typedef struct {
    unsigned char byte_buffer; // O byte que está sendo montado
    int bit_count;             // Quantos bits já estão no buffer
    FILE* output_file;         // O arquivo de saída
} BitWriter;



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

// ! até aqui tudo certo. Novas funçoes a abaixo

int codificar_ac_rle(int zig_zag_vetor[64], Par_RLE* pares_saida); // Pega os 63 valores de AC e cria os pares RLE

BLOCO_CODIFICADO codificar_bloco_entropia(double bloco_quantizado[8][8], int dc_anterior); //

int get_category(int coeficiente); // Usa a tabela 2 para me retornar a categoria do coeficiente

void get_mantissa(int coeficiente, int categoria, char* mantissa_str); 

void executar_compressao_entropica(YCbCrImg img_quantizada, const char* nome_arquivo_saida);

// ! -----------------------------------------
   

#endif
