// include "ctype.h" //- needed if using the tolower() below
#include "g_local.h" 
 
char *ReadEntFile(char *filename) 
 { 
 
    FILE      *fp; 
    char      *filestring = NULL; 
    long int   i = 0; 
    int         ch; 
 
    for (;;) 
    { 
       fp = fopen(filename, "r"); 
       if (!fp) break; 
 
       for (i=0; (ch = fgetc(fp)) != EOF; i++) 
       ; 
 
       filestring = gi.TagMalloc(i+1, TAG_LEVEL); 
       if (!filestring) break; 
 
       fseek(fp, 0, SEEK_SET); 
       for (i=0; (ch = fgetc(fp)) != EOF; i++) 
       filestring[i] = ch; 
       filestring[i] = '\0'; 
 
       break; 
    } 
 
    if (fp) fclose(fp); 
 
    return(filestring); 
 } 
 
 char *LoadEntFile(char *mapname, char *entities) 
 { 
    char   entfilename[MAX_QPATH] = ""; 
    char   *newentities;      

	// Nick - move location of entfile directory.
	// Depends on game being built
#ifndef XATRIX 
    	sprintf(entfilename, "baseq2/entfiles/%s.ent", mapname); 
#else
    	sprintf(entfilename, "xatrix/entfiles/%s.ent", mapname); 
#endif

    	// Nick - I am not worried about this - turn on the above include ctype.h if required
	// convert string to all lowercase (for Linux) 
    	//for (i = 0; entfilename[i]; i++) 
    	//	entfilename[i] = tolower(entfilename[i]); 
 
    newentities = ReadEntFile(entfilename); 
 
    if (newentities) 
    {   //leave these dprints active they show up in the server init console section 
       gi.dprintf("Entity file %s.ent found\n", mapname); 
       return(newentities);   // reassign the ents 
    } 
    else 
    { 
       gi.dprintf("No .ent File for %s.bsp\n", mapname); 
       return(entities); 
    } 
 }
