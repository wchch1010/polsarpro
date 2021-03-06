/********************************************************************
PolSARpro v5.0 is free software; you can redistribute it and/or 
modify it under the terms of the GNU General Public License as 
published by the Free Software Foundation; either version 2 (1991) of
the License, or any later version. This program is distributed in the
hope that it will be useful, but WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. 

See the GNU General Public License (Version 2, 1991) for more details

*********************************************************************

File   : create_pauli_rgb_cce_file.c
Project  : ESA_POLSARPRO
Authors  : Eric POTTIER
Version  : 1.0
Creation : 07/2011
Update  :
*--------------------------------------------------------------------
INSTITUT D'ELECTRONIQUE et de TELECOMMUNICATIONS de RENNES (I.E.T.R)
UMR CNRS 6164

Image and Remote Sensing Group
SAPHIR Team 
(SAr Polarimetry Holography Interferometry Radargrammetry)

UNIVERSITY OF RENNES I
B�t. 11D - Campus de Beaulieu
263 Avenue G�n�ral Leclerc
35042 RENNES Cedex
Tel :(+33) 2 23 23 57 63
Fax :(+33) 2 23 23 69 63
e-mail: eric.pottier@univ-rennes1.fr

*--------------------------------------------------------------------

Description :  Creation of the PAULI RGB BMP file
Blue = 10log(T11)
Green = 10log(T33)
Red = 10log(T22)
with :
T11 = 0.5*|HH+VV|^2
T22 = 0.5*|HH-VV|^2
T33 = 2*|HV|^2

********************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifdef _WIN32
#include <dos.h>
#include <conio.h>
#endif

/* ALIASES  */

/* CONSTANTS  */

/* ROUTINES DECLARATION */
#include "../lib/PolSARproLib.h"

