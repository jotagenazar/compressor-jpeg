#ifndef FILE_HANDLER_H_ 
#define FILE_HANDLER_H_

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

    // Função de abertura de arquivos de acordo com o modo especificado
    // retorna o ponteiro para o arquivo aberto
    FILE *abrir_arquivo(char *nome_arquivo, char *modo);

    // Função que escreve o cabeçalho de um arquivo bmp a ser lido nos campos da struct correspondente
    // retorna a struct bmpfileheader com as informações preenchidas
    BmpFileHeader ler_bmp_file_header(FILE *bmp_file);

    // Função que escreve o cabeçalho da imagem presente no arquivo nos campos da struct correspondene
    // retorna a struct bmpinfoheader com as informações preenchidas
    BmpInfoHeader ler_bmp_info_header(FILE *bmp_file);

    // Função que exporta a matriz de cores e as informações da imagem em um arquivo bmp
    void exportar_bmp(  char *nome_arquivo, BmpFileHeader header, BmpInfoHeader info, 
                        unsigned char **R, unsigned char **G, unsigned char **B);

#endif
