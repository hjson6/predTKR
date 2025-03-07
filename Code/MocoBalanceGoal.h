#ifndef MOCOBALANCEGOAL_H
#define MOCOBALANCEGOAL_H

#include <OpenSim/Moco/osimMoco.h>
#include "osimMocoBalanceGoalDLL.h"

namespace OpenSim {

    class OSIMMOCOBALANCEGOAL_API MocoBalanceGoal : public MocoGoal {
        OpenSim_DECLARE_CONCRETE_OBJECT(MocoBalanceGoal, MocoGoal);

    public:
        MocoBalanceGoal() {}
        MocoBalanceGoal(std::string name) : MocoGoal(std::move(name)) {}
        MocoBalanceGoal(std::string name, double weight) 
            : MocoGoal(std::move(name), weight) {}

    protected:
        Mode getDefaultModeImpl() const override { return Mode::Cost; }
        bool getSupportsEndpointConstraintImpl() const override { return true; }
        void initializeOnModelImpl(const Model&) const override;
        void calcIntegrandImpl(const IntegrandInput& input, double& integrand) const override;
        void calcGoalImpl(const GoalInput& input, SimTK::Vector& cost) const override;
    };
}
#endif // MOCOBALANCEGOAL_H
