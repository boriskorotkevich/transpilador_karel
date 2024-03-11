#ifndef ERROR_H
#define ERROR_H

#include <string_view>
#include <utility>
#include <string>

struct error{
   std::string mensaje;
   std::string_view vista;

   error(const std::string& _mensaje, const char* ini, const char* fin)
      : mensaje(_mensaje), vista(ini, fin - ini){
      }

   error(const std::string& _mensaje, const std::string_view& _vista)
      : mensaje(_mensaje), vista(_vista){
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

   return std::make_pair(fila, columna);
}

#endif
