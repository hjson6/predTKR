#ifndef MOCOLIGAMENTGOAL_H
#define MOCOLIGAMENTGOAL_H

#include <OpenSim/Moco/osimMoco.h>
#include "osimMocoLigamentGoalDLL.h"

namespace OpenSim {

    class OSIMMOCOLIGAMENTGOAL_API MocoLigamentGoal : public MocoGoal {
        OpenSim_DECLARE_CONCRETE_OBJECT(MocoLigamentGoal, MocoGoal);

    public:
        MocoLigamentGoal() {}
        MocoLigamentGoal(std::string name) : MocoGoal(std::move(name)) {}
        MocoLigamentGoal(std::string name, double weight)
            : MocoGoal(std::move(name), weight) {}

    protected:
        Mode getDefaultModeImpl() const override { return Mode::Cost; }
        bool getSupportsEndpointConstraintImpl() const override { return true; }
        void initializeOnModelImpl(const Model&) const override;
        void calcIntegrandImpl(const IntegrandInput& input, double& integrand) const override;
        void calcGoalImpl(const GoalInput& input, SimTK::Vector& cost) const override;
    };
}
#endif // MOCOLIGAMENTGOAL_H