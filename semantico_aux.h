#ifndef SEMANTICO_AUX_H
#define SEMANTICO_AUX_H

#include "error.h"
#include "lexer.h"
#include "parser.h"
#include <charconv>
#include <iterator>
#include <string>
#include <vector>
#include <stddef.h>

enum tipo_evaluado {
   INT, BOOL
};

std::string toupper_str(const std::string_view& s) {
   std::string res;
   std::transform(s.begin( ), s.end( ), std::back_inserter(res), ::toupper);
   return res;
}

template<typename T, typename C>
bool inserta_simbolo(std::map<std::string, T*, C>& mapa, const std::string_view& s, T* v, bool sensible) {
   return (sensible ? mapa.emplace(s, v) : mapa.emplace(toupper_str(s), v)).second;
}

template<typename T, typename C>
void elimina_simbolo(std::map<std::string, T*, C>& mapa, const std::string_view& s, bool sensible) {
   auto iter = (sensible ? mapa.find(s) : mapa.find(toupper_str(s)));
   mapa.erase(iter);
}

template<typename T, typename C>
T* busca_simbolo(const std::map<std::string, T*, C>& mapa, const std::string_view& s, bool sensible) {
   auto iter = (sensible ? mapa.find(s) : mapa.find(toupper_str(s)));
   return (iter != mapa.end( ) ? iter->second : nullptr);
}

tipo_evaluado evalua(const std::unique_ptr<expresion>& ex, auto&... params) {
   if (auto p = dynamic_cast<const expresion_termino*>(ex.get( )); p != nullptr) {
      return evalua(p, params...);
   } else if (auto p = dynamic_cast<const expresion_binaria*>(ex.get( )); p != nullptr) {
      return evalua(p, params...);
   } else if (auto p = dynamic_cast<const expresion_prefija*>(ex.get( )); p != nullptr) {
      return evalua(p, params...);
   } else if (auto p = dynamic_cast<const expresion_llamada_nativa*>(ex.get( )); p != nullptr) {
      return evalua(p, params...);
   } else {
      return { };
   }
}

void evalua(const std::unique_ptr<sentencia>& s, auto&... params) {
   if (auto p = dynamic_cast<const sentencia_comando*>(s.get( )); p != nullptr) {
      return evalua(p, params...);
   } else if (auto p = dynamic_cast<const sentencia_if*>(s.get( )); p != nullptr) {
      return evalua(p, params...);
   } else if (auto p = dynamic_cast<const sentencia_while*>(s.get( )); p != nullptr) {
      return evalua(p, params...);
   } else if (auto p = dynamic_cast<const sentencia_iterate*>(s.get( )); p != nullptr) {
      return evalua(p, params...);
   } else if (auto p = dynamic_cast<const sentencia_llamada_usuario*>(s.get( )); p != nullptr) {
      return evalua(p, params...);
   } else if (auto p = dynamic_cast<const sentencia_bloque*>(s.get( )); p != nullptr) {
      return evalua(p, params...);
   } else if (auto p = dynamic_cast<const sentencia_vacia*>(s.get( )); p != nullptr) {
      return evalua(p, params...);
   }
}

#endif
