#include "g_local.h"

game_locals_t	game;
level_locals_t	level;
game_import_t	gi;
game_export_t	globals;
spawn_temp_t	st;

//Nick - 07/09/2005 - New rando map stuff - Musashi
level_locals_t g_oldmaps[MAX_OLDMAPS];

int	sm_meat_index;
int	snd_fry;
int meansOfDeath;

edict_t		*g_edicts;

cvar_t	*deathmatch;
//cvar_t	*coop; Nick 18/09/2005 - not in DM
cvar_t	*dmflags;
//cvar_t	*skill; Nick - 18/09/2005 - No skill in DM
cvar_t	*fraglimit;
cvar_t	*timelimit;
cvar_t	*password;
cvar_t	*spectator_password;
cvar_t	*maxclients;
cvar_t	*maxspectators;
cvar_t	*maxentities;
cvar_t	*g_select_empty;
cvar_t	*dedicated;

cvar_t	*filterban;

cvar_t	*sv_maxvelocity;
cvar_t	*sv_gravity;

cvar_t	*sv_rollspeed;
cvar_t	*sv_rollangle;
cvar_t	*gun_x;
cvar_t	*gun_y;
cvar_t	*gun_z;

cvar_t	*run_pitch;
cvar_t	*run_roll;
cvar_t	*bob_up;
cvar_t	*bob_pitch;
cvar_t	*bob_roll;

cvar_t	*sv_cheats;

cvar_t	*flood_msgs;
cvar_t	*flood_persecond;
cvar_t	*flood_waitdelay;

cvar_t	*sv_maplist;

// Nick - level countdown stuff
cvar_t	*countdelay;
// Nick Respawn invul
cvar_t	*r_invul;
// Nick End of map music
cvar_t *eom_muzic;
// Nick - Ripper self hurt
cvar_t *ripper_self;
// Nick - map time left
cvar_t *timeleft;

// Nick - 07/09/2005 - Random map stuff - Musashi
//CW++
cvar_t  *sv_map_file;
cvar_t  *sv_map_random;
// End Nick
// Nick - 17/09/2005
cvar_t *show_deaths;

// %%quadz - killable traps
cvar_t *sv_trap_health;
cvar_t *sv_trap_duration;
cvar_t *sv_trap_expl_damage;
cvar_t *sv_trap_expl_radius;
cvar_t *sv_trap_held_damage;
cvar_t *sv_trap_held_radius;


void SpawnEntities (char *mapname, char *entities, char *spawnpoint);
void ClientThink (edict_t *ent, usercmd_t *cmd);
qboolean ClientConnect (edict_t *ent, char *userinfo);
void ClientUserinfoChanged (edict_t *ent, char *userinfo);
void ClientDisconnect (edict_t *ent);
void ClientBegin (edict_t *ent);
void ClientCommand (edict_t *ent);
void RunEntity (edict_t *ent);
void WriteGame (char *filename, qboolean autosave);
void ReadGame (char *filename);
void WriteLevel (char *filename);
void ReadLevel (char *filename);
void InitGame (void);
void G_RunFrame (void);


//===================================================================


void ShutdownGame (void)
{
	gi.dprintf ("==== ShutdownGame ====\n");

	gi.FreeTags (TAG_LEVEL);
	gi.FreeTags (TAG_GAME);
}


/*
=================
GetGameAPI

Returns a pointer to the structure with all entry points
and global variables
=================
*/
game_export_t *GetGameAPI (game_import_t *import)
{
	gi = *import;

	globals.apiversion = GAME_API_VERSION;
	globals.Init = InitGame;
	globals.Shutdown = ShutdownGame;
	globals.SpawnEntities = SpawnEntities;

	globals.WriteGame = WriteGame;
	globals.ReadGame = ReadGame;
	globals.WriteLevel = WriteLevel;
	globals.ReadLevel = ReadLevel;

	globals.ClientThink = ClientThink;
	globals.ClientConnect = ClientConnect;
	globals.ClientUserinfoChanged = ClientUserinfoChanged;
	globals.ClientDisconnect = ClientDisconnect;
	globals.ClientBegin = ClientBegin;
	globals.ClientCommand = ClientCommand;

	globals.RunFrame = G_RunFrame;

	globals.ServerCommand = ServerCommand;

	globals.edict_size = sizeof(edict_t);

	return &globals;
}

#ifndef GAME_HARD_LINKED
// this is only here so the functions in q_shared.c and q_shwin.c can link
void Sys_Error (char *error, ...)
{
	va_list		argptr;
	char		text[1024];

	va_start (argptr, error);
	vsprintf (text, error, argptr);
	va_end (argptr);

	gi.error (ERR_FATAL, "%s", text);
}

