<pre>
/***********************************************************************/
/  03/01/2005 - Quake2plus - An enhanced DED server gamei386.so build.  /
/                      Last update - 16/01/2006                         /
/ **********************************************************************/


This is an enhanced build of baseq2 and Xatrix mod for DED quake2 server.
99% of the monster stuff has been taken out - this leads to a very fast game.
Also included are some fixes/additions by me.

***BFG has been removed in total - it will not spawn in any map***

This project is now complete unless I find a bug that has been missed over the last 2
years of playing - but that is unlikely and therefore I am releasing now as 'Final'.

**BUMMER** Famous last words.  I have found and fixed an original Xatrix bug - see
revision notes at the end of this file (this bug didn't affect the baseq2 build).



There are several new server C_VAR's.

Installation.
=============

Two binaries are supplied,  baseq2_gamei386.so and xatrix_gamei386.so.  If you use these,
just place into /quake2/baseq2/ or /quake2/xatrix/ directories respectively, renaming to
just 'gamei386.so'.  Don't forget to rename the original files in case you need to go
back!

To build from source, create a build directory, cd into it and extract the source:

> zcat quake2plus_scr.tar.gz | -pvxf -

then for build, issue:

> make

will give you the instructions.  There is _no_ need to edit the Makefile unless you wish
to change the optimisations (and know what you are doing).  Default build will work well
on all.


New CVAR's 1 to 3 are to do with map countdown and respawn protection.
======================================================================

countdelay <time in seconds> (default 0)
r_invul <time> (default 0) :Note this needs to be divided by 10; 35 == 3.5 seconds e.g.!!

If you set countdelay > 0, then all players joining server at map change will be 'frozen'
for the countdelay period with a HUD countdown counter.  All players start the map at the
same time.  Clients during this period are also NON_SOLID, so if a map only has a few 
spawn points, multiple players can all spawn on the same pad with no ill effect - after
map has started, telefrag resumes as normal.

countdelay will be the countdown time... countdelay 20 == 20 seconds from map change.

Setting countdelay 0 (default) in server.cfg  will turn off the whole countdown thing so it
runs as normal.

r_invul makes a newly spawned player invulnerable for the time set, giving a slight
protection in deathmatch.  Note here the value needs to be divided by 10 to get seconds
of r_invul; r_invul 40 == 4.0 seconds, r_invul 28 == 2.8 seconds.  r_invul 30 (3 seconds)
is a good value to use.  The newly spawned player will flash yellow during this duration.



New CVAR's 4, 5, 6 and 7 are as follows.
========================================

eom_muzic 0/1 (default 0)  (Xatrix build only)

1 turns on the xian.wav music at the end of map when in intermission.


ripper_self 0/xx (default 0) (applies to Xatrix MOD only)

The default is 0 - the gun performs as normal.
If the CVAR ripper_self is non-zero, then the ion ripper will cause self damage by that
amount.

The value is the health it will take i.e. ripper_self 10 will cause 10 health damage
if you get hit by your own 'ion's.  The usual damage caused by this weapon in DM is 30.
I found this too much for self damage - ripper_self 10 is a good value.  NOTE - if the
player has quad damage then the full damage will occur no matter who you hit :)


timeleft 0/1 (default 0)

If this is set to 1, map time left will be shown as a counter in the bottom right hand
of the HUD.  Server timelimit needs to be set for this to be displayed (of course!) as
the calculation is (timelimit - level.time) == timeleft.


show_deaths 0/1 (default 0)

If this is set, players will see their total deaths in the bottom left of HUD (with an icon).
Intermission scoreboard will display deaths instead of time.


Cvar's 8 and 9
=================
map_file <path to file>
map_random 0/1

Your map list will contain map names, one per line, no spaces.  Line starting with '//'
will be ignored.  CVAR map_file contains the path to the map list file, i.e. map_file 
"mapcfg/maps.txt".  This is the default path so that if you use Xatrix, map list will live
in /quake2/xatrix/mapcfg/, and basqe2 will live in /quake2/baseq2/mapcfg/ called 'maps.txt'

