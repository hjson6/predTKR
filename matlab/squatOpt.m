function [time, data, iter] = squatOpt(s, weights)

import org.opensim.modeling.*;
pardir = fileparts(pwd);
opensimCommon.LoadOpenSimLibraryExact([pardir '\RelWithDebInfo\osimMocoHeightGoal.dll']);
opensimCommon.LoadOpenSimLibraryExact([pardir '\RelWithDebInfo\osimMocoBalanceGoal.dll']);
opensimCommon.LoadOpenSimLibraryExact([pardir '\RelWithDebInfo\osimMocoBalanceGoalRight.dll']);
opensimCommon.LoadOpenSimLibraryExact([pardir '\RelWithDebInfo\osimMocoLigamentGoal.dll']);

modelName = {'K1L_scaled_sl_na_rots.osim',...
			 'K2L_scaled_sl_na.osim',...
			 'K3R_scaled_sl_na.osim',...
			 'K5R_scaled_sl_na.osim',...
			 'K7L_scaled_sl_na_0.osim',...
			 'K8L_scaled_sl_na.osim'};

% Load Model
disp(modelName{s.subject})
parts = split(modelName{s.subject}, {'_'});
modelType = parts{1};
whichSide = lower(modelType(end));
model = getTorqueDrivenModel(modelName{s.subject}, s.optForce);

% Initialise Moco Study
study = MocoStudy([pardir '/SquatOpt__' whichSide '.moco']);

problem = study.updProblem();
problem.setModel(model);

% Boundary Conditions
problem.setTimeBounds(MocoInitialBounds(0), MocoFinalBounds(s.finTime));

if s.rots == 1
    bound = getVariableMatrix(modelType);
    problem.setStateInfo(['/jointset/hip_' whichSide '/hip_flexion_' whichSide '/value']  , MocoBounds(bound(1,1), bound(1,2))...
        ,MocoInitialBounds(0), MocoFinalBounds(0));
    problem.setStateInfo(['/jointset/knee_' whichSide '/knee_flexion_' whichSide '/value'], MocoBounds(bound(2,1), bound(2,2))...
        ,MocoInitialBounds(0), MocoFinalBounds(0));
    problem.setStateInfo(['/jointset/ankle_' whichSide '/ankle_angle_' whichSide '/value'], MocoBounds(bound(3,1), bound(3,2))...
        ,MocoInitialBounds(0), MocoFinalBounds(0));
    problem.setStateInfo('/jointset/back/lumbar_extension/value', MocoBounds(deg2rad(-20), 0)...
        ,MocoInitialBounds(0), MocoFinalBounds(0));
else
    bound = getVariableMatrix(modelType);
    problem.setStateInfo(['/jointset/hip_' whichSide '/hip_flexion_' whichSide '/value']  , MocoBounds(bound(1,1), bound(1,2))...
        ,MocoInitialBounds(bound(1,3)), MocoFinalBounds(bound(1,4)));
    problem.setStateInfo(['/jointset/knee_' whichSide '/knee_flexion_' whichSide '/value'], MocoBounds(bound(2,1), bound(2,2))...
        ,MocoInitialBounds(bound(2,3)), MocoFinalBounds(bound(2,4)));
    problem.setStateInfo(['/jointset/ankle_' whichSide '/ankle_angle_' whichSide '/value'], MocoBounds(bound(3,1), bound(3,2))...
        ,MocoInitialBounds(bound(3,3)), MocoFinalBounds(bound(3,4)));
    problem.setStateInfo('/jointset/back/lumbar_extension/value', MocoBounds(bound(4,1), bound(4,2))...
        ,MocoInitialBounds(bound(4,3)), MocoFinalBounds(bound(4,4)));
end

if s.knee == 1
    problem.setStateInfo(['/jointset/knee_' whichSide '/knee_adduction_' whichSide '/value'], MocoBounds(deg2rad(-10), deg2rad(10)));
    problem.setStateInfo(['/jointset/knee_' whichSide '/knee_rotation_' whichSide '/value'], MocoBounds(deg2rad(-10), deg2rad(10)));
