#ifndef CODEGEN_AUX_H
#define CODEGEN_AUX_H

#include "error.h"
#include "lexer.h"
#include "parser.h"

std::string printws(int ws){
   std::string ans{};
   while(ws--){
      ans += " ";
   }
   return ans;
}

std::string mejora_id(const std::string& org, const token_registrado& tg, const std::map<tipo_token, std::string_view>& palabras){
   std::string cp = std::string(tg.vista);
   std::replace_if(cp.begin(), cp.end(), [&](const char& c){
         return (c == '_');
      }, 'X');
   
   if(tg.tipo == IDENTIFICADOR){
      return (org == "pascal" ? toupper_str(cp) : cp + "XDD");
   }else if(tg.tipo == LITERAL_ENTERA){
      return cp;
   }else{
      return std::string{palabras.find(tg.tipo)->second};
   }
}

void genera(const std::string& org, const std::unique_ptr<expresion>& ex, auto&... params) {
   if (auto p = dynamic_cast<const expresion_termino*>(ex.get( )); p != nullptr) {
      return genera(org, p, params...);
   } else if (auto p = dynamic_cast<const expresion_binaria*>(ex.get( )); p != nullptr) {
      return genera(org, p, params...);
   } else if (auto p = dynamic_cast<const expresion_prefija*>(ex.get( )); p != nullptr) {
      return genera(org, p, params...);
   } else if (auto p = dynamic_cast<const expresion_llamada_nativa*>(ex.get( )); p != nullptr) {
      return genera(org, p, params...);
   } else {
      return;
   }
}

void genera(const std::string& org, const std::unique_ptr<sentencia>& s, auto&... params) {
   if (auto p = dynamic_cast<const sentencia_comando*>(s.get( )); p != nullptr) {
      return genera(org, p, params...);
   } else if (auto p = dynamic_cast<const sentencia_if*>(s.get( )); p != nullptr) {
      return genera(org, p, params...);
   } else if (auto p = dynamic_cast<const sentencia_while*>(s.get( )); p != nullptr) {
      return genera(org, p, params...);
   } else if (auto p = dynamic_cast<const sentencia_iterate*>(s.get( )); p != nullptr) {
      return genera(org, p, params...);
   } else if (auto p = dynamic_cast<const sentencia_llamada_usuario*>(s.get( )); p != nullptr) {
      return genera(org, p, params...);
   } else if (auto p = dynamic_cast<const sentencia_bloque*>(s.get( )); p != nullptr) {
      return genera(org, p, params...);
   } else if (auto p = dynamic_cast<const sentencia_vacia*>(s.get( )); p != nullptr) {
      return genera(org, p, params...);
   } else {
      return;
   }
}

#endif
