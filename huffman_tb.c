#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

//test bench function to compare the results. takes in two strings for comp. res = 0 is a pass. 
int compareFunc(int argc, char *argv[]);

//huffman function. takes in two strings (String to handle, ("1" = compress, "\0" = decompress))
char* huffmanStart(char* input, int compress);

int main () {
    //Text to be compressed.
    //The program is not designed to handled uppercase letters.
    char *originalText = "testing text";

    //compress the string
    printf("Compressing the string.\n");
	char *compressed = huffmanStart(originalText, 1); 

    //now decompress the file. 
    printf("decompressing the string. The compressed string is: %s\n", compressed);
    char *decompressed = huffmanStart(compressed, 0);

    //compare the input file to the output file
    printf("comparing input vs output.");
    char *char_array_list[] = {originalText, decompressed};
    return compareFunc(2, char_array_list);
}



int compareFunc(int argc, char *argv[]) {
    char *compStringOne = argv[0];
    char *compStringTwo = argv[1];

    //ensure values are assigned. 
    if (compStringOne == NULL || compStringTwo == NULL) {
        printf("One or both of the Strings are NULL.\n ");
        return 1;
    }

    printf("\n arg zero is: %s\n", compStringOne);
    printf("arg One is: %s\n", compStringTwo);
    if ((strcmp(compStringOne, compStringTwo)) == 0) {
        //if the two strings are equal, then it is a pass. 
        return 0;
    }
    return 1;
}
