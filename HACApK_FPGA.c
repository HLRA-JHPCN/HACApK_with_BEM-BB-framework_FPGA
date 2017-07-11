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
 size_t a1size;
 
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

//!***c_HACApK_adot_body_lfmtx_hyp
 void  c_hacapk_adot_body_lfmtx_hyp_
 (double *zau, stc_HACApK_leafmtxp *st_leafmtxp,
  double *zu, int *pnd, int *ltmp){
 register int ip,il,it;
 int nlf,ndl,ndt,nstrtl,nstrtt,kt,itl,itt,ill;
 int st_lf_stride = st_leafmtxp->st_lf_stride;
 size_t a1size;
 int ith, nths, nthe;
 int ls, le;
 double *zaut, *zbut;
 int nd = pnd[0];

 nlf=st_leafmtxp->nlf;
 ith = omp_get_thread_num();
 nths = ltmp[ith];
 nthe = ltmp[ith+1]-1;
 zaut = (double*)malloc(sizeof(double)*nd); for(il=0;il<nd;il++)zaut[il]=0.0;
 zbut = (double*)malloc(sizeof(double)*st_leafmtxp->ktmax);
 ls = nd;
 le = 1;

 //for(ip=0; ip<nlf; ip++){
 for(ip=nths-1; ip<=nthe-1; ip++){
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
   if(nstrtl<ls)ls=nstrtl;
   if(nstrtl+ndl-1>le)le=nstrtl+ndl-1;
   if(sttmp->ltmtx==1){
     /**/
     double *a2tmp = (double *)((void*)(sttmp->a1)+sttmp->a1size);
     /**/
     kt=sttmp->kt;
	 for(il=0;il<kt;il++)zbut[il]=0.0;
     for(il=0; il<kt; il++){
       //zbut[il]=0.0;
       for(it=0; it<ndt; it++){
         itt=it+nstrtt-1;
         itl=it+il*ndt; 
         zbut[il] += sttmp->a1[itl]*zu[itt];
       }
     }
     for(il=0; il<kt; il++){
       for(it=0; it<ndl; it++){
         ill=it+nstrtl-1;
         itl=it+il*ndl; 
         zaut[ill] += a2tmp[itl]*zbut[il];
       }
     }
   } else if(sttmp->ltmtx==2){
     for(il=0; il<ndl; il++){
       ill=il+nstrtl-1; 
       for(it=0; it<ndt; it++){
         itt=it+nstrtt-1; 
         itl=it+il*ndt;
         zaut[ill] += sttmp->a1[itl]*zu[itt];
       }
     }
   }
 }
 for(il=ls-1;il<=le-1;il++){
#pragma omp atomic
   zau[il] += zaut[il];
 }
 free(zaut); free(zbut);
}
