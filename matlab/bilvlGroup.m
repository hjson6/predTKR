clear all; close all; clc;

resultDir = 'G:\My Drive\bilvlIOC\';
load([resultDir 'popOpt.mat']);
fields = fieldnames(metricSave);
subjects = fields(1:6)';

diff_rms = [];
rmse_rms = [];

for i = 1:size(metricSave.error,1)
    diffs = [];
    rmses = [];
    totes = [];
    for subs = 1:6
        result = metricSave.(subjects{subs});        
        diff = result(i,1:3);
        rmse = result(i,4:6);
        diffs(end+1,:) = diff;
        rmses(end+1,:) = rmse;
    end    
    diff_rms(end+1,:) = sqrt(mean(diffs.^2, 'all'));
    rmse_rms(end+1,:) = sqrt(mean(rmses.^2, 'all'));
end
total_err = metricSave.error;

diff_rms(:,end+1) = 1:length(diff_rms);
rmse_rms(:,end+1) = 1:length(rmse_rms);
total_err(:,end+1) = 1:length(total_err);

%%
sort_diff = sortrows(diff_rms,1);
sort_rmse = sortrows(rmse_rms,1);
sort_tote = sortrows(total_err,1);

ranking = 1;

best_diff = sort_diff(ranking,2);
best_rmse = sort_rmse(ranking,2);
best_tote = sort_tote(ranking,2);

final_diffs = [];
final_rmses = [];

for i = 1:6
    final_data = metricSave.(subjects{i});
    final_diffs(i,1:3) = final_data(best_tote,1:3);
    final_rmses(i,1:3) = final_data(best_tote,4:6);
end

clc
disp(final_diffs)
disp(' ')
disp(final_rmses)