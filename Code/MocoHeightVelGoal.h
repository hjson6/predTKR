#ifndef MOCOHEIGHTVELGOAL_H
#define MOCOHEIGHTVELGOAL_H

#include <OpenSim/Moco/osimMoco.h>
#include "osimMocoHeightVelGoalDLL.h"

namespace OpenSim {

    class OSIMMOCOHEIGHTVELGOAL_API MocoHeightVelGoal : public MocoGoal {
        OpenSim_DECLARE_CONCRETE_OBJECT(MocoHeightVelGoal, MocoGoal);

    public:
        MocoHeightVelGoal() {}
        MocoHeightVelGoal(std::string name) : MocoGoal(std::move(name)) {}
        MocoHeightVelGoal(std::string name, double weight)
            : MocoGoal(std::move(name), weight) {}

    protected:
        Mode getDefaultModeImpl() const override { return Mode::Cost; }
        bool getSupportsEndpointConstraintImpl() const override { return true; }
        void initializeOnModelImpl(const Model&) const override;
        void calcIntegrandImpl(const IntegrandInput& input, double& integrand) const override;
        void calcGoalImpl(const GoalInput& input, SimTK::Vector& cost) const override;
    };
}
#endif // MOCOHEIGHTVELGOAL_H
