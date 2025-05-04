#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define NO_SYNTH

#define ALPHABET_SIZE 27
#define MAX_NODES 2 * ALPHABET_SIZE - 1
#define MAX_CODE_LENGTH 254

/*Node of the Huffman tree */
typedef struct {
    int value;
    char letter;
    int left;
    int right;
    int valid; // 0 = inactive (used in merged node), 1 = active
} Node;

// Frequencies from Wikipedia
int englishLetterFrequencies[ALPHABET_SIZE] = {
    81, 15, 28, 43, 128, 23, 20, 61, 71,
    2, 1, 40, 24, 69, 76, 20, 1, 61,
    64, 91, 28, 10, 24, 1, 20, 1, 130
};

int leng(int n) {
    int l = 0;
    while (n > 0) {
        n /= 10;
        l++;
    }
    return l;
}

void buildHuffmanTree(Node tree[], int *rootIndex) {
    int count = 27;
    for (int i = 0; i < 27; i++) {
        tree[i].value = englishLetterFrequencies[i];
        tree[i].letter = i;
        tree[i].left = -1;
        tree[i].right = -1;
        tree[i].valid = 1;
    }

    while (1) {
        int min1 = -1, min2 = -1;
        //#pragma HLS UNROLL  //optimization: loop unrolling
        for (int i = 0; i < count; i++) {
            if (!tree[i].valid) continue;
            if (min1 == -1 || tree[i].value < tree[min1].value)
                min2 = min1, min1 = i;
            else if (min2 == -1 || tree[i].value < tree[min2].value)
                min2 = i;
        }
        if (min2 == -1) break;

        tree[min1].valid = 0;
        tree[min2].valid = 0;

        tree[count].value = tree[min1].value + tree[min2].value;
        tree[count].letter = 127;
        tree[count].left = min1;
        tree[count].right = min2;
        tree[count].valid = 1;
        count++;
    }
    //had to change i to j here would not let me run synthesis
    for (int j = 0; j < count; j++)
        if (tree[j].valid)
            *rootIndex = j;
}

void fillCodeTable(Node tree[], int rootIndex, int table[]) {
    int stack[MAX_NODES];
    int codes[MAX_NODES];
    int top = 0;

    stack[top] = rootIndex;
    codes[top++] = 0;

    while (top > 0) {
        int nodeIndex = stack[--top];
        int code = codes[top];

        if (tree[nodeIndex].letter != 127) {
            table[(int)tree[nodeIndex].letter] = code;
        } else {
            stack[top] = tree[nodeIndex].right;
            codes[top++] = code * 10 + 2;

            stack[top] = tree[nodeIndex].left;
            codes[top++] = code * 10 + 1;
        }
    }
}

char* compressString(char* input, int codeTable[]) {
    static char output[MAX_CODE_LENGTH];
    int outputPosition = 0;
    unsigned char x = 0;
    int bitsLeft = 8;

    for (int i = 0; input[i] != '\0'; i++) {
        char ch = input[i];
        // Normalize to lowercase
        if (ch >= 'A' && ch <= 'Z')
            ch = ch - 'A' + 'a';

        int letterIndex = (ch == ' ') ? 26 : ch - 'a';
        int code = codeTable[letterIndex];
        int length = leng(code);

        int bitStack[16];
        for (int k = 0; k < length; k++) {
            bitStack[k] = code % 10 - 1;
            code /= 10;
        }
        //had to change from k to l, wouldnt let me run synthesis
        for (int l = length - 1; l >= 0; l--) {
            x = (x << 1) | (bitStack[l] & 1);
            bitsLeft--;

            if (bitsLeft == 0) {
                output[outputPosition++] = x;
                x = 0;
                bitsLeft = 8;
            }
        }
    }

    if (bitsLeft != 8) {
        x <<= bitsLeft;
        output[outputPosition++] = x;
    }

    output[outputPosition] = '\0';
    return output;
}

char* decompressString(char *input, Node tree[], int rootIndex) {
    static char output[MAX_CODE_LENGTH];
    int outIndex = 0;
    int currentIndex = rootIndex;

    //for each character in the compressed string
    for (int i = 0; input[i] != '\0'; i++) {
        
        //for each byte in th character traverse the tree. 
        unsigned char byte = input[i];
        for (int b = 7; b >= 0; b--) {
            int bit = (byte >> b) & 1;

            if (bit == 0)
                currentIndex = tree[currentIndex].left;
            else
                currentIndex = tree[currentIndex].right;

            //if this is not a middle node in the tree (i.e. an actual char)
            if (tree[currentIndex].letter != 127) {
                output[outIndex++] = (tree[currentIndex].letter == 26) ?
                    ' ' : (char)(tree[currentIndex].letter + 'a');
                currentIndex = rootIndex; //reset to root of tree.
            }
        }

    }

    output[outIndex] = '\0';
    return output;
}


//input arguements: (String to handle, (1 = compress, 0 = decompress))
char* huffmanStart(char* input, int compress){
    Node tree[MAX_NODES];
    int rootIndex;
    int codeTable[27] = {0}; 

    
    buildHuffmanTree(tree, &rootIndex);
    fillCodeTable(tree, rootIndex, codeTable);

    if (input == NULL){
        printf("string to (de)compress is null. Exiting program.");
        return input;
    }    


    if (compress == 1){
        return compressString(input,codeTable);
    }
    else {
        return decompressString(input, tree, rootIndex);
    }
}
