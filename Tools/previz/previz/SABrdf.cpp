#include "SABrdf.h"

#include <iostream>
#include <algorithm>
#include <functional>
#include <random>
#include <chrono>

#include <QDebug>

#include <ceres/ceres.h>

#define PDF_UNIFORM 0


static std::random_device rd_uniform;

static std::default_random_engine normal_law_generator;

struct BRDFResidual {
	/*	
	_sample_normal[i] =	glm::normalize ( _sample_normal[i] );
	_sample_view_dir[i] = glm::normalize ( _sample_view_dir[i]);
	_light_dir = glm::normalize (_light_dir);
	*/

	BRDFResidual(glm::vec3 radiance_sample, glm::vec3 sample_normal, glm::vec3 irradiance_sample, glm::vec3 sample_view_dir, glm::vec3 light_dir, glm::vec3 sun_color): 
		_radiance_sample(radiance_sample),_sample_normal(sample_normal), _sample_view_dir(sample_view_dir), _light_dir(light_dir), _sun_color(sun_color), _irradiance_sample(irradiance_sample)
	{	}
	
	bool operator()(const double * const pd,const double* const ps, const double* const n, double* _cost_per_channel) const {

		double nDl = std::max( 0.0f, glm::dot(_sample_normal,_light_dir ));
		glm::vec3 reflect = 2.0f * ( glm::dot(_sample_normal,_light_dir )) *_sample_normal - _light_dir;

		double rDv = glm::dot(reflect,_sample_view_dir);
		double roughness = std::max(0.0, std::pow( rDv, (double) n[0] ));
		//glm::vec3 specular = ps*roughness;
				
		_cost_per_channel[0] = std::abs( (double) (_radiance_sample.r) - pd[0]* (double)(nDl*_sun_color.r +_irradiance_sample.r*glm::one_over_pi<float>()) +ps[0]*roughness );
		_cost_per_channel[1] = std::abs( (double) (_radiance_sample.g) - pd[1]* (double)(nDl*_sun_color.g +_irradiance_sample.g*glm::one_over_pi<float>()) +ps[1]*roughness );
		_cost_per_channel[2] = std::abs( (double) (_radiance_sample.b) - pd[2]* (double)(nDl*_sun_color.b +_irradiance_sample.b*glm::one_over_pi<float>()) +ps[2]*roughness );

		
		if ( pd[0] +ps[0] > 1.0 ) 
			_cost_per_channel[0] = 100000.0;

		if ( pd[1] +ps[1] > 1.0 ) 
			_cost_per_channel[1] = 100000.0;

		if ( pd[2] +ps[2] > 1.0 ) 
			_cost_per_channel[2] = 100000.0;
			
		/*
		_cost_per_channel[0] =	std::max( (double) (_radiance_sample.r) - pd[0]* (double)(nDl*_sun_color.r +_irradiance_sample.r*glm::one_over_pi<float>()), -((double) (_radiance_sample.r) - pd[0]* (double)(nDl*_sun_color.r +_irradiance_sample.r*glm::one_over_pi<float>())));
		_cost_per_channel[1] = 	std::max( (double) (_radiance_sample.g) - pd[1]* (double)(nDl*_sun_color.g +_irradiance_sample.g*glm::one_over_pi<float>()), -((double) (_radiance_sample.g) - pd[1]* (double)(nDl*_sun_color.g +_irradiance_sample.g*glm::one_over_pi<float>())));
		_cost_per_channel[2] = 	std::max( (double) (_radiance_sample.b) - pd[2]* (double)(nDl*_sun_color.b +_irradiance_sample.b*glm::one_over_pi<float>()), -((double) (_radiance_sample.b) - pd[2]* (double)(nDl*_sun_color.b +_irradiance_sample.b*glm::one_over_pi<float>())));
	*/



	
			/*
			(double) (_radiance_sample.g) -pd[1]* (double)((std::max( 0.0f, glm::dot((_sample_normal),_light_dir ))*_sun_color.g)) +
			(double) (_radiance_sample.b) -pd[2]* (double)((std::max( 0.0f, glm::dot((_sample_normal),_light_dir ))*_sun_color.b)));
			*/

		//residual[0] = T(y_) - exp(m[0] * T(x_) + c[0]);
		
		//glm::vec3 diffuse  = _pd*std::max( 0.0f, glm::dot((_sample_normal[i]),_light_dir ))*_sun_color; //*glm::one_over_pi<float>()

		//	glm::vec3 reflect = 2.0f * ( glm::dot(_sample_normal[i],_light_dir )) *_sample_normal[i] - _light_dir;
		//glm::vec3 indirect = (_irradiance_sample[i]*_pd)*glm::one_over_pi<float>();
		//glm::vec3 specular = _ps*std::max(0.0f, (float) std::pow( glm::dot(reflect,_sample_view_dir[i]),n) );

		///_cost_per_channel[i] = _radiance_sample[i] - ( diffuse + indirect + specular );

		return true;
	}

private:

