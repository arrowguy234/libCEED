// Fortran interface
#include <ceed.h>
#include <ceed-impl.h>
#include <ceed-fortran-name.h>

#include <stdlib.h>
#include <string.h>

#define FORTRAN_REQUEST_IMMEDIATE -1
#define FORTRAN_REQUEST_ORDERED -2
#define FORTRAN_NULL -3
#define FORTRAN_ELEMRESTRICT_IDENTITY -1
#define FORTRAN_RESTRICTION_IDENTITY -1
#define FORTRAN_BASIS_COLOCATED -1
#define FORTRAN_QDATA_NONE -1
#define FORTRAN_VECTOR_ACTIVE -1
#define FORTRAN_VECTOR_NONE -2

static Ceed *Ceed_dict = NULL;
static int Ceed_count = 0;
static int Ceed_n = 0;
static int Ceed_count_max = 0;

#define fCeedInit FORTRAN_NAME(ceedinit,CEEDINIT)
void fCeedInit(const char* resource, int *ceed, int *err) {
  if (Ceed_count == Ceed_count_max) {
    Ceed_count_max += Ceed_count_max/2 + 1;
    CeedRealloc(Ceed_count_max, &Ceed_dict);
  }

  Ceed *ceed_ = &Ceed_dict[Ceed_count];
  *err = CeedInit(resource, ceed_);

  if (*err == 0) {
    *ceed = Ceed_count++;
    Ceed_n++;
  }
}

#define fCeedDestroy FORTRAN_NAME(ceeddestroy,CEEDDESTROY)
void fCeedDestroy(int *ceed, int *err) {
  *err = CeedDestroy(&Ceed_dict[*ceed]);

  if (*err == 0) {
    Ceed_n--;
    if (Ceed_n == 0) {
      CeedFree(&Ceed_dict);
      Ceed_count = 0;
      Ceed_count_max = 0;
    }
  }
}

static CeedVector *CeedVector_dict = NULL;
static int CeedVector_count = 0;
static int CeedVector_n = 0;
static int CeedVector_count_max = 0;

#define fCeedVectorCreate FORTRAN_NAME(ceedvectorcreate,CEEDVECTORCREATE)
void fCeedVectorCreate(int *ceed, int *length, int *vec, int *err) {
  if (CeedVector_count == CeedVector_count_max) {
    CeedVector_count_max += CeedVector_count_max/2 + 1;
    CeedRealloc(CeedVector_count_max, &CeedVector_dict);
  }

  CeedVector* vec_ = &CeedVector_dict[CeedVector_count];
  *err = CeedVectorCreate(Ceed_dict[*ceed], *length, vec_);

  if (*err == 0) {
    *vec = CeedVector_count++;
    CeedVector_n++;
  }
}

#define fCeedVectorSetArray FORTRAN_NAME(ceedvectorsetarray,CEEDVECTORSETARRAY)
void fCeedVectorSetArray(int *vec, int *memtype, int *copymode,
                         CeedScalar *array, int *err) {
  *err = CeedVectorSetArray(CeedVector_dict[*vec], *memtype, *copymode, array);
}

#define fCeedVectorGetArray FORTRAN_NAME(ceedvectorgetarray,CEEDVECTORGETARRAY)
//TODO Need Fixing, double pointer
void fCeedVectorGetArray(int *vec, int *memtype, CeedScalar *array, int *err) {
  CeedScalar *b;
  CeedVector vec_ = CeedVector_dict[*vec];
  *err = CeedVectorGetArray(vec_, *memtype, &b);
  if (*err == 0)
    memcpy(array, b, sizeof(CeedScalar)*vec_->length);
}

#define fCeedVectorGetArrayRead \
    FORTRAN_NAME(ceedvectorgetarrayread,CEEDVECTORGETARRAYREAD)
//TODO Need Fixing, double pointer
void fCeedVectorGetArrayRead(int *vec, int *memtype, CeedScalar *array,
                             int *err) {
  const CeedScalar *b;
  *err = CeedVectorGetArrayRead(CeedVector_dict[*vec], *memtype, &b);
  CeedVector vec_ = CeedVector_dict[*vec];
  if (*err == 0)
    memcpy(array, b, sizeof(CeedScalar)*vec_->length);
}

