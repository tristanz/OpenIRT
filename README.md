# OpenIRT - IRT Models in Stata

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

## Installation / Usage

In Stata:

```stata
. net install http://tristanz.github.com/OpenIRT
. help openirt
```

## Alternatives

* [2PL models using Stata's gsem command][1]
* [IRT models using Stata's gllamm command][2]
* [IRT models using R's ltm package][3]

## LICENSE

MIT

[1]: http://www.stata.com/manuals13/semexample29g.pdf#semexample29g
[2]: http://www.gllamm.org/faqs/models/irtfitb.html
[3]: http://cran.r-project.org/web/packages/ltm/index.html