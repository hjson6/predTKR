# Bi-level IOC Squat Optimization

This repository contains MATLAB scripts for running and post-processing a bi-level inverse optimal control (IOC) workflow for squat motion simulation using OpenSim/Moco. The workflow compares simulated hip, knee, and ankle trajectories against experimental squat trajectories and estimates cost-function weight factors for individual-subject and group-level settings.

## Before you start: install OpenSim

OpenSim must be installed and configured before running these scripts. Follow the official OpenSim download/setup and MATLAB scripting instructions for your operating system and OpenSim version:

- [OpenSim core repository](https://github.com/opensim-org/opensim-core)
- [OpenSim download and setup information](https://github.com/opensim-org/opensim-core#download-and-setup)
- [OpenSim MATLAB/Python scripting setup](https://github.com/opensim-org/opensim-core#download-and-setup)
- [OpenSim Moco documentation](https://opensim-org.github.io/opensim-moco-site/docs/)

After installation, verify that MATLAB can access the OpenSim Java API, for example:

```matlab
import org.opensim.modeling.*
```

If you are using OpenSim Moco, also verify that the Moco MATLAB interface is available, for example:

```matlab
org.opensim.modeling.opensimMoco.GetMocoVersionAndDate()
```


Experimental trajectories used by the upper-level objective are assumed to have already been generated from CAMS-Knee raw data through OpenSim inverse kinematics.

## Main files

1. `squatOpt.m` solves the lower-level OpenSim/Moco squat optimization for one subject and one set of weights.
2. `weightOpt.m` evaluates or optimizes subject-specific weight factors.
3. `weightOptPop.m` optimizes one shared population/group-level weight vector across all subjects.
4. `bilevelSol.m` selects the best saved bi-level IOC result and computes performance metrics.
5. `weightComp.m` compares recovered weight factors between settings.
 
The main workflow is:

```text
weightOpt.m / weightOptPop.m
        |
        v
    squatOpt.m
        |
        v
 simulated .mot files and saved solution metrics
        |
        v
 bilevelSol.m / weightComp.m
```

## Required dependencies

The scripts are intended for a MATLAB + OpenSim/Moco workflow. The main dependencies are:

- MATLAB
- [OpenSim / OpenSim core](https://github.com/opensim-org/opensim-core), with MATLAB scripting configured
- [OpenSim Moco](https://opensim-org.github.io/opensim-moco-site/docs/)
- [PRIMA](https://github.com/libprima/prima), used here through the `prima` MATLAB function for derivative-free upper-level optimization
- Custom OpenSim/Moco goal plugins, compiled as DLLs (in \matlab\RelWithDebInfo):
  - `osimMocoHeightGoal.dll`
  - `osimMocoBalanceGoal.dll`
  - `osimMocoBalanceGoalRight.dll`
  - `osimMocoLigamentGoal.dll`
- Project-specific helper functions, for example:
  - `readMotFile.m`
- Project-specific data and model files:
  - OpenSim models in `models/`
  - saved result folders such as `Results/` and `bilvlIOC/`

## Data preparation

This workflow uses experimental squat trajectories derived from the [CAMS-Knee dataset](https://cams-knee.orthoload.com/). Access to the original CAMS-Knee data is managed by the dataset provider and may require a data-use agreement.

To reproduce the analyses, first obtain the CAMS-Knee data and process it locally through OpenSim inverse kinematics:

```text
CAMS-Knee experimental data
        |
        v
OpenSim inverse kinematics
        |
        v
processed joint-angle trajectories
        |
        v
exp_ds_full.mat and IKResults/
        |
        v
IOC analysis scripts
```

The scripts assume that the processed trajectories are already available, for example as `exp_ds_full.mat` and files in `IKResults/`. These files should contain the subject-specific hip, knee, and ankle trajectories used for comparison with the simulated results.


## Weight vector

The scripts use an 11-element weight vector. The first four entries correspond to the main objective terms:

```text
[w_com, w_balance, w_ligament, w_acceleration, ... effort weights ...]
```

The remaining entries are effort weights assigned to the model controls. In the current scripts, the intended order is approximately:

```text
[w_eff_hip_flexion,
 w_eff_hip_adduction,
 w_eff_hip_rotation,
 w_eff_knee,
 w_eff_ankle,
 w_eff_subtalar,
 w_eff_lumbar]
```

The exact control assignment is handled in `squatOpt.m` by looping through the OpenSim model `ForceSet`, so the force/control order should be checked whenever the model is modified.

## Running an individual-subject analysis

Use `weightOpt.m` for individual-subject analysis.

Important settings to check near the top of the file:

```matlab
s.subject  = 1;      % subject index
bilevel    = 0;      % 0: evaluate current weights, 1: optimize weights
s.meshVal  = 100;
s.numIter  = 1e3;
s.convTol  = 1e-4;
s.optForce = 300;
s.finTime  = 2;
s.knee     = 3;
s.rots     = 1;
```

Typical usage:

```matlab
run weightOpt.m
```

When `bilevel = 0`, the script evaluates the current manually specified weight vector. When `bilevel = 1`, it runs PRIMA/COBYLA to optimize the weight vector.

Main outputs include:

```text
<subject>_solutions.mat
<subject>_solution.xlsx
<subject>_time.mat
<subject>_time.xlsx
Results/<subject>/<subject>_sol_<iteration>.mot
```

## Running a group-level analysis

Use `weightOptPop.m` for population/group-level IOC.

This script optimizes one shared weight vector across all six subjects:

```matlab
subjects = {'K1L', 'K2L', 'K3R', 'K5R', 'K7L', 'K8L'};
for sub = 1:6
    s.subject = sub;
    [~, data, iter] = squatOpt(s, weights);
end
```

Typical usage:

```matlab
run weightOptPop.m
```

Main outputs include:

```text
popSol.mat
popSol.xlsx
Results/Group/<subject>/<subject>_sol_<iteration>.mot
```

The group-level error is computed from the subject-level errors and returned to the upper-level optimizer.

## Post-processing bi-level IOC results

Use `bilevelSol.m` after running individual or group IOC.

This script:

1. loads saved solution files,
2. selects the best solution according to a chosen criterion,
3. reads the corresponding `.mot` files,
4. compares simulated and experimental trajectories,
5. computes metrics, and
6. saves the processed result structure.

The selection criterion is controlled by:

```matlab
for best = 3  % 1: apex-difference error, 2: RMSE, 3: total error
```

The setting is controlled by:

```matlab
for setting = 1  % 1: individual, 2: group
```

Main output:

```text
bilvlIOC/individual/bilevelMetrics.mat
bilvlIOC/group/bilevelMetrics.mat
```

depending on the selected setting.

The saved metrics include:

```text
diffs   : apex flexion angle differences
rmses   : root mean square errors
pcors   : Pearson correlation coefficients
nrmses  : normalized RMSE values
rsqrs   : R-squared values
weights : selected IOC weight factors
```

## Comparing weight factors

Use `weightComp.m` after generating `bilevelMetrics.mat` for both individual and group settings.

This script:

1. loads individual and group bi-level IOC weights,
2. normalizes the weight vectors,
3. compares them with manually selected or grid-search weights,
4. plots weight factors for all subjects and the group setting.

Typical usage:

```matlab
run weightComp.m
```

Main outputs include:

```text
weights_all.fig
weights_all.pdf
```

## Notes and common issues

### 1. OpenSim must be configured before MATLAB scripts will run

If MATLAB cannot find `org.opensim.modeling`, OpenSim has not been configured correctly for MATLAB. Revisit the official OpenSim/Moco setup instructions.

### 2. Custom Moco goal DLLs must be available

`squatOpt.m` loads custom goal plugins using `opensimCommon.LoadOpenSimLibraryExact`. Make sure the DLL files exist in the expected build folder before running the optimization.

### 3. Prepare CAMS-Knee-derived trajectories first

The optimization scripts expect processed joint-angle trajectories. If `exp_ds_full.mat` or the files in `IKResults/` are missing, obtain the CAMS-Knee data, run OpenSim inverse kinematics, and generate these files locally.


### 4. Check paths before running

Some scripts assume a specific folder structure and use Windows-style path separators. If you clone the repository on another machine, update paths such as:

```matlab
models/
IKResults/
Results/
bilvlIOC/
RelWithDebInfo/
```

### 5. Check active loop values

Some scripts are intentionally configured to run only one subject or one setting. For example:

```matlab
for setting = 1
for best = 3
s.subject = 1
```

Update these values if you want to run all subjects, individual and group settings, or different selection criteria.

### 6. Recommended `.gitignore` entries

Generated data and result files can be large, so they are usually better kept outside version control:

```gitignore
exp_ds_full.mat
IKResults/
Results/
bilvlIOC/
*.mot
*.sto
*.fig
*.xlsx
```

## References and citations

If this repository is used for a publication, thesis, or report, cite the relevant external software and papers used by the workflow.

### OpenSim

This repository uses OpenSim/OpenSim Moco for musculoskeletal modelling and optimal control.

- OpenSim core repository: [opensim-org/opensim-core](https://github.com/opensim-org/opensim-core)
- OpenSim documentation and setup: [OpenSim download and setup](https://github.com/opensim-org/opensim-core#download-and-setup)

Recommended OpenSim citation:

```bibtex
@article{Seth2018OpenSim,
  title   = {OpenSim: Simulating musculoskeletal dynamics and neuromuscular control to study human and animal movement},
  author  = {Seth, Ajay and Hicks, Jennifer L. and Uchida, Thomas K. and Habib, Ayman and Dembia, Christopher L. and Dunne, James J. and Ong, Carmichael F. and DeMers, Matthew S. and Rajagopal, Apoorva and Millard, Matthew and Hamner, Samuel R. and Arnold, Edith M. and Yong, Jennifer R. and Lakshmikanth, Swathi K. and Sherman, Michael A. and Ku, Joy P. and Delp, Scott L.},
  journal = {PLOS Computational Biology},
  volume  = {14},
  number  = {7},
  pages   = {e1006223},
  year    = {2018},
  doi     = {10.1371/journal.pcbi.1006223}
}
```

### OpenSim Moco

This repository solves squat optimal-control problems using OpenSim Moco.

Recommended Moco citation:

```bibtex
@article{Dembia2020Moco,
  title   = {OpenSim Moco: Musculoskeletal optimal control},
  author  = {Dembia, Christopher L. and Bianco, Nicholas A. and Falisse, Antoine and Hicks, Jennifer L. and Delp, Scott L.},
  journal = {PLOS Computational Biology},
  volume  = {16},
  number  = {12},
  pages   = {e1008493},
  year    = {2020},
  doi     = {10.1371/journal.pcbi.1008493}
}
```

### PRIMA

This repository uses PRIMA for derivative-free upper-level optimization.

- PRIMA repository: [libprima/prima](https://github.com/libprima/prima)

Recommended PRIMA citation:

```bibtex
@misc{Zhang2023PRIMA,
  title        = {{PRIMA: Reference Implementation for Powell's Methods with Modernization and Amelioration}},
  author       = {Zhang, Z.},
  howpublished = {Available at http://www.libprima.net, DOI: 10.5281/zenodo.8052654},
  year         = {2023}
}
```

### CAMS-Knee dataset

This workflow uses experimental squat trajectories derived from the CAMS-Knee dataset. The raw dataset should be obtained from the CAMS-Knee/Orthoload website and processed through OpenSim inverse kinematics before running the IOC scripts.

- CAMS-Knee dataset: [CAMS-Knee.orthoload.com](https://cams-knee.orthoload.com/)
- CAMS-Knee publication DOI: [10.1016/j.jbiomech.2017.09.022](https://doi.org/10.1016/j.jbiomech.2017.09.022)

Recommended CAMS-Knee citations:

```bibtex
@misc{CAMS1,
  author = {Damm, P. and Taylor, W. R. and others},
  title = {{CAMS-Knee.orthoload.com}},
  year = {2019},
  note = {Julius Wolff Institute – Charité Universitätsmedizin Berlin and Institute for Biomechanics – ETH Zürich}
}

@article{CAMS2,
  title = {A comprehensive assessment of the musculoskeletal system: The CAMS-Knee data set},
  volume = {65},
  ISSN = {0021-9290},
  DOI = {10.1016/j.jbiomech.2017.09.022},
  journal = {Journal of Biomechanics},
  publisher = {Elsevier BV},
  author = {Taylor, William R. and Sch\"{u}tz, Pascal and Bergmann, Georg and List, Renate and Postolka, Barbara and Hitz, Marco and Dymke, J\"{o}rn and Damm, Philipp and Duda, Georg and Gerber, Hans and Schwachmeyer, Verena and Hosseini Nasab, Seyyed Hamed and Trepczynski, Adam and Kutzner, Ines},
  year = {2017},
  month = dec,
  pages = {32--39}
}
```

Also cite any study-specific data, OpenSim models, custom Moco goal plugins, or experimental datasets used in your analysis.
