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

File   : file_operand_value.c
Project  : ESA_POLSARPRO
Authors  : Eric POTTIER
Version  : 1.0
Creation : 08/2012
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

Description :  File (operand) Value = File

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

/* LOCAL VARIABLES */
  FILE *in_file1, *out_file;
  char file_in1[FilePathLength], file_out[FilePathLength];  
  char type_in1[10], type_in2[10], type_out[10];  
  char operand[10];
  
/* Internal variables */
  int lig, col;
  float valR, valI;
  float xr, xi;
  float mod, arg;
  
/* Matrix arrays */
  float **M_in1;
  float **M_out;

/********************************************************************
********************************************************************/
/* USAGE */

strcpy(UsageHelp,"\nfile_operand_value.exe\n");
strcat(UsageHelp,"\nParameters:\n");
strcat(UsageHelp," (string)	-if  	input file name 1\n");
strcat(UsageHelp," (string)	-it  	input file type 1\n");
strcat(UsageHelp," (float) 	-ivr 	input real value\n");
strcat(UsageHelp," (float) 	-ivi 	input imag value\n");
strcat(UsageHelp," (string)	-it2 	input file type 2\n");
strcat(UsageHelp," (string)	-of  	output file name\n");
strcat(UsageHelp," (string)	-ot  	output file type\n");
strcat(UsageHelp," (string)	-op  	operand\n");
strcat(UsageHelp," (int)   	-ofr 	Offset Row\n");
strcat(UsageHelp," (int)   	-ofc 	Offset Col\n");
strcat(UsageHelp," (int)   	-fnr 	Final Number of Row\n");
strcat(UsageHelp," (int)   	-fnc 	Final Number of Col\n");
strcat(UsageHelp,"\nOptional Parameters:\n");
strcat(UsageHelp," (string)	-mask	mask file (valid pixels)\n");
strcat(UsageHelp," (int)   	-mem 	Allocated memory for blocksize determination (in Mb)\n");
strcat(UsageHelp," (string)	-errf	memory error file\n");
strcat(UsageHelp," (noarg) 	-help	displays this message\n");

/********************************************************************
********************************************************************/
/* PROGRAM START */

if(get_commandline_prm(argc,argv,"-help",no_cmd_prm,NULL,0,UsageHelp)) {
  printf("\n Usage:\n%s\n",UsageHelp); exit(1);
  }

if(argc < 25) {
  edit_error("Not enough input arguments\n Usage:\n",UsageHelp);
  } else {
  get_commandline_prm(argc,argv,"-if",str_cmd_prm,file_in1,1,UsageHelp);
  get_commandline_prm(argc,argv,"-it",str_cmd_prm,type_in1,1,UsageHelp);
  get_commandline_prm(argc,argv,"-ivr",flt_cmd_prm,&valR,1,UsageHelp);
  get_commandline_prm(argc,argv,"-ivi",flt_cmd_prm,&valI,1,UsageHelp);
  get_commandline_prm(argc,argv,"-it2",str_cmd_prm,type_in2,1,UsageHelp);
  get_commandline_prm(argc,argv,"-of",str_cmd_prm,file_out,1,UsageHelp);
  get_commandline_prm(argc,argv,"-ot",str_cmd_prm,type_out,1,UsageHelp);
  get_commandline_prm(argc,argv,"-op",str_cmd_prm,operand,1,UsageHelp);
  get_commandline_prm(argc,argv,"-ofr",int_cmd_prm,&Off_lig,1,UsageHelp);
  get_commandline_prm(argc,argv,"-ofc",int_cmd_prm,&Off_col,1,UsageHelp);
  get_commandline_prm(argc,argv,"-fnr",int_cmd_prm,&Sub_Nlig,1,UsageHelp);
  get_commandline_prm(argc,argv,"-fnc",int_cmd_prm,&Sub_Ncol,1,UsageHelp);

  get_commandline_prm(argc,argv,"-errf",str_cmd_prm,file_memerr,0,UsageHelp);

  MemoryAlloc = -1;
  get_commandline_prm(argc,argv,"-mem",int_cmd_prm,&MemoryAlloc,0,UsageHelp);
  MemoryAlloc = my_max(MemoryAlloc,1000);

  FlagValid = 0;strcpy(file_valid,"");
  get_commandline_prm(argc,argv,"-mask",str_cmd_prm,file_valid,0,UsageHelp);
  if (strcmp(file_valid,"") != 0) FlagValid = 1;
  }

