// $Id$
/*
    Copyright (C) 2005- $Date$, Hammersmith Imanet Ltd
    This file is part of STIR.

    This file is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This file is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

   See STIR/LICENSE.txt for details
*/
/*!
  \file 
  \ingroup numerics_test
 
  \brief tests for functions in MatrixFunction.h etc.

  \author Kris Thielemans
  $Date$
  $Revision$
*/
#include "stir/numerics/max_eigenvector.h"
#include "stir/numerics/norm.h"
#include "stir/make_array.h"
#include "stir/Coordinate2D.h"
#include "stir/Coordinate3D.h"
#include "stir/Succeeded.h"
#include "stir/RunTests.h"
#include "stir/stream.h"
#include <complex>
#include <cmath>
# ifdef BOOST_NO_STDC_NAMESPACE
 namespace std { using ::sqrt; }
# endif

START_NAMESPACE_STIR


/*!
  \brief Tests MatrixFunction.h functionality
  \ingroup test_numerics

*/
class MatrixTests : public RunTests
{
public:
  void run_tests();
private:
  void run_tests_1D();
  void run_tests_2D();
  void run_tests_max_eigenvector();
};



void
MatrixTests::
run_tests()
{
  std::cerr << "Testing numerics/MatrixFunction.h functions\n";
  run_tests_1D();
  run_tests_2D();
  run_tests_max_eigenvector();
}

void
MatrixTests::
run_tests_1D()
{
  std::cerr << "Testing 1D stuff" << std::endl;

  {
    const Array<1,float> v = make_1d_array(1.F,2.F,3.F,-5.F);
    check_if_equal(norm(v),std::sqrt(square(1.F)+square(2)+square(3)+square(5)),
		   "norm of float array");
    check_if_equal(inner_product(v,v),square(1.F)+square(2)+square(3)+square(5),
		   "inner_product of float array with itself");
    const Array<1,float> v2 = make_1d_array(7.F,8.F,3.3F,-5.F);
		   check_if_equal(inner_product(v,v2),1.F*7+2*8+3*3.3F+25,
		   "inner_product of float arrays");
  }
  {
    typedef std::complex<float>  complex_t;
    const Array<1,complex_t > v = make_1d_array(complex_t(1.F,0.F),
						complex_t(2.F,-3.F));
    check_if_equal(norm(v),std::sqrt(square(1.F)+square(2)+square(3)),
		   "norm of complex array");
    check_if_equal(inner_product(v,v),
		   complex_t(square(1.F)+square(2)+square(3),0.F),
		   "inner_product of complex array with itself");
    const Array<1,complex_t > v2 = make_1d_array(complex_t(1.F,1.F),
						 complex_t(4.F,5.F));
    check_if_equal(inner_product(v,v2),
		   std::conj(v[0])*v2[0]+std::conj(v[1])*v2[1],
		   "inner_product of complex arrays");
  }
}

void
MatrixTests::
run_tests_2D()
{
  std::cerr << "Testing 2D stuff" << std::endl;
  const Array<2,float> m1 = 
    make_array(make_1d_array(3.F,4.F),
	       make_1d_array(5.F,6.F),
	       make_1d_array(1.5F,-4.6F));
  // matrix*vector
  {
    const Array<1,float> v = make_1d_array(1.F,2.F);
    check_if_equal(matrix_multiply(m1,v),
		   make_1d_array(m1[0][0]*v[0]+m1[0][1]*v[1],
				 m1[1][0]*v[0]+m1[1][1]*v[1],
				 m1[2][0]*v[0]+m1[2][1]*v[1]),
		   "matrix times vector");
  }
  // matrix*matrix
  {
    const Array<2,float> m2 = 
      make_array(make_1d_array(1.F,4.3F,6.F,8.F),
		 make_1d_array(-5.F,6.5F,2.F,5.F));    
    check_if_equal(matrix_multiply(m1,m2),
		   make_array(make_1d_array(m1[0][0]*m2[0][0]+m1[0][1]*m2[1][0],
					    m1[0][0]*m2[0][1]+m1[0][1]*m2[1][1],
					    m1[0][0]*m2[0][2]+m1[0][1]*m2[1][2],
					    m1[0][0]*m2[0][3]+m1[0][1]*m2[1][3]),
			      make_1d_array(m1[1][0]*m2[0][0]+m1[1][1]*m2[1][0],
					    m1[1][0]*m2[0][1]+m1[1][1]*m2[1][1],
					    m1[1][0]*m2[0][2]+m1[1][1]*m2[1][2],
					    m1[1][0]*m2[0][3]+m1[1][1]*m2[1][3]),
			      make_1d_array(m1[2][0]*m2[0][0]+m1[2][1]*m2[1][0],
					    m1[2][0]*m2[0][1]+m1[2][1]*m2[1][1],
					    m1[2][0]*m2[0][2]+m1[2][1]*m2[1][2],
					    m1[2][0]*m2[0][3]+m1[2][1]*m2[1][3])),
		   "matrix times matrix");
  }

  // transpose
  {
    const Array<2,float> m1_trans = 
      make_array(make_1d_array(m1[0][0],m1[1][0],m1[2][0]),
		 make_1d_array(m1[0][1],m1[1][1],m1[2][1]));
    check_if_equal(matrix_transpose(m1), m1_trans,
		   "matrix transposition");
  }
  // diagonal_matrix
  {
    const Array<2,float> d = 
      diagonal_matrix(2, 3.F);
    check_if_equal(d, 
		   make_array(make_1d_array(3.F,0.F),
			      make_1d_array(0.F,3.F)),
		   "diagonal_matrix with all diag-elems equal");
    const Array<2,float> d2 = 
      diagonal_matrix(Coordinate2D<float>(3.F,4.F));
    check_if_equal(d2, 
		   make_array(make_1d_array(3.F,0.F),
			      make_1d_array(0.F,4.F)),
		   "diagonal_matrix with differing diag-elems");
  }

}


