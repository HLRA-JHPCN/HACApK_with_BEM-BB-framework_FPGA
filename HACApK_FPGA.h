typedef struct stc_HACApK_leafmtx{
  int ltmtx;
  int kt;
  int nstrtl;
  int ndl;
  int nstrtt;
  int ndt;
  size_t a1size; //
  double *a1;
  double *a2;
}stc_HACApK_leafmtx;
typedef struct stc_HACApK_leafmtxp{
  int nd;
  int nlf;
  int nlfkt;
  int ktmax;
  int st_lf_stride; //
  stc_HACApK_leafmtx *st_lf;
}stc_HACApK_leafmtxp;
