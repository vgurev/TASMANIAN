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

#ifndef __TASMANIAN_SPARSE_GRID_WAVELET_HPP
#define __TASMANIAN_SPARSE_GRID_WAVELET_HPP

#include "tsgIOHelpers.hpp"
#include "tsgEnumerates.hpp"
#include "tsgIndexSets.hpp"
#include "tsgIndexManipulator.hpp"
#include "tsgRuleWavelet.hpp"
#include "tsgLinearSolvers.hpp"
#include "tsgGridCore.hpp"

namespace TasGrid{

class GridWavelet : public BaseCanonicalGrid{
public:
    GridWavelet();
    ~GridWavelet();

    bool isWavelet() const{ return true; }

    template<bool useAscii> void write(std::ostream &os) const;
    template<bool useAscii> void read(std::istream &is);

    void makeGrid(int cnum_dimensions, int cnum_outputs, int depth, int corder, const std::vector<int> &level_limits);
    void copyGrid(const GridWavelet *wav);
    void setNodes(MultiIndexSet &nodes, int cnum_outputs, int corder); // for FDS purposes

    int getNumDimensions() const;
    int getNumOutputs() const;
    TypeOneDRule getRule() const;
    int getOrder() const;

    int getNumLoaded() const;
    int getNumNeeded() const;
    int getNumPoints() const;

    void getLoadedPoints(double *x) const;
    void getNeededPoints(double *x) const;
    void getPoints(double *x) const; // returns the loaded points unless no points are loaded, then returns the needed points

    void getQuadratureWeights(double weights[]) const;
    void getInterpolationWeights(const double x[], double weights[]) const;

    void loadNeededPoints(const double *vals);

    void evaluate(const double x[], double y[]) const;
    void integrate(double q[], double *conformal_correction) const;

    void evaluateBatch(const double x[], int num_x, double y[]) const;

    #ifdef Tasmanian_ENABLE_BLAS
    void evaluateBlas(const double x[], int num_x, double y[]) const;
    #endif

    #ifdef Tasmanian_ENABLE_CUDA
    void loadNeededPointsCuda(CudaEngine *engine, const double *vals);
    void evaluateCudaMixed(CudaEngine*, const double*, int, double[]) const;
    void evaluateCuda(CudaEngine*, const double*, int, double[]) const;
    #endif

    void setSurplusRefinement(double tolerance, TypeRefinement criteria, int output, const std::vector<int> &level_limits);
    void clearRefinement();
    void mergeRefinement();

    void evaluateHierarchicalFunctions(const double x[], int num_x, double y[]) const;

    void setHierarchicalCoefficients(const double c[], TypeAcceleration acc);

    const double* getSurpluses() const;
    const int* getPointIndexes() const;

    void clearAccelerationData();

protected:
    void reset();

    double evalBasis(const int p[], const double x[]) const;
    void buildInterpolationMatrix();
    void recomputeCoefficients();
    void solveTransposed(double w[]) const;
    double evalIntegral(const int p[]) const;

    std::vector<double> getNormalization() const;

    Data2D<int> buildUpdateMap(double tolerance, TypeRefinement criteria, int output) const;

    bool addParent(const int point[], int direction, Data2D<int> &destination) const;
    void addChild(const int point[], int direction, Data2D<int> &destination) const;
    void addChildLimited(const int point[], int direction, const std::vector<int> &level_limits, Data2D<int> &destination) const;

private:
    RuleWavelet rule1D;

    int num_dimensions, num_outputs, order;

    Data2D<double> coefficients; // a.k.a., surpluses

    MultiIndexSet points;
    MultiIndexSet needed;

    StorageSet values;

    TasSparse::SparseMatrix inter_matrix;
};

}

#endif