If map_random is 0, then the map list will play in list order, cycling from top to bottom.

If map_random is non-zero, then maps will be played randomly from the list.
The algorithm uses 7 (MAX_OLDMAPS) as cutoff, so a maplist of greater than 7 will effectly
play all maps once before starting to repeat.  The more maps you have in you map list
file, the better the random map selection will be.  I recommend >= 10 maps.  You can have
1000 maps in the list :-0 )


Other stuff - Xatrix only.
==========================

Also included are a few tweaks to the trap code.  If a trap is 'primed' and the user gets
killed, the trap will still fire off instead of being dropped :) .  A new trap model is
also available to show proper visible weapons (trapmodel.zip).  These models live in
/quake2/baseq2/ note the common skin is in /male/.

Archive:  trapmodel.zip
  Length     Date   Time    Name
 --------    ----   ----    ----
        0  01-03-04 15:09   players/
        0  01-03-04 16:04   players/female/
   105120  01-03-04 16:04   players/female/a_trap.md2
        0  01-03-04 14:02   players/male/
   104840  01-03-04 16:04   players/male/a_trap.md2
    36515  01-03-04 11:59   players/male/skin_pu.pcx


Trap on spawn pad[s].
=====================
Now, if a trap is thrown and it lands on or around a spawn pad within a radius of x2 it's
'sucking' range (radius 512 pts), this spawn pad will be the LAST option a player will
spawn on.  I used the existing code for DMFLAG (512) 'Player Spawn Farthest'.  If a trap is
found within the spawn pad area described above, then that spawn pad will be the marked as
the worse pad to use for respawn.  Totally invisible in the game, and works 100% very nice.
DMFLAG 'spawn furthest' needs to be set for this to be used.  Add 512 to your existing DMFLAG
value to apply.

End Xatrix only stuff.
======================



General.
========


Support for map entity file (***.ent).
======================================

I have also added support for ent files.  What this allows you to do is to add entities
(power-ups, weapons and ammo) to maps 'on the fly'... all the client needs is the original
map, and no special client support.

The 'ent file' consists of all the entities of a map plus your own added ones.  You will
need to extract the entities from a map first to create the text file, then add/subtract/change
to suit.  If a map doesn't have an ent file, then it will load as normal - the server console
reports the ent file status at map change (loaded, not found etc.).

IMPORTANT.  The code looks for ent files in /quake2/baseq2/entfiles or /quake2/xatrix/enfiles
folder (depending on what gamei386.so build was done) - you will need to create this and then
add your ent files there.

The ent file should be named <map name>.ent.  I have included two ent files that convert the
standard quake2 maps q2dm2 and q2dm8 into xatrix maps.  You can view these files here:

http://www.nick.ukfsn.org/xatrix_plus/

to see how it works.  Remember these live in  /entfiles/  folder, and CLIENTS do not need
this to play, just the original maps.

If you do not need this, then you can safely ignore, and server carries on normal if an ent
file does not exist.  If it does exist, it needs to be correct!!!  Look at the examples.

To use, create the ent file and place in /entfiles/ folder.  Start the map on the server
as normal, and it will load the entities from the file on the fly.  All clients will see and
can use the added entities with no additional files client side :)



Teamed weapons.
===============
I have fixed the code so that team_weapons work if server DMFLAG weapons_stay is set - the
teamed weapons will respawn after 5 seconds so the teaming will work and weapons will randomly
swap out. (Team weapons are where two or more weapons spawn on the same point, and randomly
change - i.e. only one weapon will spawn at any one time).


Head rolling.
=============

Now when your head gets blown off, it rolls around in the air a bit, and rocks to a stand still
on the ground :)


Respawn buffer delay.
=====================

One thing I HATE in Quake2 deathmatch is the situation where you die, but are still firing
the weapon causing you to respawn immediately when you didn't mean to!!!  I have added a 0.3
second delay after death to absorb any buffered key presses during death to stop this happening.


New Intermission Scoreboard
===========================

