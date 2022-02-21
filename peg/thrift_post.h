/*
 *   Copyright (c) 2021 Masatake YAMATO
 *
 *   This source code is released for free distribution under the terms of the
 *   GNU General Public License version 2 or (at your option) any later version.
 *
 *   This module contains macros, data decls and prototypes to generate tags for Thrift IDL.
 *   Reference: https://thrift.apache.org/docs/idl
 */

/*
*   INCLUDE FILES
*/
#include "options.h"
#include "parse.h"
#include "routines.h"
#include "dependency.h"

static int makeThriftTagFull (struct parserCtx *auxil, const char *name, long offset, int kind, int role,
							  bool pushScope)
{
    printf("Nik: Enter: makeThriftTagFull(auxil, name=%s, offset=%ld, kind=%d, role=%d, pushScope=%d)\n",
            name, offset, kind, role, pushScope);
	tagEntryInfo e;
	int k = (kind == USE_KIND_STACK? PEEK_KIND (auxil): kind);
	if (role == ROLE_DEFINITION_INDEX) {
        printf("Nik:        makeThriftTagFull doing initTagEntry(e, name=%s, k=%d)\n", name, k);
		initTagEntry(&e, name, k);
    }
	else {
        printf("Nik:        makeThriftTagFull doing initRefTagEntry(e, name=%s, k=%d, role=%d)\n", name, k, role);
		initRefTagEntry(&e, name, k, role);
    }
	e.lineNumber = getInputLineNumberForFileOffset (offset);
	e.filePosition = getInputFilePositionForLine (e.lineNumber);
	e.extensionFields.scopeIndex = BASE_SCOPE (auxil);
	int scope_index = makeTagEntry (&e);
	if (pushScope)
		SET_SCOPE(auxil, scope_index);
    printf("Nik: Exist: makeThriftTagFull\n");
	return scope_index;
}

static int makeThriftTag (struct parserCtx *auxil, const char *name, long offset, int kind, bool pushScope)
{
    printf("Nik: Enter: makeThriftTag(auxil, name=%s, offset=%ld, kind=%d, pushScope=%d)\n",
            name, offset, kind, pushScope);
	return makeThriftTagFull (auxil, name, offset, kind, ROLE_DEFINITION_INDEX, pushScope);
    printf("Nik: Exist: makeThriftTag\n");
}

static vString* unliteral(const char *literal)
{
	vString *vstr = vStringNew ();

	const char *c = literal;
	if (*c == '"' || *c == '\'')
		c++;

	for (; *c != '\0'; c++)
	{
		if (*c == '\\')
		{
			c++;
			if (*c == '\0')
				break;

			/* ???
			 * How the backslash is handled is not wel explained in the document. */
			if (! (*c == '"' || *c == '\'' || *c == '\\'))
				vStringPut (vstr, '\\');
		}
		vStringPut (vstr, *c);
	}

	if (vStringLength (vstr) > 0 &&
		(vStringLast (vstr) == '"' ||
		 vStringLast (vstr) == '\''))
		vStringTruncate(vstr, vStringLength(vstr) - 1);

	return vstr;
}

static void ctxInit (struct parserCtx *auxil)
{
 	BASE_INIT(auxil, K_STRUCT);
}

static void ctxFini (struct parserCtx *auxil)
{
	BASE_FINI(auxil);
}

static void findThriftTags (void)
{
	struct parserCtx auxil;

	ctxInit (&auxil);
	// 	BASE_DEBUG_RULE(&auxil, "Const");

	pthrift_context_t *pctx = pthrift_create(&auxil);

	while ( pthrift_parse(pctx, NULL) && (!BASE_ERROR(&auxil)) );

	pthrift_destroy(pctx);
	ctxFini (&auxil);
}

extern parserDefinition* ThriftParser (void)
{
	static const char *const extensions [] = { "thrift", NULL };
	parserDefinition* def = parserNew ("Thrift");

	static parserDependency dependencies [] = {
		[0] = { DEPTYPE_FOREIGNER, "C++", NULL },
	};

	def->kindTable  = ThriftKinds;
	def->kindCount  = ARRAY_SIZE (ThriftKinds);
	def->fieldTable = ThriftFields;
	def->fieldCount = ARRAY_SIZE (ThriftFields);
	def->extensions = extensions;
	def->dependencies = dependencies;
	def->dependencyCount = ARRAY_SIZE (dependencies);
	def->parser     = findThriftTags;
	def->useCork    = true;
	def->enabled    = true;
	def->defaultScopeSeparator = ".";
	return def;
}
