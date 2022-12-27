#include <stdio.h>
#include <string.h>

//DECLARING FUNCTIONS
char* pickArray(char c);
int search(char *arg);
int mutualNums(char name[], char num[], char *arg, int size);
int verifyInput(char *arg);
int atoi(char *arg);
int fullList();

//MAIN FUNCTION 
int main(int argc, char *argv[]){
    
    char *arg;
    static char name[101];
    static char number[101];
    int inputValidation;
     
    if(argc > 2){

        printf("Too many arguments inserted\n");
        return 1;
     }
    
    if(argc == 1){

    //FUNCTION THAT PRINTS WHOLE LIST WHEN NO NUMBERS ARE INSERTED
        for(int i = 0; i < 42; i++){
        
            fgets(name, 101, stdin);

            name[strcspn(name, "\r\n")] = 0;

            fgets(number, 101, stdin);

            printf("%s, %s", name, number);
         
            if(feof(stdin)){
                break;
            }

        }

        return 0;
    }

    //FUNCTION THAT CHECKS IF USER INPUT IS VALID, AND IF SO, IT STARTS TO SEARCH
      if(!argc || !argv[0]) return 0; 

            while((arg = *(++argv))){
                
                inputValidation = verifyInput(arg);
           
                if(inputValidation == 1){
                
                search(arg);
    
           }else
           printf("Invalid input\n");
    }

    return 0;
}

//FUNCTION THAT SEARCHES THROUGH CONTACT LIST AND IF FINDS MATCH, PRINTS FOUNDED CONTACT
int search(char *arg){
    
    static char name[101];
    static char number[101];
    int res;
    int size = strlen(arg);
    int foundContacs = 0;
   
    for(int j = 0; j < 42; j++){

        //GETTING NAME AND NUMBER OF CONTACT
        fgets(name, 101, stdin);
        
        name[strcspn(name, "\r\n")] = 0;
        
        fgets(number, 101, stdin);

        //COMPARING NAME AND NUMBER WITH INSERTED NUMBERS
        res = mutualNums(name, number, arg, size);
        
        if(res == size){
            printf("%s, %s", name, number);
            foundContacs++;
        }
        
        if(feof(stdin)){
            break;
        }
        
    }
    
    if(foundContacs == 0){
            printf("Not found\n");
        }

    return 0;   
}

//FUNCTION THAT SEARCHES THROUGH EXACT CONTACT (NAME AND NUMBER)
int mutualNums(char name[], char num[], char *arg, int size){

    char *str;
    char *nameRes;
    char *numRes;
    int mutual = 0;

    for(int i = 0; i < size; i++){

        str = pickArray(arg[i]);
        nameRes = strchr(str, name[i]);
        numRes = strchr(str, num[i]);

        if(nameRes != NULL || numRes != NULL)
            mutual++;
    }

    return mutual;
}

/*FUNCTION THAT GIVES NUMBER INSERTED BY USER ITS EXACT ARRAY OF CHARACTERS,
  WHICH NUMBER REPRESENTS*/
char* pickArray(char c){

 static char searchNum[10];

        if(c == '1'){
                strcpy(searchNum, "1");  
        }else if(c == '2'){
                 strcpy(searchNum, "2AaBbCc");  
        }else if(c == '3'){
                 strcpy(searchNum, "3DdEeFf");  
        }else if(c == '4'){
                 strcpy(searchNum, "4GgHhIi");  
        }else if(c == '5'){
                 strcpy(searchNum, "5JjKkLl");  
        }else if(c == '6'){
                 strcpy(searchNum, "6MmNnOo");  
        }else if(c == '7'){
                 strcpy(searchNum, "7PpQqRrSs");  
        }else if(c == '8'){
                 strcpy(searchNum, "8TtUuVv");  
        }else if(c == '9'){
                 strcpy(searchNum, "9WwXxYyZz");  
        }else if(c == '0'){
                strcpy(searchNum, "0+");
        }
                
        return searchNum;
}

//FUNCTION THAT VERIFIES, THAT USER INSERTED VALID INPUT (NUMBERS)
int verifyInput(char *arg){
    
    int num = atoi(arg);
    int size = sizeof(arg);

      for(int i = 0; i < size; i++){
           
            if(arg[i] == 48 && num == 0){
            return 1;

            }else if(num == 0){ 
            return 0;

            }else
            return 1;

           }
    
    return 0;
}