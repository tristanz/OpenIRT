{smcl}
{cmd:help openirt} {right: Item Response Theory (IRT) Estimation}
{hline}

{title:Title}

{p2colset 5 20 22 2}{...}
{p2col :{hi:openirt} {hline 2}}Bayesian and Maximum Likelihood Estimation of Item Response Theory Models{p_end}
{p2colreset}{...}


{title:Syntax}
	
{p 8 16 2}
{opt openirt} {cmd:,} [{it:options}]

{synoptset 30 tabbed}{...}
{synopthdr}
{synoptline}
{syntab:Main (required)}
{synopt :{opt id(varname)}}Unique respondent ID{p_end}
{synopt :{opt item_prefix(name)}}Prefix of item variables{p_end}
{synopt :{opt save_item_parameters(name)}}Filename to save item parameter estimates.{p_end}
{synopt :{opt save_trait_parameters(name)}}Filename to save trait/ability parameter estimates.{p_end}

{syntab:Parameter options}
{synopt :{opt model("3PL")}}Default model type ("2PL" or "3PL").{p_end}
{synopt :{opt theta(varname)}}Variable name holding fixed trait/ability parameters.{p_end}
{synopt :{opt fixed_item_file(string)}}Filename holding fixed item parameters.{p_end}

{syntab:MCMC Options}
{synopt :{opt samplesize(integer 2000)}}Sample size for MCMC estimation.{p_end}
{synopt :{opt burnin(integer 1000)}}Burn in period for MCMC estimation.{p_end}

{title:Description}

{pstd}
{cmd:openirt} estimates 2PL and 3PL Item Response Theory (IRT) models using both Bayesian
MCMC and Maximum Likelihood methods.


{title:Options}

{dlgtab:Model}