/********************************************************************
********************************************************************/

  check_file(file_in1);
  check_file(file_out);
  if (FlagValid == 1) check_file(file_valid);

  NwinL = 1; NwinC = 1;
  NwinLM1S2 = (NwinL - 1) / 2;
  NwinCM1S2 = (NwinC - 1) / 2;

  NpolarIn = 1; NpolarOut = 1;
  Nlig = Sub_Nlig; Ncol = Sub_Ncol;

  /********************************************************************
********************************************************************/
/* INPUT FILE OPENING*/
  if ((in_file1 = fopen(file_in1, "rb")) == NULL)
    edit_error("Could not open input file : ", file_in1);

  if (FlagValid == 1) 
    if ((in_valid = fopen(file_valid, "rb")) == NULL)
      edit_error("Could not open input file : ", file_valid);

/* OUTPUT FILE OPENING*/
  if ((out_file = fopen(file_out, "wb")) == NULL)
    edit_error("Could not open input file : ", file_out);
  
/********************************************************************
********************************************************************/
/* MEMORY ALLOCATION */
/*
MemAlloc = NBlockA*Nlig + NBlockB
*/ 

/* Local Variables */
  NBlockA = 0; NBlockB = 0;
  /* Mask = (Nlig+NwinL)*(Ncol+NwinC) */ 
  NBlockA += Ncol+NwinC; NBlockB += NwinL*(Ncol+NwinC);

  if (strcmp(type_in1,"cmplx") == 0 ) {
    /* Min1 = 2*(Nlig+NwinL)*(Ncol+NwinC) */
    NBlockA += 2*(Ncol+NwinC); NBlockB += 2*NwinL*(Ncol+NwinC);
    } else {
    /* Min1 = (Nlig+NwinL)*(Ncol+NwinC) */
    NBlockA += (Ncol+NwinC); NBlockB += 2*NwinL*(Ncol+NwinC);
    }

  if (strcmp(type_out,"cmplx") == 0 ) {
    /* Mout = 2*Sub_Nlig*Sub_Ncol */
    NBlockA += 2*Sub_Ncol; NBlockB += 0;
    } else {
    /* Mout = Sub_Nlig*Sub_Ncol */
    NBlockA += Sub_Ncol; NBlockB += 0;
    }
  
/* Reading Data */
  NBlockB += Ncol + 2*Ncol + NpolarIn*2*Ncol + NpolarOut*NwinL*(Ncol+NwinC);

  memory_alloc(file_memerr, Sub_Nlig, NwinL, &NbBlock, NligBlock, NBlockA, NBlockB, MemoryAlloc);

/********************************************************************
********************************************************************/
/* MATRIX ALLOCATION */

  _VC_in = vector_float(2*Ncol);
  _VF_in = vector_float(Ncol);
  _MC_in = matrix_float(4,2*Ncol);
  _MF_in = matrix3d_float(NpolarOut,NwinL, Ncol+NwinC);

/*-----------------------------------------------------------------*/   

  Valid = matrix_float(NligBlock[0] + NwinL, Sub_Ncol + NwinC);

  if (strcmp(type_in1,"cmplx") == 0 ) 
    M_in1 = matrix_float(NligBlock[0] + NwinL, 2*(Ncol + NwinC));
  if (strcmp(type_in1,"float") == 0 ) 
    M_in1 = matrix_float(NligBlock[0] + NwinL, (Ncol + NwinC));
    
  if (strcmp(type_out,"cmplx") == 0 ) 
    M_out = matrix_float(NligBlock[0] + NwinL, 2*(Ncol + NwinC));
  if (strcmp(type_out,"float") == 0 ) 
    M_out = matrix_float(NligBlock[0] + NwinL, (Ncol + NwinC));