	const glm::vec3 _radiance_sample;
	const glm::vec3 _sample_normal;
	const glm::vec3 _sample_view_dir;
	const glm::vec3 _light_dir;
	const glm::vec3 _sun_color;
	const glm::vec3 _irradiance_sample;

};


struct ExponentialResidual {
	ExponentialResidual(double x, double y)
		: x_(x), y_(y) {}

	template <typename T> bool operator()(
		const T* const m, const T* const c, T* residual) const {
			residual[0] = T(y_) - exp(m[0] * T(x_) + c[0]);
			return true;
	}

private:
	const double x_;
	const double y_;
};



SABrdf::SABrdf(){

	_range_ps =glm::vec2(0.0,1.0);
	_range_pd =glm::vec2(0.0,1.0);
	_range_n = glm::ivec2(0,100);
}


SABrdf::~SABrdf(){

}

SABrdf::SABrdf(const SABrdf &source)
{

	_sample_normal = source._sample_normal;
	_sample_view_dir =source._sample_view_dir;
	_radiance_sample=source._radiance_sample;
	_irradiance_sample = source._irradiance_sample;
	return ;
}

SABrdf &SABrdf::operator=(const SABrdf &source)
{
	_sample_normal = source._sample_normal;
	_sample_view_dir =source._sample_view_dir;
	_radiance_sample=source._radiance_sample;
	_irradiance_sample = source._irradiance_sample;

	return *this;
}

SABrdf &SABrdf::operator+(const SABrdf &src){

	for(unsigned int k=0; k <  src._sample_normal.size(); k++){

		_sample_normal.push_back(src._sample_normal[k]);
		_sample_view_dir.push_back(src._sample_view_dir[k]);
		_radiance_sample.push_back(src._radiance_sample[k]);
		_irradiance_sample.push_back(src._irradiance_sample[k]);

	}
	return *this;
}


void SABrdf::setSunDirection(glm::vec3 sundir){
	_sundir=sundir;
}

void SABrdf::setSunColor(glm::vec3 suncolor){
	_sun_color = suncolor;
}

void SABrdf::addSamples(glm::vec3 normal, glm::vec3 sample_view_dir , glm::vec3 radiance_sample, glm::vec3 irradiance_sample ){

	//incoming_radiance, point_normal, 
	_sample_normal.push_back(normal);
	_sample_view_dir.push_back(sample_view_dir);
	_radiance_sample.push_back(radiance_sample);
	_irradiance_sample.push_back(irradiance_sample);

}

bool SABrdf::checkPrior(float pd, float ps, int n){

	return ( pd+ps <=1.0f ) && ( pd >= 0.0f) && ( ps >= 0.0f) && ( pd <= 1.0f)&& ( ps <= 1.0f);// && (n >= 0.0f)&& (n < 101.0f) ; //energy conservative test
}

