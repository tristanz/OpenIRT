// OpenIRT parameter definitions
//
// Copyright (c) 2009 Tristan Zajonc
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#include "mcmc.h"

/// Process command line and config file
void StartUp(int argc, char* argv[]) {

  // Load default mcmc_options from command line.
  DefaultStartUp(argc, argv);

  cout << "Loading config file..." << endl;

  // Load config file via SimpleIni library.
  CSimpleIniA ini(false, false, false);
  SI_Error rc = ini.LoadFile(mcmc_options.config_file.c_str());
  if (rc < 0) {
   cout << "Error loading config file:" << mcmc_options.config_file << endl;
   exit(1);
  }

  // Proccess ini file and put data into appropriate structures
  // Uses SimpleIni: http://code.jellycan.com/simpleini/
  //    Args: "section-name",  "key name", "default"
  
  CSimpleIniA::TNamesDepend keys;
  ini.GetAllKeys("model_options", keys);
  
  cout << "Loading model options..." << endl;
  model_options.sampler = ini.GetValue("model_options", "sampler");
  model_options.tune_theta = atof(ini.GetValue("model_options", "tune_theta"));
  model_options.tune_a = atof(ini.GetValue("model_options", "tune_a"));
  model_options.tune_b = atof(ini.GetValue("model_options", "tune_b"));
  model_options.tune_c = atof(ini.GetValue("model_options", "tune_c"));
  model_options.tune_d = atof(ini.GetValue("model_options", "tune_d"));
  
  cout << "Loading priors..." << endl;
  priors.a_mu = atof(ini.GetValue("priors", "a_mu"));
  priors.a_sigma = atof(ini.GetValue("priors", "a_sigma"));
  priors.b_mu = atof(ini.GetValue("priors", "b_mu"));
  priors.b_sigma = atof(ini.GetValue("priors", "b_sigma"));
  
  priors.c_alpha = atof(ini.GetValue("priors", "c_alpha"));
  priors.c_beta = atof(ini.GetValue("priors", "c_beta"));
  
  priors.aGPC_mu = atof(ini.GetValue("priors", "aGPC_mu"));
  priors.aGPC_sigma = atof(ini.GetValue("priors", "aGPC_sigma"));
  priors.bGPC_mu = atof(ini.GetValue("priors", "bGPC_mu"));
  priors.bGPC_sigma = atof(ini.GetValue("priors", "bGPC_sigma"));
  priors.dGPC_mu = atof(ini.GetValue("priors", "dGPC_mu"));
  priors.dGPC_sigma = atof(ini.GetValue("priors", "dGPC_sigma"));
  
  priors.mu_mu = atof(ini.GetValue("priors", "mu_mu"));
  priors.mu_sigma = atof(ini.GetValue("priors", "mu_sigma"));
  priors.sigma_alpha = atof(ini.GetValue("priors", "sigma_alpha"));
  priors.sigma_beta = atof(ini.GetValue("priors", "sigma_alpha"));
  
  cout << "Loading data..." << endl;
  matrix item_data(mcmc_options.test_file.c_str());
  items.id = item_data(_, 0);
  items.type = item_data(_, 1);
  items.num_categories = item_data(_, 2);

  items.a = item_data(_, 3);
  items.b = item_data(_, 4);
  items.c = item_data(_, 5);
  items.d = item_data(0, 6, items.id.rows() - 1, item_data.cols() - 1);
  items.num_items = items.id.rows();
  cout << "Items added: " << items.num_items << endl;

  matrix response_data(mcmc_options.response_file.c_str());
  responses.id = response_data(_, 0);
  responses.group = response_data(_, 1);
  responses.theta = response_data(_, 2);
  responses.x = response_data(0, 3, response_data.rows() - 1, response_data.cols() - 1);
  responses.num_responses = responses.x.rows();
  responses.num_items = responses.x.cols();
  responses.num_groups = max(responses.group);
  cout << "Responses added: " << responses.num_responses << endl;

}

/// 2PL Item Response Functions
double Irf2PL(double theta, double a, double b) {
  return(1 / (1 + exp(- D * a * (theta - b))));
}

matrix Irf2PL(matrix &theta, double a, double b) {
  return(1 / (1 + exp(- D * a * (theta - b))));
}

double Irf3PL(double theta, double a, double b, double c) {
  return(c + (1 - c) / (1 + exp(- D * a * (theta - b))));
}

matrix Irf3PL(matrix &theta, double a, double b, double c) {
  return(c + (1-c) / (1 + exp(- D * a * (theta - b))));
}

