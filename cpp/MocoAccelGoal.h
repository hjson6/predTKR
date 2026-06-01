#ifndef OPENSIM_MOCOACCELGOAL_H
#define OPENSIM_MOCOACCELGOAL_H

#include <OpenSim/Moco/osimMoco.h>
#include "osimMocoAccelGoalDLL.h"

namespace OpenSim {

class OSIMMOCOACCELGOAL_API MocoAccelGoal : public MocoGoal {
    OpenSim_DECLARE_CONCRETE_OBJECT(MocoAccelGoal, MocoGoal);

public:
    MocoAccelGoal() {}
    MocoAccelGoal(std::string name) : MocoGoal(std::move(name)) {}
    MocoAccelGoal(std::string name, double weight)
            : MocoGoal(std::move(name), weight) {}

    // Set per-coordinate weights for accelerations
    void setAccelerationWeights(const SimTK::Vector& weights) {
        _accelerationWeights = weights;
    }

protected:
    Mode getDefaultModeImpl() const override { return Mode::Cost; }
    bool getSupportsEndpointConstraintImpl() const override { return true; }
    void initializeOnModelImpl(const Model&) const override;
    void calcIntegrandImpl(
            const IntegrandInput& input, double& integrand) const override;
    void calcGoalImpl(
            const GoalInput& input, SimTK::Vector& cost) const override;

private:
    SimTK::Vector _accelerationWeights;  // Vector to store weights for each coordinate
};

} // namespace OpenSim

#endif // OPENSIM_MOCOACCELGOAL_H