#define fCeedVectorRestoreArray \
    FORTRAN_NAME(ceedvectorrestorearray,CEEDVECTORRESTOREARRAY)
void fCeedVectorRestoreArray(int *vec, CeedScalar *array, int *err) {
  *err = CeedVectorRestoreArray(CeedVector_dict[*vec], &array);
}

#define fCeedVectorRestoreArrayRead \
    FORTRAN_NAME(ceedvectorrestorearrayread,CEEDVECTORRESTOREARRAYREAD)
void fCeedVectorRestoreArrayRead(int *vec, const CeedScalar *array, int *err) {
  *err = CeedVectorRestoreArrayRead(CeedVector_dict[*vec], &array);
}

#define fCeedVectorDestroy FORTRAN_NAME(ceedvectordestroy,CEEDVECTORDESTROY)
void fCeedVectorDestroy(int *vec, int *err) {
  *err = CeedVectorDestroy(&CeedVector_dict[*vec]);

  if (*err == 0) {
    CeedVector_n--;
    if (CeedVector_n == 0) {
      CeedFree(&CeedVector_dict);
      CeedVector_count = 0;
      CeedVector_count_max = 0;
    }
  }
}

static CeedElemRestriction *CeedElemRestriction_dict = NULL;
static int CeedElemRestriction_count = 0;
static int CeedElemRestriction_n = 0;
static int CeedElemRestriction_count_max = 0;

#define fCeedElemRestrictionCreate \
    FORTRAN_NAME(ceedelemrestrictioncreate, CEEDELEMRESTRICTIONCREATE)
void fCeedElemRestrictionCreate(int *ceed, int *nelements,
                                int *esize, int *ndof, int *ncomp, int *memtype, int *copymode,
                                const int *indices, int *elemrestriction, int *err) {
  if (CeedElemRestriction_count == CeedElemRestriction_count_max) {
    CeedElemRestriction_count_max += CeedElemRestriction_count_max/2 + 1;
    CeedRealloc(CeedElemRestriction_count_max, &CeedElemRestriction_dict);
  }

  const int *indices_ = indices;
  if (*indices == FORTRAN_ELEMRESTRICT_IDENTITY) {
    indices_ = NULL;
  }

  CeedElemRestriction *elemrestriction_ =
    &CeedElemRestriction_dict[CeedElemRestriction_count];
  *err = CeedElemRestrictionCreate(Ceed_dict[*ceed], *nelements, *esize, *ndof,
                                   *ncomp,
                                   *memtype, *copymode, indices_, elemrestriction_);

  if (*err == 0) {
    *elemrestriction = CeedElemRestriction_count++;
    CeedElemRestriction_n++;
  }
}

#define fCeedElemRestrictionCreateBlocked \
    FORTRAN_NAME(ceedelemrestrictioncreateblocked,CEEDELEMRESTRICTIONCREATEBLOCKED)
void fCeedElemRestrictionCreateBlocked(int *ceed, int *nelements,
                                       int *esize, int *blocksize, int *mtype, int *cmode,
                                       int *blkindices, int *elemr, int *err) {
  *err = CeedElemRestrictionCreateBlocked(Ceed_dict[*ceed], *nelements, *esize,
                                          *blocksize, *mtype, *cmode, blkindices, &CeedElemRestriction_dict[*elemr]);
}

static CeedRequest *CeedRequest_dict = NULL;
static int CeedRequest_count = 0;
static int CeedRequest_n = 0;
static int CeedRequest_count_max = 0;

#define fCeedElemRestrictionApply \
    FORTRAN_NAME(ceedelemrestrictionapply,CEEDELEMRESTRICTIONAPPLY)
