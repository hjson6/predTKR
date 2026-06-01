#include "MocoAccelGoal.h"

using namespace OpenSim;

void MocoAccelGoal::initializeOnModelImpl(const Model&) const {
    setRequirements(1, 1);
}

void MocoAccelGoal::calcIntegrandImpl(
        const IntegrandInput& input, double& integrand) const {
    getModel().realizeAcceleration(input.state);

    // Ensure the weights vector is correctly sized
    const auto& cset = getModel().getCoordinateSet();
    if (_accelerationWeights.size() != cset.getSize()) {
        throw OpenSim::Exception("The number of weights does not match the number of coordinates.");
    }

    std::vector<double> accel;
    for (int i = 0; i < cset.getSize(); ++i) {
        auto& coord = cset.get(i);
        auto val = coord.getAccelerationValue(input.state);
        accel.push_back(val * val);
    }

    if (accel.size() != _accelerationWeights.size()) {
        std::cerr << "Error: accel and _accelerationWeights have different sizes!" << std::endl;
    }

    double weightedAccelSum = 0.0;
    for (int i = 0; i < accel.size(); ++i) {
        weightedAccelSum += accel[i] * _accelerationWeights[i];
    }

    integrand = weightedAccelSum;
}

void MocoAccelGoal::calcGoalImpl(
        const GoalInput& input, SimTK::Vector& cost) const {
    cost[0] = input.integral;
}
