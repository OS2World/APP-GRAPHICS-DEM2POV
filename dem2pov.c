/*        DEM2POV.C  v1.2a
	  convert dem to tga height-field for POV.

	  code modified by U. A. Mueller, 15 Nov. 97.

	  code modified by W. D. Kirby, 10 Aug. 96.
	  hacked from dem2xyz.c
	  converts 3 arc second to lat/long,  sol katz, mar. 94.
	  added sampling and cutting to size, sol katz, apr 94.
	  changed the calculation of start position in sampling code,
	  sol katz, jan 95.

	  Persistence of Vision (POV) raytracer can use a height-field
	  defined in a Targa (.TGA) image file format where the RGB pixel values
	  are 24 bits (3 bytes). A 16 bit unsigned integer height-field value
	  is assigned as follows:

		   Red     high byte
		   Green   low byte
		   Blue    empty
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void getheader (FILE *);
void processProfiles (FILE *, FILE *);
void writeFile (FILE *, double);
void writeCount (void);
void writeHeader (FILE *);

#define YMAX     2048          /*  max length of an NCIC scan line */
#define XMAX     2048          /*  max # of NCIC scan lines */
#define MIN(x,y)     (((x) < (y)) ? (x) : (y))
#define MAX(x,y)     (((x) > (y)) ? (x) : (y))
#define SW     0
#define NW     1
#define NE     2
#define SE     3

#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif

int base[XMAX],           /* array of base elevations */
    image[YMAX],          /* array for height-field */
    width,                /* height-field image width and height */
    height,
    hfType,               /* height-field type */
    DEMlevel,
    elevationPattern,
    groundSystem,
    groundZone,
    planeUnitOfMeasure,
    elevUnitOfMeasure,
    polygonSizes,
    accuracyCode,
    profileDimension[2],
    firstRow,
    lastRow,
    wcount = 0,
    hcount,
    eastMostSample,
    westMostSample,
    southMostSample,
    northMostSample,
    rowCount,
    columnCount,
    r,
    c,
    rowStr,
    rowEnd,
    colStr,
    colEnd,
    colInt=1,
    rowInt=1,
    outType=0;
double defaultElev = 0.0,      /* elevation for empty points */
       hfBias,
       hfNorm = 1.0,
       verticalScale=1.0,    /* to stretch or shrink elevations */
       minValue=50000.0,
       maxValue=-50000.0,
       projectParams[15],
       groundCoords[4][2],
       elevBounds[2],
       localRotation,
       spatialResolution[3],
       deltaY,
       eastMost,
       westMost,
       southMost,
       northMost;
char   mapLabel[145],
       inText[24],
       **junk;
long int cellCount = 0 ;

