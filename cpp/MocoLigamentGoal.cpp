#include "MocoLigamentGoal.h"

using namespace OpenSim;
using namespace std;

void MocoLigamentGoal::initializeOnModelImpl(const Model&) const {
    setRequirements(1, 1);
}

void MocoLigamentGoal::calcIntegrandImpl(
		const IntegrandInput& input, double& integrand) const {
	getModel().realizeVelocity(input.state);

	const ForceSet& fset = getModel().getForceSet();
	vector<string> ligamentNames;
	for (int i = 0; i < fset.getSize(); i++) {
		string fname = fset[i].getName();
		if (fname.rfind("tau", 0) != 0) {
			ligamentNames.push_back(fname);
		}
	}

	//SimTK::Vec9 strains;
	SimTK::Vec9 forces;
	double max_tension = 0;
	size_t ligNum = ligamentNames.size();
	for (int i = 0; i < ligNum; i++) {
		Blankevoort1991Ligament* ligament = dynamic_cast<Blankevoort1991Ligament*>(&fset.get(i));
		if (ligament) {
			//double strain = ligament->getStrain(input.state);
			//strains[i] = (strain > 0) ? strain : 0.0;
			double force = ligament->getTotalForce(input.state);
			forces[i] = force;
			if (force > max_tension) {
				max_tension = force;
			}
		}
		else {
			cout << "dynamic_cast failed" << endl;
		}
	}
	if (max_tension <= 0) {
		max_tension = 1;
	}
	

	//strains[0] = strains[0] * 10;
	//cout << strains << endl;
	//integrand = strains.normSqr();
	forces = forces / max_tension;
	integrand = forces.normSqr();
}

void MocoLigamentGoal::calcGoalImpl(
		const GoalInput& input, SimTK::Vector& cost) const {
    cost[0] = input.integral;
}

