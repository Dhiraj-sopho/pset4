#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
typedef uint8_t  BYTE;
int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./copy infile outfile\n");
        return 1;
    }
    FILE *inptr = fopen(argv[1], "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", argv[1]);
        return 2;
    }//till here we have opened the raw image
    FILE *img[50];
    img[0]=fopen("000.jpg","w");
    int n=0;
    BYTE buffer[512];
    while(n<2)
    {
        fread(buffer,1,512,inptr);
        if(buffer[0]==0xff&&buffer[1]==0xd8&&buffer[2]==0xff&&(buffer[3]&0xf0)==0xe0)//this is working fine
        {
            n++;
        }
        if(n!=0)
        {
            fwrite(buffer,1,512,img[0]);
        }
    }
    int eof=512;
    fclose(img[0]);
    img[1]=fopen("001.jpg","w");
    fwrite(buffer,1,512,img[1]);
    n=1;
    while(eof==512)
    {
        eof=fread(buffer,1,512,inptr);
        if(buffer[0]==0xff&&buffer[1]==0xd8&&buffer[2]==0xff&&(buffer[3]&0xf0)==0xe0)//this is working fine
        {
            char filename[7];
            n++;
            sprintf(filename,"%03i.jpg",n);
            
                fclose(img[n-1]);
                img[n]=fopen(filename,"w");
        }
    
        
        fwrite(buffer,1,512,img[n]);
    }
    fclose(img[n]);
    fclose(inptr);
    return 0;
}
