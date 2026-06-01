#ifndef MOCOHEIGHTGOAL_H
#define MOCOHEIGHTGOAL_H

#include <OpenSim/Moco/osimMoco.h>
#include "osimMocoHeightGoalDLL.h"

namespace OpenSim {

    class OSIMMOCOHEIGHTGOAL_API MocoHeightGoal : public MocoGoal {
        OpenSim_DECLARE_CONCRETE_OBJECT(MocoHeightGoal, MocoGoal);

    public:
        MocoHeightGoal() {}
        MocoHeightGoal(std::string name) : MocoGoal(std::move(name)) {}
        MocoHeightGoal(std::string name, double weight) 
            : MocoGoal(std::move(name), weight) {}

    protected:
        Mode getDefaultModeImpl() const override { return Mode::Cost; }
        bool getSupportsEndpointConstraintImpl() const override { return true; }
        void initializeOnModelImpl(const Model&) const override;
        void calcIntegrandImpl(const IntegrandInput& input, double& integrand) const override;
        void calcGoalImpl(const GoalInput& input, SimTK::Vector& cost) const override;
    };
}
#endif // MOCOHEIGHTGOAL_H
