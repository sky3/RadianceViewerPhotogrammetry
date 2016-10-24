#ifndef SA_BRDF_H
#define SA_BRDF_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <vector>

#include <limits>

class SABrdf{

public:
	SABrdf();
	~SABrdf();

	SABrdf(const SABrdf &);
    SABrdf &operator=(const SABrdf &);

	void addSamples(glm::vec3 normal, glm::vec3 sample_view_dir , glm::vec3 radiance_sample, glm::vec3 irradiance_sample );
	void solve();

	void CERES_example();
	void CERES_Solve();

	float generateVarWithPdf(glm::vec2 & range, unsigned int pdfmodel );
	int generateVarWithPdf(glm::ivec2 & range, unsigned int pdfmodel );

	bool checkPrior(float pd, float ps, int n);

	
	void setSunDirection(glm::vec3 sundir);
	void setSunColor(glm::vec3 suncolor);

	SABrdf &operator+(const SABrdf &);

	glm::vec3 getMedianeRadiance(void);

	glm::vec3 getFitDiffuse(void);
	glm::vec3 getFitSpecular(void);
	unsigned int getFitLobeN(void);

private:
	float evaluateCost(glm::vec3 _light_dir,  glm::vec3 & _sun_color, glm::vec3 & _pd,glm::vec3 &_ps, unsigned int & n );

	//--lighting model
	glm::vec3 _sundir;
	glm::vec3 _sun_color;

	//-- assume phong model
	glm::vec2 _range_pd; //range of value to born pd
	glm::vec2 _range_ps; //range of value to born ps
	glm::ivec2 _range_n; //range of value to born n


	//--assume map 
	glm::vec3 _pd;
	glm::vec3 _ps;
	unsigned int _n;

	//store samples...
	std::vector<glm::vec3> _sample_normal;
	std::vector<glm::vec3> _sample_view_dir;
	std::vector<glm::vec3> _radiance_sample;
	std::vector<glm::vec3> _irradiance_sample;

	std::vector<glm::vec3> _cost_per_channel;

};

#endif