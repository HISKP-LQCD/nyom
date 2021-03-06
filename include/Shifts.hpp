/**********************************************************************
 * Copyright (C) 2018 Bartosz Kostrzewa
 *
 * This file is part of nyom.
 *
 * nyom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * nyom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with nyom.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************/

#pragma once

#include <ctf.hpp>
#include <vector>

namespace nyom {

  typedef enum shift_dimension_t {
    xm1 = 0,
    xp1,
    ym1,
    yp1,
    zm1,
    zp1
  } shift_dimension_t;


  /**
   * @brief build a vector of shift matrices (2-index tensors). For shifts
   * by a single lattice site in negative or positive direction in each
   * of the three spatial dimensions as given in shift_dimension_t ordering.
   *
   * @param Nx global lattice extent in X direction
   * @param Ny global lattice extent in Y direciton
   * @param Nz global lattice extent in Z direction
   * @param world CTF::World from the currently defined geometry
   *
   * @return vector of shift tensors initialised to produce displacements
   * by a single lattice spacing 
   */
  std::vector< CTF::Tensor< complex<double> > > make_shifts(const int Nx,
                                                 const int Ny,
                                                 const int Nz,
                                                 CTF::World &world){
    const int shapes[2] = {NS, NS};
    int sizes[2];
    int dim_sizes[3] = { Nx, Ny, Nz };

    std::vector< CTF::Tensor< complex<double> > > shifts;

    sizes[0] = sizes[1] = Nx;
    shifts.emplace_back( CTF::Tensor< complex<double> >(2, sizes, shapes, world) );
    shifts.emplace_back( CTF::Tensor< complex<double> >(2, sizes, shapes, world) );
    
    sizes[0] = sizes[1] = Ny;
    shifts.emplace_back( CTF::Tensor< complex<double> >(2, sizes, shapes, world) );
    shifts.emplace_back( CTF::Tensor< complex<double> >(2, sizes, shapes, world) );
    
    sizes[0] = sizes[1] = Nz;
    shifts.emplace_back( CTF::Tensor< complex<double> >(2, sizes, shapes, world) );
    shifts.emplace_back( CTF::Tensor< complex<double> >(2, sizes, shapes, world) );

    // for each direction (-+x , -+y, -+z), construct the shift
    // matrix (for shifts by a single lattice site)
    for( int dir : {0, 1, 2, 3, 4, 5} ){
      int64_t npair;
      int64_t* indices;
      complex<double>* pairs;
      int dim = dir / 2;
      int offset = 1;
      if( dir % 2 == 0 ) offset = -1;
      shifts[dir].read_local(&npair, &indices, &pairs);
      for(int64_t i = 0; i < npair; ++i ){
        // In Fortran fashion, the row-index runs fastest as anywhere in CTF
        int64_t c = indices[i] / dim_sizes[dim];
        int64_t r = indices[i] % dim_sizes[dim];
        // the resulting matrix has non-zero entries on the first upper or lower
        // off-diagonal plus entries which implement periodic boundary conditions
        if( c == ( ((r+offset)+dim_sizes[dim]) % dim_sizes[dim]) ){
          pairs[i] = 1.0;
        } else {
          pairs[i] = 0.0;
        }
      }
      shifts[dir].write(npair, indices, pairs);
      free(pairs); free(indices);
      shifts[dir].sparsify(); 
    }
    return( shifts );
  }


} // namespace(nyom)
