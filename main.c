#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <dirent.h>

#define MAX_LENGTH 32
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define RESET   "\033[0m"
double startAddress = 0; //global double startaddress
int labelCounter= 0; //global counter for how many labels there are

/* function to check if it is a label */
bool islabel(char input[]){
    if ((strcmp(input, "ADD") == 0 //compare to LC3 instructions
         || strcmp(input, "AND") == 0
         || strcmp(input, "NOT") == 0
         || strcmp(input, "BR") == 0
         || strcmp(input, "BRn") == 0
         || strcmp(input, "BRz") == 0
         || strcmp(input, "BRp") == 0
         || strcmp(input, "BRnz") == 0
         || strcmp(input, "BRnp") == 0
         || strcmp(input, "BRzp") == 0
         || strcmp(input, "BRzn") == 0
         || strcmp(input, "BRpn") == 0
         || strcmp(input, "BRpz") == 0
         || strcmp(input, "BRnzp") == 0
         || strcmp(input, "BRnpz") == 0
         || strcmp(input, "BRznp") == 0
         || strcmp(input, "BRzpn") == 0
         || strcmp(input, "BRpzn") == 0
         || strcmp(input, "BRpnz") == 0
         || strcmp(input, "JMP") == 0
         || strcmp(input, "JSR") == 0
         || strcmp(input, "JSRR") == 0
         || strcmp(input, "RET") == 0
         || strcmp(input, "LD") == 0
         || strcmp(input, "LDI") == 0
         || strcmp(input, "LDR") == 0
         || strcmp(input, "LEA") == 0
         || strcmp(input, "ST") == 0
         || strcmp(input, "STI") == 0
         || strcmp(input, "STR") == 0
         || strcmp(input, "TRAP") == 0
         || strcmp(input, "RTI") == 0))
    {
        return 0;
    }
    else if(input[0] == '.'){ //if it has a . it is a LC3 instruction
        return 0;
    }
    else { //is a label
        return 1;
    }
};


/* Struct to make symbol table given by address and name */
typedef struct symboltable{
    double address;
    char labelname[50];
} labels; //we call it labels

labels* labelsPointer; //make a pointer to labels and make it global



