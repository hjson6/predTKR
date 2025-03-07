#ifndef OPENSIM_REGISTERTYPES_OSIMMOCOBALANCEGOALRIGHT_H
#define OPENSIM_REGISTERTYPES_OSIMMOCOBALANCEGOALRIGHT_H
/* -------------------------------------------------------------------------- *
 * OpenSim: RegisterTypes_osimMocoBalanceGoalRight.h                               *
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

#include "osimMocoBalanceGoalRightDLL.h"

extern "C" {

OSIMMOCOBALANCEGOALRIGHT_API void RegisterTypes_osimMocoBalanceGoalRight();

}

class osimMocoBalanceGoalRightInstantiator {
public:
    osimMocoBalanceGoalRightInstantiator();
private:
    void registerDllClasses();
};

#endif // OPENSIM_REGISTERTYPES_OSIMMOCOBALANCEGOALRIGHT_H
