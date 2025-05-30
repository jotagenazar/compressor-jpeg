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
