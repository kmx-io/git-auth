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

#include "rule.h"

void rules_init (s_rules *rr)
{
  assert(rr);
  rr->count = 0;
  rr->size = 32;
  if (!(rr->rule = calloc(rr->size, sizeof(s_rule))))
    err(10, "rules calloc");
}

void rules_free (s_rules *rr)
{
  int i;
  assert(rr);
  i = rr->count;
  while (i--)
    rule_free(rr->rule + i);
  free(rr->rule);
  bzero(rr, sizeof(s_rules));
}

void rules_enlarge (s_rules *rr)
{
  assert(rr);
  rr->size += rr->size < 1024 ? rr->size : 1024;
  if (!(rr->rule = realloc(rr->rule, rr->size * sizeof(s_rule))))
    err(10, "rules realloc");
  assert(rr->size > rr->count);
}

const s_rule * rules_add (s_rules *rr, const s_rule *r)
{
  assert(rr);
  if (rr->count == rr->size)
    rules_enlarge(rr);
  rr->rule[rr->count] = *r;
  return rr->rule + rr->count++;
}
