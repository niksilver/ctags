/*
 *   Copyright (c) 2022 Nik Silver
 *   based on the Kotlin code by Jan Dolinár
 *
 *   This source code is released for free distribution under the terms of the
 *   GNU General Public License version 2 or (at your option) any later version.
 *
 *   This module contains macros, data decls and prototypes to generate tags for Elm.
 */

/*
*   INCLUDE FILES
*/

#include "kind.h"
#include "peg_common.h"

/*
*   DATA DECLARATIONS
*/
typedef enum {
    K_MODULE,
    K_NAMESPACE,
    K_TYPE,
    K_CONSTRUCTOR,
    K_ALIAS,
    K_PORT,
    K_FUNCTION,
	COUNT_KINDS
} ElmKind;

typedef enum {
	ELM_ROLE_DEFINED,
	ELM_ROLE_IMPORTED
} elmRoles;

static roleDefinition ElmRoles [] = {
	{ true, "def", "item defined" },
	{ true, "imported", "item imported" },
};

static kindDefinition ElmKinds [COUNT_KINDS] = {
    { true, 'm', "module", "modules", },
    { true, 'n', "namespace", "modules renamed", },
    { true, 't', "type", "types",
	  .referenceOnly = true, ATTACH_ROLES(ElmRoles) },
    { true, 'c', "constructor", "constructors",
	  .referenceOnly = true, ATTACH_ROLES(ElmRoles) },
    { true, 'a', "alias", "aliases", },
    { true, 'p', "port", "ports", },
    { true, 'f', "function", "functions",
	  .referenceOnly = true, ATTACH_ROLES(ElmRoles) },
};

struct parserCtx {
    struct parserBaseCtx base;
};

/*
*   FUNCTION PROTOTYPES
*/
#define USE_KIND_STACK KIND_GHOST_INDEX
static int makeElmTag (struct parserCtx *auxil, const char *name, long offset, int kind, int role, bool pushScope);
static void addElmSignature(int scope_index, const char *str);
static vString *collapseWhitespace (const char *sig);
