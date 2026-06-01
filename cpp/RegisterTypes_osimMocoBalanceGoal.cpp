/* -------------------------------------------------------------------------- *
 * OpenSim Moco: RegisterTypes_osimMocoBalanceGoal.cpp                        *
 * -------------------------------------------------------------------------- *
 * Copyright (c) 2019 Stanford University and the Authors                     *
 *                                                                            *
 * Author(s): Hojin Song                                                      *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may    *
 * not use this file except in compliance with the License. You may obtain a  *
 * copy of the License at http://www.apache.org/licenses/LICENSE-2.0          *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an "AS IS" BASIS,          *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 * -------------------------------------------------------------------------- */
#include "MocoBalanceGoal.h"
#include "RegisterTypes_osimMocoBalanceGoal.h"

using namespace OpenSim;

static osimMocoBalanceGoalInstantiator instantiator;

OSIMMOCOBALANCEGOAL_API void RegisterTypes_osimMocoBalanceGoal() {
    try {
        Object::registerType(MocoBalanceGoal());
    } catch (const std::exception& e) {
        std::cerr << "ERROR during osimMocoBalanceGoal "
                     "Object registration:\n"
                  << e.what() << std::endl;
    }
}

osimMocoBalanceGoalInstantiator::osimMocoBalanceGoalInstantiator() {
    registerDllClasses();
}

void osimMocoBalanceGoalInstantiator::registerDllClasses() {
    RegisterTypes_osimMocoBalanceGoal();
}
