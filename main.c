#include <stdio.h>
#include <stdlib.h>

typedef struct                                /**** BMP file header structure ****/
    {
        unsigned short bfType;           /* Magic number for file */
        unsigned int   bfSize;              /* Size of file */
        unsigned short bfReserved1;    /* Reserved */
        unsigned short bfReserved2;    /* ... */
        unsigned int   bfOffBits;           /* Offset to bitmap data */
    } BMPFILEHEADER;

/* bfType deve ser = "MB" */

typedef struct                           /**** BMP file info structure ****/
    {
       unsigned int   biSize;             /* Size of info header */
       int            biWidth;               /* Width of image */
       int            biHeight;              /* Height of image */
       unsigned short biPlanes;        /* Number of color planes */
       unsigned short biBitCount;     /* Number of bits per pixel */
       unsigned int   biCompression; /* Type of compression to use */
       unsigned int   biSizeImage;     /* Size of image data */
       int            biXPelsPerMeter;    /* X pixels per meter */
       int            biYPelsPerMeter;    /* Y pixels per meter */
       unsigned int   biClrUsed;         /* Number of colors used */
       unsigned int   biClrImportant;  /* Number of important colors */
} BMPINFOHEADER;
 
// Função para o cabeçalho do arquivo
void leituraHeader(FILE *F, BMPFILEHEADER *H){
/*F é o arquivo Bitmap que deve ter sido “lido” do disco*/
     fread(&H->bfType,sizeof (unsigned short int),1,F);
     fread(&H->bfSize,sizeof (unsigned int),1,F);
     fread(&H->bfReserved1,sizeof (unsigned short int),1,F);
     fread(&H->bfReserved2,sizeof (unsigned short int),1,F);
     fread(&H->bfOffBits,sizeof (unsigned int),1,F);
}

// Função para ler o cabeçalho da imagem
void leitura_Header_Img(FILE *F, BMPINFOHEADER *H){
    fread(&H->biSize,sizeof (unsigned int),1,F);
    fread(&H->biWidth,sizeof (int),1,F);
    fread(&H->biHeight,sizeof (int),1,F);
    fread(&H->biPlanes,sizeof (unsigned short),1,F);
    fread(&H->biBitCount,sizeof (unsigned short),1,F);
    fread(&H->biCompression,sizeof (unsigned int),1,F);
    fread(&H->biSizeImage,sizeof (unsigned int),1,F);
    fread(&H->biXPelsPerMeter,sizeof (int),1,F);
    fread(&H->biYPelsPerMeter,sizeof (int),1,F);
    fread(&H->biClrUsed,sizeof (unsigned int),1,F);
    fread(&H->biClrImportant,sizeof (unsigned int),1,F);
}

void salvarImagemBMP(char *nome_arquivo, BMPFILEHEADER header, BMPINFOHEADER info, 
                     unsigned char **R, unsigned char **G, unsigned char **B) {
    FILE *saida = fopen(nome_arquivo, "wb");
    if (saida == NULL) {
        printf("Erro ao criar arquivo de saída.\n");
        exit(1);
    }

    int width = info.biWidth;
    int height = info.biHeight;
    int padding = (4 - (width * 3) % 4) % 4;
    unsigned char pad[3] = {0, 0, 0}; // até 3 bytes de padding

    // Escreve os headers
    fwrite(&header.bfType, sizeof(unsigned short), 1, saida);
    fwrite(&header.bfSize, sizeof(unsigned int), 1, saida);
    fwrite(&header.bfReserved1, sizeof(unsigned short), 1, saida);
    fwrite(&header.bfReserved2, sizeof(unsigned short), 1, saida);
    fwrite(&header.bfOffBits, sizeof(unsigned int), 1, saida);

    fwrite(&info.biSize, sizeof(unsigned int), 1, saida);
    fwrite(&info.biWidth, sizeof(int), 1, saida);
    fwrite(&info.biHeight, sizeof(int), 1, saida);
    fwrite(&info.biPlanes, sizeof(unsigned short), 1, saida);
    fwrite(&info.biBitCount, sizeof(unsigned short), 1, saida);
    fwrite(&info.biCompression, sizeof(unsigned int), 1, saida);
    fwrite(&info.biSizeImage, sizeof(unsigned int), 1, saida);
    fwrite(&info.biXPelsPerMeter, sizeof(int), 1, saida);
    fwrite(&info.biYPelsPerMeter, sizeof(int), 1, saida);
    fwrite(&info.biClrUsed, sizeof(unsigned int), 1, saida);
    fwrite(&info.biClrImportant, sizeof(unsigned int), 1, saida);

    // Escreve os pixels de baixo para cima (formato BMP)
    for (int i = height - 1; i >= 0; i--) {
        for (int j = 0; j < width; j++) {
            unsigned char pixel[3];
            pixel[0] = B[i][j]; // azul
            pixel[1] = G[i][j]; // verde
            pixel[2] = R[i][j]; // vermelho
            fwrite(pixel, sizeof(unsigned char), 3, saida);
        }
        fwrite(pad, sizeof(unsigned char), padding, saida); // padding
    }

    fclose(saida);
}

