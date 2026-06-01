% Bi-level IOC using COBYLA to optimize weight factors
clear; close all; clc;

tic

pardir = fileparts(pwd);
addpath(genpath(pardir));

bilevel = 1;
%% Upper Level Setup
w_init = [2.5, 1, 1, 0.004, 0.5, 10, 1, 1, 5, 5, 1]; % [com, bal, lig,  acc,  effi(hip_flex, hip_add, hip_rot, knee, ankle, subtalar, lumbar_ext)] 
w_norm = w_init / sum(w_init);    

% PRIMA problem formualtion
if bilevel == 1
    p = struct();
    p.objective = @(w) upperLevelObjective(w);
    p.x0 = w_norm;  % Only optimize the first three elements
    p.lb = ones(1, length(w_norm)) * 1e-10;
    p.ub = ones(1, length(w_norm)) * (1 - 1e-10);
    p.nonlcon = @(w) w_con(w);  % Apply constraints to full vector
    p.options.rhobeg = 1e0;
    p.options.maxfun = 500;
    [optWeights, minError, exitflag, output] = prima(p);

    % Display results
    fprintf('Optimized Weights: \n');
    disp(optWeights);
    fprintf('Minimum Error: %.4f\n', minError);
    fprintf('Exit Flag: %.4f\n', exitflag);
    disp('Output: ')
    disp(output)
else
    upperLevelObjective(w_norm);
end

toc
%% Lower level Optimisation (MOCO)
function error = upperLevelObjective(weights)
    
    plotting = 0;

    tic

    if any(weights < 0)
        error = 1e30;
    else
        if isfile('popSol.mat')
            load('popSol.mat');
            row = size(errortot,1);
        else
            errortot = cell(1,9);
            row = 0;
        end
        errorRMStot = [];
        apexDifftot = [];
        rmsetot     = [];
        subjects = {'K1L', 'K2L', 'K3R', 'K5R', 'K7L', 'K8L'};
        s.meshVal  = 100;
        s.numIter  = 1e3;
        s.convTol  = 1e-4;
        s.optForce = 300;
        s.finTime  = 2;
        s.print    = 0; % 0: do not print; 1: print
        
        for sub = 1:6
            %% Run MOCO
            s.subject  = sub;
            [~, data, iter] = squatOpt(s, weights);
            lowerQsim = (data(:, [4, 9, 12]));  

            %% Load Experiment Data
            load('exp_ds_full.mat');
            exp_data  = exp_ds{s.subject};
            lowerQexp = deg2rad(exp_data(:,2:4));
            
            if plotting == 1
                hold on
                plot(rad2deg(lowerQsim))
                plot(rad2deg(lowerQexp))
                hold off
                xlim([0 200])
            end

            %% Compute Metrics
            apexQsim = max(lowerQsim);
            apexQexp = max(lowerQexp);
            apexDiff = apexQsim - apexQexp;
            apexDifftot(end+1,:) = apexDiff;
            RMSE  = sqrt(mean((lowerQsim - lowerQexp).^2));
            rmsetot(end+1,:) = RMSE;
            errorRMS = sqrt(mean([apexDiff.^2, RMSE.^2]));
            errorRMStot(end+1) = errorRMS;
            
            errortot{1,sub}(row+1,:) = [rad2deg(apexDiff), rad2deg(RMSE), rad2deg(errorRMS)];
            errortot{1,9}(row+1,sub) = toc;
            
            %% Copy .mot files                    
            srcPath  = [fileparts(pwd) '\Results\' subjects{s.subject} '_sol.mot'];
            dstDir   = [fileparts(pwd) '\Results\Group\' subjects{s.subject}]; 
            dstPath  = [dstDir '\' subjects{s.subject} '_sol_' num2str(iter) '.mot'];    
            if isfile(srcPath)
                if ~isfolder(dstDir)
                    mkdir(dstDir);
                end    
                copyfile(srcPath, dstPath);
            end
        end
        error = sqrt(mean(errorRMStot.^2));
        errortot{1,7}(row+1,:) = rad2deg(error);
        errortot{1,8}(row+1,:) = weights;
        save('popSol.mat','errortot');

        for i = 1:6
            disp(['iter' sprintf('%02d', iter) ' ' subjects{i} ...
                  ' Diff:[ ' sprintf('%+08.4f ', rad2deg(apexDifftot(i,1)), rad2deg(apexDifftot(i,2)), rad2deg(apexDifftot(i,3))) ']' ...
                  ', RMSE:[ ' sprintf('%+08.4f ', rad2deg(rmsetot(i,1)), rad2deg(rmsetot(i,2)), rad2deg(rmsetot(i,3))) ']' ...
                  ', error: ' sprintf('%07.4f', rad2deg(errorRMStot(i)))])
        end
        disp(['iter' sprintf('%02d', iter) ' total error: ' sprintf('%07.4f', rad2deg(error))])
        disp('')

        %% Save in Excel
        excelfilename = 'popSol.xlsx';
        sheetNames = {'K1L', 'K2L', 'K3R', 'K5R', 'K7L', 'K8L', 'error', 'weights', 'time'};
        headers = {{'diff_hip', 'diff_knee', 'diff_ankle', 'rmse_hip', 'rmse_knee', 'rmse_ankle', 'total_error'},...
                   {'total_error'},...
                   {'com', 'bal', 'lig', 'acc', 'hip_flex', 'hip_add', 'hip_rot', 'knee', 'ankle', 'subtalar', 'lumbar_ext'},...
                   {'K1L', 'K2L', 'K3R', 'K5R', 'K7L', 'K8L'},...
                   };
        for i = 1:length(errortot)
            sheetName = sheetNames{i};
            if i <= 6
                header = headers{1};
            elseif i == 7
                header = headers{2};
            elseif i == 8
                header = headers{3};
            elseif i == 9
                header = headers{4};
            end
            data = errortot{i};
            if ~isempty(data)
                writecell(header, excelfilename, 'Sheet', sheetName, 'Range', 'A1');
                writematrix(data, excelfilename, 'Sheet', sheetName, 'Range', 'A2');
            else
                writecell({'Empty'}, excelfilename, 'Sheet', sheetName);
            end
        end
    end
end

function [cineq, ceq] = w_con(weights)
    cineq = [];              % No inequality constraints
    ceq = sum(weights) - 1;  % The sum of weights equals 1
end
