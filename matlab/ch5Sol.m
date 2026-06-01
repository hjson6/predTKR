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

plot_traj = 0;
plot_allt = 0;
plot_diff = 0;
plot_rmse = 0;
plot_pcor = 0;
plot_nrmse= 1;
plot_rsqr = 0;

gridon = 1;

ch5Metrics = struct();

load("exp_ds_full.mat");

for setting = 2
expTrajs = {};
simTrajs = {};
diffs = [];
rmses = [];
pcors = [];
nrmses= [];
rsqrs = [];
for subNum  = 1:6
    
    if setting == 1
        bestIdx = subNum;
        dirType = [1,0,0,0,1,0];
    elseif setting == 2
        bestIdx = 6;
        dirType = [0,0,0,0,0,0];
    end

    %% Exp
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
    
    if dirType(subNum) == 0
        motDir = ['C:\Users\hojin\Documents\PhD\OpenSim 4.4\squatOpt\build\Results\' subjects{subNum}];
        motName = [subjects{subNum} '_sol_' num2str(bestIdx) '.mot'];
    elseif dirType(subNum) == 1
        weight.K1L = [0.004, 1, 2.6, 0.01];
        weight.K2L = [0.004, 4, 2.9, 0.01];
        weight.K3R = [0.004, 5, 3, 0.05];
        weight.K5R = [0.004, 5, 3, 0.05];
        weight.K7L = [0.004, 5, 2.8, 0.04];
        weight.K8L = [0.004, 5, 3, 0.05];
        motDir = ['C:\Users\hojin\Documents\PhD\OpenSim 4.4\squatOpt\build\Results\CAMS_Knee\' subjects{subNum}];
        motName = [subjects{subNum} '_a' num2str(weight.(subjects{subNum})(1))...
            'b' sprintf('%02d', weight.(subjects{subNum})(2))...
            'c' sprintf('%.1f', weight.(subjects{subNum})(3))...
            'l' num2str(weight.(subjects{subNum})(4)) '.mot'];
    end
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
    
    saveDir = ['C:\Users\hojin\Documents\PhD\OpenSim 4.4\squatOpt\build\Matlab\ch5results\' settings{setting} '\'];
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

ch5Metrics.simTrajs = simTrajs;
ch5Metrics.expTrajs = expTrajs;
ch5Metrics.diffs  = diffs;
ch5Metrics.rmses  = rmses;
ch5Metrics.pcors  = pcors;
ch5Metrics.nrmses = nrmses;
ch5Metrics.rsqrs  = rsqrs;
ch5Metrics.mean  = {};
ch5Metrics.std  = {};

mean_diffs = mean(diffs, 1);
std_diffs = std(diffs, 0, 1);
ch5Metrics.mean.diffs  = mean_diffs;
ch5Metrics.std.diffs  = std_diffs;

mean_rmses = mean(rmses, 1);
std_rmses = std(rmses, 0, 1);
ch5Metrics.mean.rmses  = mean_rmses;
ch5Metrics.std.rmses  = std_rmses;

mean_pcors = mean(pcors, 1);
std_pcors = std(pcors, 0, 1);
ch5Metrics.mean.pcors  = mean_pcors;
ch5Metrics.std.pcors  = std_pcors;

mean_nrmses = mean(nrmses, 1);
std_nrmses = std(nrmses, 0, 1);
ch5Metrics.mean.nrmses  = mean_nrmses;
ch5Metrics.std.nrmses  = std_nrmses;

mean_rsqrs = mean(rsqrs, 1);
std_rsqrs = std(rsqrs, 0, 1);
ch5Metrics.mean.rsqrs  = mean_rsqrs;
ch5Metrics.std.rsqrs  = std_rsqrs;
%% Plot
close all

set(groot, 'DefaultAxesFontWeight', 'bold', 'DefaultAxesFontSize', 12);
% colours = [0.9 0 0; 0 0 0.8; 0 0.65 0];
% Define slightly stronger soft colors
colours = [1 0.4 0.4;  % Soft Red
           0.4 0.6 1;  % Soft Blue
           0.5 0.9 0.5]; % Soft Green
contour_scale = 0.6;
% traj
if plot_allt == 1
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

subjects_mean = {};
subjects_mean = subjects;
subjects_mean{end+1} = 'Mean';

% diff
if plot_diff == 1
fdiff = figure('Units', 'normalized', 'OuterPosition', [0 0.5 0.5 0.5]);
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

save([saveDir 'ch5Metrics.mat'], 'ch5Metrics')

end


%%
% % Simulated and experimental trajectories for a joint
% close all;
% % Replace these with your actual data
% time = linspace(0, 0.5, 100); % Example time vector
% 
% maxCorr_all = [];
% bestLag_all = [];
% time_lags_all = {};
% corssCorr_all = {};
% for i = 1:3
% sim_trajectory = simTraj(:,i); % Example simulated trajectory
% exp_trajectory = expTraj(:,i); % Example experimental trajectory with a lag
% 
% % Perform cross-correlation
% [crossCorr, lags] = xcorr(sim_trajectory, exp_trajectory, 'coeff');
% 
% % Convert lags to time units
% sample_rate = 1 / mean(diff(time)); % Assuming uniform time steps
% time_lags = lags / sample_rate;
% 
% % Find the peak cross-correlation and corresponding time lag
% [maxCorr, maxIndex] = max(crossCorr);
% bestLag = time_lags(maxIndex);
% 
% maxCorr_all(end+1) = maxCorr;
% bestLag_all(end+1) =bestLag;
% time_lags_all{end+1} = time_lags;
% corssCorr_all{end+1} = crossCorr;
% end
% 
% hold on
% plot(time_lags_all{1}, corssCorr_all{1}, 'LineWidth', 1.5);
% plot(time_lags_all{2}, corssCorr_all{2}, 'LineWidth', 1.5);
% plot(time_lags_all{3}, corssCorr_all{3}, 'LineWidth', 1.5);
% hold off
% xlabel('Time Lag (s)');
% ylabel('Cross-Correlation Coefficient');
% title('Cross-Correlation Between Simulated and Experimental Trajectories');
% grid on;
% 
% % Display the best lag and maximum correlation
% disp(['Maximum Cross-Correlation: ', num2str(maxCorr_all)]);
% disp(['Best Lag (seconds): ', num2str(bestLag_all)]);
% clc
% fprintf('%s %0.2f %s %0.2f %s %0.2f %s %0.2f %s %0.2f %s %0.2f %s %0.2f %s %0.2f %s\n' ,...
%     'Apex Diff (Deg) &', indiv.mean.diffs(1), '$\pm$', indiv.std.diffs(1),...
%                     '&', indiv.mean.diffs(2), '$\pm$', indiv.std.diffs(2),...
%                     '&', indiv.mean.diffs(3), '$\pm$', indiv.std.diffs(3),...
%                     '&', mean(indiv.mean.diffs), '$\pm$', mean(indiv.std.diffs),...
%                     '\\ \hline');
% fprintf('%s %0.2f %s %0.2f %s %0.2f %s %0.2f %s %0.2f %s %0.2f %s %0.2f %s %0.2f %s\n' ,...
%     'RMSE (Deg) &', indiv.mean.rmses(1), '$\pm$', indiv.std.rmses(1),...
%                '&', indiv.mean.rmses(2), '$\pm$', indiv.std.rmses(2),...
%                '&', indiv.mean.rmses(3), '$\pm$', indiv.std.rmses(3),...
%                '&', mean(indiv.mean.rmses), '$\pm$', mean(indiv.std.rmses),...
%                '\\ \hline');
% fprintf('%s %0.2f %s %0.2f %s %0.2f %s %0.2f %s %0.2f %s %0.2f %s %0.2f %s %0.2f %s\n' ,...
%     'nRMSE (\%) &', indiv.mean.nrmses(1)*100, '$\pm$', indiv.std.nrmses(1)*100,...
%               '&', indiv.mean.nrmses(2)*100, '$\pm$', indiv.std.nrmses(2)*100,...
%               '&', indiv.mean.nrmses(3)*100, '$\pm$', indiv.std.nrmses(3)*100,...
%               '&', mean(indiv.mean.nrmses)*100, '$\pm$', mean(indiv.std.nrmses)*100,...
%               '\\ \hline');
% fprintf('%s %0.2f %s %0.2f %s %0.2f %s %0.2f %s %0.2f %s %0.2f %s %0.2f %s %0.2f %s\n' ,...
%     '$r$ &', indiv.mean.pcors(1), '$\pm$', indiv.std.pcors(1),...
%         '&', indiv.mean.pcors(2), '$\pm$', indiv.std.pcors(2),...
%         '&', indiv.mean.pcors(3), '$\pm$', indiv.std.pcors(3),...
%         '&', mean(indiv.mean.pcors), '$\pm$', mean(indiv.std.pcors),...
%         '\\ \hline');

%%
% clc
% diff_abs = group.mean.diffs - indiv.mean.diffs;
% diff_per = ((group.mean.diffs ./ indiv.mean.diffs) - 1) * 100;
% disp(indiv.mean.diffs)
% disp(group.mean.diffs)
% % disp(diff_abs)
% disp(diff_per)

% rmse_abs = group.mean.rmses - indiv.mean.rmses;
% rmse_per = ((group.mean.rmses ./ indiv.mean.rmses) - 1) * 100;
% disp(indiv.mean.rmses)
% disp(group.mean.rmses)
% disp(rmse_abs)
% disp(rmse_per)

% nrmse_abs = (group.mean.nrmses - indiv.mean.nrmses)*100;
% nrmse_per = ((group.mean.nrmses ./ indiv.mean.nrmses) - 1) * 100;
% disp(indiv.mean.nrmses*100)
% disp(group.mean.nrmses*100)
% disp(nrmse_abs)
% disp(nrmse_per)

% pcor_abs = group.mean.pcors - indiv.mean.pcors;
% pcor_per = ((group.mean.pcors ./ indiv.mean.pcors) - 1) * 100;
% disp(indiv.mean.pcors)
% disp(group.mean.pcors)
% disp(pcor_abs)
% disp(pcor_per)