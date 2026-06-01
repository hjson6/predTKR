% Bi-level IOC using COBYLA to optimize weight factors
clear; close all; clc;

tic

pardir = fileparts(pwd);
addpath(genpath(pardir));

s.meshVal  = 100;
s.numIter  = 1e3;
s.convTol  = 1e-4;
% s.constTol = s.convTol;
s.optForce = 300;
s.finTime  = 2;
s.plotting = 1;
s.print    = 1; % 0: do not print, 1: print
% for con = 1:2
s.knee     = 3; % 1: uncon, 2: con, 3: con init&fin
s.rots     = 1;

bilevel = 0;
%% Upper Level Setup
% started 7/12 3:32PM
for lig = 0.02
s.subject  = 1;
subjects   = {'K1L', 'K2L', 'K3R', 'K5R', 'K7L', 'K8L'};
s.subname  = subjects{s.subject};
w_init     = [];
% Initial guess for weight factors
%            [ com,  bal, lig,  acc,   effi(hip_flex, hip_add, hip_rot, knee, ankle, subtalar, lumbar_ext)]
w_init.K1L = [ 3.5, 3, lig, 0.004, 1, 1, 1, 1, 1, 1, 1 ];
% w_init.K1L = [ 2.50, 1.0, lig, 0.004, 0.90, 1, 1, 1, 1, 1, 1 ];
w_init.K2L = [ 3.00, 3.5, 0.01, 0.004, 1.40, 1, 1, 1, 1, 1, 1 ];
w_init.K3R = [ 3.00, 2.0, 0.05, 0.004, 1.00, 1, 1, 1, 1, 1, 1 ];
w_init.K5R = [ 2.80, 7.0, 0.01, 0.004, 0.90, 1, 1, 1, 1, 1, 1 ];
w_init.K7L = [ 2.60, 1.0, 0.04, 0.004, 0.90, 1, 1, 1, 1, 1, 1 ];
w_init.K8L = [ 2.80, 3.4, 0.02, 0.004, 1.05, 1, 1, 1, 1, 1, 1 ];

w_init = w_init.(s.subname);
% w_norm = w_init / sum(w_init);
w_norm = w_init;
disp(w_init)
% disp(w_norm)

if bilevel == 1
    % PRIMA problem formualtion
    % p = struct();
    % p.objective = @upperLevelObjective;
    % p.x0 = w_norm;
    % p.lb = ones(1, length(w_norm)).*1e-10;
    % p.ub = ones(1, length(w_norm)).*(1-1e-10);
    % p.nonlcon = @w_con;
    % p.options.rhobeg = 1e-1;
    % p.options.maxfun = 500;
    % [optWeights, minError, exitflag, output] = prima(p);
    p = struct();
    p.objective = @(w) upperLevelObjective(w,s);
    p.x0 = w_norm;  % Only optimize the first three elements
    p.lb = ones(1, length(w_norm)) * 1e-10;
    p.ub = ones(1, length(w_norm)) * (1 - 1e-10);
    p.nonlcon = @(w) w_con(w);  % Apply constraints to full vector
    p.options.rhobeg = 5e-1;
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
    upperLevelObjective(w_norm,s);
end

disp(w_init)

