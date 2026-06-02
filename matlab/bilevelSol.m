clear all; close all; clc;

subjects = {'K1L','K2L','K3R','K5R','K7L','K8L'};

%% File check
% for subNum = 1:6
%     resultDir = 'G:\My Drive\bilvlIOC\Individual\';
%     fileName = [subjects{subNum} '_solutions.mat'];
%     fullDir = [resultDir, fileName];
%     load(fullDir)
%     fileNum = dir([resultDir, '\', subjects{subNum}]);
%     if length(fileNum)-2 ~= length(solutions)
%         disp('files not match!')
%         load([resultDir 'K2L_solutions.mat'])
%         diffs = solutions(:,1:3);
% 
%         load("exp_ds_full.mat");
%         expMax = max(exp_ds{subNum}(:,2:4));
% 
%         for i = 118
%             [~,motTem] = readMotFile([resultDir, subjects{subNum}, '\', subjects{subNum}, '_sol_', num2str(i), '.mot'],22, 0);
%             simMax = max(rad2deg(motTem(:, [4, 9, 12])));
%             diffMax = simMax - expMax;
%             solMax = diffs(i,:);
% 
%             % if (solMax - diffMax) > 1e-5
%                 disp(['mismatch found at ' num2str(i) 'solMax: ' num2str(solMax) 'diffMax: ' num2str(diffMax)])
%             % end
%         end
%     else
%         disp('clear')
%     end
% end

%%
settings = {'individual', 'group'};

plot_traj   = 0;
plot_allv   = 0;
plot_allh   = 1;
plot_diff   = 0;
plot_rmse   = 0;
plot_pcor   = 0;
plot_nrmse  = 0;
plot_rsqr   = 0;
plot_all    = 0;
plot_weight = 0;

gridon = 1;

bestIndicesAll = [];
weights_all = {};
bilvlMetrics = struct();

for setting = 1
expTrajs = {};
simTrajs = {};
diffs = [];
rmses = [];
pcors = [];
nrmses= [];
rsqrs = [];

for best = 3  % 1:diff, 2:rmse, 3:total error

bestDir = {'diff\', 'rmse\', 'toterr\'};
bestIndices = [];
weights = [];
bestDirName = bestDir{best};
bilvlMetrics.(bestDirName(1:end-1)) = struct();

for subNum = 1:6

    resultDir = ['C:\Users\hojin\Documents\PhD\OpenSim 4.4\squatOpt\build\bilvlIOC\' settings{setting} '\'];

    if setting == 1
        fileName = [subjects{subNum} '_solutions.mat'];
        fullDir = [resultDir, fileName];
        
        load(fullDir)
        numIter = length(solutions);
        solutions(:, 9:20) = solutions(:, 7:end);
        solutions(:, 7) = sqrt(mean(solutions(:,1:3).^2,2)); % Diff
        solutions(:, 8) = sqrt(mean(solutions(:,4:6).^2,2)); % RMSE
        solutions(:,end+1) = 1:numIter;
        sol_diffsort = sortrows(solutions, 7);
        sol_rmsesort = sortrows(solutions, 8);
        sol_totesort = sortrows(solutions, 9);
        best_diffsort = sol_diffsort(1,end);
        best_rmsesort = sol_rmsesort(1,end);
        best_totesort = sol_totesort(1,end);    
        % if best == 1
        %     bestIdx = best_diffsort;
        % elseif best == 2
        %     bestIdx = best_rmsesort;
        % elseif best == 3
        %     bestIdx = best_totesort;
        % end
        if subNum == 2 || subNum == 4
            bestIdx = best_diffsort;
        else
            bestIdx = best_totesort;
        end
        bestIndices(end+1) = bestIdx;

        weight = solutions(bestIdx,10:end-1);
        weights(end+1,:) = weight;

    elseif setting == 2
        fileName = 'popOpt.mat';
        fullDir = [resultDir, fileName];

        load(fullDir)
        numIter = length(metricSave.weight);
        
        diffrmss = [];
        rmsermss = [];
        errors = [];
        for i = 1:6
            diffrms = sqrt(mean((metricSave.(subjects{i})(:,1:3).^2),2));
            rmserms = sqrt(mean((metricSave.(subjects{i})(:,4:6).^2),2));
            diffrmss(:,end+1) = diffrms;
            rmsermss(:,end+1) = rmserms;
        end
        diffrmsrms = sqrt(mean(diffrmss.^2,2));
        rmsermsrms = sqrt(mean(rmsermss.^2,2));
        errors(:,end+1) = diffrmsrms;
        errors(:,end+1) = rmsermsrms;
        errors(:,end+1) = metricSave.error;
        errors(:,end+1) = 1:numIter;
        sol_diffsort = sortrows(errors, 1);
        sol_rmsesort = sortrows(errors, 2);
        sol_totesort = sortrows(errors, 3);
        best_diffsort = sol_diffsort(1,end);
        best_rmsesort = sol_rmsesort(1,end);
        best_totesort = sol_totesort(1,end);
        if best == 1
            bestIdx = best_diffsort;
        elseif best == 2
            bestIdx = best_rmsesort;
        elseif best == 3
            bestIdx = best_totesort;
        end            
        bestIndices(end+1) = bestIdx;        

        weight = metricSave.weight(bestIdx,:);
        weights(end+1,:) = weight;
    end

    %% Exp
    load("exp_ds_full.mat");
    time = exp_ds{subNum}(:,1);
    expTraj = exp_ds{subNum}(:,2:4);
    expTrajs{end+1} = expTraj;

    expPeaks = [];
    expIndices = [];
    for i = 1:3
        [expPeak, expIdx] = max(expTraj(:,i));
        expPeaks(end+1) = expPeak;
        expIndices(end+1) = expIdx;
    end
    %% Sim
    motDir = [resultDir, subjects{subNum}];
    if setting == 2
    if subNum == 1 || subNum == 2 || subNum == 3
        bestIdx = bestIdx+1;
    else
        bestIdx = bestIdx;
    end
    end
    motName = [subjects{subNum} '_sol_' num2str(bestIdx) '.mot'];
    [~, simTrajAll] = readMotFile([motDir '\' motName], 22, 1);
    simTraj = rad2deg(simTrajAll(:, [4, 9, 12]));
    simTrajs{end+1} = simTraj;

    simPeaks = [];
    simIndices = [];
    for i = 1:3
        [simPeak, simIdx] = max(simTraj(:,i));
        simPeaks(end+1) = simPeak;
        simIndices(end+1) = simIdx;
    end

    %% Plot
    close all;

    tickSize = 12;
    % labelSize = 14;
    
    saveDir = [resultDir, bestDir{best}];
    set(groot, 'DefaultAxesFontWeight', 'bold', 'DefaultAxesFontSize', tickSize);
    
    if plot_traj == 1
    ftraj = figure('Units', 'normalized', 'OuterPosition', [0 0 0.4 0.8]);
    t = tiledlayout(3, 1, 'TileSpacing', 'Loose', 'Padding', 'Compact');
    
    for i = 1:3
        nexttile
        hold on
        if gridon == 1
            ax = gca;
            ax.XGrid = 'off';
            ax.YGrid = 'on';
        end
        plot(time, simTraj(:,i), 'r', 'LineWidth', 2)
        plot(time, expTraj(:,i), 'b', 'LineWidth', 2)
        simInd = time(simIndices(i));
        expInd = time(expIndices(i));
        if simInd < expInd
            simLabel = 'left';
            expLabel = 'right';
        else
            simLabel = 'right';
            expLabel = 'left';
        end            
        xline(simInd, '--r', 'LineWidth', 1.5, 'Label', 'Sim Peak', 'LabelOrientation', 'horizontal', 'LabelHorizontalAlignment',simLabel);
        xline(expInd, '--b', 'LineWidth', 1.5, 'Label', 'Exp Peak', 'LabelOrientation', 'horizontal', 'LabelHorizontalAlignment',expLabel);
        hold off
        xticks(0:10:100)
        xlim([0 100])
        minval = min(min(simTraj(:,i), min(expTraj(:,i))));
        maxval = max(max(simTraj(:,i), max(expTraj(:,i))));
        dist = maxval - minval;
        ylim([minval-dist*0.15 maxval+dist*0.15])
        xlabel('Squat Cycle (%)')
        if i == 1
            ylabel('Hip Flexion Angle (Deg)')
            legend({'Simulated', 'Experimental'}, 'Location', 'northwest')
        elseif i == 2
            ylabel('Knee Flexion Angle (Deg)')
        elseif i == 3
            ylabel('Ankle Angle (Deg)')
        end
    end
    saveas(ftraj, [saveDir subjects{subNum} '.fig'])
    exportgraphics(ftraj,[saveDir subjects{subNum} '.pdf'],'ContentType','vector')
    end
    %%
    peakDiff = abs(simPeaks - expPeaks);
    rmse = sqrt(mean((simTraj - expTraj).^2, 1));
    pcor = corr(simTraj, expTraj, 'Type', 'Pearson');
    nrmse = rmse ./ (max(expTraj) - min(expTraj));
    mean_exp = mean(expTraj, 1);
    SS_res = sum((expTraj - simTraj).^2, 1);  % residual sum of squares
    SS_tot = sum((expTraj - mean_exp).^2, 1); % total sum of squares
    rsqr = 1 - (SS_res ./ SS_tot);

    diffs(subNum,:) = peakDiff;
    rmses(subNum,:) = rmse;
    pcors(subNum,:) = diag(pcor)';
    nrmses(subNum,:)= nrmse;
    rsqrs(subNum,:) = rsqr;
end

bilvlMetrics.(bestDirName(1:end-1)).simTrajs = simTrajs;
bilvlMetrics.(bestDirName(1:end-1)).expTrajs = expTrajs;
bilvlMetrics.(bestDirName(1:end-1)).diffs = diffs;
bilvlMetrics.(bestDirName(1:end-1)).rmses = rmses;
bilvlMetrics.(bestDirName(1:end-1)).pcors = pcors;
bilvlMetrics.(bestDirName(1:end-1)).nrmses = nrmses;
bilvlMetrics.(bestDirName(1:end-1)).rsqrs = rsqrs;
bilvlMetrics.(bestDirName(1:end-1)).weights = weights;
bilvlMetrics.(bestDirName(1:end-1)).mean  = {};
bilvlMetrics.(bestDirName(1:end-1)).std  = {};

mean_diffs = mean(diffs, 1);
std_diffs = std(diffs, 0, 1);
bilvlMetrics.(bestDirName(1:end-1)).mean.diffs  = mean_diffs;
bilvlMetrics.(bestDirName(1:end-1)).std.diffs  = std_diffs;

mean_rmses = mean(rmses, 1);
std_rmses = std(rmses, 0, 1);
bilvlMetrics.(bestDirName(1:end-1)).mean.rmses  = mean_rmses;
bilvlMetrics.(bestDirName(1:end-1)).std.rmses  = std_rmses;

mean_pcors = mean(pcors, 1);
std_pcors = std(pcors, 0, 1);
bilvlMetrics.(bestDirName(1:end-1)).mean.pcors  = mean_pcors;
bilvlMetrics.(bestDirName(1:end-1)).std.pcors  = std_pcors;

mean_nrmses = mean(nrmses, 1);
std_nrmses = std(nrmses, 0, 1);
bilvlMetrics.(bestDirName(1:end-1)).mean.nrmses  = mean_nrmses;
bilvlMetrics.(bestDirName(1:end-1)).std.nrmses  = std_nrmses;

mean_rsqrs = mean(rsqrs, 1);
std_rsqrs = std(rsqrs, 0, 1);
bilvlMetrics.(bestDirName(1:end-1)).mean.rsqrs  = mean_rsqrs;
bilvlMetrics.(bestDirName(1:end-1)).std.rsqrs  = std_rsqrs;

bestIndicesAll(end+1,:) = bestIndices;
%% Plot
close all

set(groot, 'DefaultAxesFontWeight', 'bold', 'DefaultAxesFontSize', 12);
% colours = [0.9 0 0; 0 0 0.8; 0 0.65 0];

colours = [1 0.4 0.4;  % Soft Red
           0.4 0.6 1;  % Soft Blue
           0.5 0.9 0.5]; % Soft Green
contour_scale = 0.6;

% traj_vert
if plot_allv == 1
    maxSim = []; minSim = []; maxExp = []; minExp = [];
    for i = 1:6
        maxSim(end+1,:) = max(simTrajs{i});
        maxExp(end+1,:) = max(expTrajs{i});
        minSim(end+1,:) = min(simTrajs{i});
        minExp(end+1,:) = min(expTrajs{i});
    end
    maxSim = max(maxSim);
    maxExp = max(maxExp);
    minSim = min(minSim);
    minExp = min(minExp);
    ylimMax = max(max(maxSim,maxExp));
    ylimMin = min(min(minSim,minExp));

    fallt = figure('Units', 'normalized', 'OuterPosition', [0 0 0.45 1.0]);
    t = tiledlayout(3, 2, 'TileSpacing', 'Compact', 'Padding', 'Compact');
    for i = 1:6
        nexttile;
        plotHandles = [];
        for j = 1:3
            hold on
            if gridon == 1
                ax = gca;
                ax.XGrid = 'off';
                ax.YGrid = 'on';
            end
            p1 = plot(time, simTrajs{i}(:,j),'Color',colours(j,:),'LineWidth',2);
            p2 = plot(time, expTrajs{i}(:,j),'--','Color',colours(j,:),'LineWidth',2);
            plotHandles = [plotHandles, p1, p2];
            hold off
        end
        xlim([0, 100])
        ylim([ylimMin, 120])
        if i == 1
            leg = legend(plotHandles, {'Hip Sim', 'Hip Exp', 'Knee Sim', 'Knee Exp', 'Ankle Sim', 'Ankle Exp'});
            leg.Position = [0.079, 0.838, 0.14, 0.1];
        end
        title(subjects{i}, 'FontSize', 14, 'FontWeight', 'bold');
    end
    title(t, [upper(settings{setting}(1)) settings{setting}(2:end)...
        ' Cost Function Results - Trajectories'], 'FontSize', 14, 'FontWeight', 'bold');
    xlabel(t, 'Squat Cycle (%)', 'FontSize', 14, 'FontWeight', 'bold');
    ylabel(t, 'Joint Angle (degrees)', 'FontSize', 14, 'FontWeight', 'bold');

    saveas(fallt, [saveDir 'traj.fig'])
    exportgraphics(fallt,[saveDir 'traj.pdf'],'ContentType','vector')
end

% traj_horz
if plot_allh == 1
    maxSim = []; minSim = []; maxExp = []; minExp = [];
    for i = 1:6
        maxSim(end+1,:) = max(simTrajs{i});
        maxExp(end+1,:) = max(expTrajs{i});
        minSim(end+1,:) = min(simTrajs{i});
        minExp(end+1,:) = min(expTrajs{i});
    end
    maxSim = max(maxSim);
    maxExp = max(maxExp);
    minSim = min(minSim);
    minExp = min(minExp);
    ylimMax = max(max(maxSim,maxExp));
    ylimMin = min(min(minSim,minExp));

    fallt = figure('Units', 'normalized', 'OuterPosition', [0 0 0.5 0.6]);
    t = tiledlayout(2, 3, 'TileSpacing', 'Compact', 'Padding', 'Compact');
    for i = 1:6
        nexttile;
        plotHandles = [];
        for j = 1:3
            hold on
            if gridon == 1
                ax = gca;
                ax.XGrid = 'off';
                ax.YGrid = 'on';
            end
            p1 = plot(time, simTrajs{i}(:,j),'Color',colours(j,:),'LineWidth',2);
            p2 = plot(time, expTrajs{i}(:,j),'--','Color',colours(j,:),'LineWidth',2);
            plotHandles = [plotHandles, p1, p2];
            hold off
        end
        xlim([0, 100])
        ylim([ylimMin, 120])
        if i == 1
            leg = legend(plotHandles, {'Hip Sim', 'Hip Exp', 'Knee Sim', 'Knee Exp', 'Ankle Sim', 'Ankle Exp'});
            leg.Position = [0.069 0.803 0.11 0.135];
            leg.ItemTokenSize = [20, 20];
        end
        title(subjects{i}, 'FontSize', 14, 'FontWeight', 'bold');
    end
    % title(t, [upper(settings{setting}(1)) settings{setting}(2:end)...
    %     ' Cost Function Results - Trajectories'], 'FontSize', 14, 'FontWeight', 'bold');
    xlabel(t, 'Squat Cycle (%)', 'FontSize', 14, 'FontWeight', 'bold');
    ylabel(t, 'Joint Angle (degrees)', 'FontSize', 14, 'FontWeight', 'bold');
    
    set(findall(fallt, '-property', 'FontWeight'), 'FontWeight', 'bold');
    saveas(fallt, [saveDir 'traj_h.fig'])
    exportgraphics(fallt,[saveDir 'traj_h.pdf'],'ContentType','vector')
end

subjects_mean = {};
subjects_mean = subjects;
subjects_mean{end+1} = 'Mean';

% diff
if plot_diff == 1
fdiff = figure('Units', 'normalized', 'OuterPosition', [0 0.5 0.5 0.5]);
mean_diffs = mean(diffs, 1);
std_diffs = std(diffs, 0, 1);
data = [diffs; mean_diffs]; 
hold on;
if gridon == 1
    ax = gca;
    ax.XGrid = 'off';
    ax.YGrid = 'on';
end
b = bar(data, 'grouped', 'FaceColor', 'flat');
for i = 1:size(data, 2)
    b(i).CData = repmat(colours(i, :), size(data, 1), 1);
    b(i).EdgeColor = colours(i, :) * contour_scale;
end

for i = 1:size(data, 2)
    x_positions = b(i).XEndPoints;
    errorbar(x_positions(end), mean_diffs(i), std_diffs(i), 'k', 'linestyle', 'none', 'linewidth', 1.5);
end
hold off;
xticks(1:size(data, 1));
xticklabels(subjects_mean);
ylim([0, 20]);
ylabel('Apex Angle Difference (Deg)');
legend({'Hip', 'Knee', 'Ankle'}, 'Location', 'northwest');

saveas(fdiff, [saveDir 'diff.fig'])
exportgraphics(fdiff,[saveDir 'diff.pdf'],'ContentType','vector')
end

% rmse
if plot_rmse == 1
frmse = figure('Units', 'normalized', 'OuterPosition', [0 0.5 0.5 0.5]);
mean_rmses = mean(rmses, 1);
std_rmses = std(rmses, 0, 1);
data = [rmses; mean_rmses]; 
hold on;
if gridon == 1
    ax = gca;
    ax.XGrid = 'off';
    ax.YGrid = 'on';
end
b = bar(data, 'grouped', 'FaceColor', 'flat');
for i = 1:size(data, 2)
    b(i).CData = repmat(colours(i, :), size(data, 1), 1);
    b(i).EdgeColor = colours(i, :) * contour_scale;
end

for i = 1:size(data, 2)
    x_positions = b(i).XEndPoints;
    errorbar(x_positions(end), mean_rmses(i), std_rmses(i), 'k', 'linestyle', 'none', 'linewidth', 1.5);
end
hold off;
xticks(1:size(data, 1));
xticklabels(subjects_mean);
ylim([0, 20]);
ylabel('RMSE (Deg)')
legend({'Hip', 'Knee', 'Ankle'}, 'Location', 'northwest');
saveas(frmse, [saveDir 'rmse.fig'])
exportgraphics(frmse,[saveDir 'rmse.pdf'],'ContentType','vector')
end

% pcor
if plot_pcor == 1
fpcor = figure('Units', 'normalized', 'OuterPosition', [0.5 0 0.5 0.5]);
mean_pcors = mean(pcors, 1);
std_pcors = std(pcors, 0, 1);
data = [pcors; mean_pcors]; 
hold on;
if gridon == 1
    ax = gca;
    ax.XGrid = 'off';
    ax.YGrid = 'on';
end
b = bar(data, 'grouped', 'FaceColor', 'flat');
for i = 1:size(data, 2)
    b(i).CData = repmat(colours(i, :), size(data, 1), 1);
    b(i).EdgeColor = colours(i, :) * contour_scale;
end
for i = 1:size(data, 2)
    x_positions = b(i).XEndPoints; % Retrieve the x-coordinates for the grouped bars
    lower_bound = max(mean_pcors(i) - std_pcors(i), 0); % Ensure lower bound is >= 0
    upper_bound = min(mean_pcors(i) + std_pcors(i), 1); % Ensure upper bound is <= 1
    errorbar(x_positions(end), mean_pcors(i), mean_pcors(i) - lower_bound, upper_bound - mean_pcors(i), ...
        'k', 'LineStyle', 'none', 'LineWidth', 1.5); % Error bars with clamped bounds
end
hold off;
xticks(1:size(data, 1));
xticklabels(subjects_mean);
ylim([0, 1]);
ylabel('Pearson Correlation Coefficient')
legend({'Hip', 'Knee', 'Ankle'}, 'Location', 'southwest')
saveas(fpcor, [saveDir 'pcor.fig'])
exportgraphics(fpcor,[saveDir 'pcor.pdf'],'ContentType','vector')
end

% nrmse
if plot_nrmse == 1
fnrmse = figure('Units', 'normalized', 'OuterPosition', [0 0.5 0.5 0.5]);
mean_nrmses = mean(nrmses, 1);
std_nrmses = std(nrmses, 0, 1);

nrmses = nrmses * 100;
std_nrmses = std_nrmses * 100;
mean_nrmses = mean_nrmses * 100;
data = [nrmses; mean_nrmses];

hold on;
if gridon == 1
    ax = gca;
    ax.XGrid = 'off';
    ax.YGrid = 'on';
end
b = bar(data, 'grouped', 'FaceColor', 'flat');
for i = 1:size(data, 2)
    b(i).CData = repmat(colours(i, :), size(data, 1), 1);
    b(i).EdgeColor = colours(i, :) * contour_scale;
end

for i = 1:size(data, 2)
    x_positions = b(i).XEndPoints;
    errorbar(x_positions(end), mean_nrmses(i), std_nrmses(i), 'k', 'linestyle', 'none', 'linewidth', 1.5);
end
hold off;
xticks(1:size(data, 1));
xticklabels(subjects_mean);
ylim([0, 100])
ylabel('Normalised RMSE (%)')
legend({'Hip', 'Knee', 'Ankle'}, 'Location', 'northwest')
saveas(fnrmse, [saveDir 'nrmse.fig'])
exportgraphics(fnrmse,[saveDir 'nrmse.pdf'],'ContentType','vector')
end

% rsqr
if plot_rsqr == 1
frsqr = figure('Units', 'normalized', 'OuterPosition', [0 0.5 0.5 0.5]);
mean_rsqrs = mean(rsqrs, 1);
std_rsqrs = std(rsqrs, 0, 1);
data = [rsqrs; mean_rsqrs]; 
hold on;
if gridon == 1
    ax = gca;
    ax.XGrid = 'off';
    ax.YGrid = 'on';
end
b = bar(data, 'grouped', 'FaceColor', 'flat');
for i = 1:size(data, 2)
    b(i).CData = repmat(colours(i, :), size(data, 1), 1);
    b(i).EdgeColor = colours(i, :) * contour_scale;
end

for i = 1:size(data, 2)
    x_positions = b(i).XEndPoints;
    errorbar(x_positions(end), mean_rsqrs(i), std_rsqrs(i), 'k', 'linestyle', 'none', 'linewidth', 1.5);
end
hold off;
xticks(1:size(data, 1));
xticklabels(subjects_mean);
ylim([0, 1])
ylabel('Normalised RMSE')
legend({'Hip', 'Knee', 'Ankle'}, 'Location', 'northwest')
ylim([0, 1])
ylabel('R-Squared')
legend({'Hip', 'Knee', 'Ankle'}, 'Location', 'southwest')
saveas(frsqr, [saveDir 'rsqr.fig'])
exportgraphics(frsqr,[saveDir 'rsqr.pdf'],'ContentType','vector')


end

%% all metrics
if plot_all == 1
fall = figure('Units', 'normalized', 'OuterPosition', [0 0 1 1]);
t = tiledlayout(2, 2, 'TileSpacing', 'Compact', 'Padding', 'Compact');
set(fall, 'DefaultAxesFontSize', 12);
set(fall, 'DefaultTextFontSize', 12);

nexttile;
mean_diffs = mean(diffs, 1);
std_diffs = std(diffs, 0, 1);
data = [diffs; mean_diffs]; 
hold on;
if gridon == 1
    ax = gca;
    ax.XGrid = 'off';
    ax.YGrid = 'on';
end
b = bar(data, 'grouped', 'FaceColor', 'flat');
for i = 1:size(data, 2)
    b(i).CData = repmat(colours(i, :), size(data, 1), 1);
    b(i).EdgeColor = colours(i, :) * contour_scale;
end

for i = 1:size(data, 2)
    x_positions = b(i).XEndPoints;
    errorbar(x_positions(end), mean_diffs(i), std_diffs(i), 'k', 'linestyle', 'none', 'linewidth', 1.5);
end
hold off;
xticks(1:size(data, 1));
xticklabels(subjects_mean);
ylim([0, 20]);
xlabel('(a)')
ylabel('Apex Angle Difference (Deg)')
legend({'Hip', 'Knee', 'Ankle'}, 'Location', 'northwest');

nexttile;
mean_rmses = mean(rmses, 1);
std_rmses = std(rmses, 0, 1);
data = [rmses; mean_rmses]; 
hold on;
if gridon == 1
    ax = gca;
    ax.XGrid = 'off';
    ax.YGrid = 'on';
end
b = bar(data, 'grouped', 'FaceColor', 'flat');
for i = 1:size(data, 2)
    b(i).CData = repmat(colours(i, :), size(data, 1), 1);
    b(i).EdgeColor = colours(i, :) * contour_scale;
end

for i = 1:size(data, 2)
    x_positions = b(i).XEndPoints;
    errorbar(x_positions(end), mean_rmses(i), std_rmses(i), 'k', 'linestyle', 'none', 'linewidth', 1.5);
end
hold off;
xticks(1:size(data, 1));
xticklabels(subjects_mean);
ylim([0, 20]);
xlabel('(b)')
ylabel('RMSE (Deg)')
legend({'Hip', 'Knee', 'Ankle'}, 'Location', 'northwest');

nexttile;
mean_pcors = mean(pcors, 1);
std_pcors = std(pcors, 0, 1);
data = [pcors; mean_pcors]; 
hold on;
if gridon == 1
    ax = gca;
    ax.XGrid = 'off';
    ax.YGrid = 'on';
end
b = bar(data, 'grouped', 'FaceColor', 'flat');
for i = 1:size(data, 2)
    b(i).CData = repmat(colours(i, :), size(data, 1), 1);
    b(i).EdgeColor = colours(i, :) * contour_scale;
end
for i = 1:size(data, 2)
    x_positions = b(i).XEndPoints; % Retrieve the x-coordinates for the grouped bars
    lower_bound = max(mean_pcors(i) - std_pcors(i), 0); % Ensure lower bound is >= 0
    upper_bound = min(mean_pcors(i) + std_pcors(i), 1); % Ensure upper bound is <= 1
    errorbar(x_positions(end), mean_pcors(i), mean_pcors(i) - lower_bound, upper_bound - mean_pcors(i), ...
        'k', 'LineStyle', 'none', 'LineWidth', 1.5); % Error bars with clamped bounds
end
hold off;
xticks(1:size(data, 1));
xticklabels(subjects_mean);
ylim([0, 1]);
xlabel('(c)')
ylabel('Pearson Correlation Coefficient')
legend({'Hip', 'Knee', 'Ankle'}, 'Location', 'southwest')

nexttile;
mean_nrmses = mean(nrmses, 1);
std_nrmses = std(nrmses, 0, 1);

nrmses = nrmses * 100;
std_nrmses = std_nrmses * 100;
mean_nrmses = mean_nrmses * 100;
data = [nrmses; mean_nrmses];

hold on;
if gridon == 1
    ax = gca;
    ax.XGrid = 'off';
    ax.YGrid = 'on';
end
b = bar(data, 'grouped', 'FaceColor', 'flat');
for i = 1:size(data, 2)
    b(i).CData = repmat(colours(i, :), size(data, 1), 1);
    b(i).EdgeColor = colours(i, :) * contour_scale;
end

for i = 1:size(data, 2)
    x_positions = b(i).XEndPoints;
    errorbar(x_positions(end), mean_nrmses(i), std_nrmses(i), 'k', 'linestyle', 'none', 'linewidth', 1.5);
end
hold off;
xticks(1:size(data, 1));
xticklabels(subjects_mean);
ylim([0, 100])
xlabel('(d)')
ylabel('Normalised RMSE (%)')
legend({'Hip', 'Knee', 'Ankle'}, 'Location', 'northwest')
saveas(fall, [saveDir 'metrics.fig'])
exportgraphics(fall, [saveDir 'metrics.pdf'], 'ContentType','image','Resolution',300)
% exportgraphics(fall, [saveDir 'metrics.pdf'], 'ContentType', 'image', 'Resolution', 300, 'ContentMargins', [0 0 0 0])

end

end
% weights_all
weights_all{end+1} = weights;

save([resultDir 'bilevelMetrics.mat'], 'bilvlMetrics')
end

%% weights
% close all
% w_all = [weights_all{1};weights_all{2}(1,:)];
% subjects_all = [subjects, 'Group'];
% 
% if plot_weight == 1
% fw = figure('Units', 'normalized', 'OuterPosition', [0.25 0.25 0.5 0.5]);
% for i = 1:size(w_all,1)-1
% hold on
% scatter(1:11, w_all(i, :), 50, 'filled', 'DisplayName', subjects_all{i});
% end
% 
% plot(1:11, w_all(end,:),'X')
% hold off
% 
% minw = min(min(w_all));
% maxw = max(max(w_all));
% rangew = maxw - minw;
% xticks(1:11)
% wname = {'w_{com}', 'w_{bal}', 'w_{lig}', 'w_{acc}'...
%     , 'w_{eff,hip flex}', 'w_{eff,hip add}', 'w_{eff,hip rot}'...
%     , 'w_{eff,knee}', 'w_{eff,ankle}', 'w_{eff,subtalar}', 'w_{eff,lumbar}'};
% xticklabels(wname)
% xtickangle(45)
% xlim([0.5, 11.5])
% ylim([minw - rangew*0.1, maxw + rangew*0.1])
% ylabel('w')
% legend(subjects_all)
% saveas(fw, [saveDir 'weights.fig'])
% exportgraphics(fw,[saveDir 'weights.pdf'],'ContentType','vector')
% end