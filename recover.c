#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 512

int main(int argc, char *argv[])
{
    //ENSURE PROPER USAGE
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./recover image\n");
        return 1;
    }

    char *infile = argv[1];

    //OPEN INPUT FILE
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    //DECLARATIONS
    unsigned char buffer[BUFFER_SIZE];

    int count = 0;
    int found = 0;

    FILE *img = NULL;

    //START TO ITERATE IN FILE
    while (fread(buffer, BUFFER_SIZE, 1, inptr) == 1)
    {
        if (buffer[0] == 0xff &&
            buffer[1] == 0xd8 &&
            buffer[2] == 0xff &&
            (buffer[3] & 0xf0) == 0xe0)
        {
            //FOUND NEW PIC SO CLOSE OLD ONE IF EXIST
            if (found == 1)
            {
                fclose(img);
            }
            else
            {
                found = 1;
            }

            char image[8];
            sprintf(image, "%03i.jpg", count);
            img = fopen(image, "w");
            count++;
        }
        if (found == 1)
        {
            fwrite(buffer, BUFFER_SIZE, 1, img);
        }
    }

    fclose(inptr);
    fclose(img);

    return 0;
}