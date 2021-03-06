#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtins.h"

extern int _debug;

int echo( char *argv[] );
int lexit( char *argv[] );
int lcd( char *argv[] );
int lkill( char *argv[] );
int lls( char *argv[] );
int undefined( char *argv[] );

void onBuiltErr( char *name );


builtin_pair builtins_table[] = {
    {"exit", &lexit}, {"lecho", &echo}, {"lcd", &lcd}, {"lkill", &lkill}, {"lls", &lls}, {NULL, NULL}};

int runBuildIn( char *name, char **arg )
{
  builtin_pair *pair = builtins_table;

  while ( pair->name != NULL )
  {
    if ( strcmp( name, pair->name ) == 0 )
    {
      pair->fun( arg );
      return 1;
    }

    pair++;
  }

  return 0;
}

int echo( char *argv[] )
{
  int i = 1;

  if ( argv[i] ) writeOut( argv[i++] );

  while ( argv[i] )
  {
    writeOut( " " );
    writeOut( argv[i++] );
  }

  writeOut( "\n" );
  return 0;
}

int lexit( char *argv[] )
{
  if ( argv[1] != NULL )
  {
    onBuiltErr( argv[0] );
    return -1;
  }

  exit( 0 );
}


int lcd( char *argv[] )
{
  if ( argv[1] != NULL && argv[2] != NULL )
  {
    onBuiltErr( argv[0] );
    return -1;
  }

  char *dPath;

  if ( argv[1] == NULL )
  {
    dPath = getenv( "HOME" );
  }
  else
  {
    dPath = argv[1];
  }

  int res = chdir( dPath );

  if ( res == -1 )
  {
    onBuiltErr( argv[0] );
    return errno;
  }

  return 0;
}


int lkill( char *argv[] )
{
  if ( argv[1] == NULL )
  {
    onBuiltErr( argv[0] );
    return -1;
  }

  long int sigNum, pid;
  int res;

  if ( argv[2] == NULL )
  {
    pid = strtol( argv[1], 0, 10 );

    if ( pid == 0 && strcmp( argv[1], "0" ) != 0 )
    {
      onBuiltErr( argv[0] );
      return -1;
    }

    sigNum = SIGTERM;
  }
  else
  {
    if ( argv[3] != NULL )
    {
      onBuiltErr( argv[0] );
      return -1;
    }

    sigNum = strtol( argv[1] + 1, 0, 10 );
    pid = strtol( argv[2], 0, 10 );

    if ( sigNum == 0 || ( pid == 0 && strcmp( argv[2], "0" ) != 0 ) )
    {
      onBuiltErr( argv[0] );
      return errno;
    }
  }

  if ( _debug ) printf( "__Trying to kill %d with %d", pid, sigNum );

  res = kill( pid, sigNum );

  if ( res == -1 )
  {
    onBuiltErr( argv[0] );
    return errno;
  }

  return 0;
}


int lls( char *argv[] )
{
  if ( argv[1] != NULL )
  {
    onBuiltErr( argv[0] );
    return -1;
  }

  DIR *pDir;
  struct dirent *pDirent;
  char cwd[1024];

  getcwd( cwd, sizeof( cwd ) );
  pDir = opendir( cwd );

  if ( pDir == NULL )
  {
    onBuiltErr( argv[0] );
    return -1;
  }

  while ( ( pDirent = readdir( pDir ) ) != NULL )
  {
    if ( pDirent->d_name[0] != '.' )
    {
      writeOut( pDirent->d_name );
      writeOut( "\n" );
    }
  }

  int res = closedir( pDir );

  if ( res == -1 )
  {
    onBuiltErr( argv[0] );
    return -1;
  }

  return 0;
}

void onBuiltErr( char *name )
{
  writeErr( "Builtin " );
  writeErr( name );
  writeErr( " error.\n" );
}

int undefined( char *argv[] )
{
  fprintf( stderr, "Command %s undefined.\n", argv[0] );
  return BUILTIN_ERROR;
}
