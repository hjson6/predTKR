#include "MocoHeightVelGoal.h"

using namespace OpenSim;

void MocoHeightVelGoal::initializeOnModelImpl(const Model&) const {
    setRequirements(1, 1);
}

void MocoHeightVelGoal::calcIntegrandImpl(
        const IntegrandInput& input, double& integrand) const {
    getModel().realizeVelocity(input.state);

    const SimTK::Vec3& COM = getModel().calcMassCenterVelocity(input.state);

    integrand = COM[1] * COM[1];
}

void MocoHeightVelGoal::calcGoalImpl(
        const GoalInput& input, SimTK::Vector& cost) const {
    cost[0] = input.integral;
}