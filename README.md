# GIT-AUTH

## Synopsis

    GIT_AUTH_ID=ID git-auth COMMAND [ARGS ...]</td>

## Description

**git-auth** starts by reading rules from /etc/git/auth.conf, one rule
per line. Empty lines and lines starting with # are ignored. Each rule
is made of tokens separated by one or more spaces.

**git-auth** executes COMMAND and ARGS using execvp if the requested ID,
COMMAND and ARGS match any of the rules defined in /etc/git/auth.conf.

**git-auth** matches a rule by matching all the rule's tokens with ID,
COMMAND and ARGS in order. Comparison is case sensitive.

A wildcard token "*" matches any string.

## Environment

 * **GIT_AUTH_ID** : an arbitrary token to match with the rules.
This is usually set for each public key in /home/git/.ssh/authorized_keys
with
    environment="GIT_AUTH_ID=..." ssh-rsa ...

## Configuration
Access control rules for each git repository go into
`/etc/git/auth.conf`.

The syntax of this file is one rule per line.
Each rule is composed of symbols separated by spaces.
In order :
 - A **GIT_AUTH_ID** value that was set in
   `/home/git/.ssh/authorized_keys`.
 - A `git-shell` command : either `git-upload-pack` for read access,
   or `git-receive-pack` for write access.
 - A repository path, relative to `/home/git`,
   example: `thodg/config.git`

## Files

 * /etc/git/auth.conf
 * /home/git/.ssh/authorized_keys
