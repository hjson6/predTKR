#ifndef MOCOBALANCEGOALRIGHT_H
#define MOCOBALANCEGOALRIGHT_H

#include <OpenSim/Moco/osimMoco.h>
#include "osimMocoBalanceGoalRightDLL.h"

namespace OpenSim {

    class OSIMMOCOBALANCEGOALRIGHT_API MocoBalanceGoalRight : public MocoGoal {
        OpenSim_DECLARE_CONCRETE_OBJECT(MocoBalanceGoalRight, MocoGoal);

    public:
        MocoBalanceGoalRight() {}
        MocoBalanceGoalRight(std::string name) : MocoGoal(std::move(name)) {}
        MocoBalanceGoalRight(std::string name, double weight) 
            : MocoGoal(std::move(name), weight) {}

    protected:
        Mode getDefaultModeImpl() const override { return Mode::Cost; }
        bool getSupportsEndpointConstraintImpl() const override { return true; }
        void initializeOnModelImpl(const Model&) const override;
        void calcIntegrandImpl(const IntegrandInput& input, double& integrand) const override;
        void calcGoalImpl(const GoalInput& input, SimTK::Vector& cost) const override;
    };
}
#endif // MOCOBALANCEGOALRIGHT_H