glm::vec3  SABrdf::getMedianeRadiance(void){

	std::vector<float> red;
	std::vector<float> green;
	std::vector<float> blue;

	for(unsigned int kp=0 ; kp< _radiance_sample.size();kp++){
		red.push_back(_radiance_sample[kp].r);
		green.push_back(_radiance_sample[kp].g);
		blue.push_back(_radiance_sample[kp].b);
	}

	std::sort(red.begin(), red.end(), std::greater<float>());
	std::sort(green.begin(), green.end(), std::greater<float>());
	std::sort(blue.begin(), blue.end(), std::greater<float>());


	return glm::vec3( red[red.size()/2 ], green[green.size()/2 ], blue[blue.size()/2 ]);
}

void SABrdf::CERES_example(void){
	const int kNumObservations = 67;
	const double data[] = {
		0.000000e+00, 1.133898e+00,
		7.500000e-02, 1.334902e+00,
		1.500000e-01, 1.213546e+00,
		2.250000e-01, 1.252016e+00,
		3.000000e-01, 1.392265e+00,
		3.750000e-01, 1.314458e+00,
		4.500000e-01, 1.472541e+00,
		5.250000e-01, 1.536218e+00,
		6.000000e-01, 1.355679e+00,
		6.750000e-01, 1.463566e+00,
		7.500000e-01, 1.490201e+00,
		8.250000e-01, 1.658699e+00,
		9.000000e-01, 1.067574e+00,
		9.750000e-01, 1.464629e+00,
		1.050000e+00, 1.402653e+00,
		1.125000e+00, 1.713141e+00,
		1.200000e+00, 1.527021e+00,
		1.275000e+00, 1.702632e+00,
		1.350000e+00, 1.423899e+00,
		1.425000e+00, 1.543078e+00,
		1.500000e+00, 1.664015e+00,
		1.575000e+00, 1.732484e+00,
		1.650000e+00, 1.543296e+00,
		1.725000e+00, 1.959523e+00,
		1.800000e+00, 1.685132e+00,
		1.875000e+00, 1.951791e+00,
		1.950000e+00, 2.095346e+00,
		2.025000e+00, 2.361460e+00,
		2.100000e+00, 2.169119e+00,
		2.175000e+00, 2.061745e+00,
		2.250000e+00, 2.178641e+00,
		2.325000e+00, 2.104346e+00,
		2.400000e+00, 2.584470e+00,
		2.475000e+00, 1.914158e+00,
		2.550000e+00, 2.368375e+00,
		2.625000e+00, 2.686125e+00,
		2.700000e+00, 2.712395e+00,
		2.775000e+00, 2.499511e+00,
		2.850000e+00, 2.558897e+00,
		2.925000e+00, 2.309154e+00,
		3.000000e+00, 2.869503e+00,
		3.075000e+00, 3.116645e+00,
		3.150000e+00, 3.094907e+00,
		3.225000e+00, 2.471759e+00,
		3.300000e+00, 3.017131e+00,
		3.375000e+00, 3.232381e+00,
		3.450000e+00, 2.944596e+00,
		3.525000e+00, 3.385343e+00,
		3.600000e+00, 3.199826e+00,
		3.675000e+00, 3.423039e+00,
		3.750000e+00, 3.621552e+00,
		3.825000e+00, 3.559255e+00,
		3.900000e+00, 3.530713e+00,
		3.975000e+00, 3.561766e+00,
		4.050000e+00, 3.544574e+00,
		4.125000e+00, 3.867945e+00,
		4.200000e+00, 4.049776e+00,
		4.275000e+00, 3.885601e+00,
		4.350000e+00, 4.110505e+00,
		4.425000e+00, 4.345320e+00,
		4.500000e+00, 4.161241e+00,
		4.575000e+00, 4.363407e+00,
		4.650000e+00, 4.161576e+00,
		4.725000e+00, 4.619728e+00,
		4.800000e+00, 4.737410e+00,
		4.875000e+00, 4.727863e+00,
		4.950000e+00, 4.669206e+00,
	};

	double m = 0.0;
	double c = 0.0;

	ceres::Problem _problem;
	for (int i = 0; i < kNumObservations; ++i) {
		_problem.AddResidualBlock(
			new ceres::AutoDiffCostFunction<ExponentialResidual, 1, 1, 1>(	new ExponentialResidual(data[2 * i], data[2 * i + 1])),
			NULL,
			&m, &c);
	}
	_problem.SetParameterUpperBound(&m, 0, 0.28);


	ceres::Solver::Options options;
	options.max_num_iterations = 25;

	options.linear_solver_type = ceres::SPARSE_NORMAL_CHOLESKY;
	options.minimizer_progress_to_stdout = true;


	ceres::Solver::Summary summary;
	ceres::Solve(options, &_problem, &summary);
	qDebug()<< "\n";
	qDebug()<<summary.BriefReport().c_str();
	qDebug() << "Initial m: " << 0.0 << " c: " << 0.0 << "\n";
	qDebug() << "Final   m: " << m << " c: " << c << "\n";

}

