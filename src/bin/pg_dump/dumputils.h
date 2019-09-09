/*-------------------------------------------------------------------------
 *
 * Utility routines for SQL dumping
 *	Basically this is stuff that is useful in both pg_dump and pg_dumpall.
 *	Lately it's also being used by psql and bin/scripts/ ...
 *
 *
 * Portions Copyright (c) 1996-2015, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/bin/pg_dump/dumputils.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef DUMPUTILS_H
#define DUMPUTILS_H

#include "libpq-fe.h"
#include "pqexpbuffer.h"

/*
 * Data structures for simple lists of OIDs and strings.  The support for
 * these is very primitive compared to the backend's List facilities, but
 * it's all we need in pg_dump.
 */
typedef struct SimpleOidListCell
{
	struct SimpleOidListCell *next;
	Oid			val;
} SimpleOidListCell;

typedef struct SimpleOidList
{
	SimpleOidListCell *head;
	SimpleOidListCell *tail;
} SimpleOidList;

typedef struct SimpleStringListCell
{
	struct SimpleStringListCell *next;
	char		val[FLEXIBLE_ARRAY_MEMBER];		/* null-terminated string here */
} SimpleStringListCell;

typedef struct SimpleStringList
{
	SimpleStringListCell *head;
	SimpleStringListCell *tail;
} SimpleStringList;

#define atooid(x)  ((Oid) strtoul((x), NULL, 10))

/*
 * Preferred strftime(3) format specifier for printing timestamps in pg_dump
 * and friends.
 *
 * We don't print the timezone on Windows, because the names are long and
 * localized, which means they may contain characters in various random
 * encodings; this has been seen to cause encoding errors when reading the
 * dump script.  Think not to get around that by using %z, because
 * (1) %z is not portable to pre-C99 systems, and
 * (2) %z doesn't actually act differently from %Z on Windows anyway.
 */
#ifndef WIN32
#define PGDUMP_STRFTIME_FMT  "%Y-%m-%d %H:%M:%S %Z"
#else
#define PGDUMP_STRFTIME_FMT  "%Y-%m-%d %H:%M:%S"
#endif

extern int	quote_all_identifiers;
extern PQExpBuffer (*getLocalPQExpBuffer) (void);

extern const char *fmtId(const char *identifier);
extern const char *fmtQualifiedId(int remoteVersion,
			   const char *schema, const char *id);
extern char *formatPGVersionNumber(int version_number, bool include_minor,
					  char *buf, size_t buflen);
extern void appendStringLiteral(PQExpBuffer buf, const char *str,
					int encoding, bool std_strings);
extern void appendStringLiteralConn(PQExpBuffer buf, const char *str,
						PGconn *conn);
extern void appendStringLiteralDQ(PQExpBuffer buf, const char *str,
					  const char *dqprefix);
extern void appendByteaLiteral(PQExpBuffer buf,
				   const unsigned char *str, size_t length,
				   bool std_strings);
extern void appendShellString(PQExpBuffer buf, const char *str);
extern void appendConnStrVal(PQExpBuffer buf, const char *str);
extern void appendPsqlMetaConnect(PQExpBuffer buf, const char *dbname);
extern bool parsePGArray(const char *atext, char ***itemarray, int *nitems);
extern bool buildACLCommands(const char *name, const char *subname, const char *nspname,
				 const char *type, const char *acls, const char *owner,
				 const char *prefix, int remoteVersion,
				 PQExpBuffer sql);
extern bool buildDefaultACLCommands(const char *type, const char *nspname,
						const char *acls, const char *owner,
						int remoteVersion,
						PQExpBuffer sql);
extern bool processSQLNamePattern(PGconn *conn, PQExpBuffer buf,
					  const char *pattern,
					  bool have_where, bool force_escape,
					  const char *schemavar, const char *namevar,
					  const char *altnamevar, const char *visibilityrule);
extern void buildShSecLabelQuery(PGconn *conn, const char *catalog_name,
					 Oid objectId, PQExpBuffer sql);
extern void emitShSecLabels(PGconn *conn, PGresult *res,
				PQExpBuffer buffer, const char *objtype, const char *objname);
extern void set_dump_section(const char *arg, int *dumpSections);

extern void simple_string_list_append(SimpleStringList *list, const char *val);
extern bool simple_string_list_member(SimpleStringList *list, const char *val);

extern bool variable_is_guc_list_quote(const char *name);

extern bool SplitGUCList(char *rawstring, char separator,
			 char ***namelist);

#endif   /* DUMPUTILS_H */
