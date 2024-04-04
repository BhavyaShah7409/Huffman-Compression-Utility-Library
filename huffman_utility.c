// huffman_utility.c

#include "huffman_utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

MinHeapNode* newNode(char data, unsigned freq) {
    MinHeapNode* node = (MinHeapNode*)malloc(sizeof(MinHeapNode));
    if (!node) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    node->data = data;
    node->freq = freq;
    node->left = node->right = NULL;
    return node;
}

MinHeap* createMinHeap(unsigned capacity) {
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    if (!minHeap) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (MinHeapNode**)malloc(capacity * sizeof(MinHeapNode*));
    if (!minHeap->array) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    return minHeap;
}

void swapNodes(MinHeapNode** a, MinHeapNode** b) {
    MinHeapNode* temp = *a;
    *a = *b;
    *b = temp;
}

void minHeapify(MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;
    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;
    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;
    if (smallest != idx) {
        swapNodes(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

bool isSizeOne(MinHeap* minHeap) {
    return (minHeap->size == 1);
}

MinHeapNode* extractMin(MinHeap* minHeap) {
    MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

void insertMinHeap(MinHeap* minHeap, MinHeapNode* minHeapNode) {
    ++minHeap->size;
    int i = minHeap->size - 1;
    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = minHeapNode;
}

MinHeapNode* buildHuffmanTree(char data[], int freq[], int size) {
    MinHeapNode *left, *right, *top;
    MinHeap* minHeap = createMinHeap(size);
    for (int i = 0; i < size; ++i)
        insertMinHeap(minHeap, newNode(data[i], freq[i]));
    while (!isSizeOne(minHeap)) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);
        top = newNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        insertMinHeap(minHeap, top);
    }
    return extractMin(minHeap);
}

void writeHuffmanTree(FILE* outputFile, MinHeapNode* root) {
    if (root->left == NULL && root->right == NULL) {
        fputc('1', outputFile); // Indicate leaf node
        fputc(root->data, outputFile);
        return;
    }
    fputc('0', outputFile); // Indicate internal node
    writeHuffmanTree(outputFile, root->left);
    writeHuffmanTree(outputFile, root->right);
}

void compressFile(const char* inputFileName, const char* outputFileName) {
    FILE* inputFile = fopen(inputFileName, "r");
    if (!inputFile) {
        fprintf(stderr, "Error: Unable to open input file %s\n", inputFileName);
        exit(EXIT_FAILURE);
    }
    FILE* outputFile = fopen(outputFileName, "wb");
    if (!outputFile) {
        fprintf(stderr, "Error: Unable to create output file %s\n", outputFileName);
        fclose(inputFile);
        exit(EXIT_FAILURE);
    }
    
    // Count frequency of characters
    int freq[256] = {0};
    char c;
    while ((c = fgetc(inputFile)) != EOF)
        ++freq[(unsigned char)c];
    fseek(inputFile, 0, SEEK_SET); // Reset file pointer
    
    // Write frequency table to compressed file
    fwrite(freq, sizeof(int), 256, outputFile);
    
    // Build Huffman tree
    char data[256];
    int k = 0;
    for (int i = 0; i < 256; ++i) {
        if (freq[i] != 0) {
            data[k++] = (char)i;
        }
    }
    MinHeapNode* root = buildHuffmanTree(data, freq, k);
    
    // Write Huffman tree to compressed file
    writeHuffmanTree(outputFile, root);
    
    // Encode characters and write to compressed file
    unsigned char buffer = 0;
    int bitCount = 0;
    while ((c = fgetc(inputFile)) != EOF) {
        MinHeapNode* current = root;
        while (current->left != NULL && current->right != NULL) {
            if ((c & (1 << bitCount)) == 0) // Move left
                current = current->left;
            else // Move right
                current = current->right;
            ++bitCount;
            if (bitCount == 8) { // Buffer full, write to file
                fputc(buffer, outputFile);
                buffer = 0;
                bitCount = 0;
            }
        }
    }
    
    // Write remaining bits in buffer to file
    if (bitCount > 0) {
        fputc(buffer, outputFile);
    }
    
    // Close files
    fclose(inputFile);
    fclose(outputFile);
}

MinHeapNode* readHuffmanTree(FILE* inputFile) {
    int bit = fgetc(inputFile);
    if (bit == '1') { // Leaf node
        char data = fgetc(inputFile);
        return newNode(data, 0);
    }
    // Internal node
    MinHeapNode* node = newNode('$', 0);
    node->left = readHuffmanTree(inputFile);
    node->right = readHuffmanTree(inputFile);
    return node;
}

void decompressFile(const char* inputFileName, const char* outputFileName) {
    FILE* inputFile = fopen(inputFileName, "rb");
    if (!inputFile) {
        fprintf(stderr, "Error: Unable to open input file %s\n", inputFileName);
        exit(EXIT_FAILURE);
    }
    FILE* outputFile = fopen(outputFileName, "w");
    if (!outputFile) {
        fprintf(stderr, "Error: Unable to create output file %s\n", outputFileName);
        fclose(inputFile);
        exit(EXIT_FAILURE);
    }
    
    // Read frequency table from compressed file
    int freq[256];
    fread(freq, sizeof(int), 256, inputFile);
    
    // Reconstruct Huffman tree
    MinHeapNode* root = readHuffmanTree(inputFile);
    
    // Decode compressed data and write to output file
    MinHeapNode* current = root;
    unsigned char buffer = fgetc(inputFile);
    while (!feof(inputFile)) {
        for (int i = 7; i >= 0; --i) {
            if (current->left == NULL && current->right == NULL) { // Leaf node
                fputc(current->data, outputFile);
                current = root;
            }
            if (buffer & (1 << i)) // Move right
                current = current->right;
            else // Move left
                current = current->left;
        }
        buffer = fgetc(inputFile);
    }
    
    // Close files
    fclose(inputFile);
    fclose(outputFile);
}
