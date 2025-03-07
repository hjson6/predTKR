#include "MocoCustomEffortGoal.h"

using namespace OpenSim;

void MocoCustomEffortGoal::initializeOnModelImpl(const Model&) const {
    setRequirements(1, 1);
}

void MocoCustomEffortGoal::calcIntegrandImpl(
        const IntegrandInput& input, double& integrand) const {
    getModel().realizeVelocity(input.state);
    const auto& controls = getModel().getControls(input.state);
    integrand = controls.normSqr();
}

void MocoCustomEffortGoal::calcGoalImpl(
        const GoalInput& input, SimTK::Vector& cost) const {
    cost[0] = input.integral;
}
