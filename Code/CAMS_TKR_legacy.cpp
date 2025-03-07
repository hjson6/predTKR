#include <OpenSim/Actuators/TorqueActuator.h>
#include <OpenSim/Common/STOFileAdapter.h>
#include <OpenSim/Moco/osimMoco.h>
#include "MocoBalanceGoal.h"
#include "MocoLigamentGoal.h"

using namespace OpenSim;
using namespace SimTK;
using namespace std;
namespace fs = std::filesystem;

// Function to split a string into tokens
vector<string> splitString(const string& str, char delimiter) {
	vector<string> tokens;
	istringstream iss(str);
	string token;
	while (getline(iss, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
}

// Function to extract the dataset name from the file name
string extractDatasetName(const string& filename) {
	string datasetName = filename;
	size_t extensionPos = datasetName.find("_prediction.sto");
	if (extensionPos != string::npos) {
		datasetName = datasetName.substr(0, extensionPos);
	}

	return datasetName;
}

// Create Model
//Model editModel(const string& modelFileName, vector<double> ligLengths) {
Model editModel(const string & modelFileName) {
	Model model(modelFileName);

	//auto state = model.initSystem();

	//const Set<Coordinate>& coord = model.getCoordinateSet();
	////std::cout << coord[3] << endl;
	//auto* F_vertical = new CoordinateActuator();
	//F_vertical->setName("femur_ty");
	//F_vertical->setCoordinate(&coord[3]);
	//F_vertical->setOptimalForce(750);
	//F_vertical->setMinControl(-1);
	//F_vertical->setMaxControl(0);
	//model.addForce(F_vertical);

	auto* T_hip_actu_r = new TorqueActuator();
	T_hip_actu_r->setName("hip_actu_r");
	T_hip_actu_r->setBodyA(model.getBodySet().get("pelvis"));
	T_hip_actu_r->setBodyB(model.getBodySet().get("femur_r"));
	T_hip_actu_r->setOptimalForce(100);
	T_hip_actu_r->setMinControl(-1);
	T_hip_actu_r->setMaxControl(1);
	T_hip_actu_r->setTorqueIsGlobal(true);
	model.addForce(T_hip_actu_r);
	//
	auto* T_knee_actu_r = new TorqueActuator();
	T_knee_actu_r->setName("knee_actu_r");
	T_knee_actu_r->setBodyA(model.getBodySet().get("femur_r"));
	T_knee_actu_r->setBodyB(model.getBodySet().get("tibia_r"));
	T_knee_actu_r->setOptimalForce(200);
	T_knee_actu_r->setMinControl(-1);
	T_knee_actu_r->setMaxControl(1);
	T_knee_actu_r->setTorqueIsGlobal(true);
	model.addForce(T_knee_actu_r);
	//
	auto* T_ankle_actu_r = new TorqueActuator();
	T_ankle_actu_r->setName("ankle_actu_r");
	T_ankle_actu_r->setBodyA(model.getBodySet().get("tibia_r"));
	T_ankle_actu_r->setBodyB(model.getBodySet().get("calcn_r"));
	T_ankle_actu_r->setOptimalForce(100);
	T_ankle_actu_r->setMinControl(-1);
	T_ankle_actu_r->setMaxControl(1);
	T_ankle_actu_r->setTorqueIsGlobal(true);
	model.addForce(T_ankle_actu_r);
	//
	auto* T_lumbar_actu = new TorqueActuator();
	T_lumbar_actu->setName("lumbar_actu");
	T_lumbar_actu->setBodyA(model.getBodySet().get("torso"));
	T_lumbar_actu->setBodyB(model.getBodySet().get("pelvis"));
	T_lumbar_actu->setOptimalForce(100);
	T_lumbar_actu->setMinControl(-1);
	T_lumbar_actu->setMaxControl(1);
	T_lumbar_actu->setTorqueIsGlobal(true);
	model.addForce(T_lumbar_actu);


  return model;
}

// Custom Cost Functions
//class MocoBalanceGoal : public MocoGoal {
//	OpenSim_DECLARE_CONCRETE_OBJECT(MocoBalanceGoal, MocoGoal);
//
//public:
//	MocoBalanceGoal() {}
//	MocoBalanceGoal(string name) : MocoGoal(move(name)) {}
//	MocoBalanceGoal(string name, double weight) : MocoGoal(move(name), weight) {}
//
//protected:
//	Mode getDefaultModeImpl() const override { return Mode::Cost; }
//	bool getSupportsEndpointConstraintImpl() const override { return true; }
//	void initializeOnModelImpl(const Model&) const override {
//		setRequirements(1, 1);
//	}
//	void calcIntegrandImpl(
//		const IntegrandInput& input, double& integrand) const override {
//		getModel().realizeAcceleration(input.state);
//
//		//COM
//		const auto& COM = getModel().calcMassCenterPosition(input.state);
//		
//		//Foot
//		const auto& torso_mc = getModel().getBodySet().get("torso").getMassCenter();
//		const auto& torso_pos = getModel().getBodySet().get("torso").findStationLocationInGround(input.state, torso_mc);
//		const auto& calcn_r_mc = getModel().getBodySet().get("calcn_r").getMassCenter();
//		const auto& calcn_r_pos = getModel().getBodySet().get("calcn_r").findStationLocationInGround(input.state, calcn_r_mc);
//		//const auto& calcn_l_mc = getModel().getBodySet().get("calcn_l").getMassCenter();
//		//const auto& calcn_l_pos = getModel().getBodySet().get("calcn_l").findStationLocationInGround(input.state, calcn_l_mc);
//
//		//const auto& x_diff = COM[0] - calcn_r_pos[0];
//		//const auto& z_diff = COM[2] - (calcn_r_pos[2] + calcn_l_pos[2]);
//
//		integrand = (COM[0] - calcn_r_pos[0]) * (COM[0] - calcn_r_pos[0]);
//	}
//
//	void calcGoalImpl(
//		const GoalInput& input, SimTK::Vector& cost) const override {
//		cost[0] = input.integral;
//	}
//};

//class MocoLigamentGoal : public MocoGoal {
//	OpenSim_DECLARE_CONCRETE_OBJECT(MocoLigamentGoal, MocoGoal);
//
//public:
//	MocoLigamentGoal() {}
//	MocoLigamentGoal(string name) : MocoGoal(move(name)) {}
//	MocoLigamentGoal(string name, double weight) : MocoGoal(move(name), weight) {}
//
//protected:
//	Mode getDefaultModeImpl() const override { return Mode::Cost; }
//	bool getSupportsEndpointConstraintImpl() const override { return true; }
//	void initializeOnModelImpl(const Model&) const override {
//		setRequirements(1, 1);
//	}
//	void calcIntegrandImpl(
//		const IntegrandInput& input, double& integrand) const override {
//		getModel().realizeAcceleration(input.state);
//
//		const auto& fset = getModel().getForceSet();
//
//		Ligament* cPT = dynamic_cast<Ligament*>(&fset.get(0));
//		Ligament* aLCL = dynamic_cast<Ligament*>(&fset.get(1));
//		Ligament* mLCL = dynamic_cast<Ligament*>(&fset.get(2));
//		Ligament* pLCL = dynamic_cast<Ligament*>(&fset.get(3));
//		Ligament* aMCL = dynamic_cast<Ligament*>(&fset.get(4));
//		Ligament* mMCL = dynamic_cast<Ligament*>(&fset.get(5));
//		Ligament* pMCL = dynamic_cast<Ligament*>(&fset.get(6));
//		Ligament* sMPFL = dynamic_cast<Ligament*>(&fset.get(7));
//		Ligament* mMPFL = dynamic_cast<Ligament*>(&fset.get(8));
//		Ligament* iMPFL = dynamic_cast<Ligament*>(&fset.get(9));
//		Ligament* sLPFL = dynamic_cast<Ligament*>(&fset.get(10));
//		Ligament* mLPFL = dynamic_cast<Ligament*>(&fset.get(11));
//		Ligament* iLPFL = dynamic_cast<Ligament*>(&fset.get(12));
//		
//		const auto& cPT_tension = cPT->getTension(input.state);
//		const auto& aLCL_tension = aLCL->getTension(input.state);
//		const auto& mLCL_tension = mLCL->getTension(input.state);
//		const auto& pLCL_tension = pLCL->getTension(input.state);
//		const auto& aMCL_tension = aMCL->getTension(input.state);
//		const auto& mMCL_tension = mMCL->getTension(input.state);
//		const auto& pMCL_tension = pMCL->getTension(input.state);
//		const auto& sMPFL_tension = sMPFL->getTension(input.state);
//		const auto& mMPFL_tension = mMPFL->getTension(input.state);
//		const auto& iMPFL_tension = iMPFL->getTension(input.state);
//		const auto& sLPFL_tension = sLPFL->getTension(input.state);
//		const auto& mLPFL_tension = mLPFL->getTension(input.state);
//		const auto& iLPFL_tension = iLPFL->getTension(input.state);
//
//		const auto& cPT_pcsa = cPT->get_pcsa_force();
//		const auto& aLCL_pcsa = aLCL->get_pcsa_force();
//		const auto& mLCL_pcsa = mLCL->get_pcsa_force();
//		const auto& pLCL_pcsa = pLCL->get_pcsa_force();
//		const auto& aMCL_pcsa = aMCL->get_pcsa_force();
//		const auto& mMCL_pcsa = mMCL->get_pcsa_force();
//		const auto& pMCL_pcsa = pMCL->get_pcsa_force();
//		const auto& sMPFL_pcsa = sMPFL->get_pcsa_force();
//		const auto& mMPFL_pcsa = mMPFL->get_pcsa_force();
//		const auto& iMPFL_pcsa = iMPFL->get_pcsa_force();
//		const auto& sLPFL_pcsa = sLPFL->get_pcsa_force();
//		const auto& mLPFL_pcsa = mLPFL->get_pcsa_force();
//		const auto& iLPFL_pcsa = iLPFL->get_pcsa_force();
//
//		const auto& cPT_norm = cPT_tension / cPT_pcsa;
//		const auto& aLCL_norm = aLCL_tension / aLCL_pcsa;
//		const auto& mLCL_norm = mLCL_tension / mLCL_pcsa;
//		const auto& pLCL_norm = pLCL_tension / pLCL_pcsa;
//		const auto& aMCL_norm = aMCL_tension / aMCL_pcsa;
//		const auto& mMCL_norm = mMCL_tension / mMCL_pcsa;
//		const auto& pMCL_norm = pMCL_tension / pMCL_pcsa;
//		const auto& sMPFL_norm = sMPFL_tension / sMPFL_pcsa;
//		const auto& mMPFL_norm = mMPFL_tension / mMPFL_pcsa;
//		const auto& iMPFL_norm = iMPFL_tension / iMPFL_pcsa;
//		const auto& sLPFL_norm = sLPFL_tension / sLPFL_pcsa;
//		const auto& mLPFL_norm = mLPFL_tension / mLPFL_pcsa;
//		const auto& iLPFL_norm = iLPFL_tension / iLPFL_pcsa;
//
//		//SimTK::Mat<1, 17> Tensions(cPT_norm, mPT_norm, lPT_norm,
//		//	aPCL_norm, pPCL_norm, aLCL_norm, mLCL_norm, aMCL_norm,
//		//	mMCL_norm, pMCL_norm, sMPFL_norm, mMPFL_norm, iMPFL_norm,
//		//	sLPFL_norm, mLPFL_norm, iLPFL_norm);
//
//		//SimTK::Mat<1, 15> Tensions(cPT_norm, aPCL_norm, pPCL_norm, aLCL_norm,
//		//	mLCL_norm, pLCL_norm, aMCL_norm, mMCL_norm, pMCL_norm, sMPFL_norm,
//		//	mMPFL_norm, iMPFL_norm, sLPFL_norm, mLPFL_norm, iLPFL_norm);
//
//		//SimTK::Mat<1, 8> Tensions(aPCL_norm, pPCL_norm, aLCL_norm, 
//		//	mLCL_norm, pLCL_norm, aMCL_norm, mMCL_norm, pMCL_norm);
//
//		//SimTK::Mat<1, 6> Tensions(aLCL_norm, mLCL_norm, pLCL_norm, aMCL_norm, mMCL_norm, pMCL_norm);
//
//		SimTK::Vec6 Tensions(aLCL_norm, mLCL_norm, pLCL_norm, aMCL_norm, mMCL_norm, pMCL_norm);
//
//		integrand = Tensions.normSqr();
//	}
//
//	void calcGoalImpl(
//		const GoalInput& input, SimTK::Vector& cost) const override {
//		cost[0] = input.integral;
//	}
//};

int main() {
	string whichModel = "ref"; // exp, ref
	string component = "tibial"; // femoral, tibial, combined *Don't use combined for neutral
	string plane = "vv"; // vv = varus-valgus; ir = axial rotation; sg = saggital plane;

	vector<string> modelFiles;
	vector<vector<vector<double>>> variableValues;
	vector<vector<double>> ligLengths;
	double scaleFactors[] = { 1, 1, 1, 1, 1, 1 };

	if (whichModel == "exp") {
		// Models
		modelFiles = {
			"Models/K1L_scaled-sl-c.osim",
			"Models/K2L_scaled-sl-c.osim",
			"Models/K3R_scaled-sl-c.osim",
			"Models/K5R_scaled-sl-c.osim",
			"Models/K7L_scaled-sl-c.osim",
			"Models/K8L_scaled-sl-c.osim"
		};

		// Joint Parameters
		variableValues = {   // {min, max, initial, final}
			// K1L
			{
				{0, 82, 18.09771859, 3.11420397}, // hip
				{0, 71, 19.96429145, 11.57690248}, // knee
				{-3, 9, -2.26584073, -2.93278605}, // ankle
				{-45.5, -25, -26, -26} // lumbar
			},
			// K2L
			{
				{0, 107, 21.84308377, 23.22191181}, // hip
				{0, 93, 9.46770703, 10.53393252}, // knee
				{-5, 16, -4.81725349, -4.83598057}, // ankle
				{-0.2, 12.6, 1.7, 3.7} // lumbar
			},
			// K3R
			{
				{0, 77, 14.19381299, 11.56933215}, // hip
				{0, 72, 16.51788149, 11.89534598}, // knee
				{0, 16, 0.95454653, 0.09508392}, // ankle
				{-25, -2, -5.9, -2.6} // lumbar
			},
			// K5R
			{
				{0, 82, 15.06096006, 8.5426932}, // hip
				{0, 92, 20.76184136, 20.01270929}, // knee
				{0, 25.5, 4.23535532, 4.3214813}, // ankle
				{-31.6, -8, -10.5, -8} // lumbar
			},
			// K7L
			{
				{0, 104, 11.60753618, 12.88409056}, // hip
				{0, 81, 23.11125186, 12.4287375}, // knee
				{0, 16, 8.22506838, 8.93151063}, // ankle
				{-14.1,	0, -10.7, -1.6} // lumbar
			},
			// K8L
			{
				{0, 117, 0.8795295, 1.34336163}, // hip
				{0, 87, 7.12023746, 6.03907694}, // knee
				{-2.5, 12, -2.22311738, -0.23139406}, // ankle
				{-14.2, 2.6, -14.2, -5.3} // lumbar
			},
		};
		//double variableValues[][4][4] = {   // {min, max, initial, final}
		//	// K1L
		//	{
		//		{0, 120, 18.09771859, 3.11420397}, // hip
		//		{0, 120, 19.96429145, 11.57690248}, // knee
		//		{-3, 10, -2.26584073, -2.93278605}, // ankle
		//		{-45.5, -25, -26, -26} // lumbar
		//	},
		//	// K2L
		//	{
		//		{0, 120, 21.4068384, 23.22191181}, // hip
		//		{0, 120, 10.20055563, 10.53393252}, // knee
		//		{-5, 20, -3.22199853, -4.83598057}, // ankle
		//		{-0.2, 12.6, 1.7, 3.7} // lumbar
		//	},
		//	// K3R
		//	{
		//		{0, 120, 14.19381299, 11.56933215}, // hip
		//		{0, 120, 16.51788149, 11.89534598}, // knee
		//		{0, 20, 0.95454653, 0.09508392}, // ankle
		//		{-25, -2, -5.9, -2.6} // lumbar
		//	},
		//	// K5R
		//	{
		//		{0, 120, 15.06096006, 8.5426932}, // hip
		//		{0, 120, 20.76184136, 20.01270929}, // knee
		//		{0, 25, 4.23535532, 4.3214813}, // ankle
		//		{-31.6, -8, -10.5, -8} // lumbar
		//	},
		//	// K7L
		//	{
		//		{0, 120, 11.60753618, 12.88409056}, // hip
		//		{0, 120, 23.11125186, 12.4287375}, // knee
		//		{0, 20, 8.22506838, 8.93151063}, // ankle
		//		{-14.1,	0, -10.7, -1.6} // lumbar
		//	},
		//	// K8L
		//	{
		//		{0, 120, 0.8795295, 1.34336163}, // hip
		//		{0, 120, 7.12023746, 6.03907694}, // knee
		//		{-2.5, 15, -2.22311738, -0.23139406}, // ankle
		//		{-14.2, 2.6, -14.2, -5.3} // lumbar
		//	},
		//};

		// Ligament Lengths
		ligLengths = {
			//K1L
			{0.0483214, 0.0257958, 0.0274251, 0.0616245, 0.0613266, 0.0542386, 0.0854019, 0.087297,
			0.0880398, 0.0451882, 0.0453497, 0.0459236, 0.0444411, 0.0435229, 0.0433932},

			//K2L
			{0.0488592, 0.027247, 0.0292385, 0.0623607, 0.0622071, 0.0553356, 0.0845591, 0.0864611,
			0.087295, 0.038354, 0.0385109, 0.0390135, 0.0377076, 0.0369457, 0.0368548},

			//K3R
			{0.0525656, 0.028327, 0.0300608, 0.0659333, 0.0658664, 0.0581643, 0.0922337, 0.0944063,
			0.0954413, 0.0425735, 0.0426203, 0.0430774, 0.0419289, 0.0409732, 0.0407511},

			//K5R
			{0.0488496, 0.0259948, 0.0274823, 0.060357, 0.0602345, 0.0532068, 0.0879267, 0.0898449,
			0.0907132, 0.0426415, 0.0427184, 0.0432004, 0.0419798, 0.0410482, 0.040854},

			//K7L
			{0.0516491, 0.0256889, 0.027052, 0.0655096, 0.0655121, 0.0584697, 0.0879762, 0.0900207,
			0.0909352, 0.0385824, 0.0386606, 0.0391034, 0.0379791, 0.0371435, 0.0369756},

			//K8L
			{0.0485922, 0.0241323, 0.0254286, 0.0617584, 0.0616407, 0.054036, 0.0866676, 0.088631,
			0.0894748, 0.0418658, 0.0419815, 0.0424865, 0.0411937, 0.0403135, 0.0401608}
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
			"Models/Catelli-V4.0_TKR_ligament_SL_edit.osim"
		};

		// Joint Parameters
		variableValues = {
			{
				{0, 120, 0, 0}, // hip
				{0, 120, 0, 0}, // knee
				{0, 30, 0, 0}, // ankle
				{-50, 0, 0, 0} // lumbar
			}
			//{
			//	{0, 82, 0, 0}, // hip
			//	{0, 92, 0, 0}, // knee
			//	{0, 25.5, 0, 0}, // ankle
			//	{-31.6, 0, 0, 0} // lumbar
			//}
		};

	}
	else { std::cout << "Please specify the model" << endl; return 1; }

	// ================
	// Run Optimisation
	// ================
	vector<double> comWeight = { 1 };
	vector<double> balWeight = { 10 };
	vector<double> ligWeight = { 3 };
	vector<double> accWeight = { 0.001 };
	double numIter = 1000;

	for (int l = 0; l < ligWeight.size(); ++l) {
		const double lig = ligWeight[0];
		ostringstream ossLig;
		ossLig << setfill('0') << setw(2) << lig;
		string ligStr = ossLig.str();
		for (int a = 0; a < accWeight.size(); ++a) {
			const double acc = accWeight[0];
			ostringstream ossAcc;
			ossAcc << setfill('0') << setw(4) << acc * 1000;
			string accStr = ossAcc.str();
			for (int b = 0; b < balWeight.size(); ++b) {
				const double bal = balWeight[b];
				ostringstream ossBal;
				ossBal << setfill('0') << setw(2) << bal;
				string balStr = ossBal.str();
				for (int c = 0; c < comWeight.size(); ++c) {
					const double com = comWeight[c];
					ostringstream ossCom;
					ossCom << setfill('0') << setw(2) << com;
					string comStr = ossCom.str();
					for (int i = 0; i < modelFiles.size(); ++i) {
						//if (i == 4) {
							// Set up the problem
						MocoStudy study;
						MocoProblem& problem = study.updProblem();
						//Model model = editModel(modelFiles[i], ligLengths[i]);
						Model model = editModel(modelFiles[i]);
						problem.setModelAsCopy(model);
						State state = model.initSystem();
						problem.setTimeBounds(0, 2);

						string modelFullName = model.getName();
						size_t endIndex = modelFullName.find('_');
						string modelName = (endIndex != string::npos) ? modelFullName.substr(0, endIndex) : modelFullName;
						string outputName;
						string angle; string fem_angle; string tib_angle; string comp_name;

						if (whichModel == "exp") {
							// Exp result name
							outputName = "Results/CAMS_Knee/" + modelName + "/" + modelName + "/" + modelName + "_a" + accStr + "b" + balStr + "c" + comStr + ".sto";
							cout << "" << endl; cout << "Solving: " + outputName << endl; cout << "" << endl;
						}
						else if (whichModel == "ref") {
							// Ref result name
							const auto& fset = model.getJointSet();
							const auto& femur_weld_r = fset.get("femur_weld_r");
							const auto& tibial_plat_weld_r = fset.get("tibial_plat_weld_r");
							const auto& fem_offset = femur_weld_r.get_frames(1);
							const auto& tib_offset = tibial_plat_weld_r.get_frames(1);
							const auto& fem_rot = fem_offset.get_orientation();
							const auto& tib_rot = tib_offset.get_orientation();
							double fem = 0; double tib = 0;

							if (plane == "vv") {
								fem = fem_rot[1] * 180 / Pi;
								tib = tib_rot[0] * 180 / Pi;
							} else if (plane == "ir") {
								fem = fem_rot[2] * 180 / Pi;
								tib = tib_rot[1] * 180 / Pi;
							} else if (plane == "sg") {
								fem = fem_rot[0] * 180 / Pi;
								tib = tib_rot[2] * 180 / Pi;
							}
							else { cout << "Please specify the plane" << endl; return 1; }

							string fem_str = to_string(static_cast<int>(round(abs(fem))));
							string tib_str = to_string(static_cast<int>(round(abs(tib))));

							fem_str = string(2 - fem_str.length(), '0') + fem_str;
							tib_str = string(2 - tib_str.length(), '0') + tib_str;

							if (fem < 0) { fem_str = '-' + fem_str; }
							else { fem_str = fem_str; }
							if (tib < 0) { tib_str = '-' + tib_str; }
							else { tib_str = tib_str; }

							if (component == "femoral") {
								angle = fem_str;
								comp_name = "fem";
							} else if (component == "tibial") {
								angle = tib_str;
								comp_name = "tib";
							} else if (component == "combined") {
								fem_angle = fem_str;
								tib_angle = tib_str;
								comp_name = "comb";
							}
							else { std::cout << "Please use correct component orientation type" << endl; return 1; }

							if (angle == "00") {
								outputName = "Results/Reference/neutral.sto";
							} else {
								if (component == "combined") {
									outputName = "Results/Reference/" + comp_name + "_" + plane + "_F" + fem_angle + "_T" + tib_angle + ".sto";
								} else {
									outputName = "Results/Reference/" + comp_name + "_" + plane + "_" + angle + ".sto";
								}
							}
							cout << "" << endl; cout << "Solving: " + outputName << endl; cout << "" << endl;
						}
						else { cout << "Please specify the model type" << endl; }

						// if (fs::exists(outputName)) { cout << "Output File Exists!" << endl; cout << "" << endl; }
						// else {
							// Set boundary conditions
							problem.setStateInfo("/jointset/hip_r/hip_flexion_r/value", MocoBounds(variableValues[i][0][0] * Pi / 180, variableValues[i][0][1] * Pi / 180),
								MocoInitialBounds(variableValues[i][0][2] * Pi / 180), MocoFinalBounds(variableValues[i][0][3] * Pi / 180));
							problem.setStateInfo("/jointset/knee_r/knee_flexion_r/value", MocoBounds(variableValues[i][1][0] * Pi / 180, variableValues[i][1][1] * Pi / 180),
								MocoInitialBounds(variableValues[i][1][2] * Pi / 180), MocoFinalBounds(variableValues[i][1][3] * Pi / 180));
							problem.setStateInfo("/jointset/ankle_r/ankle_angle_r/value", MocoBounds(variableValues[i][2][0] * Pi / 180, variableValues[i][2][1] * Pi / 180),
								MocoInitialBounds(variableValues[i][2][2] * Pi / 180), MocoFinalBounds(variableValues[i][2][3] * Pi / 180));
							problem.setStateInfo("/jointset/back/lumbar_extension/value", MocoBounds(variableValues[i][3][0] * Pi / 180, variableValues[i][3][1] * Pi / 180),
								MocoInitialBounds(variableValues[i][3][2] * Pi / 180), MocoFinalBounds(variableValues[i][3][3] * Pi / 180));
							problem.setStateInfoPattern("/jointset/.*/speed", {}, MocoInitialBounds(0), MocoFinalBounds(0));

							// Set goals
							auto* height = problem.addGoal<MocoOutputGoal>("height");
							height->setOutputPath("|com_position");
							height->setOutputIndex(1);
							height->setExponent(2);
							height->setWeight(com);
							auto* balance = problem.addGoal<MocoBalanceGoal>("balance");
							balance->setWeight(bal);
							auto* ligTension = problem.addGoal<MocoLigamentGoal>("ligament");
							ligTension->setWeight(lig);
							auto* effortGoal = problem.addGoal<MocoControlGoal>("effort");
							effortGoal->setExponent(2);
							effortGoal->setWeight(0.01);

							// 2,10,1,0.01,0.002 seems working

							// Configure the solver
							MocoCasADiSolver& solver = study.initCasADiSolver();
							solver.set_multibody_dynamics_mode("implicit");
							solver.set_minimize_implicit_multibody_accelerations(true);
							solver.set_implicit_multibody_accelerations_weight(acc);
							//solver.set_minimize_implicit_auxiliary_derivatives(true);
							//solver.set_implicit_auxiliary_derivatives_weight(0.001);
							solver.set_num_mesh_intervals(50);
							solver.set_verbosity(2);
							solver.set_optim_solver("ipopt");
							solver.set_optim_ipopt_print_level(4);
							solver.set_optim_max_iterations(numIter);
							solver.set_optim_convergence_tolerance(1e-4);
							solver.set_optim_constraint_tolerance(1e-4);

							MocoTrajectory guess = solver.createGuess("bounds");
							guess.resampleWithNumTimes(2);
							solver.setGuess(guess);

							// Solve the problem
							MocoSolution solution = study.solve();
							study.print("osimSquatGoal.xml");
							solution.unseal();

							// Save the solution
							if (whichModel == "exp") {
								solution.write("Results/CAMS_Knee/" + modelName + "/" + modelName + "/" + modelName + "_a" + accStr + "b" + balStr + "c" + comStr + ".sto");
								std::cout << "Results/CAMS_Knee/" + modelName + "/" + modelName + "/" + modelName + "_a" + accStr + "b" + balStr + "c" + comStr + ".sto saved!" << endl;
							}
							else if (whichModel == "ref") {

								if (angle == "00") {
									solution.write("Results/Reference/neutral.sto");
									std::cout << "Results/Reference/neutral.sto saved!" << endl;
								}
								else {
									if (component == "combined") {
										solution.write("Results/Reference/" + comp_name + "_" + plane + "_F" + fem_angle + "_T" + tib_angle + ".sto");
										std::cout << "Results/Reference/" + comp_name + "_" + plane + "_F" + fem_angle + "_T" + tib_angle + ".sto" << endl;
									}
									else {
										solution.write("Results/Reference/" + comp_name + "_" + plane + "_" + angle + ".sto");
										std::cout << "Results/Reference/" + comp_name + "_" + plane + "_" + angle + ".sto" << endl;
									}
								}
							}
							else { std::cout << "which model?" << endl; return 1; }

							std::cout << "Solution status: " << solution.getStatus() << endl;
							//study.visualize(solution);
						//}
						//}
					}
				}
			}
		}
	}


	// ==================================================================================
	// Print Out Peak Flexion Angle Differences Between the Experiment and the Prediction
	// ==================================================================================
	if (whichModel == "exp") {
		// Dataset
		string directoryPath = "Results/CAMS_Knee"; // Replace with your directory path
		vector<string> datasetFiles;

		WIN32_FIND_DATA findFileData;
		HANDLE hFind = FindFirstFile((directoryPath + "/*").c_str(), &findFileData);

		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
					datasetFiles.push_back(directoryPath + "/" + findFileData.cFileName);
				}
			} while (FindNextFile(hFind, &findFileData) != 0);
			FindClose(hFind);
		}
		else {
			cerr << "Error opening directory" << endl;
			return 1;
		}

		// Print the contents of the vector
		for (const string& fileName : datasetFiles) {
			std::cout << fileName << endl;
		}

		// Get Peak Flexion Angles for Each Joint
		vector<vector<double>> resultArray(datasetFiles.size(), vector<double>(3, 0.0));
		vector<vector<double>> diffArray(datasetFiles.size(), vector<double>(3, 0.0));

		// Peak Flexion Angles - Experimental
		double expArray[][3] = {
		{81.69399072, 70.65350934, 9.38602396},
		{107.0939643, 92.95941183, 16.05635248},
		{77.2417225, 71.67502451, 16.04911708},
		{81.80970554, 92.36328771, 25.53585152},
		{104.3089424, 80.57350014, 16.23760788},
		{117.4530983, 87.46181932, 12.22363194}
		};

		for (size_t i = 0; i < datasetFiles.size(); ++i) {
			const auto& datasetFile = datasetFiles[i];
			string datasetName = extractDatasetName(datasetFile);

			// Open the dataset file
			ifstream inputFile(datasetFile);

			if (!inputFile.is_open()) {
				std::cout << "Failed to open the file." << endl;
				return 1;
			}

			string line;
			bool foundHeader = false;
			int hipFlexionColumn = -1, kneeFlexionColumn = -1, ankleFlexionColumn = -1;
			double maxHipFlexion = 0.0, maxKneeFlexion = 0.0, maxAnkleFlexion = 0.0;

			// Read the file line by line
			while (getline(inputFile, line)) {
				if (!foundHeader) {
					// Find the header row containing the desired columns
					if (line.find("time") != string::npos) {
						vector<string> headers = splitString(line, '\t');

						// Find the column indices of the desired variables
						for (size_t i = 0; i < headers.size(); ++i) {
							if (headers[i] == "/jointset/hip_r/hip_flexion_r/value") {
								hipFlexionColumn = static_cast<int>(i);
							}
							else if (headers[i] == "/jointset/knee_r/knee_flexion_r/value") {
								kneeFlexionColumn = static_cast<int>(i);
							}
							else if (headers[i] == "/jointset/ankle_r/ankle_angle_r/value") {
								ankleFlexionColumn = static_cast<int>(i);
							}
						}

						foundHeader = true;
					}
				}
				else {
					// Find the maximum values in the desired columns
					vector<string> data = splitString(line, '\t');

					if (data.size() > max({ hipFlexionColumn, kneeFlexionColumn, ankleFlexionColumn })) {
						double hipFlexionValue = stod(data[hipFlexionColumn]);
						double kneeFlexionValue = stod(data[kneeFlexionColumn]);
						double ankleFlexionValue = stod(data[ankleFlexionColumn]);

						double hipFlexionDegrees = (hipFlexionValue * 180.0 / Pi);
						double kneeFlexionDegrees = (kneeFlexionValue * 180.0 / Pi);
						double ankleFlexionDegrees = (ankleFlexionValue * 180.0 / Pi);

						maxHipFlexion = max(maxHipFlexion, hipFlexionDegrees);
						maxKneeFlexion = max(maxKneeFlexion, kneeFlexionDegrees);
						maxAnkleFlexion = max(maxAnkleFlexion, ankleFlexionDegrees);
					}
				}
			}

			// Store the maximum values in the result array
			resultArray[i] = { maxHipFlexion, maxKneeFlexion, maxAnkleFlexion };

			// Calculate the difference
			for (int j = 0; j < 3; j++) {
				diffArray[i][j] = resultArray[i][j] - expArray[i][j];
			}

			// Close the file
			inputFile.close();
		}

		std::cout << "" << endl;
		std::cout << "" << endl;

		std::cout << "Experimental Maximum Flexion Angles" << endl;
		std::cout << setw(10) << " " << setw(10) << "hip" << setw(10) << "knee" << setw(10) << "ankle" << endl;

		for (size_t i = 0; i < sizeof(expArray) / sizeof(expArray[0]); ++i) {
			string datasetName = extractDatasetName(datasetFiles[i]);
			size_t lastSlashPos = datasetName.find_last_of("/");
			if (lastSlashPos != string::npos && lastSlashPos + 1 < datasetName.length()) {
				// Extract and print the filename without the extension
				string fileName = datasetName.substr(lastSlashPos + 1);
				size_t extensionPos = fileName.rfind(".");
				if (extensionPos != string::npos) {
					fileName = fileName.substr(0, extensionPos);
				}
				std::cout << left << setw(10) << fileName;
			}
			for (size_t j = 0; j < sizeof(expArray[0]) / sizeof(expArray[0][0]); ++j) {
				std::cout << setw(10) << expArray[i][j];
			}
			std::cout << endl;
		}

		std::cout << "" << endl;
		std::cout << "" << endl;

		std::cout << "Predicted Maximum Flexion Angles" << endl;
		std::cout << setw(10) << " " << setw(10) << "hip" << setw(10) << "knee" << setw(10) << "ankle" << endl;
		for (size_t i = 0; i < resultArray.size(); ++i) {
			string datasetName = extractDatasetName(datasetFiles[i]);
			size_t lastSlashPos = datasetName.find_last_of("/");
			if (lastSlashPos != string::npos && lastSlashPos + 1 < datasetName.length()) {
				// Extract and print the filename without the extension
				string fileName = datasetName.substr(lastSlashPos + 1);
				size_t extensionPos = fileName.rfind(".");
				if (extensionPos != string::npos) {
					fileName = fileName.substr(0, extensionPos);
				}
				std::cout << left << setw(10) << fileName;
			}
			for (size_t j = 0; j < resultArray[i].size(); ++j) {
				std::cout << setw(10) << resultArray[i][j];
			}
			std::cout << endl;
		}

		std::cout << "" << endl;
		std::cout << "" << endl;

		std::cout << "Maximum Flexion Angle Differences: EXP VS PRED" << endl;
		std::cout << setw(10) << " " << setw(10) << "hip" << setw(10) << "knee" << setw(10) << "ankle" << endl;
		for (size_t i = 0; i < diffArray.size(); ++i) {
			string datasetName = extractDatasetName(datasetFiles[i]);
			size_t lastSlashPos = datasetName.find_last_of("/");
			if (lastSlashPos != string::npos && lastSlashPos + 1 < datasetName.length()) {
				// Extract and print the filename without the extension
				string fileName = datasetName.substr(lastSlashPos + 1);
				size_t extensionPos = fileName.rfind(".");
				if (extensionPos != string::npos) {
					fileName = fileName.substr(0, extensionPos);
				}
				std::cout << left << setw(10) << fileName;
			}
			for (size_t j = 0; j < diffArray[i].size(); ++j) {
				std::cout << setw(10) << diffArray[i][j];
			}
			std::cout << endl;
		}
	}
	else if (whichModel == "ref") {
		// Dataset
		string directoryPath = "Results/Reference"; // Replace with your directory path
		vector<string> datasetFiles;

		WIN32_FIND_DATA findFileData;
		HANDLE hFind = FindFirstFile((directoryPath + "/*").c_str(), &findFileData);

		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
					datasetFiles.push_back(directoryPath + "/" + findFileData.cFileName);
				}
			} while (FindNextFile(hFind, &findFileData) != 0);
			FindClose(hFind);
		}
		else {
			cerr << "Error opening directory" << endl;
			return 1;
		}

		// Print the contents of the vector
		for (const string& fileName : datasetFiles) {
			std::cout << fileName << endl;
		}

		// Get Peak Flexion Angles for Each Joint
		vector<vector<double>> resultArray(datasetFiles.size(), vector<double>(3, 0.0));

		for (size_t i = 0; i < datasetFiles.size(); ++i) {
			const auto& datasetFile = datasetFiles[i];
			string datasetName = extractDatasetName(datasetFile);

			// Open the dataset file
			ifstream inputFile(datasetFile);

			if (!inputFile.is_open()) {
				std::cout << "Failed to open the file." << endl;
				return 1;
			}

			string line;
			bool foundHeader = false;
			int hipFlexionColumn = -1, kneeFlexionColumn = -1, ankleFlexionColumn = -1;
			double maxHipFlexion = 0.0, maxKneeFlexion = 0.0, maxAnkleFlexion = 0.0;

			// Read the file line by line
			while (getline(inputFile, line)) {
				if (!foundHeader) {
					// Find the header row containing the desired columns
					if (line.find("time") != string::npos) {
						vector<string> headers = splitString(line, '\t');

						// Find the column indices of the desired variables
						for (size_t i = 0; i < headers.size(); ++i) {
							if (headers[i] == "/jointset/hip_r/hip_flexion_r/value") {
								hipFlexionColumn = static_cast<int>(i);
							}
							else if (headers[i] == "/jointset/knee_r/knee_flexion_r/value") {
								kneeFlexionColumn = static_cast<int>(i);
							}
							else if (headers[i] == "/jointset/ankle_r/ankle_angle_r/value") {
								ankleFlexionColumn = static_cast<int>(i);
							}
						}

						foundHeader = true;
					}
				}
				else {
					// Find the maximum values in the desired columns
					vector<string> data = splitString(line, '\t');

					if (data.size() > max({ hipFlexionColumn, kneeFlexionColumn, ankleFlexionColumn })) {
						double hipFlexionValue = stod(data[hipFlexionColumn]);
						double kneeFlexionValue = stod(data[kneeFlexionColumn]);
						double ankleFlexionValue = stod(data[ankleFlexionColumn]);

						double hipFlexionDegrees = (hipFlexionValue * 180.0 / Pi);
						double kneeFlexionDegrees = (kneeFlexionValue * 180.0 / Pi);
						double ankleFlexionDegrees = (ankleFlexionValue * 180.0 / Pi);

						maxHipFlexion = max(maxHipFlexion, hipFlexionDegrees);
						maxKneeFlexion = max(maxKneeFlexion, kneeFlexionDegrees);
						maxAnkleFlexion = max(maxAnkleFlexion, ankleFlexionDegrees);
					}
				}
			}

			// Store the maximum values in the result array
			resultArray[i] = { maxHipFlexion, maxKneeFlexion, maxAnkleFlexion };

			// Close the file
			inputFile.close();
		}

		std::cout << "" << endl;
		std::cout << "" << endl;

		std::cout << "Predicted Maximum Flexion Angles" << endl;
		std::cout << std::setw(15) <<  "hip" << std::setw(10) << "knee" << std::setw(10) << "ankle" << std::endl;
		for (size_t i = 0; i < resultArray.size(); ++i) {
			string datasetName = extractDatasetName(datasetFiles[i]);
			size_t lastSlashPos = datasetName.find_last_of("/");
			if (lastSlashPos != string::npos && lastSlashPos + 1 < datasetName.length()) {
				// Extract and print the filename without the extension
				string fileName = datasetName.substr(lastSlashPos + 1);
				size_t extensionPos = fileName.rfind(".");
				if (extensionPos != string::npos) {
					fileName = fileName.substr(0, extensionPos);
				}
				std::cout << left << setw(10) << fileName << setw(20);
			}
			for (size_t j = 0; j < resultArray[i].size(); ++j) {
				std::cout << setw(10) << resultArray[i][j];
			}
			std::cout << endl;
		}

		std::cout << "" << endl;
		std::cout << "" << endl;

	}
	else { std::cout << "Use a correct model" << endl; return 1; }

	return EXIT_SUCCESS;

}