/* If we have a label as an argument (i.e. LD MESG) we need to make the array pointer into decimal number */
void labelToOffsetConverter(char* str, double currentAddress){
    int labeloffset=0, counter=0;
    for (int i = 0; i < labelCounter; ++i) { //go all labels over and compare the str name to the label name
        if ((strcmp(str, ((labelsPointer + i) -> labelname))) == 0){
            for (int j = 0; j < 32; ++j) { //if a label clear the str array
                str[j] = '\0';
            }
            str[0] = '#'; //insert # into first char
            labeloffset = (int)((labelsPointer+i)->address-(currentAddress-1)); //get the offset-1 cause the address goes 1 up with every call
            if (labeloffset<0){ //if offset is negative make positive and input - into second char
                labeloffset = labeloffset*(-1);
                str[1] = '-';
            }
            int a[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // make a new array to hold the binary numbers
            for(int z=0;labeloffset>0;z++)
            {
                a[z]=labeloffset%10; //modulus 10 to get the number as chars
                labeloffset=labeloffset/10; // divide by 10 to get the numbers as chars
                counter++;
            }
            if(str[1] == '-'){ //if the offset is negative use this
                for (int k = 0; k < counter; ++k) {
                    str[k+2] = a[counter-k-1]+'0';
                }
            }
            else { //if offset is positive use this
                for (int k = 0; k < counter; ++k) {
                    str[k+1] = a[counter-k-1]+'0';
                }
            }
            break; //don't go through other labels to check for strcmp
        }
    }
}





/* Number from whole char[] to a double */
double CharConverter(char input[]){
    double number=0;
    for (int i = 0; i < 16; ++i) {
        if (input[i] != 'x' && input[i] != '#' && input[i] != '\0' && input[i] != 'R' && input[i] != 'r' && input[i] != '-'){
            number += (-48+input[i])*(pow(10,16-i)); //16 bits at most so we pow it with 16-i
        }
    }
    return number;
}


/* Converter from hexadecimal to decimal */
double hextodecimal(char input[]){
    int counter=0; //counter for how big the number is in length
    while(input[counter] != '\0' && input[counter] != '\n'){counter++;} //plusses 1 if there is a char that is not newline or \0
    double totalnummer = 0; //the total number from the hex
    double numberToAdd = 0; //current char to add as decimal
    for (int i = 0; i < counter; i++) {
        if (input[i] == 'A' || input[i] == 'a'){numberToAdd = 10;}
        else if (input[i] == 'B' || input[i] == 'b'){numberToAdd = 11;}
        else if (input[i] == 'C' || input[i] == 'c'){numberToAdd = 12;}
        else if (input[i] == 'D' || input[i] == 'd'){numberToAdd = 13;}
        else if (input[i] == 'E' || input[i] == 'e'){numberToAdd = 14;}
        else if (input[i] == 'F' || input[i] == 'f'){numberToAdd = 15;}
        else {numberToAdd = input[i]-48;}

        if (input[i] != 'x' && input[i] != 'X' && input[i] != '-' && input[i] != ' ')
        {totalnummer += numberToAdd * pow(16,counter-1-i);}
    }
    return totalnummer;
}



/* converts a char array to a binary array (takes registers, hex and decimal) */
void numberConverter(char number[],char *array, int arrcounter, int inputlength, FILE* wp){
    int counter=0; //counter for how big the number is in length
    while(number[counter] != '\0' && number[counter] != '\n'){counter++;} //plusses 1 if there is a char that is not newline or \0

    if (number[0] == '#' || number[0] == 'R' || number[0] == 'r') //if it is decimal or a register
    {
        double nummer = CharConverter(number) / pow(10,16-(counter-1)); // we get the input turned from register or number to a double
        int a[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // make a new array to hold the binary numbers
        for(int i=0;(int)nummer>0;i++)
        {
            a[i]=(int)nummer%2; //modulus 2 to get a 1 or 0
            nummer=nummer/2; // divide by 2 to get to next number in binary
        }

        if(number[1] == '-'){ //if it is negative we have to flip all bits and +1 in the end
            for (int i = 0; i < 16; ++i) { //this is flipping bits
                if(a[i] == 1){
                    a[i] = 0;
                }
                else{
                    a[i] = 1;
                }
            }
            counter = 0;
            while(a[counter] == 1){ //add 1 if already 1 make 0 and check next
                a[counter] =0;
                counter++;
            }
            a[counter] = 1;//make the counter bit a 1
            a[15] = 1; //last bit is always 1 if it is negative
        }

        for (int j = 0; j < inputlength; j++) { //new for loop to make our binary numbers be placed in the original array
            array[arrcounter++] = a[inputlength-j-1] + '0'; // + '0' is to make the int to a char
        }
    }
    else{
        double totalnummer = 0; //total number to make binary
        double numberToAdd = 0; //number to add to total number
        for (int i = 0; i < counter; i++) {
            if (number[i] == 'A' || number[i] == 'a'){numberToAdd = 10;}
            else if (number[i] == 'B' || number[i] == 'b'){numberToAdd = 11;}
            else if (number[i] == 'C' || number[i] == 'c'){numberToAdd = 12;}
            else if (number[i] == 'D' || number[i] == 'd'){numberToAdd = 13;}
            else if (number[i] == 'E' || number[i] == 'e'){numberToAdd = 14;}
            else if (number[i] == 'F' || number[i] == 'f'){numberToAdd = 15;}
            else {numberToAdd = number[i]-48;} //if already a number make it into non-ascii value

            if (number[i] != 'x' && number[i] != 'X' && number[i] != '-')
            {totalnummer += numberToAdd * pow(16,counter-1-i);}
        }


        int a[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // make a new array to hold the binary numbers
        for(int i=0;(int)totalnummer>0;i++)
        {
            a[i]=(int)totalnummer%2; //modulus 2 to get a 1 or 0
            totalnummer=totalnummer/2; // divide by 2 to get to next number in binary
        }

        if(number[1] == '-'){//if it is negative we have to flip all bits and +1 in the end
            for (int i = 0; i < 16; ++i) {//this is flipping bits
                if(a[i] == 1){
                    a[i] = 0;
                }
                else{
                    a[i] = 1;
                }
            }
            counter = 0;
            while(a[counter] == 1){//add 1 if already 1 make 0 and check next
                a[counter] =0;
                counter++;
            }
            a[counter] = 1; //make the counter bit a 1
            a[15] = 1;//last bit is always 1 if it is negative
        }


        for (int j = 0; j < inputlength; j++) { //new for loop to make our binary numbers be placed in the original array
            array[arrcounter++] = a[inputlength-j-1] + '0'; // + '0' is to make the int to a char
        }
    }
}




/* This is the converter from all inputs of a line into binary representation */
void LC3Converter(char arr1[],int firstcounter,char arr2[],char arr3[],char arr4[],char* strarray, FILE* wp){
    long inputasciivalue=0;
    for (int i = 0; i < firstcounter; ++i) {
        inputasciivalue += arr1[i]; // we will switch the sum of the ascii numbers
    }


    if (arr1[0] == 'R' && arr1[1] == 'T'){ //RTI
        char word[16] = "1000000000000000"; //RTI is always set to this
        for (int i = 0; i < 16; ++i) {
            strarray[i] = word[i];
        }
    }


    else if(arr1[0] == 'B' || arr1[0] == 'b'){ //Branch
        int counter=0;
        char word[16] = "0000000"; //first 7 chars is set to this
        while(arr1[counter]!=' '){counter++;}//get length of first input
        for (int i = 2; i < counter+1; ++i) {//check for n, z or p
            if (arr1[i] == 'n' || arr1[i] == 'N') word[4] = '1';
            else if (arr1[i] == 'z' || arr1[i] == 'Z') word[5] = '1';
            else if (arr1[i] == 'p' || arr1[i] == 'P') word[6] = '1';
        }

        if (word[4] == '0' && word[5] == '0' && word[6] == '0'){
            word[4] = '1'; word[5] = '1'; word[6] = '1'; //if no nzp all will be 1
        }
        numberConverter(arr2,word,7,9, wp);//get binary of input of length "inputlength"
        for (int i = 0; i < 16; ++i) {
            strarray[i] = word[i];
        }
    }



    switch(inputasciivalue)
    {
        case 201: //ADD
        {
            char word[16] = "0001";//first 4 chars is set to this
            numberConverter(arr2, word, 4,3, wp);//get binary of input of length "inputlength"
            numberConverter(arr3,word,7,3, wp);//get binary of input of length "inputlength"
            if (arr4[0] == 'x' || arr4[0] == 'X' || arr4[0] == '#'){//If 4th argument of input is not a register use this
                word[10] = '1';
                numberConverter(arr4,word,11,5, wp);//get binary of input of length "inputlength"
            } else {//If 4th argument of input is a register use this
                word[10] = '0'; word[11] = '0'; word[12] = '0';
                numberConverter(arr4,word,13,3, wp);//get binary of input of length "inputlength"
            }
            for (int i = 0; i < 16; ++i) {
                strarray[i] = word[i];
            }
            break;
        }


        case 211: //AND
        {
            char word[16] = "0101";//first 4 chars is set to this
            numberConverter(arr2, word, 4,3, wp);//get binary of input of length "inputlength"
            numberConverter(arr3,word,7,3, wp);//get binary of input of length "inputlength"
            if (arr4[0] == 'x' || arr4[0] == 'X' || arr4[0] == '#'){ //If not 4th argument of input is a register use this
                word[10] = '1';
                numberConverter(arr4,word,11,5, wp);//get binary of input of length "inputlength"
            } else { //If 4th argument of input is a register use this
                word[10] = '0'; word[11] = '0'; word[12] = '0';
                numberConverter(arr4,word,13,3, wp);//get binary of input of length "inputlength"
            }
            for (int i = 0; i < 16; ++i) {
                strarray[i] = word[i];
            }
            break;
        }



        case 241: //NOT
        {
            char word[16] = "1001";//first 4 chars is set to this
            numberConverter(arr2, word, 4,3, wp);//get binary of input of length "inputlength"
            numberConverter(arr3,word,7,3, wp);//get binary of input of length "inputlength"
            for (int i = 0; i < 6; ++i) {//last 6 chars is set to this
                word[15-i] = '1';
            }
            for (int i = 0; i < 16; ++i) {
                strarray[i] = word[i];
            }
            break;
        }


        case 231: //JMP
        {
            char word[16] = "1100000";//first 7 chars is set to this
            numberConverter(arr2,word,7,3, wp);//get binary of input of length "inputlength"
            for (int i = 0; i < 6; ++i) {//last 6 chars is set to this
                word[15-i] = '0';
            }
            for (int i = 0; i < 16; ++i) {
                strarray[i] = word[i];
            }
            break;
        }



        case 239: //JSR
        {
            if (arr1[0] == 'R' || arr1[0] == 'r') {break;}
            char word[16] = "01001";//first 5 chars is set to this
            numberConverter(arr2,word,5,11, wp);//get binary of input of length "inputlength"
            for (int i = 0; i < 16; ++i) {
                strarray[i] = word[i];
            }
            break;
        }



        case 321: //JSRR
        {
            char word[16] = "0100000";//first 7 chars is set to this
            numberConverter(arr2,word,7,3, wp);//get binary of input of length "inputlength"
            for (int i = 0; i < 6; ++i) {//last 6 chars is set to this
                word[15-i] = '0';
            }
            for (int i = 0; i < 16; ++i) {
                strarray[i] = word[i];
            }
            break;
        }



        case 235: //RET
        {
            char word[16] = "1100000111000000";//RET is always set to this
            for (int i = 0; i < 16; ++i) {
                strarray[i] = word[i];
            }
            break;
        }



        case 144: //LD
        {
            char word[16] = "0010";//first 4 chars is set to this
            numberConverter(arr2,word,4,3, wp);//get binary of input of length "inputlength"
            numberConverter(arr3,word,7,9, wp);//get binary of input of length "inputlength"
            for (int i = 0; i < 16; ++i) {
                strarray[i] = word[i];
            }
            break;
        }




        case 217: //LDI
        {
            char word[16] = "1010";//first 4 chars is set to this
            numberConverter(arr2,word,4,3, wp);//get binary of input of length "inputlength"
            numberConverter(arr3,word,7,9, wp);//get binary of input of length "inputlength"
            for (int i = 0; i < 16; ++i) {
                strarray[i] = word[i];
            }
            break;
        }





        case 226: //LDR
        {
            char word[16] = "0110";//first 4 chars is set to this
            numberConverter(arr2,word,4,3, wp);//get binary of input of length "inputlength"
            numberConverter(arr3,word,7,3, wp);//get binary of input of length "inputlength"
            numberConverter(arr4,word,10,6, wp);//get binary of input of length "inputlength"
            for (int i = 0; i < 16; ++i) {
                strarray[i] = word[i];
            }
            break;
        }





        case 210: //LEA
        {
            char word[16] = "1110";//first 4 chars is set to this
            numberConverter(arr2,word,4,3, wp);//get binary of input of length "inputlength"
            numberConverter(arr3,word,7,9, wp);//get binary of input of length "inputlength"
            for (int i = 0; i < 16; ++i) {
                strarray[i] = word[i];
            }
            break;
        }




        case 167: //ST
        {
            char word[16] = "0011";//first 4 chars is set to this
            numberConverter(arr2,word,4,3, wp);//get binary of input of length "inputlength"
            numberConverter(arr3,word,7,9, wp);//get binary of input of length "inputlength"
            for (int i = 0; i < 16; ++i) {
                strarray[i] = word[i];
            }
            break;
        }





        case 240: //STI
        {
            char word[16] = "1011";//first 4 chars is set to this
            numberConverter(arr2,word,4,3, wp);//get binary of input of length "inputlength"
            numberConverter(arr3,word,7,9, wp);//get binary of input of length "inputlength"
            for (int i = 0; i < 16; ++i) {
                strarray[i] = word[i];
            }
            break;
        }





        case 249: //STR
        {
            char word[16] = "0111";//first 4 chars is set to this
            numberConverter(arr2,word,4,3, wp);//get binary of input of length "inputlength"
            numberConverter(arr3,word,7,3, wp);//get binary of input of length "inputlength"
            numberConverter(arr4,word,10,6, wp);//get binary of input of length "inputlength"
            for (int i = 0; i < 16; ++i) {
                strarray[i] = word[i];
            }
            break;
        }




        case 311: //TRAP
        {
            char word[16] = "11110000";//first 8 chars is set to this
            numberConverter(arr2,word,8,8, wp);//get binary of input of length "inputlength"
            for (int i = 0; i < 16; ++i) {
                strarray[i] = word[i];
            }
            break;
        }
    }
}



/* This is used for .stringz to output every char as binary representation */
void stringzFunktion(char str[], char* strarray, int counter2, FILE* wp){
    int ascii =0; //ascii value to get binary from
    int a[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // make a new array to hold the binary numbers
    for (int i = 0; i < counter2 ; ++i) {
        ascii = str[i]; //get ascii value
        for (int l = 0; l < 16; ++l) {
            a[l] = 0; // make all 0s in a[]
        }

        for(int k=0; ascii>0;k++)
        {
            a[k]=ascii%2; //modulus 2 to get a 1 or 0
            ascii=ascii/2; // divide by 2 to get to next number in binary
        }
        for (int j = 0; j < 16; ++j) { //print all 16 binaries and write to file
            fprintf(wp,"%d", a[15-j]);
        }
        fprintf(wp,"\n");

    }
    for (int j = 0; j < 16; ++j) { //insert NULL terminator for string
        strarray[j] = '0';
    }
}



/* If we have a . instruction we use this function */
void dotInputDivider(char str[], char* strarray, FILE* wp){
    int counter=0, counter2=0;
    char str1[64] = "";
    while(str[counter] == ' '){counter++;} // if you entered a lot of spaces at start it will go to first real char

    while (str[counter] != ' ' && str[counter] != '\n' && str[counter] != '\0'){ // check if you hit a space after first input
        counter++;
    }
    counter++; // move to next char after space
    while(str[counter] == ' '){counter++;} // if you entered a lot of spaces at start it will go to first real char

    if (str[counter] == '"'){ //if we have a .stringz use this
        counter++;
        while(str[counter] != '\n' && str[counter] != '"'){ //checks if the char is a space or comma
            str1[counter2++] = str[counter++]; //inserts 2nd part of the input into a new array
        }
        stringzFunktion(str1, strarray, counter2, wp); //use stringz function to output binary
    }


    else {
        while(str[counter] != ' ' && str[counter] != ',' && str[counter] != '\n' && str[counter] != '\0'){ //checks if the char is a space or comma
            str1[counter2++] = str[counter++]; //inserts 2nd part of the input into a new array
        }

        if (str[1] == 'B' || str[1] == 'b'){ //if we have .BLKW use this
            double loopholder = CharConverter(str1); //get the second input as a double
            loopholder = loopholder / pow(10,16-counter2+1); //get the double to the right size
            for (int i = 0; i < loopholder-1; ++i) { //allocate memory space by 16 0s
                fprintf(wp,"0000000000000000\n");
            }
            for (int j = 0; j < 16; ++j) {
                strarray[j] = '0';
            }
        }

        else if (str[1] == 'E' || str[1] == 'e'){ //if we have .END use this
            for (int j = 0; j < 16; ++j) {
                strarray[j] = '\0'; //reset array
            }
        }

        else { //.FILL and .ORIG use this
            numberConverter(str1, strarray, 0, 16, wp); //convert char[] to binary
        }
    }
}



/* Split up the input into 4 arrays and check for labels and send to LC3 converter */
void inputDivider(char str[], char* strarray, FILE* wp, double currentaddress){
    int counter=0; //counter for whole input
    int counter2=0; //counter to input into multiple arrays
    char str1[MAX_LENGTH]=""; // First part of input
    char str2[MAX_LENGTH]=""; // Second part of input
    char str3[MAX_LENGTH]=""; // Third part of input
    char str4[MAX_LENGTH]=""; // Fourth part of input

    while(str[counter] == ' '){counter++;} // if you entered a lot of spaces at start it will go to first real char


    while (str[counter] != ' ' && str[counter] != '\n'){ // check if you hit a space after first input
        str1[counter] = str[counter]; // insert first input into a new char[]
        counter++;
    }
    counter++; // move to next char after space
    while(str[counter] == ' '){counter++;} // if you entered a lot of spaces at start it will go to first real char


    while(str[counter] != ' ' && str[counter] != ',' && str[counter] != '\n' && str[counter] != '\0'){ //checks if the char is a space or comma
        str2[counter2++] = str[counter++]; //inserts 2nd part of the input into a new array
    }
    ++counter; //goes to next char in input array
    while(str[counter] == ' '){counter++;} // if you entered a lot of spaces at start it will go to first real char
    counter2=0; //resets counter2

    while(str[counter] != ' ' && str[counter] != ',' && str[counter] != '\n' && str[counter] != '\0'){// checks if the char is space, comma or newline
        str3[counter2++] = str[counter++]; //inputs the third input into new char[] if there is one
    }
    ++counter; //goes to next char in input array
    while(str[counter] == ' '){counter++;} // if you entered a lot of spaces at start it will go to first real char
    counter2=0; //resets counter2


    while(str[counter] != ' ' && str[counter] != ',' && str[counter] != '\n' && str[counter] != '\0'){ // checks if the char is space, comma or newline
        str4[counter2++] = str[counter++]; //inputs the fourth input into new char[] if there is one
    }

    if (islabel(str2) == 1 && str2[0] != '\0'){ //if there is a label get the offset in decimal
        labelToOffsetConverter(str2,currentaddress);
    }
    if (islabel(str3) == 1 && str3[0] != '\0'){ //if there is a label get the offset in decimal
        labelToOffsetConverter(str3,currentaddress);
    }
    if (islabel(str4) == 1 && str4[0] != '\0'){ //if there is a label get the offset in decimal
        labelToOffsetConverter(str4,currentaddress);
    }

    LC3Converter(str1, counter - 1, str2, str3, str4,strarray, wp); //sends it to our converter function
}



/* creator for symbol table */
void createTable(char file[]){
    FILE *pointer; // pointer for file
    int counter = 0; // counter for words
    char firstWord[50] = ""; //char[] for first input in string
    char str[60] = ""; //char[] for input
    double currentAddress = 0; //current address to make labels
    labelCounter= 0; //how many labels there are

    pointer = fopen(file , "r"); //open file to be read

    while ((fgets(str,60,pointer)) != '\0') { //read whole file, but 1 line at a time
        for (int i = 0; i < counter ; ++i) {
            firstWord[i] = '\0'; //reset firstword array
        }
        counter = 0; //reset counter
        while(str[counter] == ' '){counter++;} //removes spaces from input
        for (int j = 0; j < 60; ++j) {
            if(str[counter] == '\0'){break;}
            str[j] = str[counter++];
        }

        counter=0; //reset counter again for when repeating while statement
        while (str[counter] != ' ' && str[counter] != '\n' && str[counter] != '\0'){
            firstWord[counter] = str[counter]; //get first part of input string
            counter++;
        }
        if (islabel(firstWord) == 1){ //check if it is a label
            labelCounter++; //one more label is in the program
        }
        for (int i = 0; i < 60 ; ++i) {
            str[i] = '\0'; //reset firstword array
        }
    }

    labelsPointer = (labels*) malloc(labelCounter * sizeof(labels)); //allocate space for labels on heap



    pointer = fopen(file , "r"); //open file to be read
    fgets(str,60,pointer); //get first line
    while(str[counter] != ' '){counter++;} //remove first part of input
    for (int i = 0; i < 5; ++i) {
        firstWord[i] = str[counter+i+1]; //get the hex number part (i.e. x3000)
    }

    startAddress = hextodecimal(firstWord); //use function to make it into a double
    currentAddress = startAddress; //set current address to this

    for (int i = 0; i < 50 ; ++i) {
        firstWord[i] = '\0'; //reset firstword array
    }


    counter = 0; //reset counter
    labelCounter = 0; //reset labelcounter
    pointer = fopen(file , "r"); //reset pointer
    while ((fgets(str,60,pointer)) != '\0') { //read whole file, but 1 line at a time
        for (int i = 0; i < counter ; ++i) {
            firstWord[i] = '\0'; //reset firstword array
        }
        counter = 0; //reset counter
        while(str[counter] == ' '){counter++;}//removes spaces from input
        for (int j = 0; j < 60; ++j) {
            if(str[counter] == '\0'){break;}
            str[j] = str[counter++];
        }

        counter=0; //reset counter again for when repeating while statement
        while (str[counter] != ' ' && str[counter] != '\n' && str[counter] != '\0'){
            firstWord[counter] = str[counter]; //get first part of input string
            counter++;
        }
        if (islabel(firstWord) == 1){ //check if it is a label
            (labelsPointer+labelCounter)->address=currentAddress; //make address for label
            for (int i = 0; i < counter; ++i) {//insert firstword name into labelname
                (labelsPointer+labelCounter)->labelname[i] = firstWord[i];
            }
            labelCounter++; //one more label is in the program
        }
        for (int i = 0; i < 60 ; ++i) {
            str[i] = '\0'; //reset firstword array
        }
        currentAddress++; //current address is +1 now
    }
    fclose(pointer); //close the pointer
}








/* main function */
int main () {
    FILE *fp, *wp;
    char str[60]="0",c[60]="0", firstword[60]="0", file[60]="0", tempfilename[5]="0";
    double currentAddress = 0; //current address for the labels
    int counter=0, firstwordcounter=0; //counters for second time we run the file

    struct dirent *de; //creates a pointer to the "current" directory
    DIR *dr = opendir("."); //opens a DIR pointer to all directories in project
    if (dr == NULL){
        printf("Cannot open directory!");
    }

    /* This finds the file in cmake-build-debug */
    while((de = readdir(dr)) != NULL) {
        int length = strlen(de->d_name) - 1;
        if (length < 5) {}
        else {
            for (int i = 0; i < 4; ++i) {
                tempfilename[3-i] = de->d_name[length-i]; //get last 4 chars of temporary filename
            }
            tempfilename[4] = '\0'; //have to do this otherwise ending is random string of chars
            if(strcmp(tempfilename,".asm") == 0){ //compare with ending of .asm
                for (int i = 0; i < length+1; ++i) {
                    file[i] = de->d_name[i];
                }
            }
        }
    }

    fp = fopen(file , "r");
    if(fp == NULL) { //check if file exists
        printf("There is no file ending with ["BOLDRED".asm"RESET"]");
        perror("");
        return(-1);
    }


    createTable(file); //make symbol table

    printf("\tSymbol Table\n");
    for (int i = 0; i < labelCounter; ++i) {
        printf("Offset:%.0lf\tName:%s\n",(labelsPointer + i) ->address-startAddress,(labelsPointer + i) ->labelname); //output symbol table
    }

    /* opening file for reading */
    fp = fopen(file , "r");
    wp = fopen("machinecode.obj","w");

    currentAddress = startAddress; // the current address is the global startaddress from start
    while ((fgets(str,60,fp)) != '\0') { //get whole file one line at a time
        counter=0; //reset counters
        firstwordcounter=0;
        for (int i = 0; i < 60; ++i) {
            firstword[i] = '\0'; //reset firstword array
        }
        while(str[counter] == ' '){counter++;} //check for spaces
        while(str[counter] != ' ' && str[counter] != '\n' && str[counter] != '\0'){
            firstword[firstwordcounter++] = str[counter++]; // get first part of input
        }
        counter=0; //reset counter
        if (islabel(firstword)){ //check if it is a label
            firstwordcounter++; //1 up the array if there is a label
            while(str[firstwordcounter] != '\n' && str[firstwordcounter] != '\0') { //if it is a label remove from str
                str[counter++] = str[firstwordcounter];
                str[firstwordcounter++] = '\0';
            }
        }

        counter=0; //reset counter
        while(str[counter] == ' '){counter++;} //check for spaces
        if(counter != 0) {
            for (int j = 0; j < 60; ++j) {
                if (str[counter] == '\0') { break; }
                str[j] = str[counter++];
            }
        }

        if (str[0] == '.'){
            dotInputDivider(str,c, wp); //use our function if there is a . on first char
        }
        else {
            inputDivider(str, c, wp,currentAddress); //use function to get binary representation of not . instructions
        }


        if(c[0] != '\0'){
            fprintf(wp,"%s\n", c); //write our binary representation to the file
        }
        currentAddress++; //address is 1 higher now
    }
    fclose(fp);//close pointers
    fclose(wp);
    free(labelsPointer); //free the labelspointer from malloc() call
    return(0); //end program
}