c CLASS = B
c  
c  
c  This file is generated automatically by the setparams utility.
c  It sets the number of processors and the class of the NPB
c  in this directory. Do not modify it by hand.
c  
        integer nx_default, ny_default, nz_default
        parameter (nx_default=256, ny_default=256, nz_default=256)
        integer nit_default, lm, lt_default
        parameter (nit_default=20, lm = 8, lt_default=8)
        integer debug_default
        parameter (debug_default=0)
        integer ndim1, ndim2, ndim3
        parameter (ndim1 = 8, ndim2 = 8, ndim3 = 8)
        integer one, nv, nr, ir
        parameter (one=1)
        logical  convertdouble
        parameter (convertdouble = .false.)
        character compiletime*11
        parameter (compiletime='09 Dec 2017')
        character npbversion*5
        parameter (npbversion='3.3.1')
        character cs1*8
        parameter (cs1='gfortran')
        character cs2*6
        parameter (cs2='$(F77)')
        character cs3*6
        parameter (cs3='(none)')
        character cs4*6
        parameter (cs4='(none)')
        character cs5*5
        parameter (cs5='-O -g')
        character cs6*10
        parameter (cs6='-O -static')
        character cs7*6
        parameter (cs7='randi8')
