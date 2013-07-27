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

#include <assert.h>
#include <string.h>

#include "sympackage.h"

void sympackage_init (s_sympackage *sp)
{
  assert(sp);
  symtable_init(&sp->static_table);
  symtable_init(&sp->dynamic_table);
}

void sympackage_free (s_sympackage *sp)
{
  assert(sp);
  symtable_free(&sp->static_table);
  symtable_free_all(&sp->dynamic_table);
}

t_sym sympackage_intern (s_sympackage *sp, const char *string)
{
  t_sym s;
  assert(sp);
  s = symtable_find(&sp->static_table, string);
  if (!s)
    s = symtable_find(&sp->dynamic_table, string);
  if (!s)
    s = symtable_add(&sp->dynamic_table, string_clone(string));
  return s;
}

t_sym sympackage_intern_n (s_sympackage *sp, const char *string, size_t len)
{
  t_sym s;
  assert(sp);
  s = symtable_find_n(&sp->static_table, string, len);
  if (!s)
    s = symtable_find_n(&sp->dynamic_table, string, len);
  if (!s)
    s = symtable_add(&sp->dynamic_table, string_clone_n(string, len));
  return s;
}

t_sym sympackage_intern_static (s_sympackage *sp, const char *string)
{
  t_sym s;
  assert(sp);
  s = symtable_find(&sp->static_table, string);
  if (!s)
    s = symtable_add(&sp->static_table, string);
  return s;
}
