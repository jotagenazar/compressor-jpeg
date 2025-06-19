#ifndef FILE_HANDLER_H_ 
#define FILE_HANDLER_H_

/*

Biblioteca contendo as estruturas responsáveis por lidar com a manipulação de arquivos .bmp, como funções de leitura e manipulação
de arquivos, definição de estruturas de header e funçòes de exportação de imagens RGB para um arquivo .bmp

*/


/************************************
* INCLUDES
*************************************/

#include <stdio.h>
#include <stdlib.h>
#include "YCbCr_handler.h"


/************************************
* TYPEDEFS AND STRUCTS
*************************************/

// BMP file header structure
typedef struct 
{
    unsigned short bfType;          // Magic number for file, bfType deve ser = "MB"
    unsigned int   bfSize;          // Size of file 
    unsigned short bfReserved1;     // Reserved
    unsigned short bfReserved2;     // Reserved
    unsigned int   bfOffBits;       // Offset to bitmap data
} BmpFileHeader;


// BMP file info structure 
typedef struct 
{
    unsigned int   biSize;           // Size of info header
    int            biWidth;          // Width of image
    int            biHeight;         // Height of image
    unsigned short biPlanes;         // Number of color planes
    unsigned short biBitCount;       // Number of bits per pixel 
    unsigned int   biCompression;    // Type of compression to use 
    unsigned int   biSizeImage;      // Size of image data 
    int            biXPelsPerMeter;  // X pixels per meter 
    int            biYPelsPerMeter;  // Y pixels per meter 
    unsigned int   biClrUsed;        // Number of colors used 
    unsigned int   biClrImportant;   // Number of important colors 
} BmpInfoHeader;


/************************************
* GLOBAL FUNCTION PROTOTYPES
*************************************/

// Função de abertura de arquivos de acordo com o modo especificado.
// Retorna o ponteiro para o arquivo aberto
FILE* abrir_arquivo(char *nome_arquivo, char *modo);


// Função que escreve o cabeçalho de um arquivo bmp a ser lido nos campos da struct correspondente.
// Retorna a struct bmpfileheader com as informações preenchidas.
// Supõe que o ponteiro já aponta a localização do file_header no arquivo (SEEK_SET)
BmpFileHeader ler_bmp_file_header(FILE *bmp_file);


// Função que escreve o cabeçalho da imagem presente no arquivo nos campos da struct correspondente.
// Retorna a struct bmpinfoheader com as informações preenchidas.
// Supõe que o ponteiro já aponta a localização do file_header no arquivo (SEEK_SET + sizeof(BmpFileHeader))
BmpInfoHeader ler_bmp_info_header(FILE *bmp_file);


// Função que escreve a matriz RGB, contida no arquivo e nas estruturas BmpFileHeader e BmpInfoHeader, na estrutura
// RGBImg, todos passados como parâmetro.
// Supõe que o ponteiro já aponta a localização do file_header no arquivo (SEEK_SET + BmpFileHeader.btOffBits)
void ler_bmp_rgb(FILE *bmp_file, RGBImg rgb_img);

// Função que escreve os headers bmp no arquivo aberto passado como parâmetro
void escrever_headers_bmp(FILE *output_file, BmpFileHeader header, BmpInfoHeader info);

// Função que exporta a matriz de cores e as informações da imagem em um arquivo bmp
void exportar_bmp(  char *nome_arquivo, BmpFileHeader header, BmpInfoHeader info, 
                    RGBImg rgb_img);

#endif