/********************************************************************
*********************************************************************
*
*            -- Function : Main
*
*********************************************************************
********************************************************************/
int main(int argc, char *argv[])
{

#define NPolType 5
/* LOCAL VARIABLES */
  int Config;
  char *PolTypeConf[NPolType] = {"S2", "C3", "T3", "C4", "T4"};

  FILE *filebmp;
  char FileOutput[FilePathLength];
  
/* Internal variables */
  int lig, col, ii, l, Npts;
  float xx;
  float minmin, maxmax;
  float minred, maxred;
  float mingreen, maxgreen;
  float minblue, maxblue;
  int automatic;
  int extracol;

/* Matrix arrays */
  float *datatmp;
  char *dataimg;
  float ***M_in;

/********************************************************************
********************************************************************/
/* USAGE */

strcpy(UsageHelp,"\ncreate_pauli_rgb_cce_file.exe\n");
strcat(UsageHelp,"\nParameters:\n");
strcat(UsageHelp," (string)	-id  	input directory\n");
strcat(UsageHelp," (string)	-of  	output RGB BMP file\n");
strcat(UsageHelp," (string)	-iodf	input-output data format\n");
strcat(UsageHelp," (int)   	-ofr 	Offset Row\n");
strcat(UsageHelp," (int)   	-ofc 	Offset Col\n");
strcat(UsageHelp," (int)   	-fnr 	Final Number of Row\n");
strcat(UsageHelp," (int)   	-fnc 	Final Number of Col\n");
strcat(UsageHelp," (int)   	-auto	Automatic color enhancement (1 / 0)\n");
strcat(UsageHelp," if automatic = 0\n");
strcat(UsageHelp," (float) 	-minb	blue channel : min value\n");
strcat(UsageHelp," (float) 	-maxb	blue channel : max value\n");
strcat(UsageHelp," (float) 	-minr	red channel : min value\n");
strcat(UsageHelp," (float) 	-maxr	red channel : max value\n");
strcat(UsageHelp," (float) 	-ming	green channel : min value\n");
strcat(UsageHelp," (float) 	-maxg	green channel : max value\n");
strcat(UsageHelp,"\nOptional Parameters:\n");
strcat(UsageHelp," (string)	-mask	mask file (valid pixels)\n");
strcat(UsageHelp," (int)   	-mem 	Allocated memory for blocksize determination (in Mb)\n");
strcat(UsageHelp," (string)	-errf	memory error file\n");
strcat(UsageHelp," (noarg) 	-help	displays this message\n");
strcat(UsageHelp," (noarg) 	-data	displays the help concerning Data Format parameter\n");

/********************************************************************
********************************************************************/

strcpy(UsageHelpDataFormat,"\nPolarimetric Input-Output Data Format\n\n");
for (ii=0; ii<NPolType; ii++) CreateUsageHelpDataFormat(PolTypeConf[ii]); 
strcat(UsageHelpDataFormat,"\n");

/********************************************************************
********************************************************************/
/* PROGRAM START */

if(get_commandline_prm(argc,argv,"-help",no_cmd_prm,NULL,0,UsageHelp)) {
  printf("\n Usage:\n%s\n",UsageHelp); exit(1);
  }
if(get_commandline_prm(argc,argv,"-data",no_cmd_prm,NULL,0,UsageHelpDataFormat)) {
  printf("\n Usage:\n%s\n",UsageHelpDataFormat); exit(1);
  }

automatic = 1;

if(argc < 17) {
  edit_error("Not enough input arguments\n Usage:\n",UsageHelp);
  } else {
  get_commandline_prm(argc,argv,"-id",str_cmd_prm,in_dir,1,UsageHelp);
  get_commandline_prm(argc,argv,"-of",str_cmd_prm,FileOutput,1,UsageHelp);
  get_commandline_prm(argc,argv,"-iodf",str_cmd_prm,PolType,1,UsageHelp);
  get_commandline_prm(argc,argv,"-ofr",int_cmd_prm,&Off_lig,1,UsageHelp);
  get_commandline_prm(argc,argv,"-ofc",int_cmd_prm,&Off_col,1,UsageHelp);
  get_commandline_prm(argc,argv,"-fnr",int_cmd_prm,&Sub_Nlig,1,UsageHelp);
  get_commandline_prm(argc,argv,"-fnc",int_cmd_prm,&Sub_Ncol,1,UsageHelp);
  get_commandline_prm(argc,argv,"-auto",int_cmd_prm,&automatic,1,UsageHelp);
  if (automatic == 0) {
    get_commandline_prm(argc,argv,"-minb",flt_cmd_prm,&minblue,1,UsageHelp);
    get_commandline_prm(argc,argv,"-maxb",flt_cmd_prm,&maxblue,1,UsageHelp);
    get_commandline_prm(argc,argv,"-minr",flt_cmd_prm,&minred,1,UsageHelp);
    get_commandline_prm(argc,argv,"-maxr",flt_cmd_prm,&maxred,1,UsageHelp);
    get_commandline_prm(argc,argv,"-ming",flt_cmd_prm,&mingreen,1,UsageHelp);
    get_commandline_prm(argc,argv,"-maxg",flt_cmd_prm,&maxgreen,1,UsageHelp);
    }

  get_commandline_prm(argc,argv,"-errf",str_cmd_prm,file_memerr,0,UsageHelp);

  MemoryAlloc = -1;
  get_commandline_prm(argc,argv,"-mem",int_cmd_prm,&MemoryAlloc,0,UsageHelp);
  MemoryAlloc = my_max(MemoryAlloc,1000);

  FlagValid = 0;strcpy(file_valid,"");
  get_commandline_prm(argc,argv,"-mask",str_cmd_prm,file_valid,0,UsageHelp);
  if (strcmp(file_valid,"") != 0) FlagValid = 1;

  Config = 0;
  for (ii=0; ii<NPolType; ii++) if (strcmp(PolTypeConf[ii],PolType) == 0) Config = 1;
  if (Config == 0) edit_error("\nWrong argument in the Polarimetric Data Format\n",UsageHelpDataFormat);
  }

/********************************************************************
********************************************************************/

  check_dir(in_dir);
  check_dir(out_dir);
  if (FlagValid == 1) check_file(file_valid);

  NwinL = 1; NwinC = 1;

  extracol = (int) fmod(4 - (int) fmod(3*Sub_Ncol, 4), 4);
  //Sub_Ncol = Sub_Ncol - (int) fmod((float) Sub_Ncol, 4.);
  NcolBMP = 3*Sub_Ncol + extracol;

/* INPUT/OUPUT CONFIGURATIONS */
  read_config(in_dir, &Nlig, &Ncol, PolarCase, PolarType);

/* POLAR TYPE CONFIGURATION */
  PolTypeConfig(PolType, &NpolarIn, PolTypeIn, &NpolarOut, PolTypeOut, PolarType);
  NpolarOut = 9; strcpy(PolTypeOut,"T3");
    
  file_name_in = matrix_char(NpolarIn,1024); 

/* INPUT/OUTPUT FILE CONFIGURATION */
  init_file_name(PolTypeIn, in_dir, file_name_in);

/* INPUT FILE OPENING*/
  for (Np = 0; Np < NpolarIn; Np++)
    if ((in_datafile[Np] = fopen(file_name_in[Np], "rb")) == NULL)
      edit_error("Could not open input file : ", file_name_in[Np]);

  if (FlagValid == 1) {
    if ((in_valid = fopen(file_valid, "rb")) == NULL)
      edit_error("Could not open input file : ", file_valid);
    }

/* OUTPUT FILE OPENING*/
  if ((filebmp = fopen(FileOutput, "wb")) == NULL)
    edit_error("Could not open output file : ", FileOutput);
  
/********************************************************************
********************************************************************/
/* MEMORY ALLOCATION */
/*
MemAlloc = NBlockA*Nlig + NBlockB
*/ 

/* Local Variables */
  NBlockA = 0; NBlockB = 0;

  /* Mask */ 
  NBlockA += Sub_Ncol; NBlockB += 0;

  /* DataTmp = Sub_Nlig*Sub_Ncol */
  NBlockB += Sub_Nlig*Sub_Ncol;

  /* Min = NpolarOut*Nlig*Ncol */
  NBlockA += NpolarOut*Sub_Ncol; NBlockB += 0;

/* Reading Data */
  NBlockB += Ncol + 2*Ncol + NpolarIn*2*Ncol + NpolarOut*NwinL*(Ncol+NwinC);

  memory_alloc(file_memerr, Sub_Nlig, 0, &NbBlock, NligBlock, NBlockA, NBlockB, MemoryAlloc);

/********************************************************************
********************************************************************/
/* MATRIX ALLOCATION */

  _VC_in = vector_float(2*Ncol);
  _VF_in = vector_float(Ncol);
  _MC_in = matrix_float(4,2*Ncol);
  _MF_in = matrix3d_float(NpolarOut,NwinL, Ncol+NwinC);

/*-----------------------------------------------------------------*/   
 
  M_in = matrix3d_float(NpolarOut, NligBlock[0], Sub_Ncol);
  Valid = matrix_float(NligBlock[0], Sub_Ncol);
  datatmp = vector_float(Sub_Nlig*Sub_Ncol);

/********************************************************************
********************************************************************/
/* MASK VALID PIXELS (if there is no MaskFile */
  if (FlagValid == 0) { 
    for (lig = 0; lig < NligBlock[0]; lig++) 
      for (col = 0; col < Sub_Ncol; col++) 
        Valid[lig][col] = 1.;
    }

/********************************************************************
********************************************************************/
if (automatic == 1) {
/********************************************************************
********************************************************************/
/* DATA PROCESSING BLUE CHANNEL */

for (Np = 0; Np < NpolarIn; Np++) rewind(in_datafile[Np]);
if (FlagValid == 1) rewind(in_valid);

Npts = -1;

for (Nb = 0; Nb < NbBlock; Nb++) {

  if (FlagValid == 1) read_block_matrix_float(in_valid, Valid, Nb, NbBlock, NligBlock[Nb], Sub_Ncol, 1, 1, Off_lig, Off_col, Ncol);

  if (NbBlock > 2) {printf("%f\r", 100. * Nb / (NbBlock - 1));fflush(stdout);}

  if (strcmp(PolTypeIn,"S2")==0) {
    read_block_S2_noavg(in_datafile, M_in, "T3", 9, Nb, NbBlock, NligBlock[Nb], Sub_Ncol, 1, 1, Off_lig, Off_col, Ncol);

    } else {

    /* Case of C,T or I */
    read_block_TCI_noavg(in_datafile, M_in, NpolarIn, Nb, NbBlock, NligBlock[Nb], Sub_Ncol, 1, 1, Off_lig, Off_col, Ncol);

    if (strcmp(PolTypeIn,"C3")==0) C3_to_T3(M_in, NligBlock[Nb], Sub_Ncol, 0, 0);
    if (strcmp(PolTypeIn,"C4")==0) C4_to_T3(M_in, NligBlock[Nb], Sub_Ncol, 0, 0);
    if (strcmp(PolTypeIn,"T4")==0) T4_to_T3(M_in, NligBlock[Nb], Sub_Ncol, 0, 0);
    }

  for (lig = 0; lig < NligBlock[Nb]; lig++) {
    for (col = 0; col < Sub_Ncol; col++) {
      if (Valid[lig][col] == 1.) {  
        Npts++;
        datatmp[Npts] =  fabs(M_in[T311][lig][col]);
        if (datatmp[Npts] > eps) datatmp[Npts] = 10. * log10(datatmp[Npts]);
        else Npts--;
        } /* valid */
      } /*col*/
    } /*lig*/
  
  } // NbBlock

  Npts++;
  
  minblue = INIT_MINMAX; maxblue = -minblue;

/* DETERMINATION OF THE MIN / MAX OF THE BLUE CHANNEL */
  MinMaxContrastMedian(datatmp, &minblue, &maxblue, Npts);

/********************************************************************
********************************************************************/
/* DATA PROCESSING RED CHANNEL */

for (Np = 0; Np < NpolarIn; Np++) rewind(in_datafile[Np]);
if (FlagValid == 1) rewind(in_valid);

Npts = -1;

for (Nb = 0; Nb < NbBlock; Nb++) {

  if (FlagValid == 1) read_block_matrix_float(in_valid, Valid, Nb, NbBlock, NligBlock[Nb], Sub_Ncol, 1, 1, Off_lig, Off_col, Ncol);

  if (NbBlock > 2) {printf("%f\r", 100. * Nb / (NbBlock - 1));fflush(stdout);}

  if (strcmp(PolTypeIn,"S2")==0) {
    read_block_S2_noavg(in_datafile, M_in, "T3", 9, Nb, NbBlock, NligBlock[Nb], Sub_Ncol, 1, 1, Off_lig, Off_col, Ncol);

    } else {

    /* Case of C,T or I */
    read_block_TCI_noavg(in_datafile, M_in, NpolarIn, Nb, NbBlock, NligBlock[Nb], Sub_Ncol, 1, 1, Off_lig, Off_col, Ncol);

    if (strcmp(PolTypeIn,"C3")==0) C3_to_T3(M_in, NligBlock[Nb], Sub_Ncol, 0, 0);
    if (strcmp(PolTypeIn,"C4")==0) C4_to_T3(M_in, NligBlock[Nb], Sub_Ncol, 0, 0);
    if (strcmp(PolTypeIn,"T4")==0) T4_to_T3(M_in, NligBlock[Nb], Sub_Ncol, 0, 0);
    }

  for (lig = 0; lig < NligBlock[Nb]; lig++) {
    for (col = 0; col < Sub_Ncol; col++) {
      if (Valid[lig][col] == 1.) {  
        Npts++;
        datatmp[Npts] =  fabs(M_in[T322][lig][col]);
        if (datatmp[Npts] > eps) datatmp[Npts] = 10. * log10(datatmp[Npts]);
        else Npts--;
        } /* valid */
      } /*col*/
    } /*lig*/
  
  } // NbBlock
  
  Npts++;

  minred = INIT_MINMAX; maxred = -minred;

/* DETERMINATION OF THE MIN / MAX OF THE RED CHANNEL */
  MinMaxContrastMedian(datatmp, &minred, &maxred, Npts);

/********************************************************************
********************************************************************/
/* DATA PROCESSING GREEN CHANNEL */

for (Np = 0; Np < NpolarIn; Np++) rewind(in_datafile[Np]);
if (FlagValid == 1) rewind(in_valid);

Npts = -1;

for (Nb = 0; Nb < NbBlock; Nb++) {

  if (FlagValid == 1) read_block_matrix_float(in_valid, Valid, Nb, NbBlock, NligBlock[Nb], Sub_Ncol, 1, 1, Off_lig, Off_col, Ncol);

  if (NbBlock > 2) {printf("%f\r", 100. * Nb / (NbBlock - 1));fflush(stdout);}

  if (strcmp(PolTypeIn,"S2")==0) {
    read_block_S2_noavg(in_datafile, M_in, "T3", 9, Nb, NbBlock, NligBlock[Nb], Sub_Ncol, 1, 1, Off_lig, Off_col, Ncol);

    } else {

    /* Case of C,T or I */
    read_block_TCI_noavg(in_datafile, M_in, NpolarIn, Nb, NbBlock, NligBlock[Nb], Sub_Ncol, 1, 1, Off_lig, Off_col, Ncol);

    if (strcmp(PolTypeIn,"C3")==0) C3_to_T3(M_in, NligBlock[Nb], Sub_Ncol, 0, 0);
    if (strcmp(PolTypeIn,"C4")==0) C4_to_T3(M_in, NligBlock[Nb], Sub_Ncol, 0, 0);
    if (strcmp(PolTypeIn,"T4")==0) T4_to_T3(M_in, NligBlock[Nb], Sub_Ncol, 0, 0);
    }

  for (lig = 0; lig < NligBlock[Nb]; lig++) {
    for (col = 0; col < Sub_Ncol; col++) {
      if (Valid[lig][col] == 1.) {  
        Npts++;
        datatmp[Npts] =  fabs(M_in[T333][lig][col]);
        if (datatmp[Npts] > eps) datatmp[Npts] = 10. * log10(datatmp[Npts]);
        else Npts--;
        } /* valid */
      } /*col*/
    } /*lig*/
  
  } // NbBlock
  
  Npts++;

  mingreen = INIT_MINMAX; maxgreen = -mingreen;

/* DETERMINATION OF THE MIN / MAX OF THE BLUE CHANNEL */
  MinMaxContrastMedian(datatmp, &mingreen, &maxgreen, Npts);
/********************************************************************
********************************************************************/
}
/*******************************************************************/
/* OUTPUT BMP FILE CREATION */
/*******************************************************************/
  minmin = INIT_MINMAX; maxmax = -minmin;

  if (minblue <= minmin) minmin = minblue;
  if (minred <= minmin) minmin = minred;
  if (mingreen <= minmin) minmin = mingreen;
  if (maxblue <= minmin) minmin = maxblue;
  if (maxred <= minmin) minmin = maxred;
  if (maxgreen <= minmin) minmin = maxgreen;
  
  if (maxmax <= minblue) maxmax = minblue;
  if (maxmax <= minred) maxmax = minred;
  if (maxmax <= mingreen) maxmax = mingreen;
  if (maxmax <= maxblue) maxmax = maxblue;
  if (maxmax <= maxred) maxmax = maxred;
  if (maxmax <= maxgreen) maxmax = maxgreen;

  minblue = minmin; minred = minmin; mingreen = minmin;
  maxblue = maxmax; maxred = maxmax; maxgreen = maxmax;

  dataimg = vector_char(NcolBMP);

/* BMP HEADER */
  //write_header_bmp_24bit(Sub_Nlig, NcolBMP, filebmp);
  write_header_bmp_24bit(Sub_Nlig, Sub_Ncol, filebmp);
  
/********************************************************************
********************************************************************/
/* DATA PROCESSING */

for (Np = 0; Np < NpolarIn; Np++) rewind(in_datafile[Np]);
if (FlagValid == 1) rewind(in_valid);

/* OFFSET READING */
  for (lig = 0; lig < Off_lig; lig++) {
    if (strcmp(PolTypeIn,"S2")==0) {
      for (Np = 0; Np < NpolarIn; Np++)
        fread(&datatmp[0], sizeof(float), 2 * Ncol, in_datafile[Np]);
      } else {
      for (Np = 0; Np < NpolarIn; Np++)
        fread(&datatmp[0], sizeof(float), Ncol, in_datafile[Np]);
      }
    if (FlagValid == 1) fread(&datatmp[0], sizeof(float), Ncol, in_valid);
    }

/* READ INPUT DATA FILE AND CREATE DATATMP 
   CORRESPONDING TO OUTPUTFORMAT */

  if (strcmp(PolTypeIn,"S2")==0) {
    for (Np = 0; Np < NpolarIn; Np++)
      fseek(in_datafile[Np], Sub_Nlig*Ncol*2*sizeof(float), SEEK_CUR);
    } else {
    for (Np = 0; Np < NpolarIn; Np++)
      fseek(in_datafile[Np], Sub_Nlig*Ncol*sizeof(float), SEEK_CUR);
    }
      
  if (FlagValid == 1) fseek(in_valid, Sub_Nlig*Ncol*sizeof(float), SEEK_CUR);

/********************************************************************
********************************************************************/

for (Nb = 0; Nb < NbBlock; Nb++) {

  if (FlagValid == 1) {
    fseek(in_valid, -NligBlock[Nb]*Ncol*sizeof(float), SEEK_CUR);
    read_block_matrix_float(in_valid, Valid, Nb, NbBlock, NligBlock[Nb], Sub_Ncol, 1, 1, 0, Off_col, Ncol);
    fseek(in_valid, -NligBlock[Nb]*Ncol*sizeof(float), SEEK_CUR);
    }
    
  if (NbBlock > 2) {printf("%f\r", 100. * Nb / (NbBlock - 1));fflush(stdout);}

  if (strcmp(PolTypeIn,"S2")==0) {

    for (Np = 0; Np < NpolarIn; Np++)
      fseek(in_datafile[Np], -NligBlock[Nb]*2*Ncol*sizeof(float), SEEK_CUR);
    read_block_S2_noavg(in_datafile, M_in, "T3", 9, Nb, NbBlock, NligBlock[Nb], Sub_Ncol, 1, 1, 0, Off_col, Ncol);
    for (Np = 0; Np < NpolarIn; Np++)
      fseek(in_datafile[Np], -NligBlock[Nb]*2*Ncol*sizeof(float), SEEK_CUR);

    } else {

    /* Case of C,T or I */
    for (Np = 0; Np < NpolarIn; Np++)
      fseek(in_datafile[Np], -NligBlock[Nb]*Ncol*sizeof(float), SEEK_CUR);
    read_block_TCI_noavg(in_datafile, M_in, NpolarIn, Nb, NbBlock, NligBlock[Nb], Sub_Ncol, 1, 1, 0, Off_col, Ncol);
    for (Np = 0; Np < NpolarIn; Np++)
      fseek(in_datafile[Np], -NligBlock[Nb]*Ncol*sizeof(float), SEEK_CUR);

    if (strcmp(PolTypeIn,"C3")==0) C3_to_T3(M_in, NligBlock[Nb], Sub_Ncol, 0, 0);
    if (strcmp(PolTypeIn,"C4")==0) C4_to_T3(M_in, NligBlock[Nb], Sub_Ncol, 0, 0);
    if (strcmp(PolTypeIn,"T4")==0) T4_to_T3(M_in, NligBlock[Nb], Sub_Ncol, 0, 0);
    }

  for (lig = 0; lig < NligBlock[Nb]; lig++) {
    for (col = 0; col < Sub_Ncol; col++) {
      if (Valid[NligBlock[Nb] - 1 - lig][col] == 1.) {  
        xx = fabs(M_in[T311][NligBlock[Nb] - 1 - lig][col]);
        if (xx <= eps) xx = eps;
        xx = 10 * log10(xx);
        if (xx > maxblue) xx = maxblue;
        if (xx < minblue) xx = minblue;
        xx = (xx - minblue) / (maxblue - minblue);
        if (xx > 1.) xx = 1.;        
        if (xx < 0.) xx = 0.;
        l = (int) (floor(255 * xx));
        dataimg[3 * col + 0] = (char) (l);

        xx = fabs(M_in[T333][NligBlock[Nb] - 1 - lig][col]);
        if (xx <= eps) xx = eps;
        xx = 10 * log10(xx);
        if (xx > maxgreen) xx = maxgreen;
        if (xx < mingreen) xx = mingreen;
        xx = (xx - mingreen) / (maxgreen - mingreen);
        if (xx > 1.) xx = 1.;
        if (xx < 0.) xx = 0.;
        l = (int) (floor(255 * xx));
        dataimg[3 * col + 1] =  (char) (l);

        xx = fabs(M_in[T322][NligBlock[Nb] - 1 - lig][col]);
        if (xx <= eps) xx = eps;
        xx = 10 * log10(xx);
        if (xx > maxred) xx = maxred;
        if (xx < minred) xx = minred;
        xx = (xx - minred) / (maxred - minred);
        if (xx > 1.) xx = 1.;
        if (xx < 0.) xx = 0.;
        l = (int) (floor(255 * xx));
        dataimg[3 * col + 2] =  (char) (l);
        } else {
        l = (int) (floor(255 * 0.));
        dataimg[3 * col + 0] = (char) (l);
        dataimg[3 * col + 1] = (char) (l);
        dataimg[3 * col + 2] = (char) (l);
        } /* valid */
      } /*col*/
    for (col = 0; col < extracol; col++) {
      l = (int) (floor(255 * 0.));
      dataimg[3 * Sub_Ncol + col] = (char) (l);
      } /*col*/
    fwrite(&dataimg[0], sizeof(char), NcolBMP, filebmp);
    } /*lig*/
  
  } // NbBlock

/********************************************************************
********************************************************************/
/* MATRIX FREE-ALLOCATION */
/*
  free_vector_char(dataimg);
  free_vector_float(datatmp);
  free_matrix3d_float(M_in, NpolarOut, NligBlock[0]);
  free_matrix_float(Valid, NligBlock[0]);
*/
/********************************************************************
********************************************************************/
/* OUTPUT FILE CLOSING*/
  fclose(filebmp);

/* INPUT FILE CLOSING*/
  if (FlagValid == 1) fclose(in_valid);
  for (Np = 0; Np < NpolarIn; Np++) fclose(in_datafile[Np]);

/********************************************************************
********************************************************************/

  return 1;
}


