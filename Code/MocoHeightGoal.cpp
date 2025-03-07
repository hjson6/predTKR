#include "MocoHeightGoal.h"

using namespace OpenSim;

void MocoHeightGoal::initializeOnModelImpl(const Model&) const {
    setRequirements(1, 1);
}

void MocoHeightGoal::calcIntegrandImpl(
        const IntegrandInput& input, double& integrand) const {
    getModel().realizeAcceleration(input.state);

    const SimTK::Vec3& COM = getModel().calcMassCenterPosition(input.state);
    //const SimTK::Vec3& COM = getModel().calcMassCenterAcceleration(input.state);

    integrand = COM[1] * COM[1];
}

void MocoHeightGoal::calcGoalImpl(
        const GoalInput& input, SimTK::Vector& cost) const {
    cost[0] = input.integral;
}