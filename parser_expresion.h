#ifndef PARSER_EXPRESION_H
#define PARSER_EXPRESION_H

#include "error.h"
#include "lexer.h"
#include "parser_aux.h"
#include <memory>
#include <utility>
#include <vector>

struct expresion {
   std::string_view vista;

   expresion(const control_vista& cv)
   : vista(cv) {
   }

   virtual ~expresion( ) = 0;
};

struct expresion_termino : expresion {
   const token_anotado& token;

   expresion_termino(const control_vista& cv, const token_anotado& t)
   : expresion(cv), token(t) {
   }
};

struct expresion_binaria : expresion {
   std::unique_ptr<expresion> izq;
   const token_anotado& op;
   std::unique_ptr<expresion> der;

   expresion_binaria(const control_vista& cv, std::unique_ptr<expresion> i, const token_anotado& p, std::unique_ptr<expresion> d)
   : expresion(cv), izq(std::move(i)), op(p), der(std::move(d)) {
   }
};

struct expresion_prefija : expresion {
   const token_anotado& op;
   std::unique_ptr<expresion> ex;

   expresion_prefija(const control_vista& cv, const token_anotado& p, std::unique_ptr<expresion> e)
   : expresion(cv), op(p), ex(std::move(e)) {
   }
};

struct expresion_llamada : expresion {
   const token_anotado& funcion;
   std::vector<std::unique_ptr<expresion>> parametros;

   expresion_llamada(const control_vista& cv, const token_anotado& f, std::vector<std::unique_ptr<expresion>> p)
   : expresion(cv), funcion(f), parametros(std::move(p)) {
   }
};

std::unique_ptr<expresion> expr(const token_anotado*&);
std::unique_ptr<expresion> expr_primaria(const token_anotado*&);
std::unique_ptr<expresion> expr_unaria(const token_anotado*&);
std::unique_ptr<expresion> expr_n_aria(const token_anotado*&, int);

std::vector<std::unique_ptr<expresion>> lista_expr(const token_anotado*& p) {
   // ...
}

std::unique_ptr<expresion> expr_primaria(const token_anotado*& p) {
   //...
}

std::unique_ptr<expresion> expr_unaria(const token_anotado*& p) {
   //...
}

std::unique_ptr<expresion> expr_n_aria(const token_anotado*& p, int prec) {
   control_vista cv(p);
   auto ex = expr_unaria(p);
   while (es_operador_binario(p->tipo) && precedencia(p->tipo) >= prec) {
      auto operador = p;
      ex = std::make_unique<expresion_binaria>(cv, std::move(ex), *operador , expr_n_aria(++p, precedencia(operador->tipo) + asociatividad(operador->tipo)));
   }
   return ex;
}

std::unique_ptr<expresion> expr(const token_anotado*& p) {
   return expr_n_aria(p, 0);
}

#endif
