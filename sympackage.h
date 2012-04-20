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

#ifndef SYMPACKAGE_H
#define SYMPACKAGE_H

#include "symtable.h"

typedef struct sympackage {
  s_symtable static_table;
  s_symtable dynamic_table;
} s_sympackage;

void sympackage_init (s_sympackage *sp);
void sympackage_free (s_sympackage *sp);
t_sym sympackage_intern (s_sympackage *sp, const char *string);
t_sym sympackage_intern_n (s_sympackage *sp, const char *string, size_t len);
t_sym sympackage_intern_static (s_sympackage *sp, const char *string);

#endif
