* Example with simulated NAEP data.
cap program drop openirt
net install http://people.fas.harvard.edu/~tzajonc/stata/openirt/openirt, force

clear
set mem 50m

* Save 12 NAEP items locally
sysuse naep_items, clear
drop if id < 10
save naep_items, replace
count

* load response data
sysuse naep_children

* Preliminary exploration: percent correct score
egen percent_correct = rowmean(item*)
kdensity percent_correct, xtitle("Percent Correct") title(NAEP Percent Correct Score Distribution)

* Often useful to check that items are positive correlated 
* with percent correct score
corr percent_correct item*

* Estimate both item parameters and ability
openirt, id(id) item_prefix(item) save_item_parameters("items.dta") save_trait_parameters("traits.dta")

*openirt, id(id) item_prefix(item) save_item_parameters(items) save_trait_parameters(traits) ///
*	samplesize(100) burnin(0) thin(1) model("2PL") fixed_item_file(naep_items.dta)

* Merge in ability estimates
merge id using traits, sort

* Compare distributions using different estimates
	
* Multiple plausible values can be combined to form better
* kdensity estimate.  Using one plausible value works better though too.
kdensity(theta_pv1), bw(.4) gen(x1 d1)
kdensity(theta_pv2), bw(.4) gen(x2 d2) at(x1)
kdensity(theta_pv3), bw(.4) gen(x3 d3) at(x1)
kdensity(theta_pv4), bw(.4) gen(x4 d4) at(x1)
kdensity(theta_pv5), bw(.4) gen(x5 d5) at(x1)
egen d = rowmean(d*)
line(d x1)

twoway (kdensity theta, bw(.4)) (kdensity theta_eap, bw(.4)) ///
	(line d x1) (kdensity theta_mle, bw(.4)), ///
	xtitle("Theta") title("True Theta vs EAP, PV, MLE Estimates") ///
	legend(order(1 "True" 2 "EAP" 3 "PV" 4 "MLE"))

drop x* d*

* QQ plot distribution comparisons
qqplot(theta_eap theta), xtitle("Theta (True)") ytitle("Theta (EAP)") title("") saving(tmp1.gph, replace)
qqplot(theta_mle theta), xtitle("Theta (True)") ytitle("Theta (MLE)") title("") saving(tmp2.gph, replace)
qqplot(theta_pv1 theta), xtitle("Theta (True)") ytitle("Theta (PV1)") title("") saving(tmp3.gph, replace)
qqplot(theta_pv2 theta), xtitle("Theta (True)") ytitle("Theta (PV2)") title("") saving(tmp4.gph, replace)
graph combine tmp1.gph tmp2.gph tmp3.gph tmp4.gph

* Graph TRUE vs EAP
twoway (scatter theta_eap theta) (function y=x, range(-3 3)), ///
	xtitle("Theta (True)") ytitle("Theta (EAP)") title("") ///
  text(3 3 "y = x", place(e)) legend(off)

* Graph TRUE vs MLE
twoway (scatter theta_mle theta) (function y=x, range(-3 3)), ///
	xtitle("Theta (True)") ytitle("Theta (MLE)") title("") ///
  text(3 3 "y = x", place(e)) legend(off)

* Item comparisons 
* Load true parameters, for reference.
sysuse naep_items, clear
merge id using items, sort

twoway (scatter a_eap a) (function y = x, range(0 1.5)), xtitle(Discrimination (True)) ytitle(Discrimination (EAP))  ///
	xscale(range(0 1.5)) ///
	yscale(range(1 1.5))
	
twoway (scatter b_eap b) (function y = x, range(-2 2)), xtitle(Difficulty (True)) ytitle(Difficulty (EAP))  ///
	xscale(range(-2 2)) ///
	yscale(range(-2 2))
	
twoway (scatter c_eap c) (function y = x, range(0 .6)), xtitle(Guessing (True)) ytitle(Guessing (EAP))  ///
	xscale(range(0 .6)) ///
	yscale(range(0 .6))
	
