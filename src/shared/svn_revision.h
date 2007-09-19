// $Id: svn_revision.h 1510 2007-09-15 01:07:32Z Burlex $

#ifndef _SVN_REVISION_H
#define _SVN_REVISION_H

static const char * REVISION_TEXT = "$Rev: 1510 $";

inline int g_getRevision()
{
	const char * p = REVISION_TEXT + 6;
	return atoi(p);
}

/////////////////////
#endif