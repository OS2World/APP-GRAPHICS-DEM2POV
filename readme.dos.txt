DEM2POV v1.2a

Converts USGS Digital Elevation Model data files to tga height-field for
POV raytracer. converted W. D. Kirby, 30 mar 96. hacked from dem2xyz.c
18 apr 95. converts 3 arc second to lat/long, sol katz, mar. 94. added
sampling and cutting to size, sol katz, apr 94. changed the calculation
of start position in sampling code, sol katz, jan 95.

Dedication: Dedicated to free speech, and repeal of the 1996 U. S.
Communications Decency Act (CDA).


This program converts a USGS Digital Elevation Model (DEM) file into a
height-field .TGA file for use a Persistence-of-Vision (POV)
photorealistic raytracer heightfield.

v1.2a   fix to reading Accuracy Code for Linux (Thanks to Bjarne Nygaard
        nygabny@inet.uni-c.dk)
v1.2    Modified source for portablility, outputs all rows & columns, and
        bug fixes 3/30/96   
v1.1c   Recompiled with static libraries 4 nov. 95
v1.1b   Corrected elev units from DEM file header
v1.1a   Expanded documentation and example, and included missing files
v1.1    Added feature for normalized output

Requirements: The executable program requires at least a 386 cpu with DOS.

Usage:

       >DEM2POV input.dem [output.tga]

If only the .dem file name is given on the command line the program will
list the DEM file header information and then terminate. The information
listed is described in detail in the USGS DEM Users Guide. DEM2POV
provides some interpretation of the header coding such as units used,
coordinates, and data array sizes. The DEM header information can be
copied to file using the ">" redirect command, or for example

       >DEM2POV input.dem > output.txt

A DEM file has columns and rows of elevation data that are read in
column order first. Column profiles begin from the west boundary , and
the rows of data in a column profile runs from south to north. It should
be noted that for 1-degree DEM's that data run exactly along the
boundaries, and thus are duplicated in adjacnet DEM files. 

When the command line includes the desired Targa .TGA image filename the
program asks for a few additional parameters. 

The first parameter requested selects the output data to be included
("0") all data, ("1") for samples, and ("2") for a subset. The data in
the DEM file is column and row oriented, and just prior to this the
program prints out the number of columns and rows within the file data. 

(all data)

Selecting ("0") all rows and columns of data will be converted. 

(samples)

For ("1") samples mode the program will then request the sampling
interval for the columns, and then the row sampling interval. 

(subset)

For a ("2") subset mode the program will request the starting and ending
columns and rows, and the output will contain all the data between these
bounds. The program has been modified in version 1.2 to output all
columns and rows in a DEM file. If TGA files are to be combined from
multiple adjacent 1- degree DEM files a subset method should be used to
remove the duplicate data at the overlapped boundaries. 

The second major input requests the type of height-field desired. This
can be ("0") for actual heights, and ("1") for normalized heights. For
normalized heights the normalization factor is defined in terms of the
DEM file min and max heights.

(actual heights)

For actual height value cases ("0") the scaling factor for the elevation
data is requested next. This is useful for elevation unit conversions
(e.g., 3.281 for meters to feet). 

<normalized heights>

For normalized height values ("1") the elevations in the DEM file plus a
user inputted bias are normalized by the factor 

               Norm = 65535.0/(Elevation_maximum + Bias)

where Elevation_maximum is from the DEM file header and Bias is the next
user input.

(bias)

The third major input is for a possible bias value to be used on actual
or scaled data values. If a negative value is entered the value is
subtracted from each actual or scaled input value. If normalization is
selected scaling is not performed, and the bias will be applied to the
DEM file units. For actual heights the height-field output will contain
the scaled elevation values plus the bias.

<scale factor>

The actual output heights can be scaled by this input value. This input
is good for conversion of elevation units.

<default value>

