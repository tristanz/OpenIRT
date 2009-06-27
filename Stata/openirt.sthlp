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

{synoptset 35 tabbed}{...}
{synopthdr}
{synoptline}
{syntab:Required options}
{synopt :{opt id(varname)}}unique respondent ID{p_end}
{synopt :{opt item_prefix(name)}}prefix of item variables{p_end}
{synopt :{opt save_item_parameters("filename")}}filename to save item parameter estimates{p_end}
{synopt :{opt save_trait_parameters("filename")}}filename to save trait/ability parameter estimates{p_end}

{syntab:Parameter options}
{synopt :{opt model("3PL")}}default model type ("2PL" or "3PL"){p_end}
{synopt :{opt theta(varname)}}variable name holding fixed trait/ability parameters{p_end}
{synopt :{opt fixed_item_file("filename")}}filename holding fixed item parameters{p_end}

{syntab:MCMC options}
{synopt :{opt samplesize(integer 2000)}}Sample size for MCMC estimation{p_end}
{synopt :{opt burnin(integer 1000)}}Burn in period for MCMC estimation{p_end}

{title:Description}

{pstd}
{cmd:openirt} estimates 2PL and 3PL Item Response Theory (IRT) models using both Bayesian MCMC and Maximum Likelihood methods.

{title:Options}

{dlgtab:Required Options}

{phang} 
{opt id(varname)} specifies the variable holding the unique integer respondent ID.

{phang}
{opt item_prefix(name)} specifies the prefix used for each item or questions. For instance, item_prefix(item) would select any items with names item# (i.e. item1, item2, item3, etc). Item IDs must be unique but do not need to be contiguous. 

{phang}
{opt save_item_parameters("filename")} specifies the filename where item parameters should be saved. If the file already exists, it will be overwritten.

{phang} 
{opt save_trait_parameters("filename")} specifies the filename where trait/ability parameters should be saved. If the file already exists, it will be overwritten.

{dlgtab:Parameter Options}

{phang}
{opt model("3PL")} specifies the default model for items.  The default model is the three parameter logistic model (3PL).  {opt model("2PL")} forces the guessing parameter to zero, given the two parameter logistic model.  Fixed items specified in {opt fixed_item_file(filename)} will override the default model type.  This allows a mixture of 3PL and 2PL models.

{phang}
{opt theta(varname)} specifies variable name holding fixed trait or ability parameters.  Any missing entries will be treated as free parameters.

{phang}
{opt fixed_item_file("filename")} specifies filename holding fixed item types and parameters.  The  file must include at least four variables: {it:id}, {it:type}, {it:a}, {it:b}, {it:c}.  {it:id} gives the unique item identifier that matches the {opt item_prefix(name)} id.  {it:type} should equal 1 for 2PL items and 2 for 3PL items. {it:a} is the item discrimination parameter, {it:b} is the item difficulty parameter, and {it:c} is the  item guessing parameter. Note: {cmd: openirt} assumes all items use the normal ogive metric ({it:D = 1.7}).

{dlgtab:MCMC Options}

{phang}
{opt samplesize(2000)} specifies the number of post burn in MCMC iterations (default = 2000). Plausible values are drawn at evenly spaced intervals from this sample, and EAP estimates are based on the mean of the entire sample. Larger sample sizes will reduce the monte carlo standard error.  In most applications the standard error of measurement dominates the monte carlo standard error after several thousand iterations.

{phang}
{opt burnin(1000)} specifies the number of burn in MCMC iterations (default = 1000).  MCMC estimates rely on the chain converging to a stationary state.  In most IRT applications this occurs quite quickly -- within several hundred iterations.  If estimates do not appear to be converging, increase the burn in period.

{title:Discussion}

{pstd}OpenIRT estimates 2PL and 3PL Item Response Theory (IRT) models for dichotomous (correct / incorrect) data using both Bayesian and Maximum Likelihood methods.{p_end}

