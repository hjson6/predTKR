clear; close all; clc;
pardir = fileparts(pwd);
addpath(genpath(pardir));
subjects = {'K1L', 'K1L', 'K3R', 'K5R', 'K7L', 'K8L'};

saveoption = 0;
iter = 1;
metrics.apexDiff = [];
metrics.rmse = [];
metrics.pcorr = [];
metrics.Qexp = {};
metrics.Qsim = {};

for subject = 1:6
    parts = split(subjects{subject},'_');
    [sim_time, sim_data] = readMotFile([pardir '\Results\Group\' parts{1} '\' parts{1} '_sol_' num2str(iter) '.mot']);
    lowerQsim = rad2deg(sim_data(:, [4, 9, 12]));
    metrics.Qsim{subject} = lowerQsim;
    load('exp_ds_full.mat');
    exp_time = exp_ds{subject}(:,1);
    lowerQexp = exp_ds{subject}(:,2:4);
    metrics.Qexp{subject} = lowerQexp;
    
    apexSim = max(lowerQsim);
    apexExp = max(lowerQexp);
    apexDiff = abs(apexSim - apexExp);
    metrics.apexDiff(end+1,:) = apexDiff;
    disp(['apex diff: ' num2str(apexDiff)])
    
    rmse  = sqrt(mean((lowerQsim - lowerQexp).^2));
    metrics.rmse(end+1,:) = rmse;
    disp(['rmse:      ' num2str(rmse)])
    
    r = corr(lowerQsim, lowerQexp, 'Type', 'Pearson');
    pcorr = diag(r)';
    metrics.pcorr(end+1,:) = pcorr;
    disp(['p coeff:   ' num2str(pcorr)])
end

plotGroup(metrics, saveoption)
plotMetrics(metrics, saveoption)

%%
function plotGroup(metrics, saveoption)
    
    % Define tiled layout for 2x3 grid
    figure;
    set(gcf, 'Position', [100, 50, 1200, 800]); % Adjust figure size
    tiledlayout(2, 3, 'TileSpacing', 'compact', 'Padding', 'compact');
    
    for i = 1:6
        % Metrics
        lowerQexp = metrics.Qexp{i};
        lowerQsim = metrics.Qsim{i};
        rmse      = metrics.rmse(i,:);
        pcorr     = metrics.pcorr(i,:);
        apexDiff  = metrics.apexDiff(i,:);
        
        % Add a subplot
        ax = nexttile;
        hold on;
        
        % Define colors for joints
        hipColor   = [0.0000 0.4470 0.7410]; % Blue  (default MATLAB color)
        kneeColor  = [0.8500 0.3250 0.0980]; % Red   (default MATLAB color)
        ankleColor = [0.4660 0.6740 0.1880]; % Green (default MATLAB color)
        
        % Plot lowerQexp (dotted lines)
        plot(0:0.5:100, lowerQexp(:,1), '--', 'Color', hipColor, 'LineWidth', 1.5); % Hip
        plot(0:0.5:100, lowerQexp(:,2), '--', 'Color', kneeColor, 'LineWidth', 1.5); % Knee
        plot(0:0.5:100, lowerQexp(:,3), '--', 'Color', ankleColor, 'LineWidth', 1.5); % Ankle
        
        % Plot lowerQsim (solid lines)
        plot(0:0.5:100, lowerQsim(:,1), '-', 'Color', hipColor, 'LineWidth', 1.5); % Hip
        plot(0:0.5:100, lowerQsim(:,2), '-', 'Color', kneeColor, 'LineWidth', 1.5); % Knee
        plot(0:0.5:100, lowerQsim(:,3), '-', 'Color', ankleColor, 'LineWidth', 1.5); % Ankle
        hold off;
        
        xlim([0 100]);
        ylim([0 120]);
        
        % Add labels, title, and legend for each subplot
        xlabel('Time (%)');
        ylabel('Joint Trajectories (deg)', 'FontWeight', 'bold');
        subjects = {'K1L', 'K1L', 'K3R', 'K5R', 'K7L', 'K8L'};
        title([subjects{i}], 'FontWeight', 'bold');
        if i == 1
        legend({'Hip (Exp)', 'Knee (Exp)', 'Ankle (Exp)', ...
                'Hip (Sim)', 'Knee (Sim)', 'Ankle (Sim)'}, ...
                'Location', 'northwest', 'FontSize', 8);
        end
    end
    
    % Add a common title for all subplots
    sgtitle('Joint Trajectories Across Subjects', 'FontWeight', 'bold');
        
    if saveoption == 1
        folderPath = fullfile(pwd, 'results');
        
        if ~exist(folderPath, 'dir')
            mkdir(folderPath);
        end
        
        saveas(gcf, fullfile(folderPath, 'traj.fig'));
        exportgraphics(gcf, fullfile(folderPath, 'traj.pdf'), 'ContentType', 'vector');
    end

end

function plotMetrics(metrics,saveoption)

    % Define data
    subjects = {'K1L', 'K1L', 'K3R', 'K5R', 'K7L', 'K8L'};
    hipColor   = [0.0000 0.4470 0.7410]; % Blue (default MATLAB color)
    kneeColor  = [0.8500 0.3250 0.0980]; % Red (default MATLAB color)
    ankleColor = [0.4660 0.6740 0.1880]; % Green (default MATLAB color)
    barColors  = [hipColor; kneeColor; ankleColor];
    
    % Data for each metric
    apexDiff = metrics.apexDiff; % Replace with your actual data
    rmse     = metrics.rmse;     % Replace with your actual data
    pcorr    = metrics.pcorr;    % Replace with your actual data
    
    % Create a tiled layout
    figure;
    set(gcf, 'Position', [100, 100, 600, 800]); % Adjust figure size
    t = tiledlayout(3, 1, 'TileSpacing', 'compact', 'Padding', 'compact');
    
    % Plot for apexDiff
    nexttile;
    b1 = bar(apexDiff, 'FaceColor', 'flat');
    for k = 1:3
        b1(k).CData = repmat(barColors(k, :), size(apexDiff, 1), 1);
    end
    xticks(1:length(subjects));
    xticklabels(subjects);
    ylabel('Apex Diff (deg)', 'FontWeight', 'bold');
    title('Apex Difference', 'FontWeight', 'bold');
    
    % Plot for rmse
    nexttile;
    b2 = bar(rmse, 'FaceColor', 'flat');
    for k = 1:3
        b2(k).CData = repmat(barColors(k, :), size(rmse, 1), 1);
    end
    xticks(1:length(subjects));
    xticklabels(subjects);
    ylabel('RMSE (deg)', 'FontWeight', 'bold');
    title('Root Mean Squared Error', 'FontWeight', 'bold');
    
    % Plot for pcorr
    nexttile;
    b3 = bar(pcorr, 'FaceColor', 'flat');
    for k = 1:3
        b3(k).CData = repmat(barColors(k, :), size(pcorr, 1), 1);
    end
    xticks(1:length(subjects));
    xticklabels(subjects);
    ylabel('Correlation Coefficient (r)', 'FontWeight', 'bold');
    title('Pearson Correlation', 'FontWeight', 'bold');
    ylim([0 1]); % Adjust as needed for pcorr range
    
    % Add legend to the entire figure
    legend({'Hip', 'Knee', 'Ankle'}, 'Location', 'southoutside', 'Orientation', 'horizontal', 'FontWeight', 'bold');
    t.Padding = 'compact'; % Ensure tight padding
    t.TileSpacing = 'compact'; % Reduce spacing between tiles

    if saveoption == 1
        folderPath = fullfile(pwd, 'results');
        
        if ~exist(folderPath, 'dir')
            mkdir(folderPath);
        end
        
        saveas(gcf, fullfile(folderPath, 'traj.fig'));
        exportgraphics(gcf, fullfile(folderPath, 'traj.pdf'), 'ContentType', 'vector');
    end

end