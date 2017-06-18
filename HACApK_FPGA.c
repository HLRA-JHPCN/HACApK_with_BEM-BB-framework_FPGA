#include	<stdio.h>
#include	<stdlib.h>
#include	<time.h>
#include	"mpi.h"
#include	"HACApK_FPGA.h"
#include        <ISO_Fortran_binding.h>

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
