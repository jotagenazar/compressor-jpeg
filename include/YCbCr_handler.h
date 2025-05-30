#ifndef YCBCR_HANDLER_H 
#define YCBCR_HANDLER_H

/************************************
* INCLUDES
*************************************/

    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>


/************************************
* MACROS AND DEFINES
*************************************/


/************************************
* TYPEDEFS AND STRUCTS
*************************************/

    // Struct que contém a matriz dinâmica RGB de uma imagem
    typedef struct 
    {
        int height;          
        int width;            
        unsigned char **R;
        unsigned char **G;
        unsigned char **B;
    } RGBImg;

    // Struct que contém a matriz dinâmica YCbCr de uma imagem
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

    // Função que desaloca corretamente a matriz RGB contida na estrutura
    void liberar_RGB(RGBImg rgb_img);

    YCbCrImg alocar_YCbCr(int width, int height);

    void liberar_YCbCr(YCbCrImg YCbCr_img);

    void RGB2YCbCr(YCbCrImg YCbCr_img, RGBImg rgb_img);

    void YCbCr2RGB(YCbCrImg YCbCr_img, RGBImg rgb_img);


#endif
