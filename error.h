#ifndef ERROR_H
#define ERROR_H

#include <string>
#include <string_view>
#include <utility>

struct error{
   std::string mensaje;
   std::string_view vista;

   error(const std::string& m, const char* ini, const char* fin)
      : mensaje(m), vista(ini, fin - ini){
      }

   error(const std::string& m, const std::string_view& v)
      : mensaje(m), vista(v){
      }
};

std::pair<int, int> posicion(const std::string& s, const std::string_view& v){
   int fila = 0, columna = 0;
   for(auto itr = &s[0]; itr != v.begin(); ++itr){
      if(*itr == '\n'){
         ++fila, columna = 0;
      }else{
         ++columna;
      }
   }

   return { fila, columna };
}

#endif
