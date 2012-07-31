/******************************************************************************
* fpattern.c
// Copyright 2011 Chris Sanchez
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
// ====================================================================
//
*	Functions for matching filename patterns to filenames.
*
* Usage
*	(See "fpattern.h".)
*
* Notes
*	These pattern matching capabilities are modeled after those found in
*	the UNIX command shells.
*
*	`DELIM' must be defined to 1 if pathname separators are to be handled
*	explicitly.
*
* History
*	1.00 1997-01-03 David Tribble.
*		First cut.
*	1.01 1997-01-03 David Tribble.
*		Added SUB pattern character.
*		Added fpattern_matchn().
*	1.02 1997-01-12 David Tribble.
*		Fixed missing lowercase matching cases.
*	1.03 1997-01-13 David Tribble.
*		Pathname separator code is now controlled by DELIM macro.
*	1.04 1997-01-14 David Tribble.
*		Added QUOTE macro.
*	1.05 1997-01-15 David Tribble.
*		Handles special case of empty pattern and empty filename.
*	1.06 1997-01-26 David Tribble.
*		Changed range negation character from '^' to '!', ala Unix.
*	1.07 1997-08-02 David Tribble.
*		Uses the 'FPAT_XXX' constants.
*
* Limitations
*	This code is copyrighted by the author, but permission is hereby
*	granted for its unlimited use provided that the original copyright
*	and authorship notices are retained intact.
*
*	Other queries can be sent to:
*	    dtribble@techie.com
*
* Copyright ©1997 by David R. Tribble, all rights reserved.
*/

#include <cslib.h>

/* System includes */

#include <stddef.h>


/* Local includes */

#include <util/UtilPattern.h>


/* Local constants */

#ifndef NULL
#define NULL		((void *) 0)
#endif

#ifndef FALSE
#define FALSE		0
#endif

#ifndef TRUE
#define TRUE		1
#endif

#define SUBCHR		FPAT_CLOSP

//#ifndef DELIM
//#define DELIM		0
//#endif

#define DEL		FPAT_DEL

#ifdef UNIX
#define DEL2CHR		FPAT_DEL
#else /*DOS*/
#define DEL2CHR		FPAT_DEL2
#endif

#ifdef UNIX
#define QUOTECHR		FPAT_QUOTE
#else /*DOS*/
#define QUOTECHR		FPAT_QUOTE2
#endif


/* Local function macros */

#ifdef UNIX
#define lowercase(c)	(c)
#else /*DOS*/
#define lowercase(c)	tolower(c)
#endif


/*-----------------------------------------------------------------------------
* fpattern_isvalid()
*	Checks that filename pattern 'pat' is a well-formed pattern.
*
* Returns
*	1 (true) if 'pat' is a valid filename pattern, otherwise 0 (false).
*
* Caveats
*	If 'pat' is null, 0 (false) is returned.
*
*	If 'pat' is empty (""), 1 (true) is returned, and it is considered a
*	valid (but degenerate) pattern (the only filename it matches is the
*	empty ("") string).
*/

int fpattern_isvalid(const char *pat)
{
    int		len;

    /* Check args */
    if (pat == NULL)
        return (FALSE);

    /* Verify that the pattern is valid */
    for (len = 0;  pat[len] != '\0';  len++)
    {
        switch (pat[len])
        {
        case FPAT_SET_L:
            /* Char set */
            len++;
            if (pat[len] == FPAT_SET_NOT)
                len++;			/* Set negation */

            while (pat[len] != FPAT_SET_R)
            {
                if (pat[len] == QUOTECHR)
                    len++;		/* Quoted char */
                if (pat[len] == '\0')
                    return (FALSE);	/* Missing closing bracket */
                len++;

                if (pat[len] == FPAT_SET_THRU)
                {
                    /* Char range */
                    len++;
                    if (pat[len] == QUOTECHR)
                        len++;		/* Quoted char */
                    if (pat[len] == '\0')
                        return (FALSE);	/* Missing closing bracket */
                    len++;
                }

                if (pat[len] == '\0')
                    return (FALSE);	/* Missing closing bracket */
            }
            break;

        case QUOTECHR:
            /* Quoted char */
            len++;
            if (pat[len] == '\0')
                return (FALSE);		/* Missing quoted char */
            break;

        case FPAT_NOT:
            /* Negated pattern */
            len++;
            if (pat[len] == '\0')
                return (FALSE);		/* Missing subpattern */
            break;

        default:
            /* Valid character */
            break;
        }
    }

    return (TRUE);
}


/*-----------------------------------------------------------------------------
* fpattern_submatch()
*	Attempts to match subpattern 'pat' to subfilename 'fname'.
*
* Returns
*	1 (true) if the subfilename matches, otherwise 0 (false).
*
* Caveats
*	This does not assume that 'pat' is well-formed.
*
*	If 'pat' is empty (""), the only filename it matches is the empty ("")
*	string.
*
*	Some non-empty patterns (e.g., "") will match an empty filename ("").
*/

