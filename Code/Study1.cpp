#include "filesystem"
#include "utilities.h"
#include "MocoAccelGoal.h"
#include "MocoHeightGoal.h"
#include "MocoHeightVelGoal.h"
#include "MocoBalanceGoal.h"
#include "MocoBalanceGoalRight.h"
#include "MocoLigamentGoal.h"
#include <OpenSim/Moco/osimMoco.h>
#include <OpenSim/Common/STOFileAdapter.h>
#include <OpenSim/Actuators/TorqueActuator.h>
#include <OpenSim/Actuators/CoordinateActuator.h>
#include <OpenSim/Moco/MocoOutputConstraint.h>
#include <OpenSim/Common/Constant.h>

using namespace OpenSim;
using namespace SimTK;
using namespace std;
namespace fs = std::filesystem;

// Create Model
//Model editModel(const string& modelFileName, vector<double> ligLengths) {
Model editModel(const string& modelFileName) {
	Model model(modelFileName);

	const Set<Coordinate>& coord = model.getCoordinateSet();

	// Coordinate Indices
	//int pelv_tilt = 0;
	//int pelv_tx = 1;
	//int pelv_ty = 2;
	//int hip_flex = 3;
	//int hip_add = 4;
	//int hip_rot = 5;
	//int knee_flex = 6;
	//int knee_add = 7;
	//int knee_rot = 8;
	//int patella = 9;
	//int ankle = 10;
	//int subtalar = 11;
	//int lumbar_ext = 12;
	//int patella = 7;
	//int ankle = 8;
	//int subtalar = 9;
	//int lumbar_ext = 10;
	std::vector<std::string> includedNames = {"hip_flexion_r", "knee_angle_r", "ankle_angle_r"};

	for (int i = 0; i < coord.getSize(); i++) {

		std::string coordName = coord[i].getName();
		if (std::find(includedNames.begin(), includedNames.end(), coordName) != includedNames.end()) {

			double optForce = 300;

			//if (i == hip_flex) {
			//	optForce = 200;
			//} 
			//else if (i == knee_flex) {
			//	optForce = 200;
			//}
			//else {
			//	optForce = 100;
			//}

			auto* coordActu = new CoordinateActuator();
			coordActu->setName("tau_" + coord[i].getName());
			coordActu->setCoordinate(&coord[i]);
			coordActu->setOptimalForce(optForce);
			coordActu->setMinControl(-1);
			coordActu->setMaxControl(1);
			model.addForce(coordActu);

			// Print the actuator information.
			std::cout << coordActu->getName() << ": " << coordActu->getOptimalForce() << "N" << endl;
		}
	}

	model.finalizeConnections();

	return model;
}

int main() {
	// ================
	// Set Up Variables	
	// ================
	vector<double> weights = { 1,0,0.004,1,1,1 };
	//double sum = std::accumulate(weights.begin(), weights.end(), 0.0);
	//for (double& weight : weights) { weight /= sum; }
	//for (int i = 0; i < weights.size(); i++) {
	//	std::cout << weights[i] << endl;
	//}
	double comWeight = weights[0];
	double balWeight = weights[1];
	double accWeight = weights[2];
	vector<double> effWeights(weights.begin() + 3, weights.end());
	double finTime = 2;
	int meshVal = 100;
	int numIter = 1e3;
	const double& convTol = 1e-4;
	const double& constTol = 1e-4;

	string modelFiles = "Models/squatToStand_3dof9musc.osim";

	// ================
	// Run Optimisation
	// ================
	// Set Model
	Model model = editModel(modelFiles);
	State state = model.initSystem();
	model.realizeAcceleration(state);
	
	// Set Up Moco
	MocoStudy study;
	MocoProblem& problem = study.updProblem();
	problem.setModelAsCopy(model);
	problem.setTimeBounds(0, finTime);

	// Set boundary conditions
	problem.setStateInfo("/jointset/hip_r/hip_flexion_r/value", MocoBounds(-120 * Pi / 180, 0),
		MocoInitialBounds(0), MocoFinalBounds(0));
	problem.setStateInfo("/jointset/knee_r/knee_angle_r/value", MocoBounds(-120 * Pi / 180, 0),
		MocoInitialBounds(0), MocoFinalBounds(0));
	problem.setStateInfo("/jointset/ankle_r/ankle_angle_r/value", MocoBounds(-30 * Pi / 180, 0),
		MocoInitialBounds(0), MocoFinalBounds(0));
	//problem.setStateInfo("/jointset/back/lumbar_extension/value",
	//	MocoBounds(variableValues[subject][3][0] * Pi / 180, variableValues[subject][3][1] * Pi / 180),
	//	MocoInitialBounds(variableValues[subject][3][2] * Pi / 180), MocoFinalBounds(variableValues[subject][3][3] * Pi / 180));


	problem.setStateInfoPattern("/jointset/.*/speed", {}, MocoInitialBounds(0), MocoFinalBounds(0));

	// Set goals
	problem.addGoal<MocoHeightGoal>("height", comWeight);
	problem.addGoal<MocoBalanceGoalRight>("balance", balWeight);
	auto* effortGoal = problem.addGoal<MocoControlGoal>("effort", 1);
	effortGoal->setExponent(2);
	const ForceSet& fset = model.getForceSet();
	for (int i = 0; i < fset.getSize(); i++) {
		effortGoal->setWeightForControl("/forceset/" + fset[i].getName(), effWeights[i]);
	}


	// Configure the solver
	MocoCasADiSolver& solver = study.initCasADiSolver();
	solver.set_multibody_dynamics_mode("implicit");
	solver.set_minimize_implicit_multibody_accelerations(true);
	solver.set_implicit_multibody_accelerations_weight(accWeight);
	solver.set_num_mesh_intervals(meshVal);
	solver.set_verbosity(2);
	solver.set_optim_solver("ipopt");
	//solver.set_optim_ipopt_print_level(4);
	solver.set_optim_max_iterations(numIter);
	solver.set_optim_convergence_tolerance(convTol);
	solver.set_optim_constraint_tolerance(constTol);

	MocoTrajectory guess = solver.createGuess("bounds");
	guess.resampleWithNumTimes(2);
	solver.setGuess(guess);
	study.print("Study1.moco");

	// Solve the problem
	MocoSolution solution = study.solve();
	solution.unseal();
	//auto predicted_motion = solution.getStatesTrajectory();

	// Save the solution
	solution.write("Results/Study1/solution.mot");

	std::cout << "Solution status: " << solution.getStatus() << endl;

	return EXIT_SUCCESS;

}