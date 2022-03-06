/*
 *   Copyright (c) 2022 Nik Silver
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

/* We only define roles which aren't def(ined)
 */
typedef enum {
	ELM_ROLE_IMPORTED
} elmRoles;

static roleDefinition ElmRoles [] = {
	{ true, "imported", "item imported" },
};

/* Use referenceOnly = true when a tag must always appear
 * as role that's not def(ined).
 */
static kindDefinition ElmKinds [COUNT_KINDS] = {
    { true, 'm', "module", "modules",
	  .referenceOnly = false, ATTACH_ROLES(ElmRoles) },
    { true, 'n', "namespace", "modules renamed", },
    { true, 't', "type", "types",
	  .referenceOnly = false, ATTACH_ROLES(ElmRoles) },
    { true, 'c', "constructor", "constructors",
	  .referenceOnly = false, ATTACH_ROLES(ElmRoles) },
    { true, 'a', "alias", "aliases", },
    { true, 'p', "port", "ports", },
    { true, 'f', "function", "functions",
	  .referenceOnly = false, ATTACH_ROLES(ElmRoles) },
};

struct parserCtx {
    struct parserBaseCtx base;
};

/*
*   FUNCTION PROTOTYPES
*/
#define USE_KIND_STACK KIND_GHOST_INDEX
static int makeElmTag (struct parserCtx *auxil, const char *name, long offset, int kind, int role);
static int makeElmTagWithScope (struct parserCtx *auxil, const char *name, long offset, int kind, int role);
static void addElmSignature(int scope_index, const char *str);
static vString *collapseWhitespace (const char *sig);