void SABrdf::CERES_Solve(void){
			 
	const int kNumObservations = 1;
	const double data[] = {
		0.000000e+00, 1.133898e+00,
	};

	double m = 0.0;
	double c = 0.0;

	ceres::Problem _problem;

	//my unknown to optimize 
	_pd = glm::vec3(0.0f,0.0f,0.0f);
	_ps = glm::vec3(0.0f,0.0f,0.0f);
	_n =0;

	glm::vec3 mediane_samples = getMedianeRadiance()/_sun_color;

	double pd[3];
	pd[0] =mediane_samples.r;pd[1] =mediane_samples.g;pd[2] =mediane_samples.b;
	double ps[3]; ps[0] =0.5; ps[1]=0.5; ps[2] = 0.5;

	double pn[1]; pn[0]=50;


	int Nsample =_sample_normal.size();
	for (int icost=0; icost < Nsample ; icost++){
		_problem.AddResidualBlock( new ceres::NumericDiffCostFunction<BRDFResidual,ceres::CENTRAL, 3, 3, 3, 1> ( 
			new BRDFResidual( _radiance_sample[icost], _sample_normal[icost], _irradiance_sample[icost], _sample_view_dir[icost], _sundir, _sun_color)),
			NULL, pd, ps, pn);
	}

	//here set constraints
	_problem.SetParameterLowerBound(pd, 0, 0.0);   _problem.SetParameterUpperBound(pd, 0, 1.0);
	_problem.SetParameterLowerBound(pd, 1, 0.0);   _problem.SetParameterUpperBound(pd, 1, 1.0);
	_problem.SetParameterLowerBound(pd, 2, 0.0);   _problem.SetParameterUpperBound(pd, 2, 1.0);

	_problem.SetParameterLowerBound(ps, 0, 0.0);   _problem.SetParameterUpperBound(ps, 0, 1.0);
	_problem.SetParameterLowerBound(ps, 1, 0.0);   _problem.SetParameterUpperBound(ps, 1, 1.0);
	_problem.SetParameterLowerBound(ps, 2, 0.0);   _problem.SetParameterUpperBound(ps, 2, 1.0);

	_problem.SetParameterLowerBound(pn, 0, 0.0);   _problem.SetParameterUpperBound(pn, 0, 100.0);

	ceres::Solver::Options options;
	options.max_num_iterations = 25000;
	options.minimizer_type = ceres::TRUST_REGION;
	options.linear_solver_type = ceres::SPARSE_NORMAL_CHOLESKY;
	options.minimizer_progress_to_stdout = false;


	ceres::Solver::Summary summary;
	ceres::Solve(options, &_problem, &summary);


	_pd =glm::vec3(pd[0],pd[1],pd[2]);

	_ps =glm::vec3(ps[0],ps[1],ps[2]);
	_n = pn[0];

	/*
	qDebug()<< "\n";
	qDebug()<<summary.BriefReport().c_str();

	qDebug() << "Initial m: " << 0.0 << " c: " << 0.0 << "\n";
	qDebug() << "Final   m: " << m << " c: " << c << "\n";

	qDebug() << "Final   pd: " << pd[0]*255<<" "<< pd[1]*255<<" "<<pd[2]*255<< "\n";*/

}

