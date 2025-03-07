#include <OpenSim/Common/STOFileAdapter.h>
#include <OpenSim/Moco/osimMoco.h>
#include <OpenSim/Actuators/CoordinateActuator.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <array>
#include <cmath>
#include <OpenSim/Common/SimmSpline.h>
#include <OpenSim/Common/Constant.h>


using namespace OpenSim;
using namespace std;
using namespace SimTK;

int main() {

	// Ligament
	//                     cPT aLCL  mLCL  pLCL  aMCL  mMCL  pMCL   aPCL   pPCL  sMPFL mMPFL iMPFL sLPFL  mLPFL iLPFL
	//double ligFactors[] = { 0, 0.03, 0.03, 0.03, 0.04, 0.04, 0.05, -0.24, -0.03, 0.07, 0.07, 0.07, 0.06, 0.06, 0.06 }; //blankvoort
	double ligFactors[]   = { 0, -0.25, -0.05, 0.08, 0.04, 0.04, -0.03, -0.24, -0.03, 0.07, 0.07, 0.07, 0.06, 0.06, 0.06 }; //blankvoort
	double ligStiffness[] = { 6000, 2000, 2000, 2000, 2750, 2750, 2750, 9000, 9000, 1200, 1100, 1000, 1200, 1100, 1000 };
	
	string modelName = "K1L_scaled_sl_na";
	Model model = Model("Models/" + modelName + ".osim");
	auto& state = model.initSystem();
	model.realizeAcceleration(state);
	const auto& fset = model.getForceSet();
	cout << fset.getSize() << endl;
	
	Vec9 forces;
	double max_tension;

	for (int i = 0; i < fset.getSize(); ++i) {
		Blankevoort1991Ligament* ligament = dynamic_cast<Blankevoort1991Ligament*>(&fset.get(i));
		cout << ligament->getName() << endl;
		cout << "length: " << ligament->getLength(state) << endl;
		//ligament->set_linear_stiffness(ligStiffness[i]);
		//ligament->setSlackLengthFromReferenceStrain(ligFactors[i], state);
		cout << "stiffness: " << ligament->get_linear_stiffness() << endl;
		cout << "slack length: " << ligament->get_slack_length() << endl;
		cout << "ref strain: " << ligFactors[i] << endl;
		cout << "strain: " << ligament->getStrain(state) << endl;
		cout << "force: " << ligament->getTotalForce(state) << endl;
		cout << "" << endl;
		double ligforce = ligament->getTotalForce(state);
		forces[i] = ligforce;

		if (forces[i] > max_tension) {
			max_tension = forces[i];
		}
	}

	//cout << max_tension << endl;

	cout << "" << endl;
	for (int i = 0; i < fset.getSize(); ++i) {
		cout << forces[i] << endl;
	}

	forces = forces/max_tension;

	cout << "" << endl;
	for (int i = 0; i < fset.getSize(); ++i) {
		cout << forces[i] << endl;
	}

	model.finalizeConnections();
	//model.setName(modelName);
	//model.print("Models/" + modelName + ".osim");

	//string modelName = "K1L_scaled_sl_na";
	//Model model("Models/" + modelName + ".osim");
	//auto& state = model.initSystem();
	//model.realizeAcceleration(state);
	//
	////SimTK::Vector accel;
	//std::vector<double> accel;
	//const auto& cset = model.getCoordinateSet();
	//for (int i = 0; i < cset.getSize(); ++i) {
	//	auto& coord = cset.get(i);
	//	string coordName = coord.getName();
	//	if (coordName.find("patellar") != std::string::npos) {
	//		continue;  // Skip this coordinate if the name contains "patellar"
	//	}
	//	auto val = coord.getAccelerationValue(state);
	//	cout << coordName << ": " << val << endl;
	//	accel.push_back(val);
	//}
	//
	//for (int i = 0; i < accel.size(); ++i) {
	//	cout << accel[i] << endl;
	//}


	//const auto& cset = model.getCoordinateSet();
	//for (int i = 0; i < cset.getSize(); ++i) {
	//	auto coord = cset.get(i);
	//	if (coord.getName() == "knee_flexion_l") {
	//		cout << coord.getName() << endl;
	//		cout << "value_pre: " << coord.get_default_value() << endl;
	//		coord.upd_default_value(90 * Pi / 180);
	//		cout << "value_new: " << coord.get_default_value() << endl;
	//	}
	//	else {
	//		continue;
	//	}
	//}
	//model.updCoordinateSet().get("knee_flexion_l").setValue(state, kneeFlexion90deg);
	//model.realizePosition(state);
	//
	//cout << "Ligament Parameters at 90 Degrees Knee Flexion:" << endl;
	//
	//// Check ligament properties at 90-degree knee flexion
	//for (int i = 0; i < fset.getSize(); ++i) {
	//	Blankevoort1991Ligament* ligament = dynamic_cast<Blankevoort1991Ligament*>(&fset.get(i));
	//	if (ligament) {
	//		cout << ligament->getName() << endl;
	//		cout << "Length: " << ligament->getLength(state) << endl;
	//		cout << "Stiffness: " << ligament->get_linear_stiffness() << endl;
	//		cout << "Slack Length: " << ligament->get_slack_length() << endl;
	//		cout << "Strain: " << ligament->getStrain(state) << endl;
	//		cout << "Force: " << ligament->getTotalForce(state) << endl;
	//		cout << "" << endl;
	//	}
	//}

	return 0;

}