/*
double IrfGPC(int k, double theta, double a, double b, matrix &d, int num_categories) {
  matrix numer(num_categories, 1, false);
  if(num_categories == 2) {
    d = 0.0;
  }
  numer[0] = a * (theta - b); // normalized d0 = 0, not in d.
  for (int i = 1; i < num_categories; ++i) {
      numer[i] = numer[i-1] + D * a * (theta - b + d[i-1]);
  }
  
  numer = exp(numer);
  double p = numer[k-1]/sum(numer);
  
  // Exponents of big numbers turn into NaN.  Fix major cases:
  if(isnan(p)) {
    // cerr << "UNSTABLE: GPC giving NaN due to out of bounds theta or b";
    if(k == 1 && theta > b) {
      return(0);
    } else if (k == 1 && theta < b) {
      return(1);
    } else if (k == num_categories &&  theta > b + d[num_categories - 2]) {
      return(1);
    } else if (k == num_categories && theta < b + d[num_categories - 2]) {
      return(0);
    } else {
      cerr << "FATAL: GPC out of range.";
      exit(1);
    }
  }
  // this can become zero.  need safe log.
  if(p < std::numeric_limits<double>::min()) {
    p = std::numeric_limits<double>::min();
  }
  return(p);
}

template <matrix_order O, matrix_style S>
matrix IrfGPC(const Matrix<int, O, S> &k, matrix &theta, double a, double b, matrix &d, int num_categories) {
  matrix p(theta.rows(),1,false);
  for(int i = 0; i < theta.rows(); ++i) {
    p[i] = IrfGPC(k[i], theta[i], a, b, d, num_categories);
  }
  return(p);
}
*/

// Item density function
double Density2PL(int x, double theta, double a, double b) {
  if(x==1) {
    return(Irf2PL(theta, a, b));
  } else {
    return(1-Irf2PL(theta, a, b));
  }
}

template <matrix_order O, matrix_style S>
matrix Density2PL(const Matrix<int, O, S> &x, matrix &theta, double a, double b) {
  int N = theta.rows();
  matrix p(N, 1, false);
  for(int i = 0; i < theta.rows(); ++i) {
    if(x[i]==1) {
      p[i] = Irf2PL(theta[i], a, b);
    } else {
      p[i] = 1 - Irf2PL(theta[i], a, b);
    }
  }
  return(p);
}

double Density3PL(int x, double theta, double a, double b, double c) {
  if(x==1) {
    return(Irf3PL(theta, a, b, c));
  } else {
    return(1-Irf3PL(theta, a, b, c));
  }
}

template <matrix_order O, matrix_style S>
matrix Density3PL(const Matrix<int, O, S> &x, matrix &theta, const double a, const double b, const double c) {
  int N = theta.rows();
  matrix p(N, 1, false);
  for(int i = 0; i < N; ++i) {
    if(x[i]==1) {
      p[i] = Irf3PL(theta[i], a, b, c);
    } else {
      p[i] = 1 - Irf3PL(theta[i], a, b, c);
    }
  }
  return(p);
}

/*
// the way the IRF is written is is the density
double DensityGPC(int x, double theta, double a, double b, matrix &d, int num_categories) {
  return(IrfGPC(x, theta, a, b, d, num_categories));
}

template <matrix_order O, matrix_style S>
matrix DensityGPC(const Matrix<int, O, S> &x, matrix &theta, double a, double b, matrix &d, int num_categories) {
  return(IrfGPC(x, theta, a, b, d, num_categories));
}
*/

// IRT simulation (augmentation) functions
int Random2PL(const double theta, const double a, const double b) {
  return(myrng.rbern(Irf2PL(theta, a, b)));
}

int Random3PL(const double theta, const double a, const double b, const double c) {
  return(myrng.rbern(Irf3PL(theta, a, b, c))); 
}

/*
int RandomGPC(double theta, double a, double b, matrix &d, int num_categories) {
  double u = myrng.runif();
  double p = 0;
  for(int k = 1; k < num_categories; ++k) {
    p += IrfGPC(k, theta, a, b, d, num_categories);
    if(u < p) {
      return(k);
    } 
  }
  return(num_categories);
}
*/

/// 2PL missing response parameter.
class Missing2PLParameter : public Parameter<int> {
public:
  /// Default constructors that call base constructors. (REQUIRED)
  Missing2PLParameter() : Parameter<int>() {}
  Missing2PLParameter(bool track, string name, int i, int j) : Parameter<int>(track, name), i_(i), j_(j) {}

