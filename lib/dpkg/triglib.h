/*
 * libdpkg - Debian packaging suite library routines
 * triglib.h - declarations for trigger handling
 *
 * Copyright © 2007 Canonical, Ltd.
 *   written by Ian Jackson <ian@chiark.greenend.org.uk>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBDPKG_TRIGLIB_H
#define LIBDPKG_TRIGLIB_H

#include <dpkg/macros.h>
#include <dpkg/dpkg-db.h>

DPKG_BEGIN_DECLS

/*
 * Hooks for more sophisticated processing in dpkg proper.
 *
 * We do things like this so we can get most of the trigger tracking
 * in dpkg-query, dselect, and so on, but avoid the transitional
 * processing and deferred trigproc queue management other than when
 * we're actually doing real package management work.
 */

const char *illegal_triggername(const char *p);

struct trigfileint {
	struct pkginfo *pkg;
	struct filenamenode *fnn;
	struct trigfileint *samefile_next;
	struct {
		struct trigfileint *next, *back;
	} inoverall;
};

/* The first two hooks are normally NULL.
 * If non-NULL, we're dpkg proper and we might need to invent trigger
 * activations as the first run of a triggers-supporting dpkg. */
struct trig_hooks {
	void (*enqueue_deferred)(struct pkginfo *pend);
	void (*transitional_activate)(enum modstatdb_rw cstatus);

	struct filenamenode *(*namenode_find)(const char *filename, int nonew);
	struct trigfileint **(*namenode_interested)(struct filenamenode *fnn);

	/* Returns a pointer from nfmalloc. */
	const char *(*namenode_name)(struct filenamenode *fnn);
};

#define TRIGHOOKS_DEFINE_NAMENODE_ACCESSORS				 \
  static struct trigfileint **th_nn_interested(struct filenamenode *fnn) \
    { return &fnn->trig_interested; }					 \
  static const char *th_nn_name(struct filenamenode *fnn)		 \
    { return fnn->name; }

void trig_override_hooks(const struct trig_hooks *hooks);

char *trig_get_triggersdir(const char *admindir);

void trig_file_activate_byname(const char *trig, struct pkginfo *aw);
void trig_file_activate(struct filenamenode *trig, struct pkginfo *aw);

int trig_note_pend_core(struct pkginfo *pend, const char *trig /*not copied!*/);
int trig_note_pend(struct pkginfo *pend, const char *trig /*not copied!*/);
int trig_note_aw(struct pkginfo *pend, struct pkginfo *aw);
void trig_clear_awaiters(struct pkginfo *notpend);

void trig_enqueue_awaited_pend(struct pkginfo *pend);
void trig_fixup_awaiters(enum modstatdb_rw cstatus);

void trig_file_interests_ensure(void);
void trig_file_interests_save(void);

typedef void trig_parse_cicb(const char *trig, void *user);
void trig_cicb_interest_delete(const char *trig, void *user);
void trig_cicb_interest_add(const char *trig, void *user);
void trig_cicb_statuschange_activate(const char *trig, void *user);
void trig_parse_ci(const char *file, trig_parse_cicb *interest,
                   trig_parse_cicb *activate, void *user);

void trig_incorporate(enum modstatdb_rw cstatus, const char *admindir);

DPKG_END_DECLS

#endif /* LIBDPKG_TRIGLIB_H */