/********************************************************************
********************************************************************/
/* MASK VALID PIXELS (if there is no MaskFile */
  if (FlagValid == 0) 
    for (lig = 0; lig < NligBlock[0] + NwinL; lig++) 
      for (col = 0; col < Sub_Ncol + NwinC; col++) 
        Valid[lig][col] = 1.;

/********************************************************************
********************************************************************/
/* DATA PROCESSING */

for (Nb = 0; Nb < NbBlock; Nb++) {

  if (NbBlock > 2) PrintfLine(Nb,NbBlock);

  if (FlagValid == 1) read_block_matrix_float(in_valid, Valid, Nb, NbBlock, NligBlock[Nb], Sub_Ncol, NwinL, NwinC, Off_lig, Off_col, Ncol);

  if (strcmp(type_in1,"cmplx") == 0 ) 
    read_block_matrix_cmplx(in_file1, M_in1, Nb, NbBlock, NligBlock[Nb], Sub_Ncol, NwinL, NwinC, Off_lig, Off_col, Ncol);
  if (strcmp(type_in1,"float") == 0 ) 
    read_block_matrix_float(in_file1, M_in1, Nb, NbBlock, NligBlock[Nb], Sub_Ncol, NwinL, NwinC, Off_lig, Off_col, Ncol);

  for (lig = 0; lig < NligBlock[Nb]; lig++) {
    PrintfLine(lig,NligBlock[Nb]);
    for (col = 0; col < Sub_Ncol; col++) {
      if (Valid[lig][col] == 1.) {
        if (strcmp(type_in1,"cmplx") == 0 ) {
          if (strcmp(type_in2,"cmplx") == 0 ) {
            if (strcmp(operand,"addval") == 0 ) {
              M_out[lig][2*col] = M_in1[lig][2*col]+valR;
              M_out[lig][2*col+1] = M_in1[lig][2*col+1]+valI;
              }
            if (strcmp(operand,"subval") == 0 ) {
              M_out[lig][2*col] = M_in1[lig][2*col]-valR;
              M_out[lig][2*col+1] = M_in1[lig][2*col+1]-valI;
              }
            if (strcmp(operand,"mulval") == 0 ) {
              xr = M_in1[lig][2*col]+valR-M_in1[lig][2*col+1]+valI;
              xi = M_in1[lig][2*col]+valI+M_in1[lig][2*col+1]+valR;
              M_out[lig][2*col] = xr;
              M_out[lig][2*col+1] = xi;
              }
            if (strcmp(operand,"divval") == 0 ) {
              xr = M_in1[lig][2*col]+valR+M_in1[lig][2*col+1]+valI;
              xi = -M_in1[lig][2*col]+valI+M_in1[lig][2*col+1]+valR;
              M_out[lig][2*col] = xr/(valR*valR+valI*valI);
              M_out[lig][2*col+1] = xi/(valR*valR+valI*valI);
              }
            }
          if (strcmp(type_in2,"float") == 0 ) {
            if (strcmp(operand,"addval") == 0 ) {
              M_out[lig][2*col] = M_in1[lig][2*col]+valR;
              M_out[lig][2*col+1] = M_in1[lig][2*col+1];
              }
            if (strcmp(operand,"subval") == 0 ) {
              M_out[lig][2*col] = M_in1[lig][2*col]-valR;
              M_out[lig][2*col+1] = M_in1[lig][2*col+1];
              }
            if (strcmp(operand,"mulval") == 0 ) {
              M_out[lig][2*col] = M_in1[lig][2*col]*valR;
              M_out[lig][2*col+1] = M_in1[lig][2*col+1]*valR;
              }
            if (strcmp(operand,"divval") == 0 ) {
              M_out[lig][2*col] = M_in1[lig][2*col]/(valR+eps);
              M_out[lig][2*col+1] = M_in1[lig][2*col+1]/(valR+eps);
              }
            if (strcmp(operand,"xy") == 0 ) {
              mod = sqrt(M_in1[lig][2*col]*M_in1[lig][2*col]+M_in1[lig][2*col+1]*M_in1[lig][2*col+1]);
              arg = atan2(M_in1[lig][2*col+1],M_in1[lig][2*col]);
              M_out[lig][2*col] = exp(valR*log(mod))*cos(valR*arg);
              M_out[lig][2*col+1] = exp(valR*log(mod))*sin(valR*arg);
              }
            }
          }
          
        if (strcmp(type_in1,"float") == 0 ) {
          if (strcmp(type_in2,"cmplx") == 0 ) {
            if (strcmp(operand,"addval") == 0 ) {
              M_out[lig][2*col] = M_in1[lig][col]+valR;
              M_out[lig][2*col+1] = valI;
              }
            if (strcmp(operand,"subval") == 0 ) {
              M_out[lig][2*col] = M_in1[lig][col]-valR;
              M_out[lig][2*col+1] = -valI;
              }
            if (strcmp(operand,"mulval") == 0 ) {
              M_out[lig][2*col] = M_in1[lig][col]*valR;
              M_out[lig][2*col+1] = M_in1[lig][col]*valI;
              }
            if (strcmp(operand,"divval") == 0 ) {
              M_out[lig][2*col] = M_in1[lig][col]*valR/(valR*valR+valI*valI);
              M_out[lig][2*col+1] = -M_in1[lig][col]*valI/(valR*valR+valI*valI);
              }
            }
          if (strcmp(type_in2,"float") == 0 ) {
            if (strcmp(operand,"addval") == 0 ) {
              M_out[lig][col] = M_in1[lig][col]+valR;
              }
            if (strcmp(operand,"subval") == 0 ) {
              M_out[lig][col] = M_in1[lig][col]-valR;
              }
            if (strcmp(operand,"mulval") == 0 ) {
              M_out[lig][col] = M_in1[lig][col]*valR;
              }
            if (strcmp(operand,"divval") == 0 ) {
              M_out[lig][col] = M_in1[lig][col]/(valR+eps);
              }
            if (strcmp(operand,"inf") == 0 ) {
              if (M_in1[lig][col] <= valR) M_out[lig][col] = 1.;
              else M_out[lig][col] = 0.;
              }
            if (strcmp(operand,"sup") == 0 ) {
              if (valR <= M_in1[lig][col]) M_out[lig][col] = 1.;
              else M_out[lig][col] = 0.;
              }
            if (strcmp(operand,"xy") == 0 ) {
              M_out[lig][col] = exp(valR*log(fabs(M_in1[lig][col])));
              }
            }
          }
     
        } else {
        if (strcmp(type_out,"cmplx") == 0 ) {M_out[lig][2*col] = 0.; M_out[lig][2*col+1] = 0.;}
        if (strcmp(type_out,"float") == 0 ) {M_out[lig][col] = 0.;}
        }
      }
    }
  
  if (strcmp(type_out,"cmplx") == 0 ) write_block_matrix_cmplx(out_file, M_out, NligBlock[Nb], Sub_Ncol, 0, 0, Sub_Ncol);
  if (strcmp(type_out,"float") == 0 ) write_block_matrix_float(out_file, M_out, NligBlock[Nb], Sub_Ncol, 0, 0, Sub_Ncol);

  } // NbBlock

/********************************************************************
********************************************************************/
/* MATRIX FREE-ALLOCATION */
/*
  free_matrix_float(M_in1, NligBlock[0] + NwinL);
  free_matrix_float(M_in2, NligBlock[0] + NwinL);
  free_matrix_float(M_out, NligBlock[0]);
  free_matrix_float(Valid, NligBlock[0] + NwinL);
*/  
/********************************************************************
********************************************************************/
/* OUTPUT FILE CLOSING*/
  fclose(out_file);

/* INPUT FILE CLOSING*/
  if (FlagValid == 1) fclose(in_valid);
  fclose(in_file1);

/********************************************************************
********************************************************************/

  return 1;
}


