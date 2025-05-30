#ifndef YCBCR_HANDLER_H 
#define YCBCR_HANDLER_H

/************************************
* INCLUDES
*************************************/

    #include <stdio.h>
    #include <stdlib.h>


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
        unsigned char **Y;
        unsigned char **Cb;
        unsigned char **Cr;
    } YCbCrImg;

/************************************
* GLOBAL FUNCTION PROTOTYPES
*************************************/

    // Função que cria a struct para a matriz RGB com as dimensões passadas, aloca dinamicamente
    // a matriz e retorna a struct
    RGBImg alocar_RGB(int width, int height);

    // Função que desaloca corretamente a matriz RGB contida na estrutura
    void liberar_RGB(RGBImg rgb_img);


#endif
