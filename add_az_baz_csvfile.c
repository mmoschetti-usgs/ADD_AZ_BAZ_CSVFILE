#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "params.h"


/* Calculate distance and azimuth/back-azimuth between two (lon,lat) coordinates 
 * using delaz.f subroutine
 */

// function declaration
void delaz_(float *lat1, float *lon1, float *lat2, float *lon2, float *dist, float *az, float *baz);
int getcols( const char * const line, const char * const delim, char ***out_storage);
void strip(char *s);
void assign_cols_flatfile(char **columns, float *stLat, float *stLon, float *evLon, float *evLat);
//void assign_cols_flatfile_reprocess(char **columns, float *stLat, float *stLon, float *evMag, float *evLon, float *evLat, float *evDep, int *evYear, int *evMon, int *evDay, int *evHour, int *evMin, float *evSec, char *network, char * stationNm);
//void assign_cols_flatfile(char **columns, float *stLat, float *stLon, float *evMag, float *evLon, float *evLat, float *evDep, int *evYear, int *evMon, int *evDay, int *evHour, int *evMin, float *evSec, char *network, char * stationNm);
char * replace(char const * const original, char const * const pattern, char const * const replacement );
int compute_epochTime(int yearIn, int monthIn, int dayIn, int hourIn, int minIn, int secIn);

/*--------------------------------------------------------------------------*/
void assign_cols_flatfile(char **columns, float *stLat, float *stLon, float *evLon, float *evLat)
/*--------------------------------------------------------------------------*/
{
//
  *stLon=atof(columns[1]);
  *stLat=atof(columns[2]);
  *evLon=atof(columns[3]);
  *evLat=atof(columns[4]);
//  fprintf(stderr,"assign_cols_flatfile, evMag/stLat: %f %f\n", *evMag, *stLat);
//  fprintf(stderr,"assign_cols_flatfile, network/stationNm: %s %s\n", network, stationNm);

}

/*--------------------------------------------------------------------------*/
void strip(char *s)
/*--------------------------------------------------------------------------*/
{
    char *p2 = s;
    while(*s != '\0') {
        if(*s != '\t' && *s != '\n') {
            *p2++ = *s++;
        } else {
            ++s;
        }
    }
    *p2 = '\0';
}

/*--------------------------------------------------------------------------*/
int main (int argc, char *argv[])
/*--------------------------------------------------------------------------*/
{
  FILE *fpcsvFileMod, *fpcsvFile;
  int cnt, cntMod=0, cntOrig=0, cnt1, cnt2, cnt3;
  int hlines;
  int cols_found, diffSec, minDiff;
  int evYear, evMon, evDay, evHour, evMin;
  int evYear2, evMon2, evDay2, evHour2, evMin2;
  int epochTimeFlatFile, epochTimeFlatFile2; 
  float objMisfit, objMtmp, diffMag;
  float stLon, stLat, evMag, evLon, evLat, evDep, evSec;
  float stLon2, stLat2, evMag2, evLon2, evLat2, evDep2, evSec2;
  float dist, dist2, az, baz, mindist;
  char csvFile[200], csvFileMod[200];
  char buff[BUFFLEN], buff2[BUFFLEN], buffFin[BUFFLEN];
  char network[5], stationNm[10], network2[5], stationNm2[10];
  char **columns;
  char **columns2;
  char **columnsOrig;
  char **columnsFinal;
  char delim[] = ",";

/* CHECK INPUT ARGUMENTS */
  if ( argc != 4 ) {
    fprintf(stderr,"USAGE: %s [csvfile] [output csvfile with dist/distDeg/az/baz]\n", argv[0]);
    exit(1);
  }
  sscanf(argv[1],"%s", csvFile);
  sscanf(argv[2],"%s", csvFileMod);

// Open flatfiles
  fprintf(stderr,"\nINPUT/OUTPUT FILES:\n");
  if ((fpcsvFile = fopen(csvFile, "r")) == NULL) {
    fprintf(stderr,"Could not open csvfile, %s\n", csvFile);
    exit(0);
  }
  else {
    fprintf(stderr,"Opened csvfile, %s\n", csvFile);
  }
//
  fpcsvFileMod = fopen(csvFileMod, "w");
  fprintf(stderr,"Writing to csvfile, %s\n\n", csvFileMod);

// READ/APPEND FLATFILE
// header lines
  hlines=2;
  for (cnt1=0; cnt1<hlines; cnt1++) {
    fgets(buff,BUFFLEN,fpcsvFile);
//    strip(buff);
    fprintf(fpcsvFileMod,"%s",buff);
  }
  cnt1=0;

//LOOP OVER CSVFILE LINES
  while( fgets(buff,BUFFLEN,fpcsvFile) ) {
    if ( strlen(buff) > BUFFLEN ) {
      fprintf(stderr,"Increase BUFFLEN from %d.\n", (int)BUFFLEN);
      exit(1);
    }
//    strip(buff);
    buff[strcspn(buff, "\r\n")] = 0;
//    fprintf(fpcsvFileMod,"%s,",buff);
    columns = NULL;
    cols_found = getcols(buff, delim, &columns);
//fprintf(stderr,"cols_found: %d\n",cols_found);
//fprintf(stderr,"columns: %d %s %s\n",atoi(columns[0]),columns[1], columns[2]);
    assign_cols_flatfile(columns, &stLat, &stLon, &evLon, &evLat);
    free(columns);
    delaz_(&stLat,&stLon,&evLat,&evLon,&dist,&az,&baz);
    fprintf(stderr,"%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f\n", stLat, stLon, evLat, evLon, dist, az, baz);
}

// close files
  fclose(fpcsvFile);
  fclose(fpcsvFileMod);

  return 0;
}

