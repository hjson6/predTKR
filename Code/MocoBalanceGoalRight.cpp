#include "MocoBalanceGoalRight.h"

using namespace OpenSim;

void MocoBalanceGoalRight::initializeOnModelImpl(const Model&) const {
    setRequirements(1, 1);
}

void MocoBalanceGoalRight::calcIntegrandImpl(
        const IntegrandInput& input, double& integrand) const {
    getModel().realizeAcceleration(input.state);

    const SimTK::Vec3& COM = getModel().calcMassCenterPosition(input.state);
    const SimTK::Vec3& calcn_pos = getModel().getBodySet().get("calcn_r").getPositionInGround(input.state);
    const SimTK::Vec3& toes_pos  = getModel().getBodySet().get("toes_r").getPositionInGround(input.state);
    double foot = (calcn_pos[0] + toes_pos[0]) / 2;
    //const SimTK::Vec3& calcn_mc = getModel().getBodySet().get("calcn_r").getMassCenter();
    //const SimTK::Vec3& calcn_pos = getModel().getBodySet().get("calcn_r").findStationLocationInGround(input.state, calcn_mc);
    //double diff = COM[0] - calcn_pos[0];
    double diff = COM[0] - foot;

    integrand = diff * diff;
}

void MocoBalanceGoalRight::calcGoalImpl(
        const GoalInput& input, SimTK::Vector& cost) const {
    cost[0] = input.integral;
}