void fCeedElemRestrictionApply(int *elemr, int *tmode, int *lmode,
                               int *uvec, int *ruvec, int *rqst, int *err) {
  int createRequest = 1;
  // Check if input is CEED_REQUEST_ORDERED(-2) or CEED_REQUEST_IMMEDIATE(-1)
  if (*rqst == FORTRAN_REQUEST_IMMEDIATE || *rqst == FORTRAN_REQUEST_ORDERED)
    createRequest = 0;

  if (createRequest && CeedRequest_count == CeedRequest_count_max) {
    CeedRequest_count_max += CeedRequest_count_max/2 + 1;
    CeedRealloc(CeedRequest_count_max, &CeedRequest_dict);
  }

  CeedRequest *rqst_;
  if      (*rqst == FORTRAN_REQUEST_IMMEDIATE) rqst_ = CEED_REQUEST_IMMEDIATE;
  else if (*rqst == FORTRAN_REQUEST_ORDERED  ) rqst_ = CEED_REQUEST_ORDERED;
  else rqst_ = &CeedRequest_dict[CeedRequest_count];

  *err = CeedElemRestrictionApply(CeedElemRestriction_dict[*elemr], *tmode,
                                  *lmode, CeedVector_dict[*uvec], CeedVector_dict[*ruvec], rqst_);

  if (*err == 0 && createRequest) {
    *rqst = CeedRequest_count++;
    CeedRequest_n++;
  }
}

#define fCeedRequestWait FORTRAN_NAME(ceedrequestwait, CEEDREQUESTWAIT)
void fCeedRequestWait(int *rqst, int *err) {
  // TODO Uncomment this once CeedRequestWait is implemented
  //*err = CeedRequestWait(&CeedRequest_dict[*rqst]);

  if (*err == 0) {
    CeedRequest_n--;
    if (CeedRequest_n == 0) {
      CeedFree(&CeedRequest_dict);
      CeedRequest_count = 0;
      CeedRequest_count_max = 0;
    }
  }
}

#define fCeedElemRestrictionDestroy \
    FORTRAN_NAME(ceedelemrestrictiondestroy,CEEDELEMRESTRICTIONDESTROY)
void fCeedElemRestrictionDestroy(int *elem, int *err) {
  *err = CeedElemRestrictionDestroy(&CeedElemRestriction_dict[*elem]);

  if (*err == 0) {
    CeedElemRestriction_n--;
    if (CeedElemRestriction_n == 0) {
      CeedFree(&CeedElemRestriction_dict);
      CeedElemRestriction_count = 0;
      CeedElemRestriction_count_max = 0;
    }
  }
}

static CeedBasis *CeedBasis_dict = NULL;
static int CeedBasis_count = 0;
static int CeedBasis_n = 0;
static int CeedBasis_count_max = 0;

#define fCeedBasisCreateTensorH1Lagrange \
    FORTRAN_NAME(ceedbasiscreatetensorh1lagrange, CEEDBASISCREATETENSORH1LAGRANGE)
void fCeedBasisCreateTensorH1Lagrange(int *ceed, int *dim,
                                      int *ndof, int *P, int *Q, int *quadmode, int *basis,
                                      int *err) {
  if (CeedBasis_count == CeedBasis_count_max) {
    CeedBasis_count_max += CeedBasis_count_max/2 + 1;
    CeedRealloc(CeedBasis_count_max, &CeedBasis_dict);
  }

  *err = CeedBasisCreateTensorH1Lagrange(Ceed_dict[*ceed], *dim, *ndof, *P, *Q,
                                         *quadmode, &CeedBasis_dict[CeedBasis_count]);

  if (*err == 0) {
    *basis = CeedBasis_count++;
    CeedBasis_n++;
  }
}

#define fCeedBasisCreateTensorH1 \
    FORTRAN_NAME(ceedbasiscreatetensorh1, CEEDBASISCREATETENSORH1)
void fCeedBasisCreateTensorH1(int *ceed, int *dim, int *ndof, int *P1d,
                              int *Q1d, const CeedScalar *interp1d, const CeedScalar *grad1d,
                              const CeedScalar *qref1d, const CeedScalar *qweight1d, int *basis, int *err) {
  if (CeedBasis_count == CeedBasis_count_max) {
    CeedBasis_count_max += CeedBasis_count_max/2 + 1;
    CeedRealloc(CeedBasis_count_max, &CeedBasis_dict);
  }

  *err = CeedBasisCreateTensorH1(Ceed_dict[*ceed], *dim, *ndof, *P1d, *Q1d,
                                 interp1d, grad1d,
                                 qref1d, qweight1d, &CeedBasis_dict[CeedBasis_count]);

  if (*err == 0) {
    *basis = CeedBasis_count++;
    CeedBasis_n++;
  }
}

