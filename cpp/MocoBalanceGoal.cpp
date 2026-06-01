#include "MocoBalanceGoal.h"

using namespace OpenSim;

void MocoBalanceGoal::initializeOnModelImpl(const Model&) const {
    setRequirements(1, 1);
}

void MocoBalanceGoal::calcIntegrandImpl(
        const IntegrandInput& input, double& integrand) const {
    getModel().realizeAcceleration(input.state);

    const SimTK::Vec3& COM = getModel().calcMassCenterPosition(input.state);
    const SimTK::Vec3& calcn_mc = getModel().getBodySet().get("calcn_l").getMassCenter();
    const SimTK::Vec3& calcn_pos = getModel().getBodySet().get("calcn_l").findStationLocationInGround(input.state, calcn_mc);
    double diff = COM[0] - calcn_pos[0];

    integrand = diff * diff;
}

void MocoBalanceGoal::calcGoalImpl(
        const GoalInput& input, SimTK::Vector& cost) const {
    cost[0] = input.integral;
}
