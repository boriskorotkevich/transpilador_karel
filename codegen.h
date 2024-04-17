#ifndef CODEGEN_H
#define CODEGEN_H

#include "codegen_aux.h"
#include "lexer.h"
#include "parser.h"
#include "semantico.h"

#include <map>
#include <sstream>

struct configuracion_id {
   std::string_view origen;
   std::map<std::string, std::string> traduccion;
};

struct codegen_base {
   protected:
      int tab;

   public:
      const std::map<tipo_token, std::string_view> palabras, simbolos;

      codegen_base(int t, std::map<tipo_token, std::string_view>&& p, std::map<tipo_token, std::string_view>&& s)
      : tab(t), palabras(std::move(p)), simbolos(std::move(s)){
      }

      codegen_base(int t, std::map<tipo_token, std::string_view>&& p)
      : tab(t), palabras(std::move(p)){
      }

      void genera(const std::vector<std::unique_ptr<sentencia>>& vs, std::ostream& os, const configuracion_id& config) const {
         for(const auto& sentencia : vs){
            genera(sentencia, os, config);
         }
      }

      void genera(const std::unique_ptr<expresion>& ex, auto&... params) const {
         ::genera(ex, *this, params...);
      }

      void genera(const std::unique_ptr<sentencia>& s, auto&... params) const {
         ::genera(s, *this, params...);
      }

      virtual void genera(const expresion_termino*, std::ostream&, const configuracion_id&) const = 0;
      virtual void genera(const expresion_binaria*, std::ostream&, const configuracion_id&) const = 0;
      virtual void genera(const expresion_prefija*, std::ostream&, const configuracion_id&) const = 0;
      virtual void genera(const expresion_llamada_nativa*, std::ostream&, const configuracion_id&) const = 0;

      virtual void genera(const sentencia_comando*, std::ostream&, const configuracion_id&) const = 0;
      virtual void genera(const sentencia_if*, std::ostream&, const configuracion_id&) const = 0;
      virtual void genera(const sentencia_while*, std::ostream&, const configuracion_id&) const = 0;
      virtual void genera(const sentencia_iterate*, std::ostream&, const configuracion_id&) const = 0;
      virtual void genera(const sentencia_llamada_usuario*, std::ostream&, const configuracion_id&) const = 0;
      virtual void genera(const sentencia_bloque*, std::ostream&, const configuracion_id&) const = 0;
      virtual void genera(const sentencia_vacia*, std::ostream&, const configuracion_id&) const = 0;

      virtual void genera(const arbol_sintactico&, const tabla_simbolos&, std::ostream&, const configuracion_id&) const = 0;
      virtual void ajusta_id(std::string&, bool) const = 0;
};

template<typename T>
void genera(const T* p, const codegen_base& cb, std::ostream& os, const configuracion_id& config) {
   cb.genera(p, os, config);
}

std::string traduce(const token_registrado& t, const std::map<tipo_token, std::string_view>& palabras, const configuracion_id& config){
   if(t.tipo == IDENTIFICADOR){
      if (config.origen == "pascal") {
         return config.traduccion.find(toupper_str(t.vista))->second;
      } else {
         return config.traduccion.find(std::string(t.vista))->second;
      }
   }else if(t.tipo == LITERAL_ENTERA){
      return std::string(t.vista);
   }else{
      return std::string{palabras.find(t.tipo)->second};
   }
}

std::string codegen(const codegen_base& cb, const std::vector<token_registrado>& tokens, const arbol_sintactico& arbol, const tabla_simbolos& tabla, const std::string_view& origen) {
   std::set<std::string> ocupados;
   for(const auto [t, sv] : cb.palabras) {
      ocupados.insert(std::string(sv));
   }

   configuracion_id config = { origen };
   for(const token_registrado& tr : tokens){
      std::string inicial = (origen == "pascal" ? toupper_str(std::string(tr.vista)) : std::string(tr.vista));
      if (!config.traduccion.contains(inicial)) {
         std::string convertido = inicial;
         cb.ajusta_id(convertido, true);
         while (!ocupados.insert(convertido).second) {
            cb.ajusta_id(convertido, false);
         }
         config.traduccion[inicial] = convertido;
      }
   }

   std::ostringstream os;
   cb.genera(arbol, tabla, os, config);
   return std::move(os).str( );
}

#endif