elseif s.knee == 2
    problem.setStateInfo(['/jointset/knee_' whichSide '/knee_adduction_' whichSide '/value'], MocoBounds(0, 0));
    problem.setStateInfo(['/jointset/knee_' whichSide '/knee_rotation_' whichSide '/value'], MocoBounds(0, 0));
elseif s.knee == 3
    problem.setStateInfo(['/jointset/knee_' whichSide '/knee_adduction_' whichSide '/value'], MocoBounds(deg2rad(-10), deg2rad(10))...
        ,MocoInitialBounds(0), MocoFinalBounds(0));
    problem.setStateInfo(['/jointset/knee_' whichSide '/knee_rotation_' whichSide '/value'], MocoBounds(deg2rad(-10), deg2rad(10))...
        ,MocoInitialBounds(0), MocoFinalBounds(0));
end

problem.setStateInfoPattern('/jointset/.*/speed', [], 0, 0);
% problem.setControlInfoPattern('/forceset/.*', [], 0, 0);

% Update Goals
filename = [modelType '_w_data.mat'];
if isfile(filename)
    loadedData = load(filename);
    w_array = loadedData.w_array;
else
    w_array = [];
end
w_array(end+1,:) = weights;
save(filename, 'w_array');
% writematrix(w_array,[modelType '_w_data.xlsx']);
iter = size(w_array,1);

w.com = weights(1);
w.bal = weights(2);
w.lig = weights(3);
w.acc = weights(4);
w.eff = weights(5:end);

problem.updGoal('height').setWeight(w.com);
problem.updGoal('balance').setWeight(w.bal);
problem.updGoal('ligament').setWeight(w.lig);
EffortGoal = MocoControlGoal.safeDownCast(problem.updGoal('effort'));
% EffortGoal.setWeight(w.eff);
EffortGoal.setWeight(1);
EffortGoal.setExponent(2);
fSet = model.getForceSet();
for i = 9:fSet.getSize()-1
    fName = fSet.get(i).getName();
    % disp("/forceset/"+char(fName))
    EffortGoal.setWeightForControl("/forceset/"+char(fName),w.eff(i-8));
end

% Configure the solver.
solver = study.initCasADiSolver();
solver.set_multibody_dynamics_mode("implicit");
solver.set_minimize_implicit_multibody_accelerations(true);
solver.set_implicit_multibody_accelerations_weight(w.acc);
% solver.set_minimize_implicit_auxiliary_derivatives(true);
% solver.set_implicit_auxiliary_derivatives_weight(0.001);
solver.set_num_mesh_intervals(s.meshVal);
solver.set_verbosity(2);
solver.set_optim_solver("ipopt");
if s.print == 0
    solver.set_optim_ipopt_print_level(4);
end
solver.set_optim_max_iterations(s.numIter);
solver.set_optim_convergence_tolerance(s.convTol);
% solver.set_optim_constraint_tolerance(s.constTol);

% Specify an initial guess.
guess = solver.createGuess("bounds");
guess.resampleWithNumTimes(2);
guess.setState("/jointset/ground_pelvis/pelvis_ty/value", [0.927, 0.930]);
guess.setState(['/jointset/hip_'   whichSide '/hip_flexion_'  whichSide '/value'], [bound(1,3), bound(1,4)]);
guess.setState(['/jointset/knee_'  whichSide '/knee_flexion_' whichSide '/value'], [bound(2,3), bound(2,4)]);
guess.setState(['/jointset/ankle_' whichSide '/ankle_angle_'  whichSide '/value'], [bound(3,3), bound(3,4)]);
guess.setState("/jointset/back/lumbar_extension/value", [bound(4,3), bound(4,4)]);
solver.setGuess(guess);
study.print([pardir '\SquatOpt_0.moco']);

