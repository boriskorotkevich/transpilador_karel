#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include "lexer.h"
#include "parser_pascal.h"

std::ostream& operator<<(std::ostream& os, const token_registrado& tr){
   os << tr.tipo << " " << tr.vista;
   return os;
}

std::ostream& operator<<(std::ostream& os, const std::unique_ptr<expresion>& e){
   if(auto ans = dynamic_cast<const expresion_termino*>(e.get()); ans != nullptr){
      return os << ans->token.vista;
   }else if(auto ans = dynamic_cast<const expresion_binaria*>(e.get()); ans != nullptr){
      return os << "(" << ans->izq << " " << ans->op.vista << " " << ans->der << ")";
   }else if(auto ans = dynamic_cast<const expresion_prefija*>(e.get()); ans != nullptr){
      return os << ans->op.vista << "(" << ans->ex << ")";
   }else if(auto ans = dynamic_cast<const expresion_llamada_nativa*>(e.get()); ans != nullptr){
      return os << ans->funcion.vista << "(" << ans->parametro << ")";
   }else{
      return os;
   }
}

std::ostream& operator<<(std::ostream& os, const std::unique_ptr<sentencia>& s);

std::ostream& operator<<(std::ostream& os, const std::vector<std::unique_ptr<sentencia>>& s){
   for(const auto& sentencia : s){
      os << sentencia << std::endl;
   }
   return os;
}

std::ostream& operator<<(std::ostream& os, const std::unique_ptr<sentencia>& s){
   if(auto ans = dynamic_cast<const sentencia_comando*>(s.get()); ans != nullptr){
      return os << ans->comando.vista << "();";
   }else if(auto ans = dynamic_cast<const sentencia_if*>(s.get()); ans != nullptr){
      os << "if(" << ans->condicion << "){" << std::endl << ans->parte_si << "}";
      if(!ans->parte_no.empty()){
         os << "else{" << std::endl << ans->parte_no << "}";
      }
      return os;
   }else if(auto ans = dynamic_cast<const sentencia_while*>(s.get()); ans != nullptr){
      return os << "while(" << ans->condicion << "){" << std::endl << ans->cuerpo << "}";
   }else if(auto ans = dynamic_cast<const sentencia_iterate*>(s.get()); ans != nullptr){
      return os << "iterate(" << ans->condicion << "){" << std::endl << ans->cuerpo << "}";
   }else if(auto ans = dynamic_cast<const sentencia_llamada_usuario*>(s.get()); ans != nullptr){
      return os << ans->funcion.vista << "(" << ans->parametro << ");";
   }else if(auto ans = dynamic_cast<const sentencia_bloque*>(s.get()); ans != nullptr){
      return os << "{" << std::endl << ans->cuerpo << "}";
   }else if(auto ans = dynamic_cast<const sentencia_vacia*>(s.get()); ans != nullptr){
      return os << ";";
   }else{
      return os;
   }
}

std::ostream& operator<<(std::ostream& os, const declaracion_funcion& decl_fun){
   os << "void " << decl_fun.nombre.vista << "(";
   if (decl_fun.parametro != nullptr) {
      os << decl_fun.parametro->vista;
   }
   os << "){" << std::endl << decl_fun.cuerpo << "}" << std::endl;
   return os;
}

std::ostream& operator<<(std::ostream& os, const declaracion_prototipo& decl_fun){
   os << "void " << decl_fun.nombre.vista << "(";
   if (decl_fun.parametro != nullptr) {
      os << decl_fun.parametro->vista;
   }
   os << ");" << std::endl;
   return os;
}

#endif