#define fCeedBasisView FORTRAN_NAME(ceedbasisview, CEEDBASISVIEW)
void fCeedBasisView(int *basis, int *err) {
  *err = CeedBasisView(CeedBasis_dict[*basis], stdout);
}

#define fCeedQRFactorization \
    FORTRAN_NAME(ceedqrfactorization, CEEDQRFACTORIZATION)
void fCeedQRFactorization(CeedScalar *mat, CeedScalar *tau, int *m, int *n,
                          int *err) {
  *err = CeedQRFactorization(mat, tau, *m, *n);
}

#define fCeedBasisGetColocatedGrad \
    FORTRAN_NAME(ceedbasisgetcolocatedgrad, CEEDBASISGETCOLOCATEDGRAD)
void fCeedBasisGetColocatedGrad(int *basis, CeedScalar *colograd1d,
                                int *err) {
  *err = CeedBasisGetColocatedGrad(CeedBasis_dict[*basis], colograd1d);
}

#define fCeedBasisApply FORTRAN_NAME(ceedbasisapply, CEEDBASISAPPLY)
void fCeedBasisApply(int *basis, int *nelem, int *tmode, int *emode,
                     const CeedScalar *u, CeedScalar *v, int *err) {
  *err = CeedBasisApply(CeedBasis_dict[*basis], *nelem, *tmode, *emode, u, v);
}

#define fCeedBasisGetNumNodes \
    FORTRAN_NAME(ceedbasisgetnumnodes, CEEDBASISGETNUMNODES)
void fCeedBasisGetNumNodes(int *basis, int *P, int *err) {
  *err = CeedBasisGetNumNodes(CeedBasis_dict[*basis], P);
}

#define fCeedBasisGetNumQuadraturePoints \
    FORTRAN_NAME(ceedbasisgetnumquadraturepoints, CEEDBASISGETNUMQUADRATUREPOINTS)
void fCeedBasisGetNumQuadraturePoints(int *basis, int *Q, int *err) {
  *err = CeedBasisGetNumQuadraturePoints(CeedBasis_dict[*basis], Q);
}

#define fCeedBasisDestroy FORTRAN_NAME(ceedbasisdestroy,CEEDBASISDESTROY)
void fCeedBasisDestroy(int *basis, int *err) {
  *err = CeedBasisDestroy(&CeedBasis_dict[*basis]);

  if (*err == 0) {
    CeedBasis_n--;
    if (CeedBasis_n == 0) {
      CeedFree(&CeedBasis_dict);
      CeedBasis_count = 0;
      CeedBasis_count_max = 0;
    }
  }
}

#define fCeedGaussQuadrature FORTRAN_NAME(ceedgaussquadrature, CEEDGAUSSQUADRATURE)
void fCeedGaussQuadrature(int *Q, CeedScalar *qref1d, CeedScalar *qweight1d,
                          int *err) {
  *err = CeedGaussQuadrature(*Q, qref1d, qweight1d);
}

#define fCeedLobattoQuadrature \
    FORTRAN_NAME(ceedlobattoquadrature, CEEDLOBATTOQUADRATURE)
void fCeedLobattoQuadrature(int *Q, CeedScalar *qref1d, CeedScalar *qweight1d,
                            int *err) {
  *err = CeedLobattoQuadrature(*Q, qref1d, qweight1d);
}

static CeedQFunction *CeedQFunction_dict = NULL;
static int CeedQFunction_count = 0;
static int CeedQFunction_n = 0;
static int CeedQFunction_count_max = 0;

struct fContext {
  void (*f)(void *ctx, int *nq,
            const CeedScalar *u,const CeedScalar *u1,const CeedScalar *u2,
            const CeedScalar *u3,
            const CeedScalar *u4,const CeedScalar *u5,const CeedScalar *u6,
            const CeedScalar *u7,
            const CeedScalar *u8,const CeedScalar *u9,const CeedScalar *u10,
            const CeedScalar *u11,
            const CeedScalar *u12,const CeedScalar *u13,const CeedScalar *u14,
            const CeedScalar *u15,
            CeedScalar *v,CeedScalar *v1, CeedScalar *v2,CeedScalar *v3,
            CeedScalar *v4,CeedScalar *v5, CeedScalar *v6,CeedScalar *v7,
            CeedScalar *v8,CeedScalar *v9, CeedScalar *v10,CeedScalar *v11,
            CeedScalar *v12,CeedScalar *v13, CeedScalar *v14,CeedScalar *v15, int *err);
  void *innerctx;
};

