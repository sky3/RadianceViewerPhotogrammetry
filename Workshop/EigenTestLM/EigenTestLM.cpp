// Eigen_xp.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include <stdlib.h>

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include <iostream>
#include <Eigen/Dense>
#include <Eigen/Sparse>

//#include <unsupported/Eigen/NonLinearOptimization>

#include <unsupported/Eigen/LevenbergMarquardt>


// LM minimize for the model y = a x + b
typedef std::vector<Eigen::Vector3d,Eigen::aligned_allocator<Eigen::Vector3d> > Point2DVector;

Point2DVector GeneratePoints();

// Generic functor

template<typename _Scalar, int NX = Eigen::Dynamic, int NY = Eigen::Dynamic>
struct Functor
{
	typedef _Scalar Scalar;
	enum {
		InputsAtCompileTime = NX,
		ValuesAtCompileTime = NY
	};

	typedef Eigen::Matrix<Scalar,InputsAtCompileTime,1> InputType;
	typedef Eigen::Matrix<Scalar,ValuesAtCompileTime,1> ValueType;
	typedef Eigen::Matrix<Scalar,ValuesAtCompileTime,InputsAtCompileTime> JacobianType;
	typedef Eigen::ColPivHouseholderQR<JacobianType> QRSolver;

	int m_inputs, m_values;

	Functor() : m_inputs(InputsAtCompileTime), m_values(ValuesAtCompileTime) {}
	Functor(int inputs, int values) : m_inputs(inputs), m_values(values) {}

	int inputs() const { return m_inputs; }
	int values() const { return m_values; }

};


struct MyFunctor : Functor<double>
{
	int operator()(const Eigen::VectorXd &x, Eigen::VectorXd &fvec) const
	{
		// "a" in the model is x(0), and "b" is x(1)
		for(unsigned int i = 0; i < this->Points.size(); ++i)
		{
			fvec(i) = this->Points[i](2) - (x(0) * this->Points[i](0)* this->Points[i](0) + x(1)*this->Points[i](1) + x(2) ) ;
		}

		return 0;
	}

	Point2DVector Points;

	int inputs() const { return 3; } // There are two parameters of the model
	int values() const { return this->Points.size(); } // The number of observations
};


struct MyFunctorNumericalDiff : Eigen::NumericalDiff<MyFunctor> {};

Point2DVector GeneratePoints(const unsigned int numberOfPoints)
{
	Point2DVector points;
	// Model y = 2*x + 5 with some noise (meaning that the resulting minimization should be about (2,5)
	for(unsigned int i = 0; i < numberOfPoints; ++i)
	{
		double x = static_cast<double>(i);
		Eigen::Vector3d point;

		point(0) = x;
		point(1) = cos(x);
		point(2) = 1.5*x*x + 5.0 + ( std::rand()% 100 ) /1000.0  + 4.0*(cos(x));

		points.push_back(point);
	}

	return points;
}


/* BRDF */
typedef Eigen::Vector3d BRDFSampleData_radiance;
typedef Eigen::Vector3d BRDFSampleData_viewer;

std::vector<std::pair< BRDFSampleData_radiance,BRDFSampleData_viewer>> GenerateBRDF(const unsigned int numberOfPoints);

struct BRDFFunctor : Functor<double>
{

	
	std::vector<std::pair<BRDFSampleData_viewer,BRDFSampleData_radiance>>   _sample;

	int inputs() const { return 3; } // NB Parameters model
	int values() const { return this->_sample.size(); } // The number of observations


	int operator()(const Eigen::VectorXd &x, Eigen::VectorXd &fvec) const
	{

		//defines bounds
		/*
		vec p_min = _f->getParametersMin();
		vec p_max = _f->getParametersMax();
		for(int i=0; i<_f->nbParameters(); ++i)
		{
		if(x[i] < p_min[i] || x[i] > p_max[i])
		{
		return -1;
		}
		}
			*/
		if ( x(0) <0.0 ){
			return -1;
		}

		

		Eigen::Vector3d light_dir = Eigen::Vector3d(0.0,1.0,1.0) - Eigen::Vector3d(0.0,0.0,0.0); 
		Eigen::Vector3d normal_dir(0.0,1.0,0.0);   

		//-------------------
		// x(0) is diffuse kd
		// x(1) is specular ks
		// x(2) is n roughness

		for(unsigned int i = 0; i < this->_sample.size(); ++i)
		{

			//generate view observation
			Eigen::Vector3d view_dir(_sample[i].first); 

			view_dir.normalize();
			light_dir.normalize();
			normal_dir.normalize();

			//compute half vector 
			Eigen::Vector3d half_v =  (light_dir +view_dir);// / ( ( light_dir +view_dir).norm());
			half_v.normalize();

			double  diffuse  =  x(0)*std::max(0.0,normal_dir.dot(light_dir));
			double  specular =  x(1)*std::max(0.0,(1.0/6.28)*(x(2)+2.0)*std::pow( std::cos(normal_dir.dot(half_v)), x(2)));

			fvec(i) = _sample[i].second(0) - ( diffuse + specular );

			// double spec = std::max(0.0,(1.0/6.28)*(n+2.0)*std::pow( std::cos(normal_dir.dot(half_v)), n));
			//double diffuse = 0.2 *std::max(0.0,normal_dir.dot(light_dir));
			// fvec(i) = this->Points[i](2) - (x(0) * this->Points[i](0)* this->Points[i](0) + x(1)*this->Points[i](1) + x(2) ) ;
		}

			std::cout << "x that minimizes the function: " << std::endl << x << std::endl;
		return 0;
	}



