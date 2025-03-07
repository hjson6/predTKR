#include "MocoArmOrientationGoal.h"
#include <OpenSim/Simulation/Model/Model.h>
#include <OpenSim/Simulation/Model/BodySet.h>
#include <OpenSim/Simulation/Model/Station.h>

using namespace OpenSim;
using namespace SimTK;
using namespace std;

MocoArmOrientationGoal::Mode MocoArmOrientationGoal::getDefaultModeImpl() const {
    return Mode::Cost;
}

bool MocoArmOrientationGoal::getSupportsEndpointConstraintImpl() const {
    return true;
}

void MocoArmOrientationGoal::initializeOnModelImpl(const Model&) const {
    setRequirements(1, 1);
}

void MocoArmOrientationGoal::calcIntegrandImpl(const IntegrandInput& input, double& integrand) const {
    getModel().realizeAcceleration(input.state);

    Vec3 hand_mc = getModel().getBodySet().get("hand_r").getMassCenter();
    Transform hand_rot = getModel().getBodySet().get("hand_r").findTransformBetween(input.state, getModel().getGround());
        
    Mat44 transformationMatrix(0.0335296, -0.701545, 0.711836, 0.275437,
        0.995049, -0.0432398, -0.0894845, 0.626178,
        0.0935571, 0.711312, 0.696622, 0.074089,
        0, 0, 0, 1);

    // Extract rotation matrix and translation vector from the transformation matrix
    Mat33 rotmat33(transformationMatrix(0, 0), transformationMatrix(0, 1), transformationMatrix(0, 2),
        transformationMatrix(1, 0), transformationMatrix(1, 1), transformationMatrix(1, 2),
        transformationMatrix(2, 0), transformationMatrix(2, 1), transformationMatrix(2, 2));

    Rotation rotation(rotmat33);

    Vec3 translation(transformationMatrix(0, 3), transformationMatrix(1, 3), transformationMatrix(2, 3));

    // Create a Transform object from the rotation matrix and translation vector
    Transform target_transform(rotation, translation);

    cout << "Transform is: " << target_transform << endl;
   //cout << "balance: " << fabs(COM[0] - calcn_r_pos[0]) << endl;
   
    integrand = hand_rot;
}

void MocoBalanceGoal::calcGoalImpl(const GoalInput& input, SimTK::Vector& cost) const {
    cost[0] = input.integral;
}
