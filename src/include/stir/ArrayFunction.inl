// $Id$
/*!

  \file
  \ingroup Array

  \brief Implementations for ArrayFunction.h

  \author Kris Thielemans (some functions based on some earlier work by Darren Hague)
  \author PARAPET project

  $Date$
  $Revision$
  
  \warning Compilers without partial specialisation of templates are
   catered for by explicit instantiations. If you need it for any other
   types, you'd have to add them by hand.
 */
/*
    Copyright (C) 2000 PARAPET partners
    Copyright (C) 2000- $Date$, Hammersmith Imanet Ltd
    See STIR/LICENSE.txt for details
*/

START_NAMESPACE_STIR

//----------------------------------------------------------------------
// element wise and in place numeric functions
//----------------------------------------------------------------------

#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
template <class elemT>
inline Array<1,elemT>&
in_place_log(Array<1,elemT>& v)  
{	
  for(int i=v.get_min_index(); i<=v.get_max_index(); i++)
    v[i] = log(v[i]); 
  return v; 
}

#else
inline Array<1,float>& 
in_place_log(Array<1,float>& v)  
{	
  for(int i=v.get_min_index(); i<=v.get_max_index(); i++)
    v[i] = log(v[i]); 
  return v; 
}
#endif


template <int num_dimensions, class elemT>
inline Array<num_dimensions, elemT>& 
in_place_log(Array<num_dimensions, elemT>& v)  
{	
  for(int i=v.get_min_index(); i<=v.get_max_index(); i++)
    in_place_log(v[i]); 
  return v; 
}

#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
template <class elemT>
inline Array<1,elemT>& 
in_place_exp(Array<1,elemT>& v)  
{	
  for(int i=v.get_min_index(); i<=v.get_max_index(); i++)
    v[i] = exp(v[i]); 
  return v; 
}
#else
inline Array<1,float>& 
in_place_exp(Array<1,float>& v)  
{	
  for(int i=v.get_min_index(); i<=v.get_max_index(); i++)
    v[i] = exp(v[i]); 
  return v; 
}
#endif

template <int num_dimensions, class elemT>
inline Array<num_dimensions, elemT>& 
in_place_exp(Array<num_dimensions, elemT>& v)  
{	
  for(int i=v.get_min_index(); i<=v.get_max_index(); i++)
    in_place_exp(v[i]); 
  return v; 
}

#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
template <class elemT>
inline Array<1,elemT>& 
in_place_abs(Array<1,elemT>& v)  
{	
  for(int i=v.get_min_index(); i<=v.get_max_index(); i++)
    if (v[i] < 0)
      v[i] = -v[i];
  return v; 
}
#else
inline Array<1,float>& 
in_place_abs(Array<1,float>& v)  
{	
  for(int i=v.get_min_index(); i<=v.get_max_index(); i++)
    if (v[i] < 0)
      v[i] = -v[i];
  return v; 
}
#endif

template <int num_dimensions, class elemT>
inline Array<num_dimensions, elemT>& 
in_place_abs(Array<num_dimensions, elemT>& v)  
{	
  for(int i=v.get_min_index(); i<=v.get_max_index(); i++)
    in_place_abs(v[i]); 
  return v; 
}


// this generic function does not seem to work of f is an overloaded function
#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
template <class elemT, class FUNCTION>
inline Array<1,elemT>& 
in_place_apply_function(Array<1,elemT>& v, FUNCTION f)  
{	
  for(int i=v.get_min_index(); i<=v.get_max_index(); i++)
    v[i] = f(v[i]); 
  return v; 
}
#else
inline Array<1,float>& 
in_place_apply_function(Array<1,float>& v, float (*f)(float))  
{	
  for(int i=v.get_min_index(); i<=v.get_max_index(); i++)
    v[i] = f(v[i]); 
  return v; 
}
#endif


template <int num_dimensions, class elemT, class FUNCTION>
inline Array<num_dimensions, elemT>& 
in_place_apply_function(Array<num_dimensions, elemT>& v, FUNCTION f)  
{	
  for(int i=v.get_min_index(); i<=v.get_max_index(); i++)
    in_place_apply_function(v[i], f); 
  return v; 
}


