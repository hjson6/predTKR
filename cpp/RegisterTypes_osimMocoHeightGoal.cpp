/* -------------------------------------------------------------------------- *
 * OpenSim Moco: RegisterTypes_osimMocoHeightGoal.cpp                         *
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
#include "MocoHeightGoal.h"
#include "RegisterTypes_osimMocoHeightGoal.h"

using namespace OpenSim;

static osimMocoHeightGoalInstantiator instantiator;

OSIMMOCOHEIGHTGOAL_API void RegisterTypes_osimMocoHeightGoal() {
    try {
        Object::registerType(MocoHeightGoal());
    } catch (const std::exception& e) {
        std::cerr << "ERROR during osimMocoHeightGoal "
                     "Object registration:\n"
                  << e.what() << std::endl;
    }
}

osimMocoHeightGoalInstantiator::osimMocoHeightGoalInstantiator() {
    registerDllClasses();
}

void osimMocoHeightGoalInstantiator::registerDllClasses() {
    RegisterTypes_osimMocoHeightGoal();
}
