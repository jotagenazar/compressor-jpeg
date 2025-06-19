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

// Matriz usada para acelerar o preenchimento do vetor na ordem zigzag
extern const int zigzag[64][2];

// Tabela 3 - Prefixos para Coeficientes DC 
extern const char* huffman_dc_lum_codes[12];

// Tabela 4 - Prefixos para o Coeficiente AC
extern const char* huffman_ac_lum_codes[16][11];

/************************************
* TYPEDEFS AND STRUCTS
*************************************/

// Struct usada para armazenar o par RLE
typedef struct 
{
    int zeros; //Número de zeros seguidos
    int coeficiente; //Valor não-zero
} Par_RLE;

// Struct para guardar as caracteristicas de um bloco apos passar pela
// codificacao entropica
typedef  struct{
    int DC_dif; 
    Par_RLE vetor_par_rle[64]; 
    int qtd_pares;
} BLOCO_CODIFICADO;

// Struct que auxilia a armazenar os bits para escrever os bytes no arquivo final
typedef struct {
    unsigned char byte_buffer; 
    int bit_count;             
    FILE* output_file;         
} BitWriter;


/************************************
* GLOBAL FUNCTION PROTOTYPES
*************************************/

// Função que aplica a quantizacao na imagem passada como parâmetro, realizando a operação de multiplicação de matrizes bloco a bloco
// e retornando a nova imagem YCbCr com os valores quantizados 
YCbCrImg quantizar_imagem(YCbCrImg img_dct, double k);

// Função que aplica a inversa da quantizacao na imagem passada como parâmetro e retornando uma imagem YCbCr com os blocos recuperados.
YCbCrImg desquantizar_imagem(YCbCrImg img_dct, double k);

// Funcao que aplica a codificao entropica na imagem passado como parametro, e escreve em um arquivo de saída  
void executar_codificacao_entropica(YCbCrImg img_quantizada, const char* nome_arquivo_saida);

#endif