static int CeedQFunctionFortranStub(void *ctx, int nq,
                                    const CeedScalar *const *u, CeedScalar *const *v) {
  struct fContext *fctx = ctx;
  int ierr;

  CeedScalar ctx_=1.0;
  fctx->f((void*)&ctx_,&nq,u[0],u[1],u[2],u[3],u[4],u[5],u[6],u[7],
          u[8],u[9],u[10],u[11],u[12],u[13],u[14],u[15],
          v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],
          v[8],v[9],v[10],v[11],v[12],v[13],v[14],v[15],&ierr);
  return ierr;
}

#define fCeedQFunctionCreateInterior \
    FORTRAN_NAME(ceedqfunctioncreateinterior, CEEDQFUNCTIONCREATEINTERIOR)
void fCeedQFunctionCreateInterior(int* ceed, int* vlength,
                                  void (*f)(void *ctx, int *nq,
                                      const CeedScalar *u,const CeedScalar *u1,const CeedScalar *u2,
                                      const CeedScalar *u3,
                                      const CeedScalar *u4,const CeedScalar *u5,const CeedScalar *u6,
                                      const CeedScalar *u7,
                                      const CeedScalar *u8,const CeedScalar *u9,const CeedScalar *u10,
                                      const CeedScalar *u11,
                                      const CeedScalar *u12,const CeedScalar *u13,const CeedScalar *u14,
                                      const CeedScalar *u15,
                                      CeedScalar *v,CeedScalar *v1, CeedScalar *v2,CeedScalar *v3,
                                      CeedScalar *v4,CeedScalar *v5, CeedScalar *v6,CeedScalar *v7,
                                      CeedScalar *v8,CeedScalar *v9, CeedScalar *v10,CeedScalar *v11,
                                      CeedScalar *v12,CeedScalar *v13, CeedScalar *v14,CeedScalar *v15,
                                      int *err),
                                  const char *focca, int *qf, int *err) {
  if (CeedQFunction_count == CeedQFunction_count_max) {
    CeedQFunction_count_max += CeedQFunction_count_max/2 + 1;
    CeedRealloc(CeedQFunction_count_max, &CeedQFunction_dict);
  }

  CeedQFunction *qf_ = &CeedQFunction_dict[CeedQFunction_count];
  *err = CeedQFunctionCreateInterior(Ceed_dict[*ceed], *vlength,
                                     CeedQFunctionFortranStub,focca, qf_);

  if (*err == 0) {
    *qf = CeedQFunction_count++;
    CeedQFunction_n++;
  }

  struct fContext *fctx;
  *err = CeedMalloc(1, &fctx);
  if (*err) return;
  fctx->f = f; fctx->innerctx = NULL;

  *err = CeedQFunctionSetContext(*qf_, fctx, sizeof(struct fContext));

}

#define fCeedQFunctionAddInput \
    FORTRAN_NAME(ceedqfunctionaddinput,CEEDQFUNCTIONADDINPUT)
void fCeedQFunctionAddInput(int *qf, const char *fieldname,
                            CeedInt *ncomp, CeedEvalMode *emode, int *err) {
  CeedQFunction qf_ = CeedQFunction_dict[*qf];

  *err = CeedQFunctionAddInput(qf_, fieldname, *ncomp, *emode);
}

#define fCeedQFunctionAddOutput \
    FORTRAN_NAME(ceedqfunctionaddoutput,CEEDQFUNCTIONADDOUTPUT)
void fCeedQFunctionAddOutput(int *qf, const char *fieldname,
                             CeedInt *ncomp, CeedEvalMode *emode, int *err) {
  CeedQFunction qf_ = CeedQFunction_dict[*qf];

  *err = CeedQFunctionAddOutput(qf_, fieldname, *ncomp, *emode);
}

#define fCeedQFunctionApply \
    FORTRAN_NAME(ceedqfunctionapply,CEEDQFUNCTIONAPPLY)