int main (int argc, char *argv[])
{
   FILE *inf, *outf;

   if (argc < 2)
   {
      fprintf (stderr, "DEM to POV height-field v1.3 by U. A. Mueller 11/15/97 \n");
      fprintf (stderr, "  based on v1.2 by W. D. Kirby 1/30/96 \n");
      fprintf (stderr, "  based on DEM to x,y,z ascii file, ver 3.1, 1-95 -- Sol Katz, BLM \n\n");
      fprintf (stderr, "Usage: dem2pov dem_name [tga_file]\n");
      fprintf (stderr, "            dem_name only for header info\n");
      exit (1);
   }

   if ((inf = fopen (argv[1], "r")) == NULL)
   {
      perror (argv[1]);
      exit (1);
   }

   fprintf (stderr, "DEM to POV height-field v1.3 by U. A. Mueller 11/15/97 \n");
   fprintf (stderr, "  based on v1.2 by W. D. Kirby 1/30/96 \n");
   fprintf (stderr, "  based on DEM to x,y,z ascii file, ver 3.1, 1-95 -- Sol Katz, BLM \n");
   getheader (inf);
   deltaY = spatialResolution[0] == 3.0 ? spatialResolution[1] / 3600.0 : spatialResolution[1] ;
   eastMost  = MAX (groundCoords[NE][0], groundCoords[SE][0]);
   westMost  = MIN (groundCoords[NW][0], groundCoords[SW][0]);
   northMost = MAX (groundCoords[NE][1], groundCoords[NW][1]);
   southMost = MIN (groundCoords[SW][1], groundCoords[SE][1]);
   eastMostSample =  ((int) (eastMost / spatialResolution[0])) * (int) spatialResolution[0];
   westMostSample =  ((int) (westMost / spatialResolution[0])) * (int) spatialResolution[0] ;
   northMostSample = ((int) (northMost / spatialResolution[1])) * (int) spatialResolution[1] ;
   southMostSample = ((int) (southMost / spatialResolution[1])) * (int) spatialResolution[1] ;

   if (spatialResolution[0] == 3.0)
   { /* then it's a 1x1 degree DEM */
      printf("eastMostSample  %10f, westMostSample  %10f\n", eastMostSample / 3600.0, westMostSample / 3600.0);
      printf("northMostSample %10f, southMostSample %10f\n", northMostSample / 3600.0, southMostSample / 3600.0);
   }

   columnCount = (eastMostSample  - westMostSample) / (int) spatialResolution[0] + 1;
   rowCount    = (northMostSample - southMostSample) / (int) spatialResolution[1] + 1;

   if (columnCount != profileDimension[1])
   {
      printf ("       CALCULATED column Count %d != HEADER %d \n", columnCount, profileDimension[1]);
      printf ("       will use SMALLER column Count \n");
      columnCount =MIN ( profileDimension[1], columnCount);
   }

   printf ("number of rows %d, number of columns %d\n", rowCount, columnCount);

   if (argc > 2)
   {
      if ((outf = fopen(argv[2], "wb")) == NULL)
      {
        fprintf (stderr, "can't open output file %s\n", argv[2]);
	exit (1);
      }
   }
   else
     exit(0);

   /* Interactive option entries */
   colInt = rowInt = 1 ;
   fprintf (stderr,"Enter 0 for all,  1 for samples,  2 for subset : ");
   scanf ("%d",&outType);
   /* default height-field image width and height */
   width  = rowCount;
   height = columnCount;

   if (outType == 1)   /* sample DEM file */
   {
      fprintf (stderr,"Enter column sample interval :");
      scanf ("%d", &colInt);
      fprintf (stderr,"Enter row    sample interval :");
      scanf ("%d", &rowInt);
      /* adjust deltaY */
      deltaY = deltaY * rowInt;
      /* image size */
      width  = rowCount / rowInt;
      height = columnCount / colInt;
   }
   else if (outType == 2)  /* output subset of DEM file */
   {
      fprintf (stderr,"Enter start column  :");
      scanf ("%d", &colStr);
      fprintf (stderr,"Enter end   column  :");
      scanf ("%d", &colEnd);
      fprintf (stderr,"Enter start row     :");
      scanf ("%d", &rowStr);
      fprintf (stderr,"Enter end   row     :");
      scanf ("%d", &rowEnd);
      /* image size */
      width  = rowEnd - rowStr + 1;
      height = colEnd - colStr + 1;
      rowStr--;   /* correct for 0 start index */
      colStr--;
   }

   fprintf (stderr,"Height-field type (0) Actual heights (1) Normalized : ");
   scanf ("%i",&hfType);

   /* For actual height-field values only */
   if (hfType == 0)
   {
     fprintf (stderr,"Enter a vertical scaling factor : ");
     scanf ("%lf", &verticalScale);
   }

   /* Since output must be unsigned */
   if (elevBounds[0] < 0.0)
      fprintf (stderr, " Warning negative elevation values in input \n");

   fprintf (stderr, "Enter elevation bias: ");
   scanf ("%lf", &hfBias);
   fprintf (stderr, "Enter default elevation (final output units) : ");
   scanf ("%lf", &defaultElev);

   if (defaultElev < 0.0)
   {
      fprintf (stderr, "\nDefualt elevation must be => 0 \n");
      exit (1);
   }

   fprintf (stderr, "\n");

   /* Normalize using max value of unsigned short integer (16 bit) */
   if (hfType == 1)
     hfNorm = 65535.0 / (elevBounds[1] + hfBias);

   /* adjust the end of the file */
   if (outType == 2 && columnCount > colEnd)
     columnCount = colEnd;

   /* write TGA image file header */
   writeHeader (outf);
   /* have to read everything */
   processProfiles (inf, outf);
   fprintf (stderr, "\n  %ld pixels were written to the file. \n", cellCount);
   fprintf (stderr, " For converted data points: \n");
   fprintf (stderr, "      MinValue = %10.3f MaxValue = %10.3f \n", minValue, maxValue);
   return (0);
}

