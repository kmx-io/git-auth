/*
 * git-auth  -  restrict git commands
 * Copyright 2012 Thomas de Grivel <billitch@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdlib.h>

#include "symbol.h"

typedef struct symtable {
  size_t count;
  size_t size;
  t_sym *sym;
} s_symtable;

void symtable_init (s_symtable *st);
s_symtable * symtable_copy (s_symtable *dest, s_symtable *st);
void symtable_free (s_symtable *st);
void symtable_free_all (s_symtable *st);
void symtable_enlarge (s_symtable *st);
t_sym symtable_find (s_symtable *st, const char *string);
t_sym symtable_find_n (s_symtable *st, const char *string, size_t len);
t_sym symtable_add (s_symtable *st, t_sym sym);
t_sym symtable_intern (s_symtable *st, const char *string);

#endif
