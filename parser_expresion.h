#ifndef PARSER_EXPRESION_H
#define PARSER_EXPRESION_H

#include "error.h"
#include "lexer.h"
#include "parser.h"
#include "parser_aux.h"

std::unique_ptr<expresion> expr(const token_registrado*&);
std::unique_ptr<expresion> expr_primaria(const token_registrado*&);
std::unique_ptr<expresion> expr_unaria(const token_registrado*&);
std::unique_ptr<expresion> expr_n_aria(const token_registrado*&, int);

std::unique_ptr<expresion> expr_primaria(const token_registrado*& p) {
   control_vista cv(skipws(p));
   if (es_funcion_nativa(p->tipo)) {
      auto funcion = p++;
      espera(skipws(p), PARENTESIS_IZQ);
      auto param = expr(p);
      espera(skipws(p), PARENTESIS_DER);
      return std::make_unique<expresion_llamada_nativa>(cv, *funcion, std::move(param));
   } else if (es_termino(p->tipo)) {
      return std::make_unique<expresion_termino>(cv, *p++);
   } else if (p->tipo == PARENTESIS_IZQ) {
      auto ex = expr(++p);
      espera(skipws(p), PARENTESIS_DER);
      return ex;
   } else {
      throw error("Expresion esperada", p->vista);
   }
}

std::unique_ptr<expresion> expr_unaria(const token_registrado*& p) {
   control_vista cv(skipws(p));
   if (es_operador_prefijo(p->tipo)) {
      auto operador = p;
      return std::make_unique<expresion_prefija>(cv, *operador, expr_unaria(++p));
   } else {
      return expr_primaria(p);
   }
}

std::unique_ptr<expresion> expr_n_aria(const token_registrado*& p, int prec) {
   control_vista cv(skipws(p));
   auto ex = expr_unaria(p);
   while (es_operador_binario(skipws(p)->tipo) && precedencia(p->tipo) >= prec) {
      auto operador = p;
      ex = std::make_unique<expresion_binaria>(cv, std::move(ex), *operador , expr_n_aria(++p, precedencia(operador->tipo) + asociatividad(operador->tipo)));
   }
   return ex;
}

std::unique_ptr<expresion> expr(const token_registrado*& p) {
   return expr_n_aria(p, 0);
}

#endif
