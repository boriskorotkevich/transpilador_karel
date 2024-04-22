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
#include <exception>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string_view>

#ifdef __EMSCRIPTEN__
   #include <emscripten.h>
#endif

#ifdef __EMSCRIPTEN__
   extern "C" EMSCRIPTEN_KEEPALIVE
#endif
const char* traduce(const char* entrada_cstr, const char* vorigen_cstr, const char* vdestino_cstr) {
   std::string entrada(entrada_cstr);
   entrada += '\n';

   try{
      std::string_view vorigen(vorigen_cstr), vdestino(vdestino_cstr);
      std::vector<std::string> versiones = { "java", "pascal", "ruby" };
      if (!std::binary_search(versiones.begin(), versiones.end(), vorigen) ||
          !std::binary_search(versiones.begin(), versiones.end(), vorigen)) {
         return "---> ERROR: Version de origen o destino no disponible";
      }

      std::vector<token_registrado> tokens;
      if(vorigen == "pascal"){
         lexer_pascal pascal;
         tokens = lexer(pascal, entrada);
      }else if(vorigen == "java"){
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
      if(vorigen == "pascal"){
         parser_pascal pascal;
         arbol = parser(pascal, tokens);
      }else if(vorigen == "java"){
         parser_java java;
         arbol = parser(java, tokens);
      }else{
         parser_ruby ruby;
         arbol = parser(ruby, tokens);
      }

#ifdef DEBUG
      std::cerr << arbol << std::endl;
#endif

      tabla_simbolos tabla = semantico(arbol, tokens.back( ), (vorigen == "java" || vorigen == "ruby"));
#ifdef DEBUG
      std::cerr << "No hay debug en el semantico" << std::endl;
#endif

      static std::string salida;
      if(vdestino == "pascal"){
         lexer_pascal lex_pascal;
         codegen_pascal pascal;
         salida = codegen(pascal, lex_pascal, tokens, arbol, tabla, vorigen);
      }else if(vdestino == "java"){
         lexer_java lex_java;
         codegen_java java;
         salida = codegen(java, lex_java, tokens, arbol, tabla, vorigen);
      }else{
         lexer_pascal lex_ruby;
         codegen_ruby ruby;
         salida = codegen(ruby, lex_ruby, tokens, arbol, tabla, vorigen);
      }

      return salida.c_str();
   }catch(const error& e){
      static std::string error; std::ostringstream oss;
      auto [l, c] = posicion(entrada, e.vista);
      oss << "---> ERROR [" << l + 1 << " : " << c + 1 << "] | " << e.mensaje << " " << e.vista;
      error = std::move(oss).str();
      return error.c_str();
   }
}

#ifndef __EMSCRIPTEN__
   int main(int argc, char* argv[]){
      if(argc != 4 && argc != 5){
         std::cout << "Uso: " << argv[0] << " ruta_entrada version_origen version_destino [ruta_salida]\n";
         return 0;
      }

      std::ifstream f(argv[1]);
      if (!f.is_open( )) {
         std::cout << "---> ERROR: No se pudo abrir el archivo de entrada" << std::endl;
         return 0;
      }
      std::stringstream ss;
      f >> ss.rdbuf();

      std::string salida = traduce(std::move(ss).str().c_str(), argv[2], argv[3]);
      if (salida.starts_with("---> ERROR:")) {
         std::cout << salida << std::endl;
         return 0;
      }

      if (argc > 4) {
         std::ofstream f(argv[4]);
         if (!f.is_open()) {
            std::cout << "---> ERROR: No se pudo abrir el archivo de salida" << std::endl;
            return 0;
         }
         f << salida;
      } else {
         std::cout << salida;
      }
   }
#endif