  int RandomPosterior() {
    return Random2PL(responses.theta(i_), items.a(j_), items.b(j_));
  }
  
  /// Draw starting value randomly p = 0.5
  int StartingValue() {
    return myrng.rbern(0.5);
  }
  
  /// Save back to global location
  void Save(int new_value) {
    responses.x(i_,j_) = new_value;
  }
  
  /// Return value from global location.
  int Value() {
    return responses.x(i_,j_);
  }
private:
  int i_;
  int j_;
};


/// 3PL missing response parameter.
class Missing3PLParameter : public Parameter<int> {
public:
  /// Default constructors that call base constructors. (REQUIRED)
  Missing3PLParameter() : Parameter<int>() {}
  Missing3PLParameter(bool track, string name, int i, int j) : Parameter<int>(track, name), i_(i), j_(j) {}

  int RandomPosterior() {
    return Random3PL(responses.theta(i_), items.a(j_), items.b(j_), items.c(j_));
  }
  
  // Better option?
  int StartingValue() {
    return myrng.rbern(0.5);
  }
  
  /// Save back to global location
  void Save(int new_value) {
    responses.x(i_,j_) = new_value;
  }
  
  /// Return value from global location.
  int Value() {
    return responses.x(i_,j_);
  }
private:
  int i_;
  int j_;
};

/*
/// GPC missing response parameter.
class MissingGPCParameter : public Parameter<int> {
public:
  /// Default constructors that call base constructors. (REQUIRED)
  MissingGPCParameter() : Parameter<int>() {}
  MissingGPCParameter(bool track, string name, int i, int j) : Parameter<int>(track, name), i_(i), j_(j) {}

  int RandomPosterior() {
    return RandomGPC(responses.theta(i_), items.a(j_), items.b(j_), items.d(j_,_), items.num_categories(j_));
  }
  
  /// Draw starting value randomly p = 0.5 from 1/2.
  int StartingValue() {
    return myrng.rbern(0.5) + 1;
  }
  
  /// Save back to global location
  void Save(int new_value) {
    responses.x(i_,j_) = new_value;
  }
  
  /// Return value from global location.
  int Value() {
    return responses.x(i_,j_);
  }
private:
  int i_;
  int j_;
};
*/

/// 2PL a parameter
class A2PLParameter : public Parameter<double> {
public:
  /// Default constructors that call base constructors. (REQUIRED)
  A2PLParameter() : Parameter<double>() {}
  A2PLParameter(bool track, string name, int j) : Parameter<double>(track, name), j_(j) {}

  double LogDensity(double a) {
    return sum(log(Density2PL(responses.x(_, j_), responses.theta, a, items.b(j_))))
      + log(dlnorm(a, priors.a_mu, priors.a_sigma));
  }
  
  /// Draw starting from prior
  double StartingValue() {
    return myrng.rlnorm(priors.a_mu, priors.a_sigma);
  }
  
  /// Save back to global location
  void Save(double new_value) {
    items.a(j_) = new_value;
  }
  
  /// Return value from global location.
  double Value() {
    return items.a(j_);
  }
private:
  int j_;
};

/// 2PL b parameter
class B2PLParameter : public Parameter<double> {
public:
  /// Default constructors that call base constructors. (REQUIRED)
  B2PLParameter() : Parameter<double>() {}
  B2PLParameter(bool track, string name, int j) : Parameter<double>(track, name), j_(j) {}

  double LogDensity(double b) {
    return sum(log(Density2PL(responses.x(_, j_), responses.theta, items.a(j_), b)))
      + log(dnorm(b, priors.b_mu, priors.b_sigma));
  }
  
  /// Draw starting from prior
  double StartingValue() {
    return myrng.rnorm(priors.b_mu, priors.b_sigma);
  }
  
  /// Save back to global location
  void Save(double new_value) {
    items.b(j_) = new_value;
  }
  
  /// Return value from global location.
  double Value() {
    return items.b(j_);
  }
private:
  int j_;
};

class A3PLParameter : public Parameter<double> {
public:
  /// Default constructors that call base constructors. (REQUIRED)
  A3PLParameter() : Parameter<double>() {}
  A3PLParameter(bool track, string name, int j) : Parameter<double>(track, name), j_(j) {}

  double LogDensity(double a) {
    return sum(log(Density3PL(responses.x(_, j_), responses.theta, a, items.b(j_), items.c(j_))))
      + log(dlnorm(a, priors.a_mu, priors.a_sigma));
  }
  