{pstd}The software allows for missing (free) and fixed item parameters, abilities, and responses. This allows, for instance, equating of multiple overlapping test forms; equating test forms using a known reference population; and placing students on known ability metric using fixed item parameters from an item data bank such as TIMSS or NAEP (see, e.g, Das and Zajonc(2009)).{p_end}

{pstd}Unlike some other IRT software, OpenIRT includes both Bayesian MCMC and Maximum Likelihood estimates.  The software estimates expected posterior (EAP), plausible value (PV), and maximum likelihood (MLE) estimates of the underlying latent trait -- often called theta or ability.{p_end}

{pstd}Plausible values, or multiple imputations, are draws from the posterior of each respondent's ability parameter.  While potentially poor measures of each respondent's ability, plausible values allow accurate estimation of distributional quantities, such as the upper and lower quartiles, or fraction of students passing a particular threshold.  If the number of items is small, EAP and MLE estimates will generally yield very poor estimates of such quantities, with EAP understanding the standard deviation and MLE estimates overestimating the standard deviation.  See Das and Zajonc (2009) and Mislevy et al (1992).{p_end}

{pstd}{it:Note on speed}: Estimation can be slow due to the large number of free  parameters estimated using MCMC simulation.  Users with large data sets may wish to use small subsamples of data before running an analysis on the full sample.{p_end}

{title:Examples:  Scoring single form.}

{phang2}{cmd:. sysuse naep_children, clear}{p_end}
{phang2}{cmd:. openirt, id(id) save_item_parameters("items.dta") save_trait_parameters("traits.dta")}{p_end}

{title:Examples:  Scoring multiple overlapping forms.}

{phang}Create two overlapping exams:{p_end}
{phang2}{cmd:. sysuse naep_children, clear}{p_end}
{phang2}{cmd:. recode item1 item2 (0/1 = .) if _n =< 250}{p_end}
{phang2}{cmd:. recode item3 item4 (0/1 = .) if _n > 250}{p_end}

{phang}Score overlapping exams:{p_end}
{phang2}{cmd:. openirt, id(id) save_item_parameters("items.dta") save_trait_parameters("traits.dta")}{p_end}

{title:Examples:  Linking to fixed item parameters.}

{phang}Create 10 fixed items:{p_end}
{phang2}{cmd:. sysuse naep_items, clear}{p_end}
{phang2}{cmd:. keep if id < 10}{p_end}
{phang2}{cmd:. save fixed_items, replace}{p_end}

{phang}Score exam using mixed fixed and free items:{p_end}
{phang2}{cmd:. sysuse naep_children, clear}{p_end}
{phang2}{cmd:. openirt, id(id) save_item_parameters("items.dta") save_trait_parameters("traits.dta") fixed_item_file("fixed_items.dta")}{p_end}


{title:References}

{phang}
Das, J. and Zajonc, T. (2009)
"India shining and Bharat drowning: Comparing two Indian states to the worldwide distribution in mathematics achievement {it:Journal of Development Economics}

{phang}
Mislevy, R.J. and Beaton, A.E. and Kaplan, B. and Sheehan, K.M. (1992) "Estimating population characteristics from sparse matrix samples of item responses" {it:Journal of Educational Measurement}. 29:2

{phang}
Patz, R.J. and Junker, B.W. (1999) "A straightforward approach to Markov chain Monte Carlo methods for item response models" {it:Journal of Educational and Behavioral Statistics}. 24:2

{phang} 
Van der Linden, W.J. and Hambleton, R.K. (1997) {it:Handbook of modern item response theory}. Springer Verlag.

{title:Author}

{pstd}Tristan Zajonc{p_end}
{pstd}John F. Kennedy School of Government{p_end}
{pstd}Harvard University, Cambridge, MA 02138.{p_end}
{pstd}Email: tristanz@gmail.com{p_end}
{pstd}Web: http://ksghome.harvard.edu/~zajonct/{p_end}



