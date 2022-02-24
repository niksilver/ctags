/*
 *   Copyright (c) 2022 Nik Silver
 *   based on the Kotlin code by Jan Dolinár
 *
 *   This source code is released for free distribution under the terms of the
 *   GNU General Public License version 2 or (at your option) any later version.
 *
 *   This module contains functions to generate tags for Elm.
 */

/*
*   INCLUDE FILES
*/
#include "parse.h"
#include "trace.h"

// #define NIK_PRINT(fmt, vargs...) fprintf(stderr, "Nik: "); fprintf(stderr, fmt, ##vargs)
#define NIK_PRINT(fmt, vargs...)

/*
* FUNCTION DEFINITIONS
*/
static int makeElmTag (struct parserCtx *auxil, const char *name, long offset, int kind, int role, bool pushScope)
{
    NIK_PRINT("Enter: makeElmTag(auxil=%p, name=%s, offset=%ld, kind=%d, role=%d, pushScope=%d)\n",
            auxil, name, offset, kind, role, pushScope);
    //int k = PEEK_KIND(auxil);
    tagEntryInfo e;
	int k = (kind == USE_KIND_STACK? PEEK_KIND (auxil): kind);
    NIK_PRINT("       makeElmTag: k=%d\n", k);

	if (role == ROLE_DEFINITION_INDEX) {
        initTagEntry(&e, name, k);
    } else {
		initRefTagEntry(&e, name, k, role);
    }

    e.lineNumber = getInputLineNumberForFileOffset (offset);
    e.filePosition = getInputFilePositionForLine (e.lineNumber);
    e.extensionFields.scopeIndex = BASE_SCOPE(auxil);
    int scope_index = makeTagEntry (&e);
    if (pushScope)
    {
        SET_SCOPE(auxil, scope_index);
    }
	return scope_index;
}

static void addElmSignature(int scope_index, const char *sig) {
    tagEntryInfo *e = getEntryInCorkQueue (scope_index);

    vString *vstr = collapseWhitespace(sig);
    const char *stripped = vStringValue(vstr);

    if (e)
        e->extensionFields.signature = eStrdup (stripped);

    vStringDelete(vstr);
}

/* For a signature such as "a1   b2  c3" we want to transform it
 * to "a1 b2 c3" for the signature field.
 */
static vString *collapseWhitespace (const char *sig) {
	vString *vstr = vStringNew ();

	const char *c = sig;
    int found_ws = 0;

	for (; *c != '\0'; c++)
    {
        if (*c == ' ' || *c == '\t' || *c == '\r' || *c == '\n' || *c == '\f') {
            if (found_ws)
            {
                // We found whitespace just before, so ignore this
                continue;
            }

            found_ws = 1;
        }
        else
        {
            found_ws = 0;
        }
        vStringPut (vstr, *c);
    }

    return vstr;
}

static void ctxInit (struct parserCtx *auxil)
{
    BASE_INIT(auxil, K_MODULE);
}

static void ctxFini (struct parserCtx *auxil)
{
    BASE_FINI(auxil);
}

static void findElmTags (void)
{
    struct parserCtx auxil;

    ctxInit (&auxil);
    pelmish_context_t *pctx = pelmish_create(&auxil);

    while (pelmish_parse(pctx, NULL) && (!BASE_ERROR(&auxil)) );

    pelmish_destroy(pctx);
    ctxFini (&auxil);
}

extern parserDefinition* ElmishParser (void)
{
    static const char *const extensions [] = { "elmish", NULL };
    parserDefinition* def = parserNew ("Elmish");
    def->kindTable = ElmKinds;
    def->kindCount = ARRAY_SIZE (ElmKinds);
    def->extensions = extensions;
    def->parser = findElmTags;
    def->useCork = true;
	def->enabled    = true;
    //def->requestAutomaticFQTag = true;
    def->defaultScopeSeparator = ".";
    return def;
}