	int df(const Eigen::VectorXf &x, Eigen::MatrixXf &fjac) const
	{
		// Implement dy/dx = 2*x
		fjac(0) = x(0);
		fjac(1) = x(1);
		fjac(2) = x(0);
		
		return 0;
	}




};


struct BRDFNumericalDiff : Eigen::NumericalDiff<BRDFFunctor> {};

std::vector<std::pair< BRDFSampleData_radiance,BRDFSampleData_viewer>> GenerateBRDF(const unsigned int numberOfPoints, float percentage_of_noisy_sample, float p_diffuse, float p_specular, float n_spec){

	std::vector<std::pair< BRDFSampleData_radiance,BRDFSampleData_viewer>> _sample;

	Eigen::Vector3d light_dir = Eigen::Vector3d(0.0,1.0,1.0) - Eigen::Vector3d(0.0,0.0,0.0); 
	Eigen::Vector3d normal_dir(0.0,1.0,0.0); 
	
	srand (time(NULL));

	for(unsigned int i = 0; i < numberOfPoints; ++i)
	{
		double x = static_cast<double>(i);

		//generate view observation randomly
		float  gx = (( std::rand()% 100 ) /100.0 ) *2.0 -1.0;
		float  gz = (( std::rand()% 100 ) /100.0 ) *2.0 -1.0;
	

		Eigen::Vector3d view_dir(gx, 0.5, gz); 
		Eigen::Vector3d radiance;

		view_dir.normalize();
		light_dir.normalize();
		normal_dir.normalize();

		//compute half vector 
		Eigen::Vector3d half_v =  (light_dir +view_dir);//  / ( ( light_dir +view_dir).norm());
		half_v.normalize();

		double n =n_spec;

		double spec = p_specular*std::max(0.0,(1.0/6.28)*(n+2.0)*std::pow( std::cos(normal_dir.dot(half_v)), n)) ;//+ ( std::rand()% 100 ) /1000.0;
		double diffuse =p_diffuse*( std::max(0.0,normal_dir.dot(light_dir)  +( 2.0*(std::rand()% 100)-100.0) /1000.0 ));


		//generate a percentage of correct sample.
		if ( i < numberOfPoints*percentage_of_noisy_sample){
			 spec = p_specular*std::max(0.0,(1.0/6.28)*(n+2.0)*std::pow( std::cos(normal_dir.dot(half_v)), n)) ; 
			 diffuse = p_diffuse*( std::max(0.0,normal_dir.dot(light_dir) ));
		}


		//1.5* x*x + 5.0 +

		radiance(0) =  diffuse+spec;
		radiance(1) =  diffuse+spec;
		radiance(2) =  diffuse+spec;

		_sample.push_back(std::make_pair<>(view_dir, radiance) );
	}

	return _sample;

}



/*end  BRDF*/


int main(int , char *[])
{

	srand (time(NULL));


	unsigned int numberOfPoints = 100;
	float percentage_of_noisy_samples =0.10f;

	float p_diffuse = 0.6;
	float p_specular = 0.21;
	float n_spec = 3;

	std::vector<std::pair< BRDFSampleData_radiance,BRDFSampleData_viewer>> sample_to_fit = GenerateBRDF(numberOfPoints,percentage_of_noisy_samples,p_diffuse, p_specular, n_spec);


	Eigen::VectorXd x(3);

	//mon initialisation
	x[0] = 0.5;
	x[1] = 0.5;
	x[2] = 1;

	BRDFNumericalDiff fit_brdf;
	fit_brdf._sample = sample_to_fit;
	Eigen::LevenbergMarquardt<BRDFNumericalDiff> lm_brdf(fit_brdf);

	
	Eigen::LevenbergMarquardtSpace::Status statuslm_brdf = lm_brdf.minimize(x);
	std::cout << "status: " << statuslm_brdf << std::endl;

	
	Eigen::LevenbergMarquardtSpace::Status statuslm_brdf2= lm_brdf.minimize(x);
	std::cout << "status: " << statuslm_brdf2 << std::endl;

	//std::cout << "info: " << lm.info() << std::endl;

	std::cout << "x that minimizes the function: " << std::endl << x << std::endl;

	/*
	Point2DVector points = GeneratePoints(numberOfPoints);

	Eigen::VectorXd x(3);
	x.fill(0.0f);

	MyFunctorNumericalDiff functor;
	functor.Points = points;
	Eigen::LevenbergMarquardt<MyFunctorNumericalDiff> lm(functor);

	Eigen::LevenbergMarquardtSpace::Status status = lm.minimize(x);
	std::cout << "status: " << status << std::endl;

	//std::cout << "info: " << lm.info() << std::endl;

	std::cout << "x that minimizes the function: " << std::endl << x << std::endl;
	*/

	return 0;
}