The last interactive input defines a default elevation in the output
units to be used for those pixel points where DEM data are not
available. This is useful for eliminating sharp changes at the
height-field edges where sparse data situations may occur.

Output

The Targa image file format (.TGA) used by POV for height-fields is a 24
bit per pixel color image file used to store unsigned 16 bit height data
in an x,y format. The Blue data byte is not used here, the Red byte
entry for each pixel contains the high order byte of the height data,
and the Green byte entry contains the low order byte of the height data.
For the typical DEM file the output is rotated 90 degrees to the right
with North to the right and West at the top with a top down raster scan.

The height field is mapped to the x-z plane, with its lower left corner
sitting at the origin. It extends to 1 in the positive x direction and
to 1 in the positive z direction. It is 1 unit high in the y direction.
You can translate it, scale it, and rotate it to your heart's content."


Within POV the height is assumed to be an unsigned 16 bit integer, and
thus the TGA pixel height values range from 0 to +65,535 which maps to
POV units from 0 to 1.0 in the y- direction.

Example inputs

An example DEM file for Kirkland, AZ, USA in included which has
elevations that range form 1158 to 1820 meters, and has 470 rows and 390
columns. This will produce a TGA file image size of 470x390 if all data
were used.

For a case where the scaling is 1.0 and the bias was 0.0 the height-
field file values would range from 1158 to 1820. Within POV units these
elevations would correspond to the following


     1158   =>  0.0177
     1820   =>  0.0278

If the user selected to normalize with no bias the DEM to POV units
association would be

     1158   =>  0.6326
     1820   =>  1.0

If the user selected to normalize with a bias equal to the minimum
elevation (-1158) the DEM to POV units association would be

     1158   =>  0.0
     1820   =>  1.0

A graphics file view is useful for previewing the results with black to
green corresponding to 0.0 to .5, and red to brown corresponding to the
higher elevations 0.5 to 1.0.

(hftest.pov)

A POV test case for kirkland.dem is included which applies a gradient
color mapping as a function of elevation. The peaks are thus white and
the lowlands are brown and green. DEM2POV converted all data points, and
used normalization with a bias of -1158. 


Files Included

     DEM2POV.EXE         DOS executable program
     DEM2POV.C           C source code
     DEM2POV.IDE         BC++ v4.5 + Power Pack project file
     DEM2POV.MAK         BC++ v4.5 + Power Pack makefile
     32RTM.EXE           Borland DOS extender
     DPMI32VM.OVL        Borland DOS extender
     KIRKLAND.ZIP        Compressed .DEM sample file ( almost 1.2MB
                                               uncompressed)
     HFTEST.POV          POV 2.2 test script file for height-field
     HFTEST.GIF          Example POV output using Kirkland.dem
     MAKEFILE            Makefile for DJGPP GCC v2.0
     README.TXT          This file


Source Code

The original dem2xyz.c source code was obtained from the Internet site
ftp.blm.gov which has a number of USGS data conversion utilities. The
code has been modified to add further interpretation of the DEM file
header, and for conversion of the data to a .TGA height-field format.
The code has been updated to make it portable.

The code has been modifed to be ANSI compliant. 1/30/96

Compiling source

The source code has been compiled and tested with the folowing 32 bit
compilers

            DJGPP v1.12m4 & v2.0,
            Watcom v10.0, and
            Borland C++ v4.5 with Power Pack.

The code has been compiled using Borland C++ for a standard 16 bit DOS
target, but some of the DEM header file information printed out is in
error because of integer sizes. However, in cases tested the .TGA
outputs were not in error.

Executable

The executable was created using BC++ v4.5 with Power Pack for 32 bit
protected mode DOS.  The BC++ 32 bit DOS extender files 32RTM.exe and
DPMI32VM.OVL are included.  

Example Inputs

The following examples illustrate the inputs that can be used. Remember
that most DEM files have data in columns that run S-N and rows that run
W-E which create a height-field image with North on the right and West
at the top in normal screen coordinates.

                                                        Example #
                                                      #1     #2     #3

