#include "SplatData.h"
#include "../external/happly.h"
#include "../utils/GeneralUtils.h"

float sigmoid(float x)
{
    return 1.0f / (1.0f + expf(-x));
}

SplatData::SplatData(const std::string &path)
{
    loadFromPLY(path);
}

bool SplatData::loadFromPLY(const std::string &path)
{
    std::cout << "Loading " << path << "..." << std::endl;
    happly::PLYData inData(path);
    std::cout << "Loading positions..." << std::endl;
	std::vector<float> pointsX = inData.getElement("vertex").getProperty<float>("x");
	std::vector<float> pointsY = inData.getElement("vertex").getProperty<float>("y");
	std::vector<float> pointsZ = inData.getElement("vertex").getProperty<float>("z");

	//Harmonics
    std::cout << "Loading harmonics..." << std::endl;
	std::vector<float> shDC0 = inData.getElement("vertex").getProperty<float>("f_dc_0");
	std::vector<float> shDC1 = inData.getElement("vertex").getProperty<float>("f_dc_1");
	std::vector<float> shDC2 = inData.getElement("vertex").getProperty<float>("f_dc_2");

    std::vector<float> shRest0 = inData.getElement("vertex").getProperty<float>("f_rest_0");
    std::vector<float> shRest1 = inData.getElement("vertex").getProperty<float>("f_rest_1");
    std::vector<float> shRest2 = inData.getElement("vertex").getProperty<float>("f_rest_2");
    std::vector<float> shRest3 = inData.getElement("vertex").getProperty<float>("f_rest_3");
    std::vector<float> shRest4 = inData.getElement("vertex").getProperty<float>("f_rest_4");
    std::vector<float> shRest5 = inData.getElement("vertex").getProperty<float>("f_rest_5");
    std::vector<float> shRest6 = inData.getElement("vertex").getProperty<float>("f_rest_6");
    std::vector<float> shRest7 = inData.getElement("vertex").getProperty<float>("f_rest_7");
    std::vector<float> shRest8 = inData.getElement("vertex").getProperty<float>("f_rest_8");
    std::vector<float> shRest9 = inData.getElement("vertex").getProperty<float>("f_rest_9");
    std::vector<float> shRest10 = inData.getElement("vertex").getProperty<float>("f_rest_10");
    std::vector<float> shRest11 = inData.getElement("vertex").getProperty<float>("f_rest_11");
    std::vector<float> shRest12 = inData.getElement("vertex").getProperty<float>("f_rest_12");
    std::vector<float> shRest13 = inData.getElement("vertex").getProperty<float>("f_rest_13");
    std::vector<float> shRest14 = inData.getElement("vertex").getProperty<float>("f_rest_14");
    std::vector<float> shRest15 = inData.getElement("vertex").getProperty<float>("f_rest_15");
    std::vector<float> shRest16 = inData.getElement("vertex").getProperty<float>("f_rest_16");
    std::vector<float> shRest17 = inData.getElement("vertex").getProperty<float>("f_rest_17");
    std::vector<float> shRest18 = inData.getElement("vertex").getProperty<float>("f_rest_18");
    std::vector<float> shRest19 = inData.getElement("vertex").getProperty<float>("f_rest_19");
    std::vector<float> shRest20 = inData.getElement("vertex").getProperty<float>("f_rest_20");
    std::vector<float> shRest21 = inData.getElement("vertex").getProperty<float>("f_rest_21");
    std::vector<float> shRest22 = inData.getElement("vertex").getProperty<float>("f_rest_22");
    std::vector<float> shRest23 = inData.getElement("vertex").getProperty<float>("f_rest_23");
    std::vector<float> shRest24 = inData.getElement("vertex").getProperty<float>("f_rest_24");
    std::vector<float> shRest25 = inData.getElement("vertex").getProperty<float>("f_rest_25");
    std::vector<float> shRest26 = inData.getElement("vertex").getProperty<float>("f_rest_26");
    std::vector<float> shRest27 = inData.getElement("vertex").getProperty<float>("f_rest_27");
    std::vector<float> shRest28 = inData.getElement("vertex").getProperty<float>("f_rest_28");
    std::vector<float> shRest29 = inData.getElement("vertex").getProperty<float>("f_rest_29");
    std::vector<float> shRest30 = inData.getElement("vertex").getProperty<float>("f_rest_30");
    std::vector<float> shRest31 = inData.getElement("vertex").getProperty<float>("f_rest_31");
    std::vector<float> shRest32 = inData.getElement("vertex").getProperty<float>("f_rest_32");
    std::vector<float> shRest33 = inData.getElement("vertex").getProperty<float>("f_rest_33");
    std::vector<float> shRest34 = inData.getElement("vertex").getProperty<float>("f_rest_34");
    std::vector<float> shRest35 = inData.getElement("vertex").getProperty<float>("f_rest_35");
    std::vector<float> shRest36 = inData.getElement("vertex").getProperty<float>("f_rest_36");
    std::vector<float> shRest37 = inData.getElement("vertex").getProperty<float>("f_rest_37");
    std::vector<float> shRest38 = inData.getElement("vertex").getProperty<float>("f_rest_38");
    std::vector<float> shRest39 = inData.getElement("vertex").getProperty<float>("f_rest_39");
    std::vector<float> shRest40 = inData.getElement("vertex").getProperty<float>("f_rest_40");
    std::vector<float> shRest41 = inData.getElement("vertex").getProperty<float>("f_rest_41");
    std::vector<float> shRest42 = inData.getElement("vertex").getProperty<float>("f_rest_42");
    std::vector<float> shRest43 = inData.getElement("vertex").getProperty<float>("f_rest_43");
    std::vector<float> shRest44 = inData.getElement("vertex").getProperty<float>("f_rest_44");

    std::cout << "Loading rotations..." << std::endl;
	std::vector<float> rot_x = inData.getElement("vertex").getProperty<float>("rot_0");
	std::vector<float> rot_y = inData.getElement("vertex").getProperty<float>("rot_1");
	std::vector<float> rot_z = inData.getElement("vertex").getProperty<float>("rot_2");
	std::vector<float> rot_w = inData.getElement("vertex").getProperty<float>("rot_3");

    std::cout << "Loading scales..." << std::endl;
	std::vector<float> scale_x = inData.getElement("vertex").getProperty<float>("scale_0");
	std::vector<float> scale_y = inData.getElement("vertex").getProperty<float>("scale_1");
	std::vector<float> scale_z = inData.getElement("vertex").getProperty<float>("scale_2");

    std::cout << "Loading opacities..." << std::endl;
	std::vector<float> opacities = inData.getElement("vertex").getProperty<float>("opacity");

    _numberOfGaussians = pointsX.size();
    std::cout << "Configuring " << _numberOfGaussians << " Gaussians...\n";
    ProgressBar* bar = new ProgressBar(_numberOfGaussians, "Gaussians Configured: ");
	for (int i = 0; i < _numberOfGaussians; i++)
	{
        bar->update(1);
		_positions.push_back(glm::vec4(pointsX[i], pointsY[i], pointsZ[i], 1.0f));  
        glm::vec4 scale = glm::vec4(exp(scale_x[i]), exp(scale_y[i]), exp(scale_z[i]), 1.0f);
        _scales.push_back(scale);
		_opacities.push_back(sigmoid(opacities[i]));
        glm::vec4 rotation = glm::normalize(glm::vec4(rot_x[i], rot_y[i], rot_z[i], rot_w[i]));
        _rotations.push_back(rotation);
        std::vector<float> covariance = build_covariance3D(glm::vec3(scale), rotation);
        for (int j = 0; j < 6; j++)
        {
            _covariances3D.push_back(covariance[j]);
        }
        _colors.push_back(glm::vec4(shDC0[i], shDC1[i], shDC2[i], 1.0));
        _colors.push_back(glm::vec4(shRest0[i], shRest1[i], shRest2[i], 1.0));
        _colors.push_back(glm::vec4(shRest3[i], shRest4[i], shRest5[i], 1.0));
        _colors.push_back(glm::vec4(shRest6[i], shRest7[i], shRest8[i], 1.0));
        _colors.push_back(glm::vec4(shRest9[i], shRest10[i], shRest11[i], 1.0));
        _colors.push_back(glm::vec4(shRest12[i], shRest13[i], shRest14[i], 1.0));
        _colors.push_back(glm::vec4(shRest15[i], shRest16[i], shRest17[i], 1.0));
        _colors.push_back(glm::vec4(shRest18[i], shRest19[i], shRest20[i], 1.0));
        _colors.push_back(glm::vec4(shRest21[i], shRest22[i], shRest23[i], 1.0));
        _colors.push_back(glm::vec4(shRest24[i], shRest25[i], shRest26[i], 1.0));
        _colors.push_back(glm::vec4(shRest27[i], shRest28[i], shRest29[i], 1.0));
        _colors.push_back(glm::vec4(shRest30[i], shRest31[i], shRest32[i], 1.0));
        _colors.push_back(glm::vec4(shRest33[i], shRest34[i], shRest35[i], 1.0));
        _colors.push_back(glm::vec4(shRest36[i], shRest37[i], shRest38[i], 1.0));
        _colors.push_back(glm::vec4(shRest39[i],shRest40[i], shRest41[i], 1.0));
        _colors.push_back(glm::vec4(shRest42[i], shRest43[i], shRest44[i], 1.0));
    }
    std::cout << std::endl;
    _center = glm::vec3(0.0f);
    std::cout << "PLY file read!\n";
    return true;
}

const glm::vec3 &SplatData::getCenter() const
{
    return _center;
}
int SplatData::getNumberOfGaussians() const
{
    return _numberOfGaussians;
}

const std::vector<glm::vec4> &SplatData::getPositions() const
{
    return _positions;
}
const std::vector<glm::vec4> &SplatData::getScales() const
{
    return _scales;
}
const std::vector<glm::vec4> &SplatData::getColors() const
{
    return _colors;
}
const std::vector<glm::vec4> &SplatData::getRotations() const
{
    return _rotations;
}
const std::vector<float> &SplatData::getOpacities() const
{
    return _opacities;
}
const std::vector<float> &SplatData::getCovariances3D() const
{
    return _covariances3D;
}