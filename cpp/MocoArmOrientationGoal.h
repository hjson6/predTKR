#ifndef MOCOARMORIENTATIONGOAL_H
#define MOCOARMORIENTATIONGOAL_H

#include <OpenSim/Moco/osimMoco.h>
#include <OpenSim/Simulation/Model/ModelComponent.h>
#include <OpenSim/Simulation/Model/Model.h>
#include <OpenSim/Common/Exception.h>

using namespace OpenSim;
using namespace SimTK;

class MocoArmOrientationGoal : public MocoGoal {
    OpenSim_DECLARE_CONCRETE_OBJECT(MocoArmOrientationGoal, MocoGoal);

public:
    MocoArmOrientationGoal() = default;
    MocoArmOrientationGoal(std::string name) : MocoGoal(std::move(name)) {}
    MocoArmOrientationGoal(std::string name, double weight) : MocoGoal(std::move(name), weight) {}

protected:
    Mode getDefaultModeImpl() const override;
    bool getSupportsEndpointConstraintImpl() const override;
    void initializeOnModelImpl(const Model&) const override;
    void calcIntegrandImpl(const IntegrandInput& input, double& integrand) const override;
    void calcGoalImpl(const GoalInput& input, SimTK::Vector& cost) const override;
};

#endif // MOCOARMORIENTATIONGOAL_H
