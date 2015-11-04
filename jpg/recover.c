/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Recovers JPEGs from a forensic image.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint8_t BYTE;

int main(int argc, char* argv[])
{
    FILE* file = fopen("card.raw", "r");
    if (file == NULL)
    {
        printf("Could not open card.raw");
        return 3;
    }
    
    FILE *ourImage = NULL;
    int images = 0;
    
    while(!feof(file))
    {
        BYTE buffer[512];
        char jpgName[8];
        
        if ((buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && buffer[3] == 0xe0) 
        || (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && buffer[3] == 0xe1))
        {
            if (ourImage != NULL)
            {
                fclose(ourImage);
            } 
            
            sprintf(jpgName, "%03d.jpg", images);
            ourImage = fopen(jpgName, "w");
            fwrite(buffer, sizeof(buffer), 1, ourImage);
            images++;
        }
        else
        {
            if (images > 0)
            {
                fwrite(buffer, sizeof(buffer), 1, ourImage);
            }
        }
        fread(buffer, sizeof(buffer), 1, file);
    }
    if (images > 0)
    {
        fclose (ourImage);
    }
    
    fclose(file);
    
    return 0;
}