template <int num_dim, typename elemT, typename FunctionObjectPtr> 
inline void
in_place_apply_array_function_on_1st_index(Array<num_dim, elemT>& array, FunctionObjectPtr f)
{
  assert(array.is_regular());
  const int outer_min_index = array.get_min_index();
  const int outer_max_index = array.get_max_index();

  // construct a vector with a full_iterator for every array[i]
  VectorWithOffset<
#ifndef _MSC_VER
    typename 
#endif
      Array<num_dim-1, elemT>::full_iterator > 
    full_iterators (outer_min_index, outer_max_index);  
  for (int i=outer_min_index; i<=outer_max_index; ++i)
    full_iterators[i] = array[i].begin_all();
  
  // allocate 1d array
  Array<1, elemT> array1d (outer_min_index, outer_max_index);

  while (full_iterators[outer_min_index] != array[outer_min_index].end_all())
  {
    // copy elements into 1d array
    for (int i=outer_min_index; i<=outer_max_index; ++i)    
      array1d[i] = *full_iterators[i];
    
    // apply function
    (*f)(array1d);
    
    // put results back
    // and increment full_iterators to do next index
    for (int i=outer_min_index; i<=outer_max_index; ++i)
      *full_iterators[i]++ = array1d[i];
  }
    
}

template <int num_dim, typename elemT, typename FunctionObjectPtr> 
inline void
apply_array_function_on_1st_index(Array<num_dim, elemT>& out_array, 
                                  const Array<num_dim, elemT>& in_array, 
                                  FunctionObjectPtr f)
{
  assert(in_array.is_regular());
  assert(out_array.is_regular());
  const int in_min_index = in_array.get_min_index();
  const int in_max_index = in_array.get_max_index();
  const int out_min_index = out_array.get_min_index();
  const int out_max_index = out_array.get_max_index();

  // construct a vector with a full_iterator for every in_array[i]
  VectorWithOffset< typename Array<num_dim-1, elemT>::const_full_iterator > 
    in_full_iterators (in_min_index, in_max_index);  
  for (int i=in_min_index; i<=in_max_index; ++i)
    in_full_iterators[i] = in_array[i].begin_all();
  // same for out_array[i]
  VectorWithOffset<typename  Array<num_dim-1, elemT>::full_iterator > 
    out_full_iterators (out_min_index, out_max_index);  
  for (int i=out_min_index; i<=out_max_index; ++i)
    out_full_iterators[i] = out_array[i].begin_all();
  
  // allocate 1d array
  Array<1, elemT> in_array1d (in_min_index, in_max_index);
  Array<1, elemT> out_array1d (out_min_index, out_max_index);

  while (in_full_iterators[in_min_index] != in_array[in_min_index].end_all())
  {
    assert(out_full_iterators[out_min_index] != out_array[out_min_index].end_all());
    // copy elements into 1d array
    // increment in_full_iterators for next index
    for (int i=in_min_index; i<=in_max_index; ++i)    
      in_array1d[i] = *(in_full_iterators[i]++);
    
    // apply function
    (*f)(out_array1d, in_array1d);
    assert(out_array1d.get_min_index() == out_min_index);
    assert(out_array1d.get_max_index() == out_max_index);
    
    // put results back
    // increment out_full_iterators for next index
    for (int i=out_min_index; i<=out_max_index; ++i)    
      *(out_full_iterators[i]++) = out_array1d[i];    
  }
  assert(out_full_iterators[out_min_index] == out_array[out_min_index].end_all());
    
}

#ifdef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
// silly business for deficient compilers (including VC 6.0)
#define elemT float
#define FunctionObjectPtrIter ActualFunctionObjectPtrIter
#endif

#ifdef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
template <int num_dim>
#else
template <int num_dim, typename elemT, typename FunctionObjectPtrIter> 
#endif
inline void 
in_place_apply_array_functions_on_each_index(Array<num_dim, elemT>& array, 
                                             FunctionObjectPtrIter start, 
                                             FunctionObjectPtrIter stop)
{
  assert(start+num_dim == stop);
  assert(num_dim > 1);
  in_place_apply_array_function_on_1st_index(array, *start);

  ++start;
  for (typename Array<num_dim, elemT>::iterator restiter = array.begin(); restiter != array.end(); ++restiter)
    in_place_apply_array_functions_on_each_index(*restiter, start, stop);
}

