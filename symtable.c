
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
#include <err.h>

#include "symtable.h"

void symtable_init (s_symtable *st)
{
  assert(st);
  st->count = 0;
  st->size = 128;
  if (!(st->sym = calloc(st->size, sizeof(t_sym))))
    err(10, "symtable_init calloc");
}

s_symtable * symtable_copy (s_symtable *dest, s_symtable *st)
{
  assert(dest);
  assert(st);
  dest->count = st->count;
  dest->size = st->size;
  if (!(dest->sym = calloc(dest->size, sizeof(t_sym))))
    err(10, "symtable_copy calloc");
  return dest;
}

void symtable_free (s_symtable *st)
{
  assert(st);
  free(st->sym);
  bzero(st, sizeof(s_symtable));
}

void symtable_free_all (s_symtable *st)
{
  assert(st);
  {
    int i = st->count;
    while (i--)
      free((void*)st->sym[i]);
  }
  free(st->sym);
  bzero(st, sizeof(s_symtable));
}

void symtable_enlarge (s_symtable *st)
{
  assert(st);
  st->size += st->size < 1024 ? st->size : 1024;
  if (!(st->sym = realloc(st->sym, st->size)))
    err(10, "symtable realloc");
  assert(st->size > st->count);
}

t_sym symtable_find (s_symtable *st, const char *string)
{
  assert(st);
  {
    t_sym *s = st->sym;
    int i = st->count;
    while (i--) {
      if ((*s == string) || (string && !strcmp(string, *s)))
	return *s;
      s++;
    }
  }
  return NULL;
}

static int strcmp_n (const char *a, const char *b, size_t len_b)
{
  const unsigned char *aa = (const unsigned char *) a;
  const unsigned char *bb = (const unsigned char *) b;
  while (len_b && *aa == *bb && *aa) {
    aa++;
    bb++;
    len_b--;
  }
  if ((!*aa && !len_b) || (*aa == *bb))
    return 0;
  if (!*aa || *aa < *bb)
    return -1;
  return 1;
}

t_sym symtable_find_n (s_symtable *st, const char *string, size_t len)
{
  assert(st);
  {
    t_sym *s = st->sym;
    int i = st->count;
    while (i--) {
      if (*s == string || (string && len && !strcmp_n(*s, string, len)))
	return *s;
      s++;
    }
  }
  return NULL;
}

t_sym symtable_add (s_symtable *st, t_sym sym)
{
  assert(st);
  if (st->count == st->size)
    symtable_enlarge(st);
  return st->sym[st->count++] = sym;
}

t_sym symtable_intern (s_symtable *st, const char *string)
{
  t_sym s = symtable_find(st, string);
  if (!s)
    s = symtable_add(st, string);
  return s;
}
