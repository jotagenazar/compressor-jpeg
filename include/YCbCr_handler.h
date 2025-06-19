#ifndef YCBCR_HANDLER_H 
#define YCBCR_HANDLER_H

/*

Biblioteca contendo as funções responsáveis por lidar com a manipulação de imagens YCbCr, como funções de alocação,
transformação RGB para YCbCr, downsampling de imagens YCbCr e execução de transformação DCT e IDCT para mudança de domínio
de imagens YCbCr.

Blocos a partir da DCT são 8x8 por padrão.

*/


/************************************
* INCLUDES
*************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>


/************************************
* MACROS AND DEFINES
*************************************/

// valor do divisor pelo qual as dimensões das matrizes Cb e Cr serão divididas no processo de downsampling
#define DOWNSAMPLE_DIVISOR 2

// Matrizes de transformação C e C transposta usadas na DCT
extern const double C[8][8]; 
extern const double Ct[8][8];


/************************************
* TYPEDEFS AND STRUCTS
*************************************/

// Struct que contém a matriz dinâmica RGB de uma imagem e suas informações de dimensão
typedef struct 
{
    int height;          
    int width;            
    unsigned char **R;
    unsigned char **G;
    unsigned char **B;
} RGBImg;


// Struct que contém a matriz dinâmica YCbCr de uma imagem e suas informações de dimensão.
// As dimensões sempre representam os valores da matriz Y para o caso de imagens que sofreram downsampling
typedef struct 
{
    int height;          
    int width;            
    double **Y;
    double **Cb;
    double **Cr;
} YCbCrImg;


/************************************
* GLOBAL FUNCTION PROTOTYPES
*************************************/

// Função que cria a struct para a matriz RGB com as dimensões passadas, aloca dinamicamente
// a matriz e retorna a struct
RGBImg alocar_RGB(int width, int height);

// Função que cria a struct para a matriz YCbCr com as dimensões passadas, aloca dinamicamente
// a matriz e retorna a struct
YCbCrImg alocar_YCbCr(int width, int height);

// Função que cria a struct para a matriz YCbCr reduzida usada no downsampling com as dimensões passadas, 
// aloca dinamicamente a matriz e retorna a struct. O divisor para a diminuição é o definido no macro DOWNSAMPLE_DIVISOR.
// As dimensões passadas sempre representam as dimensões da matriz Y para o caso de imagens que sofreram downsampling
YCbCrImg alocar_YCbCr_downsampled(int width, int height);



// Função que desaloca corretamente a matriz RGB cujos ponteiros estão contidos na struct passada
void liberar_RGB(RGBImg rgb_img);

// Função que desaloca corretamente a matriz YCbCr cujos ponteiros estão contidos na struct passada
void liberar_YCbCr(YCbCrImg YCbCr_img);

// Função que desaloca corretamente a matriz YCbCr reduzida cujos ponteiros estão contidos na struct passada.
// O divisor para a diminuição é o definido no macro DOWNSAMPLE_DIVISOR
void liberar_YCbCr_downsampled(YCbCrImg YCbCr_downsampled);



// Procedimento responsável por converter a matriz RGB para o formato YCbCr e preencher as matrizes já alocadas
// na struct YCbCr passada como parametro com os valores convertidos
void RGB_to_YCbCr(RGBImg rgb_img, YCbCrImg YCbCr_img);

// Procedimento responsável por converter a matriz YCbCr para o formato RGB e preencher as matrizes já alocadas
// na struct RGB passada como parametro com os valores convertidos
void YCbCr_to_RGB(YCbCrImg YCbCr_img, RGBImg rgb_img);



// Função que realiza o downsample de uma imagem YCbCr passada através da struct como parâmetro, retornando a struct
// YCbCr com dimensões de Cb e Cr reduzidas dividindo as dimensões por DOWNSAMPLE_DIVISOR
YCbCrImg downsample_YCbCr(YCbCrImg YCbCr_img);

// Função que realiza o upsample de uma imagem YCbCr reduzida passada através da struct como parâmetro, retornando a struct
// YCbCr com dimensões e valores de Cb e Cr restaurados de acordo com DOWNSAMPLE_DIVISOR
YCbCrImg upsample_YCbCr(YCbCrImg YCbCr_downsampled);


// Função que aplica a DCT na imagem passada como parâmetro, realizando a operação de multiplicação de matrizes bloco a bloco
// e retornando a nova imagem YCbCr_downsampled no dominio das frequencias 
YCbCrImg aplicar_DCT_YCbCr_downsampled(YCbCrImg YCbCr_img);

// Função que aplica a inversa da DCT na imagem no dominio de frequências passada como parâmetro, realizando a operação de 
// multiplicação de matrizes entre cada bloco de coeficientes e retornando uma imagem YCbCr_downsampled com os blocos recuperados.
YCbCrImg aplicar_IDCT_YCbCr_downsampled(YCbCrImg YCbCr_DCT);



#endif