{phang}
{opt noconstant}; see
{helpb estimation options##noconstant:[R] estimation options}. 

{phang}
{opt hascons} indicates that a user-defined constant or its equivalent
is specified among the independent variables in {varlist}.  Some caution is
recommended when specifying this option, as resulting estimates may not be as
accurate as they otherwise would be.  Use of this option requires "sweeping"
the constant last, so the moment matrix must be accumulated in absolute rather
than deviation form.  This option may be safely specified when the means of
the dependent and independent variables are all reasonable and there is not
much collinearity between the independent variables.  The best
procedure is to view {opt hascons} as a reporting option -- estimate with
and without {opt hascons} and verify that the coefficients and standard errors
of the variables not affected by the identity of the constant are unchanged.

{phang}
{opt tsscons} forces the total sum of squares to be computed as though
the model has a constant, that is, as deviations from the mean of the
dependent variable.  This is a rarely used option that has an effect only when
specified with {opt noconstant}.  It affects only the total sum of squares and
all results derived from the total sum of squares.

{dlgtab:SE/Robust}

INCLUDE help vce_asymptall

{pmore}
{cmd:vce(ols)}, the default, uses the standard variance estimator for ordinary
least-squares regression.

{pmore}
{cmd:regress} also allows the following:

{phang2}
{cmd:vce(hc2)} and {cmd:vce(hc3)} specify an alternative bias correction for the
robust variance calculation.  {cmd:vce(hc2)} and {cmd:vce(hc3)} may not be
specified with the {helpb svy} prefix.  In the unclustered case,
{cmd:vce(robust)} uses (sigma-hat_j)^2={n/(n-k)}(u_j)^2 as an estimate of the
variance of the jth observation, where u_j is the calculated residual and
n/(n-k) is included to improve the overall estimate's small-sample properties.

{pmore2}
{cmd:vce(hc2)} instead uses u_j^2/(1-h_jj) as the observation's variance
estimate, where h_jj is the diagonal element of the hat (projection) matrix.
This estimate is unbiased if the model really is homoskedastic.
{cmd:vce(hc2)} tends to produce slightly more conservative confidence
intervals.

{pmore2}
{cmd:vce(hc3)} uses u_j^2/(1-h_jj)^2 as suggested by Davidson and MacKinnon
(1993), who report that this method tends to produce better results when the
model really is heteroskedastic.  {cmd:vce(hc3)} produces confidence intervals
that tend to be even more conservative.

{dlgtab:Reporting}

{phang}
{opt level(#)}; see {helpb estimation options##level():[R] estimation options}.

{phang}
{opt beta} asks that standardized beta coefficients be reported instead of
confidence intervals.  The {opt beta} coefficients are the regression
coefficients obtained by first standardizing all variables to have a mean of 0
and a standard deviation of 1.  {opt beta} may not be specified with 
{cmd:vce(cluster} {it:clustvar}{cmd:)} or the {helpb svy} prefix.

{phang}
{opt eform(string)} is used only in programs and ado-files that use 
{cmd:regress} to fit models other than linear regression.  {opt eform()}
specifies that the coefficient table be displayed in exponentiated form as
defined in {manhelp maximize R} and that {it:string} be used to label the
exponentiated coefficients in the table.

{phang}
{opt noheader} suppresses the display of the ANOVA table and summary
statistics at the top of the output; only the coefficient table is displayed.
This option is often used in programs and ado-files.

{phang}
{opt plus} specifies that the output table be made extendable.  This option is
often used in programs and ado-files.

{phang}
{opth depname(varname)} is used only in programs and ado-files that use 
{cmd:regress} to fit models other than linear regression.  {opt depname()} may
be specified only at estimation time.  {it:varname} is recorded as the
identity of the dependent variable, even though the estimates are calculated
using {it:depvar}.  This method affects the labeling of the output -- not
the results calculated -- but could affect subsequent calculations made
by {cmd:predict}, where the residual would be calculated as deviations from
{it:varname} rather than {it:depvar}.  {opt depname()} is most typically used
when {it:depvar} is a temporary variable (see  {manhelp macro P}) used as a
proxy for {it:varname}.

{pmore}
{opt depname()} is not allowed with the {helpb svy} prefix.

{pstd}
The following option is available with {cmd:regress} but is not shown in the
dialog box:

{phang}
{opt mse1} is used only in programs and ado-files that use {cmd:regress} to
fit models other than linear regression and is not allowed with the
{helpb svy} prefix.  {opt mse1} sets the mean squared
error to 1, thus forcing the variance-covariance matrix of the estimators to be
(X'DX)^-1 and affecting calculated standard errors.  Degrees of freedom for t
statistics are calculated as n rather than n-k.


{title:Examples:  linear regression}

{pstd}Setup{p_end}
{phang2}{cmd:. sysuse auto}{p_end}
{phang2}{cmd:. generate weightsq = weight^2}{p_end}
{phang2}{cmd:. regress mpg weight weightsq foreign}{p_end}

{pstd}Obtain beta coefficients without refitting model{p_end}
{phang2}{cmd:. regress, beta}{p_end}

{pstd}Suppress intercept term{p_end}
{phang2}{cmd:. regress weight length, noconstant}{p_end}

{phang2}{cmd:. generate domestic =! foreign}{p_end}

{pstd}Model already has constant{p_end}
{phang2}{cmd:. regress weight length domestic foreign, hascons}{p_end}


{title:Examples:  regression with robust standard errors}

        {hline}
{phang2}{cmd:. sysuse auto, clear}{p_end}
{phang2}{cmd:. generate gpmw = ((1/mpg)/weight)*100*1000}{p_end}
{phang2}{cmd:. regress gpmw foreign}{p_end}
{phang2}{cmd:. regress gpmw foreign, vce(robust)}{p_end}
{phang2}{cmd:. regress gpmw foreign, vce(hc2)}{p_end}
{phang2}{cmd:. regress gpmw foreign, vce(hc3)}{p_end}
        {hline}
{phang2}{cmd:. webuse regsmpl, clear}{p_end}
{phang2}{cmd:. regress ln_wage age age2 tenure, vce(cluster id)}{p_end}
        {hline}


{title:Example:  weighted regression}

{phang2}{cmd:. sysuse census}{p_end}
{phang2}{cmd:. tabulate region, gen(region)}{p_end}
{phang2}{cmd:. regress death medage region2-region4 [aw=pop]}{p_end}


{title:Examples:  linear regression with survey data}

{pstd}Setup{p_end}
{phang2}{cmd:. webuse highschool, clear}

{pstd}Perform linear regression using survey data{p_end}
{phang2}{cmd:. svy: regress weight height}

{pstd}Setup{p_end}
{phang2}{cmd:. generate male = sex == 1 if !missing(sex)}

{pstd}Perform linear regression using survey data for a subpopulation{p_end}
{phang2}{cmd:. svy, subpop(male): regress weight height}


{title:Reference}

{phang}
Van der Linden, W.J. and Hambleton, R.K. (1997)
{it:Handbook of modern item response theory}.
Springer Verlag.

{phang}
Patz, R.J. and Junker, B.W. (1999)
"A straightforward approach to Markov chain Monte Carlo methods for item response models"
{it:Journal of Educational and Behavioral Statistics}. 24:2

{title:Author}

{pstd}Tristan Zajonc{p_end}
{pstd}John F. Kennedy School of Government{p_end}
{pstd}Harvard University, Cambridge, MA 02138.{p_end}
{pstd}Email: tristanz@gmail.com{p_end}
{pstd}Web: http://ksghome.harvard.edu/~zajonct/{p_end}



