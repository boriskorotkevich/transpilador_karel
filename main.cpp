#define DEBUG

#include <functional>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

#include "lexer_pascal.h"
#include "lexer_java.h"
#include "lexer_ruby.h"
#include "debug.h"
#include "error.h"

int main(int argc, char* argv[]){
   std::string entrada;

   try{
      if(argc != 4){
         std::cout << "El programa no hace magia =(" << std::endl;
         return 0;
      }

      std::vector<std::string> versiones = {"java", "ruby", "pascal"};
      auto itr_vorigen = std::find(versiones.begin(), versiones.end(), argv[2]);
      auto itr_vdestino = std::find(versiones.begin(), versiones.end(), argv[3]);

      if(itr_vorigen == versiones.end() || itr_vdestino == versiones.end()){
         std::cout << "ERROR: Version de origen o destino no disponible" << std::endl;
         return 0;
      }

      std::fstream f(argv[1], std::ios::in);
      std::stringstream ss;
      f >> ss.rdbuf();
      entrada = std::move(ss).str();

      std::vector<token_registrado> tokens;
      if(*itr_vorigen == "pascal"){    // lexer_pascal
         lexer_pascal pascal;
         tokens = lexer(&pascal, entrada);
      }else if(*itr_vorigen == "java"){ // lexer_java
         lexer_java java;
         tokens = lexer(&java, entrada);
      }else{                           // lexer_ruby
         lexer_ruby ruby;
         tokens = lexer(&ruby, entrada);
      }

#ifdef DEBUG
      for(const token_registrado& tr : tokens){
         std::cerr << tr << std::endl; 
      }
#endif

   }catch(const error& e){
      auto [l, c] = posicion(entrada, e.vista);
      std::cout << "---> ERROR [" << l + 1 << " : " << c + 1 << "] | " << e.mensaje << std::endl;
   }
}
