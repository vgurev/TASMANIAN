/*
 * Copyright (c) 2017, Miroslav Stoyanov
 *
 * This file is part of
 * Toolkit for Adaptive Stochastic Modeling And Non-Intrusive ApproximatioN: TASMANIAN
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
 *    and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse
 *    or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * UT-BATTELLE, LLC AND THE UNITED STATES GOVERNMENT MAKE NO REPRESENTATIONS AND DISCLAIM ALL WARRANTIES, BOTH EXPRESSED AND IMPLIED.
 * THERE ARE NO EXPRESS OR IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, OR THAT THE USE OF THE SOFTWARE WILL NOT INFRINGE ANY PATENT,
 * COPYRIGHT, TRADEMARK, OR OTHER PROPRIETARY RIGHTS, OR THAT THE SOFTWARE WILL ACCOMPLISH THE INTENDED RESULTS OR THAT THE SOFTWARE OR ITS USE WILL NOT RESULT IN INJURY OR DAMAGE.
 * THE USER ASSUMES RESPONSIBILITY FOR ALL LIABILITIES, PENALTIES, FINES, CLAIMS, CAUSES OF ACTION, AND COSTS AND EXPENSES, CAUSED BY, RESULTING FROM OR ARISING OUT OF,
 * IN WHOLE OR IN PART THE USE, STORAGE OR DISPOSAL OF THE SOFTWARE.
 */

#ifndef __TSG_C2FORT_CPP
#define __TSG_C2FORT_CPP

#include "TasmanianSparseGrid.hpp"

using std::cerr;
using std::endl;

using namespace TasGrid;

extern "C" void tsgc2fstr_(int *length, const char *str);
extern "C" void tsgc2fvec_(int *length, double *vect);
extern "C" void tsgc2fmat_(int *rows, int *cols, double *mat);

TasmanianSparseGrid **_tsg_grid_list;
int _tsg_num_grids;