void RBG2YCbCr(unsigned char **R, unsigned char **G, unsigned char **B, 
    unsigned char **Y, unsigned char **Cb, unsigned char **Cr, int n_linhas, int n_colunas){

        for (int i=0; i < n_linhas; i++){
            for (int j=0; j < n_colunas; j++){
                Y[i][j] = (0.299 * R[i][j]) + (0.587 * G[i][j]) + (0.144 * B[i][j]);
            }
        }

        for (int i=0; i < n_linhas; i++){
            for (int j=0; j < n_colunas; j++){
                Cb[i][j] = 0.564 * (B[i][j] - Y[i][j]);
            }
        }

        for (int i=0; i < n_linhas; i++){
            for (int j=0; j < n_colunas; j++){
                Cr[i][j] = 0.713 * (R[i][j] - Y[i][j]);
            }
        }

}

void YCbCr2RGB(unsigned char **R, unsigned char **G, unsigned char **B, 
    unsigned char **Y, unsigned char **Cb, unsigned char **Cr, int n_linhas, int n_colunas){

        for (int i=0; i < n_linhas; i++){
            for (int j=0; j < n_colunas; j++){
                R[i][j] = (Y[i][j] + (1.402 * Cr[i][j]) ) ;
            }
        }

        for (int i=0; i < n_linhas; i++){
            for (int j=0; j < n_colunas; j++){
                G[i][j] = (Y[i][j] - (0.344 * Cb[i][j]) - (0.714 * Cr[i][j]));
            }
        }

        for (int i=0; i < n_linhas; i++){
            for (int j=0; j < n_colunas; j++){
                B[i][j] = (Y[i][j] + (1.772 * Cb[i][j]) ) ;
            }
        }

}



int main() {
    
    FILE *img;
    char nome_arq_img[100];
    scanf("%s", nome_arq_img);

    img = fopen(nome_arq_img, "rb");
    if (img == NULL){
        printf("Erro ao abrir arquivo \n");
        exit(1);
    }

    //Vou ler o Header da imagem original para poder colocar isso no meu arquivo final
    //E posicionar o ponteiro para começar a ler a imagem
    BMPFILEHEADER header_img;
    leituraHeader(img, &header_img);

    BMPINFOHEADER info_header_img;
    leitura_Header_Img(img, &info_header_img);

    printf("file size header = %d \n", header_img.bfSize);
    printf("file size info header = %d \n", info_header_img.biSize);

    /* ------------------------------------------   */

    unsigned char **R, **G, **B;

    int width = info_header_img.biWidth;
    int height = info_header_img.biHeight;
    int padding = (4 - (width * 3) % 4) % 4; // Cada linha deve ser múltipla de 4 bytes

    // Alocar memória para as matrizes
    R = malloc(height * sizeof(unsigned char*));
    G = malloc(height * sizeof(unsigned char*));
    B = malloc(height * sizeof(unsigned char*));
    for (int i = 0; i < height; i++) {
        R[i] = malloc(width * sizeof(unsigned char));
        G[i] = malloc(width * sizeof(unsigned char));
        B[i] = malloc(width * sizeof(unsigned char));
    }

    // Posiciona o ponteiro para o início dos dados da imagem
    fseek(img, header_img.bfOffBits, SEEK_SET);

    // Leitura linha por linha, de baixo para cima (formato BMP)
    for (int i = height - 1; i >= 0; i--) {
        for (int j = 0; j < width; j++) {
            unsigned char b, g, r;
            fread(&b, sizeof(unsigned char), 1, img);
            fread(&g, sizeof(unsigned char), 1, img);
            fread(&r, sizeof(unsigned char), 1, img);
            B[i][j] = b;
            G[i][j] = g;
            R[i][j] = r;
        }
        // Descartar bytes de padding no final da linha
        fseek(img, padding, SEEK_CUR);
    }

    salvarImagemBMP("imagem_saida.bmp", header_img, info_header_img, R, G, B);
    printf("Imagem salva como imagem_saida.bmp\n");


    return 0;
}