// OpenIRT v. 1.0
//
// MCMC estimation of 2PL and 3PL Item Response Models.
//
// MIT LICENSE:
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

#include "openirt.h"

int main(int argc, char* argv[]) {
	// Timer
  boost::timer timer;
  // Process command line and load config file.
  StartUp(argc, argv);
	InitResults();
	
	Sampler sampler(mcmc_options);
	
	cout << "Adding missing responses..." << endl;
	for(int i = 0; i < responses.num_responses; ++i) {
		for(int j = 0; j < items.num_items; ++j) {
			if(approx_equal(responses.x(i, j),MISSING)) {
				if(items.type(j) == TYPE_2PL) {
					Missing2PLParameter p(false, "missing", i, j);
					sampler.AddStep(new GibbsStep<Missing2PLParameter, int>(p));
				} else if (items.type(j) == TYPE_3PL) {
					Missing3PLParameter p(false, "missing", i, j);
					sampler.AddStep(new GibbsStep<Missing3PLParameter, int>(p));					
				}
			}
		}
	}
	
	cout << "Adding missing item parameters..." << endl;
	for(int j = 0; j < items.num_items; ++j) {
		if(approx_equal(items.a(j), MISSING)) {
			if(items.type(j) == TYPE_2PL) {
				A2PLParameter a(true, "a"+to_string<int>(items.id(j)), j);
				sampler.AddStep(new SliceStep<A2PLParameter>(a, 1, 0, dInf));
				B2PLParameter b(true, "b"+to_string<int>(items.id(j)), j);
				sampler.AddStep(new SliceStep<B2PLParameter>(b, 1, -dInf, dInf));
			} else if (items.type(j) == TYPE_3PL) {
				A3PLParameter a(true, "a"+to_string<int>(items.id(j)), j);
				sampler.AddStep(new SliceStep<A3PLParameter>(a, 1, 0, dInf));
				B3PLParameter b(true, "b"+to_string<int>(items.id(j)), j);
				sampler.AddStep(new SliceStep<B3PLParameter>(b, 1, -dInf, dInf));
				C3PLParameter c(true, "c"+to_string<int>(items.id(j)), j);
				sampler.AddStep(new SliceStep<C3PLParameter>(c, 1, 0, 1));
			}
		}
	}	
	
	cout << "Adding missing ability parameters..." << endl;
	for(int i = 0; i < responses.num_responses; ++i) {
		if(approx_equal(responses.theta(i), MISSING)) {
			AbilityParameter theta(true, "theta" + to_string<int>(responses.id(i)), i);
			sampler.AddStep(new SliceStep<AbilityParameter>(theta, 1, -dInf, dInf));
		}
	}
	
	// Run sampler
	sampler.Run();
	
	// Calculate MLE abilty estimates
	MLETheta();
	
	// Save results
	SaveResults();
	
	cout << endl << "Total elapsed time: " << timer.elapsed() << endl;
  
  return 0;
}
