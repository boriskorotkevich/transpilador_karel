#ifndef CODEGEN_AUX_H
#define CODEGEN_AUX_H

#include "error.h"
#include "lexer.h"
#include "parser.h"

std::string printws(int ws){
   return std::string(ws, ' ');
}

std::string mejora_id(const token_registrado& t, const std::map<tipo_token, std::string_view>& palabras, const std::string_view& origen){
   std::string cp = std::string(t.vista);
   std::replace_if(cp.begin(), cp.end(), [&](const char& c){
      return (c == '_');
   }, 'X');

   if(t.tipo == IDENTIFICADOR){
      return (origen == "pascal" ? toupper_str(cp) : cp + "XDD");
   }else if(t.tipo == LITERAL_ENTERA){
      return cp;
   }else{
      return std::string{palabras.find(t.tipo)->second};
   }
}

void genera(const std::unique_ptr<expresion>& ex, auto&... params) {
   if (auto p = dynamic_cast<const expresion_termino*>(ex.get( )); p != nullptr) {
      return genera(p, params...);
   } else if (auto p = dynamic_cast<const expresion_binaria*>(ex.get( )); p != nullptr) {
      return genera(p, params...);
   } else if (auto p = dynamic_cast<const expresion_prefija*>(ex.get( )); p != nullptr) {
      return genera(p, params...);
   } else if (auto p = dynamic_cast<const expresion_llamada_nativa*>(ex.get( )); p != nullptr) {
      return genera(p, params...);
   } else {
      return;
   }
}

void genera(const std::unique_ptr<sentencia>& s, auto&... params) {
   if (auto p = dynamic_cast<const sentencia_comando*>(s.get( )); p != nullptr) {
      return genera(p, params...);
   } else if (auto p = dynamic_cast<const sentencia_if*>(s.get( )); p != nullptr) {
      return genera(p, params...);
   } else if (auto p = dynamic_cast<const sentencia_while*>(s.get( )); p != nullptr) {
      return genera(p, params...);
   } else if (auto p = dynamic_cast<const sentencia_iterate*>(s.get( )); p != nullptr) {
      return genera(p, params...);
   } else if (auto p = dynamic_cast<const sentencia_llamada_usuario*>(s.get( )); p != nullptr) {
      return genera(p, params...);
   } else if (auto p = dynamic_cast<const sentencia_bloque*>(s.get( )); p != nullptr) {
      return genera(p, params...);
   } else if (auto p = dynamic_cast<const sentencia_vacia*>(s.get( )); p != nullptr) {
      return genera(p, params...);
   } else {
      return;
   }
}

#endif
