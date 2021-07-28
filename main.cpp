// Nick Dobmeier
// CS 4349 HW6

#include <string>
#include <iostream>
#include <stdlib.h>
#include <vector> // used for Vectors
using namespace std;

// Global Variable
const short NUM_ALPHABET = 122-32+1;        // allowed chars from [space] to 'z'


class Letter{
public:
    char letter;
    int frequency;
    vector<char> huffCode;
    
    Letter(){
        letter = 0;
        frequency = 0;
    }
    
    Letter(char letter, int frequency){
        this->letter = letter;
        this-> frequency = frequency;
    }
};



class LetterNode{
public:
    Letter* payload;       // instead of creating new Letter objects, just point to the letter in the original countLetter array in Main
    LetterNode* left;
    LetterNode* right;
    
    LetterNode(Letter* letter){     // constructor used when creating LEAF nodes (ie. actual letters)
        payload = letter;
        left = NULL;
        right = NULL;
    }
    
    LetterNode(Letter* letter, LetterNode* left, LetterNode* right){    // constructor used when creating INTERNAL nodes
        this->payload = letter;
        this->left = left;
        this->right = right;
    }
};



class MinHeap{
private:
    vector<LetterNode*> heap;   // since will be placing trees back in the Heap, needs to be heap of NODE pointers
    // note: vectors are pass by VALUE by default
    
    
    
    void buildHeap(){
        for(int i = (heap.size()/2)-1; i >= 0; i--)     // (size/2)-1  skips over ALL nodes with no children
        {                                               // include i==0, since have not checked that node yet
            minHeapify(i);
        }
    }
    
    
    void minHeapify(int index)      // percolate DOWN the heap
    {
        do{
            int leftChild = (index*2)+1;
            int rightChild = (index*2)+2;
            int smallest = index;
            
            if(rightChild < heap.size() && heap[index]->payload->frequency > heap[rightChild]->payload->frequency){
                smallest = rightChild;
            }
            if(leftChild < heap.size() && heap[smallest]->payload->frequency > heap[leftChild]->payload->frequency){
                smallest = leftChild;
            }
            
            if(smallest != index){
                swap(index, smallest);
                index = smallest;
            }else{
                break;
            }
        }while (1);
    }
    
    
    void swap(int indexA, int indexB)
    {
        LetterNode* temp = heap[indexA];
        heap[indexA] = heap[indexB];
        heap[indexB] = temp;
    }                           // indexA will now point to what indexB was pointing at, and vice-versa
    
    
public:
    
    MinHeap(Letter countLetter [])
    {
        for(int i=0; i < NUM_ALPHABET; i++){
            if(countLetter[i].frequency != 0){
                char TEMP = countLetter[i].letter; //*******
                heap.push_back(new LetterNode(&countLetter[i]));        // "new" keyword returns and ADDRESS *******
            }
        }
        buildHeap();        // O(n) + O(n)  is better than  O(nlgn)
    }
    
    
    LetterNode* extractMin()
    {
        if(heap.empty() == false){
            LetterNode* remove = heap[0];
            if((heap.size()-1) != 0){           // if this is NOT the last element in the heap
                swap(0, heap.size()-1);         // move the minimum element to the END, then remove it
            }
            
            heap.pop_back();
            
            if(heap.empty() == false){
                minHeapify(0);
            }
            return remove;
        }
        throw -1;       // throw an exception when there is nothing left to extract
    }
    
    
    void add(LetterNode* item)      // add subtrees back to the Min-Heap
    {
        heap.push_back(item);
        int currIndex = heap.size()-1;
        while(currIndex > 0 && heap[currIndex]->payload->frequency <= heap[(currIndex-1)/2]->payload->frequency)    // <=  keeps everything on RIGHTMOST side even when equal
        {
            swap(currIndex, (currIndex-1)/2);
            currIndex = (currIndex-1)/2;            //percolate up tree until find proper location
        }
    }
    
    
    int getSize(){return heap.size();}
    
};




