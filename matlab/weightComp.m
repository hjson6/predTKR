clear all; close all; clc;

dirs = {'C:\Users\hojin\Documents\PhD\OpenSim 4.4\squatOpt\build\Matlab\ch5results',...
        'C:\Users\hojin\Documents\PhD\OpenSim 4.4\squatOpt\build\bilvlIOC'};
settings = {'Individual', 'group'};
subjects = {'K1L', 'K2L', 'K3R', 'K5R', 'K7L', 'K8L'};

setting  = 1;
saveplot = 1;

saveDir = [dirs{2} '\' settings{setting} '\toterr\'];

w_ch5indiv = [ 2.50, 1.0, 0.05, 0.004, 1, 1, 1, 1, 1, 1, 1 ;
               2.90, 4.0, 0.01, 0.004, 1, 1, 1, 1, 1, 1, 1 ;
               3.00, 2.0, 0.05, 0.004, 1, 1, 1, 1, 1, 1, 1 ;
               2.80, 5.0, 0.01, 0.004, 1, 1, 1, 1, 1, 1, 1 ;
               2.80, 5.0, 0.04, 0.004, 1, 1, 1, 1, 1, 1, 1 ;
               2.80, 1.0, 0.03, 0.004, 1, 1, 1, 1, 1, 1, 1 ];
for i = 1:6
    wsum = sum(w_ch5indiv(i,:));
    w_ch5indiv(i,:) = w_ch5indiv(i,:)./wsum;
end
w_ch5group = [ 2.80, 1.0, 0.03, 0.004, 1, 1, 1, 1, 1, 1, 1 ];
w_ch5group = w_ch5group./sum(w_ch5group);

ch6indiv = load([dirs{2} '\' settings{1} '\bilevelMetrics.mat']).bilvlMetrics.toterr;
ch6group = load([dirs{2} '\' settings{2} '\bilevelMetrics.mat']).bilvlMetrics.toterr;
w_ch6indiv = ch6indiv.weights;
w_ch6group = ch6group.weights(1,:);

%% Re-normalise
nw_ch5group = w_ch5group./sum(w_ch5group);
nw_ch5indiv = w_ch5indiv;
nw_ch6group = w_ch6group./sum(w_ch6group);
nw_ch6indiv = [];
for i = 1:6
    nw_ch6indiv(i,:) = w_ch6indiv(i,:)./sum(w_ch6indiv(i,:));
end

%%
% close all
% 
% fwindiv = figure('Units', 'normalized', 'OuterPosition', [0 0 0.4 0.8]);
% t = tiledlayout(3, 2, 'TileSpacing', 'compact', 'Padding', 'compact');
% 
% labels = {'w_{com}','w_{bal}','w_{lig}','w_{acc}', ...
%           'w_{eff,hip\_flex}','w_{eff,hip\_add}','w_{eff,hip\_rot}', ...
%           'w_{eff,knee}','w_{eff,subtalar}','w_{eff,ankle}','w_{eff,lumbar}'};
% 
% windiv5 = w_ch5indiv;
% windiv6 = w_ch6indiv;
% windiv5(:,3:4) = windiv5(:,3:4)*100;
% windiv6(:,3:4) = windiv6(:,3:4)*100;
% 
% for i = 1:6
%     nexttile;
%     bar([windiv5(i,:); windiv6(i,:)]', 'grouped', 'BarWidth', 1)
%     xticks(1:11)
%     xticklabels(labels)
%     xtickangle(90)
%     ylim([0, 0.55])
%     ylabel('Weight Factor')
%     title(subjects{i})
%     legend('Grid Search', 'Bi-Level IOC', 'Location', 'northeast')
% end
% 
% sgtitle('Individual Setting Comparison','FontWeight', 'bold', 'FontSize', 16)
% 
% if saveplot == 1
% saveas(fwindiv, [saveDir, 'windiv.fig'])
% exportgraphics(fwindiv,[saveDir, 'windiv.pdf'],'ContentType','vector')
% end

%%
% fwgroup = figure(2);
% 
% wgroup5 = w_ch5group;
% wgroup6 = w_ch6group;
% wgroup5(:,3:4) = wgroup5(:,3:4)*100;
% wgroup6(:,3:4) = wgroup6(:,3:4)*100;
% 
% labels = {'w_{com}','w_{bal}','w_{lig}','w_{acc}', ...
%           'w_{eff,hip\_flex}','w_{eff,hip\_add}','w_{eff,hip\_rot}', ...
%           'w_{eff,knee}','w_{eff,subtalar}','w_{eff,ankle}','w_{eff,lumbar}'};
% 
% labels{3} = [labels{3} '(x 10^{-2})'];
% labels{4} = [labels{4} '(x 10^{-2})'];
% 
% bar([wgroup5; wgroup6]', 'grouped', 'BarWidth', 1)
% xticks(1:11)
% xticklabels(labels)
% xtickangle(90)
% ylim([0, 0.3])
% ylabel('Weight Factor')
% title('Group Setting Comparison','FontWeight', 'bold', 'FontSize', 12)
% legend('Grid Search', 'Bi-Level IOC', 'Location', 'northeast')
% 
% if saveplot == 1
% saveas(fwgroup, [saveDir, 'wgroup.fig'])
% exportgraphics(fwgroup,[saveDir, 'wgroup.pdf'],'ContentType','vector')
% end

%%
% % Create figure with normalized size
% fweights = figure('Units', 'normalized', 'OuterPosition', [0.5 0.1 0.5 1.0]);
% t = tiledlayout(4, 3, 'TileSpacing', 'compact', 'Padding', 'compact');
% 
% % Define weight factor labels
% labels = {'w_{com}','w_{balance}','w_{tension}','w_{acc}', ...
%           'w_{effort,hip\_flex}','w_{effort,hip\_add}',...
%           'w_{effort,hip\_rot}', 'w_{effort,knee}',...
%           'w_{effort,subtalar}','w_{effort,ankle}',...
%           'w_{effort,lumbar}'};
% 
% windiv5 = nw_ch5indiv;
% windiv6 = nw_ch6indiv;
% w5 = windiv5;
% w6 = windiv6;
% 
% % Loop through weight factors (11 total subplots)
% for i = 1:11
%     nexttile;
% 
%     % Prepare data for bar chart (transpose so each group represents a subject)
%     data = [w5(:,i), w6(:,i)];
% 
%     % Plot grouped bars
%     b = bar(data, 'grouped', 'BarWidth', 1);
% 
%     % Formatting
%     xticks(1:7)
%     xticklabels(subjects)
%     xtickangle(90)
%     ylim([0, max(data,[],'all')*1.1])
%     title(labels{i})
% 
%     % Add legend to the last tile and move it outside
%     if i == 11
%         lgd = legend(b, {'Grid Search', 'Bi-Level IOC'}, 'FontWeight','bold');
%         lgd.Position = [0.73, 0.075, 0.2, 0.1];
%         lgd.ItemTokenSize = [50, -10];
%     end
% end
% ylabel(t,'Weight Factor','FontWeight', 'bold', 'FontSize', 16)
% sgtitle('Individual Setting Comparison','FontWeight', 'bold', 'FontSize', 16)
% 
% if saveplot == 1
% saveas(fweights, [saveDir, 'weights.fig'])
% exportgraphics(fweights,[saveDir, 'weights.pdf'],'ContentType','vector')
% end

%%
% % Create figure with normalized size
% fweights_all = figure('Units', 'normalized', 'OuterPosition', [0.5 0.1 0.5 1.0]);
% t = tiledlayout(4, 3, 'TileSpacing', 'compact', 'Padding', 'compact');
% 
% % Define weight factor labels
% labels = {'w_{com}','w_{balance}','w_{tension}','w_{acc}', ...
%           'w_{effort,hip\_flex}','w_{effort,hip\_add}',...
%           'w_{effort,hip\_rot}', 'w_{effort,knee}',...
%           'w_{effort,subtalar}','w_{effort,ankle}',...
%           'w_{effort,lumbar}'};
% subs = [subjects, 'Group'];
% 
% wgroup5 = nw_ch5group;
% wgroup6 = nw_ch6group;
% windiv5 = nw_ch5indiv;
% windiv6 = nw_ch6indiv;
% w5_all = [windiv5;wgroup5];
% w6_all = [windiv6;wgroup6];
% 
% % Loop through weight factors (11 total subplots)
% for i = 1:11
%     nexttile;
% 
%     % Prepare data for bar chart (transpose so each group represents a subject)
%     data = [w5_all(:,i), w6_all(:,i)];
% 
%     % Plot grouped bars
%     b = bar(data, 'grouped', 'BarWidth', 1);
% 
%     % Formatting
%     xticks(1:7)
%     xticklabels(subs)
%     xtickangle(90)
%     ax = gca;
%     ax.FontSize = 12;
%     ax.XAxis.FontWeight = 'bold';
%     ax.YAxis.FontWeight = 'bold';
% 
%     ylim([0, max(data,[],'all')*1.1])
%     title(labels{i})
% 
%     % Add legend to the last tile and move it outside
%     if i == 11
%         lgd = legend(b, {'Grid Search', 'Bi-Level IOC'}, 'FontWeight','bold');
%         lgd.Position = [0.73, 0.075, 0.2, 0.1];
%         lgd.ItemTokenSize = [50, 10];
%     end
% end
% ylabel(t,'Weight Factor','FontWeight', 'bold', 'FontSize', 16)
% 
% if saveplot == 1
% saveas(fweights_all, [saveDir, 'weights_all.fig'])
% exportgraphics(fweights_all,[saveDir, 'weights_all.pdf'],'ContentType','vector')
% end
%%
% Create figure with normalized size
fweights_all = figure('Units', 'normalized', 'OuterPosition', [0.1 0.1 0.9 1.0]);
t = tiledlayout(3, 4, 'TileSpacing', 'compact', 'Padding', 'compact');

% Define weight factor labels
labels = {'w_{com}','w_{balance}','w_{tension}','w_{acc}', ...
          'w_{effort,hip\_flex}','w_{effort,hip\_add}',...
          'w_{effort,hip\_rot}', 'w_{effort,knee}',...
          'w_{effort,subtalar}','w_{effort,ankle}',...
          'w_{effort,lumbar}'};
subs = [subjects, 'Group'];

wgroup5 = nw_ch5group;
wgroup6 = nw_ch6group;
windiv5 = nw_ch5indiv;
windiv6 = nw_ch6indiv;
w5_all = [windiv5;wgroup5];
w6_all = [windiv6;wgroup6];

fprintf('%s %s %s %s %s %s %s %s %s %s %s %s\n', ...
    '    ', 'w_com','w_balance','w_tension','w_acc', ...
          'w_effort_hip_flex','w_effort_hip_add',...
          'w_effort_hip_rot', 'w_effort_knee',...
          'w_effort_subtalar','w_effort_ankle',...
          'w_effort_lumbar')
for i = 1:7
    fprintf('%s % 0.4f % 0.4f % 0.4f % 0.4f % 0.4f % 0.4f % 0.4f % 0.4f % 0.4f % 0.4f % 0.4f\n', ...
        subs{i}, w6_all(i,1), w6_all(i,2), w6_all(i,3), w6_all(i,4), w6_all(i,5), w6_all(i,6), ...
        w6_all(i,7), w6_all(i,8), w6_all(i,9), w6_all(i,10), w6_all(i,11));
end

% Loop through weight factors (11 total subplots)
for i = 1:11
    nexttile;
    
    % Prepare data for bar chart (transpose so each group represents a subject)
    data = w6_all(:,i);
    
    % Plot grouped bars
    b = bar(data, 'grouped', 'BarWidth', 1);
    
    % Formatting
    xticks(1:7)
    xticklabels(subs)
    xtickangle(90)
    ax = gca;
    ax.FontSize = 20;
    ax.XAxis.FontWeight = 'bold';
    ax.YAxis.FontWeight = 'bold';

    ylim([0, max(data,[],'all')*1.1])
    title(labels{i})
    
    % % Add legend to the last tile and move it outside
    % if i == 11
    %     lgd = legend(b, {'Grid Search', 'Bi-Level IOC'}, 'FontWeight','bold');
    %     lgd.Position = [0.73, 0.075, 0.2, 0.1];
    %     lgd.ItemTokenSize = [50, 10];
    % end
end
% ylabel(t,'Weight Factor','FontWeight', 'bold', 'FontSize', 16)

if saveplot == 1
saveas(fweights_all, [saveDir, 'weights_all.fig'])
exportgraphics(fweights_all,[saveDir, 'weights_all.pdf'],'ContentType','vector')
end

%% group vs individual distance
% clc
% close all
% ds = [];
% for i = 1:6
%     d = sqrt(sum((nw_ch6indiv(i,:)-nw_ch6group).^2));
%     ds(end+1) = d;
% end
% disp(ds)
% figure(1);
% hold on
% % fds = scatter(1:6,ds*10,'filled');
% % fdiff = scatter(1:6,mean(ch6indiv.diffs,2)','filled');
% % frmse = scatter(1:6,mean(ch6indiv.rmses,2)','filled');
% fds = plot(1:6,ds*10,'LineWidth',2);
% % fdiff = plot(1:6,mean(ch6indiv.diffs,2),'LineWidth',2);
% % frmse = plot(1:6,mean(ch6indiv.rmses,2),'LineWidth',2);
% fdiff = plot(1:6,ch6indiv.diffs(:,2)','LineWidth',2);
% frmse = plot(1:6,ch6indiv.rmses(:,2)','LineWidth',2);
% fnrmse = plot(1:6,ch6indiv.nrmses(:,2)'.*10,'LineWidth',2);
% 
% hold off
% xticks(1:6)
% xticklabels(subjects)
% legend([fds,fdiff,frmse,fnrmse], {'ds', 'diff', 'rmse', 'nrmse'}, 'FontWeight','bold');
% 
% wgrep = repmat(nw_ch6group, size(nw_ch6indiv, 1), 1);
% rdmat = abs(nw_ch6indiv - wgrep) ./ abs(wgrep);
% rds = mean(rdmat, 2)';
% disp(rds)
% figure(2)
% hold on
% % frds = scatter(1:6,rds*10,'filled');
% % fdiff = scatter(1:6,mean(ch6indiv.diffs,2)','filled');
% % frmse = scatter(1:6,mean(ch6indiv.rmses,2)','filled');
% frds = plot(1:6,rds*10,'LineWidth',2);
% % fdiff = plot(1:6,mean(ch6indiv.diffs,2),'LineWidth',2);
% % frmse = plot(1:6,mean(ch6indiv.rmses,2),'LineWidth',2);
% fdiff = plot(1:6,ch6indiv.diffs(:,2)','LineWidth',2);
% frmse = plot(1:6,ch6indiv.rmses(:,2)','LineWidth',2);
% fnrmse = plot(1:6,ch6indiv.nrmses(:,2)'.*10,'LineWidth',2);
% hold off
% xticks(1:6)
% xticklabels(subjects)
% legend([frds,fdiff,frmse,fnrmse], {'rds', 'diff', 'rmse', 'nrmse'}, 'FontWeight','bold');
% 
% %%
% 
% obj = [];
% obj_com = [];
% obj_bal = [];
% obj_lig = [];
% obj_acc = [];
% obj_eff = [];
% 
% for i = 1:6
% sub = i;
% if sub == 1
%     objective=0.516810;
%     objective_accelerations=0.039118;
%     objective_balance=0.002183;
%     objective_effort=0.043343;
%     objective_height=0.420517;
%     objective_ligament=0.011649;
% elseif sub == 2
%     objective=0.430937;
%     objective_accelerations=0.050918;
%     objective_balance=0.006994;
%     objective_effort=0.031449;
%     objective_height=0.340428;
%     objective_ligament=0.001149;
% elseif sub == 3
%     objective=0.545440;
%     objective_accelerations=0.041525;
%     objective_balance=0.003767;
%     objective_effort=0.040479;
%     objective_height=0.450241;
%     objective_ligament=0.009429;
% elseif sub == 4
%     objective=0.334359;
%     objective_accelerations=0.036344;
%     objective_balance=0.001163;
%     objective_effort=0.027726;
%     objective_height=0.267655;
%     objective_ligament=0.001471;
% elseif sub == 5
%     objective=0.471632;
%     objective_accelerations=0.059545;
%     objective_balance=0.000919;
%     objective_effort=0.016585;
%     objective_height=0.385317;
%     objective_ligament=0.009266;
% elseif sub == 6
%     objective=0.445869;
%     objective_accelerations=0.057087;
%     objective_balance=0.003712;
%     objective_effort=0.023062;
%     objective_height=0.356457;
%     objective_ligament=0.005552;
% end
% 
% obj(end+1) = objective;
% obj_com(end+1) = objective_height;
% obj_bal(end+1) = objective_balance;
% obj_lig(end+1) = objective_ligament;
% obj_acc(end+1) = objective_accelerations;
% obj_eff(end+1) = objective_effort;
% end
% 
% objs = [obj;obj_com;obj_bal;obj_lig;obj_acc;obj_eff];
% 
% %% 44 correlation
% clc
% % diff_subdiff = mean(ch6indiv.diffs,2);
% % diff_subrmse = mean(ch6indiv.rmses,2);
% % diff_subnrmse = mean(ch6indiv.nrmses,2)*100;
% diff_subdiff = ch6indiv.diffs(:,2);
% diff_subrmse = ch6indiv.rmses(:,2);
% diff_subnrmse = ch6indiv.nrmses(:,2)*100;
% error_metrics = [diff_subdiff,diff_subrmse,diff_subnrmse];
% 
% [rho, pval] = corr(nw_ch6indiv, error_metrics);
% disp(rho)
% 
% figure;
% heatmap(rho, 'Colormap', parula);
% xlabel('Error Metrics');
% ylabel('Weight Factors');
% title('Correlation Between Weight Factors and Error Metrics');
% 
% % Define weight and error metric labels
% weight_labels = {'w_com', 'w_bal', 'w_lig', 'w_acc', 'w_eff_hipflex', ...
%                  'w_eff_hipadd', 'w_eff_hiprot', 'w_eff_knee', 'w_eff_subtalar', ...
%                  'w_eff_ankle', 'w_eff_lumbar'};
% 
% error_labels = {'Apex Diff', 'RMSE', 'NRMSE'};
% 
% % Create table
% corr_table = array2table(rho, 'RowNames', weight_labels, 'VariableNames', error_labels);
% 
% % Display in MATLAB
% disp(corr_table);
% 
% %%
% clc
% close all
% mean_weights = mean(nw_ch6indiv);
% std_weights = std(nw_ch6indiv);
% disp(table(mean_weights', std_weights', 'VariableNames', {'Mean', 'StdDev'}, 'RowNames', weight_labels));
% [rho_w, pval_w] = corr(nw_ch6indiv);
% figure;
% heatmap(rho_w, 'Colormap', parula);
% title('Correlation Between Weight Factors');
% for i = 1:6
%     temp_weights = nw_ch6indiv;
%     temp_errors = error_metrics;
%     temp_weights(i, :) = [];
%     temp_errors(i, :) = [];
%     [rho_loo, ~] = corr(temp_weights, temp_errors);
%     disp(['Leave-One-Out Subject ', num2str(i), ' Correlation']);
%     disp(rho_loo);
% end
