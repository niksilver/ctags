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
 * This allows us to save and restore the module scope.
 * We want to do this because it's unhelpful to say that an imported item
 * is named as being in the scope of the importing module.
 */

// Remember the scope of the module tag. But at first there is none (CORK_NIL).
int elm_module_scope_index;

#define ELM_INIT_MODULE_SCOPE \
    elm_module_scope_index = CORK_NIL
#define ELM_SAVE_MODULE_SCOPE \
    if (elm_module_scope_index != CORK_NIL) { \
        POP_SCOPE(auxil); \
    }
#define ELM_RESTORE_MODULE_SCOPE \
    if (elm_module_scope_index != CORK_NIL) { \
        SET_SCOPE(auxil, elm_module_scope_index); \
    }

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
	COUNT_KINDS,
} ElmKind;

/* We only define roles which aren't def(ined)
 */
typedef enum {
	ELM_ROLE_IMPORTED
} elmRoles;

static roleDefinition ElmRoles [] = {
	{ true, "imported", "item imported" },
};

typedef enum {
	F_MODULENAME,
    COUNT_FIELDS,
} ElmField;

static fieldDefinition ElmFields[COUNT_FIELDS] = {
	{ .name        = "moduleName",
	  .description = "actual name of renamed module",
	  .enabled     = true },
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
static int makeElmTagSettingScope (struct parserCtx *auxil, const char *name, long offset, int kind, int role);
static void addElmSignature(int scope_index, const char *str);
static vString *collapseWhitespace (const char *sig);