void Com_Printf (char *msg, ...)
{
	va_list		argptr;
	char		text[1024];

	va_start (argptr, msg);
	vsprintf (text, msg, argptr);
	va_end (argptr);

	gi.dprintf ("%s", text);
}

#endif

//======================================================================


/*
=================
ClientEndServerFrames
=================
*/
void ClientEndServerFrames (void)
{
	int		i;
	edict_t	*ent;

	// calc the player views now that all pushing
	// and damage has been added
	for (i=0 ; i<maxclients->value ; i++)
	{
		ent = g_edicts + 1 + i;
		if (!ent->inuse || !ent->client)
			continue;
		ClientEndServerFrame (ent);
	}

}

/*
=================
CreateTargetChangeLevel

Returns the created target changelevel
=================
*/
edict_t *CreateTargetChangeLevel(char *map)
{
	edict_t *ent;

	ent = G_Spawn ();
	ent->classname = "target_changelevel";
	Com_sprintf(level.nextmap, sizeof(level.nextmap), "%s", map);
	ent->map = level.nextmap;
	return ent;
}

/* Nick - 08/09/2005 - this is gone - replaced by Musashi stuff
=================
EndDMLevel

The timelimit or fraglimit has been exceeded
=================

void EndDMLevel (void)
{
	edict_t		*ent;
	char *s, *t, *f;
	static const char *seps = " ,\n\r";

	// stay on same level flag
	if ((int)dmflags->value & DF_SAME_LEVEL)
	{
		BeginIntermission (CreateTargetChangeLevel (level.mapname) );
		return;
	}

	// see if it's in the map list
	if (*sv_maplist->string) {
		s = strdup(sv_maplist->string);
		f = NULL;
		t = strtok(s, seps);
		while (t != NULL) {
			if (Q_stricmp(t, level.mapname) == 0) {
				// it's in the list, go to the next one
				t = strtok(NULL, seps);
				if (t == NULL) { // end of list, go to first one
					if (f == NULL) // there isn't a first one, same level
						BeginIntermission (CreateTargetChangeLevel (level.mapname) );
					else
						BeginIntermission (CreateTargetChangeLevel (f) );
				} else
					BeginIntermission (CreateTargetChangeLevel (t) );
				free(s);
				return;
			}
			if (!f)
				f = t;
			t = strtok(NULL, seps);
		}
		free(s);
	}

	if (level.nextmap[0]) // go to a specific map
		BeginIntermission (CreateTargetChangeLevel (level.nextmap) );
	else {	// search for a changelevel
		ent = G_Find (NULL, FOFS(classname), "target_changelevel");
		if (!ent)
		{	// the map designer didn't include a changelevel,
			// so create a fake ent that goes back to the same level
			BeginIntermission (CreateTargetChangeLevel (level.mapname) );
			return;
		}
		BeginIntermission (ent);
	}
}
*/


//Nick - 07/09/2005 - Random map stuff - Musashi
/*
=================
FindNextMap

Returns the BSP name of the next map in the server's map list after the current level.
=================
*/
char *FindNextMap(FILE *iostream)
{
        char            bspname[MAX_QPATH];
        char            *buf = NULL;
        qboolean        finished = false;
        qboolean        found_first;
        int                     fnum;
        int                     nmaps = 0;

        while (!finished)
        {
                fnum = fscanf(iostream, "%s", bspname);
                if (fnum != EOF)
                {
                        if ((bspname[0] == '/') && (bspname[1] == '/')) //ignore commented-out lines
                                continue;

                        ++nmaps;
                        if (Q_stricmp(level.mapname, bspname) == 0)
                        {
                                finished = true;
                                fnum = fscanf(iostream, "%s", bspname);
                                if ((fnum == EOF) || ((bspname[0] == '/') && (bspname[1] == '/'))) //current map is las$
                                {
                                        rewind(iostream);
                                        found_first = false;
                                        while (!found_first)
                                        {
                                                fscanf(iostream, "%s", bspname);
                                                if ((bspname[0] == '/') && (bspname[1] == '/'))
                                                        continue;
                                                else
                                                        found_first = true;
                                        }
                                }
                        }
                }
                else    //current map not in list, or map list empty
                {
                        finished = true;
                        if (nmaps > 0)  //there's at least one map in the list, so select the first
                        {
                                rewind(iostream);
                                found_first = false;
                                while (!found_first)
                                {
                                        fscanf(iostream, "%s", bspname);
                                        if ((bspname[0] == '/') && (bspname[1] == '/'))
                                                continue;
                                        else
                                                found_first = true;
                                }
                        }
                        else
                        {
                                gi.dprintf("** Map list is empty\n");
                                sprintf(bspname, "q2dm1");
                        }
                }
        }
        //if (strlen(bspname))
        if (Q_strlenz(bspname))
                buf = G_CopyString(bspname);

        return buf;
}
//CW--

