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

std::unique_ptr<expresion> expr(const token_registrado*&);
std::unique_ptr<expresion> expr_primaria(const token_registrado*&);
std::unique_ptr<expresion> expr_unaria(const token_registrado*&);
std::unique_ptr<expresion> expr_n_aria(const token_registrado*&, int);

std::unique_ptr<expresion> expr_primaria(const token_registrado*& p) {
   control_vista cv(p);
   if (es_funcion_nativa(p->tipo)) {
      auto funcion = p;
      auto param = expr(p += 2);
      espera(p, PARENTESIS_DER);
      return std::make_unique<expresion_llamada_nativa>(cv, *funcion, std::move(param));
   } else if (es_termino(p->tipo)) {
      return std::make_unique<expresion_termino>(cv, *p++);
   } else if (p->tipo == PARENTESIS_IZQ) {
      auto ex = expr(++p);
      espera(p, PARENTESIS_DER);
      return ex;
   } else {
      throw error("Expresion esperada", p->vista);
   }
}

std::unique_ptr<expresion> expr_unaria(const token_registrado*& p) {
   control_vista cv(p);
   if (es_operador_prefijo(p->tipo)) {
      auto operador = p;
      return std::make_unique<expresion_prefija>(cv, *operador, expr_unaria(++p));
   } else {
      return expr_primaria(p);
   }
}

std::unique_ptr<expresion> expr_n_aria(const token_registrado*& p, int prec) {
   control_vista cv(p);
   auto ex = expr_unaria(p);
   while (es_operador_binario(p->tipo) && precedencia(p->tipo) >= prec) {
      auto operador = p;
      ex = std::make_unique<expresion_binaria>(cv, std::move(ex), *operador , expr_n_aria(++p, precedencia(operador->tipo) + asociatividad(operador->tipo)));
   }
   return ex;
}

std::unique_ptr<expresion> expr(const token_registrado*& p) {
   return expr_n_aria(p, 0);
}

#endif