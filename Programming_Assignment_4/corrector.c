#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>
#include <fcntl.h>
#include <dirent.h>

char** GetGenderAndSurname(FILE* databaseptr,char nameInF[]){
    char gender[5],name[80],surname[80];
    char** buffer = malloc(2 * sizeof(char*));
    buffer[0] = malloc(20 * sizeof(char));
    buffer[1] = malloc(80 * sizeof(char));

    strcpy(buffer[0],"-1");

    while (fscanf(databaseptr,"%s %s %s", gender,name ,surname) == 3)
    {
        if (!strcmp(nameInF,name))
        {
            strcpy(buffer[0],gender);
            strcpy(buffer[1],surname);
            break;
        }
        
    }
    fseek(databaseptr,0,SEEK_SET);
    return buffer;
} 
void CorrectTxt(FILE* fileptr,FILE* databaseptr){
    char  word[100];
    while (fscanf(fileptr,"%s",word) == 1)
    {
        char tempTitle[10], tempSurname[100];
        char ** GenderName = GetGenderAndSurname(databaseptr,word);
        

        if (strcmp(GenderName[0],"-1") != 0)
        {
            fseek(fileptr,ftell(fileptr) - strlen(word) - 4,SEEK_SET);

            fscanf(fileptr,"%s",tempTitle);
            if (strcmp(GenderName[0],"m") == 0 && strcmp(tempTitle,"Mr.") != 0)
            {
              fseek(fileptr,ftell(fileptr) - strlen(tempTitle),SEEK_SET);
              fputs("Mr.",fileptr);  
            } else if(strcmp(GenderName[0],"f") == 0 && strcmp(tempTitle,"Ms.") != 0)
            {
                fseek(fileptr,ftell(fileptr) - strlen(tempTitle),SEEK_SET);
                fputs("Ms.",fileptr); 
            }
            fseek(fileptr, 2 + strlen(word),SEEK_CUR);

            fscanf(fileptr,"%s",tempSurname);
            if(strcmp(GenderName[1],tempSurname) != 0){
                fseek(fileptr,ftell(fileptr) - strlen(tempSurname),SEEK_SET);
                fputs(GenderName[1],fileptr); 
            }
            
        }
        
        
        free(GenderName[0]);
        free(GenderName[1]);
        free(GenderName);
    }
}

void traverseFiles(FILE* databaseptr,char basePath[])
{
    char path[800];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    
    if (!dir){
        return;
    }
    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, "..") != 0 && strcmp(dp->d_name, ".") != 0)
        {
            
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);

            

            unsigned int len = strlen(dp->d_name);
            for (int i = 0; i < len; i++)
            {
                // Checks if filename has .txt extension
                if (i + 4 == len && dp->d_name[i] == '.' && dp->d_name[i+1] == 't' && dp->d_name[i+2] == 'x' && dp->d_name[i+3] == 't')
                {
                    // checks if it is database.txt in main directory.
                    if (strcmp(path,"./database.txt") != 0)
                    {
                        FILE* fileptr = fopen(path,"r+");
                        CorrectTxt(fileptr,databaseptr);
                        
                        fsync(fileno(fileptr));
                        fclose(fileptr);
                        
                    }
                    break;
                }
                
            }

            traverseFiles(databaseptr,path);
        }
    }

    closedir(dir);
}



int main(int argc, char *argv[]){
    

    FILE* databaseptr = fopen("./database.txt","r");
    
    
    

    
    traverseFiles(databaseptr,".");

    fclose(databaseptr);
    return 0;
}