#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
template <typename elemT, typename FunctionObjectPtrIter> 
#endif
inline void 
in_place_apply_array_functions_on_each_index(Array<1, elemT>& array, FunctionObjectPtrIter start, FunctionObjectPtrIter stop)
{
  assert(start+1 == stop);
  (**start)(array);
}


#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
template <int num_dim, typename elemT, typename FunctionObjectPtrIter> 
inline void 
apply_array_functions_on_each_index(Array<num_dim, elemT>& out_array, 
                                    const Array<num_dim, elemT>& in_array, 
                                    FunctionObjectPtrIter start, 
                                    FunctionObjectPtrIter stop)
{
  assert(start+num_dim == stop);
  assert(num_dim > 1);
    
  assert(out_array.is_regular());
  BasicCoordinate<num_dim, int> tmp_out_min_indices, tmp_out_max_indices;
  out_array.get_regular_range(tmp_out_min_indices, tmp_out_max_indices);
  tmp_out_min_indices[1] = in_array.get_min_index();
  tmp_out_max_indices[1] = in_array.get_max_index();
  Array<num_dim, elemT> tmp_out_array(IndexRange<num_dim>(tmp_out_min_indices, tmp_out_max_indices));
  
  for (int i=in_array.get_min_index(); i<=in_array.get_max_index(); ++i)
    apply_array_functions_on_each_index(tmp_out_array[i], in_array[i], start+1, stop);

  apply_array_function_on_1st_index(out_array, tmp_out_array, *start);

}
#endif

// specialisation that uses ArrayFunctionObject::is_trivial etc
#ifdef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
template <int num_dim>
#else
template <int num_dim, typename elemT> 
#endif
inline void 
apply_array_functions_on_each_index(Array<num_dim, elemT>& out_array, 
                                    const Array<num_dim, elemT>& in_array, 
                                    ActualFunctionObjectPtrIter start, 
                                    ActualFunctionObjectPtrIter stop)
{
  assert(start+num_dim == stop);
  assert(num_dim > 1);
    

  //cerr << "apply_array_functions_on_each_index dim " << num_dim << std::endl;
  if ((**start).is_trivial())
    {
      for (int i=max(out_array.get_min_index(), in_array.get_min_index());
	   i<=min(out_array.get_max_index(),in_array.get_max_index());
	   ++i)
	apply_array_functions_on_each_index(out_array[i], in_array[i], start+1, stop);
    }
  else
    {
      assert(out_array.is_regular());
 
      IndexRange<1> influencing_indices;
      if ((**start).get_influencing_indices(influencing_indices, 
                                            IndexRange<1>(out_array.get_min_index(),
                                                          out_array.get_max_index()))
        == Succeeded::no)
        influencing_indices = IndexRange<1>(influencing_indices.get_min_index(),
                                            in_array.get_max_index());
      else
      {
        influencing_indices =
          IndexRange<1>(max(influencing_indices.get_min_index(), 
                            in_array.get_min_index()),
                        min(influencing_indices.get_max_index(), 
                            in_array.get_max_index()));
      }
      BasicCoordinate<num_dim, int> tmp_out_min_indices, tmp_out_max_indices;
      out_array.get_regular_range(tmp_out_min_indices, tmp_out_max_indices);
      tmp_out_min_indices[1] = influencing_indices.get_min_index();
      tmp_out_max_indices[1] = influencing_indices.get_max_index();
      Array<num_dim, elemT> tmp_out_array(IndexRange<num_dim>(tmp_out_min_indices, tmp_out_max_indices));
      
      for (int i=influencing_indices.get_min_index(); i<=influencing_indices.get_max_index(); ++i)
	apply_array_functions_on_each_index(tmp_out_array[i], in_array[i], start+1, stop);
      
      apply_array_function_on_1st_index(out_array, tmp_out_array, *start);
    }

}

