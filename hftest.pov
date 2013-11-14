// Persistence of Vision Raytracer
// DEM2POV test 


#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"

camera {
   location <-600.0, 200.0, -600.0>
   direction <0.0, 0.0, 4.0>   //4.0 for telephoto effect
   up <0.0, 1.0, 0.0>        
   right <4/3, 0.0, 0.0>
   look_at <0.0, 50.0, 0.0>
}

// Define a couple of colors for the light sources.
#declare MainLight = color red 0.8 green 0.8 blue 0.8
#declare FillLight = color red 0.23 green 0.23 blue 0.25
// Light source (main)
light_source { <0.0, 300.0, -60.0> color MainLight }
// Light source ( shadow filler )
light_source { <500.0, 300.0, 600.0> color FillLight }

height_field  {

   tga "kirkland.tga"    // x 0-1.0 y 0-1.0 z 0-1.0

//   water_level 0.0
//     pigment { White }
   pigment { gradient y  // color map for elevation
		color_map {
			[0.0 color red 1.0 green 0.5 blue 0.0]
			[0.25 color red 0.5 green 0.5 blue 0.0]
			[0.300 color red 0.5 green 0.75 blue 0.25]
			[0.500 color red 0.25 green 0.25 blue 0.25]
			[0.9 color red 0.75 green 0.75 blue 0.75]
			[1.0 color red 1.0 green 1.0 blue 1.0]
		}
	}
   
   finish {
	  crand 0.025         // dither  - not used often, but this image needs it. 
	  ambient 0.2         // Very dark shadows
	  diffuse 0.8         // Whiten the whites
	  phong 0.2          // shiny
	  phong_size 100.0    // with tight highlights
	  specular 0.5
	  roughness 0.05
   }
   translate <-0.5, 0.0, -0.5>  // Center the image by half  
   scale < 1200, 64.0, 1200 >  // scale horiz x 1200 and vert x 64

}


// end of file
