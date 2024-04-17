#ifndef CODEGEN_JAVA_H
#define CODEGEN_JAVA_H

#include "codegen.h"

struct codegen_java : codegen_base {
   using codegen_base::genera;
   mutable int nivel_ind = 1;

   codegen_java()
   : codegen_base(
      3,
   {
      {RETURN, "return"},
      {SAL_INS, "return"},
      {APAGATE, "turnoff"},
      {GIRA_IZQ, "turnleft"},
      {AVANZA, "move"},
      {COGE_ZUM, "pickbeeper"},
      {DEJA_ZUM, "putbeeper"},
      {PRECEDE, "pred"},
      {SUCEDE, "succ"},
      {ES_CERO, "iszero"},
      {FRENTE_LIB, "frontIsClear"},
      {FRENTE_BLOQ, "frontIsBlocked"},
      {IZQUIERDA_LIB, "leftIsClear"},
      {IZQUIERDA_BLOQ, "leftIsBlocked"},
      {DERECHA_LIB, "rightIsClear"},
      {DERECHA_BLOQ, "rightIsBlocked"},
      {JUNTO_ZUM, "nextToABeeper"},
      {NJUNTO_ZUM, "notNextToABeeper"},
      {ALGUN_ZUM_BAG, "anyBeepersInBeeperBag"},
      {NINGUN_ZUM_BAG, "noBeepersInBeeperBag"},
      {ORIENTADO_NORTE, "facingNorth"},
      {ORIENTADO_SUR, "facingSouth"},
      {ORIENTADO_ESTE, "facingEast"},
      {ORIENTADO_OESTE, "facingWest"},
      {NORIENTADO_NORTE, "notFacingNorth"},
      {NORIENTADO_SUR, "notFacingSouth"},
      {NORIENTADO_ESTE, "notFacingEast"},
      {NORIENTADO_OESTE, "notFacingWest"}
   },
   /* simbolos */
   {
      {NOT, "!"},
      {OR, "||"},
      {AND, "&&"}
   }) {
      return;
   }

   void genera(const expresion_termino* ex, std::ostream& os, const configuracion_id& config) const {
      os << traduce(ex->token, palabras, config);
   }

   void genera(const expresion_binaria* ex, std::ostream& os, const configuracion_id& config) const {
      os << "( ";
      genera(ex->izq, os, config);
      os << " " << simbolos.find(ex->op.tipo)->second << " ";
      genera(ex->der, os, config);
      os << " )";
   }

   void genera(const expresion_prefija* ex, std::ostream& os, const configuracion_id& config) const {
      os << simbolos.find(ex->op.tipo)->second << "( ";
      genera(ex->ex, os, config);
      os << " )";
   }

   void genera(const expresion_llamada_nativa* ex, std::ostream& os, const configuracion_id& config) const {
      os << palabras.find(ex->funcion.tipo)->second << "(";
      genera(ex->parametro, os, config);
      os << ")";
   }

   void genera(const sentencia_comando* s, std::ostream& os, const configuracion_id& config) const {
      os << printws(nivel_ind * tab) << palabras.find(s->comando.tipo)->second << "();\n";
   }

   void genera(const sentencia_if* s, std::ostream& os, const configuracion_id& config) const {
      os << printws(nivel_ind++ * tab) << "if(";
      genera(s->condicion, os, config);
      os << "){\n";
      genera(s->parte_si, os, config);
      os << printws(--nivel_ind * tab) << "}";
      if(!s->parte_no.empty()){
         os << "else{\n";
         ++nivel_ind;
         genera(s->parte_no, os, config);
         os << printws(--nivel_ind * tab) << "}";
      }
      os << "\n";
   }

   void genera(const sentencia_while* s, std::ostream& os, const configuracion_id& config) const {
      os << printws(nivel_ind++ * tab) << "while(";
      genera(s->condicion, os, config);
      os << "){\n";
      genera(s->cuerpo, os, config);
      os << printws(--nivel_ind * tab) << "}\n";
   }

   void genera(const sentencia_iterate* s, std::ostream& os, const configuracion_id& config) const {
      os << printws(nivel_ind++ * tab) << "iterate(";
      genera(s->condicion, os, config);
      os << "){\n";
      genera(s->cuerpo, os, config);
      os << printws(--nivel_ind * tab) << "}\n";
   }

   void genera(const sentencia_llamada_usuario* s, std::ostream& os, const configuracion_id& config) const {
      os << printws(nivel_ind * tab) << traduce(s->funcion, palabras, config) << "(";
      if(s->parametro != nullptr){
         genera(s->parametro, os, config);
      }
      os << ");\n";
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
      os << "class program {\n";
      for (const auto& funcion : arbol.funciones) {
         if(funcion.cuerpo != nullptr){
            os << printws(nivel_ind++ * tab) << "void " << traduce(funcion.nombre, palabras, config) << "(" << (funcion.parametro ? traduce(*(funcion.parametro), palabras, config) : "") << "){\n";
            genera(*funcion.cuerpo, os, config);
            os << printws(--nivel_ind * tab) << "}\n\n";
         }
      }

      os << printws(nivel_ind++ * tab) << "program(){\n";
      genera(arbol.mains[0], os, config);
      os << printws(--nivel_ind * tab) << "}\n";
      os << "}\n";
   }

   std::string ajusta_id(const std::string& s) const {
      std::string res = s;
      std::replace(res.begin( ), res.end( ), '-', '_');
      if (res[0] == '_') {
         res.erase(res.begin( ));
      }
      return res;
   }

   void agrega_posfijo(std::string& s) const {
      s += '_';
   }
};

#endif