% Solve the problem.
solution = study.solve();
solution.unseal();
resultPath = [pardir '\Results\' modelType '_sol.mot'];
solution.write(resultPath);
% disp(solution.getStatus());
% study.visualize(solution);

% Call the read function
[time, data] = readMotFile(resultPath);

end
%% Model Creation and Plotting Convenience Functions 

function addCoordinateActuator(model, coordName, optForce)

import org.opensim.modeling.*;

% Get the coordinate set from the model
coordSet = model.updCoordinateSet();

% Create a new CoordinateActuator
actu = CoordinateActuator();

% Set the name of the actuator
actu.setName(['tau_' coordName]);

% Retrieve the coordinate using its name
coord = coordSet.get(coordName); % Ensure coordName is a valid name

% Set the actuator's coordinate
actu.setCoordinate(coord);

% Set optimal force and control limits
actu.setOptimalForce(optForce);
actu.setMinControl(-1);
actu.setMaxControl(1);

% Add the actuator to the model
model.addForce(actu);
% disp(char(actu.getName())+": "+num2str(actu.getOptimalForce())+"N")
% disp(char(actu.getName()))

end

function model = getTorqueDrivenModel(modelName, optForce)

import org.opensim.modeling.*;
pardir = fileparts(pwd);

% Load the base model.
model = Model([pardir '\Models\' modelName]);

% Initialize the system
% model.updForceSet().clearAndDestroy();
model.initSystem();

% Get the coordinate set from the model
cSet = model.getCoordinateSet();
exList = ["pelvis_tilt", "pelvis_tx", "pelvis_ty"   ,...
          "knee_adduction_r"  , "knee_adduction_l"  ,...
          "knee_rotation_r"   , "knee_rotation_l"   ,...
          "patellar_flexion_r", "patellar_flexion_l"];
          % "hip_adduction_r"   , "hip_adduction_l"   ,...
          % "hip_rotation_r"    , "hip_rotation_l"    ,...,...
          % "subtalar_angle_r"  , "subtalar_angle_l"
for i = 0:cSet.getSize()-1
    coordName = cSet.get(i).getName();
    if ismember(char(coordName), exList)
        continue;
    % elseif strcmp(coordName, "lumbar_extension") || strcmp(coordName, "ankle_angle_l")
    %     addCoordinateActuator(model, char(coordName), 100);
    else
        addCoordinateActuator(model, char(coordName), optForce);
    end
end
end

function variableMatrix = getVariableMatrix(modelType)
    % [min, max, init, fin]
    expData = readmatrix([fileparts(pwd) '\IKResults\exp_' modelType '.xlsx']);
    init = expData(1,:);
    fin  = expData(end,:);
    variableMatrices = struct(...
        'K1L', [-2, 120, init(3), fin(3);      % hip
                 0, 120, init(4), fin(4);      % knee
                -1,  25, init(5), fin(5);      % ankle
               -55, -22, init(6), fin(6)],...  % lumbar
        'K2L', [ 0, 120, init(3), fin(3);      % hip
                 0, 120, init(4), fin(4);      % knee
                -3,  25, init(5), fin(5);      % ankle
               -25,   0, init(6), fin(6)],...  % lumbar
        'K3R', [ 0, 125, init(3), fin(3);      % hip
                 0, 125, init(4), fin(4);      % knee
                 0,  25, init(5), fin(5);      % ankle
               -25,   2, init(6), fin(6)],...  % lumbar
        'K5R', [ 0, 125, init(3), fin(3);      % hip
                 0, 125, init(4), fin(4);      % knee
                 0,  25, init(5), fin(5);      % ankle
               -45,  0 , init(6), fin(6)],...  % lumbar
        'K7L', [ 0, 125, init(3), fin(3);      % hip
                 0, 125, init(4), fin(4);      % knee
                 0,  25, init(5), fin(5);      % ankle
               -25,   0, init(6), fin(6)],...  % lumbar
        'K8L', [ 0, 125, init(3), fin(3);      % hip
                 0, 125, init(4), fin(4);      % knee
                -5,  25, init(5), fin(5);      % ankle
               -15,   0, init(6), fin(6)]...   % lumbar
             );
         
    fields = fieldnames(variableMatrices);  % Get the field names

    for i = 1:numel(fields)
        % Access each field, convert to radians, and assign back
        variableMatrices.(fields{i}) = deg2rad(variableMatrices.(fields{i}));
    end
    
    % Check if the model type is valid
    if isfield(variableMatrices, modelType)
        variableMatrix = variableMatrices.(modelType); % Return corresponding 4x4 matrix
    else
        error('Invalid model type. Please use one of the following: K1L, K2L, K3R, K5R, K7L, K8L');
    end
end