//TODO Need Fixing, double pointer
void fCeedQFunctionApply(int *qf, int *Q,
                         const CeedScalar *u,const CeedScalar *u1,const CeedScalar *u2,
                         const CeedScalar *u3,
                         const CeedScalar *u4,const CeedScalar *u5,const CeedScalar *u6,
                         const CeedScalar *u7,
                         const CeedScalar *u8,const CeedScalar *u9,const CeedScalar *u10,
                         const CeedScalar *u11,
                         const CeedScalar *u12,const CeedScalar *u13,const CeedScalar *u14,
                         const CeedScalar *u15,
                         CeedScalar *v,CeedScalar *v1, CeedScalar *v2,CeedScalar *v3,
                         CeedScalar *v4,CeedScalar *v5, CeedScalar *v6,CeedScalar *v7,
                         CeedScalar *v8,CeedScalar *v9, CeedScalar *v10,CeedScalar *v11,
                         CeedScalar *v12,CeedScalar *v13, CeedScalar *v14,CeedScalar *v15, int *err) {
  CeedQFunction qf_ = CeedQFunction_dict[*qf];
  const CeedScalar **in;
  *err = CeedCalloc(16, &in);
  if (*err) return;
  in[0] = u;
  in[1] = u1;
  in[2] = u2;
  in[3] = u3;
  in[4] = u4;
  in[5] = u5;
  in[6] = u6;
  in[7] = u7;
  in[8] = u8;
  in[9] = u9;
  in[10] = u10;
  in[11] = u11;
  in[12] = u12;
  in[13] = u13;
  in[14] = u14;
  in[15] = u15;
  CeedScalar **out;
  *err = CeedCalloc(16, &out);
  if (*err) return;
  out[0] = v;
  out[1] = v1;
  out[2] = v2;
  out[3] = v3;
  out[4] = v4;
  out[5] = v5;
  out[6] = v6;
  out[7] = v7;
  out[8] = v8;
  out[9] = v9;
  out[10] = v10;
  out[11] = v11;
  out[12] = v12;
  out[13] = v13;
  out[14] = v14;
  out[15] = v15;
  *err = CeedQFunctionApply(qf_, *Q, (const CeedScalar * const*)in, out);
  if (*err) return;

  *err = CeedFree(&in);
  if (*err) return;
  *err = CeedFree(&out);
}

#define fCeedQFunctionDestroy \
    FORTRAN_NAME(ceedqfunctiondestroy,CEEDQFUNCTIONDESTROY)
void fCeedQFunctionDestroy(int *qf, int *err) {
  CeedFree(&CeedQFunction_dict[*qf]->ctx);
  *err = CeedQFunctionDestroy(&CeedQFunction_dict[*qf]);

  if (*err) return;
  CeedQFunction_n--;
  if (CeedQFunction_n == 0) {
    *err = CeedFree(&CeedQFunction_dict);
    CeedQFunction_count = 0;
    CeedQFunction_count_max = 0;
  }
}

static CeedOperator *CeedOperator_dict = NULL;
static int CeedOperator_count = 0;
static int CeedOperator_n = 0;
static int CeedOperator_count_max = 0;

#define fCeedOperatorCreate \
    FORTRAN_NAME(ceedoperatorcreate, CEEDOPERATORCREATE)
void fCeedOperatorCreate(int* ceed,
                         int* qf, int* dqf, int* dqfT, int *op, int *err) {
  if (CeedOperator_count == CeedOperator_count_max)
    CeedOperator_count_max += CeedOperator_count_max/2 + 1,
                              CeedOperator_dict =
                                realloc(CeedOperator_dict, sizeof(CeedOperator)*CeedOperator_count_max);

  CeedOperator *op_ = &CeedOperator_dict[CeedOperator_count];

  CeedQFunction dqf_  = NULL, dqfT_ = NULL;
  if (*dqf  != FORTRAN_NULL) dqf_  = CeedQFunction_dict[*dqf ];
  if (*dqfT != FORTRAN_NULL) dqfT_ = CeedQFunction_dict[*dqfT];

  *err = CeedOperatorCreate(Ceed_dict[*ceed], CeedQFunction_dict[*qf], dqf_,
                            dqfT_, op_);
  if (*err) return;
  *op = CeedOperator_count++;
  CeedOperator_n++;
}

