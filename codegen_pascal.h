#ifndef CODEGEN_PASCAL_H
#define CODEGEN_PASCAL_H

#include "codegen.h"

struct codegen_pascal : codegen_base {
   using codegen_base::genera;
   mutable int nivel_ind = 1;

   codegen_pascal()
   : codegen_base(
      4,
   {
      {SAL_INS, "sal-de-instruccion"},
      {RETURN, "sal-de-instruccion"},
      {COMO, "como"},
      {APAGATE, "apagate"},
      {GIRA_IZQ, "gira-izquierda"},
      {AVANZA, "avanza"},
      {COGE_ZUM, "coge-zumbador"},
      {DEJA_ZUM, "deja-zumbador"},
      {PRECEDE, "precede"},
      {SUCEDE, "sucede"},
      {ES_CERO, "si-es-cero"},
      {FRENTE_LIB, "frente-libre"},
      {FRENTE_BLOQ, "frente-bloqueado"},
      {IZQUIERDA_LIB, "izquierda-libre"},
      {IZQUIERDA_BLOQ, "izquierda-bloqueda"},
      {DERECHA_LIB, "derecha-libre"},
      {DERECHA_BLOQ, "derecha-bloqueada"},
      {JUNTO_ZUM, "junto-a-zumbador"},
      {NJUNTO_ZUM, "no-junto-a-zumbador"},
      {ALGUN_ZUM_BAG, "algun-zumbador-en-la-mochila"},
      {NINGUN_ZUM_BAG, "ningun-zumbador-en-la-mochila"},
      {ORIENTADO_NORTE, "orientado-al-norte"},
      {ORIENTADO_SUR, "orientado-al-sur"},
      {ORIENTADO_ESTE, "orientado-al-este"},
      {ORIENTADO_OESTE, "orientado-al-oeste"},
      {NORIENTADO_NORTE, "no-orientado-al-norte"},
      {NORIENTADO_SUR, "no-orientado-al-sur"},
      {NORIENTADO_ESTE, "no-orientado-al-este"},
      {NORIENTADO_OESTE, "no-orientado-al-oeste"},
      {NOT, "no"},
      {OR, "o"},
      {AND, "y"},
   }) {
      return;
   }

   void genera(const expresion_termino* ex, std::ostream& os, const configuracion_id& config) const {
      os << traduce(ex->token, palabras, config);
   }

   void genera(const expresion_binaria* ex, std::ostream& os, const configuracion_id& config) const {
      os << "( ";
      genera(ex->izq, os, config);
      os << " " << palabras.find(ex->op.tipo)->second << " ";
      genera(ex->der, os, config);
      os << " )";
   }

   void genera(const expresion_prefija* ex, std::ostream& os, const configuracion_id& config) const {
      os << palabras.find(ex->op.tipo)->second << "( ";
      genera(ex->ex, os, config);
      os << " )";
   }

   void genera(const expresion_llamada_nativa* ex, std::ostream& os, const configuracion_id& config) const {
      os << palabras.find(ex->funcion.tipo)->second << "(";
      genera(ex->parametro, os, config);
      os << ")";
   }

   void genera(const sentencia_comando* s, std::ostream& os, const configuracion_id& config) const {
      auto itr = palabras.find(s->comando.tipo);
      os << printws(nivel_ind * tab) << itr->second << ";\n";
   }

   void genera(const sentencia_if* s, std::ostream& os, const configuracion_id& config) const {
      os << printws(nivel_ind++ * tab) << "si ";
      genera(s->condicion, os, config);
      os << " entonces inicio\n";
      genera(s->parte_si, os, config);
      os << printws(--nivel_ind * tab) << "fin";
      if(!s->parte_no.empty()){
         os << " sino inicio\n";
         ++nivel_ind;
         genera(s->parte_no, os, config);
         os << printws(--nivel_ind * tab) << "fin";
      }
      os << ";\n";
   }

   void genera(const sentencia_while* s, std::ostream& os, const configuracion_id& config) const {
      os << printws(nivel_ind++ * tab) << "mientras ";
      genera(s->condicion, os, config);
      os << " hacer inicio\n";
      genera(s->cuerpo, os, config);
      os << printws(--nivel_ind * tab) << "fin;\n";
   }

   void genera(const sentencia_iterate* s, std::ostream& os, const configuracion_id& config) const {
      os << printws(nivel_ind++ * tab) << "repetir ";
      genera(s->condicion, os, config);
      os << " veces inicio\n";
      genera(s->cuerpo, os, config);
      os << printws(--nivel_ind * tab) << "fin;\n";
   }

   void genera(const sentencia_llamada_usuario* s, std::ostream& os, const configuracion_id& config) const {
      os << printws(nivel_ind * tab) << traduce(s->funcion, palabras, config);
      if(s->parametro != nullptr){
         os << "(";
         genera(s->parametro, os, config);
         os << ")";
      }
      os << ";\n";
   }

   void genera(const sentencia_bloque* s, std::ostream& os, const configuracion_id& config) const {
      if(!s->cuerpo.empty()){
         genera(s->cuerpo, os, config);
      }
   }

   void genera(const sentencia_vacia* s, std::ostream& os, const configuracion_id& config) const {
      os << printws(nivel_ind * tab) << ";\n";
   }

   void genera(const arbol_sintactico& arbol, const tabla_simbolos& tabla, std::ostream& os, const configuracion_id& config) const {
      std::vector<const declaracion_funcion*> prototipar;
      std::set<const declaracion_funcion*> definidas;
      for (const auto& funcion : arbol.funciones) {
         auto iter = tabla.grafica_dependencias.find(&funcion);
         if (iter != tabla.grafica_dependencias.end( )) {
            definidas.insert(&funcion);
            for (auto llamada : iter->second) {
               if (!definidas.contains(llamada)) {
                  prototipar.push_back(llamada);
               }
            }
         }
      }
      std::sort(prototipar.begin( ), prototipar.end( ));
      prototipar.erase(std::unique(prototipar.begin( ), prototipar.end( )), prototipar.end( ));

      os << "iniciar-programa\n";
      for (auto decl : prototipar) {
         os << printws(nivel_ind * tab) << "define-prototipo-instruccion " << traduce(decl->nombre, palabras, config) << (decl->parametro ? "(" + traduce(*(decl->parametro), palabras, config) + ")": "") << ";\n";
      }

      os << "\n";
      for (const auto& funcion : arbol.funciones) {
         if(funcion.cuerpo != nullptr){
            os << printws(nivel_ind++ * tab) << "define-nueva-instruccion " << traduce(funcion.nombre, palabras, config) << (funcion.parametro ? "(" + traduce(*(funcion.parametro), palabras, config) + ")": "") << " como inicio\n";
            genera(*funcion.cuerpo, os, config);
            os << printws(--nivel_ind * tab) << "fin;\n\n";
         }
      }

      os << printws(nivel_ind++ * tab) << "inicia-ejecucion\n";
      genera(arbol.mains[0], os, config);
      os << printws(--nivel_ind * tab) << "termina-ejecucion\n";
      os << "finalizar-programa\n";
   }

   std::string ajusta_id(const std::string& s) const {
      return s;
   }

   void agrega_posfijo(std::string& s) const {
      s += '_';
   }
};

#endif