>DEM2POV kirkland.dem kirkland.tga

Enter 0 for all, 1 for samples 2 for subset:           0      1      2
  {case = 1}
  Enter column sample interval                       N/A      4    N/A
  Enter row sample interval                          N/A      4    N/A
  {case = 2}
  Enter start column                                 N/A    N/A     11
  Enter end column                                   N/A    N/A    200
  Enter start row                                    N/A    N/A     26
  Enter end row                                      N/A    N/A    225

Height-field type (0) Actual heights (1) Normalized    1      1      0
  {case = 0}
  Enter vertical scale factor                        N/A     N/A  3.281

Enter elevation bias                               -1158   -1158  -3799

Enter default elevation                                0      0       0


N/A = not applicable since input is not reqested



Example #1 is simple and was actually used to render the sample case. It
uses all the data in the DEM file which has 470 rows and 390 columns,
and the resulting TGA image will be 390 x 470 x 16M. It is normalized
and biased such that the resulting heights range from 0.0 to 1.0 in POV
units, and sized 1 x 1 in the horizontal plane. The bias of -1158 was
selected since the DEM file header shows the minimum height was 1158
feet, and the bias pulls all heights down by that much before
normalizing. The last entry for default elevation mostly effects the
edges of the height-field where there was no DEM data available. This is
the simplest input for POV, and probably the most useful if actual
horizontal and vertical scale factors don't have to be preserved.

Example # 2 is a little more complex, but useful for getting smaller
height-field files that can be quickly rendered by POV for proofing a
scene before doing a full blown rendering. The DEM file data is sampled
at every 4 th row and column, and results in a height-field image that
is 1/8 the size (117 x 97 x 16M) in the horizontal plane. So it can be
used just like example #1 and would render faster, but will not be as
smooth as a full height-field.

Example #3 is quite complex where only a small horizontal region is of
interest and the height-field units are in meters. The selected data
covers a rectangular region that ranges from column 11 to 200 and rows
26 to 225, and results in a height-field image that is 200 x 190 x 16M.
The input has elevation units in meters and scaling by 3.281 converts
the elevations to . Then the bias value of -3799 lowers all elevations
by 1158 meters (i.e., -1158 x 3.281).


Tips

As a general rule it is best to use the normalized elevations such that
the height-field has the highest resolution, and then correct to desired
values within the POV script since the horizontal dimensions must be
scaled as well. However, a TGA height-field really over resolves the
elevations since the values can range from 0 to 65535.

Other Heightfield Utilities:

HF-LAB

John Beale (beale@jumpjibe.stanford.edu) has a long history of
developing excellent terrain heightfield utilities and the creation of
fantastic raytraced artwork. This includes the artifical terrian
generator GFORGE, and currently has HF-LAB in beta testing. HF-LAb can
accept a .TGA heightfield file as generated by DEM2POV as input to be
viewed, edited, manipulated, and converted to other file formats such as
.GIF. Visit one of his World Wide Web home pages at
http://www2.best.com/~beale or http://www-leland.stanford.edu/~beale to
see more of his terrain modeling and art work.

FRACTINT

This is a public domain fractal generation and display program that can
accept a GIF format output from HF-LAB, and combined with a selected
color map the heightfield can be effectively contoured. More can be
learned about FRACTINT at http://spanky.triumf.ca/www/welcome1.html.

Work in Progress:

The program is being modified so that multiple DEM files can be combined
into a single heightfield output. For viewing there is the possiblity of
adding a color mapped graphic output such as a GIF and/or PNG image
file.

Disclaimer:

The author assumes no responsibility for the use of the program, source
code, documentation, sample data, and etc. 

Have fun.

Bill Kirby
email: wdkirby@ix.netcom.com

comments welcomed


                          APPENDIX I