#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
// has to be here to get general 1D specialisation to compile
template <typename elemT>
#endif
inline void 
apply_array_functions_on_each_index(Array<1, elemT>& out_array, 
                                    const Array<1, elemT>& in_array, 
                                    ActualFunctionObjectPtrIter start, 
                                    ActualFunctionObjectPtrIter stop)
{
  assert(start+1 == stop);
  (**start)(out_array, in_array);
}


#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
template <typename elemT, typename FunctionObjectPtrIter> 
inline void 
apply_array_functions_on_each_index(Array<1, elemT>& out_array, 
                                    const Array<1, elemT>& in_array, 
                                    FunctionObjectPtrIter start, FunctionObjectPtrIter stop)
{
  assert(start+1 == stop);
  (**start)(out_array, in_array);
}
#endif

#ifdef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
#undef elemT
#undef FunctionObjectPtrIter
#endif

//----------------------------------------------------------------------
// some functions specific for 1D Arrays
//----------------------------------------------------------------------

template<class elemT>
inline elemT 
inner_product (const Array<1,elemT> & v1, const Array<1,elemT> &v2)
{
  return (v1 * v2).sum(); 
}

template<class elemT>
inline double 
norm (const Array<1,elemT> & v1)
{
  return sqrt((double)inner_product(v1,v1));
}

#if 0
// something like this for complex elemTs, but we don't use them
inline double 
norm (const Array<1,complex<double>> & v1)
{
  return sqrt(inner_product(v1,conjugate(v1)).re());

}
#endif

template<class elemT>
inline double
angle (const Array<1,elemT> & v1, const Array<1,elemT> &v2)
{
  return acos(inner_product(v1,v2)/norm(v1)/ norm(v2));
}


//----------------------------------------------------------------------
// some matrix manipulations
//----------------------------------------------------------------------
//TODO to be tested
// KT 29/10/98 as they need to be tested, I disabled them for the moment
#if 0
template <class elemT>	
inline Array<2,elemT>
matrix_transpose (const Array<2,elemT>& matrix) 
{
  Array<2,elemT> new_matrix(matrix.get_min_index2(), matrix.get_max_index2(),
			      matrix.get_min_index1(), matrix.get_max_index1());
  for(int j=matrix.get_min_index2(); j<=matrix.get_max_index2(); j++)
    for(int i=matrix.get_min_index1(); i<=matrix.get_max_index1(); i++) {
      new_matrix[i][j] = matrix[j][i];
    }
  return new_matrix; 
}

template <class elemT>	
inline Array<2,elemT>
matrix_multiply(const Array<2,elemT> &m1, const Array<2,elemT>& m2) 
{

  //KT TODO what to do with growing ? 
  // for the moment require exact matches on sizes as in assert() below

  // make sure matrices are conformable for multiplication
  assert(m1.get_width() == m2.get_height() && m1.get_w_min() == m2.get_h_min());  
	        
  Array<2,elemT> retval(m1.get_h_min(), m1.get_h_max(), 
			  m2.get_w_min(), m2.get_w_max());
#if 0
  // original version, works only when member of Array<2,*>
  for (int i=0; i<height; i++)
    {
      register elemT *numi = &(retval.num[i][0]);// use these temp vars for efficiency
      register elemT numik;
      for(int k=0; k<width; k++)
	{
	  numik = num[i][k];
	  const Array<1,elemT> ivtk = iv[k];
	  for(register int j=0; j<iv.width; j++) 
	    numi[j] += (numik*ivtk[j]);
	}
    }
#endif
  // new version by KT
  for (int i=m1.get_h_min(); i<=m1.get_h_max(); i++)
    {
      // use these temp vars for efficiency (is this necessary ?)
      const Array<1,elemT>& m1_row_i = m1[i];
      Array<1,elemT>& new_row_i = retval[i];
      for(int k=m1.get_w_min(); k<=m1.get_w_max(); k++)
	{
	  elemT m1_ik = m1_row_i[k];
	  const Array<1,elemT>& m2_row_k = m2[k];
	  for(int j=m2.get_w_min(); j<=m2.get_w_max(); j++) 
	    new_row_i[j] += (m1_ik*m2_row_k[j]);
	}
    }
  return retval;
}
#endif // 0 (2D routines)

END_NAMESPACE_STIR

