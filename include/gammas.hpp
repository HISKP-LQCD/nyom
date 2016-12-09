#pragma once

#include <map>
#include <numeric>
#include <vector>
#include <complex>
#include <ctf.hpp>

namespace nyom {

const int gamma_sizes[2] = { 4, 4 };
const int gamma_shapes[2] = { NS, NS };

std::map < std::string, CTF::Tensor< std::complex<double> > > g;

std::vector < std::string > i_g;

void init_gammas(CTF::World& dw){
  for( std::string g1 : { "I", "0", "1", "2", "3", "5" } ){
    i_g.push_back(g1);
  }
  
  g["I"] = CTF::Tensor<std::complex<double> >(2, gamma_sizes, gamma_shapes, dw);
  g["0"] = CTF::Tensor<std::complex<double> >(2, gamma_sizes, gamma_shapes, dw);
  g["1"] = CTF::Tensor<std::complex<double> >(2, gamma_sizes, gamma_shapes, dw);
  g["2"] = CTF::Tensor<std::complex<double> >(2, gamma_sizes, gamma_shapes, dw);
  g["3"] = CTF::Tensor<std::complex<double> >(2, gamma_sizes, gamma_shapes, dw);
  g["5"] = CTF::Tensor<std::complex<double> >(2, gamma_sizes, gamma_shapes, dw);
  g["Im5"] = CTF::Tensor<std::complex<double> >(2, gamma_sizes, gamma_shapes, dw);
  g["Ip5"] = CTF::Tensor<std::complex<double> >(2, gamma_sizes, gamma_shapes, dw);

  for( std::string g1 : { "0", "1", "2", "3", "5" } ){
    for( std::string g2 : { "0", "1", "2", "3" } ){
      if( g1 == g2 ) continue;
      i_g.push_back(g1+g2);
      g[g1+g2] = CTF::Tensor<std::complex<double> >(2, gamma_sizes, gamma_shapes, dw);
    }
  } 

  // remember that in CTF the left-most index runs fastest -> each row below corresponds to a column of the resulting g
  int64_t npair;
  int64_t* idx;
  std::complex<double>* pairs;

  g["I"].read_local(&npair,&idx,&pairs);
  for(int64_t i = 0; i < npair; ++i){
    if( idx[i]%4==idx[i]/4 ) pairs[i] = std::complex<double>(1,0);
  }
  g["I"].write(npair,idx,pairs); g["I"].sparsify();  
  free(idx); free(pairs); 

  g["0"].read_local(&npair,&idx,&pairs);
  for(int64_t i = 0; i < npair; ++i){
    if(idx[i]==2 || idx[i]==7 || idx[i]==8 || idx[i]==13) pairs[i] = std::complex<double>(1,0);
  }
  g["0"].write(npair,idx,pairs);
  free(idx); free(pairs); 
   
  g["1"].read_local(&npair,&idx,&pairs);
  for(int64_t i = 0; i < npair; ++i){
    if( idx[i]==3 || idx[i]==6 ) pairs[i] = std::complex<double>(0,-1);
    if( idx[i]==9 || idx[i]==12) pairs[i] = std::complex<double>(0,1);
  }
  g["1"].write(npair,idx,pairs);
  free(idx); free(pairs); 

  g["2"].read_local(&npair,&idx,&pairs);
  for(int64_t i = 0; i < npair; ++i){
    if( idx[i]==3 ) pairs[i] = std::complex<double>(1,0);
    if( idx[i]==6 ) pairs[i] = std::complex<double>(-1,0);
    if( idx[i]==9 ) pairs[i] = std::complex<double>(-1,0);
    if( idx[i]==12) pairs[i] = std::complex<double>(1,0);
  }
  g["2"].write(npair,idx,pairs);
  free(idx); free(pairs); 

  g["3"].read_local(&npair,&idx,&pairs);
  for(int64_t i = 0; i < npair; ++i){
    if( idx[i]==2 ) pairs[i] = std::complex<double>(0,-1);
    if( idx[i]==7 ) pairs[i] = std::complex<double>(0,1);
    if( idx[i]==8 ) pairs[i] = std::complex<double>(0,1);
    if( idx[i]==13) pairs[i] = std::complex<double>(0,-1);
  }
  g["3"].write(npair,idx,pairs);
  free(idx); free(pairs);

  g["5"]["ab"] = (g["0"])["aI"] * (g["1"])["IJ"] * (g["2"])["JK"] * (g["3"])["Kb"];

  for( std::string g1 : { "0", "1", "2", "3", "5" } ){
    g[g1].sparsify();
    for( std::string g2 : { "0", "1", "2", "3" } ){
      if( g1 == g2 ) continue;
      g[g1+g2]["ab"] = (g[g1])["aI"] * (g[g2])["Ib"];
      g[g1+g2].sparsify();
    }
  }

  g["Im5"]["ab"] = 0.5*( (g["I"])["ab"] - (g["5"])["ab"] ); g["Im5"].sparsify(); 
  g["Ip5"]["ab"] = 0.5*( (g["I"])["ab"] + (g["5"])["ab"] ); g["Ip5"].sparsify(); 
}

} // namespace nyom