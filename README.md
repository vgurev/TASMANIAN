# TASMANIAN

The Toolkit for Adaptive Stochastic Modeling and Non-Intrusive ApproximatioN is a collection of robust libraries for high dimensional integration and interpolation as well as parameter calibration. The code consists of several modules that can be used individually or conjointly.

The current stable version (5.0) and more detals regarding TASMANIAN can be found at: http://tasmanian.ornl.gov/

Sparse Grids
==============

Sparse Grids is a family of algorithms for constructing multidimensional quadrature and interpolation rules from tensor products of one dimensional such rules. Tasmanian Sparse Grids Module implements a wide variety of one dimensional rules based on global and local function basis.

DREAM
==============
DiffeRential Evolution Adaptive Metropolis (DREAM) is an algorithm for sampling from a general probability density when only the probability density function is known. The method can be applied to problems of Bayesian inference and optimization, including custom defined models as well as surrogate models constructed with the Tasmanian Sparse Grids module. 