//CW++
/*
=================
FindRandomMap

Returns a BSP name from the server's map list (apart from the current level).
=================
*/
char *FindRandomMap(FILE *iostream)
{
        char            bspname[MAX_QPATH];
        char            *buf = NULL;
        qboolean        finished = false;
        qboolean        match_found;
        int                     fnum;
        int                     nmaps = 0;
        int                     nmaps_diff = 0;
        int                     count;
        int                     iterations = 0;
        int                     i;
	int			nmapsinlist; // Nick - replace MAX_OLDMAPS define.

//      Count the number of bsp names, and the number of bsp names that are different to the current
//      one, in the server's map list.
        while (!finished)
        {
                fnum = fscanf(iostream, "%s", bspname);
                if (fnum != EOF)
                {
                        if ((bspname[0] == '/') && (bspname[1] == '/')) //ignore commented-out lines
                                continue;

                        ++nmaps;
                        if (!Q_stricmp(level.mapname, bspname) == 0)
                                ++nmaps_diff;
                }
                else
                        finished = true;
        }

// If there's at least one bsp name that's different to the current one, randomly select one
// from the map list.

        if (nmaps > 0)
        {
                if (nmaps_diff > 0)
                {
                        finished = false;
                        while (!finished)
                        {
                                rewind(iostream);
                                count = (rand() % nmaps) + 1;
                                while (count > 0)
                                {
                                        if ((bspname[0] == '/') && (bspname[1] == '/'))         //ignore commented-out $
                                                continue;

                                        --count;
                                        fnum = fscanf(iostream, "%s", bspname);
                                        if (fnum == EOF)        // shouldn't happen!
                                        {
                                                finished = true;
                                                break;
                                        }
                                }

//Compare it to the list of the last MAX_OLDMAPS bsps played.
//Choose again if it's in this recent-bsp list, unless the number of unique maps
//in the server's map list is <= MAX_OLDMAPS (ie. we're going to get repetitions
//within the recent-bsp list). Don't replay the last map if possible.

                                if (nmaps_diff >= MAX_OLDMAPS)
                                {
                                        match_found = false;
                                        for (i = 0; i < MAX_OLDMAPS; i++)
                                        {
                        //gi.dprintf("g_oldmap: %s\n", g_oldmaps[i].mapname);

                                                if ( (Q_stricmp(g_oldmaps[i].mapname, bspname) == 0) )
                                                        match_found = true;
                			}
                                        if (!match_found)
                                                finished = true;
                                }
                                else
                                {
                                        if (!Q_stricmp(level.mapname, bspname) == 0)
                                                finished = true;
                                }

//If we've done a few iterations without producing a valid selection, it's likely
//that the server's map list is full of repeated bsp names (which b0rks the
//selection algorithm). In this case, just use the last bsp name chosen.


                                if (++iterations > 10)
                                        finished = true;
                        }

                }
                else    // no different maps, so replay the current one
                {
                        gi.dprintf("** Map list has no different maps\n");
                        sprintf(bspname, level.mapname);
                }
        }
        else
        {
                gi.dprintf("** Map list is empty\n");
                sprintf(bspname, "q2dm1");
        }

        //if (strlen(bspname))
        if (Q_strlenz(bspname))
                buf = G_CopyString(bspname);

        return buf;
}