static int fpattern_submatch(const char *pat, const char *fname)
{
    int		fch;
    int		pch;
    int		i;
    int		yes, match;
    int		lo, hi;

    /* Attempt to match subpattern against subfilename */
    while (*pat != '\0')
    {
        fch = *fname;
        pch = *pat;
        pat++;

        switch (pch)
        {
        case FPAT_ANY:
            /* Match a single char */
        #if DELIM
            if (fch == DEL  ||  fch == DEL2CHR  ||  fch == '\0')
                return (FALSE);
        #else
            if (fch == '\0')
                return (FALSE);
        #endif
            fname++;
            break;

        case FPAT_CLOS:
            /* Match zero or more chars */
            i = 0;
        #if DELIM
            while (fname[i] != '\0'  &&
                    fname[i] != DEL  &&  fname[i] != DEL2CHR)
                i++;
        #else
            while (fname[i] != '\0')
                i++;
        #endif
            while (i >= 0)
            {
                if (fpattern_submatch(pat, fname+i))
                    return (TRUE);
                i--;
            }
            return (FALSE);

        case SUBCHR:
            /* Match zero or more chars */
            i = 0;
            while (fname[i] != '\0'  &&
        #if DELIM
                    fname[i] != DEL  &&  fname[i] != DEL2CHR  &&
        #endif
                    fname[i] != '.')
                i++;
            while (i >= 0)
            {
                if (fpattern_submatch(pat, fname+i))
                    return (TRUE);
                i--;
            }
            return (FALSE);

        case QUOTECHR:
            /* Match a quoted char */
            pch = *pat;
            if (lowercase(fch) != lowercase(pch)  ||  pch == '\0')
                return (FALSE);
            fname++;
            pat++;
            break;

        case FPAT_SET_L:
            /* Match char set/range */
            yes = TRUE;
            if (*pat == FPAT_SET_NOT)
            {
               pat++;
               yes = FALSE;	/* Set negation */
            }

            /* Look for [s], [-], [abc], [a-c] */
            match = !yes;
            while (*pat != FPAT_SET_R  &&  *pat != '\0')
            {
                if (*pat == QUOTECHR)
                    pat++;	/* Quoted char */

                if (*pat == '\0')
                    break;
                lo = *pat++;
                hi = lo;

                if (*pat == FPAT_SET_THRU)
                {
                    /* Range */
                    pat++;

                    if (*pat == QUOTECHR)
                        pat++;	/* Quoted char */

                    if (*pat == '\0')
                        break;
                    hi = *pat++;
                }

                if (*pat == '\0')
                    break;

                /* Compare character to set range */
                if (lowercase(fch) >= lowercase(lo)  &&
                    lowercase(fch) <= lowercase(hi))
                    match = yes;
            }

            if (!match)
                return (FALSE);

            if (*pat == '\0')
                return (FALSE);		/* Missing closing bracket */

            fname++;
            pat++;
            break;

        case FPAT_NOT:
            /* Match only if rest of pattern does not match */
            if (*pat == '\0')
                return (FALSE);		/* Missing subpattern */
            i = fpattern_submatch(pat, fname);
            return !i;

#if DELIM
        case DEL:
    #if DEL2CHR != DEL
        case DEL2CHR:
    #endif
            /* Match path delimiter char */
            if (fch != DEL  &&  fch != DEL2CHR)
                return (FALSE);
            fname++;
            break;
#endif

        default:
            /* Match a (non-null) char exactly */
            if (lowercase(fch) != lowercase(pch))
                return (FALSE);
            fname++;
            break;
        }
    }

    /* Check for complete match */
    if (*fname != '\0')
        return (FALSE);

    /* Successful match */
    return (TRUE);
}


/*-----------------------------------------------------------------------------
* fpattern_match()
*	Attempts to match pattern 'pat' to filename 'fname'.
*
* Returns
*	1 (true) if the filename matches, otherwise 0 (false).
*
* Caveats
*	If 'fname' is null, zero (false) is returned.
*
*	If 'pat' is null, zero (false) is returned.
*
*	If 'pat' is empty (""), the only filename it matches is the empty
*	string ("").
*
*	If 'fname' is empty, the only pattern that will match it is the empty
*	string ("").
*
*	If 'pat' is not a well-formed pattern, zero (false) is returned.
*
*	Upper and lower case letters are treated the same; alphabetic
*	characters are converted to lower case before matching occurs.
*	Conversion to lower case is dependent upon the current locale setting.
*/

int fpattern_match(const char *pat, const char *fname)
{
    int		rc;

    /* Check args */
    if (fname == NULL)
        return (FALSE);

    if (pat == NULL)
        return (FALSE);

    /* Verify that the pattern is valid, and get its length */
    if (!fpattern_isvalid(pat))
        return (FALSE);

    /* Attempt to match pattern against filename */
    if (fname[0] == '\0')
        return (pat[0] == '\0');	/* Special case */
    rc = fpattern_submatch(pat, fname);

    return (rc);
}


/*-----------------------------------------------------------------------------
* fpattern_matchn()
*	Attempts to match pattern 'pat' to filename 'fname'.
*	This operates like fpattern_match() except that it does not verify that
*	pattern 'pat' is well-formed, assuming that it has been checked by a
*	prior call to fpattern_isvalid().
*
* Returns
*	1 (true) if the filename matches, otherwise 0 (false).
*
* Caveats
*	If 'fname' is null, zero (false) is returned.
*
*	If 'pat' is null, zero (false) is returned.
*
*	If 'pat' is empty (""), the only filename it matches is the empty ("")
*	string.
*
*	If 'pat' is not a well-formed pattern, unpredictable results may occur.
*
*	Upper and lower case letters are treated the same; alphabetic
*	characters are converted to lower case before matching occurs.
*	Conversion to lower case is dependent upon the current locale setting.
*
* See also
*	fpattern_match().
*/

int fpattern_matchn(const char *pat, const char *fname)
{
    int		rc;

    /* Check args */
    if (fname == NULL)
        return (FALSE);

    if (pat == NULL)
        return (FALSE);

    /* Assume that pattern is well-formed */

    /* Attempt to match pattern against filename */
    rc = fpattern_submatch(pat, fname);

    return (rc);
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/


/* End fpattern.c */