% end
end
toc
%% Lower level Optimisation (MOCO)
function plotVertical(subject, lowerQexp, lowerQsim, apexDiff, rmse, pcorr, iter, saveoption)
    % Define tiled layout with 3 rows and 3 columns
    figure;
    set(gcf, 'Position', [0, 100, 600, 600]); % [left, bottom, width, height]
    tiledlayout(3, 3, 'TileSpacing', 'compact', 'Padding', 'compact');
    
    % Large plot spanning top 2 rows and all 3 columns
    ax1 = nexttile([2, 3]); % Spanning 2 rows and 3 columns
    hold on
    % Define colors for joints
    hipColor = [0 0.4470 0.7410];    % Blue (default MATLAB color)
    kneeColor = [0.8500 0.3250 0.0980]; % Red (default MATLAB color)
    ankleColor = [0.4660 0.6740 0.1880]; % Green (default MATLAB color)
    
    % Plot lowerQexp (dotted lines)
    plot(0:0.5:100, lowerQexp(:,1), '--', 'Color', hipColor, 'LineWidth', 1.5); % Hip
    plot(0:0.5:100, lowerQexp(:,2), '--', 'Color', kneeColor, 'LineWidth', 1.5); % Knee
    plot(0:0.5:100, lowerQexp(:,3), '--', 'Color', ankleColor, 'LineWidth', 1.5); % Ankle
    
    % Plot lowerQsim (solid lines)
    plot(0:0.5:100, lowerQsim(:,1), '-', 'Color', hipColor, 'LineWidth', 1.5); % Hip
    plot(0:0.5:100, lowerQsim(:,2), '-', 'Color', kneeColor, 'LineWidth', 1.5); % Knee
    plot(0:0.5:100, lowerQsim(:,3), '-', 'Color', ankleColor, 'LineWidth', 1.5); % Ankle
    hold off
    xlim([0 100])
    
    % Add labels, legend, and title
    xlabel(ax1, 'Time (%)');
    ylabel(ax1, 'Joint Trajectories (deg)', 'FontWeight', 'bold');
    legend(ax1, {'Hip (Exp)', 'Knee (Exp)', 'Ankle (Exp)', ...
                 'Hip (Sim)', 'Knee (Sim)', 'Ankle (Sim)'}, ...
                 'Location', 'northwest');
    % title(ax1, 'Joint Trajectories');
    
    % Smaller individual plots in the last row
    % Apex Diff
    ax2 = nexttile; % First tile in the last row
    bar(ax2, apexDiff, 'FaceColor', 'flat');
    xticks(ax2, 1:3);
    xticklabels(ax2, {'Hip', 'Knee', 'Ankle'});
    % set(ax2, 'FontWeight', 'bold');
    barColors = [hipColor; kneeColor; ankleColor];
    for i = 1:length(apexDiff)
        hold on
        b = bar(ax2, i, apexDiff(i));
        b.FaceColor = 'flat';
        b.CData = barColors(i, :);
    end
    hold off
    ylabel(ax2, 'Apex Difference (deg)', 'FontWeight','bold');
    if (max(apexDiff)<=10)
        ylim(ax2, [0 10]);
    end
    % title(ax2, 'Apex Difference');
    
    % RMSE
    ax3 = nexttile; % Second tile in the last row
    bar(ax3, rmse, 'FaceColor', 'flat');
    xticks(ax3, 1:3);
    xticklabels(ax3, {'Hip', 'Knee', 'Ankle'});
    for i = 1:length(rmse)
        hold on
        b = bar(ax3, i, rmse(i));
        b.FaceColor = 'flat';
        b.CData = barColors(i, :);
    end
    hold off
    ylabel(ax3, 'RMSE (deg)', 'FontWeight','bold');
    if (max(rmse) <= 10)
        ylim(ax3, [0 10]);
    end
    
    % Pearson's Correlation
    ax4 = nexttile; % Third tile in the last row
    bar(ax4, pcorr, 'FaceColor', 'flat');
    xticks(ax4, 1:3);
    xticklabels(ax4, {'Hip', 'Knee', 'Ankle'});
    for i = 1:length(pcorr)
        hold on
        b = bar(ax4, i, pcorr(i));
        b.FaceColor = 'flat';
        b.CData = barColors(i, :);
    end
    hold off
    ylim(ax4, [0 1]);
    ylabel(ax4, 'Correlation Coefficient, r', 'FontWeight','bold');
    % title(ax4, 'Pearson''s Correlation Coefficient');
    
    sgtitle(['Subject - ' subject], 'FontWeight','bold');
    
    if saveoption == 1
        folderPath = fullfile(pwd, 'results_0');
        if ~exist(folderPath, 'dir')
            mkdir(folderPath);
        end
        
        saveas(gcf, [folderPath '\' subject '_vert_' num2str(iter) '.fig']);
        exportgraphics(gcf, [folderPath '\' subject '_vert_' num2str(iter) '.png']);
        exportgraphics(gcf, [folderPath '\pdf\' subject '_vert_' num2str(iter) '.pdf'], 'ContentType', 'vector');
    end
end

function error = upperLevelObjective(weights,s)
    
    tic

    if any(weights < 0)
        error = 1e30;
    else
        % Run MOCO
        [~, data, iter] = squatOpt(s, weights);
        % lowerQsim = (data(:, [4, 9, 10]));
        lowerQsim = (data(:, [4, 9, 12]));
        
        % Load Experiment Data
        load('exp_ds.mat');
        exp_data  = exp_ds{s.subject};
        lowerQexp = deg2rad(exp_data(:,2:4));
        
        % Apex Joint Angle Difference
        apexQsim = max(lowerQsim);
        apexQexp = max(lowerQexp);
        apexDiff = apexQsim - apexQexp;
        % ApexDiffAbs_norm = apexDiffAbs / max(apexDiffAbs);  % Normalize apexDiffAbs
    
        % RMSE
        RMSE  = sqrt(mean((lowerQsim - lowerQexp).^2));
        % RMSE_norm = RMSE / max(RMSE);  % Normalize RMSE
        
        % Pearson's correlation coefficients
        r = corr(lowerQsim, lowerQexp, 'Type', 'Pearson');
        pcorr = diag(r);
        
        % Total Error
        errorRMS = sqrt(mean([apexDiff.^2, RMSE.^2]));
        % errorRMS = sqrt(mean(apexDiffAbs.^2));
        % totalNormalizedError  = mean([RMSE_norm, ApexDiffAbs_norm]);

        if s.plotting == 1
            saveoption = 1;
            plotVertical(s.subname, rad2deg(lowerQexp),...
                rad2deg(lowerQsim), rad2deg(abs(apexDiff)),...
                rad2deg(RMSE), pcorr, iter, saveoption)
        end
            
        % Save some good solutions
        % if any(apexDiffAbs < apexThreshold) || any(RMSE < rmseThreshold)
        filename = [s.subname '_solutions.mat'];
        sol = [rad2deg(apexDiff), rad2deg(RMSE), rad2deg(errorRMS), weights];
        if isfile(filename)
            loadedData = load(filename);
            solutions  = loadedData.solutions;
        else
            solutions  = [];
        end    
        solutions(end+1, :) = sol;
        save(filename, 'solutions');
        writematrix(solutions,[s.subname '_solution.xlsx']);
                
        srcPath  = [fileparts(pwd) '\Results\' s.subname '_sol.mot'];
        dstDir   = [fileparts(pwd) '\Results\' s.subname]; 
        dstPath  = [dstDir '\' s.subname '_sol_' num2str(iter) '.mot'];    
        if isfile(srcPath)
            if ~isfolder(dstDir)
                mkdir(dstDir);
            end    
            copyfile(srcPath, dstPath);
        end
        
        % Time taken for each MOCO run
        timefilename = [s.subname '_time.mat'];
        if isfile(timefilename)
            loadtime = load(timefilename);
            time = loadtime.time;
        else
            time = [];
        end
        time(end+1, 1) = toc;
        save(timefilename,'time');
        writematrix(time,[s.subname '_time.xlsx']);
    
        disp(['weights: ' num2str(weights)])
        disp(['error:   ' num2str(rad2deg(errorRMS))])
        disp(['A_Diff:  ' num2str(rad2deg(apexDiff))])
        disp(['RMSE:    ' num2str(rad2deg(RMSE))])

        error = errorRMS;
        toc
    end
end

function [cineq, ceq] = w_con(weights)
    cineq = [];              % No inequality constraints
    ceq = sum(weights) - 1;  % The sum of weights equals 1
end
