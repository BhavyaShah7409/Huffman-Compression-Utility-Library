#include "huffman_utility.h"
#include <stdio.h>

int main() {
    const char* inputFileName = "input.txt";
    const char* compressedFileName = "compressed.bin";
    const char* decompressedFileName = "decompressed.txt";

    // Compress the input file
    printf("Compressing file...\n");
    compressFile(inputFileName, compressedFileName);
    printf("Compression completed.\n");

    // Decompress the compressed file
    printf("Decompressing file...\n");
    decompressFile(compressedFileName, decompressedFileName);
    printf("Decompression completed.\n");

    return 0;
}