void
MatrixTests::
run_tests_max_eigenvector()
{
  std::cerr << "Testing max_eigenvector stuff" << std::endl;
  set_tolerance(.01);
  {
    float max_eigenvalue;
    Array<1,float> max_eigenvector;
    {
      const Array<2,float> d = 
	diagonal_matrix(Coordinate3D<float>(3.F,4.F,-2.F));
      Succeeded success =
	absolute_max_eigenvector_using_power_method(max_eigenvalue,
						    max_eigenvector,
						    d, 
						    make_1d_array(1.F,2.F,3.F),
						    /*tolerance=*/ .001,
						    1000UL);
      check(success == Succeeded::yes, 
	    "abs_max_using_power: succeeded (float diagonal matrix)");
      
      check_if_equal(max_eigenvalue, 4.F, 
		     "abs_max_using_power: eigenvalue (float diagonal matrix)");
      check_if_equal(max_eigenvector, make_1d_array(0.F,1.F,0.F), 
		     "abs_max_using_power: eigenvector (float diagonal matrix)");
      success =
	absolute_max_eigenvector_using_shifted_power_method(max_eigenvalue,
							    max_eigenvector,
							    d, 
							    make_1d_array(1.F,2.F,3.F),
							    .5F, // note: shift should be small enough that it doesn't make the most negative eigenvalue 'larger'
							    /*tolerance=*/ .001,
							    1000UL);
      check(success == Succeeded::yes, 
	    "abs_max_using_shifted_power: succeeded (float diagonal matrix)");
      
      check_if_equal(max_eigenvalue, 4.F, 
		     "abs_max_using_shifted_power: eigenvalue (float diagonal matrix)");
      check_if_equal(max_eigenvector, make_1d_array(0.F,1.F,0.F), 
		     "abs_max_using_shifted_power: eigenvector (float diagonal matrix)");

      success =
	max_eigenvector_using_power_method(max_eigenvalue,
					   max_eigenvector,
					   d, 
					   make_1d_array(1.F,-2.F,-3.F),
					   /*tolerance=*/ .001,
					   1000UL);
      check(success == Succeeded::yes, 
	    "max_using_power: succeeded (float diagonal matrix)");
      
      check_if_equal(max_eigenvalue, 4.F, 
		     "max_using_power: eigenvalue (float diagonal matrix)");
      check_if_equal(max_eigenvector, make_1d_array(0.F,1.F,0.F), 
		     "max_using_power: eigenvector (float diagonal matrix)");

      success =
	max_eigenvector_using_power_method(max_eigenvalue,
					   max_eigenvector,
					   Array<2,float>(d*(-1.F)), 
					   make_1d_array(1.F,2.F,3.F),
					   /*tolerance=*/ .001,
					   1000UL);
      check(success == Succeeded::yes, 
	    "max_using_power: succeeded (float diagonal matrix with large negative value)");
      
      check_if_equal(max_eigenvalue, 2.F, 
		     "max_using_power: eigenvalue (float diagonal matrix with large negative value)");
      check_if_equal(max_eigenvector, make_1d_array(0.F,0.F,1.F), 
		     "max_using_power: eigenvector (float diagonal matrix with large negative value)");
    }
  }
}

END_NAMESPACE_STIR


int main()
{
  stir::MatrixTests tests;
  tests.run_tests();
  return tests.main_return_value();
}
