## OpenIRT v. 1.0 - Bayesian and Maximum Likelihood Estimation of IRT Models.

OpenIRT estimates 2PL and 3PL Item Response Theory (IRT) models for
dichotomous data. It includes Bayesian MCMC estimation of item parameters
and abilities, and maximum likelihood ability estimates.  The Bayesian
methods include both expected posterior (EAP) and plausible values (PV)
ability and item parameter estimates.

OpenIRT is designed to be flexible. It allows any combination of items,
abilities, and responses to be missing.  For instance, fixed anchor
items can be used to place children and items on a known scale; missing
responses can be used to link multiple overlapping test forms; and known
abilities can be used to calibrate new items and ability parameters.

### Installation

In Stata:

```stata
. net install http://people.fas.harvard.edu/~tzajonc/stata/openirt
. help openirt
```

### LICENSE

MIT