#define fCeedOperatorSetField \
    FORTRAN_NAME(ceedoperatorsetfield,CEEDOPERATORSETFIELD)
void fCeedOperatorSetField(int *op, const char *fieldname,
                           int *r, int *b, int *v, int *err) {
  CeedElemRestriction r_;
  CeedBasis b_;
  CeedVector v_;

  CeedOperator op_ = CeedOperator_dict[*op];

  if (*r == FORTRAN_NULL) {
    r_ = NULL;
  } else if (*r == FORTRAN_RESTRICTION_IDENTITY) {
    r_ = CEED_RESTRICTION_IDENTITY;
  } else {
    r_ = CeedElemRestriction_dict[*r];
  }
  if (*b == FORTRAN_NULL) {
    b_ = NULL;
  } else if (*b == FORTRAN_BASIS_COLOCATED) {
    b_ = CEED_BASIS_COLOCATED;
  } else {
    b_ = CeedBasis_dict[*b];
  }
  if (*v == FORTRAN_NULL) {
    v_ = NULL;
  } else if (*v == FORTRAN_VECTOR_ACTIVE) {
    v_ = CEED_VECTOR_ACTIVE;
  } else if (*v == FORTRAN_VECTOR_NONE) {
    v_ = CEED_VECTOR_NONE;
  } else {
    v_ = CeedVector_dict[*v];
  }

  *err = CeedOperatorSetField(op_, fieldname, r_, b_, v_);
}

#define fCeedOperatorApply FORTRAN_NAME(ceedoperatorapply, CEEDOPERATORAPPLY)
void fCeedOperatorApply(int *op, int *ustatevec,
                        int *resvec, int *rqst, int *err) {
  // TODO What vector arguments can be NULL?
  CeedVector resvec_;
  if (*resvec == FORTRAN_NULL) resvec_ = NULL;
  else resvec_ = CeedVector_dict[*resvec];

  int createRequest = 1;
  // Check if input is CEED_REQUEST_ORDERED(-2) or CEED_REQUEST_IMMEDIATE(-1)
  if (*rqst == -1 || *rqst == -2) {
    createRequest = 0;
  }

  if (createRequest && CeedRequest_count == CeedRequest_count_max) {
    CeedRequest_count_max += CeedRequest_count_max/2 + 1;
    CeedRealloc(CeedRequest_count_max, &CeedRequest_dict);
  }

  CeedRequest *rqst_;
  if (*rqst == -1) rqst_ = CEED_REQUEST_IMMEDIATE;
  else if (*rqst == -2) rqst_ = CEED_REQUEST_ORDERED;
  else rqst_ = &CeedRequest_dict[CeedRequest_count];

  *err = CeedOperatorApply(CeedOperator_dict[*op],
                           CeedVector_dict[*ustatevec], resvec_, rqst_);
  if (*err) return;
  if (createRequest) {
    *rqst = CeedRequest_count++;
    CeedRequest_n++;
  }
}

#define fCeedOperatorApplyJacobian \
    FORTRAN_NAME(ceedoperatorapplyjacobian, CEEDOPERATORAPPLYJACOBIAN)
void fCeedOperatorApplyJacobian(int *op, int *qdatavec, int *ustatevec,
                                int *dustatevec, int *dresvec, int *rqst, int *err) {
// TODO Uncomment this when CeedOperatorApplyJacobian is implemented
//  *err = CeedOperatorApplyJacobian(CeedOperator_dict[*op], CeedVector_dict[*qdatavec],
//             CeedVector_dict[*ustatevec], CeedVector_dict[*dustatevec],
//             CeedVector_dict[*dresvec], &CeedRequest_dict[*rqst]);
}

#define fCeedOperatorDestroy \
    FORTRAN_NAME(ceedoperatordestroy, CEEDOPERATORDESTROY)
void fCeedOperatorDestroy(int *op, int *err) {
  *err = CeedOperatorDestroy(&CeedOperator_dict[*op]);
  if (*err) return;
  CeedOperator_n--;
  if (CeedOperator_n == 0) {
    *err = CeedFree(&CeedOperator_dict);
    CeedOperator_count = 0;
    CeedOperator_count_max = 0;
  }
}
