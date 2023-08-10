#include <stdlib.h>
#include <stdio.h>
#include<stdint.h>
#include<math.h>


typedef struct {
  char Type[2];
} BMPSignature;

typedef struct {
  uint32_t FileSize;
  uint32_t Reserved;
  uint32_t OffBits;
} BMPFileHeader;

typedef struct {
  uint32_t HeaderSize;
  uint32_t Width;
  uint32_t Height;
  uint16_t Planes;
  uint16_t BPP;
  uint32_t Compression;
  uint32_t ImageSize;
  uint32_t XPPM;
  uint32_t YPPM;
  uint32_t ClrUsed;
  uint32_t ClrImportant;
} BMPImageHeader;
int p[512];
double fade(double t) { return t * t * t * (t * (t * 6 - 15) + 10); }
double lerp(double t, double a, double b) { return a + t * (b - a); }
double grad(int hash, double x, double y, double z) {
      int h = hash & 15;                      
      double u = h<8 ? x : y,                 
             v = h<4 ? y : h==12||h==14 ? x : z;
      return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
   }
   
double noise(double x, double y, double z) {
      int X = (int)floor(x) & 255,                  
          Y = (int)floor(y) & 255,                  
          Z = (int)floor(z) & 255;
      x -= floor(x);                                
      y -= floor(y);                                
      z -= floor(z);
      double u = fade(x),                                
             v = fade(y),                                
             w = fade(z);
      int A = p[X]+Y, AA = p[A]+Z, AB = p[A+1]+Z,      
          B = p[X+1]+Y, BA = p[B]+Z, BB = p[B+1]+Z;   
 
      return lerp(w, lerp(v, lerp(u, grad(p[AA  ], x  , y  , z   ), 
                                     grad(p[BA  ], x-1, y  , z   )),
                             lerp(u, grad(p[AB  ], x  , y-1, z   ), 
                                     grad(p[BB  ], x-1, y-1, z   ))),
                     lerp(v, lerp(u, grad(p[AA+1], x  , y  , z-1 ), 
                                     grad(p[BA+1], x-1, y  , z-1 )), 
                             lerp(u, grad(p[AB+1], x  , y-1, z-1 ),
                                     grad(p[BB+1], x-1, y-1, z-1 ))));
   }
   void loadPermutation(char* fileName){
	FILE* fp = fopen(fileName,"r");
	int permutation[256],i;
	
	for(i=0;i<256;i++)
		fscanf(fp,"%d",&permutation[i]);
	
	fclose(fp);
	
	for (int i=0; i < 256 ; i++) p[256+i] = p[i] = permutation[i];
}

void createImage(const char* name, uint32_t ImageWidth, uint32_t ImageHeight) {

  // init structs
  BMPSignature BMP_S;
  BMPFileHeader BMP_FH;
  BMPImageHeader BMP_IH;

  BMP_S.Type[0] = 'B';
  BMP_S.Type[1] = 'M';

  // calculate padding
  uint8_t padding = (ImageWidth * 3) % 4;

  BMP_FH.FileSize = (ImageWidth * 3 + padding) * ImageHeight + 54;
  BMP_FH.Reserved = 0;
  BMP_FH.OffBits = 54;

  BMP_IH.HeaderSize = 40;
  BMP_IH.Width = ImageWidth;
  BMP_IH.Height = ImageHeight;
  BMP_IH.Planes = 1;
  BMP_IH.BPP = 24;
  BMP_IH.Compression = 0;
  BMP_IH.ImageSize = 0;
  BMP_IH.XPPM = 0;
  BMP_IH.YPPM = 0;
  BMP_IH.ClrUsed = 0;
  BMP_IH.ClrImportant = 0;
  loadPermutation("PerlinData.txt");

  // create File
  FILE* nFile;
  nFile = fopen(name, "wb");

  fwrite(&BMP_S, sizeof(BMP_S), 1, nFile);
  fwrite(&BMP_FH, sizeof(BMP_FH), 1, nFile);
  fwrite(&BMP_IH, sizeof(BMP_IH), 1, nFile);

  // color values
  uint8_t red;
  uint8_t green;
  uint8_t blue;

  // painting
  for(uint32_t y = 0; y < ImageHeight; y++) {
    for(uint32_t x = 0; x < ImageWidth; x++) {
      red = 137;
      blue = (50 * noise(x/ImageWidth,(double)y/ImageHeight,rand()))+50;
      green =(50 * noise(x/ImageWidth,(double)y/ImageHeight,rand()))+50 ;

      fputc(blue, nFile);
      fputc(green, nFile);
      fputc(red, nFile);
    }
    if(padding) {
      for(uint8_t i = 0; i < padding; i++) {
        fputc(0, nFile);
      }
    }
  }
  // close File
  fclose(nFile);
}