extern "C"{

void tsggag_(int *num_active){
    *num_active = 0;
    for (int i = 0; i < _tsg_num_grids; i++ )
        if ( _tsg_grid_list[i] != 0 ) (*num_active)++;
}
void tsgend_(){
    if (_tsg_grid_list != 0){
        for(int i=0; i<_tsg_num_grids; i++){
            if (_tsg_grid_list[i] != 0) delete _tsg_grid_list[i];
        }
        delete[] _tsg_grid_list;
        _tsg_grid_list = 0;
        _tsg_num_grids = 0;
    }
}
void tsgnew_(int *returnID){
    if ( _tsg_grid_list == 0 ){
        _tsg_num_grids = 4; // assume we are working with 4 grids
        _tsg_grid_list = new TasmanianSparseGrid*[_tsg_num_grids];
        for(int i=0; i<_tsg_num_grids; i++) _tsg_grid_list[i] = 0;
    }
    int id = 0;
    while((id < _tsg_num_grids) && (_tsg_grid_list[id] != 0)) id++;
    // double the number of grids if the assumed number was too small
    if (id == _tsg_num_grids){
        TasmanianSparseGrid **_tsg_grid_list_old = _tsg_grid_list;
        _tsg_num_grids *= 2;
        _tsg_grid_list = new TasmanianSparseGrid*[_tsg_num_grids];
        for(int i=0; i<_tsg_num_grids/2; i++) _tsg_grid_list[i] = _tsg_grid_list_old[i];
        for(int i=_tsg_num_grids/2; i<_tsg_num_grids; i++) _tsg_grid_list[i] = 0;
        delete[] _tsg_grid_list_old;
        _tsg_grid_list_old = 0;
    }
    _tsg_grid_list[id] = new TasmanianSparseGrid();
    *returnID = id;
}
void tsgfre_(int *id){
    if (*id < _tsg_num_grids){
        if (_tsg_grid_list[*id] != 0){
            delete _tsg_grid_list[*id];
            _tsg_grid_list[*id] = 0;
        }
    }
    for (int i = 0; i < _tsg_num_grids; i++ )
        if ( _tsg_grid_list[i] != 0 ) return;
    tsgend_();
}
////////////////////////////////////////////////////////////////////////
//   MAIN INTERFACE
////////////////////////////////////////////////////////////////////////

void tsggvm_(int *ver){ *ver = TasmanianSparseGrid::getVersionMajor(); }
void tsggvn_(int *ver){ *ver = TasmanianSparseGrid::getVersionMinor(); }
void tsggli_(){
    // this is clumsy, may have to think of something else
    const char *lic = TasmanianSparseGrid::getLicense();
    int l = 0;
    while(lic[l] != '\0') l++;
    tsgc2fstr_(&l, lic);
}

// read/write
void tsgwri_(int *id, const char *filename, int *binary){ _tsg_grid_list[*id]->write(filename, (*binary != 0)); }
void tsgrea_(int *id, const char *filename, int *status){
    try{
        _tsg_grid_list[*id]->read(filename);
        *status = 1;
    }catch(std::runtime_error &e){
        cerr << e.what() << endl;
        *status = 0;
    }
}

// create
void tsgmg_(int *id, int *dimensions, int *outputs, int *depth, int *type, int *rule, int *opt_flags,
            const int *aniso_weights, double *alpha, double *beta, const char *customRuleFilename, const int *llimits){
    double al, be;
    const int  *aw, *ll;
    const char *cfn;

    aw  = (opt_flags[0] != 0 ) ? aniso_weights      : 0;
    al  = (opt_flags[1] != 0 ) ? *alpha             : 0.0;
    be  = (opt_flags[2] != 0 ) ? *beta              : 0.0;
    cfn = (opt_flags[3] != 0 ) ? customRuleFilename : 0;
    ll  = (opt_flags[4] != 0 ) ? llimits            : 0;

    _tsg_grid_list[*id]->makeGlobalGrid(*dimensions, *outputs, *depth,
        OneDimensionalMeta::getIOTypeInt(*type), OneDimensionalMeta::getIORuleInt(*rule),
        aw, al, be, cfn, ll);
}
void tsgms_(int *id, int *dimensions, int *outputs, int *depth, int *type, int *rule, int *opt_flags,
            const int *aniso_weights, const int *llimits){
    const int *aw, *ll;

    aw  = (opt_flags[0] != 0 ) ? aniso_weights      : 0;
    ll  = (opt_flags[1] != 0 ) ? llimits            : 0;

    _tsg_grid_list[*id]->makeSequenceGrid(*dimensions, *outputs, *depth,
        OneDimensionalMeta::getIOTypeInt(*type), OneDimensionalMeta::getIORuleInt(*rule), aw, ll);
}
void tsgml_(int *id, int *dimensions, int *outputs, int *depth, int *opt_flags,
            int *order, int *rule, const int *llimits){
    int ord, ru;
    const int *ll;

    ru  = (opt_flags[0] != 0) ? *rule   : 1;
    ord = (opt_flags[1] != 0) ? *order  : 1;
    ll  = (opt_flags[2] != 0) ? llimits : 0;

    _tsg_grid_list[*id]->makeLocalPolynomialGrid(*dimensions, *outputs, *depth, ord, OneDimensionalMeta::getIORuleInt(ru), ll);
}
void tsgmw_(int *id, int *dimensions, int *outputs, int *depth, int *opt_flags, int *order, const int *llimits){
    int ord;
    const int *ll;

    ord = (opt_flags[0] != 0) ? *order  : 1;
    ll  = (opt_flags[1] != 0) ? llimits : 0;

    _tsg_grid_list[*id]->makeWaveletGrid(*dimensions, *outputs, *depth, ord, ll);
}
void tsgmf_(int *id, int *dimensions, int *outputs, int *depth, int *type, int *opt_flags, const int *aniso_weights, const int *llimits){
    const int *ll, *aw;

    aw = (opt_flags[0] != 0 ) ? aniso_weights : 0;
    ll = (opt_flags[1] != 0 ) ? llimits       : 0;

    _tsg_grid_list[*id]->makeFourierGrid(*dimensions, *outputs, *depth, OneDimensionalMeta::getIOTypeInt(*type), aw, ll);
}

// copy/updare
void tsgcp_(int *id, int *source){ _tsg_grid_list[*id]->copyGrid(_tsg_grid_list[*source]); }

void tsgug_(int *id, int *depth, int *type, int *opt_flags, const int *anisotropic_weights){
    const int *aw;
    aw = (opt_flags[0] != 0) ? anisotropic_weights : 0;
    _tsg_grid_list[*id]->updateGlobalGrid(*depth, OneDimensionalMeta::getIOTypeInt(*type), aw);
}
void tsgus_(int *id, int *depth, int *type, int *opt_flags, const int *anisotropic_weights){
    const int *aw;
    aw = (opt_flags[0] != 0) ? anisotropic_weights : 0;
    _tsg_grid_list[*id]->updateSequenceGrid(*depth, OneDimensionalMeta::getIOTypeInt(*type), aw);
}

// getAlpha/Beta/Order/Dims/Outs/Rule //
void tsggal_(int *id, double *alpha){ *alpha = _tsg_grid_list[*id]->getAlpha(); }
void tsggbe_(int *id, double *beta){ *beta = _tsg_grid_list[*id]->getBeta(); }
void tsggor_(int *id, int *order){ *order = _tsg_grid_list[*id]->getOrder(); }
void tsggnd_(int *id, int *dims){ *dims = _tsg_grid_list[*id]->getNumDimensions(); }
void tsggno_(int *id, int *outs){ *outs = _tsg_grid_list[*id]->getNumOutputs(); }
void tsggru_(int *id, int *rule){ *rule = OneDimensionalMeta::getIORuleInt(_tsg_grid_list[*id]->getRule()); }

// getNumNeeded/Loaded/Points
void tsggnn_(int *id, int *num_points){ *num_points = _tsg_grid_list[*id]->getNumNeeded(); }
void tsggnl_(int *id, int *num_points){ *num_points = _tsg_grid_list[*id]->getNumLoaded(); }
void tsggnp_(int *id, int *num_points){ *num_points = _tsg_grid_list[*id]->getNumPoints(); }

// getLoaded/Needed/Points
void tsgglp_(int *id, double *points){ _tsg_grid_list[*id]->getLoadedPoints(points); }
void tsggdp_(int *id, double *points){ _tsg_grid_list[*id]->getNeededPoints(points); }
void tsggpp_(int *id, double *points){ _tsg_grid_list[*id]->getPoints(points); }

// getQuadrature/InterpolationWeights
void tsggqw_(int *id, double *weights){ _tsg_grid_list[*id]->getQuadratureWeights(weights); }
void tsggiw_(int *id, const double *x, double *weights){ _tsg_grid_list[*id]->getInterpolationWeights(x,weights); }

// set/is/clear/getDomainTransform
void tsgsdt_(int *id, const double *transform_a, const double *transform_b){ _tsg_grid_list[*id]->setDomainTransform(transform_a, transform_b); }
void tsgidt_(int *id, int *result){ *result = (_tsg_grid_list[*id]->isSetDomainTransfrom()) ? 1 : 0; }
void tsgcdt_(int *id){ _tsg_grid_list[*id]->clearDomainTransform(); }
void tsggdt_(int *id, double *transform_a, double *transform_b){ _tsg_grid_list[*id]->getDomainTransform(transform_a, transform_b); }

// loadNeededPoints
void tsglnp_(int *id, const double *vals){ _tsg_grid_list[*id]->loadNeededPoints(vals); }

// evaluate/Fast/Batch/integrate
void tsgeva_(int *id, const double *x, double *y){ _tsg_grid_list[*id]->evaluate(x, y); }
void tsgevf_(int *id, const double *x, double *y){ _tsg_grid_list[*id]->evaluateFast(x, y); }
void tsgevb_(int *id, const double *x, int *num_x, double *y){ _tsg_grid_list[*id]->evaluateBatch(x, (*num_x), y); }
void tsgint_(int *id, double *q){ _tsg_grid_list[*id]->integrate(q); }

// hierarchical functions/coefficients
void tsgehf_(int *id, const double *x, int *num_x, double *y){
    _tsg_grid_list[*id]->evaluateHierarchicalFunctions(x, (*num_x), y);}
void tsgehs_(int *id, const double *x, int *num_x, int *pntr, int *indx, double *vals){
    _tsg_grid_list[*id]->evaluateSparseHierarchicalFunctionsStatic(x, *num_x, pntr, indx, vals);}
void tsgehz_(int *id, const double *x, int *num_x, int *num_nz){
    *num_nz = _tsg_grid_list[*id]->evaluateSparseHierarchicalFunctionsGetNZ(x, *num_x);}
void tsgghc_(int *id, double *c){
    const double *cc = _tsg_grid_list[*id]->getHierarchicalCoefficients();
    _tsg_grid_list[*id]->isFourier() ? std::copy(cc, cc + 2 * _tsg_grid_list[*id]->getNumPoints() * _tsg_grid_list[*id]->getNumOutputs(), c)
                                     : std::copy(cc, cc + _tsg_grid_list[*id]->getNumPoints() * _tsg_grid_list[*id]->getNumOutputs(), c);
}
void tsgshc_(int *id, double *c){_tsg_grid_list[*id]->setHierarchicalCoefficients(c);}

// setAnisotropic/Surplus/Refinement
void tsgsar_(int *id, int *type, int *min_growth, int *output, int *opt_flags, const int *llimits){
    const int *ll;
    ll = (opt_flags[0] != 0) ? llimits : 0;
    _tsg_grid_list[*id]->setAnisotropicRefinement(OneDimensionalMeta::getIOTypeInt(*type), *min_growth, *output, ll);
}
void tsgeac_(int *id, int *type, int *output, int *result){
    int *coeff = _tsg_grid_list[*id]->estimateAnisotropicCoefficients(OneDimensionalMeta::getIOTypeInt(*type), *output);
    int num_coeff = _tsg_grid_list[*id]->getNumDimensions();
    if ((*type == 2) || (*type == 4) || (*type == 6)) num_coeff *= 2;
    for(int i=0; i<num_coeff; i++) result[i] = coeff[i];
    delete[] coeff;
}
void tsgssr_(int *id, double *tol, int *output, int *opt_flags, const int *llimits){
    const int *ll;
    ll = (opt_flags[0] != 0) ? llimits : 0;
    _tsg_grid_list[*id]->setSurplusRefinement(*tol, *output, ll); }
void tsgshr_(int *id, double *tol, int *type, int *opt_flags, int *output, const int *llimits){
    int theout;
    const int *ll;

    theout = (opt_flags[0] != 0) ? *output : -1;
    ll     = (opt_flags[1] != 0) ? llimits : 0;

    _tsg_grid_list[*id]->setSurplusRefinement(*tol, OneDimensionalMeta::getIOTypeRefinementInt(*type), theout, ll);
}
void tsgcre_(int *id){ _tsg_grid_list[*id]->clearRefinement(); }
void tsgmre_(int *id){ _tsg_grid_list[*id]->mergeRefinement(); }

// set/is/clear/getConformalTransform
void tsgsca_(int *id, int *trunc){ _tsg_grid_list[*id]->setConformalTransformASIN(trunc); }
void tsgica_(int *id, int *result){ *result = (_tsg_grid_list[*id]->isSetConformalTransformASIN()) ? 1 : 0; }
void tsgcct_(int *id){ _tsg_grid_list[*id]->clearConformalTransform(); }
void tsggca_(int *id, int *trunc){ _tsg_grid_list[*id]->getConformalTransformASIN(trunc); }

// isGlobal/Sequence/LocalPolynomial/Wavelet/Fourier
void tsgisg_(int *id, int *status){*status = (_tsg_grid_list[*id]->isGlobal()) ? 1 : 0;}
void tsgiss_(int *id, int *status){*status = (_tsg_grid_list[*id]->isSequence()) ? 1 : 0;}
void tsgisl_(int *id, int *status){*status = (_tsg_grid_list[*id]->isLocalPolynomial()) ? 1 : 0;}
void tsgisw_(int *id, int *status){*status = (_tsg_grid_list[*id]->isWavelet()) ? 1 : 0;}
void tsgisf_(int *id, int *status){*status = (_tsg_grid_list[*id]->isFourier()) ? 1 : 0;}

// print stats
void tsgpri_(int *id){ _tsg_grid_list[*id]->printStats(); }

// get/enableAcceleration
void tsgacc_(int *id, int *acc){
    _tsg_grid_list[*id]->enableAcceleration(AccelerationMeta::getIOIntAcceleration(*acc));
}
void tsggac_(int *id, int *acc){
    TypeAcceleration accel = _tsg_grid_list[*id]->getAccelerationType();
    *acc = AccelerationMeta::getIOAccelerationInt(accel);
}
void tsgsgi_(int *id, int *gpuID){ _tsg_grid_list[*id]->setGPUID(*gpuID); }
void tsgggi_(int *id, int *gpuID){ *gpuID = _tsg_grid_list[*id]->getGPUID(); }
void tsggng_(int *gpus){ *gpus = TasmanianSparseGrid::getNumGPUs(); }
void tsgggm_(int *gpuID, int *mem){ *mem = TasmanianSparseGrid::getGPUMemory(*gpuID); }


}
#endif
