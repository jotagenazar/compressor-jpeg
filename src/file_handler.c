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

    FILE *abrir_arquivo(char *filename, char *modo)
    {
        FILE *file;

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


    void ler_bmp_rgb(FILE *bmp_file, RGBImg rgb_img)
    {
        int padding = (4 - (rgb_img.width * 3) % 4) % 4;

        for (int i = rgb_img.height - 1; i >= 0; i--) 
        {
            for (int j = 0; j < rgb_img.width; j++) 
            {
                unsigned char b, g, r;

                fread(&b, sizeof(unsigned char), 1, bmp_file);
                fread(&g, sizeof(unsigned char), 1, bmp_file);
                fread(&r, sizeof(unsigned char), 1, bmp_file);

                rgb_img.B[i][j] = b;
                rgb_img.G[i][j] = g;
                rgb_img.R[i][j] = r;
            }

            fseek(bmp_file, padding, SEEK_CUR); // descartar bytes de padding no final da linha
        }
    }


    void exportar_bmp(  char *nome_arquivo, BmpFileHeader header, BmpInfoHeader info, 
                        RGBImg rgb_img) 
    {
        FILE *output_file = abrir_arquivo(nome_arquivo, "wb");

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

                pixel[0] = rgb_img.B[i][j]; // azul
                pixel[1] = rgb_img.G[i][j]; // verde
                pixel[2] = rgb_img.R[i][j]; // vermelho
                
                fwrite(pixel, sizeof(unsigned char), 3, output_file);
            }

            fwrite(pad, sizeof(unsigned char), padding, output_file); // padding
        }

        fclose(output_file);
    }