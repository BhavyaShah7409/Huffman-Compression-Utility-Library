#ifndef HUFFMAN_UTILITY_H
#define HUFFMAN_UTILITY_H

#include <stdio.h>
#include <stdbool.h>

// Structure for Huffman tree node
typedef struct MinHeapNode {
    char data;
    unsigned freq;
    struct MinHeapNode *left, *right;
} MinHeapNode;

// Structure for Min Heap
typedef struct MinHeap {
    unsigned size;
    unsigned capacity;
    MinHeapNode** array;
} MinHeap;

// Function declarations
MinHeapNode* newNode(char data, unsigned freq);
MinHeap* createMinHeap(unsigned capacity);
void swapNodes(MinHeapNode** a, MinHeapNode** b);
void minHeapify(MinHeap* minHeap, int idx);
bool isSizeOne(MinHeap* minHeap);
MinHeapNode* extractMin(MinHeap* minHeap);
void insertMinHeap(MinHeap* minHeap, MinHeapNode* minHeapNode);
MinHeapNode* buildHuffmanTree(char data[], int freq[], int size);
void writeHuffmanTree(FILE* outputFile, MinHeapNode* root);
void compressFile(const char* inputFileName, const char* outputFileName);
MinHeapNode* readHuffmanTree(FILE* inputFile);
void decompressFile(const char* inputFileName, const char* outputFileName);

#endif
