//
// $Id$
//
/*!
  \file
  \ingroup scatter
  \brief Implementations of functions defined in scatter.h
         Function calculates the integral along LOR in a 
                    image (attenuation or emission). 
					(From scatter point to detector coordinate)

  \author Pablo Aguiar
  \author Charalampos Tsoumpas
  \author Kris Thielemans

 
 
  $Date$
  $Revision$

    Copyright (C) 2004- $Date$, Hammersmith Imanet
    See STIR/LICENSE.txt for details
*/


#include "local/stir/Scatter.h"
#include "stir/VoxelsOnCartesianGrid.h"
#include "stir/recon_buildblock/ProjMatrixElemsForOneBin.h"
#include "stir/recon_buildblock/RayTraceVoxelsOnCartesianGrid.h"

using namespace std;

START_NAMESPACE_STIR

float integral_scattpoint_det (
	  const DiscretisedDensityOnCartesianGrid<3,float>& image_as_density,
	  const CartesianCoordinate3D<float>& scatter_point, 
      const CartesianCoordinate3D<float>& detector_coord)
{
  const VoxelsOnCartesianGrid<float>& image =
  dynamic_cast<const VoxelsOnCartesianGrid<float>&>(image_as_density);

  const CartesianCoordinate3D<float> voxel_size = image.get_voxel_size();

  ProjMatrixElemsForOneBin lor;
  RayTraceVoxelsOnCartesianGrid(lor, 
                              scatter_point, 
                              detector_coord/voxel_size,
							  voxel_size,
							  0.1F);
  float sum = 0;  // add up values along LOR
  {     

	  ProjMatrixElemsForOneBin::iterator element_ptr =lor.begin() ;
	  
	  while (element_ptr != lor.end())
	  {
		  const BasicCoordinate<3,int> coords = element_ptr->get_coords();
		  
		  if (coords[1] >= image.get_min_index() && 
			  coords[1] <= image.get_max_index() &&
			  coords[2] >= image[coords[1]].get_min_index() && 
			  coords[2] <= image[coords[1]].get_max_index() &&
			  coords[3] >= image[coords[1]][coords[2]].get_min_index() && 
			  coords[3] <= image[coords[1]][coords[2]].get_max_index())
			  sum += image[coords[1]][coords[2]][coords[3]] * element_ptr->get_value();
		  else
			  break;
		  ++element_ptr;		
	  }	      
  }  
  return sum; 
}

END_NAMESPACE_STIR




