#include	<stdio.h>
#include	<stdlib.h>
#include	<time.h>
#include	"mpi.h"
#include	"HACApK_FPGA.h"
#ifndef _PGI
#include        <ISO_Fortran_binding.h>
#endif

//!***c_HACApK_adot_body_lfmtx
 void  c_hacapk_adot_body_lfmtx_
 (double *zau, stc_HACApK_leafmtxp *st_leafmtxp, double *zu, double *zbu){
 register int ip,il,it;
 int nlf,ndl,ndt,nstrtl,nstrtt,kt,itl,itt,ill;
 int st_lf_stride = st_leafmtxp->st_lf_stride;
 int a1size;
 
 nlf=st_leafmtxp->nlf;
  //fprintf(stderr,"nlf=%d \n",nlf);

 for(ip=0; ip<nlf; ip++){
   /**/
   stc_HACApK_leafmtx *sttmp;
   sttmp = (void *)(st_leafmtxp->st_lf) + st_lf_stride * ip;
   //fprintf(stderr, "%d: %p\n", ip, sttmp);
   /**/

   ndl   =sttmp->ndl; 
   ndt   =sttmp->ndt;
   nstrtl=sttmp->nstrtl; 
   nstrtt=sttmp->nstrtt;
   //fprintf(stderr,"ip=%d, ndl=%d, ndt=%d, nstrtl=%d, nstrtt=%d \n",ip,ndl,ndt,nstrtl,nstrtt);
   if(sttmp->ltmtx==1){
     /**/
     double *a2tmp = (double *)((void*)(sttmp->a1)+sttmp->a1size);
     /**/
     kt=sttmp->kt;
     
     for(il=0; il<kt; il++){
       zbu[il]=0.0;
       for(it=0; it<ndt; it++){
         itt=it+nstrtt-1;
         itl=it+il*ndt; 
         zbu[il] += sttmp->a1[itl]*zu[itt];
       }
     }
     for(il=0; il<kt; il++){
       for(it=0; it<ndl; it++){
         ill=it+nstrtl-1;
         itl=it+il*ndl; 
         zau[ill] += a2tmp[itl]*zbu[il];
       }
     }
   } else if(sttmp->ltmtx==2){
     for(il=0; il<ndl; il++){
       ill=il+nstrtl-1; 
       for(it=0; it<ndt; it++){
         itt=it+nstrtt-1; 
         itl=it+il*ndt;
         zau[ill] += sttmp->a1[itl]*zu[itt];
       }
     }
   }
 }
}

void c_hacapk_bicgstab_dump_
(
 stc_HACApK_leafmtxp *st_leafmtxp,
 double *u, double *b, double*param, int *nd
 )
 {
   int i, ip;
   FILE *F;
   int nlf=st_leafmtxp->nlf;
   printf("*nd = %d %d\n", *nd, st_leafmtxp->nd);

   F = fopen("dump_etc.dat", "wb");
   if(F==NULL){printf("can't open dump_etc.dat for output\n");return;}
   fwrite(nd, sizeof(int), 1, F);
   fclose(F);

   F = fopen("dump_u.dat", "wb");
   if(F==NULL){printf("can't open dump_u.dat for output\n");return;}
   fwrite(u, sizeof(double), *nd, F);
   fclose(F);

   F = fopen("dump_u.txt", "w");
   if(F==NULL){printf("can't open dump_u.txt for output\n");return;}
   for(i=0;i<*nd;i++)fprintf(F, "%f\n", u[i]);
   fclose(F);

   F = fopen("dump_b.dat", "wb");
   if(F==NULL){printf("can't open dump_b.dat for output\n");return;}
   fwrite(b, sizeof(double), *nd, F);
   fclose(F);

   F = fopen("dump_b.txt", "w");
   if(F==NULL){printf("can't open dump_b.txt for output\n");return;}
   for(i=0;i<*nd;i++)fprintf(F, "%f\n", b[i]);
   fclose(F);

   F = fopen("dump_h.dat", "wb");
   if(F==NULL){printf("can't open dump_h.dat for output\n");return;}
   fwrite(nd, sizeof(int), 1, F);
   fwrite(&st_leafmtxp->nlf, sizeof(int), 1, F);
   fwrite(&st_leafmtxp->nlfkt, sizeof(int), 1, F);
   fwrite(&st_leafmtxp->ktmax, sizeof(int), 1, F);
   fwrite(&st_leafmtxp->st_lf_stride, sizeof(int), 1, F);
   int st_lf_stride = st_leafmtxp->st_lf_stride;
   for(ip=0; ip<nlf; ip++){
	 stc_HACApK_leafmtx *sttmp;
	 sttmp = (void *)(st_leafmtxp->st_lf) + st_lf_stride * ip;
	 int ndl       = sttmp->ndl;
	 int ndt       = sttmp->ndt;
	 int nstrtl    = sttmp->nstrtl;
	 int nstrtt    = sttmp->nstrtt;
	 int ltmtx     = sttmp->ltmtx;
	 size_t a1size = sttmp->a1size;
	 fwrite(&ndl, sizeof(int), 1, F);
	 fwrite(&ndt, sizeof(int), 1, F);
	 fwrite(&nstrtl, sizeof(int), 1, F);
	 fwrite(&nstrtt, sizeof(int), 1, F);
	 fwrite(&ltmtx, sizeof(int), 1, F);
	 //fwrite(&a1size, sizeof(size_t), 1, F);
	 //printf("%d : %d %d %d %d %d %zu\n", ip, ndl, ndt, nstrtl, nstrtt, ltmtx, a1size);
	 if(sttmp->ltmtx==1){
	   double *a2tmp = (double *)((void*)(sttmp->a1)+sttmp->a1size);
	   int kt=sttmp->kt;
	   fwrite(&kt, sizeof(int), 1, F);
	   fwrite(sttmp->a1, sizeof(double), kt*ndt, F);
	   fwrite(a2tmp, sizeof(double), kt*ndl, F);
	 } else if(sttmp->ltmtx==2){
	   fwrite(sttmp->a1, sizeof(double), ndl*ndt, F);
	 }
   }
   fclose(F);
 }