/*
=================
EndDMLevel

The timelimit or fraglimit has been exceeded.
=================
*/
void EndDMLevel(void)
{
        edict_t *ent;

//CW++
        FILE    *mapstream;
        char    bspname[MAX_QPATH];
        char    endmsg[130];

        //level.starttime = 0.0;

//      Write the players' scores to the console.

// Nick change this to use original
//      if (sv_gametype->value == G_FFA)
//              PrintFFAScores();
//      else
//              PrintTeamScores();
// End Nick

//CW--

        // stay on same level flag
        if ((int)dmflags->value & DF_SAME_LEVEL)
        {
                BeginIntermission(CreateTargetChangeLevel(level.mapname));
                return;
        }


//      if (*level.forcemap)
//      {
//CW++
//              Check that the bsp exists. If not, then reload the current level.
//
//              if (!FileExists(level.forcemap, FILE_MAP))
//              {
//                      gi_bprintf(PRINT_CHAT, "** Cannot open bsp: \"%s\"\n   (Restarting current level).\n\n", level.$
//                      BeginIntermission(CreateTargetChangeLevel(level.mapname));
//              }
//              else
//CW--
//                      BeginIntermission(CreateTargetChangeLevel(level.forcemap));
//
//              return;
//      }

//CW++
//      Select a map from the list (either randomly or sequentially, as flagged).

	mapstream = (FILE *)OpenMaplistFile(true);
        if (mapstream != (FILE *)0)
        {
                if (sv_map_random->value)
                        sprintf(bspname, "%s", FindRandomMap(mapstream));
                else
                        sprintf(bspname, "%s", FindNextMap(mapstream));

                fclose(mapstream);
        	//gi.dprintf("File closed\n");
//              Check that the bsp exists. If not, then reload the current level.

                if (!FileExists(bspname, FILE_MAP))
                {
                        //gi_bprintf(PRINT_CHAT, "** Cannot open bsp: \"%s\"\n   (Restarting current level).\n\n", bspn$
                        BeginIntermission(CreateTargetChangeLevel(level.mapname));
                        return;
                }

		//gi.bprintf (PRINT_CHAT, "Next Map: %s\n", bspname);

                BeginIntermission(CreateTargetChangeLevel(bspname));
                return;
        }
//CW--

        if (level.nextmap[0])                   // go to a specific map
                BeginIntermission(CreateTargetChangeLevel(level.nextmap) );
        else                                                    // search for a changelevel
        {
                ent = G_Find(NULL, FOFS(classname), "target_changelevel");
                if (!ent)
                {       // the map designer didn't include a changelevel, so create a fake ent that goes back to the sa$
                        BeginIntermission(CreateTargetChangeLevel(level.mapname));
                        return;
                }
                BeginIntermission(ent);
        }
}
// End Nick


/*
=================
CheckDMRules
=================
*/
void CheckDMRules (void)
{
	int			i;
	gclient_t	*cl;

	if (level.intermissiontime)
		return;

	if (!deathmatch->value)
		return;

	if (timelimit->value)
	{
		if (level.time >= timelimit->value*60)
		{
			gi.bprintf (PRINT_HIGH, "Timelimit hit.\n");
			EndDMLevel ();
			return;
		}
	}

	if (fraglimit->value)
	{
		for (i=0 ; i<maxclients->value ; i++)
		{
			cl = game.clients + i;
			if (!g_edicts[i+1].inuse)
				continue;

			if (cl->resp.score >= fraglimit->value)
			{
				gi.bprintf (PRINT_HIGH, "Fraglimit hit.\n");
				EndDMLevel ();
				return;
			}
		}
	}
}


/*
=============
ExitLevel
=============
*/
void ExitLevel (void)
{
	int		i;
	edict_t	*ent;
	char	command [256];

	Com_sprintf (command, sizeof(command), "gamemap \"%s\"\n", level.changemap);
	gi.AddCommandString (command);
	level.changemap = NULL;
	level.exitintermission = 0;
	level.intermissiontime = 0;
	ClientEndServerFrames ();

	// clear some things before going to next level
	for (i=0 ; i<maxclients->value ; i++)
	{
		ent = g_edicts + 1 + i;
		if (!ent->inuse)
			continue;
		if (ent->health > ent->client->pers.max_health)
			ent->health = ent->client->pers.max_health;
	}

}

/*
================
G_RunFrame

Advances the world by 0.1 seconds
================
*/
void G_RunFrame (void)
{
	int		i;
	edict_t	*ent;

	level.framenum++;
	level.time = level.framenum*FRAMETIME;

	// choose a client for monsters to target this frame
	// Nick - remove AI_SetSightClient ();

	// exit intermissions

	if (level.exitintermission)
	{
		ExitLevel ();
		return;
	}

	//
	// treat each object in turn
	// even the world gets a chance to think
	//
	ent = &g_edicts[0];
	for (i=0 ; i<globals.num_edicts ; i++, ent++)
	{
		if (!ent->inuse)
			continue;

		level.current_entity = ent;

		VectorCopy (ent->s.origin, ent->s.old_origin);

		// if the ground entity moved, make sure we are still on it
		if ((ent->groundentity) && (ent->groundentity->linkcount != ent->groundentity_linkcount))
		{
			ent->groundentity = NULL;
			//if ( !(ent->flags & (FL_SWIM|FL_FLY)) && (ent->svflags & SVF_MONSTER) )
			//{
			//	M_CheckGround (ent);
			//}
		}

		if (i > 0 && i <= maxclients->value)
		{
			ClientBeginServerFrame (ent);
			continue;
		}

		G_RunEntity (ent);
	}

	// see if it is time to end a deathmatch
	CheckDMRules ();

	// build the playerstate_t structures for all players
	ClientEndServerFrames ();
}