  /// Draw starting from prior
  double StartingValue() {
    return myrng.rlnorm(priors.a_mu, priors.a_sigma);
  }
  
  /// Save back to global location
  void Save(double new_value) {
    items.a(j_) = new_value;
  }
  
  /// Return value from global location.
  double Value() {
    return items.a(j_);
  }
private:
  int j_;
};

/// 2PL b parameter
class B3PLParameter : public Parameter<double> {
public:
  /// Default constructors that call base constructors. (REQUIRED)
  B3PLParameter() : Parameter<double>() {}
  B3PLParameter(bool track, string name, int j) : Parameter<double>(track, name), j_(j) {}

  double LogDensity(double b) {
    return sum(log(Density3PL(responses.x(_, j_), responses.theta, items.a(j_), b, items.c(j_))))
      + log(dnorm(b, priors.b_mu, priors.b_sigma));
  }
  
  /// Draw starting from prior
  double StartingValue() {
    return myrng.rnorm(priors.b_mu, priors.b_sigma);
  }
  
  /// Save back to global location
  void Save(double new_value) {
    items.b(j_) = new_value;
  }
  
  /// Return value from global location.
  double Value() {
    return items.b(j_);
  }
private:
  int j_;
};


/// 2PL b parameter
class C3PLParameter : public Parameter<double> {
public:
  /// Default constructors that call base constructors. (REQUIRED)
  C3PLParameter() : Parameter<double>() {}
  C3PLParameter(bool track, string name, int j) : Parameter<double>(track, name), j_(j) {}

  double LogDensity(double c) {
    return sum(log(Density3PL(responses.x(_, j_), responses.theta, items.a(j_), items.b(j_), c)))
      + log(dnorm(c, priors.c_alpha, priors.c_beta));
  }
  
  /// Draw starting from prior
  double StartingValue() {
    return myrng.rbeta(priors.c_alpha, priors.c_beta);
  }
  
  /// Save back to global location
  void Save(double new_value) {
    items.c(j_) = new_value;
  }
  
  /// Return value from global location.
  double Value() {
    return items.c(j_);
  }
private:
  int j_;
};

/*
/// GPC a parameter
class AGPCParameter : public Parameter<double> {
public:
  /// Default constructors that call base constructors. (REQUIRED)
  AGPCParameter() : Parameter<double>() {}
  AGPCParameter(bool track, string name, int j, int category) : Parameter<double>(track, name), j_(j) {}

  double LogDensity(double a) {
    return sum(log(DensityGPC(responses.x(_, j_), responses.theta, a, items.b(j_), items.d(j_,_), items.num_categories(j_))))
      + log(dlnorm(a, priors.aGPC_mu, priors.aGPC_sigma));
  }
  
  /// Draw starting from prior
  double StartingValue() {
    return myrng.rlnorm(priors.aGPC_mu, priors.aGPC_sigma);
  }
  
  /// Save back to global location
  void Save(double new_value) {
    items.a(j_) = new_value;
  }
  
  /// Return value from global location.
  double Value() {
    return items.a(j_);
  }
private:
  int j_;
};

/// GPC b parameter
class BGPCParameter : public Parameter<double> {
public:
  /// Default constructors that call base constructors. (REQUIRED)
  B2PLParameter() : Parameter<double>() {}
  B2PLParameter(bool track, string name, int j) : Parameter<double>(track, name), j_(j) {}

  double LogDensity(double b) {
    return sum(log(DensityGPC(responses.x(_, j_), responses.theta, items.a(j_), b, items.d(j_,_), items.num_categories(j_))))
      + log(dnorm(b, priors.bGPC_mu, priors.bGPC_sigma));
  }
  
  /// Draw starting from prior
  double StartingValue() {
    return myrng.rnorm(priors.bGPC_mu, priors.bGPC_sigma);
  }
  
  /// Save back to global location
  void Save(double new_value) {
    items.b(j_) = new_value;
  }
  
  /// Return value from global location.
  double Value() {
    return items.b(j_);
  }
private:
  int j_;
};

/// GPC d parameter
class DGPCParameter : public Parameter<double> {
public:
  /// Default constructors that call base constructors. (REQUIRED)
  DGPCParameter() : Parameter<double>() {}
  DGPCParameter(bool track, string name, int j, int d_index) : Parameter<double>(track, name), j_(j) d_index_(d_index) {}

  double LogDensity(double d) {
    matrix d_new = items.d(j_,_);
    d_new(d_index_) = d
    return sum(log(DensityGPC(responses.x(_, j_), responses.theta, items.a(j_), items.b(j_), d_new, items.num_categories(j_))))
      + log(dnorm(d, priors.dGPC_mu, priors.dGPC_sigma));
  }
  
  /// Draw starting from prior
  /// TODO: ordering?
  double StartingValue() {
    return myrng.rnorm(priors.dGPC_mu, priors.dGPC_sigma);
  }
  
  /// Save back to global location
  void Save(double new_value) {
    items.d(j_, d_index_) = new_value;
  }
  
  /// Return value from global location.
  double Value() {
    return items.d(j_, d_index_);
  }
private:
  int j_;
  int d_index_;
};
*/

