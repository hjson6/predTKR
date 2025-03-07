#ifndef OPENSIM_MOCOCUSTOMEFFORTGOAL_H
#define OPENSIM_MOCOCUSTOMEFFORTGOAL_H

#include <OpenSim/Moco/osimMoco.h>
#include "osimMocoCustomEffortGoalDLL.h"

namespace OpenSim {

class OSIMMOCOCUSTOMEFFORTGOAL_API MocoCustomEffortGoal : public MocoGoal {
    OpenSim_DECLARE_CONCRETE_OBJECT(MocoCustomEffortGoal, MocoGoal);

public:
    MocoCustomEffortGoal() {}
    MocoCustomEffortGoal(std::string name) : MocoGoal(std::move(name)) {}
    MocoCustomEffortGoal(std::string name, double weight)
            : MocoGoal(std::move(name), weight) {}

protected:
    Mode getDefaultModeImpl() const override { return Mode::Cost; }
    bool getSupportsEndpointConstraintImpl() const override { return true; }
    void initializeOnModelImpl(const Model&) const override;
    void calcIntegrandImpl(
            const IntegrandInput& input, double& integrand) const override;
    void calcGoalImpl(
            const GoalInput& input, SimTK::Vector& cost) const override;
};

} // namespace OpenSim

#endif // OPENSIM_MOCOCUSTOMEFFORTGOAL_H