Digital Elevation Model (DEM) data used by DEM2POV are available from
the USGS.  There are other DEM file formats in use by Geographic
Information Systems (GIS) programs and data sources, so beware.
USGS DEM files may be purchased from USGS on CDROM or obtained from a 
number of Internet FTP sites. Important Internet sites are

USGS World Wide Web home page (excellent starting point for beginners)
   http://info.er.usgs.gov

USGS/EROS FTP site - for DEM and DLG data files
   ftp://edcftp.cr.usgs.gov/pub/data

USGS - Technical Standards and Chartographic Software
   ftp://nmdpow9.er.usgs.gov/public


                        APPENDIX II
 

DEM Data (extracted from Digital Elevation Models - Users Guide 5)

The Earth Science Information Center (ESIC) distributes digital
cartographic/geographic data files produced by the U. S. Geological Survey
(USGS) as part of the National Mapping program.  The Digital Elevation Model
(DEM) consists of a sampled array of elevations for a number of ground
positions at regularly spaced intervals.  The digital cartographic data files
from selected quadrangles currently available from ESIC include the following:

     
         Digital Elevation Models (DEM's)
          - 7.5-minute
          - 15-minute
          - 30-minute
          - 1-degree

This document describes five distinct digital elevation products that are
distributed by the USGS in the standard DEM format:

     1.   7.5-Minute DEM (30- x 30-m data spacing, cast on Universal
Transverse Mercator (UTM) projection).  Provides coverage in 7.5- x 7.5-minute
blocks.  Each product provides the same coverage as a standard USGS 7.5-minute
map series quadrangle.  

     2.   1-Degree DEM (3- x 3-arc-second data spacing).  Provides coverage in
1- x 1-degree blocks. Two products (three in some regions of Alaska) provide
the same coverage as a standard USGS 1- x 2-degree map series quadrangle.  The
basic elevation model is produced by or for the Defense Mapping Agency (DMA),
but is distributed by USGS in the DEM data record format.

     3.   30-Minute DEM (2- x 2-arc-second data spacing).  Consists of four
15- x 15-minute DEM blocks.  Two 30-minute DEM's provide the same coverage as
a standard USGS 30- x 60-minute map series quadrangle.  Saleable units will be
30- x 30-minute blocks, that is, four 15- x 15-minute DEM's representing one
half of a 1:100,000-scale map (30- x 60-minute coverage).

     4.   15-minute Alaska DEM (2- x 3-arc-second data spacing, latitude by
longitude).  Provides coverage similar to a 15-minute DEM, except that the
longitudinal cell limits vary from 20 minutes at the southernmost latitude of
Alaska to 36 minutes at the northernmost latitude limits of Alaska.  Coverage
of one DEM will generally correspond to a l:63,360-scale quadrangle.

     5.   7.5-minute Alaska DEM (1- x 2-arc-second data spacing, latitude by
longitude).  Provides coverage similar to a 7.5-minute DEM, except that the
longitudinal cell limits vary from 10 minutes at the southernmost latitude of
Alaska to 18-minutes at the northernmost latitude limits of Alaska.

The UTM-based DEM's (7.5-minute DEM) and the geographic-based DEM's
(30-minute, Alaska, and 1-degree DEM's) are identical in logical data
structure but differ in sampling interval, geographic reference system, areas
covered, and horizontal and vertical accuracy. Knowledge of all of these
properties is essential to ensure that the user does not exceed the useful
limits of the data for required applications.  The 7.5-minute UTM DEM's are
available for selected quadrangles, which are indicated on a status graphic
published biannually by USGS.  The 1-degree DEM's are available for all of the
contiguous United States, Hawaii, and portions of Alaska, Puerto Rico, and the
Virgin Islands.  Many of the original 1-degree DEM's are being replaced with
more accurate digital models through a cooperative regridding project with
DMA, scheduled for completion in 1995.  As they become available, these
1-degree DEM's will replace their corresponding product.  The 30-minute and
Alaska DEM's are new DEM series and are available on a limited basis as
projects are completed.