/* read profiles */
void processProfiles (FILE *inputFile, FILE *outputFile)
{
   int    c, r, mod, count, tempInt, lastProfile = 0, profileID[2], profileSize[2];
   double planCoords[2], elevExtremea[2];

   for (c = 1; c <= columnCount; c++)
   {
      count = fscanf (inputFile, "%6d%6d%6d%6d", &profileID[0], &profileID[1], &profileSize[0], &profileSize[1]);

      if (count != 4)
      {
        fprintf (stderr, "\nshort read of %d on column %d\n", count, c);
        fprintf (stderr, "\n  %ld pixels were written to the file. \n", cellCount);
        exit (1);
      }

      fscanf (inputFile, "%24c", inText);
      inText[20] = 'E';
      planCoords[0]   = strtod (inText, junk);
      fscanf (inputFile, "%24c", inText);
      inText[20] = 'E';
      planCoords[1]   = strtod (inText, junk);
      fscanf (inputFile, "%24c", inText);
      inText[20] = 'E';
      fscanf (inputFile, "%24c", inText);
      inText[20] = 'E';
      elevExtremea[0] = strtod (inText, junk);
      fscanf (inputFile, "%24c", inText);
      inText[20] = 'E';
      elevExtremea[1] = strtod (inText, junk);

      /* next 2 lines are a kludge to force the end of processing */
      if (profileID[1]-1 != lastProfile)
      {
	 fprintf (stderr, "\n  %ld pixels were written to the file. \n", cellCount);
	 exit (0);
      }

      lastProfile = profileID[1];
      firstRow = ((int) (planCoords[1] - southMostSample)) / (int) spatialResolution[1];
      lastRow = firstRow + profileSize[0] - 1;

      for (r = 0 ; r < firstRow; r++)
        base[r] = defaultElev;

      /* read in all the data for this column */
      for (r = firstRow; r <= lastRow; r++)
      {
	 count = fscanf (inputFile, "%6d", &tempInt);
	 base[r] = tempInt;
      }

      /* if cutting out a section, adjust the rows */
      if (outType == 2)
      {
	 if (firstRow < rowStr)
	 {
	    firstRow = rowStr;
	    planCoords[1] = planCoords[1] + (firstRow - 1) * (deltaY);
	 }

	 if (lastRow > rowEnd)
           lastRow = rowEnd;
      }

      mod = c % colInt;

      if (mod == 0 && c >= colStr)
	 writeFile (outputFile, planCoords[1]);
   }
}