During the game, the original scoreboard show on the HUD.  At the end of the game (intermission)
there is now a new scoreboard showing clients in a list - nice and tidy and easier to read. The
top player is highlighted - also the 'next map' is announced at the bottom of the scoreboard.



***** NOTE *****
This is for a ded DM server ONLY!!!  Using as a game binary can produce funny effects/CRASHES.
	==* This is to be used only on DED servers running deathmatch 1 *==

The Makefile supplied has some build notes.  Default is a generic build for Linux only.
This is a stable build/version on Linux.




Questions etc.
==============

The code is commented in places (search for Nick), but it other places it isn't ;)  When I was
removing all the monster stuff, I just removed sections or functions as I was debugging with
little or no notes.  So the code is partially documented.




Revisions.
==========
Jan/2006 - In p-weapon.c the check for next weapon in NoAmmoWeaponChange looked for
a pickup item called 'ionrippergun'.  As I had fixed the code to return correctly from
this function, there was no next weapon called that - it should have been 'ionripper'.
This is fixed and works again - it only affects the Xatrix game.
Also I noticed that the rocket launcher isn't in this section of code either (or in any ID code)
so they either forgot it, or left it out for whatever reason.  I have added it anyway, so
that if your next weapon is the rocket launcher and you run out of ammo, it will change to
it and not the blaster.  I have added the grenade launcher to this check list too.


Dec/2005 - Added a forced deathmatch->value == 1 in case anybody starts a local server/client
game - this would make the code do funny things if deathmatch wasn't set.

Nov/2005 - Fixed a small bug with the intermission scoreboard, it now stays up during
map change.  Fixed the old Xatrix weapon frame errors if a weapon runs out of ammo and changes
whilst using quadfire.  Found and fixed a line using wrong strcmp.  Fixed an incorrect check
when in countdown mode.

Oct/2005 - Removed extra body gibs.  These are great fun, but do cause a few lag problems if
everybody gets fragged together - plus I have had reports of 'no free edicts' on map fact2,
which has a funny dead body in acid constantly spewing out limbs and stuff, although I cannot
reproduce this error at all - also note this is _not_ a dedicated DM map - it is a game map,
so therefore it is not expected (nor supported) to be used with this code - if game maps do
work, it is an added bonus.
Documented map list instructions.

Sept/2005 - Sourceforge release  - Really have fixed the trap code - it is now self contained
and doesn't use any grenade code at all.  Added Musashi's random map change code. Redesigned
all new intermission scoreboard.  Added deathscores count. Added improved string functions
within code for speed.

09/03/2005 - Finally fixed the trap code to stop it going off like a handgrenade if the trap is
'activated' just as the player is killer - it now gets thrown _all_ the time as I intended.
Made first map spawns all random (if level < 15 seconds and 'spawn furthest' and 'countdelay'is on).
Added the 'trap on spawn' protection.

01/03/2005 - Added the proper Phalanx obituaries (Xatrix seemed to have missed these?) - now:
instead of 'killed yourself' you get 'evaporated yourself' and radius damage is shown as 'fried by'.
Added spawn protection if a trap is on the spawn pad.
Fixed a small bug in timeleft counter if timelimit >= 18 (> 1000 seconds) as it was 3 chars only.
On first spawns on map change they are all randomly selected.

12/02/2005 - Added timeleft CVAR, and head rolling death.  I moved the
timeleft counter to bottom right so frag counter goes back to where it belongs.
I think this is the final release. :)

30/01/2005 - Fixed a small bug with respawn code when countdelay is on.  Added ent file support
ripper damage and eom music as CVAR's.
03/01/2005 - Original release of this code to public.

Some of code/ideas and inspiration come from http://www.quakesrc.org/ forums.
Thanks to entfile stuff, I believe came from Quake2 jumpmod, via the above forums.
Thanks to Musashi of AwakeningII for the random map change code.
Thanks to Reiner (aka Fatty) for the bug report (via server logs :-) )  re fact2 crash.
nick@linicks (aka Lethe & Bill Stokes)
</pre>