void SABrdf::solve(){

	std::srand (time(NULL));

	//generate pd, ps and n to init the process


	float epsilon_happy = 0.001f;
	//float energy_cost = std::numeric_limits<float>::infinity();;

	glm::vec3 current_pd;
	glm::vec3 current_ps;

	int current_n=1;


	_pd = glm::vec3(0.001f);
	_ps = glm::vec3(0.001f);
	_n = 1;

	//1st evaluate the mediane per channel
	glm::vec3 mediane_samples = getMedianeRadiance()/_sun_color;///( _sun_color*glm::one_over_pi<float>()*0.8f );
	_pd = mediane_samples;

	/*
	glm::vec3 diffuse  = _pd*glm::one_over_pi<float>()*std::max( 0.0f, glm::dot((_sample_normal[i]),_sundir ))*_sun_color; 

	glm::vec3 indirect = (_irradiance_sample[i]*_pd) ;
	*/
	//_pd = mediane_samples ;

	// we assume more than 1 sample...



	/*
	std::cout<<"NB eq"<<_radiance_sample.size()<<std::endl;
	std::cout<<"Mediane "<<mediane_samples.r<<" "<<mediane_samples.g<<" "<<mediane_samples.b<<std::endl;
	std::cout<<"Mediane "<<mediane_samples.r*255<<" "<<mediane_samples.g*255<<" "<<mediane_samples.b*255<<std::endl;
	*/
	//use the mediane to define normal law

	float std_dev = 0.15;


	std::normal_distribution<float> distribution_r(mediane_samples.r, mediane_samples.r*std_dev);
	std::normal_distribution<float> distribution_g(mediane_samples.g, mediane_samples.g*std_dev);
	std::normal_distribution<float> distribution_b(mediane_samples.b, mediane_samples.b*std_dev);

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator_r (seed);

	seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator_g (seed);

	seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator_b (seed);

	glm::vec3 ps =  glm::vec3(0.01f,0.01f,0.01f);
	unsigned int pn = 1;

	glm::vec3 pd;
	int NB_ITER_MAX =5000;
	for (unsigned int channel =0; channel <1; channel ++){

		int iter =0;

		pd = _pd;

		float current_cost =0.0f;
		float energy_cost = std::numeric_limits<float>::infinity();

		while ( iter < NB_ITER_MAX  && energy_cost > epsilon_happy  ) {
			/*
			//generate cases
			if ( channel ==0){
			pd = _pd;
			pd.x = ((float)iter/(float)NB_ITER_MAX);
			}
			if ( channel ==1){
			pd = _pd;
			pd.y = ((float)iter/(float)NB_ITER_MAX);
			}
			if ( channel ==2){
			pd = _pd;
			pd.z = ((float)iter/(float)NB_ITER_MAX);
			}
			*/
			glm::vec3 pd =  glm::vec3( distribution_r(generator_r), distribution_g(generator_g),  distribution_b(generator_b) );

			float st =generateVarWithPdf(_range_ps, PDF_UNIFORM );
			ps = glm::vec3(st,st,st);
			pn = generateVarWithPdf(_range_n, PDF_UNIFORM);

			//

			//float t =generateVarWithPdf(_range_ps, PDF_UNIFORM );


			/*
			glm::vec3 ps =  glm::vec3( 
			generateVarWithPdf(_range_ps, PDF_UNIFORM ),
			generateVarWithPdf(_range_ps, PDF_UNIFORM ),
			generateVarWithPdf(_range_ps, PDF_UNIFORM ) );
			*/

			if ( !checkPrior(pd.r, ps.r, pn) || !checkPrior(pd.g, ps.g, pn) || !checkPrior(pd.b, ps.b, pn)){
				while (!checkPrior(pd.r, ps.r, pn) || !checkPrior(pd.g, ps.g, pn) || !checkPrior(pd.b, ps.b, pn)){

					pd =  glm::vec3( distribution_r(generator_r), distribution_g(generator_g),  distribution_b(generator_b) );
					st =generateVarWithPdf(_range_ps, PDF_UNIFORM );
					ps = glm::vec3(st,st,st);
					pn = generateVarWithPdf(_range_n, PDF_UNIFORM);
				}
			}

			//estimate costs constraint with all sample
			current_cost = evaluateCost( _sundir, _sun_color, pd, ps, pn);

			if (current_cost < energy_cost ){

				_pd = pd; 
				_ps = ps;
				_n = pn;
				energy_cost =current_cost ;
			}

			iter=iter+1;

		}
		///std::cout<<"Iter "<<iter<<" energy_cost"<<energy_cost<<std::endl;


	}
	/*

	std::cout<<"DEBUG INFO: _pd"<<_pd[0]<<" "<<_pd[1]<<" "<<_pd[2]<<" _ps"<<_ps[0]<<" "<<_ps[1]<<" "<<_ps[2]<<" "<<"_n "<<_n<<std::endl;
	std::cout<<"DEBUG INFO: _pd"<<_pd.x<<" "<<_pd.y<<" "<<_pd.z<<" _ps"<<_ps[0]<<" "<<_ps[1]<<" "<<_ps[2]<<" "<<"_n "<<_n<<std::endl;
	std::cout<<"DEBUG INFO: _pd"<<_pd[0]*255<<" "<<_pd[1]*255<<" "<<_pd[2]*255<<" _ps"<<_ps[0]*255<<" "<<_ps[1]*255<<" "<<_ps[2]*255<<" "<<"_n "<<_n<<std::endl;
	*/

	//return the result with a status...
}

