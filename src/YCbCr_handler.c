/************************************
* INCLUDES
************************************/

    #include "../include/YCbCr_handler.h"

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

    RGBImg alocar_RGB(int width, int height) 
    {
        RGBImg rgb_img;

        rgb_img.height = height;
        rgb_img.width = width;

        rgb_img.R = malloc(rgb_img.height * sizeof(unsigned char*));
        rgb_img.G = malloc(rgb_img.height * sizeof(unsigned char*));
        rgb_img.B = malloc(rgb_img.height * sizeof(unsigned char*));

        for (int i = 0; i < height; i++) 
        {
            rgb_img.R[i] = malloc(rgb_img.width * sizeof(unsigned char));
            rgb_img.G[i] = malloc(rgb_img.width * sizeof(unsigned char));
            rgb_img.B[i] = malloc(rgb_img.width * sizeof(unsigned char));
        }

        return rgb_img;
    }


    void liberar_RGB(RGBImg rgb_img)
    {
        for (int i = 0; i < rgb_img.height; i++) 
        {
            free(rgb_img.R[i]);
            free(rgb_img.G[i]);
            free(rgb_img.B[i]);
        }

        free(rgb_img.R); free(rgb_img.G); free(rgb_img.B);
    }


    YCbCrImg alocar_YCbCr(int width, int height) 
    {
        YCbCrImg YCbCr_img;

        YCbCr_img.height = height;
        YCbCr_img.width = width;

        YCbCr_img.Y = malloc(YCbCr_img.height * sizeof(double*));
        YCbCr_img.Cb = malloc(YCbCr_img.height * sizeof(double*));
        YCbCr_img.Cr = malloc(YCbCr_img.height * sizeof(double*));

        for (int i = 0; i < height; i++) 
        {
            YCbCr_img.Y[i] = malloc(YCbCr_img.width * sizeof(double));
            YCbCr_img.Cb[i] = malloc(YCbCr_img.width * sizeof(double));
            YCbCr_img.Cr[i] = malloc(YCbCr_img.width * sizeof(double));
        }

        return YCbCr_img;
    }

    void liberar_YCbCr(YCbCrImg YCbCr_img)
    {
        for (int i = 0; i < YCbCr_img.height; i++) 
        {
            free(YCbCr_img.Y[i]);
            free(YCbCr_img.Cb[i]);
            free(YCbCr_img.Cr[i]);
        }

        free(YCbCr_img.Y); free(YCbCr_img.Cb); free(YCbCr_img.Cr);
    }

    void liberar_YCbCr_reduced(YCbCrImg YCbCr_img)
    {
        for (int i = 0; i < YCbCr_img.height; i++) 
        {
            free(YCbCr_img.Y[i]);
        }

        for (int i = 0; i < YCbCr_img.height/2; i++)
        {
            free(YCbCr_img.Cb[i]);
            free(YCbCr_img.Cr[i]);
        } 

        free(YCbCr_img.Y); free(YCbCr_img.Cb); free(YCbCr_img.Cr);
    }


    void RGB2YCbCr(YCbCrImg YCbCr_img, RGBImg rgb_img)
    {
        //double R, G, B;
        double Y, Cb, Cr;

        for (int i = 0; i < rgb_img.height; i++) {
            for (int j = 0; j < rgb_img.width; j++) {

                Y  = 0.299 * rgb_img.R[i][j] + 0.587 * rgb_img.G[i][j] + 0.114 * rgb_img.B[i][j];
                Cb = 0.564 * (rgb_img.B[i][j] - Y);
                Cr = 0.713 * (rgb_img.R[i][j] - Y);

                YCbCr_img.Y[i][j]  = Y - 128.0;
                YCbCr_img.Cb[i][j] = Cb;
                YCbCr_img.Cr[i][j] = Cr;
            }
        }
    }

  void YCbCr2RGB(YCbCrImg YCbCr_img, RGBImg rgb_img)
    {
        double Y, Cb, Cr;
        int R, G, B;

        for (int i = 0; i < YCbCr_img.height; i++) {
            for (int j = 0; j < YCbCr_img.width; j++) {
                Y  = YCbCr_img.Y[i][j] + 128.0;  // Reverte o level shifting
                Cb = YCbCr_img.Cb[i][j];
                Cr = YCbCr_img.Cr[i][j];

                R = round(Y + 1.402 * Cr);
                G = round(Y - 0.344 * Cb - 0.714 * Cr);
                B = round(Y + 1.772 * Cb);

                // Clamp entre 0 e 255
                if (R < 0) R = 0; 
                if (R > 255) R = 255;
                if (G < 0) G = 0; 
                if (G > 255) G = 255;
                if (B < 0) B = 0; 
                if (B > 255) B = 255;

                rgb_img.R[i][j] = (unsigned char)R;
                rgb_img.G[i][j] = (unsigned char)G;
                rgb_img.B[i][j] = (unsigned char)B;
            }
        }
    }

    YCbCrImg downsamplig(YCbCrImg YCbCr_img)
    {
        YCbCrImg YCbCr_img_reduced;

        int height = YCbCr_img.height;
        int width = YCbCr_img.width;

        // ver se preciso fazer padding na Cb e Cr
        int height_pad;
        if (height % 16 == 0){
            height_pad = height;
        } else {
            height_pad = height + (16 - (height % 16));
        }

        int width_pad;
        if (width % 16 == 0){
            width_pad = width;
        } else {
            width_pad = width + (16 - (width % 16));
        }

        // Alocar espaco para a nova cb e cr que recebera o padding para poder fazer o downsampling
        double **Cb_padded = malloc(height_pad * sizeof(double*));
        double **Cr_padded = malloc(height_pad * sizeof(double*));

        for (int i = 0; i < height_pad; i++) {
            // Cria espaco na matriz
            Cb_padded[i] = malloc(width_pad * sizeof(double));
            Cr_padded[i] = malloc(width_pad * sizeof(double));

            for (int j = 0; j < width_pad; j++) {
                // Copia a matriz e colocando padding se precisar
                 int i_orig = i;
                int j_orig = j;
                if (i >= height) i_orig = height - 1;
                if (j >= width) j_orig = width - 1;
                Cb_padded[i][j] = YCbCr_img.Cb[i_orig][j_orig];
                Cr_padded[i][j] = YCbCr_img.Cr[i_orig][j_orig];
            }
        }

        // Aloca espaco para o Cb e Cr com metade do tamanho
        int reduced_height = height_pad / 2;
        int reduced_width = width_pad / 2;

        double** Cb_reduzida = malloc(reduced_height * sizeof(double*));
        double** Cr_reduzida = malloc(reduced_height * sizeof(double*));
        for (int i = 0; i < reduced_height; i++) {
            Cb_reduzida[i] = malloc(reduced_width * sizeof(double));
            Cr_reduzida[i] = malloc(reduced_width * sizeof(double));
        }
        
        // Faz o downsampling
        for (int i = 0; i < height_pad; i += 2) {
            for (int j = 0; j < width_pad; j += 2) {
                double cb_soma = Cb_padded[i][j] + Cb_padded[i+1][j] + Cb_padded[i][j+1] + Cb_padded[i+1][j+1];
                double cr_soma = Cr_padded[i][j] + Cr_padded[i+1][j] + Cr_padded[i][j+1] + Cr_padded[i+1][j+1];

                Cb_reduzida[i/2][j/2] = cb_soma / 4.0;
                Cr_reduzida[i/2][j/2] = cr_soma / 4.0;
            }
        }

        // Cria espaco para Y e copia
        double** Y_padded = malloc(height_pad * sizeof(double*));
        for (int i = 0; i < height_pad; i++) {
            Y_padded[i] = malloc(width_pad * sizeof(double));
            for (int j = 0; j < width_pad; j++) {
                int i_orig = i;
                int j_orig = j;
                if (i >= height) 
                    i_orig = height - 1;
                if (j >= width) 
                    j_orig = width - 1;

                Y_padded[i][j] = YCbCr_img.Y[i_orig][j_orig];
            }
        }

        // Seta os ponteiros
        YCbCr_img_reduced.height = height_pad;
        YCbCr_img_reduced.width = width_pad;
        YCbCr_img_reduced.Y = Y_padded;
        YCbCr_img_reduced.Cb = Cb_reduzida;
        YCbCr_img_reduced.Cr = Cr_reduzida;

        // Free da matriz padded
        for (int i = 0; i < height_pad; i++) {
            free(Cb_padded[i]);
            free(Cr_padded[i]);
        }
        free(Cb_padded);
        free(Cr_padded);

        // Return
        return YCbCr_img_reduced; 
    }


