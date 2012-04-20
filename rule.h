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

#ifndef RULE_H
#define RULE_H

#include <stdlib.h>

#include "symtable.h"

typedef s_symtable s_rule;

#define rule_init(r) symtable_init(r)
#define rule_free symtable_free
#define rule_free_all symtable_free_all
#define rule_add(r, s) symtable_add(r, s)

typedef struct rules {
  size_t count;
  size_t size;
  s_rule *rule;
} s_rules;

void rules_init (s_rules *rr);
void rules_free (s_rules *rr);
void rules_enlarge (s_rules *rr);
const s_rule * rules_add (s_rules *rr, const s_rule *r);

#endif
