//-------------------------------------------------------------------------
/*
 Copyright (C) 2007 Jonathon Fowler <jf@jonof.id.au>

 This file is part of JFShadowWarrior

 Shadow Warrior is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
//-------------------------------------------------------------------------

#ifndef __grpscan_h__
#define __grpscan_h__

// List of internally-known GRP files
struct grpfile {
    const char *name;
    unsigned int crcval;
    int size;
    int game;
    const struct grpfile *ref;    // For foundgrps items, is the grpfiles[] entry matched.
    struct grpfile *next;
};
extern struct grpfile grpfiles[];
extern struct grpfile *foundgrps;

int ScanGroups(void);
void FreeGroups(void);

#endif
