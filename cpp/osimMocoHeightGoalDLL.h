#ifndef OPENSIM_OSIMMOCOHEIGHTGOALDLL_H
#define OPENSIM_OSIMMOCOHEIGHTGOALDLL_H
/* -------------------------------------------------------------------------- *
 * OpenSim: osimMocoHeightGoalDLL.h                                           *
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

#ifndef _WIN32
    #define OSIMMOCOHEIGHTGOAL_API
#else
    #ifdef OSIMMOCOHEIGHTGOAL_EXPORTS
        #define OSIMMOCOHEIGHTGOAL_API __declspec(dllexport)
    #else
        #define OSIMMOCOHEIGHTGOAL_API __declspec(dllimport)
    #endif
#endif

#endif // OPENSIM_OSIMMOCOHEIGHTGOALDLL_H
