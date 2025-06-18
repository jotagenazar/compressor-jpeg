/************************************
* INCLUDES
************************************/

#include "../include/file_handler.h"


/************************************
* EXTERN VARIABLES
************************************/

/************************************
* PRIVATE MACROS AND DEFINES
************************************/

/************************************
* PRIVATE TYPEDEFS
************************************/

/************************************
* STATIC VARIABLES
************************************/

/************************************
* GLOBAL VARIABLES
************************************/

/************************************
* STATIC FUNCTION PROTOTYPES
************************************/

/************************************
* STATIC FUNCTIONS
************************************/

/************************************
* GLOBAL FUNCTIONS
************************************/

FILE* abrir_arquivo(char *filename, char *modo)
{
    FILE *file;

    // teste de abertura do arquivo
    file = fopen(filename, modo);
    if (file == NULL){
        printf("Erro ao abrir arquivo %s\n", filename);
        exit(1);
    }

    return file;
}


BmpFileHeader ler_bmp_file_header(FILE *bmp_file)
{
    BmpFileHeader bmp_file_header;

    // leitura das informações para os campos da struct criada
    fread(&(bmp_file_header.bfType),      sizeof (unsigned short int),    1, bmp_file);
    fread(&(bmp_file_header.bfSize),      sizeof (unsigned int),          1, bmp_file);
    fread(&(bmp_file_header.bfReserved1), sizeof (unsigned short int),    1, bmp_file);
    fread(&(bmp_file_header.bfReserved2), sizeof (unsigned short int),    1, bmp_file);
    fread(&(bmp_file_header.bfOffBits),   sizeof (unsigned int),          1, bmp_file);

    return bmp_file_header;
}


BmpInfoHeader ler_bmp_info_header(FILE *bmp_file) 
{
    BmpInfoHeader bmp_info_header;

    // leitura das informações para os campos da struct criada
    fread(&(bmp_info_header.biSize),             sizeof (unsigned int),      1,  bmp_file);
    fread(&(bmp_info_header.biWidth),            sizeof (int),               1,  bmp_file);
    fread(&(bmp_info_header.biHeight),           sizeof (int),               1,  bmp_file);
    fread(&(bmp_info_header.biPlanes),           sizeof (unsigned short),    1,  bmp_file);
    fread(&(bmp_info_header.biBitCount),         sizeof (unsigned short),    1,  bmp_file);
    fread(&(bmp_info_header.biCompression),      sizeof (unsigned int),      1,  bmp_file);
    fread(&(bmp_info_header.biSizeImage),        sizeof (unsigned int),      1,  bmp_file);
    fread(&(bmp_info_header.biXPelsPerMeter),    sizeof (int),               1,  bmp_file);
    fread(&(bmp_info_header.biYPelsPerMeter),    sizeof (int),               1,  bmp_file);
    fread(&(bmp_info_header.biClrUsed),          sizeof (unsigned int),      1,  bmp_file);
    fread(&(bmp_info_header.biClrImportant),     sizeof (unsigned int),      1,  bmp_file);

    return bmp_info_header;
}


void ler_bmp_rgb(FILE *bmp_file, RGBImg rgb_img)
{   
    // Cálculo padrão do padding em arquivos bmp, a dimensão especificada para o trabalho garante que o padding será sempre 0
    int padding = (4 - (rgb_img.width * 3) % 4) % 4;

    // leitura da matriz
    for (int i = rgb_img.height - 1; i >= 0; i--) 
    {
        for (int j = 0; j < rgb_img.width; j++) 
        {
            // caracteres auxiliares de leitura
            unsigned char r, g, b;
            
            // a ordem das matrizes se dispõe de trás para frente em arquivos bmp
            fread(&b, sizeof(unsigned char), 1, bmp_file);
            fread(&g, sizeof(unsigned char), 1, bmp_file);
            fread(&r, sizeof(unsigned char), 1, bmp_file);
            
            rgb_img.R[i][j] = r;
            rgb_img.G[i][j] = g;
            rgb_img.B[i][j] = b;
        }
        
        // descarte de bytes de padding no final da linha, no caso do trabalho não há
        fseek(bmp_file, padding, SEEK_CUR);   
    }

}


void exportar_bmp(  char *nome_arquivo, BmpFileHeader header, BmpInfoHeader info, 
                    RGBImg rgb_img) 
{
    FILE *output_file = abrir_arquivo(nome_arquivo, "wb");

    // Cálculo padrão do padding em arquivos bmp, a dimensão especificada para o trabalho garante que o padding será sempre 0
    int padding = (4 - (info.biWidth * 3) % 4) % 4;
    unsigned char pad[3] = {0, 0, 0};   // o padding pode possuir até 3 bytes

    // Escrita dos headers do arquivo e de info
    fwrite(&header.bfType,      sizeof(unsigned short), 1,  output_file);
    fwrite(&header.bfSize,      sizeof(unsigned int),   1,  output_file);
    fwrite(&header.bfReserved1, sizeof(unsigned short), 1,  output_file);
    fwrite(&header.bfReserved2, sizeof(unsigned short), 1,  output_file);
    fwrite(&header.bfOffBits,   sizeof(unsigned int),   1,  output_file);

    fwrite(&info.biSize,            sizeof(unsigned int),   1,  output_file);
    fwrite(&info.biWidth,           sizeof(int),            1,  output_file);
    fwrite(&info.biHeight,          sizeof(int),            1,  output_file);
    fwrite(&info.biPlanes,          sizeof(unsigned short), 1,  output_file);
    fwrite(&info.biBitCount,        sizeof(unsigned short), 1,  output_file);
    fwrite(&info.biCompression,     sizeof(unsigned int),   1,  output_file);
    fwrite(&info.biSizeImage,       sizeof(unsigned int),   1,  output_file);
    fwrite(&info.biXPelsPerMeter,   sizeof(int),            1,  output_file);
    fwrite(&info.biYPelsPerMeter,   sizeof(int),            1,  output_file);
    fwrite(&info.biClrUsed,         sizeof(unsigned int),   1,  output_file);
    fwrite(&info.biClrImportant,    sizeof(unsigned int),   1,  output_file);


    // Escrita dos pixels de baixo para cima seguindo o padrão de disposição do bmp
    for (int i = info.biHeight - 1; i >= 0; i--) 
    {
        for (int j = 0; j < info.biWidth; j++) 
        {
            // array auxiliar de escrita
            unsigned char pixel[3] = {  rgb_img.B[i][j], 
                                        rgb_img.G[i][j], 
                                        rgb_img.R[i][j] };

            fwrite(pixel, sizeof(unsigned char), 3, output_file);
        }

        fwrite(pad, sizeof(unsigned char), padding, output_file); // escrita do padding ao final da linha, no caso do trabalho não há
    }

    fclose(output_file);
}