class AbilityParameter : public Parameter<double> {
public:
  /// Default constructors that call base constructors. (REQUIRED)
  AbilityParameter() : Parameter<double>() {}
  AbilityParameter(bool track, string name, int i) : Parameter<double>(track, name), i_(i) {}

  double LogDensity(double theta) {
		double ldens = 0;
		for(int j = 0; j < items.num_items; ++j) {
			if(items.type(j) == TYPE_2PL) {
				ldens += log(Density2PL(responses.x(i_, j), theta, items.a(j), items.b(j)));
			} else {
				ldens += log(Density3PL(responses.x(i_, j), theta, items.a(j), items.b(j), items.c(j)));
			}
		}
    ldens += log(dnorm(theta, priors.mu_mu, priors.mu_sigma));		
		return ldens;
  }
  
  /// Draw starting from prior
  double StartingValue() {
    return myrng.rnorm(priors.mu_mu, priors.mu_sigma);
  }
  
  /// Save back to global location
  void Save(double new_value) {
    responses.theta(i_) = new_value;
  }
  
  /// Return value from global location.
  double Value() {
    return responses.theta(i_);
  }
private:
  int i_;
};



/* Maximum likelihood estimates for theta functor.
 */
class DensityTheta {
public:
	DensityTheta(int i, int pv) : i_(i), pv_(pv) {}
	// Density
	double operator()(double theta) const {
		double ldens = 0;
		for(int j = 0; j < items.num_items; ++j) {
			if(responses.x(i_, j) != MISSING) {
				if(items.type(j) == TYPE_2PL) {
					ldens += log(Density2PL(responses.x(i_, j), theta, results.a_pv(j,pv_), results.b_pv(j,pv_)));
				} else {
					ldens += log(Density3PL(responses.x(i_, j), theta, results.a_pv(j,pv_), results.b_pv(j,pv_), results.c_pv(j,pv_)));
				}
			}
		}
		// Return negative log density so as to minimize.
		return -ldens;
	}
private:
	int i_;
	int pv_;
};

void MLETheta() {
	// Load raw response data without imputed
	cout << endl << "Calculating MLE ability estimates..." << endl;
  boost::progress_display show_progress(responses.num_responses);
	matrix response_data(mcmc_options.response_file.c_str());
  responses.x = response_data(0, 3, response_data.rows() - 1, response_data.cols() - 1);
  responses.theta = response_data(_, 2);

	matrix theta(5, 1, false);
	matrix theta_se(5, 1, false);
	double I;
	for(int i = 0; i < responses.num_responses; ++i) {
		if(approx_equal(responses.theta(i), MISSING)) {
			// Propagate parameter uncertainity using Rubin (1987) formula.
			double item_count = 0;
			for(int j = 0; j < items.num_items; ++j) {
				if(responses.x(i, j) != MISSING) {
					item_count++;
				}
			}
			for(int pv = 0; pv < 5; ++pv) {
				DensityTheta f(i, pv);
				// Note: Gridded olden section search to deal with multi modes.
				theta(pv) = GoldenSectionSearch<DensityTheta>(f, -5, 5, 0.001);
				// Standard errors based on inverse information matrix, calculated using Hessian.
				I = SecondDerivative<DensityTheta>(f, theta(pv), 0.00001) / item_count;
				theta_se(pv) = sqrt((1/I) * (1/item_count));
			}
			results.theta_mle(i) = mean(theta);
			// See Rubin (1987)
			matrix e = theta - mean(theta);
			results.theta_mle_se(i) = sqrt(mean(theta_se % theta_se) + 1.2 * (1/4) * sum(e % e));
	    ++show_progress;
		} else {
			results.theta_mle(i) = responses.theta(i);
			results.theta_mle_se(i) = 0;
		}
	}
}