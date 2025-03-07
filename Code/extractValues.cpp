#include <OpenSim/OpenSim.h>

using namespace OpenSim;
using namespace SimTK;
using namespace std;

int main() {
	vector<string> modelFiles;
	modelFiles = {
		"Models/K1L_scaled-sl-c_init_1991_edit.osim",
		"Models/K2L_scaled-sl-c.osim",
		"Models/K3R_scaled-sl-c.osim",
		"Models/K5R_scaled-sl-c.osim",
		"Models/K7L_scaled-sl-c.osim",
		"Models/K8L_scaled-sl-c.osim"
	};
	Model model(modelFiles[0]);
	State state = model.initSystem();
	Storage motion("Results/CAMS_Knee/K1L_a0004b05c03l16.mot");



	/*model.realizePosition(state);
	const auto timestep = motion.getMinTimeStep();
	const CoordinateSet& cset = model.getCoordinateSet();
	const ForceSet& fset = model.getForceSet();
	Blankevoort1991Ligament* aLCL = dynamic_cast<Blankevoort1991Ligament*>(&fset.get(1));
	Blankevoort1991Ligament* mLCL = dynamic_cast<Blankevoort1991Ligament*>(&fset.get(2));
	Blankevoort1991Ligament* pLCL = dynamic_cast<Blankevoort1991Ligament*>(&fset.get(3));
	Blankevoort1991Ligament* aMCL = dynamic_cast<Blankevoort1991Ligament*>(&fset.get(4));
	Blankevoort1991Ligament* mMCL = dynamic_cast<Blankevoort1991Ligament*>(&fset.get(5));
	Blankevoort1991Ligament* pMCL = dynamic_cast<Blankevoort1991Ligament*>(&fset.get(6));

	Array<double> aLCL_strain; Array<double> mLCL_strain; Array<double> pLCL_strain;
	Array<double> aMCL_strain; Array<double> mMCL_strain; Array<double> pMCL_strain;

	for (int i = 0; i < timestep; ++i) {

		for (int j = 0; j < cset.getSize(); ++j) {
			Array<double> coordValue;
			const Coordinate& coord = cset.get(j);
			motion.getDataColumn(coord.getName(), coordValue);
			double q = coordValue.get(j - 0);
			model.updCoordinateSet().get(j).setValue(state, q);
		}


		aLCL_strain[i] = aLCL->getStrain(state);
		mLCL_strain[i] = mLCL->getStrain(state);
		pLCL_strain[i] = pLCL->getStrain(state);
		aMCL_strain[i] = aMCL->getStrain(state);
		mMCL_strain[i] = mMCL->getStrain(state);
		pMCL_strain[i] = pMCL->getStrain(state);
		cout << aLCL_strain << endl;

	}*/



	cout << "succeed!" << endl;

    return 0;
}
