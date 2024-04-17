//#define DEBUG

#include "debug.h"
#include "error.h"
#include "lexer.h"
#include "lexer_java.h"
#include "lexer_pascal.h"
#include "lexer_ruby.h"
#include "parser.h"
#include "parser_java.h"
#include "parser_pascal.h"
#include "parser_ruby.h"
#include "semantico.h"
#include "codegen.h"
#include "codegen_java.h"
#include "codegen_pascal.h"
#include "codegen_ruby.h"

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>

int main(int argc, char* argv[]){
   if(argc != 4 && argc != 5){
      std::cout << "Uso: " << argv[0] << " ruta_entrada version_origen version_destino [ruta_salida]\n";
      return 0;
   }

   std::string entrada;
   try{
      std::vector<std::string> versiones = {"java", "ruby", "pascal"};
      auto itr_vorigen = std::find(versiones.begin(), versiones.end(), argv[2]);
      auto itr_vdestino = std::find(versiones.begin(), versiones.end(), argv[3]);

      if(itr_vorigen == versiones.end() || itr_vdestino == versiones.end()){
         std::cout << "---> ERROR: Version de origen o destino no disponible" << std::endl;
         return 0;
      }

      std::ifstream f(argv[1]);
      if (!f.is_open( )) {
         std::cout << "---> ERROR: No se pudo abrir el archivo de entrada" << std::endl;
         return 0;
      }
      std::stringstream ss;
      f >> ss.rdbuf();
      entrada = std::move(ss).str();
      entrada.push_back('\n');

      std::vector<token_registrado> tokens;
      if(*itr_vorigen == "pascal"){
         lexer_pascal pascal;
         tokens = lexer(pascal, entrada);
      }else if(*itr_vorigen == "java"){
         lexer_java java;
         tokens = lexer(java, entrada);
      }else{
         lexer_ruby ruby;
         tokens = lexer(ruby, entrada);
      }

#ifdef DEBUG
      std::cerr << tokens << std::endl;
#endif

      arbol_sintactico arbol;
      if(*itr_vorigen == "pascal"){
         parser_pascal pascal;
         arbol = parser(pascal, tokens);
      }else if(*itr_vorigen == "java"){
         parser_java java;
         arbol = parser(java, tokens);
      }else{
         parser_ruby ruby;
         arbol = parser(ruby, tokens);
      }

#ifdef DEBUG
      std::cerr << arbol << std::endl;
#endif

      tabla_simbolos tabla = semantico(arbol, tokens.back( ), (*itr_vorigen == "java" || *itr_vorigen == "ruby"));
#ifdef DEBUG
      //...
#endif

      std::string salida;
      if(*itr_vdestino == "pascal"){
         codegen_pascal pascal;
         salida = codegen(pascal, tokens, arbol, tabla, *itr_vorigen);
      }else if(*itr_vdestino == "java"){
         codegen_java java;
         salida = codegen(java, tokens, arbol, tabla, *itr_vorigen);
      }else{
         codegen_ruby ruby;
         salida = codegen(ruby, tokens, arbol, tabla, *itr_vorigen);
      }

      if (argc > 4) {
         std::ofstream f(argv[4]);
         if (!f.is_open( )) {
            std::cout << "---> ERROR: No se pudo abrir el archivo de salida" << std::endl;
            return 0;
         }
         f << salida;
      } else {
         std::cout << salida;
      }
   }catch(const error& e){
      auto [l, c] = posicion(entrada, e.vista);
      std::cout << "---> ERROR [" << l + 1 << " : " << c + 1 << "] | " << e.mensaje << " " << e.vista << std::endl;
   }
}
