#ifndef PARSER_H
#define PARSER_H

#include "parser_aux.h"
#include "lexer.h"
#include <memory>
#include <string_view>
#include <utility>
#include <vector>

struct control_vista {
   const token_registrado* ini;
   const token_registrado*& ref;

   control_vista(const token_registrado*& p)
   : ini(p), ref(p) {
   }

   explicit operator std::string_view( ) const {
      return { ini->vista.begin( ), size_t(ini == ref ? 0 : (ref - 1)->vista.end( ) - ini->vista.begin( )) };
   }
};

// tipos de datos para expresiones

struct expresion {
   std::string_view vista;

   expresion(const control_vista& cv)
   : vista(cv) {
   }

   virtual ~expresion() = default;
};

struct expresion_termino : expresion {
   const token_registrado& token;

   expresion_termino(const control_vista& cv, const token_registrado& t)
   : expresion(cv), token(t) {
   }

};

struct expresion_binaria : expresion {
   std::unique_ptr<expresion> izq;
   const token_registrado& op;
   std::unique_ptr<expresion> der;

   expresion_binaria(const control_vista& cv, std::unique_ptr<expresion> i, const token_registrado& p, std::unique_ptr<expresion> d)
   : expresion(cv), izq(std::move(i)), op(p), der(std::move(d)) {
   }
};

struct expresion_prefija : expresion {
   const token_registrado& op;
   std::unique_ptr<expresion> ex;

   expresion_prefija(const control_vista& cv, const token_registrado& p, std::unique_ptr<expresion> e)
   : expresion(cv), op(p), ex(std::move(e)) {
   }

};

struct expresion_llamada_nativa : expresion {
   const token_registrado& funcion;
   std::unique_ptr<expresion> parametro;

   expresion_llamada_nativa(const control_vista& cv, const token_registrado& f, std::unique_ptr<expresion> p)
   : expresion(cv), funcion(f), parametro(std::move(p)) {
   }
};

// tipos para sentencias

struct sentencia {
   std::string_view vista;

   sentencia(const control_vista& cv)
   : vista(cv) {
   }

   virtual ~sentencia() = default;
};

struct sentencia_comando : sentencia {
   const token_registrado& comando;
   sentencia_comando(const control_vista& cv, const token_registrado& t)
   : sentencia(cv), comando(t) {
   }

};

struct sentencia_if : sentencia {
   std::unique_ptr<expresion> condicion;
   std::vector<std::unique_ptr<sentencia>> parte_si;
   std::vector<std::unique_ptr<sentencia>> parte_no;

   sentencia_if(const control_vista& cv, std::unique_ptr<expresion> c, std::vector<std::unique_ptr<sentencia>> s, std::vector<std::unique_ptr<sentencia>> n)
   : sentencia(cv), condicion(std::move(c)), parte_si(std::move(s)), parte_no(std::move(n)) {
   }

};

struct sentencia_while : sentencia {
   std::unique_ptr<expresion> condicion;
   std::vector<std::unique_ptr<sentencia>> cuerpo;

   sentencia_while(const control_vista& cv, std::unique_ptr<expresion> c, std::vector<std::unique_ptr<sentencia>> v)
   : sentencia(cv), condicion(std::move(c)), cuerpo(std::move(v)) {
   }
};

struct sentencia_iterate : sentencia{
   std::unique_ptr<expresion> condicion;
   std::vector<std::unique_ptr<sentencia>> cuerpo;

   sentencia_iterate(const control_vista& cv, std::unique_ptr<expresion> c, std::vector<std::unique_ptr<sentencia>> v)
   : sentencia(cv), condicion(std::move(c)), cuerpo(std::move(v)) {
   }
};

struct sentencia_llamada_usuario : sentencia {
   const token_registrado& funcion;
   std::unique_ptr<expresion> parametro;

   sentencia_llamada_usuario(const control_vista& cv, const token_registrado& f, std::unique_ptr<expresion> p)
   : sentencia(cv), funcion(f), parametro(std::move(p)) {
   }
};

struct sentencia_return : sentencia {
   sentencia_return(const control_vista& cv)
   : sentencia(cv) {
   }
};

// tipos para árbol sintáctico

struct declaracion_funcion {
   const token_registrado& nombre;
   std::unique_ptr<expresion> parametro;
   std::vector<std::unique_ptr<sentencia>> cuerpo;
};

struct declaracion_prototipo {
   const token_registrado& nombre;
   std::unique_ptr<expresion> parametro;
};

struct arbol_sintactico {
   std::vector<std::vector<std::unique_ptr<sentencia>>> mains;
   std::vector<declaracion_funcion> funciones;
   std::vector<declaracion_prototipo> prototipos;
};

// parser base y algoritmo principal

class parser_base {
public:
   virtual ~parser_base( ) = default;
   virtual void parser_arbol(const token_registrado*&, arbol_sintactico&) const = 0;
};

arbol_sintactico parser(const parser_base& pb, const std::vector<token_registrado>& tokens) {
   auto p = &tokens[0];
   arbol_sintactico arbol;
   pb.parser_arbol(p, arbol);
   espera(p, FIN_ARCHIVO);
   return arbol;
}

#endif
