#ifndef YCBCR_HANDLER_H 
#define YCBCR_HANDLER_H

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

extern const double C[8][8]; 
extern const double Ct[8][8];

extern const int Q_Y[8][8];
extern const int Q_C[8][8];

extern const int zigzag[64][2];



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

    // Função que cria a struct para a matriz RGB com as dimensões passadas, aloca dinamicamente
    // a matriz e retorna a struct
    RGBImg alocar_RGB(int width, int height);

    // Função que desaloca corretamente a matriz RGB contida na estrutura
    void liberar_RGB(RGBImg rgb_img);

    YCbCrImg alocar_YCbCr(int width, int height);

    YCbCrImg alocar_YCbCr_reduzida(int width, int height);

    void liberar_YCbCr(YCbCrImg YCbCr_img);

    void liberar_YCbCr_reduced(YCbCrImg YCbCr_img);

    void RGB2YCbCr(YCbCrImg YCbCr_img, RGBImg rgb_img);

    void YCbCr2RGB(YCbCrImg YCbCr_img, RGBImg rgb_img);

    YCbCrImg downsamplig(YCbCrImg YCbCr_img);

    void aplicar_DCT_bloco(double B[8][8], double F[8][8]);

    double** aplicar_DCT(double** entrada, int altura, int largura);

    YCbCrImg executar_DCT(YCbCrImg entrada);

    void aplicar_IDCT_bloco(double F[8][8], double B[8][8]);

    double** aplicar_IDCT(double** entrada, int altura, int largura);

    YCbCrImg executar_IDCT(YCbCrImg entrada);

    YCbCrImg upsampling(YCbCrImg reduzido);

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
