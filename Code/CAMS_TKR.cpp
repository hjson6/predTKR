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
Model editModel(const string& modelFileName, const string& whichModel, const string& component, const string& plane, const double& angle) {
	Model model(modelFileName);

	if (whichModel == "ref") {

		string compStr1;
		string compStr2;
		SimTK::Vec3 orientation1(0, 0, 0);
		SimTK::Vec3 orientation2; (0, 0, 0);
		std::vector<double> femOrder{ 1, 2, 0 };
		std::vector<double> tibOrder{ 0, 1, 2 };
		int planeIdx;
		double desiredAngle1 = angle * Pi / 180;
		double desiredAngle2 = angle * Pi / 180;

		if (plane == "vv") {
			planeIdx = 0;
		}
		else if (plane == "ir") {
			planeIdx = 1;
		}
		else if (plane == "sg") {
			planeIdx = 2;
		}
		else { std::cout << "wrong plane" << endl; }

		if (component == "femoral") {
			compStr1 = "femur_weld_r";
			orientation1[femOrder[planeIdx]] = desiredAngle1;
		}
		else if (component == "tibial") {
			compStr1 = "tibial_plat_weld_r";
			orientation1[tibOrder[planeIdx]] = desiredAngle1;
		}
		else if (component == "combined") {
			compStr1 = "femur_weld_r";
			compStr2 = "tibial_plat_weld_r";
			orientation1[femOrder[planeIdx]] = desiredAngle1;
			orientation2[tibOrder[planeIdx]] = desiredAngle2;
		}
		else { std::cout << "wrong component" << endl; }

		if (component == "combined") {
			auto& upd_fem = model.updJointSet().get(compStr1);
			auto& upd_tib = model.updJointSet().get(compStr2);
			upd_fem.upd_frames(1).set_orientation(orientation1);
			upd_tib.upd_frames(1).set_orientation(orientation2);
		}
		else {
			auto& upd = model.updJointSet().get(compStr1);
			upd.upd_frames(1).set_orientation(orientation1);
		}
	}

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
	std::vector<std::string> excludedNames = { "pelvis_tilt", "pelvis_tx", "pelvis_ty",
										   "knee_adduction_r", "knee_rotation_r", "patellar_flexion_r",
										   "knee_adduction_l", "knee_rotation_l", "patellar_flexion_l" };

	for (int i = 0; i < coord.getSize(); i++) {

		std::string coordName = coord[i].getName();
		if (std::find(excludedNames.begin(), excludedNames.end(), coordName) != excludedNames.end()) {
			continue;
		}

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

	model.finalizeConnections();

	return model;
}

int main() {
	// ================
	// Set Up Variables	
	// ================

	string whichModel = "exp"; // exp, ref
	//string component = "tibial"; // femoral, tibial, combined *Don't use combined for neutral
	vector<string> component = { "tibial", "femoral" };
	string plane = "vv"; // vv = varus-valgus; ir = axial rotation; sg = saggital plane;
	//vector<double> weights = { 2, 7, 1e-5, 0.003, 0.5, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02 }; // acc, bal, com, lig, effi
	//vector<double> weights = {2.433828577, 0.932261477, 0.917714022, 2.83183E-09, 0.71504762, 10.07091569, 1.022030431, 0.920095211, 5.144120459, 5.162245846, 1}; // acc, bal, com, lig, effi
	//vector<double> weights = { 3, 0, 10, 0.002, 0.02, 0.02, 0.02, 0.02 }; // acc, bal, com, lig, effi
	//vector<double> weights = {2.5, 1, 1, 2e-4, 0.004, 1};
	vector<double> weights = { 2.5, 0.1, 0.015, 0.004, 0.9, 1, 1, 1, 1, 1, 1 };
	double sum = std::accumulate(weights.begin(), weights.end(), 0.0);
	for (double& weight : weights) { weight /= sum; }
	for (int i = 0; i < weights.size(); i++) {
		std::cout << weights[i] << endl;
	}
	double comWeight = weights[0];
	//double comvelWeight = weights[1];
	double balWeight = weights[1];
	double ligWeight = weights[2];
	double accWeight = weights[3];
	//double effWeight = weights[4];
	vector<double> effWeights(weights.begin() + 4, weights.end());
	double finTime = 2;
	double subjects = 3;
	double optForce = 300;
	double misalignment = 0;
	int meshVal = 100;
	int numIter = 1e3;
	const double& convTol = 1e-4;
	const double& constTol = 1e-4;

	vector<string> modelFiles;
	vector<vector<vector<double>>> variableValues;
	double scaleFactors[] = { 1, 1, 1, 1, 1, 1 };
	if (whichModel == "exp") {
		// Models
		modelFiles = {
			"Models/K1L_scaled_sl_na.osim",
			//"Models/K1L_scaled-sl_1991_knee3_edit.osim",
			"Models/K2L_scaled_sl_na.osim",
			"Models/K3R_scaled_sl_na.osim",
			"Models/K5R_scaled-sl_1991_knee3_edit.osim",
			"Models/K7L_scaled_sl_na_0.osim",
			"Models/K8L_scaled-sl_1991_knee3_edit_edit.osim"
		};

		variableValues = {   // {min, max, initial, final} https://pdfs.semanticscholar.org/6a9e/f899e231ae97e2c1e8ba0174ec02ca4625da.pdf
			//K1L
			{
				{-2,120,17.6804,-1.8976}, // hip
				{0,120,14.3198,3.1288}, // knee
				{-1,25,-0.046046,-0.24193}, // ankle
				{-55,-22,-22.2279,-22.6767}, // lumbar
			},
			//K2L
			{
				{0,120,21.6287,22.606}, // hip
				{0,120,1.847,2.5187}, // knee
				{-3,25,-2.4244,-2.4205}, // ankle
				{-25,0,-0.72483,-2.7197}, // lumbar
			},
			//K3R
			{
				{0,125,10.0987,8.2774}, // hip
				{0,125,4.1928,0.65917}, // knee
				{0,25,1.8606,1.3815}, // ankle
				{-25,2,-2.2115,1.407}, // lumbar
			},
			//K5R
			{
				{0,125,11.0822,4.5716}, // hip
				{0,125,13.2456,11.8971}, // knee
				{0,25,5.119,5.4873}, // ankle
				{-45,0,-16.6375,-14.4588}, // lumbar
			},
			//K7L
			{
				{0,125,7.2589,9.489}, // hip
				{0,125,11.7545,0.92006}, // knee
				{0,25,5.1706,3.7186}, // ankle
				{-25,0,-12.2898,-2.1126}, // lumbar
			},
			//K8L
			{
				{0,125,11.203,12.102}, // hip
				{0,125,4.0195,3.2399}, // knee
				{-5,25,-3.8394,-1.6033}, // ankle
				{-15,0,-7.9013,-1.7785}, // lumbar
			},
		};
	}
	else if (whichModel == "ref") {

		// Models
		modelFiles = {
			//"Models/K5R_scaled-sl-c - Copy.osim"
			//"Models/K5R_edit.osim"
			//"Models/Ref_sl - Copy.osim"
			//"Models/Ref_sl_edit.osim"
			//"Models/TKR_Lig_Moir&Marra_Catelli_PLrigid_SL_CAMS_Knee.osim"
			//"Models/Catelli-V4.0_TKR_ligament_SL_edit.osim"
			//"Models/Catelli-V4.0_TKR_ligament_SL_fixedKnee_edit.osim"
			//"Models/Catelli-V4.0_TKR_ligament_SL_freeKnee_edit.osim"

			"Models/K1L_scaled-sl_1991_knee3_edit_neutral.osim",
		};

		// Joint Parameters
		variableValues = {
			//{
			//	{0, 120, 0, 0}, // hip flex
			//	{0, 120, 0, 0}, // knee
			//	{0, 30, 0, 0}, // ankle
			//	{-30, 0, 0, 0}, // lumbar
			//	{-5, 25}, // subtalar
			//	{-40, 10} // hip rot
			//}
			//{
			//	{0, 82, 0, 0}, // hip
			//	{0, 92, 0, 0}, // knee
			//	{0, 25.5, 0, 0}, // ankle
			//	{-31.6, 0, 0, 0} // lumbar
			//}
			{ // for K1L
				{0, 125, 18.09771859, 3.11420397}, // hip
				{0, 125, 19.96429145, 11.57690248}, // knee
				{-5, 25, -2.26584073, -2.93278605}, // ankle
				{-30, -26, -26, -26} // lumbar
			 }
		};

	}
	else { std::cout << "Please specify the model" << endl; return 1; }

	// ================
	// Run Optimisation
	// ================
	// Set Model
	int align = 0;
	int subject = subjects - 1;
	Model model = editModel(modelFiles[subject], whichModel, component[align], plane, misalignment);
	State state = model.initSystem();
	model.realizeAcceleration(state);

	// Solution File Name
	string modelFullName = model.getName(); size_t endIndex = modelFullName.find('_');
	string modelName = (endIndex != string::npos) ? modelFullName.substr(0, endIndex) : modelFullName;
	string outputName; string angle; string fem_angle; string tib_angle; string comp_name;
	std::string accStr = formatNumberDec(accWeight, 4, 3); std::string balStr = formatNumber(balWeight, 2);
	std::string comStr = formatNumber(comWeight, 2);       std::string ligStr = formatNumberDec(ligWeight, 3, 1);
	Vec3 COM = model.calcMassCenterPosition(state);
	std::cout << "height: " << fabs(COM[1]) << endl;
	const JointSet& jset = model.getJointSet();
	string whichSide = "_l";
	if (modelName.back() == 'L') {
		whichSide = "_l";
	}
	else if (modelName.back() == 'R') {
		whichSide = "_r";
	}
	else {
		whichSide = "_unknown";
	}
	std::cout << "/jointset/hip" + whichSide + "/hip_flexion" + whichSide + "/value" << endl;

	if (whichModel == "exp") {
		// Exp result name
		outputName = "Results/CAMS_Knee/" + modelName + "_a" + accStr + "b" + balStr + "c" + comStr + "l" + ligStr + ".sto";
		std::cout << "" << endl; std::cout << "Solving: " + outputName << endl; std::cout << "" << endl;
	}
	else if (whichModel == "ref") {
		// Ref result name
		const auto& femur_weld_r = jset.get("femur_weld_r");
		const auto& tibial_plat_weld_r = jset.get("tibial_plat_weld_r");
		const auto& fem_offset = femur_weld_r.get_frames(1);
		const auto& tib_offset = tibial_plat_weld_r.get_frames(1);
		const auto& fem_rot = fem_offset.get_orientation();
		const auto& tib_rot = tib_offset.get_orientation();
		double fem = 0; double tib = 0;

		if (plane == "vv") {
			fem = fem_rot[1] * 180 / Pi;
			tib = tib_rot[0] * 180 / Pi;
		}
		else if (plane == "ir") {
			fem = fem_rot[2] * 180 / Pi;
			tib = tib_rot[1] * 180 / Pi;
		}
		else if (plane == "sg") {
			fem = fem_rot[0] * 180 / Pi;
			tib = tib_rot[2] * 180 / Pi;
		}
		else { std::cout << "Please specify the plane" << endl; return 1; }

		string fem_str = to_string(static_cast<int>(round(abs(fem))));
		string tib_str = to_string(static_cast<int>(round(abs(tib))));

		fem_str = string(2 - fem_str.length(), '0') + fem_str;
		tib_str = string(2 - tib_str.length(), '0') + tib_str;

		if (fem < 0) { fem_str = '-' + fem_str; }
		else { fem_str = fem_str; }
		if (tib < 0) { tib_str = '-' + tib_str; }
		else { tib_str = tib_str; }

		if (component[align] == "femoral") {
			angle = fem_str;
			comp_name = "fem";
		}
		else if (component[align] == "tibial") {
			angle = tib_str;
			comp_name = "tib";
		}
		else if (component[align] == "combined") {
			fem_angle = fem_str;
			tib_angle = tib_str;
			comp_name = "comb";
		}
		else { std::cout << "Please use correct component orientation type" << endl; return 1; }

		if (angle == "00") {
			outputName = "Results/Reference/neutral.sto";
		}
		else {
			if (component[align] == "combined") {
				outputName = "Results/Reference/" + comp_name + "_" + plane + "_F" + fem_angle + "_T" + tib_angle + ".sto";
			}
			else {
				outputName = "Results/Reference/" + comp_name + "_" + plane + "_" + angle + ".sto";
			}
		}
		std::cout << "" << endl; std::cout << "Solving: " + outputName << endl; std::cout << "" << endl;
	}
	else { std::cout << "Please specify the model type" << endl; }

	// Set Up Moco
	MocoStudy study;
	MocoProblem& problem = study.updProblem();
	problem.setModelAsCopy(model);
	problem.setTimeBounds(0, finTime);

	if (fs::exists(outputName)) { std::cout << "Output File Exists!" << endl; }
	else {
		// Set boundary conditions
		//problem.setStateInfo("/jointset/ground_pelvis/pelvis_tilt/value", MocoBounds(variableValues[subject][4][0] * Pi / 180, variableValues[subject][4][1] * Pi / 180),
		//	MocoInitialBounds(variableValues[subject][4][2] * Pi / 180), MocoFinalBounds(variableValues[subject][4][3] * Pi / 180));
		problem.setStateInfo("/jointset/hip" + whichSide + "/hip_flexion" + whichSide + "/value",
			MocoBounds(variableValues[subject][0][0] * Pi / 180, variableValues[subject][0][1] * Pi / 180),
			MocoInitialBounds(variableValues[subject][0][2] * Pi / 180), MocoFinalBounds(variableValues[subject][0][3] * Pi / 180));
		problem.setStateInfo("/jointset/knee" + whichSide + "/knee_flexion" + whichSide + "/value",
			MocoBounds(variableValues[subject][1][0] * Pi / 180, variableValues[subject][1][1] * Pi / 180),
			MocoInitialBounds(variableValues[subject][1][2] * Pi / 180), MocoFinalBounds(variableValues[subject][1][3] * Pi / 180));
		problem.setStateInfo("/jointset/ankle" + whichSide + "/ankle_angle" + whichSide + "/value",
			MocoBounds(variableValues[subject][2][0] * Pi / 180, variableValues[subject][2][1] * Pi / 180),
			MocoInitialBounds(variableValues[subject][2][2] * Pi / 180), MocoFinalBounds(variableValues[subject][2][3] * Pi / 180));
		problem.setStateInfo("/jointset/back/lumbar_extension/value",
			MocoBounds(variableValues[subject][3][0] * Pi / 180, variableValues[subject][3][1] * Pi / 180),
			MocoInitialBounds(variableValues[subject][3][2] * Pi / 180), MocoFinalBounds(variableValues[subject][3][3] * Pi / 180));
		//problem.setStateInfo("/jointset/hip_l/hip_lotation_l/value", MocoBounds(variableValues[subject][5][0] * Pi / 180, variableValues[subject][5][1] * Pi / 180));
		//problem.setStateInfo("/jointset/subtalar_l/subtalar_angle_l/value", MocoBounds(-20 * Pi / 180, 10 * Pi / 180));
		//problem.setStateInfo("/jointset/knee" + whichSide + "/knee_adduction" + whichSide + "/value", MocoBounds(-10 * Pi / 180, 10 * Pi / 180),
		//	MocoInitialBounds(0), MocoFinalBounds(0));
		//problem.setStateInfo("/jointset/knee" + whichSide + "/knee_rotation" + whichSide + "/value", MocoBounds(-10 * Pi / 180, 10 * Pi / 180),
		//	MocoInitialBounds(0), MocoFinalBounds(0));

		problem.setStateInfoPattern("/jointset/.*/speed", {}, MocoInitialBounds(0), MocoFinalBounds(0));

		// Set goals
		//MocoAccelGoal accelGoal("acceleration");
		//Vector weights(12);
		//for (int i = 0; i < 12; ++i) {
		//	weights[i] = 0.002;
		//}
		//accelGoal.setAccelerationWeights(weights);
		//problem.addGoal<MocoAccelGoal>(accelGoal);
		//problem.addGoal<MocoHeightVelGoal>("heightvel", comvelWeight);
		//auto* jrGoal = problem.addGoal<MocoJointReactionGoal>("jointreaction", ligWeight);
		//jrGoal->setJointPath("/jointset/knee" + whichSide);
		//jrGoal->setLoadsFrame("child");
		//jrGoal->setExpressedInFramePath("/bodyset/tibia" + whichSide);
		//jrGoal->setReactionMeasures({ "force-y", "moment-z"});
		problem.addGoal<MocoHeightGoal>("height", comWeight);
		if (modelName.back() == 'L') {
			problem.addGoal<MocoBalanceGoal>("balance", balWeight);
		}	else if (modelName.back() == 'R') {
			problem.addGoal<MocoBalanceGoalRight>("balance", balWeight);
		}
		problem.addGoal<MocoLigamentGoal>("ligament", ligWeight);
		auto* effortGoal = problem.addGoal<MocoControlGoal>("effort", 1);
		effortGoal->setExponent(2);
		const ForceSet& fset = model.getForceSet();
		for (int i = 9; i < fset.getSize(); i++) {
			effortGoal->setWeightForControl("/forceset/" + fset[i].getName(), effWeights[i-9]);
		}

		// Configure the solver
		MocoCasADiSolver& solver = study.initCasADiSolver();
		solver.set_multibody_dynamics_mode("implicit");
		solver.set_minimize_implicit_multibody_accelerations(true);
		solver.set_implicit_multibody_accelerations_weight(accWeight);
		//solver.set_minimize_implicit_auxiliary_derivatives(true);
		//solver.set_implicit_auxiliary_derivatives_weight(0.001);
		solver.set_num_mesh_intervals(meshVal);
		solver.set_verbosity(2);
		solver.set_optim_solver("ipopt");
		//solver.set_optim_ipopt_print_level(4);
		solver.set_optim_max_iterations(numIter);
		solver.set_optim_convergence_tolerance(convTol);
		solver.set_optim_constraint_tolerance(constTol);

		MocoTrajectory guess = solver.createGuess("bounds");
		guess.resampleWithNumTimes(2);
		guess.setState("/jointset/ground_pelvis/pelvis_ty/value", { 0.927, 0.949 });
		guess.setState("/jointset/hip" + whichSide + "/hip_flexion" + whichSide + "/value",
			{ variableValues[subject][0][2] * Pi / 180, variableValues[subject][0][3] * Pi / 180 });
		guess.setState("/jointset/knee" + whichSide + "/knee_flexion" + whichSide + "/value",
			{ variableValues[subject][1][2] * Pi / 180, variableValues[subject][1][3] * Pi / 180 });
		guess.setState("/jointset/ankle" + whichSide + "/ankle_angle" + whichSide + "/value",
			{ variableValues[subject][2][2] * Pi / 180, variableValues[subject][2][3] * Pi / 180 });
		guess.setState("/jointset/back/lumbar_extension/value",
			{ variableValues[subject][3][2] * Pi / 180, variableValues[subject][3][3] * Pi / 180 });
		solver.setGuess(guess);
		study.print("SquatOpt_" + whichSide + ".moco");

		// Insert previous solution from a less complex problem
		//MocoTrajectory guess = solver.createGuess();
		//auto prevSolution = MocoTrajectory("Results/CAMS_Knee/" + modelName + "_solution.sto");
		//auto prevStatesTable = prevSolution.exportToStatesTable();
		//auto prevControlsTable = prevSolution.exportToControlsTable();
		//guess.insertStatesTrajectory(prevStatesTable, true);
		//guess.insertControlsTrajectory(prevControlsTable, true);
		//solver.setGuess(guess);

		// Solve the problem
		MocoSolution solution = study.solve();
		solution.unseal();
		//auto predicted_motion = solution.getStatesTrajectory();

		// Save the solution
		solution.write("Results/" + modelName + "sol.mot");

		std::cout << "Solution status: " << solution.getStatus() << endl;
	}



	// ==================================================================================
	// Print Out Peak Flexion Angle Differences Between the Experiment and the Prediction
	// ==================================================================================

		//if (whichModel == "exp") {
		//	// Dataset
		//	string directoryPath = "Results/CAMS_Knee"; // Replace with your directory path
		//	vector<string> datasetFiles;
		//
		//	WIN32_FIND_DATA findFileData;
		//	HANDLE hFind = FindFirstFile((directoryPath + "/*").c_str(), &findFileData);
		//
		//	if (hFind != INVALID_HANDLE_VALUE) {
		//		do {
		//			if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
		//				datasetFiles.push_back(directoryPath + "/" + findFileData.cFileName);
		//			}
		//		} while (FindNextFile(hFind, &findFileData) != 0);
		//		FindClose(hFind);
		//	}
		//	else {
		//		cerr << "Error opening directory" << endl;
		//		return 1;
		//	}
		//
		//	// Print the contents of the vector
		//	for (const string& fileName : datasetFiles) {
		//		std::cout << fileName << endl;
		//	}
		//
		//	// Get Peak Flexion Angles for Each Joint
		//	vector<vector<double>> resultArray(datasetFiles.size(), vector<double>(3, 0.0));
		//	vector<vector<double>> diffArray(datasetFiles.size(), vector<double>(3, 0.0));
		//
		//	// Peak Flexion Angles - Experimental
		//	double expArray[][3] = {
		//	{81.69399072, 70.65350934, 9.38602396},
		//	{107.0939643, 92.95941183, 16.05635248},
		//	{77.2417225, 71.67502451, 16.04911708},
		//	{81.80970554, 92.36328771, 25.53585152},
		//	{104.3089424, 80.57350014, 16.23760788},
		//	{117.4530983, 87.46181932, 12.22363194}
		//	};
		//
		//	for (size_t i = 0; i < datasetFiles.size(); ++i) {
		//		const auto& datasetFile = datasetFiles[subject];
		//		string datasetName = extractDatasetName(datasetFile);
		//
		//		// Open the dataset file
		//		ifstream inputFile(datasetFile);
		//
		//		if (!inputFile.is_open()) {
		//			std::cout << "Failed to open the file." << endl;
		//			return 1;
		//		}
		//
		//		string line;
		//		bool foundHeader = false;
		//		int hipFlexionColumn = -1, kneeFlexionColumn = -1, ankleFlexionColumn = -1;
		//		double maxHipFlexion = 0.0, maxKneeFlexion = 0.0, maxAnkleFlexion = 0.0;
		//
		//		// Read the file line by line
		//		while (getline(inputFile, line)) {
		//			if (!foundHeader) {
		//				// Find the header row containing the desired columns
		//				if (line.find("time") != string::npos) {
		//					vector<string> headers = splitString(line, '\t');
		//
		//					// Find the column indices of the desired variables
		//					for (size_t i = 0; i < headers.size(); ++i) {
		//						if (headers[subject] == "/jointset/hip_r/hip_flexion_r/value") {
		//							hipFlexionColumn = static_cast<int>(i);
		//						}
		//						else if (headers[subject] == "/jointset/knee_r/knee_flexion_r/value") {
		//							kneeFlexionColumn = static_cast<int>(i);
		//						}
		//						else if (headers[subject] == "/jointset/ankle_r/ankle_angle_r/value") {
		//							ankleFlexionColumn = static_cast<int>(i);
		//						}
		//					}
		//
		//					foundHeader = true;
		//				}
		//			}
		//			else {
		//				// Find the maximum values in the desired columns
		//				vector<string> data = splitString(line, '\t');
		//
		//				if (data.size() > max({ hipFlexionColumn, kneeFlexionColumn, ankleFlexionColumn })) {
		//					double hipFlexionValue = stod(data[hipFlexionColumn]);
		//					double kneeFlexionValue = stod(data[kneeFlexionColumn]);
		//					double ankleFlexionValue = stod(data[ankleFlexionColumn]);
		//
		//					double hipFlexionDegrees = (hipFlexionValue * 180.0 / Pi);
		//					double kneeFlexionDegrees = (kneeFlexionValue * 180.0 / Pi);
		//					double ankleFlexionDegrees = (ankleFlexionValue * 180.0 / Pi);
		//
		//					maxHipFlexion = max(maxHipFlexion, hipFlexionDegrees);
		//					maxKneeFlexion = max(maxKneeFlexion, kneeFlexionDegrees);
		//					maxAnkleFlexion = max(maxAnkleFlexion, ankleFlexionDegrees);
		//				}
		//			}
		//		}
		//
		//		// Store the maximum values in the result array
		//		resultArray[subject] = { maxHipFlexion, maxKneeFlexion, maxAnkleFlexion };
		//
		//		// Calculate the difference
		//		for (int l = 0; l < 3; l++) {
		//			diffArray[subject][l] = resultArray[subject][l] - expArray[subject][l];
		//		}
		//
		//		// Close the file
		//		inputFile.close();
		//	}
		//
		//	std::cout << "" << endl;
		//	std::cout << "" << endl;
		//
		//	std::cout << "Experimental Maximum Flexion Angles" << endl;
		//	std::cout << setw(10) << " " << setw(10) << "hip" << setw(10) << "knee" << setw(10) << "ankle" << endl;
		//
		//	for (size_t i = 0; i < sizeof(expArray) / sizeof(expArray[0]); ++i) {
		//		string datasetName = extractDatasetName(datasetFiles[subject]);
		//		size_t lastSlashPos = datasetName.find_last_of("/");
		//		if (lastSlashPos != string::npos && lastSlashPos + 1 < datasetName.length()) {
		//			// Extract and print the filename without the extension
		//			string fileName = datasetName.substr(lastSlashPos + 1);
		//			size_t extensionPos = fileName.rfind(".");
		//			if (extensionPos != string::npos) {
		//				fileName = fileName.substr(0, extensionPos);
		//			}
		//			std::cout << left << setw(10) << fileName;
		//		}
		//		for (size_t l = 0; l < sizeof(expArray[0]) / sizeof(expArray[0][0]); ++l) {
		//			std::cout << setw(10) << expArray[subject][l];
		//		}
		//		std::cout << endl;
		//	}
		//
		//	std::cout << "" << endl;
		//	std::cout << "" << endl;
		//
		//	std::cout << "Predicted Maximum Flexion Angles" << endl;
		//	std::cout << setw(10) << " " << setw(10) << "hip" << setw(10) << "knee" << setw(10) << "ankle" << endl;
		//	for (size_t i = 0; i < resultArray.size(); ++i) {
		//		string datasetName = extractDatasetName(datasetFiles[subject]);
		//		size_t lastSlashPos = datasetName.find_last_of("/");
		//		if (lastSlashPos != string::npos && lastSlashPos + 1 < datasetName.length()) {
		//			// Extract and print the filename without the extension
		//			string fileName = datasetName.substr(lastSlashPos + 1);
		//			size_t extensionPos = fileName.rfind(".");
		//			if (extensionPos != string::npos) {
		//				fileName = fileName.substr(0, extensionPos);
		//			}
		//			std::cout << left << setw(10) << fileName;
		//		}
		//		for (size_t l = 0; l < resultArray[subject].size(); ++l) {
		//			std::cout << setw(10) << resultArray[subject][l];
		//		}
		//		std::cout << endl;
		//	}
		//
		//	std::cout << "" << endl;
		//	std::cout << "" << endl;
		//
		//	std::cout << "Maximum Flexion Angle Differences: EXP VS PRED" << endl;
		//	std::cout << setw(10) << " " << setw(10) << "hip" << setw(10) << "knee" << setw(10) << "ankle" << endl;
		//	for (size_t i = 0; i < diffArray.size(); ++i) {
		//		string datasetName = extractDatasetName(datasetFiles[subject]);
		//		size_t lastSlashPos = datasetName.find_last_of("/");
		//		if (lastSlashPos != string::npos && lastSlashPos + 1 < datasetName.length()) {
		//			// Extract and print the filename without the extension
		//			string fileName = datasetName.substr(lastSlashPos + 1);
		//			size_t extensionPos = fileName.rfind(".");
		//			if (extensionPos != string::npos) {
		//				fileName = fileName.substr(0, extensionPos);
		//			}
		//			std::cout << left << setw(10) << fileName;
		//		}
		//		for (size_t l = 0; l < diffArray[subject].size(); ++l) {
		//			std::cout << setw(10) << diffArray[subject][l];
		//		}
		//		std::cout << endl;
		//	}
		//}
		//else if (whichModel == "ref") {
		//	// Dataset
		//	string directoryPath = "Results/Reference/"; // Replace with your directory path
		//	vector<string> datasetFiles;
		//
		//	WIN32_FIND_DATA findFileData;
		//	HANDLE hFind = FindFirstFile((directoryPath + "/*").c_str(), &findFileData);
		//
		//	if (hFind != INVALID_HANDLE_VALUE) {
		//		do {
		//			if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
		//				datasetFiles.push_back(directoryPath + "/" + findFileData.cFileName);
		//			}
		//		} while (FindNextFile(hFind, &findFileData) != 0);
		//		FindClose(hFind);
		//	}
		//	else {
		//		cerr << "Error opening directory" << endl;
		//		return 1;
		//	}
		//
		//	// Print the contents of the vector
		//	for (const string& fileName : datasetFiles) {
		//		std::cout << fileName << endl;
		//	}
		//
		//	// Get Peak Flexion Angles for Each Joint
		//	vector<vector<double>> resultArray(datasetFiles.size(), vector<double>(3, 0.0));
		//
		//	for (size_t i = 0; i < datasetFiles.size(); ++i) {
		//		const auto& datasetFile = datasetFiles[subject];
		//		string datasetName = extractDatasetName(datasetFile);
		//
		//		// Open the dataset file
		//		ifstream inputFile(datasetFile);
		//
		//		if (!inputFile.is_open()) {
		//			std::cout << "Failed to open the file." << endl;
		//			return 1;
		//		}
		//
		//		string line;
		//		bool foundHeader = false;
		//		int hipFlexionColumn = -1, kneeFlexionColumn = -1, ankleFlexionColumn = -1;
		//		double maxHipFlexion = 0.0, maxKneeFlexion = 0.0, maxAnkleFlexion = 0.0;
		//
		//		// Read the file line by line
		//		while (getline(inputFile, line)) {
		//			if (!foundHeader) {
		//				// Find the header row containing the desired columns
		//				if (line.find("time") != string::npos) {
		//					vector<string> headers = splitString(line, '\t');
		//
		//					// Find the column indices of the desired variables
		//					for (size_t i = 0; i < headers.size(); ++i) {
		//						if (headers[subject] == "/jointset/hip_r/hip_flexion_r/value") {
		//							hipFlexionColumn = static_cast<int>(i);
		//						}
		//						else if (headers[subject] == "/jointset/knee_r/knee_flexion_r/value") {
		//							kneeFlexionColumn = static_cast<int>(i);
		//						}
		//						else if (headers[subject] == "/jointset/ankle_r/ankle_angle_r/value") {
		//							ankleFlexionColumn = static_cast<int>(i);
		//						}
		//					}
		//
		//					foundHeader = true;
		//				}
		//			}
		//			else {
		//				// Find the maximum values in the desired columns
		//				vector<string> data = splitString(line, '\t');
		//
		//				if (data.size() > max({ hipFlexionColumn, kneeFlexionColumn, ankleFlexionColumn })) {
		//					double hipFlexionValue = stod(data[hipFlexionColumn]);
		//					double kneeFlexionValue = stod(data[kneeFlexionColumn]);
		//					double ankleFlexionValue = stod(data[ankleFlexionColumn]);
		//
		//					double hipFlexionDegrees = (hipFlexionValue * 180.0 / Pi);
		//					double kneeFlexionDegrees = (kneeFlexionValue * 180.0 / Pi);
		//					double ankleFlexionDegrees = (ankleFlexionValue * 180.0 / Pi);
		//
		//					maxHipFlexion = max(maxHipFlexion, hipFlexionDegrees);
		//					maxKneeFlexion = max(maxKneeFlexion, kneeFlexionDegrees);
		//					maxAnkleFlexion = max(maxAnkleFlexion, ankleFlexionDegrees);
		//				}
		//			}
		//		}
		//
		//		// Store the maximum values in the result array
		//		resultArray[subject] = { maxHipFlexion, maxKneeFlexion, maxAnkleFlexion };
		//
		//		// Close the file
		//		inputFile.close();
		//	}
		//
		//	std::cout << "" << endl;
		//	std::cout << "" << endl;
		//
		//	std::cout << "Predicted Maximum Flexion Angles" << endl;
		//	std::cout << std::setw(15) << "hip" << std::setw(10) << "knee" << std::setw(10) << "ankle" << std::endl;
		//	for (size_t i = 0; i < resultArray.size(); ++i) {
		//		string datasetName = extractDatasetName(datasetFiles[subject]);
		//		size_t lastSlashPos = datasetName.find_last_of("/");
		//		if (lastSlashPos != string::npos && lastSlashPos + 1 < datasetName.length()) {
		//			// Extract and print the filename without the extension
		//			string fileName = datasetName.substr(lastSlashPos + 1);
		//			size_t extensionPos = fileName.rfind(".");
		//			if (extensionPos != string::npos) {
		//				fileName = fileName.substr(0, extensionPos);
		//			}
		//			std::cout << left << setw(10) << fileName << setw(20);
		//		}
		//		for (size_t l = 0; l < resultArray[subject].size(); ++l) {
		//			std::cout << setw(10) << resultArray[subject][l];
		//		}
		//		std::cout << endl;
		//	}
		//
		//	std::cout << "" << endl;
		//	std::cout << "" << endl;
		//
		//}
		//else { std::cout << "Use a correct model" << endl; return 1; }


	return EXIT_SUCCESS;

}