/* write data to height-field file*/
void writeFile (FILE *outputFile, double YCoord)
{
   int     r, i, elev_mod, elev_div;
   double  tempFloat;

   hcount = 0;

   /* fill empty cells */
   if (firstRow > rowStr)
   {
      elev_mod = (int)defaultElev % 256;
      elev_div = (int)defaultElev / 256;

      /* write leading empty pixels this DEM column as default elevation */
      for (r = rowStr; r < firstRow; r += rowInt)
      {
	 putc ((char)0, outputFile);                        /* Blue  empty     */
	 putc ((char)(elev_mod), outputFile); /* Green low byte  */
	 putc ((char)(elev_div), outputFile); /* Red   high byte */
	 cellCount++ ;
	 hcount++;
      }
   }

   for (r = firstRow; r <= lastRow; r += rowInt)
   {
      tempFloat = ((float)base[r] * verticalScale) + hfBias;      /* now, scale the raw value */
      tempFloat *= hfNorm;      /* Normalize */

      if (tempFloat < 0.0)
        tempFloat = 0.0;

      if (tempFloat > 65535.0)
        tempFloat = 65535.0;

      if (tempFloat > maxValue)
        maxValue = tempFloat;

      if (tempFloat < minValue)
        minValue = tempFloat;

      /* write pixel */
      putc ((char)0, outputFile);                      /* Blue  empty     */
      putc ((char)((int)tempFloat % 256), outputFile); /* Green low byte  */
      putc ((char)((int)tempFloat / 256), outputFile); /* Red   high byte */
      cellCount++ ;
      /*    move up the delta y distance */
      YCoord += deltaY;
      hcount++;
   }

   /* write remaining pixels this column as default elevation */
   if (hcount < width)
   {
      for (i = hcount; i < width; ++i)
      {
	 putc ((char)0, outputFile);                        /* Blue  empty     */
	 putc ((char)((int)defaultElev % 256), outputFile); /* Green low byte  */
	 putc ((char)((int)defaultElev / 256), outputFile); /* Red   high byte */
	 cellCount++ ;
      }
   }

   wcount++;
}

