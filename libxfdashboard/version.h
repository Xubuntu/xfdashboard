/*
 * version: variables and functions to check the version
 * 
 * Copyright 2012-2021 Stephan Haller <nomad@froevel.de>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

/**
 * SECTION:version
 * @short_description: Variables and functions to check version of libxfdashboard
 *
 * Provides version information macros, useful for checks in configure scripts
 * and to write code against different versions of libxfdashboard that do not
 * provide the same API.
 */

#ifndef __LIBXFDASHBOARD_VERSION__
#define __LIBXFDASHBOARD_VERSION__

#if !defined(__LIBXFDASHBOARD_H_INSIDE__) && !defined(LIBXFDASHBOARD_COMPILATION)
#error "Only <libxfdashboard/libxfdashboard.h> can be included directly."
#endif

/**
 * LIBXFDASHBOARD_MAJOR_VERSION:
 *
 * Return value: the major version number of the libxfdashboard library, from the
 * headers used at application compile time, rather than from the library
 * linked against at application run time.
 * (e.g. in libxfdashboard version 0.7.1 this is 0.)
 */
#define LIBXFDASHBOARD_MAJOR_VERSION (1)

/**
 * LIBXFDASHBOARD_MINOR_VERSION:
 *
 * Return value: the minor version number of the libxfdashboard library, from the
 * headers used at application compile time, rather than from the library
 * linked against at application run time.
 * (e.g. in libxfdashboard version 0.7.1 this is 7.)
 */
#define LIBXFDASHBOARD_MINOR_VERSION (0)

/**
 * LIBXFDASHBOARD_MICRO_VERSION:
 *
 * Return value: the micro version number of the libxfdashboard library, from the
 * headers used at application compile time, rather than from the library
 * linked against at application run time.
 * (e.g. in libxfdashboard version 0.7.1 this is 1.)
 */
#define LIBXFDASHBOARD_MICRO_VERSION (0)

/**
 * LIBXFDASHBOARD_CHECK_VERSION:
 * @major: major version (e.g. 0 for version 0.7.1)
 * @minor: minor version (e.g. 7 for version 0.7.1)
 * @micro: micro version (e.g. 1 for version 0.7.1)
 *
 * Return value: %TRUE if the version of the libxfdashboard header files
 * is the same as or newer than the passed-in version, %FALSE
 * otherwise.
 */
#define LIBXFDASHBOARD_CHECK_VERSION(major,minor,micro) \
	(LIBXFDASHBOARD_MAJOR_VERSION>(major) || \
		(LIBXFDASHBOARD_MAJOR_VERSION==(major) && \
			LIBXFDASHBOARD_MINOR_VERSION>(minor)) || \
		(LIBXFDASHBOARD_MAJOR_VERSION==(major) && \
			LIBXFDASHBOARD_MINOR_VERSION==(minor) && \
			LIBXFDASHBOARD_MICRO_VERSION>=(micro)))

#endif	/* __LIBXFDASHBOARD_VERSION__ */
