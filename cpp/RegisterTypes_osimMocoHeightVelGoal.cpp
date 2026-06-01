/* -------------------------------------------------------------------------- *
 * OpenSim Moco: RegisterTypes_osimMocoHeightVelGoal.cpp                         *
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
#include "MocoHeightVelGoal.h"
#include "RegisterTypes_osimMocoHeightVelGoal.h"

using namespace OpenSim;

static osimMocoHeightVelGoalInstantiator instantiator;

OSIMMOCOHEIGHTVELGOAL_API void RegisterTypes_osimMocoHeightVelGoal() {
    try {
        Object::registerType(MocoHeightVelGoal());
    } catch (const std::exception& e) {
        std::cerr << "ERROR during osimMocoHeightVelGoal "
                     "Object registration:\n"
                  << e.what() << std::endl;
    }
}

osimMocoHeightVelGoalInstantiator::osimMocoHeightVelGoalInstantiator() {
    registerDllClasses();
}

void osimMocoHeightVelGoalInstantiator::registerDllClasses() {
    RegisterTypes_osimMocoHeightVelGoal();
}
