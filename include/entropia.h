#ifndef ENTROPIA_HANDLER_H
#define ENTROPIA_HANDLER_H

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



extern const int zigzag[64][2];



/************************************
* TYPEDEFS AND STRUCTS
*************************************/


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

    

#endif
