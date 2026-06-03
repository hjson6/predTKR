clear; close all; clc;
pardir = fileparts(pwd);
addpath(genpath(pardir));
bestSim = {'K1L_sol_98.mot', 'K2L_sol_11.mot', 'K3R_sol_69.mot',...
           'K5R_sol_8.mot' , 'K7L_sol_91.mot', 'K8L_sol_90.mot'};

% bestSim = {'K5R_sol_03.mot', 'K5R_sol_04.mot', 'K5R_sol_07.mot'...
%            'K5R_sol_08.mot', 'K5R_sol_10.mot', 'K5R_sol_11.mot'...
%            'K5R_sol_15.mot', 'K5R_sol_24.mot', 'K5R_sol_39.mot'...
%            'K5R_sol_97.mot'};

    saveoption = 0;

for subject = 1:6
    
    parts = split(bestSim{subject},'_');
    [sim_time, sim_data] = readMotFile([pardir '\Results\' parts{1} '\' bestSim{subject}]);
    lowerQsim = rad2deg(sim_data(:, [4, 9, 12]));
    load('exp_ds_full.mat');
    exp_time = exp_ds{subject}(:,1);
    lowerQexp = exp_ds{subject}(:,2:4);
    % exp_time = exp_ds{4}(:,1);
    % lowerQexp = exp_ds{4}(:,2:4);
    
    apexSim = max(lowerQsim);
    apexExp = max(lowerQexp);
    apexDiff = abs(apexSim - apexExp);
    
    rmse  = sqrt(mean((lowerQsim - lowerQexp).^2));
    
    disp(['apex diff: ' num2str(apexDiff)])
    disp(['rmse:      ' num2str(rmse)])
    
    r = corr(lowerQsim, lowerQexp, 'Type', 'Pearson');
    pcorr = diag(r)';
    disp(['p coeff:   ' num2str(pcorr)])
    
    plotVertical(parts{1}, lowerQexp, lowerQsim, apexDiff, rmse, pcorr, saveoption)
    plotHorizontal(parts{1}, lowerQexp, lowerQsim, apexDiff, rmse, pcorr, saveoption)
end

%%
function plotVertical(subject, lowerQexp, lowerQsim, apexDiff, rmse, pcorr, saveoption)
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
    ylim(ax2, [0 10]);
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
    if any(rmse > 10)
        ylim(ax3, [0 15]);
    else
        ylim(ax3, [0 10]);
    end
    % title(ax3, 'RMSE');
    
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
        folderPath = fullfile(pwd, 'results');
        if ~exist(folderPath, 'dir')
            mkdir(folderPath);
        end
        
        saveas(gcf, [folderPath '\' subject '_vert.fig']);
        exportgraphics(gcf, [folderPath '\' subject '_vert.pdf'], 'ContentType', 'vector');
    end
end
function plotHorizontal(subject, lowerQexp, lowerQsim, apexDiff, rmse, pcorr, saveoption)
    figure;
    set(gcf, 'Position', [570, 100, 1000, 600]); % [left, bottom, width, height]
    tiledlayout(3, 5, 'TileSpacing', 'compact', 'Padding', 'compact');
    
    % Large plot spanning top 2 rows and all 3 columns
    ax1 = nexttile([3, 4]); % Spanning 2 rows and 3 columns
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
    ylabel(ax1, 'Joint Trajectories (deg)', 'FontWeight','bold');
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
    barColors = [hipColor; kneeColor; ankleColor];
    for i = 1:length(apexDiff)
        hold on
        b = bar(ax2, i, apexDiff(i));
        b.FaceColor = 'flat';
        b.CData = barColors(i, :);
    end
    hold off
    ylabel(ax2, 'Apex Difference (deg)', 'FontWeight','bold');
    ylim(ax2, [0 10]);
    % title(ax2, 'Apex Diff');
    
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
    if any(rmse > 10)
        ylim(ax3, [0 15]);
    else
        ylim(ax3, [0 10]);
    end
    % title(ax3, 'RMSE');
    
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
    % title(ax4, 'Pearson''s Correlation');
    
    sgtitle(['Subject - ' subject], 'FontWeight','bold');
    
    if saveoption == 1
        folderPath = fullfile(pwd, 'results');
        if ~exist(folderPath, 'dir')
            mkdir(folderPath);
        end
        
        saveas(gcf, [folderPath '\' subject '_horz.fig']);
        exportgraphics(gcf, [folderPath '\' subject '_horz.pdf'], 'ContentType', 'vector');
    end
end