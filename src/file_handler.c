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

    FILE *abrir_arquivo(char *nome_arquivo, char *modo)
    {
        FILE *arquivo;

        arquivo = fopen(nome_arquivo, modo);
        if (arquivo == NULL){
            printf("Erro ao abrir arquivo %s\n", nome_arquivo);
            exit(1);
        }

        return arquivo;
    }

    BmpFileHeader ler_bmp_file_header(FILE *bmp_file)
    {
        BmpFileHeader bmp_file_header;

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

    void exportar_bmp(  char *nome_arquivo, BmpFileHeader header, BmpInfoHeader info, 
                        unsigned char **R, unsigned char **G, unsigned char **B) 
    {
        FILE *output_file = fopen(nome_arquivo, "wb");
        if (output_file == NULL) {
            printf("Erro ao criar arquivo de saída.\n");
            exit(1);
        }

        int width = info.biWidth;
        int height = info.biHeight;
        int padding = (4 - (width * 3) % 4) % 4;
        unsigned char pad[3] = {0, 0, 0}; // até 3 bytes de padding

        // Escreve os headers
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

        // Escreve os pixels de baixo para cima (formato BMP)
        for (int i = height - 1; i >= 0; i--) 
        {
            for (int j = 0; j < width; j++) 
            {
                unsigned char pixel[3];
                pixel[0] = B[i][j]; // azul
                pixel[1] = G[i][j]; // verde
                pixel[2] = R[i][j]; // vermelho
                fwrite(pixel, sizeof(unsigned char), 3, output_file);
            }

            fwrite(pad, sizeof(unsigned char), padding, output_file); // padding
        }

        fclose(output_file);
    }