class HuffmanCode{
private:
    LetterNode* root = NULL;
    
    
    void dfsDetermineHuffmanCode(LetterNode* curr, vector<char> tempCode)
    {
        if(curr == NULL){   // when ROOT is null
            return;
        }
        if(curr->left == NULL && curr->right == NULL){
            curr->payload->huffCode = tempCode;
            return;
        }
        if(curr->left != NULL){
            tempCode.push_back('0');    // 0  for left branch
            dfsDetermineHuffmanCode(curr->left, tempCode);
            tempCode.pop_back();
        }
        if(curr->right != NULL){
            tempCode.push_back('1');    // 1  for right branch
            dfsDetermineHuffmanCode(curr->right, tempCode);
        }
    }
    
    void dfsDestrcutorHelper(LetterNode* curr)
    {
        if(curr == NULL){   // when ROOT is null
            printf("Root is NULL\n");
            return;
        }
        if(curr->left == NULL && curr->right == NULL){
            return;
        }
        if(curr->left != NULL){
            dfsDestrcutorHelper(curr->left);
            delete curr->left;      // delete LEFT child LetterNode object (internal nodes only)
        }
        if(curr->right != NULL){
            dfsDestrcutorHelper(curr->right);
            delete curr->right;     // delete RIGHT child LetterNode object
        }
        delete curr->payload;       // only delete payload of INTERNAL nodes (which were allocated on the heap)
    }                               //    payload of LEAF nodes are actually elements of the 'countLetter' array
    
    
public:
    
    HuffmanCode(Letter countLetter [])
    {
        MinHeap minHeap(countLetter);
        while (minHeap.getSize() >= 2) {
            //LetterNode* small1 = new LetterNode (minHeap.extractMin());
            //LetterNode* small2 = new LetterNode (minHeap.extractMin());
            LetterNode* small1 = minHeap.extractMin();
            LetterNode* small2 = minHeap.extractMin();
            
            Letter* internalNode = new Letter(0, small2->payload->frequency + small1->payload->frequency);
            LetterNode* subRoot = new LetterNode(internalNode, small2, small1);
            minHeap.add(subRoot);
            root = subRoot;     // subRoot is redundent, but using as reminder for notation
                                // http://www.cplusplus.com/forum/beginner/2699/
            
        }
        vector<char> tempCode;
        dfsDetermineHuffmanCode(root, tempCode);
    }
    
    ~HuffmanCode(){
        dfsDestrcutorHelper(root);
        delete root;
    }

};



int main(int argc, const char * argv[])
{
    string input;// = "abcdefghijklmnopqrstuvwxyz";
    printf("input string:  ");
    getline(cin, input);
    
    Letter countLetter [NUM_ALPHABET];
    for(int i=0; i < NUM_ALPHABET; i++){
        countLetter[i].letter = i+32;
        countLetter[i].frequency = 0;
    }
    
    
    for(int i=0; i < input.length(); i++)
    {
        if(input[i] < ' ' || input[i] > 'z'){   // values between ASCII 32-122 are allowed (inclusive)
            printf("found an INVALID character in the string. TERMINATING.\n");
            return 1;
        }
        countLetter[input[i]-32].frequency += 1;
    }
    
    
    HuffmanCode huffmanTree (countLetter);
    
    
    // HuffmanTree uses payloads to point DIRECTLY to the actual Letter objects of the countLetter array. So those H-Codes are updated
    printf("characters%6sFrequency%6sCodeword\n", " ", " ");
    for(int j=0; j < NUM_ALPHABET; j++)
    {
        if(countLetter[j].frequency != 0)
        {
            printf("%c%15s%d%14s", j+32, " ", countLetter[j].frequency, " ");
     
            for(int h=0; h < countLetter[j].huffCode.size(); h++){
                printf("%c", countLetter[j].huffCode[h]);
            }
            printf("\n");
        }
    }
    
    return 0;
}