float SABrdf::evaluateCost( glm::vec3 _light_dir,  glm::vec3 & _sun_color, glm::vec3 & _pd,glm::vec3 &_ps, unsigned int & n ){

	//go through all the  samples to analyze and estimate the cost with  the model
	unsigned int Nsample;

	Nsample =_sample_normal.size();

	_cost_per_channel.resize(Nsample);

	for( int i = 0; i < Nsample; ++i){

		_sample_normal[i] =	glm::normalize ( _sample_normal[i] );
		_sample_view_dir[i] = glm::normalize ( _sample_view_dir[i]);

		_light_dir = glm::normalize (_light_dir);

		//		glm::vec3 half_vector = glm::normalize( (_light_dir + _sample_view_dir[i]) );

		glm::vec3 reflect = 2.0f * ( glm::dot(_sample_normal[i],_light_dir )) *_sample_normal[i] - _light_dir;

		glm::vec3 diffuse  = _pd*std::max( 0.0f, glm::dot((_sample_normal[i]),_light_dir ))*_sun_color; //*glm::one_over_pi<float>()

		glm::vec3 indirect = (_irradiance_sample[i]*_pd)*glm::one_over_pi<float>();

		//glm::vec3 specular = _ps*std::max(0.0f, (float)((1.0f/6.28f)*(n+2)*std::pow( std::cos(glm::dot(_sample_normal[i],half_vector)),n)));

		glm::vec3 specular = _ps*std::max(0.0f, (float) std::pow( glm::dot(reflect,_sample_view_dir[i]),n) );

		//force specular to be 0.... ?

		_cost_per_channel[i] = _radiance_sample[i] - ( diffuse +indirect + specular );

	}

	//evaluate cost 
	float t_cost = 0.0f;

	for( unsigned int kk = 0; kk < _cost_per_channel.size(); kk++){

		//each line has the SAME WEIGHT !!!!
		t_cost += std::abs(_cost_per_channel[kk].r) + std::abs(_cost_per_channel[kk].g) + std::abs(_cost_per_channel[kk].b) ;
	}

	return std::abs(t_cost);
}

float SABrdf::generateVarWithPdf(glm::vec2 & range, unsigned int pdfmodel ){

	float ifun = rd_uniform() %100;

	return ifun/100.0f;
}

int SABrdf::generateVarWithPdf(glm::ivec2 & range, unsigned int pdfmodel ){
	//std::srand  (time(NULL));
	unsigned int ifun = std::rand() %100 +1;

	return ifun;
}


glm::vec3 SABrdf::getFitDiffuse(void){
	return _pd;
}

glm::vec3 SABrdf::getFitSpecular(void){
	return _ps;
}

unsigned int SABrdf::getFitLobeN(void){
	return _n;
}