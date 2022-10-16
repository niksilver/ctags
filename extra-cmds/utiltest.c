/*
*   Copyright (c) 2022 Masatake YAMATO
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License version 2 or (at your option) any later version.
*/

#include "general.h"

#include "acutest.h"
#include "htable.h"
#include "routines.h"
#include <string.h>

static void test_htable_update(void)
{
	hashTable *htable = hashTableNew (3, hashCstrhash, hashCstreq,
									  eFree, NULL);
	TEST_CHECK(htable != NULL);

	hashTablePutItem (htable, strdup("a"), "A");
	TEST_CHECK (hashTableUpdateItem (htable,  "a", "B") == true);
	TEST_CHECK (hashTableUpdateItem (htable,  "b", "B") == false);
	TEST_CHECK (strcmp (hashTableGetItem (htable, "a"), "B") == 0);
	TEST_CHECK (hashTableUpdateOrPutItem (htable,  "a", "C") == true);
	TEST_CHECK (hashTableUpdateOrPutItem (htable,  strdup("x"), "X") == false);
	TEST_CHECK (strcmp (hashTableGetItem (htable, "x"), "X") == 0);
	hashTableDelete(htable);
}

static void test_routines_strrstr(void)
{
	TEST_CHECK(strcmp(strrstr("abcdcdb", "cd"), "cdb") == 0);
}

TEST_LIST = {
   { "htable/update",    test_htable_update    },
   { "routines/strrstr", test_routines_strrstr },
   { NULL, NULL }     /* zeroed record marking the end of the list */
};