/* Ref: USGS Digital Elevation Models - Data Users Guide 5, 1993 */
/* Operates on old and new DEM header format.                    */
/* However, new header format extention items are skipped over.  */
void getheader (FILE *inputFile)
{
   int k, l, profileID[2];

   fscanf (inputFile, "%144c%6i%6i%6i%6i", mapLabel, &DEMlevel, &elevationPattern, &groundSystem, &groundZone);

   for (k=0; k<15; k++)
   {
      fscanf (inputFile,"%24c", inText);    /* 7 - Map projection parameters */
      inText[20] = 'E';
      projectParams[k]  = strtod (inText,junk);
   }

   fscanf (inputFile,"%6i%6i%6i", &planeUnitOfMeasure, &elevUnitOfMeasure, &polygonSizes);

   for (k = 0; k < 4 ; k++)
   { /* SW, NW, NE, SE corners */
      for (l = 0; l < 2 ; l++)
      { /* eastings then northings */
	fscanf (inputFile,"%24c", inText);
	inText[20] = 'E';
	groundCoords[k][l] = strtod (inText,junk);
      }
   }

   fscanf (inputFile, "%24c", inText);
   inText[20] = 'E';
   elevBounds[0] = strtod(inText, junk);
   fscanf (inputFile, "%24c", inText);
   inText[20] = 'E';
   elevBounds[1] = strtod(inText, junk);
   fscanf (inputFile, "%24c", inText);
   inText[20] = 'E';
   localRotation  = strtod(inText, junk);
   fscanf (inputFile, "%6c", inText);
   inText[6] = '\0';
   accuracyCode = atoi(inText);
   inText[12] = '\0';    /* terminate string buffer */
   fscanf (inputFile, "%12c", inText);
   spatialResolution[0] = (double)atof(inText);
   fscanf (inputFile, "%12c", inText);
   spatialResolution[1] = (double)atof(inText);
   fscanf (inputFile, "%12c", inText);
   spatialResolution[2] = (double)atof(inText);
   fscanf (inputFile, "%6d", &profileDimension[0]);
   fscanf (inputFile, "%6d", &profileDimension[1]);
   /* Test for column and row data start */
   fscanf (inputFile, "%6d%6d", &profileID[0], &profileID[1]);

   if ((profileID[0]!=1) || (profileID[1]!=1))
   {
      fseek (inputFile, 1024, 0);  /* new format */
      printf ("DEM Header Format: New \n");
   }
   else
   {
     fseek (inputFile, 864, 0); /* old format */
     printf ("DEM Header Format: Old \n");;
   }

   printf ("[1&2] Map label: %s\n", mapLabel);
   printf ("[ 3] DEM level:  %i\n", DEMlevel );
   printf ("[ 4] elevation pattern: %i  ", elevationPattern);

   switch (elevationPattern)
   {
     case 1: {
               printf ("(=>regular) \n"); break;
             }
     case 2: {
               printf ("(=>random ) \n"); break;
             }
     default:{
               printf ("(=>?%i?   ) \n", elevationPattern);
             }
   }

   printf("[ 5] planimetric reference system: %i ", groundSystem);

   switch (groundSystem)
   {
     case 0: {
               printf("(=>geographic) \n"); break;
             }
     case 1: {
               printf("(=>UTM        ) \n"); break;
             }
     case 2: {
               printf("(=>State Plane) \n"); break;
             }
     default:{
               printf("(=>?%i?       ) \n", groundSystem);
             }
   }

   printf ("[ 6] ground zone: %i\n", groundZone);
   printf ("[ 7] proj: %15.7f %15.7f %15.7f\n", projectParams[0], projectParams[1], projectParams[2]);
   printf ("[ 7] proj: %15.7f %15.7f %15.7f\n", projectParams[3], projectParams[4], projectParams[5]);
   printf ("[ 7] proj: %15.7f %15.7f %15.7f\n", projectParams[6], projectParams[7], projectParams[8]);
   printf ("[ 7] proj: %15.7f %15.7f %15.7f\n", projectParams[9], projectParams[10], projectParams[11]);
   printf ("[ 7] proj: %15.7f %15.7f %15.7f\n", projectParams[12], projectParams[13], projectParams[14]);
   printf ("[ 8] plane unit of measure: %i ",planeUnitOfMeasure );

   switch (planeUnitOfMeasure)
   {
     case 0: {
               printf ("(=>radians )\n"); break;
             }
     case 1: {
               printf ("(=>feet    )\n"); break;
             }
     case 2: {
               printf ("(=>meters  )\n"); break;
             }
     case 3: {
               printf ("(=>arc-sec )\n"); break;
             }
     default:{
               printf ("(=>?%d?    )\n", planeUnitOfMeasure);
             }
   }

   printf ("[ 9] elevation measurement units code: %i ", elevUnitOfMeasure);

   switch (elevUnitOfMeasure)
   {
     case 1: {
               printf ("(=>feet )\n"); break;
             }
     case 2: {
               printf ("(=>meter   )\n"); break;
             }
     default:{
               printf ("(=>?%d?   )\n", elevUnitOfMeasure); break;
             }
   }

   printf ("[10] polygon sides: %i\n",polygonSizes );
   printf ("[11] ground coordinates\n");
   printf ("     NW (%14.5f, %14.5f)  NE (%14.5f, %14.5f)\n", groundCoords[1][0], groundCoords[1][1],
          groundCoords[2][0], groundCoords[2][1]);
   printf ("     SW (%14.5f, %14.5f)  SE (%14.5f, %14.5f)\n", groundCoords[0][0], groundCoords[0][1],
          groundCoords[3][0], groundCoords[3][1]);
   printf ("[12] elev. min: %15.5f, max: %15.5f, [14] accuracy code: %i\n", elevBounds[0], elevBounds[1], accuracyCode);
   printf ("[13] rotation: %14.5f degrees\n", localRotation * 180.0/M_PI);
   printf ("[15] spatial resolution: %15.5f, %15.5f, %15.5f \n", spatialResolution[0], spatialResolution[1], spatialResolution[2]);
   printf ("[16] map size is %i x %i (row x columns)\n", profileDimension[0], profileDimension[1]);
}

void writeHeader (FILE *outputFile)
{
   int i;

   fprintf(stderr," TGA header width = %i  height = %i \n", width, height);

   /* Write TGA image header */
   for (i = 0; i < 10; i++)      /* 00, 00, 02, then 7 00's... */
      if (i == 2)
	 putc ((short)i, outputFile);
      else
	 putc (0, outputFile);

   putc (0, outputFile); /* y origin set to "First_Line" */
   putc (0, outputFile);
   putc ((short)(width % 256), outputFile);  /* write width and height */
   putc ((short)(width / 256), outputFile);
   putc ((short)(height % 256), outputFile);
   putc ((short)(height / 256), outputFile);
   putc (24, outputFile);  /* 24 bits/pixel (16 million colors!) */
   putc (32, outputFile);  /* Bitmask, pertinent bit: top-down raster */
}
